#pragma warning(push)
#pragma warning(disable: 4505)


internal f32 PixelsToDIP(float pixels) {
    // TODO(Cian): Fix rendering so that rounding isn't necessary
    return F32_ROUND(pixels / (global_os->display.dpi / UI_DEFAULT_DENSITY));
}

internal f32 DIPToPixels(float dp) {
    return F32_ROUND (dp  * (global_os->display.dpi / UI_DEFAULT_DENSITY));
}


internal void UI_WidgetAddProperty(UI_Widget *widget, UI_WidgetProperty property) {
    u32 idx = property / 64;
    widget->properties[idx] |= ((u64)1 << (property % 64));
}

internal b32 UI_WidgetHasProperty(UI_Widget *widget, UI_WidgetProperty property) {
    u32 idx = property / 64;
    // NOTE(Cian): Can think of !! as a clamp between {0,1}, it's just 2 NOTS together, not really required here but kinda cool
    return !!((u64)widget->properties[idx] & ((u64)1 <<  (property % 64)));
}

internal void UI_WidgetRemoveProperty(UI_Widget *widget, UI_WidgetProperty property) {
    u32 idx = property / 64;
    // NOTE(Cian): ~ is the complement of a series of bits, e.g. flips the bits
    widget->properties[idx] &= (~((u64)1 << (property % 64)));
}

internal UI_Widget* UI_InitWidget(char *name) {
    // NOTE(Cian): All temporary and very liable to change lol
    
    u32 idx = StringToCRC32(name) % UI_MAX_WIDGETS;
    
    UI_Widget *at_location = ui_state->widgets[idx];
    
    UI_Widget *under_construction = null;
    
    // NOTE(Cian): HashTable stuff
    if(at_location == null) {
        under_construction = (UI_Widget*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(UI_Widget));
        ui_state->widgets[idx] = under_construction;
    } else {
        UI_Widget *prev = null;
        while(at_location) {
            // TODO(Cian): @string_function_replace when we have our own string functions replace this with our comparision equivalent
            // NOTE(Cian): This means we have found our widget that was previously created, how do we know if instead this was us accidentally creating a 2nd widget with the same name??
            if(strcmp(name, at_location->string) == 0) {
                under_construction = at_location;
                // NOTE(Cian): Widget has already been created in previous frame, save layout from last fram and reset current layout
                under_construction->old_layout = under_construction->curr_layout;
                under_construction->curr_layout = {0};
                break;
            }
            prev = at_location;
            at_location = at_location->hash_next;
        }
        // NOTE(Cian): Widget hasn't been created yet
        if(under_construction == null) {
            under_construction = (UI_Widget*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(UI_Widget));
            prev->hash_next = under_construction;
        }
    }
    // NOTE(Cian): Widget data stuff
    under_construction->string = name;
    under_construction->parameters[0] = ui_state->width_stack.current;
    under_construction->parameters[1] = ui_state->height_stack.current;
    // TODO(Cian): add more thingys here when we start actually doing them
    
    // NOTE(Cian): Widget Tree stuff
    if(ui_state->parent_stack.current) {
        under_construction->tree_parent = ui_state->parent_stack.current;
        
        if(ui_state->prev_widget == under_construction->tree_parent) {
            ui_state->prev_widget->tree_first_child = under_construction;
        } else {
            ui_state->prev_widget->tree_next_sibling = under_construction;
            under_construction->tree_prev_sibling = ui_state->prev_widget;
        }
        
        under_construction->tree_parent->tree_last_child = under_construction;
    }
    
    ui_state->prev_widget = under_construction;
    return under_construction;
}

internal void UI_Begin() {
    // NOTE(Cian): Create the main window and add as parent
    // TODO(Cian): Make a Widget init function once we have more knowledge about different use cases
    f32 display_width = (f32)global_os->display.width;
    f32 display_height = (f32)global_os->display.height;
    
    UI_PushWidth(display_width, 1.0f);
    UI_PushHeight(display_height, 1.0f);
    UI_Widget *main_row_container = UI_InitWidget("main");
    UI_PopWidth();
    UI_PopHeight();
    
    // TODO(Cian): Push size auto for both dimensions, e.g. by default widgets placed directly in the main_container are size auto
    
    ui_state->root_widget = main_row_container;
    UI_PushParent(main_row_container);
    UI_WidgetAddProperty(main_row_container, UI_WidgetProperty_Container);
    UI_WidgetAddProperty(main_row_container, UI_WidgetProperty_LayoutHorizontal);
    
}

internal inline f32 UI_GetSize(UI_Widget *widget, u32 axis) {
    f32 result = 0;
    
    if(widget->parameters[axis].is_ratio) {
        f32 parent_size = widget->tree_parent->curr_layout.elements[axis + 2];
        result = widget->parameters[axis].ratio * parent_size;
    }else {
        result = widget->parameters[axis].size; 
    }
    
    return result;
}

// TODO(Cian): Both of these layout functions loop through the children, maybe these loops could occur at the same time? Doubt it really matters for performance whatsoever but it's kinda bothering my OCD
// NOTE(Cian): Lays out the widgets in the direction of the Container e.g. if a row it will lay out/re-size horizontally
internal void UI_LayoutInFlow(UI_Widget *first_child, f32 available, u32 axis) {
    f32 sum_delta = 0;
    f32 sum = 0;
    
    f32 offset = 0;
    UI_Widget *curr = first_child;
    while(curr) {
        f32 pref_size = curr->parameters[axis].size;
        
        sum_delta +=  (pref_size * (1 - curr->parameters[axis].strictness));
        sum += curr->parameters[axis].size;
        
        // NOTE(Cian): Attempt to layout with the preferred size
        curr->curr_layout.elements[axis] = offset;
        curr->curr_layout.elements[axis + 2] = pref_size;
        offset += curr->curr_layout.elements[axis + 2];
        
        curr = curr->tree_next_sibling;
    }
    
    offset = 0;
    if(sum > available && sum_delta != 0) {
        
        curr = first_child;
        while(curr) {
            
            f32 factor = (curr->parameters[axis].size * (1 - curr->parameters[axis].strictness)) / sum_delta; 
            
            curr->curr_layout.width -= (factor * (sum - available)); 
            curr->curr_layout.elements[axis] = offset;
            offset += curr->curr_layout.elements[axis + 2];
            
            curr = curr->tree_next_sibling;
        }
    } 
}

// NOTE(Cian): Lays out the widgets in the non-flow axis e.g. if a row it will do layout/re-sizing on the height, simply "centers" the widget on this axis
internal void UI_LayoutNonFlow() {
    
}
// NOTE(Cian): 
internal void UI_TraverseTree(UI_Widget *root) {
    // TODO(Cian): Take padding into account here;
    f32 parent_width = root->curr_layout.width;
    f32 parent_height = root->curr_layout.height;
    
    if(UI_WidgetHasProperty(root, UI_WidgetProperty_Container)) {
        if(UI_WidgetHasProperty(root, UI_WidgetProperty_LayoutHorizontal)) {
            UI_LayoutInFlow(root->tree_first_child, parent_width, 0);
        } else if (UI_WidgetHasProperty(root, UI_WidgetProperty_LayoutVertical)) {
            // TODO(Cian): Columns
            UI_LayoutInFlow(root->tree_first_child, parent_height, 1);
        } else {
            UI_LayoutInFlow(root->tree_first_child, parent_width, 0);
        }
        
        // TODO(Cian): Lil annoying that I have a third loop here, need to try and clean this up once I get things working
        UI_Widget *curr = root->tree_first_child;
        while(curr) {
            if(UI_WidgetHasProperty(curr, UI_WidgetProperty_Container)) {
                UI_TraverseTree(curr);
            }
            curr = curr->tree_next_sibling;
        }
    }
}

internal void UI_DoLayout() {
    UI_Widget *root = ui_state->root_widget;
    // NOTE(Cian): Roots incoming constraints will always be tight so just set it's layout to be that
    root->curr_layout = {0, 0, root->parameters[0].size, root->parameters[1].size};
    
    UI_TraverseTree(root);
}

internal void UI_Render(UI_Widget *root) {
    UI_Widget *curr = root->tree_first_child;
    
    while(curr) {
        if(UI_WidgetHasProperty(curr, UI_WidgetProperty_RenderBackground)){
            f32 height = 60;
            if(UI_WidgetHasProperty(curr, UI_WidgetProperty_Container))
                height = 120;
            nvgBeginPath(global_vg);
            nvgRect(global_vg, curr->curr_layout.x,  curr->curr_layout.y, curr->curr_layout.width, height);
            nvgFillColor(global_vg, curr->color);
            nvgFill(global_vg);
        }
        
        if(UI_WidgetHasProperty(curr, UI_WidgetProperty_Container)) {
            UI_Render(curr);
        }
        curr = curr->tree_next_sibling;
    }
}

internal void UI_End() {
    // TODO(Cian): Need to either clear autolayout state here or, make the UI_State as part of the frame_arena and UI_Widgets be maintained seperately in permanent arena
    ui_state->parent_stack.size = 0;
    ui_state->width_stack.size = 0;
    ui_state->height_stack.size = 0;
    
    ui_state->prev_widget = null;
    ui_state->parent_stack.current = null;
    
    //Autolayout for rendering and next frame goes here
    /* NOTE(Cian): Auto-layout works as follows:
    *  Traverse downwards through the tree, for every parent, measure it's children, this may
*  need to be recursive, we add childrens 
*/
    UI_DoLayout();
    UI_Render(ui_state->root_widget);
}

internal void UI_BeginRow(char *string) {
    UI_Widget *row = UI_InitWidget(string);
    UI_PushParent(row);
    UI_WidgetAddProperty(row, UI_WidgetProperty_Container);
    UI_WidgetAddProperty(row, UI_WidgetProperty_LayoutHorizontal);
}

internal void UI_EndRow() {
    UI_PopParent();
}

internal void UI_BeginPanel(char *string, NVGcolor color) {
    UI_Widget *panel_container = UI_InitWidget(string);
    UI_WidgetAddProperty(panel_container, UI_WidgetProperty_Container);
    UI_WidgetAddProperty(panel_container, UI_WidgetProperty_RenderBackground);
    panel_container->color = color;
    UI_PushParent(panel_container);
    // TODO(Cian): Need to come up with a proper string ID generator for cases where user can't specify
    UI_WidthFill
        UI_BeginRow("panel_row");
}

internal void UI_EndPanel() {
    UI_EndRow();
    UI_PopParent();
}

internal void UI_TestBox(char *string, NVGcolor color) {
    UI_Widget *test_box = UI_InitWidget(string);
    test_box->color = color;
    UI_WidgetAddProperty(test_box, UI_WidgetProperty_RenderBackground);
}

internal void UI_PushParent(UI_Widget *parent) {
    u32 *size = &ui_state->parent_stack.size;
    ui_state->parent_stack.stack[*size] = ui_state->parent_stack.current;
    *size++;
    ui_state->parent_stack.current = parent;
}

internal void UI_PopParent() {
    u32 *size = &ui_state->parent_stack.size;
    *size--;
    ui_state->parent_stack.current = ui_state->parent_stack.stack[*size];
}

internal void UI_PushWidthRatio(f32 ratio, f32 strictness) {
    u32 *size = &ui_state->width_stack.size;
    ui_state->width_stack.stack[*size] = ui_state->width_stack.current;
    (*size)++;
    
    ui_state->width_stack.current.is_ratio = true;
    ui_state->width_stack.current.ratio = ratio;
    ui_state->width_stack.current.strictness = strictness;
}

internal void UI_PushWidth(f32 size, f32 strictness) {
    u32 *s_size = &ui_state->width_stack.size;
    ui_state->width_stack.stack[*s_size] = ui_state->width_stack.current;
    (*s_size)++;
    
    ui_state->width_stack.current.is_ratio = false;
    ui_state->width_stack.current.size = size;
    ui_state->width_stack.current.strictness = strictness;
}

internal void UI_PopWidth() {
    u32 *size = &ui_state->width_stack.size;
    (*size)--;
    ui_state->width_stack.current = ui_state->width_stack.stack[*size];
}

internal void UI_PushHeightRatio(f32 ratio, f32 strictness) {
    u32 *size = &ui_state->height_stack.size;
    ui_state->height_stack.stack[*size] = ui_state->height_stack.current;
    (*size)++;
    
    ui_state->height_stack.current.is_ratio = true;
    ui_state->height_stack.current.ratio = ratio;
    ui_state->height_stack.current.strictness = strictness;
}

internal void UI_PushHeight(f32 size, f32 strictness) {
    u32 *s_size = &ui_state->height_stack.size;
    ui_state->height_stack.stack[*s_size] = ui_state->height_stack.current;
    (*s_size)++;
    ui_state->height_stack.current.is_ratio = false;
    ui_state->height_stack.current.size = size;
    ui_state->height_stack.current.strictness = strictness;
}

internal void UI_PopHeight() {
    u32 *size = &ui_state->height_stack.size;
    (*size)--;
    ui_state->height_stack.current = ui_state->height_stack.stack[*size];
}

internal void UI_Enqueue(UI_RecursiveQueue *queue, UI_Widget *widget) {
    assert(queue->size != UI_MAX_WIDGETS );
    
    queue->rear = (++queue->rear) % UI_MAX_WIDGETS;
    queue->widget_queue[queue->rear] = widget;
    queue->size++;
}

internal UI_Widget*  UI_Dequeue(UI_RecursiveQueue *queue) {
    assert(queue->size != 0);
    
    UI_Widget *result = queue->widget_queue[queue->front];
    queue->front = (++queue->front) % UI_MAX_WIDGETS;
    queue->size--;
    return result;
}