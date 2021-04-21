#pragma warning(push)
#pragma warning(disable: 4505)

// TODO(Cian): @UI @Important Having some perf issues since latest UI changes, not sure why, after demo implement some basic profiling and will probably have to rewrite the UI system with those insights in mind

internal f32 ui_pixels_to_dp(float pixels) {
    return F32_ROUND(pixels / (os->display.dpi / UI_OS_DEFAULT_DENSITY));
}

internal f32 ui_dp_to_pixels(float dp) {
    return F32_ROUND (dp  * (os->display.dpi / UI_OS_DEFAULT_DENSITY));
}

internal UI_ID ui_init_id(u32 hash) {
    UI_ID res = {hash, UI_ID_Property_Regular};
    return res;
}

internal UI_ID ui_non_interactable_id() {
    UI_ID res = {0, UI_ID_Property_NonInteractable};
    return res;
}

internal UI_ID ui_null_id() {
    UI_ID res = {0, UI_ID_Property_NonInteractable};
    return res;
}

internal b32 ui_is_id_equal(UI_ID id_1, UI_ID id_2) {
    b32 res = false;
    
    if(id_1.hash == id_2.hash && id_1.property == id_2.property)
        res = true;
    
    return res;
}

// NOTE(Cian): Brings widgets to the front relative to their parent
internal void ui_bring_to_front(UI_Widget *widget) {
    if( widget != ui->sorted_containers_start){
        if(ui->curr_frame > 0) {
            UI_Widget *curr = widget;
            while(curr) {
                UI_Widget *parent = widget->window_parent;
                UI_Widget *swap_widget = null;
                UI_Widget *last_descendant = null;
                
                UI_Widget *old_neighbour_left = null;
                UI_Widget *old_neighbour_right = curr->next_sorted_container;
                
                UI_Widget *swap_neighbour = null;
                
                if(parent->front_child) {
                    swap_widget = parent->front_child;
                    while(swap_widget->front_child) {
                        swap_widget = swap_widget->front_child;
                    }
                } else {
                    swap_widget = parent;
                }
                
                //widget to the left of the swap widget
                swap_neighbour = swap_widget->prev_sorted_container;
                
                //the leftmost child in this widget chain
                last_descendant = widget;
                while(last_descendant->front_child) {
                    last_descendant = last_descendant->front_child;
                }
                
                old_neighbour_left = last_descendant->prev_sorted_container;
                
                curr->next_sorted_container = swap_widget;
                swap_widget->prev_sorted_container = curr;
                
                old_neighbour_left->next_sorted_container = old_neighbour_right;
                old_neighbour_right->prev_sorted_container = old_neighbour_left;
                
                if(!swap_neighbour && swap_widget == ui->sorted_containers_start) {
                    ui->sorted_containers_start = last_descendant;
                    last_descendant->prev_sorted_container = null;
                } else {
                    last_descendant->prev_sorted_container = swap_neighbour;
                    swap_neighbour->next_sorted_container = last_descendant;
                }
                
                curr->window_parent->front_child = curr;
                
                //bring our parents to the front relative to their parent e.g. clicking a window "bubbles up"
                if(widget->window_parent != ui->sorted_containers_end)
                    curr = widget->window_parent;
                else
                    curr = null;
            }
            
        } else {
            
            if(!ui->sorted_containers_start) {
                ui->sorted_containers_start = widget;
                ui->sorted_containers_end = widget;
            } else {
                widget->next_sorted_container = ui->sorted_containers_start;
                ui->sorted_containers_start->prev_sorted_container = widget;
                ui->sorted_containers_start = widget;
                widget->window_parent->front_child = widget;
            }
        }
    }
}

internal void ui_widget_add_property(UI_Widget *widget, UI_Widget_Property property) {
    u32 idx = property / 64;
    widget->properties[idx] |= ((u64)1 << (property % 64));
}

internal b32 ui_widget_has_property(UI_Widget *widget, UI_Widget_Property property) {
    u32 idx = property / 64;
    // NOTE(Cian): Can think of !! as a clamp between {0,1}, it's just 2 NOTS together, not really required here but kinda cool
    return !!((u64)widget->properties[idx] & ((u64)1 <<  (property % 64)));
}

internal void ui_widget_remove_property(UI_Widget *widget, UI_Widget_Property property) {
    u32 idx = property / 64;
    // NOTE(Cian): ~ is the complement of a series of bits, e.g. flips the bits
    widget->properties[idx] &= (~((u64)1 << (property % 64)));
}

internal b32 ui_widgets_compare_property(UI_Widget *widget_1, UI_Widget *widget_2, UI_Widget_Property property) {
    b32 result = false;
    
    if(ui_widget_has_property(widget_1, property) && ui_widget_has_property(widget_2, property)) {
        result = true;
    }
    
    return result;
}

internal b32 ui_widgets_share_one_property(UI_Widget *widget_1, UI_Widget *widget_2, UI_Widget_Property property_1, UI_Widget_Property property_2) {
    return ui_widgets_compare_property(widget_1, widget_2, property_1) || ui_widgets_compare_property(widget_1, widget_2, property_2);
}

internal void ui_push_parent(UI_Widget *parent) {
    u32 size = ui->parent_stack.size;
    if(size > 0)
        ui->parent_stack.stack[size - 1] = ui->parent_stack.current;
    ui->parent_stack.current = parent;
    ui->parent_stack.size++;
}

internal void ui_pop_parent() {
    //when a parent ends, it needs to be the previous widget, not its last child, so its siblings can correctly reference it
    ui->prev_widget = ui->parent_stack.current;
    ui->parent_stack.size--;
    u32 size = ui->parent_stack.size;
    ui->parent_stack.current = ui->parent_stack.stack[size - 1];
}

internal void ui_push_window(UI_Widget *window) {
    u32 size = ui->window_stack.size;
    if(size > 0)
        ui->window_stack.stack[size - 1] = ui->window_stack.current;
    ui->window_stack.current = window;
    ui->window_stack.size++;
}

internal void ui_pop_window() {
    //when a window ends, it needs to be the previous widget, not its last child, so its siblings can correctly reference it
    ui->prev_widget = ui->window_stack.current;
    ui->window_stack.size--;
    u32 size = ui->window_stack.size;
    ui->window_stack.current = ui->window_stack.stack[size - 1];
}

internal void ui_push_width(f32 size, f32 strictness) {
    u32 *s_size = &ui->width_stack.size;
    ui->width_stack.stack[*s_size] = ui->width_stack.current;
    (*s_size)++;
    ui->width_stack.current.size = size;
    ui->width_stack.current.strictness = strictness;
}

internal void ui_pop_width() {
    u32 *size = &ui->width_stack.size;
    (*size)--;
    ui->width_stack.current = ui->width_stack.stack[*size];
}

internal void ui_push_padding(V4 padding) {
    u32 *s_size = &ui->padding_stack.size;
    ui->padding_stack.stack[*s_size] = ui->padding_stack.current;
    (*s_size)++;
    
    ui->padding_stack.current = padding;
}

internal void ui_pop_padding() {
    u32 *size = &ui->padding_stack.size;
    (*size)--;
    ui->padding_stack.current = ui->padding_stack.stack[*size];
}

internal void ui_push_height(f32 size, f32 strictness) {
    u32 *s_size = &ui->height_stack.size;
    ui->height_stack.stack[*s_size] = ui->height_stack.current;
    (*s_size)++;
    ui->height_stack.current.size = size;
    ui->height_stack.current.strictness = strictness;
}

internal void ui_pop_height() {
    u32 *size = &ui->height_stack.size;
    (*size)--;
    ui->height_stack.current = ui->height_stack.stack[*size];
}

internal UI_Widget* ui_init_widget(String8 string, b32 *newly_created) {
    b32 stub_bool = false;
    b32 *p_stub_bool = &stub_bool;
    b32 **p_newly_created = &p_stub_bool;
    
    if(newly_created)
        p_newly_created = &newly_created;
    
    UI_Widget *widget = null;
    
    if(string.data) {
        // NOTE(Cian): Process our string to append or replace the hash_string
        String8 hash_string = {};
        
        u32 pound_count = 0;
        u32 first_pound_loc = 0;
        for(u32 i = 0; i < string.size; i++) {
            char curr = string.data[i];
            
            if(curr == '#') {
                if(pound_count == 0) {
                    first_pound_loc = i;
                }
                pound_count++;
            } else {
                if(pound_count < 2)
                    pound_count = 0;
            }
        }
        
        if(pound_count == 2) {
            hash_string.size = string.size - 2;
            hash_string.data = (char*)memory_arena_push(&os->frame_arena, hash_string.size);
            // TODO(Cian): @UI should probably check if this memory is ok
            
            u32 size_1 = first_pound_loc + 1;
            u32 src_offset_2 = first_pound_loc + 2;
            u32 size_2 = string.size - src_offset_2;
            
            //Copy string preceding ##
            stbsp_snprintf(hash_string.data, size_1, string.data);
            //Copy string following ##
            stbsp_snprintf(hash_string.data + (size_1 - 1), size_2, string.data + src_offset_2);
            
            //Remove appended piece from main string;
            string.data[first_pound_loc] = null;
            string.size = first_pound_loc + 1;
        } else if(pound_count == 3) {
            // NOTE(Cian): We want to use just the string after the two pound signs for hashing
            u32 src_offset = first_pound_loc + 3;
            
            hash_string.size = string.size - src_offset;
            hash_string.data = (char*)memory_arena_push(&os->frame_arena, hash_string.size);
            
            stbsp_snprintf(hash_string.data, hash_string.size, string.data + src_offset);
            
            //Remove appended piece from main string;
            string.data[first_pound_loc] = null;
            string.size = first_pound_loc + 1;
        } else {
            hash_string = string;
        }
        UI_Widget *window = null;
        if(ui->window_stack.current)
            window = ui->parent_stack.current;
        
        u32 seed = 0;
        
        //seed every hash by the window parent of the widget
        if(window)
            seed = window->id.hash;
        
        u32 hash = string_to_crc32(hash_string.data, seed);
        u32 idx = hash % UI_WIDGET_TABLE_SIZE;
        
        widget = ui->widgets[idx];
        if(widget == null) {
            if(ui->widget_free_list_count > 0) {
                widget = ui->widget_free_list;
                ui->widget_free_list = widget->hash_next; // we reuse the hash_next pointer as the free-list linked list pointer
                widget->hash_next = null;
                ui->widget_size++;
            } else {
                widget = (UI_Widget*)memory_arena_push(&os->permanent_arena, sizeof(UI_Widget));
                ui->widgets[idx] = widget;
                ui->widget_size++;
            }
            **p_newly_created = true; 
        } else {
            UI_Widget *previous = null;
            while(widget) {
                if(hash == widget->id.hash)
                    break;
                previous = widget;
                widget = widget->hash_next;
            }
            
            if(widget == null) {
                if(ui->widget_free_list_count > 0) {
                    widget = ui->widget_free_list;
                    ui->widget_free_list = widget->hash_next; // we reuse the hash_next pointer as the free-list linked list pointer
                    widget->hash_next = null;
                    ui->widget_size++;
                } else {
                    widget = (UI_Widget*)memory_arena_push(&os->permanent_arena, sizeof(UI_Widget));
                    ui->widget_size++;
                }
            }
            if(previous) 
                previous->hash_next = widget;
            
            **p_newly_created = false;
        }
        widget->id = ui_init_id(hash);
    } else {
        u32 idx = 0;
        
        u64 last_frame = (ui->curr_frame == 0) ? 0 : ui->curr_frame;
        
        for(idx; idx < UI_WIDGET_TABLE_SIZE; idx++) {
            widget = ui->widgets[idx];
            
            if(widget == null || widget) {
                break;
            }
        }
        
        if(widget == null || (widget->id.property == UI_ID_Property_NonInteractable && widget->last_frame < last_frame)) {
            widget = (UI_Widget*)memory_arena_push(&os->permanent_arena, sizeof(UI_Widget));
            ui->widgets[idx] = widget;
        }
        UI_Widget *hash_next = widget->hash_next;
        *widget = {};
        widget->hash_next = hash_next;
        
        widget->id = ui_non_interactable_id();
        
        string = string_from_cstring("non_interactable");
    }
    widget->string = string;
    
    widget->old_layout = widget->curr_layout;
    //widget->curr_layout = {0};
    widget->last_frame = ui->curr_frame;
    
    widget->parameters[0] = ui->width_stack.current;
    widget->parameters[1] = ui->height_stack.current;
    
    if(ui->window_stack.current)
        widget->window_parent = ui->parent_stack.current;
    
    widget->style = &widget_style_table[UI_Widget_Style_Default];
    
    if(ui->parent_stack.current) {
        widget->tree_parent = ui->parent_stack.current;
        if(ui->prev_widget == widget->tree_parent) {
            ui->prev_widget->tree_first_child = widget;
        } else if(ui->prev_widget->tree_parent == widget->tree_parent){
            ui->prev_widget->tree_next_sibling = widget;
            widget->tree_prev_sibling = ui->prev_widget;
        }
        
        widget->tree_parent->tree_last_child = widget;
    }
    
    ui->prev_widget = widget;
    
    return widget;
}

internal void ui_begin() {
    // NOTE(Cian): Create the main window and add as parent
    // TODO(Cian):  @UI Make a UI_Widget init function once we have more knowledge about different use cases
    f32 display_width = (f32)os->display.width;
    f32 display_height = (f32)os->display.height;
    
    ui_push_width(display_width, 1.0f);
    ui_push_height(display_height, 1.0f);
    // NOTE(Cian): Some non-interactable widgets will still be explicitly named for debugging purposes
    // TODO(Cian): @UI might reqork this so it only does this in debug mode?
    String8 main_row_string = string_from_cstring("main");
    UI_Widget *main_window = ui_init_widget(main_row_string, null);
    
    if(ui->curr_frame == 0)
        ui_bring_to_front(main_window);
    
    main_window->curr_layout = v4(0, 0, display_width, display_height);
    ui_pop_width();
    ui_pop_height();
    
    ui->root_widget = main_window;
    ui->active_window = main_window;
    ui_push_parent(main_window);
    ui_push_window(main_window);
    ui_widget_add_property(main_window, UI_Widget_Property_MainWindow);
    ui_widget_add_property(main_window, UI_Widget_Property_LayoutHorizontal);
    
    if(ui->curr_frame == 0){
        ui->active = ui_null_id();
        ui->hot = ui_null_id();
    } else {
        //find the clickable_window
        UI_Widget *container = ui->sorted_containers_start;
        while(container) {
            V4 container_rect = container->curr_layout;
            b32 mouse_is_over = math_point_in_rect(container_rect, os->mouse_pos);
            
            if(mouse_is_over) {
                ui->clickable_window = container;
                
                break;
            }
            
            ui->clickable_window = null;
            container = container->next_sorted_container;
        }
    }
}

internal void ui_render(UI_Widget *root) {
    UI_Widget **render_stack = (UI_Widget **)memory_arena_push(&os->frame_arena, sizeof(UI_Widget*) * ui->widget_size);
    UI_Widget *render_stack_current = null;
    u32 size = 0;
    
#define render_stack_push(widget) {\
if(size > 0 && size < ui->widget_size)\
render_stack[size - 1] = render_stack_current;\
render_stack_current = widget;\
size++;\
}
    
#define render_stack_pop() {\
if(size >= 1)\
size--;\
if(size > 0) {\
render_stack_current = render_stack[size - 1];\
} else {\
render_stack_current = null;\
}\
}
    UI_Widget *window = ui->sorted_containers_end;
    while(window) {
        //reset stack
        size = 0;
        render_stack_current = null;
        
        //push window onto stack
        render_stack_push(window);
        //do clipping/nvgScissor stuff here
        nvgScissor(vg_context, window->curr_layout.x, window->curr_layout.y, window->curr_layout.width, window->curr_layout.height);
        while(render_stack_current) {
            //pop element from stack
            UI_Widget *curr = render_stack_current;
            
            // TODO(Cian): Convert to relative coordinates
            f32 x = curr->curr_layout.x;
            f32 y = curr->curr_layout.y;
            
            f32 width = curr->curr_layout.width;
            f32 height = curr->curr_layout.height;
            
            f32 border_thickness = curr->style->border_thickness;
            
            render_stack_pop();
            //render it
            if(ui_widget_has_property(curr, UI_Widget_Property_RenderBackground)) {
                // TODO(Cian): handle cases of rounded backgrounds
                // TODO(Cian): handle rendering of hot/active effects e.g. check if widget wants them rendered and check if hot/active
                NVGcolor color = curr->style->colors[UI_ColorState_Normal].background_color;
                
                nvgBeginPath(vg_context);
                nvgRect(vg_context, x, y, width, height);
                nvgFillColor(vg_context, color);
                nvgFill(vg_context);
            }
            
            if(ui_widget_has_property(curr, UI_Widget_Property_RenderTitleBar)) {
                // TODO(Cian): handle cases of rounded backgrounds
                NVGcolor color = curr->style->colors[UI_ColorState_Normal].border_color;
                
                f32 title_height = curr->style->title_height;
                
                nvgBeginPath(vg_context);
                nvgRect(vg_context, x, y, width, title_height);
                nvgFillColor(vg_context, color);
                nvgFill(vg_context);
                
                // TODO(Cian): This text can get in the way of the close button, use nvgTextGlyphPositions to determine where text should be cutoff to ensure there is enough space, or I could use nvgTextBreakLines with a maxRow value of 1?
                nvgTextAlign(vg_context, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
                nvgFontFace(vg_context, "roboto-medium");
                nvgFontSize(vg_context, curr->style->font_size);
                nvgFillColor(vg_context, curr->style->colors[UI_ColorState_Normal].text_color);
                nvgText(vg_context, x + (width / 2), y + (title_height / 2), curr->string.data, null);
            }
            
            if(ui_widget_has_property(curr, UI_Widget_Property_RenderBorder) && !(ui_widget_has_property(curr, UI_Widget_Property_RenderBorderHot) && ui_is_id_equal(ui->hot, curr->id))) {
                // TODO(Cian): implement this
            } else {
                f32 title_height = curr->style->title_height;
                V4 border_left = v4(x, y + title_height, border_thickness,  height - title_height);
                V4 border_right = v4(x + ( width - border_thickness), y + title_height, border_thickness,  height - title_height);
                V4 border_bottom = v4(x, y + ( height - border_thickness), width, border_thickness);
                
                //render hot border effects
                if(ui_widget_has_property(curr, UI_Widget_Property_ResizeLeft)) {
                    nvgBeginPath(vg_context);
                    nvgRect(vg_context, border_left.x, border_left.y, border_left.width, border_left.height);
                    nvgFillColor(vg_context, nvgRGB(255, 255 ,255));
                    nvgFill(vg_context);
                } else if(ui_widget_has_property(curr, UI_Widget_Property_ResizeRight)) {
                    nvgBeginPath(vg_context);
                    nvgRect(vg_context, border_right.x, border_right.y, border_right.width, border_right.height);
                    nvgFillColor(vg_context, nvgRGB(255, 255 ,255));
                    nvgFill(vg_context);
                } else if(ui_widget_has_property(curr, UI_Widget_Property_ResizeBottom)) {
                    nvgBeginPath(vg_context);
                    nvgRect(vg_context, border_bottom.x, border_bottom.y, border_bottom.width, border_bottom.height);
                    nvgFillColor(vg_context, nvgRGB(255, 255 ,255));
                    nvgFill(vg_context);
                } else {
                    // TODO(Cian): draw all borders as hot
                }
            }
            
            if(ui_widget_has_property(curr, UI_Widget_Property_RenderSplit)) {
                NVGcolor color = curr->style->colors[UI_ColorState_Normal].border_color;
                
                if(ui_is_id_equal(ui->hot, curr->id))
                    color = curr->style->colors[UI_ColorState_Hot].border_color;
                
                nvgBeginPath(vg_context);
                nvgRect(vg_context, curr->splitter_rect.x, curr->splitter_rect.y, curr->splitter_rect.width, curr->splitter_rect.height);
                nvgFillColor(vg_context, color);
                nvgFill(vg_context);
            }
            //add children(excluding windows/containers) to stack left to right if parent
            UI_Widget *curr_child = curr->tree_first_child;
            while(curr_child) {
                if(!ui_widget_has_property(curr_child, UI_Widget_Property_Container)) {
                    render_stack_push(curr_child);
                }
                
                curr_child = curr_child->tree_next_sibling;
            }
        }
        
        window = window->prev_sorted_container;
    }
}

internal void ui_end() {
    // TODO(Cian): @UI Need to either clear autolayout state here or, make the State as part of the frame_arena and UI_Widgets be maintained seperately in permanent arena
    ui->parent_stack.size = 0;
    ui->width_stack.size = 0;
    ui->height_stack.size = 0;
    
    ui->prev_widget = null;
    ui->parent_stack.current = null;
    
    // TODO(Cian): maybe doing this cleanup via the sorted linked list might be better idk we'll see
    //Clean up widgets that weren't persisted this frame
    for (u32 i = 0; i < UI_WIDGET_TABLE_SIZE; i += 1) {
        
        UI_Widget *curr = ui->widgets[i];
        UI_Widget *prev = null;
        
        while(curr) {
            if(curr->last_frame != ui->curr_frame) {
                //if widget is a window, go through its children and add to free list and remove from window sorting
                if(ui_widget_has_property(curr, UI_Widget_Property_Container)) {
                    if(curr == ui->sorted_containers_start) {
                        //don't need to check if there is a next sorted container as there will always be the main window at the bottom
                        curr->next_sorted_container->prev_sorted_container = null;
                        ui->sorted_containers_start = curr->next_sorted_container;
                        
                    } else {
                        curr->prev_sorted_container->next_sorted_container = curr->next_sorted_container;
                        curr->next_sorted_container->prev_sorted_container = curr->prev_sorted_container;
                    }
                }
                
                if(prev) {
                    if(curr->hash_next) {
                        prev->hash_next = curr->hash_next;
                    } else {
                        prev->hash_next = null;
                    }
                } else {
                    if(curr->hash_next) {
                        ui->widgets[i] = curr->hash_next;
                    } else {
                        ui->widgets[i] = null;
                    }
                }
                
                if(ui_is_id_equal(ui->active, curr->id))
                    ui->active = ui_null_id();
                
                if(ui_is_id_equal(ui->hot, curr->id))
                    ui->hot = ui_null_id();
                
                //clear curr
                *curr = {};
                
                curr->hash_next = ui->widget_free_list;
                ui->widget_free_list = curr;
                ui->widget_free_list_count++;
                
            }
            prev = curr;
            curr = curr->hash_next;
        }
    }
    
    
    
    ui->curr_frame++;
    //Autolayout for rendering and next frame goes here
    /* NOTE(Cian): Auto-layout works as follows:
    *  For each window: traverse downwards through the tree, for every parent, measure it's children, this may
*  need to be recursive, we add childrens 
*/
    UI_Widget *root = ui->root_widget;
    // NOTE(Cian): Roots incoming constraints will always be tight so just set it's layout to be that
    
    //ui_do_layout(root);
    
    //rendering: for each window in sorted order recursively render it and its children
    ui_render(ui->root_widget);
    ui->active_window = null;
}
// TODO(Cian): @Checkpoint Finished split pane, discovered a bug with window sorting that causes the one movable widget to dissapear, gotta investigate
internal void ui_begin_split_pane(V4 layout, b32 split_vertical, f32 min_size_1, V2 *size_pos_1, f32 min_size_2, V2 *size_pos_2, u32 options, char *string...) {
    String8 split_string = {};
    MAKE_FORMAT_STRING(split_string, string);
    
    b32 newly_created = false;
    UI_Widget *split_pane = ui_init_widget(split_string, &newly_created);
    ui_widget_add_property(split_pane, UI_Widget_Property_RenderSplit);
    ui_push_parent(split_pane);
    
    split_pane->curr_layout = layout;
    split_pane->old_layout = layout;
    
    V4 *splitter_rect = &split_pane->splitter_rect;
    f32 total_size = size_pos_1->size + size_pos_2->size;
    
    V2 mouse_delta = {};
    b32 mouse_move = os_sum_mouse_moves(&mouse_delta);
    
    f32 mouse_delta_axis = 0;
    
    // TODO(Cian): handle cases where windows are too big a bit more gracefully
    if(split_vertical) {
        mouse_delta_axis = mouse_delta.x;
        
        if(newly_created) {
            f32 splitter_ratio = size_pos_1->size / total_size;
            size_pos_1->size = (split_pane->curr_layout.width * splitter_ratio) - 10.0f;
            splitter_ratio = size_pos_2->size / total_size;
            size_pos_2->size = (split_pane->curr_layout.width * splitter_ratio) - 10.0f;
            
            size_pos_1->pos = split_pane->curr_layout.x;
            size_pos_2->pos = split_pane->curr_layout.x + (size_pos_1->size + 20.0f);
        }
        
        splitter_rect->x = split_pane->curr_layout.x + size_pos_1->size + 8.0f;
        splitter_rect->y = split_pane->curr_layout.y; // might add some padding to this
        splitter_rect->width = 4.0f;
        splitter_rect->height = split_pane->curr_layout.height; //use border style probably here
    } else {
        mouse_delta_axis = mouse_delta.y;
        
        if(newly_created) {
            f32 splitter_ratio = size_pos_1->size / total_size;
            size_pos_1->size = (split_pane->curr_layout.height * splitter_ratio) - 10.0f;
            splitter_ratio = size_pos_2->size / total_size;
            size_pos_2->size = (split_pane->curr_layout.height * splitter_ratio) - 10.0f;
            
            // TODO(Cian): might add padding idk
            size_pos_1->pos = split_pane->curr_layout.y;
            size_pos_2->pos = split_pane->curr_layout.y + (size_pos_1->size + 20.0f);
        }
        
        splitter_rect->y = split_pane->curr_layout.y + size_pos_1->size + 8.0f;
        splitter_rect->x = split_pane->curr_layout.x; // might add some padding to this
        splitter_rect->width = split_pane->curr_layout.width;
        splitter_rect->height = 4.0f; //use border style probably here
    }
    // TODO(Cian): Generic input helpers e.g. has a rect been clicked/dragged etc
    
    
    OS_Event *mouse_down_event = null;
    OS_Event *mouse_up_event = null;
    b32 mouse_down = os_peek_mouse_button_event(&mouse_down_event, OS_Event_Type_MouseDown, OS_Mouse_Button_Left);
    b32 mouse_up = os_peek_mouse_button_event(&mouse_up_event, OS_Event_Type_MouseUp, OS_Mouse_Button_Left);
    
    V2 mouse_pos = os->mouse_pos;
    b32 mouse_over_splitter = math_point_in_rect(*splitter_rect, mouse_pos);
    
    if(ui_is_id_equal(ui->active, split_pane->id)) {
        if(os->mouse_off_screen)
            ui->active = ui_null_id();
        
        if(mouse_up) {
            ui->active = ui_null_id();
            
            if(!mouse_over_splitter || ui->clickable_window != split_pane->window_parent) {
                ui->hot = ui_null_id();
            }
            
            os_take_event(mouse_up_event);
        } else if(mouse_move) {
            if(mouse_delta_axis < min_size_1 - size_pos_1->size)
                mouse_delta_axis = min_size_1 - size_pos_1->size;
            if(mouse_delta_axis > size_pos_2->size - min_size_2)
                mouse_delta_axis = size_pos_2->size - min_size_2;
            size_pos_1->size += mouse_delta_axis;
            size_pos_2->size -= mouse_delta_axis;
            size_pos_2->pos += mouse_delta_axis;
            
        }
    } else if(ui_is_id_equal(ui->hot, split_pane->id)) { 
        if(mouse_down) {
            ui->active = split_pane->id;
            os_take_event(mouse_down_event);
        } else if(!mouse_over_splitter || ui->clickable_window != split_pane->window_parent) {
            ui->hot = ui_null_id();
        }
    } else if (mouse_over_splitter && !mouse_down && ui_is_id_equal(ui->hot, ui_null_id()) && ui->clickable_window == split_pane->window_parent) {
        ui->hot = split_pane->id;
    }
    
}

internal void ui_end_split_pane() {
    ui_pop_parent();
}
// TODO(Cian): @UI @Styles add optional arguments to allow changing the default styles, or maybe optional styles should be pushed into ctx?
internal void ui_begin_window(V4 layout, b32 *is_open, u32 options, char *title...) {
    //if options are no_move && no_resize, we set the size to be the inputed layout every frame
    //if the width or height are auto && the widget is newly created, skip input till the next frame - we won't implement this for awhile since we don't really need it tbh
    
    if(is_open) {
        if(*is_open == false)
            return;
    }
    
    String8 window_string = {};
    MAKE_FORMAT_STRING(window_string, title);
    
    b32 newly_created = false;
    UI_Widget *window = ui_init_widget(window_string, &newly_created);
    ui_push_parent(window);
    ui_push_window(window);
    ui_widget_add_property(window, UI_Widget_Property_Container);
    ui_widget_add_property(window, UI_Widget_Property_RenderBackground);
    ui_widget_add_property(window, UI_Widget_Property_LayoutVertical);
    
    if(~options & UI_ContainerOptions_NoTitle)
        ui_widget_add_property(window, UI_Widget_Property_RenderTitleBar);
    
    if(~options & UI_ContainerOptions_NoMove)
        ui_widget_add_property(window, UI_Widget_Property_Draggable);
    
    if(~options & UI_ContainerOptions_NoResize) {
        ui_widget_add_property(window, UI_Widget_Property_RenderBorderHot);
        ui_widget_add_property(window, UI_Widget_Property_Resizable);
    }
    
    if(~options & UI_ContainerOptions_NoClose) {
        ui_widget_add_property(window, UI_Widget_Property_RenderCloseButton);
    }
    
    if(newly_created || (options & (UI_ContainerOptions_NoMove | UI_ContainerOptions_NoResize))) {
        // TODO(Cian): @UI @Window need to figure out how relative positioning and immediate input work, something like adding to parents old_layout or something
        
        window->curr_layout = layout;
        window->old_layout = layout;
        
        if(layout.width == UI_AUTO_SIZE || layout.height == UI_AUTO_SIZE) {
            if(layout.width == UI_AUTO_SIZE) 
                ui_widget_add_property(window, UI_Widget_Property_Container_SizeAuto_Width);
            
            if(layout.height == UI_AUTO_SIZE) 
                ui_widget_add_property(window, UI_Widget_Property_Container_SizeAuto_Height);
            
            return;
        } 
        
        if(newly_created)
            ui_bring_to_front(window);
    }
    
    V2 mouse_delta = {};
    
    b32 mouse_move = os_sum_mouse_moves(&mouse_delta);
    
    OS_Event *mouse_down_event = null;
    OS_Event *mouse_up_event = null;
    b32 mouse_down = os_peek_mouse_button_event(&mouse_down_event, OS_Event_Type_MouseDown, OS_Mouse_Button_Left);
    b32 mouse_up = os_peek_mouse_button_event(&mouse_up_event, OS_Event_Type_MouseUp, OS_Mouse_Button_Left);
    
    V2 mouse_pos = os->mouse_pos;
    f32 title_height = window->style->title_height;
    f32 border_thickness = window->style->border_thickness;
    
    V4 title_rect = v4(window->curr_layout.x, window->curr_layout.y, window->curr_layout.width, title_height);
    V4 border_left = v4(window->curr_layout.x, window->curr_layout.y - title_height, border_thickness,  window->curr_layout.height - title_height);
    V4 border_right = v4(window->curr_layout.x + ( window->curr_layout.width - border_thickness), window->curr_layout.y + title_height, border_thickness,  window->curr_layout.height - title_height);
    V4 border_bottom = v4(window->curr_layout.x, window->curr_layout.y + ( window->curr_layout.height - border_thickness), window->curr_layout.width,  border_thickness);
    //not sure if I want the top of the window to do resizing
    //V4 border_top = v4(window->curr_layout.x, window->curr_layout.y - 20, 4.0f,  window->curr_layout.height - 20);
    
    b32 mouse_is_over_title = false;
    b32 mouse_is_over_right = false;
    b32 mouse_is_over_left = false;
    b32 mouse_is_over_bottom = false;
    
    b32 dragging_title = ui_widget_has_property(window, UI_Widget_Property_DraggingTitle);
    b32 dragging_left = ui_widget_has_property(window, UI_Widget_Property_ResizeLeft);
    b32 dragging_right = ui_widget_has_property(window, UI_Widget_Property_ResizeRight);
    b32 dragging_bottom = ui_widget_has_property(window, UI_Widget_Property_ResizeBottom);
    
    if(!(options & UI_ContainerOptions_NoMove))
        mouse_is_over_title = math_point_in_rect(title_rect, mouse_pos);
    if(!(options & UI_ContainerOptions_NoResize)) {
        mouse_is_over_right = math_point_in_rect(border_right, mouse_pos);
        mouse_is_over_left = math_point_in_rect(border_left, mouse_pos);
        mouse_is_over_bottom = math_point_in_rect(border_bottom, mouse_pos);
    }
    
    //windows are only active when dragging e.g. when button is held on the title bar
    // we only handle title bar events here since we don't want to take events away from our inner children
    // window click events are handled in ui_window_end
    // TODO(Cian): @Window for dragging and resizing add set struct clamped positions/sizes to prevent weird behaviour
    if(ui_is_id_equal(ui->active, window->id)) {
        if(os->mouse_off_screen)
            ui->active = ui_null_id();
        
        if(mouse_up) {
            ui->active = ui_null_id();
            
            if(!(mouse_is_over_title || mouse_is_over_left || mouse_is_over_right || mouse_is_over_bottom)) {
                ui->hot = ui_null_id();
            }
            
            os_take_event(mouse_up_event);
        } else {
            if(mouse_move) {
                
                if(dragging_title) {
                    window->curr_layout.x += mouse_delta.x;
                    window->curr_layout.y += mouse_delta.y;
                } else if(dragging_left) {
                    if(window->curr_layout.x < (window->curr_layout.x + window->curr_layout.width - mouse_delta.x) || mouse_delta.x < 0) {
                        window->curr_layout.x += mouse_delta.x;
                        window->curr_layout.width -= mouse_delta.x;
                    }
                } else if(dragging_right) {
                    if(window->curr_layout.x < (window->curr_layout.x + window->curr_layout.width + mouse_delta.x) || mouse_delta.x > 0) {
                        window->curr_layout.width += mouse_delta.x;
                    }
                } else if(dragging_bottom) {
                    if((window->curr_layout.y + title_height) < (window->curr_layout.y + window->curr_layout.height + mouse_delta.y) || mouse_delta.y > 0) {
                        window->curr_layout.height += mouse_delta.y;
                    }
                }
                
            }
        }
    } else if(ui_is_id_equal(ui->hot, window->id)) { 
        if(mouse_down) {
            //this only works if mouse is over window controls, to catch fall-through clicks end-window must do the same check
            ui_bring_to_front(window);
            ui->active = window->id;
            os_take_event(mouse_down_event);
        } else if(!(mouse_is_over_title || mouse_is_over_left || mouse_is_over_right || mouse_is_over_bottom)) {
            ui->hot = ui_null_id();
        }
    } else {
        
        if (!mouse_down && ui_is_id_equal(ui->hot, ui_null_id()) && ui->clickable_window == window) {
            if(mouse_is_over_title) {
                ui_widget_add_property(window, UI_Widget_Property_DraggingTitle);
                ui->hot = window->id;
            } else if(mouse_is_over_left) {
                ui_widget_add_property(window, UI_Widget_Property_ResizeLeft);
                ui->hot = window->id;
            } else if(mouse_is_over_right) {
                ui_widget_add_property(window, UI_Widget_Property_ResizeRight);
                ui->hot = window->id;
            } else if(mouse_is_over_bottom) {
                ui_widget_add_property(window, UI_Widget_Property_ResizeBottom);
                ui->hot = window->id;
            }
        }
        
    }
    
    if(!(ui_is_id_equal(ui->hot, window->id) || ui_is_id_equal(ui->active, window->id))) {
        ui_widget_remove_property(window, UI_Widget_Property_DraggingTitle);
        ui_widget_remove_property(window,  UI_Widget_Property_ResizeLeft);
        ui_widget_remove_property(window,  UI_Widget_Property_ResizeRight);
        ui_widget_remove_property(window,  UI_Widget_Property_ResizeBottom);
    }
    
    // TODO(Cian): @UI @Windows add some way of having a min/max width/height for windows when resizing, maybe use last frames info on the childrens size? Could be stored in the windows size_parameters, then we should also have some default min/max size
    
}

internal void ui_end_window() {
    UI_Widget *window = ui->window_stack.current;
    V2 mouse_pos = os->mouse_pos;
    
    OS_Event *event = null;
    b32 mouse_down = os_peek_mouse_button_event(&event, OS_Event_Type_MouseDown, OS_Mouse_Button_Left);
    
    if(mouse_down && ui->clickable_window == window && ui_widget_has_property(window, UI_Widget_Property_Draggable)) {
        ui_bring_to_front(window);
        os_take_event(event);
    }
    
    ui_pop_parent();
    ui_pop_window();
}
internal void ui_begin_row(char *string) {
    String8 widget_string = string_from_cstring(string);
    UI_Widget *row = ui_init_widget(widget_string, null);
    ui_push_parent(row);
    ui_widget_add_property(row, UI_Widget_Property_LayoutHorizontal);
}

internal void ui_end_row() {
    ui_pop_parent();
}

internal void ui_begin_column(char *string) {
    String8 widget_string = string_from_cstring(string);
    UI_Widget *col = ui_init_widget(widget_string, null);
    ui_push_parent(col);
    ui_widget_add_property(col, UI_Widget_Property_LayoutVertical);
}

internal void ui_end_column() {
    ui_pop_parent();
}

internal void ui_test_box(NVGcolor color, char *format,...) {
    String8 string = {};
    MAKE_FORMAT_STRING(string, format);
    UI_Widget *test_box = ui_init_widget(string, null);
    // TODO(Cian): @UI create a ui style table so that new colors etc can easily be added
    //test_box->color = color;
    ui_widget_add_property(test_box, UI_Widget_Property_RenderBackground);
}


