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
        UI_Widget *window = ui->active_window;
        
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
            } else {
                widget = (UI_Widget*)memory_arena_push(&os->permanent_arena, sizeof(UI_Widget));
                ui->widgets[idx] = widget;
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
                } else {
                    widget = (UI_Widget*)memory_arena_push(&os->permanent_arena, sizeof(UI_Widget));
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
    widget->window_parent = ui->active_window;
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
    ui->widget_size++;
    
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
    main_window->curr_layout = v4(0, 0, display_width, display_height);
    ui->active_window = main_window;
    ui_pop_width();
    ui_pop_height();
    
    ui->root_widget = main_window;
    ui->active_window = main_window;
    ui_push_parent(main_window);
    ui_widget_add_property(main_window, UI_Widget_Property_Container);
    ui_widget_add_property(main_window, UI_Widget_Property_LayoutHorizontal);
    
    if(ui->curr_frame == 0){
        ui->active = ui_null_id();
        ui->hot = ui_null_id();
    }
}

internal void ui_render(UI_Widget *root) {
    UI_Widget *window = root->tree_first_child;
    
    f32 x = window->curr_layout.x;
    f32 y = window->curr_layout.y;
    
    f32 width = window->curr_layout.width;
    f32 height = window->curr_layout.height;
    
    y += 20;
    height -= 20;
    
    //main body
    nvgBeginPath(vg_context);
    nvgRect(vg_context, x, y, width, height);
    nvgFillColor(vg_context, nvgRGB(30, 30 ,30));
    nvgFill(vg_context);
    
    //titlebar
    nvgBeginPath(vg_context);
    nvgRect(vg_context, x, y - 20, width, 20);
    nvgFillColor(vg_context, nvgRGB(100, 100 ,100));
    nvgFill(vg_context);
    
    b32 dragging_title = ui_widget_has_property(window, UI_Widget_Property_DraggingTitle);
    b32 dragging_left = ui_widget_has_property(window, UI_Widget_Property_ResizeLeft);
    b32 dragging_right = ui_widget_has_property(window, UI_Widget_Property_ResizeRight);
    b32 dragging_bottom = ui_widget_has_property(window, UI_Widget_Property_ResizeBottom);
    
    V4 title_rect = v4(window->curr_layout.x, window->curr_layout.y, window->curr_layout.width, 20);
    V4 border_left = v4(window->curr_layout.x, window->curr_layout.y + 20, 4.0f,  window->curr_layout.height - 20);
    V4 border_right = v4(window->curr_layout.x + ( window->curr_layout.width - 4.0f), window->curr_layout.y + 20, 4.0f,  window->curr_layout.height - 20);
    V4 border_bottom = v4(window->curr_layout.x, window->curr_layout.y + ( window->curr_layout.height - 4.0f), window->curr_layout.width,  4.0f);
    
    if(dragging_title) {
        
    } else if(dragging_left) {
        nvgBeginPath(vg_context);
        nvgRect(vg_context, border_left.x, border_left.y, border_left.width, border_left.height);
        nvgFillColor(vg_context, nvgRGB(255, 255 ,255));
        nvgFill(vg_context);
    } else if(dragging_right) {
        nvgBeginPath(vg_context);
        nvgRect(vg_context, border_right.x, border_right.y, border_right.width, border_right.height);
        nvgFillColor(vg_context, nvgRGB(255, 255 ,255));
        nvgFill(vg_context);
    } else if(dragging_bottom) {
        nvgBeginPath(vg_context);
        nvgRect(vg_context, border_bottom.x, border_bottom.y, border_bottom.width, border_bottom.height);
        nvgFillColor(vg_context, nvgRGB(255, 255 ,255));
        nvgFill(vg_context);
    }
}

internal void ui_end() {
    // TODO(Cian): @UI Need to either clear autolayout state here or, make the State as part of the frame_arena and UI_Widgets be maintained seperately in permanent arena
    ui->parent_stack.size = 0;
    ui->width_stack.size = 0;
    ui->height_stack.size = 0;
    
    ui->prev_widget = null;
    ui->parent_stack.current = null;
    ui->curr_frame++;
    //Autolayout for rendering and next frame goes here
    /* NOTE(Cian): Auto-layout works as follows:
    *  Traverse downwards through the tree, for every parent, measure it's children, this may
*  need to be recursive, we add childrens 
*/
    UI_Widget *root = ui->root_widget;
    // NOTE(Cian): Roots incoming constraints will always be tight so just set it's layout to be that
    root->curr_layout = {0, 0, root->parameters[0].size, root->parameters[1].size};
    //ui_do_layout(root);
    ui_render(ui->root_widget);
    ui->active_window = null;
}

// TODO(Cian): @UI @Styles add optional arguments to allow changing the default styles, or maybe optional styles should be pushed into ctx?
internal void ui_begin_window(V4 layout, b32 is_open, char *title...) {
    String8 window_string = {};
    MAKE_FORMAT_STRING(window_string, title);
    
    b32 newly_created = false;
    UI_Widget *window = ui_init_widget(window_string, &newly_created);
    ui_push_parent(window);
    ui_widget_add_property(window, UI_Widget_Property_Container);
    ui_widget_add_property(window, UI_Widget_Property_LayoutVertical);
    ui_widget_add_property(window, UI_Widget_Property_TitleBar);
    
    if(newly_created) {
        window->curr_layout = layout;
        window->old_layout = layout;
    }
    
    V2 mouse_delta = {};
    
    // TODO(Cian): @UI should add a method for this
    b32 mouse_move = os_sum_mouse_moves(&mouse_delta);
    
    
    OS_Event *mouse_down_event = null;
    OS_Event *mouse_up_event = null;
    b32 mouse_down = os_peek_mouse_button_event(&mouse_down_event, OS_Event_Type_MouseDown, OS_Mouse_Button_Left);
    b32 mouse_up = os_peek_mouse_button_event(&mouse_up_event, OS_Event_Type_MouseUp, OS_Mouse_Button_Left);
    
    V2 mouse_pos = os->mouse_pos;
    // TODO(Cian): This will need to use the windows style to calculate later
    V4 title_rect = v4(window->curr_layout.x, window->curr_layout.y, window->curr_layout.width, 20);
    V4 border_left = v4(window->curr_layout.x, window->curr_layout.y - 20, 4.0f,  window->curr_layout.height - 20);
    V4 border_right = v4(window->curr_layout.x + ( window->curr_layout.width - 4.0f), window->curr_layout.y + 20, 4.0f,  window->curr_layout.height - 20);
    V4 border_bottom = v4(window->curr_layout.x, window->curr_layout.y + ( window->curr_layout.height - 4.0f), window->curr_layout.width,  4.0f);
    //not sure if I want the top of the window to do resizing
    //V4 border_top = v4(window->curr_layout.x, window->curr_layout.y - 20, 4.0f,  window->curr_layout.height - 20);
    
    b32 mouse_is_over_title = math_point_in_rect(title_rect, mouse_pos);
    b32 mouse_is_over_right = math_point_in_rect(border_right, mouse_pos);
    b32 mouse_is_over_left = math_point_in_rect(border_left, mouse_pos);
    b32 mouse_is_over_bottom = math_point_in_rect(border_bottom, mouse_pos);
    
    b32 dragging_title = ui_widget_has_property(window, UI_Widget_Property_DraggingTitle);
    b32 dragging_left = ui_widget_has_property(window, UI_Widget_Property_ResizeLeft);
    b32 dragging_right = ui_widget_has_property(window, UI_Widget_Property_ResizeRight);
    b32 dragging_bottom = ui_widget_has_property(window, UI_Widget_Property_ResizeBottom);
    
    //windows are only active when dragging e.g. when button is held on the title bar
    // we only handle title bar events here since we don't want to take events away from our inner children
    // window click events are handled in ui_window_end
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
                    window->curr_layout.x += mouse_delta.x;
                    window->curr_layout.width -= mouse_delta.x;
                } else if(dragging_right) {
                    window->curr_layout.width += mouse_delta.x;
                } else if(dragging_bottom) {
                    window->curr_layout.height += mouse_delta.y;
                }
            }
        }
    } else if(ui_is_id_equal(ui->hot, window->id)) { 
        if(mouse_down) {
            ui->active = window->id;
            os_take_event(mouse_down_event);
        } else if(!(mouse_is_over_title || mouse_is_over_left || mouse_is_over_right || mouse_is_over_bottom)) {
            ui->hot = ui_null_id();
        }
    } else {
        
        if (!mouse_down && ui_is_id_equal(ui->hot, ui_null_id())) {
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
    
    // TODO(Cian): @Checkpoint first, get the style table implemented, then do all the TODO's listed above, cleanup the code, and get the resizing working. If there is still time extend to work with multiple windows
    
}

internal void ui_end_window() {
    ui_pop_parent();
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


