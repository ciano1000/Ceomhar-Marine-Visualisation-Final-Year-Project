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
                UI_Widget *parent = curr->window_parent;
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
    parent->child_parameters_sum[0] = {};
    parent->child_parameters_sum[1] = {};
    parent->child_ratio_sum[0] = 0;
    parent->child_ratio_sum[1] = 0;
    
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
    ui->active_window = window;
    u32 size = ui->window_stack.size;
    if(size > 0)
        ui->window_stack.stack[size - 1] = ui->window_stack.current;
    ui->window_stack.current = window;
    ui->window_stack.size++;
}

internal void ui_pop_window() {
    //when a window ends, it needs to be the previous widget, not its last child, so its siblings can correctly reference it
    ui->active_window = ui->active_window->window_parent; //this is a hack to let end_window work when begin_window is closed
    ui->prev_widget = ui->window_stack.current;
    ui->window_stack.size--;
    u32 size = ui->window_stack.size;
    ui->window_stack.current = ui->window_stack.stack[size - 1];
}

internal void ui_push_width(f32 min, f32 pref, f32 max) {
    u32 *s_size = &ui->width_stack.size;
    ui->width_stack.stack[*s_size] = ui->width_stack.current;
    (*s_size)++;
    ui->width_stack.current.min = min;
    ui->width_stack.current.pref = pref;
    ui->width_stack.current.max = max;
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

internal void ui_push_height(f32 min, f32 pref, f32 max) {
    u32 *s_size = &ui->height_stack.size;
    ui->height_stack.stack[*s_size] = ui->height_stack.current;
    (*s_size)++;
    ui->width_stack.current.min = min;
    ui->width_stack.current.pref = pref;
    ui->width_stack.current.max = max;
}

internal void ui_pop_height() {
    u32 *size = &ui->height_stack.size;
    (*size)--;
    ui->height_stack.current = ui->height_stack.stack[*size];
}

internal UI_Widget* ui_init_widget(String8 string, b32 *newly_created) {
    if(ui->active_window == null && ui->root_widget != null && ui->prev_widget != null)
        return null;
    
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
            window = ui->window_stack.current;
        
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
    
    //clear the tree stuff
    widget->tree_parent = null;
    widget->tree_first_child = null;
    widget->tree_last_child = null;
    widget->tree_next_sibling = null;
    widget->tree_prev_sibling = null;
    
    widget->old_layout = widget->curr_layout;
    //widget->curr_layout = {0};
    widget->last_frame = ui->curr_frame;
    
    widget->parameters[0] = ui->width_stack.current;
    widget->parameters[1] = ui->height_stack.current;
    
    if(ui->window_stack.current)
        widget->window_parent = ui->window_stack.current;
    
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
    
    // NOTE(Cian): Some non-interactable widgets will still be explicitly named for debugging purposes
    // TODO(Cian): @UI might reqork this so it only does this in debug mode?
    String8 main_row_string = string_from_cstring("main");
    UI_Widget *main_window = ui_init_widget(main_row_string, null);
    
    if(ui->curr_frame == 0)
        ui_bring_to_front(main_window);
    
    main_window->curr_layout = v4(0, 0, display_width, display_height);
    
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

internal void ui_do_layout() {
    UI_Widget **layout_stack = (UI_Widget **)memory_arena_push(&os->frame_arena, sizeof(UI_Widget*) * ui->widget_size);
    UI_Widget *layout_stack_current = null;
    u32 size = 0;
    
#define layout_stack_push(widget) {\
if(size > 0 && size < ui->widget_size)\
layout_stack[size - 1] = layout_stack_current;\
layout_stack_current = widget;\
size++;\
}
    
#define layout_stack_pop() {\
if(size >= 1)\
size--;\
if(size > 0) {\
layout_stack_current = layout_stack[size - 1];\
} else {\
layout_stack_current = null;\
}\
}
    
    layout_stack_push(ui->root_widget);
    
    V2 returned_size = {};
    UI_Widget *last_child = null;
    
    while(size > 0) {
        UI_Widget *widget = layout_stack_current;
        layout_stack_pop();
        //parents always loop through the children twice, needed because of how we use expanded widgets, first loop is for sizes, second loop is for resizing and positioning
        
        f32 parent_width = widget->curr_layout.width - widget->style->padding.x0 - widget->style->padding.x1;
        f32 parent_height = widget->curr_layout.height - widget->style->padding.y0 - widget->style->padding.y1 - widget->style->title_height;
        
        if(ui_widget_has_property(widget, UI_Widget_Property_ScrollVertical)) {
            parent_width -= UI_SEC_SCROLL_SIZE;
        } 
        
        if(ui_widget_has_property(widget, UI_Widget_Property_ScrollHorizontal)) {
            parent_height -= UI_SEC_SCROLL_SIZE;
        } 
        
        f32 initial_offset_x = widget->style->padding.x0 + widget->scroll_offset_x;
        f32 initial_offset_y = widget->style->padding.y0 + widget->style->title_height + widget->scroll_offset_y;
        
        //regular layout flow
        if(ui_widget_has_property(widget, UI_Widget_Property_Container)) {
            // TODO(Cian): Handle any menu-bar widgets if they exist, if they exist they should be the first child. Also handle auto-sized windows, e.g. we sum up the child sizes at the end to get our size for the next frame
        }
        
        if(ui_widget_has_property(widget, UI_Widget_Property_LayoutHorizontal)) {
            f32 sum_size = widget->child_parameters_sum[0].pref;
            f32 min_sum = widget->child_parameters_sum[0].min;
            f32 max_sum = widget->child_parameters_sum[0].max;
            
            f32 height_pref = widget->child_parameters_sum[1].pref;
            f32 height_min = widget->child_parameters_sum[1].pref;
            f32 height_max = widget->child_parameters_sum[1].pref;
            
            f32 offset = initial_offset_x;
            
            //the parents size will never be less than the min_sum, we just check if the parent is a window, if it is we add a scrollbar in the containers creation function
            
            UI_Widget *curr = widget->tree_first_child;
            if(sum_size > parent_width) {
                while(curr) {
                    if(ui_widget_has_property(curr, UI_Widget_Property_InstantLayout)) {
                        //some widgets handle their own layout, e.g. windows, some containers, splitters etc
                        layout_stack_push(curr);
                        curr = curr->tree_next_sibling;
                        continue;
                    }
                    
                    f32 width = UI_PARAM_IS_RATIO(curr->parameters[0]) ? curr->parameters[0].pref * parent_width: curr->parameters[0].pref;
                    curr->parameters[0].pref = width;
                    f32 height = UI_PARAM_IS_RATIO(curr->parameters[1]) ? curr->parameters[1].pref * parent_height: curr->parameters[1].pref; 
                    curr->parameters[1].pref = height;
                    
                    f32 factor = 0;
                    if(sum_size != min_sum)
                        factor = (curr->parameters[0].pref - curr->parameters[0].min) / (sum_size - min_sum);
                    curr->curr_layout.width = CLAMP_MIN(curr->parameters[0].pref - (factor * (sum_size - parent_width)), curr->parameters[0].min);
                    curr->curr_layout.x = offset;
                    offset += curr->curr_layout.width;
                    
                    if(height > parent_height && (curr->parameters[1].pref != curr->parameters[1].min)) {
                        factor = (curr->parameters[1].pref - curr->parameters[1].min) / (curr->parameters[1].pref - curr->parameters[1].min);
                        height -= factor * (height - parent_height);
                    } else if(height < parent_height && (curr->parameters[1].pref != curr->parameters[1].max)) {
                        factor = (curr->parameters[1].max - curr->parameters[1].pref) / (curr->parameters[1].max - curr->parameters[1].pref);
                        height += factor * (parent_height - height);
                    }
                    height = CLAMP_MIN(height, curr->parameters[1].min);
                    curr->curr_layout.height = height;
                    //curr->curr_layout.y = (parent_height / 2) - (height / 2) + initial_offset_y;
                    curr->curr_layout.y = initial_offset_y;
                    
                    layout_stack_push(curr);
                    curr = curr->tree_next_sibling;
                }
            } else if(sum_size < parent_width) {
                while(curr) {
                    if(ui_widget_has_property(curr, UI_Widget_Property_InstantLayout)) {
                        //some widgets handle their own layout, e.g. windows, some containers, splitters etc
                        layout_stack_push(curr);
                        curr = curr->tree_next_sibling;
                        continue;
                    }
                    
                    f32 width = UI_PARAM_IS_RATIO(curr->parameters[0]) ? curr->parameters[0].pref * parent_width: curr->parameters[0].pref;
                    curr->parameters[0].pref = width;
                    f32 height = UI_PARAM_IS_RATIO(curr->parameters[1]) ? curr->parameters[1].pref * parent_height: curr->parameters[1].pref; 
                    curr->parameters[1].pref = height;
                    
                    f32 factor = 0;
                    if(sum_size != min_sum)
                        factor = (curr->parameters[0].max - curr->parameters[0].pref) / (max_sum - sum_size);
                    curr->curr_layout.width = CLAMP_MAX(curr->parameters[0].pref + (factor * (parent_width - sum_size)), curr->parameters[0].max);
                    curr->curr_layout.x = offset;
                    offset += curr->curr_layout.width;
                    
                    if(height > parent_height && (curr->parameters[1].pref != curr->parameters[1].min)) {
                        factor = (curr->parameters[1].pref - curr->parameters[1].min) / (curr->parameters[1].pref - curr->parameters[1].min);
                        height -= factor * (height - parent_height);
                    } else if(height < parent_height && (curr->parameters[1].pref != curr->parameters[1].max)) {
                        factor = (curr->parameters[1].max - curr->parameters[1].pref) / (curr->parameters[1].max - curr->parameters[1].pref);
                        height += factor * (parent_height - height);
                    }
                    
                    height = CLAMP_MIN(height, curr->parameters[1].min);
                    curr->curr_layout.height = height;
                    //curr->curr_layout.y = (parent_height / 2) - (height / 2) + initial_offset_y;
                    curr->curr_layout.y = initial_offset_y;
                    
                    layout_stack_push(curr);
                    curr = curr->tree_next_sibling;
                }
            } else {
                while(curr) {
                    if(ui_widget_has_property(curr, UI_Widget_Property_InstantLayout)) {
                        //some widgets handle their own layout, e.g. windows, some containers, splitters etc
                        layout_stack_push(curr);
                        curr = curr->tree_next_sibling;
                        continue;
                    }
                    
                    f32 width = UI_PARAM_IS_RATIO(curr->parameters[0]) ? curr->parameters[0].pref * parent_width: curr->parameters[0].pref;
                    curr->parameters[0].pref = width;
                    f32 height = UI_PARAM_IS_RATIO(curr->parameters[1]) ? curr->parameters[1].pref * parent_height: curr->parameters[1].pref; 
                    curr->parameters[1].pref = height;
                    
                    curr->curr_layout.width = width;
                    curr->curr_layout.x = offset;
                    offset += width;
                    
                    f32 factor = 0;
                    if(height > parent_height && (curr->parameters[1].pref != curr->parameters[1].min)) {
                        factor = (curr->parameters[1].pref - curr->parameters[1].min) / (curr->parameters[1].pref - curr->parameters[1].min);
                        height -= factor * (height - parent_height);
                    } else if(height < parent_height && (curr->parameters[1].pref != curr->parameters[1].max)) {
                        factor = (curr->parameters[1].max - curr->parameters[1].pref) / (curr->parameters[1].max - curr->parameters[1].pref);
                        height += factor * (parent_height - height);
                    }
                    height = CLAMP_MIN(height, curr->parameters[1].min);
                    curr->curr_layout.height = height;
                    //curr->curr_layout.y = (parent_height / 2) - (height / 2) + initial_offset_y;
                    curr->curr_layout.y = initial_offset_y;
                    
                    layout_stack_push(curr);
                    curr = curr->tree_next_sibling;
                }
            }
            
        } else if(ui_widget_has_property(widget, UI_Widget_Property_LayoutVertical)) {
            f32 sum_size = widget->child_parameters_sum[1].pref;
            f32 min_sum = widget->child_parameters_sum[1].min;
            f32 max_sum = widget->child_parameters_sum[1].max;
            
            f32 width_pref = widget->child_parameters_sum[0].pref;
            f32 width_min = widget->child_parameters_sum[0].pref;
            f32 width_max = widget->child_parameters_sum[0].pref;
            
            f32 offset = initial_offset_y;
            
            //the parents size will never be less than the min_sum, we just check if the parent is a window, if it is we add a scrollbar in the containers creation function
            
            UI_Widget *curr = widget->tree_first_child;
            if(sum_size > parent_height) {
                while(curr) {
                    if(ui_widget_has_property(curr, UI_Widget_Property_InstantLayout)) {
                        //some widgets handle their own layout, e.g. windows, some containers, splitters etc
                        layout_stack_push(curr);
                        curr = curr->tree_next_sibling;
                        continue;
                    }
                    
                    f32 width = UI_PARAM_IS_RATIO(curr->parameters[0]) ? curr->parameters[0].pref * parent_width: curr->parameters[0].pref;
                    curr->parameters[0].pref = width;
                    f32 height = UI_PARAM_IS_RATIO(curr->parameters[1]) ? curr->parameters[1].pref * parent_height: curr->parameters[1].pref; 
                    curr->parameters[1].pref = height;
                    
                    f32 factor = 0;
                    if(sum_size != min_sum)
                        factor = (curr->parameters[1].pref - curr->parameters[1].min) / (sum_size - min_sum);
                    curr->curr_layout.height = CLAMP_MIN(curr->parameters[1].pref - (factor * (sum_size - parent_height)), curr->parameters[1].min);
                    curr->curr_layout.y = offset;
                    offset += curr->curr_layout.height;
                    
                    if(width > parent_width && (curr->parameters[0].pref != curr->parameters[0].min)) {
                        factor = (curr->parameters[0].pref - curr->parameters[0].min) / (curr->parameters[0].pref - curr->parameters[0].min);
                        width -= factor * (width - parent_width);
                    } else if(width < parent_width && (curr->parameters[0].pref != curr->parameters[0].max)) {
                        factor = (curr->parameters[0].max - curr->parameters[0].pref) / (curr->parameters[0].max - curr->parameters[0].pref);
                        width += factor * (parent_width - width);
                    }
                    width = CLAMP_MIN(width, curr->parameters[0].min);
                    curr->curr_layout.width = width;
                    //curr->curr_layout.x = (parent_width / 2) - (width / 2) + initial_offset_x;
                    curr->curr_layout.x = initial_offset_x;
                    
                    layout_stack_push(curr);
                    curr = curr->tree_next_sibling;
                }
            } else if(sum_size < parent_height) {
                while(curr) {
                    if(ui_widget_has_property(curr, UI_Widget_Property_InstantLayout)) {
                        //some widgets handle their own layout, e.g. windows, some containers, splitters etc
                        layout_stack_push(curr);
                        curr = curr->tree_next_sibling;
                        continue;
                    }
                    
                    f32 width = UI_PARAM_IS_RATIO(curr->parameters[0]) ? curr->parameters[0].pref * parent_width: curr->parameters[0].pref;
                    curr->parameters[0].pref = width;
                    f32 height = UI_PARAM_IS_RATIO(curr->parameters[1]) ? curr->parameters[1].pref * parent_height: curr->parameters[1].pref; 
                    curr->parameters[1].pref = height;
                    
                    f32 factor = 0;
                    if(max_sum != sum_size)
                        factor = (curr->parameters[1].max - curr->parameters[1].pref) / (max_sum - sum_size);
                    curr->curr_layout.height = CLAMP_MAX(curr->parameters[1].pref + (factor * (parent_height - sum_size)), curr->parameters[1].max);
                    curr->curr_layout.y = offset;
                    offset += curr->curr_layout.height;
                    
                    if(width > parent_width && (curr->parameters[0].pref != curr->parameters[0].min)) {
                        factor = (curr->parameters[0].pref - curr->parameters[0].min) / (curr->parameters[0].pref - curr->parameters[0].min);
                        width -= factor * (width - parent_width);
                    } else if(width < parent_width && (curr->parameters[0].pref != curr->parameters[0].max)) {
                        factor = (curr->parameters[0].max - curr->parameters[0].pref) / (curr->parameters[0].max - curr->parameters[0].pref);
                        width += factor * (parent_width - width);
                    }
                    
                    width = CLAMP_MIN(width, curr->parameters[0].min);
                    curr->curr_layout.width = width;
                    //curr->curr_layout.x = (parent_width / 2) - (width / 2) + initial_offset_x;
                    curr->curr_layout.x = initial_offset_x;
                    
                    layout_stack_push(curr);
                    curr = curr->tree_next_sibling;
                }
            } else {
                while(curr) {
                    if(ui_widget_has_property(curr, UI_Widget_Property_InstantLayout)) {
                        //some widgets handle their own layout, e.g. windows, some containers, splitters etc
                        layout_stack_push(curr);
                        curr = curr->tree_next_sibling;
                        continue;
                    }
                    
                    f32 width = UI_PARAM_IS_RATIO(curr->parameters[0]) ? curr->parameters[0].pref * parent_width: curr->parameters[0].pref;
                    curr->parameters[0].pref = width;
                    f32 height = UI_PARAM_IS_RATIO(curr->parameters[1]) ? curr->parameters[1].pref * parent_height: curr->parameters[1].pref; 
                    curr->parameters[1].pref = height;
                    
                    curr->curr_layout.height = height;
                    curr->curr_layout.y = offset;
                    offset += height;
                    
                    f32 factor = 0;
                    if(width > parent_width && (curr->parameters[0].pref != curr->parameters[0].min)) {
                        factor = (curr->parameters[0].pref - curr->parameters[0].min) / (curr->parameters[0].pref - curr->parameters[0].min);
                        width -= factor * (width - parent_width);
                    } else if(width < parent_width && (curr->parameters[0].pref != curr->parameters[0].max)) {
                        factor = (curr->parameters[0].max - curr->parameters[0].pref) / (curr->parameters[0].max - curr->parameters[0].pref);
                        width += factor * (parent_width - width);
                    }
                    
                    width = CLAMP_MIN(width, curr->parameters[0].min);
                    curr->curr_layout.width = width;
                    //curr->curr_layout.x = (parent_width / 2) - (width / 2) + initial_offset_x;
                    curr->curr_layout.x = initial_offset_x;
                    
                    layout_stack_push(curr);
                    curr = curr->tree_next_sibling;
                }
            }
        }
        
        /*
        widget->child_parameters_sum[0] = {};
        widget->child_parameters_sum[1] = {};
        widget->child_ratio_sum[0] = 0;
        widget->child_ratio_sum[1] = 0;*/
    }
    /*if(ui_widget_has_property(widget, UI_Widget_Property_Container)) {
        
    }*/
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
        
        //windows "old" layouts aren't actually old, they are computed this frame unlike other widgets
        f32 initial_x_offset = window->old_layout.x;
        f32 initial_y_offset = window->old_layout.y;
        
        nvgScissor(vg_context, window->curr_layout.x, window->curr_layout.y, window->curr_layout.width, window->curr_layout.height);
        
        while(render_stack_current) {
            //pop element from stack
            UI_Widget *curr = render_stack_current;
            
            f32 x = initial_x_offset + curr->curr_layout.x;
            f32 y = initial_y_offset + curr->curr_layout.y;
            
            //initial offsets are the current windows offsets, we don't need to add anything to it
            if(ui_widget_has_property(curr, UI_Widget_Property_Container)) {
                x = curr->window_parent->old_layout.x + curr->curr_layout.x;
                y = curr->window_parent->old_layout.y + curr->curr_layout.y;
            }
            
            f32 width = curr->curr_layout.width;
            f32 height = curr->curr_layout.height;
            
            //apply offsets to old_layout so we can do input next frame
            curr->old_layout.x = x;
            curr->old_layout.y = y;
            curr->old_layout.width = width;
            curr->old_layout.height = height;
            
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
                nvgText(vg_context, x + F32_FLOOR(width / 2), y + F32_FLOOR(title_height / 2), curr->string.data, null);
            }
            
            if(ui_widget_has_property(curr, UI_Widget_Property_RenderText)) {
                nvgTextAlign(vg_context, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
                nvgFontFace(vg_context, "roboto-medium");
                nvgFontSize(vg_context, curr->style->font_size);
                nvgFillColor(vg_context, curr->style->colors[UI_ColorState_Normal].text_color);
                nvgText(vg_context, x + F32_FLOOR(width / 2), y + F32_FLOOR(height / 2), curr->string.data, null);
            }
            
            if(ui_widget_has_property(curr, UI_Widget_Property_RenderCloseButton)) {
                NVGcolor color = UI_RED;
                f32 r = curr->style->title_height / 5;
                f32 button_x = x + curr->old_layout.width - curr->style->padding.x1 - (r / 2);
                f32 button_y = y + curr->style->title_height / 2;
                
                nvgBeginPath(vg_context);
                nvgCircle(vg_context, button_x, button_y, r);
                nvgFillColor(vg_context, color);
                nvgFill(vg_context);
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
            
            if(ui_widget_has_property(curr, UI_Widget_Property_ScrollVertical)) {
                NVGcolor color = UI_LIGHT;
                if(ui_is_id_equal(ui->hot, curr->id) && ui_widget_has_property(curr, UI_Widget_Property_DraggingVerticalScroll))  
                    color = UI_WHITE;
                f32 min_scroll_offset = -CLAMP_MIN(window->child_parameters_sum[1].min - UI_AVAILABLE_HEIGHT(window), 0);
                
                V4 rect = {};
                
                f32 max_scrollbar_height = window->old_layout.height - (2*window->style->border_thickness) - window->style->title_height;
                
                rect.height = CLAMP_MIN(UI_AVAILABLE_HEIGHT(curr) / curr->child_parameters_sum[1].min * max_scrollbar_height, UI_MIN_SCROLL_MAIN);
                
                f32 min_scroll_center_position = curr->old_layout.y + curr->old_layout.height - curr->style->border_thickness - (rect.height / 2);
                
                f32 max_scroll_center_position = curr->old_layout.y + curr->style->title_height + curr->style->border_thickness + (rect.height/2);
                
                f32 mapped_scroll_center = (curr->scroll_offset_y - min_scroll_offset) * ((max_scroll_center_position - min_scroll_center_position) / (-min_scroll_offset)) + min_scroll_center_position;
                
                rect.y = mapped_scroll_center - (rect.height/2);
                rect.width = UI_SEC_SCROLL_SIZE;
                rect.x = curr->old_layout.x + curr->old_layout.width - curr->style->border_thickness - rect.width;
                
                nvgBeginPath(vg_context);
                nvgRect(vg_context, rect.x, rect.y, rect.width, rect.height);
                nvgFillColor(vg_context, color);
                nvgFill(vg_context);
            }
            
            if(ui_widget_has_property(curr, UI_Widget_Property_ScrollHorizontal)) {
                NVGcolor color = UI_LIGHT;
                if(ui_is_id_equal(ui->hot, curr->id) && ui_widget_has_property(curr, UI_Widget_Property_DraggingHorizontalScroll))  
                    color = UI_WHITE;
                f32 min_scroll_offset = -CLAMP_MIN(window->child_parameters_sum[0].min - UI_AVAILABLE_WIDTH(window), 0);
                
                V4 rect = {};
                
                f32 max_scrollbar_width = window->old_layout.width - (2 * window->style->border_thickness);
                
                rect.width = CLAMP_MIN(UI_AVAILABLE_WIDTH(curr) / curr->child_parameters_sum[0].min * max_scrollbar_width, UI_MIN_SCROLL_MAIN);
                
                f32 min_scroll_center_position = curr->old_layout.x + curr->old_layout.width - curr->style->border_thickness - (rect.width / 2);
                
                if(ui_widget_has_property(window, UI_Widget_Property_ScrollVertical)) {
                    max_scrollbar_width -= UI_SEC_SCROLL_SIZE;
                    min_scroll_center_position -= UI_SEC_SCROLL_SIZE;
                    rect.width = CLAMP_MIN(UI_AVAILABLE_WIDTH(curr) / curr->child_parameters_sum[0].min * max_scrollbar_width, UI_MIN_SCROLL_MAIN); 
                }
                
                f32 max_scroll_center_position = curr->old_layout.x + curr->style->border_thickness + (rect.width/2);
                
                f32 mapped_scroll_center = (curr->scroll_offset_x - min_scroll_offset) * ((max_scroll_center_position - min_scroll_center_position) / (-min_scroll_offset)) + min_scroll_center_position;
                
                rect.x = mapped_scroll_center - (rect.width/2);
                rect.height = UI_SEC_SCROLL_SIZE;
                rect.y = curr->old_layout.y + curr->old_layout.height - curr->style->border_thickness - rect.height;
                
                nvgBeginPath(vg_context);
                nvgRect(vg_context, rect.x, rect.y, rect.width, rect.height);
                nvgFillColor(vg_context, color);
                nvgFill(vg_context);
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
            
            if(ui_widget_has_property(curr, UI_Widget_Property_Container)) {
                nvgSave(vg_context);
                
                f32 clip_width = window->curr_layout.width - window->style->padding.x1 -  window->style->padding.x0;
                f32 clip_height = window->curr_layout.height - window->style->padding.y1 -  window->style->padding.y0 - window->style->title_height;
                
                if(ui_widget_has_property(curr, UI_Widget_Property_ScrollHorizontal))
                    clip_height -= UI_SEC_SCROLL_SIZE;
                if(ui_widget_has_property(curr, UI_Widget_Property_ScrollVertical))
                    clip_width -= UI_SEC_SCROLL_SIZE;
                //do clipping/nvgScissor stuff here
                nvgScissor(vg_context, window->curr_layout.x + window->style->padding.x0, 
                           window->curr_layout.y + window->style->title_height + window->style->padding.y0, 
                           clip_width,
                           clip_height);
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
        nvgRestore(vg_context);
        window = window->prev_sorted_container;
    }
}

internal void ui_end() {
    // TODO(Cian): @UI Need to either clear autolayout state here or, make the State as part of the frame_arena and UI_Widgets be maintained seperately in permanent arena
    ui->width_stack.size = 0;
    ui->height_stack.size = 0;
    
    ui->prev_widget = null;
    ui->parent_stack.current = null;
    ui->parent_stack.size = 0;
    ui->window_stack.size = 0;
    ui->window_stack.current = null;
    
    // TODO(Cian): maybe doing this cleanup via the sorted linked list might be better idk we'll see
    //Clean up widgets that weren't persisted this frame
    for (u32 i = 0; i < UI_WIDGET_TABLE_SIZE; i += 1) {
        
        UI_Widget *curr = ui->widgets[i];
        UI_Widget *prev = null;
        
        while(curr) {
            UI_Widget *hash_next = curr->hash_next;
            
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
                
                if(curr->tree_parent->tree_first_child == curr) {
                    curr->tree_parent->tree_first_child = curr->tree_next_sibling;
                }
                
                if(curr->tree_parent->tree_last_child == curr) {
                    curr->tree_parent->tree_last_child = curr->tree_prev_sibling;
                }
                
                if(curr->tree_prev_sibling) {
                    curr->tree_prev_sibling->tree_next_sibling = curr->tree_next_sibling;
                }
                
                if(curr->tree_next_sibling) {
                    curr->tree_next_sibling->tree_prev_sibling = curr->tree_prev_sibling;
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
                
                //set curr to a temp var and clear the data it points to
                
                *curr = {};
                curr->hash_next = ui->widget_free_list;
                ui->widget_free_list = curr;
                ui->widget_free_list_count++;
                
            } else {
                prev = curr;
            }
            curr = hash_next;
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
    
    ui_do_layout();
    
    //rendering: for each window in sorted order recursively render it and its children
    ui_render(ui->root_widget);
    ui->active_window = null;
}

internal void ui_measure_widget(UI_Widget *widget) {
    f32 width_padding = widget->style->padding.x0 + widget->style->padding.x1;
    f32 height_padding = widget->style->padding.y0 + widget->style->padding.y1;
    
    if(ui_widget_has_property(widget, UI_Widget_Property_LayoutHorizontal) || ui_widget_has_property(widget, UI_Widget_Property_LayoutVertical)) {
        //V3 our_width = SUM_V3(widget->child_parameters_sum[0], {width_padding, width_padding, width_padding});
        
        f32 *min_width = &widget->parameters[0].min;
        f32 *pref_width = &widget->parameters[0].pref;
        f32 *max_width = &widget->parameters[0].max;
        
        //Below code coincidentally handles auto-sizing since auto-sized values are always 0
        if((widget->child_parameters_sum[0].min + width_padding) > *min_width) {
            *min_width = widget->child_parameters_sum[0].min + width_padding;
        }
        
        if((widget->child_parameters_sum[0].pref + width_padding) > *pref_width) {
            *pref_width = widget->child_parameters_sum[0].pref + width_padding;
        }
        
        if((widget->child_parameters_sum[0].max + width_padding) > *max_width) {
            *max_width = widget->child_parameters_sum[0].max + width_padding;
        }
        
        f32 *min_height = &widget->parameters[1].min;
        f32 *pref_height = &widget->parameters[1].pref;
        f32 *max_height = &widget->parameters[1].max;
        
        if((widget->child_parameters_sum[1].min + height_padding) > *min_height) {
            *min_height = widget->child_parameters_sum[1].min + height_padding;
        }
        
        if((widget->child_parameters_sum[1].pref + height_padding) > *pref_height) {
            *pref_height = widget->child_parameters_sum[1].pref + height_padding;
        }
        
        if((widget->child_parameters_sum[1].max + height_padding) > *max_height) {
            *max_height = widget->child_parameters_sum[1].max + height_padding;
        }
        // TODO(Cian): @UI not sure about ratios at all, make sure to investigate later
        *pref_width += (*pref_width * widget->child_ratio_sum[0]);
        *pref_height += (*pref_height * widget->child_ratio_sum[1]);
        
    } else if(ui_widget_has_property(widget, UI_Widget_Property_RenderText)) {
        nvgTextAlign(vg_context, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
        nvgFontFace(vg_context, "roboto-medium");
        nvgFontSize(vg_context, widget->style->font_size);
        f32 bounds[4];
        f32 width = nvgTextBounds(vg_context, 0, 0, widget->string.data, null, bounds);
        f32 height = bounds[3] - bounds[1];
        
        // NOTE(Cian): Render text widgets are semi-auto sized by default to ensure the text fits
        if(width > widget->parameters[0].min)
            widget->parameters[0].min = width;
        
        if(height > widget->parameters[1].min)
            widget->parameters[1].min = height;
        
        //need to check for ratios here
        if(!UI_PARAM_IS_RATIO(widget->parameters[0])) {
            if((width + width_padding) > widget->parameters[0].pref)
                widget->parameters[0].pref = width + width_padding;
        }
        if(!UI_PARAM_IS_RATIO(widget->parameters[0])) {
            if((height + height_padding) > widget->parameters[1].pref)
                widget->parameters[1].pref = height + height_padding;
        }
        //ensure the max size is at least the same as the pref
        if(widget->parameters[0].pref > widget->parameters[0].max)
            widget->parameters[0].max = widget->parameters[0].pref;
        //ensure the max size is at least the same as the pref
        if(widget->parameters[1].pref > widget->parameters[1].max)
            widget->parameters[1].max = widget->parameters[1].pref;
        
    } else {
        //default fallthrough to handle auto-widgets that haven't had explicit auto-sizing defined yet
        if(UI_PARAM_IS_AUTO(widget->parameters[0])) {
            widget->parameters[0] = v3(0, 100, 200);
        }
        
        if(UI_PARAM_IS_AUTO(widget->parameters[1])) {
            widget->parameters[1] = v3(0, 100, 200);
        }
    }
    
    //add to the parents sum - ratio sized widgets only add their min & max, not the pref since that stores the ratio - the ratio is added to the parents child_ratio_sum
    if(ui_widget_has_property(widget->tree_parent, UI_Widget_Property_LayoutHorizontal)) {
        widget->tree_parent->child_parameters_sum[0].min += widget->parameters[0].min;
        if(!UI_PARAM_IS_RATIO(widget->parameters[0])) {
            widget->tree_parent->child_parameters_sum[0].pref += widget->parameters[0].pref;
        } else {
            widget->tree_parent->child_ratio_sum[0] += widget->parameters[0].pref;
        }
        widget->tree_parent->child_parameters_sum[0].max += widget->parameters[0].max;
        
        widget->tree_parent->child_parameters_sum[1].min = (widget->parameters[1].min > widget->tree_parent->child_parameters_sum[1].min) ? widget->parameters[1].min : widget->tree_parent->child_parameters_sum[1].min;
        
        if(!UI_PARAM_IS_RATIO(widget->parameters[1])) {
            widget->tree_parent->child_parameters_sum[1].pref = (widget->parameters[1].pref > widget->tree_parent->child_parameters_sum[1].pref) ? widget->parameters[1].pref : widget->tree_parent->child_parameters_sum[1].pref;
        } else {
            widget->tree_parent->child_ratio_sum[1] += widget->parameters[1].pref;
        }
        widget->tree_parent->child_parameters_sum[1].max = (widget->parameters[1].max > widget->tree_parent->child_parameters_sum[1].max) ? widget->parameters[1].max : widget->tree_parent->child_parameters_sum[1].max;
        
    } else if(ui_widget_has_property(widget->tree_parent, UI_Widget_Property_LayoutVertical)) {
        widget->tree_parent->child_parameters_sum[1].min += widget->parameters[1].min;
        if(!UI_PARAM_IS_RATIO(widget->parameters[1])) {
            widget->tree_parent->child_parameters_sum[1].pref += widget->parameters[1].pref;
        } else {
            widget->tree_parent->child_ratio_sum[1] += widget->parameters[1].pref;
        }
        widget->tree_parent->child_parameters_sum[1].max += widget->parameters[1].max;
        
        widget->tree_parent->child_parameters_sum[0].min = (widget->parameters[0].min > widget->tree_parent->child_parameters_sum[0].min) ? widget->parameters[0].min : widget->tree_parent->child_parameters_sum[0].min;
        
        if(!UI_PARAM_IS_RATIO(widget->parameters[0])) {
            widget->tree_parent->child_parameters_sum[0].pref = (widget->parameters[0].pref > widget->tree_parent->child_parameters_sum[0].pref) ? widget->parameters[0].pref : widget->tree_parent->child_parameters_sum[0].pref;
        } else {
            widget->tree_parent->child_ratio_sum[0] += widget->parameters[0].pref;
        }
        widget->tree_parent->child_parameters_sum[0].max = (widget->parameters[0].max > widget->tree_parent->child_parameters_sum[0].max) ? widget->parameters[0].max : widget->tree_parent->child_parameters_sum[0].max;
    }
    // TODO(Cian): Handle other special sizing requirements as needed
}

internal void ui_begin_row(char *string) {
    String8 widget_string = string_from_cstring(string);
    UI_Widget *row = ui_init_widget(widget_string, null);
    ui_push_parent(row);
    ui_widget_add_property(row, UI_Widget_Property_LayoutHorizontal);
}

internal void ui_end_row() {
    UI_Widget *row = ui->parent_stack.current;
    
    ui_pop_parent();
    //give child measurements to parent
    if(ui->parent_stack.current) {
        ui_measure_widget(row);
    }
}

internal void ui_begin_column(char *string) {
    String8 widget_string = string_from_cstring(string);
    UI_Widget *col = ui_init_widget(widget_string, null);
    ui_push_parent(col);
    ui_widget_add_property(col, UI_Widget_Property_LayoutVertical);
}

internal void ui_end_column() {
    UI_Widget *column = ui->parent_stack.current;
    
    ui_pop_parent();
    //give child measurements to parent
    if(ui->parent_stack.current) {
        ui_measure_widget(column);
    }
}

internal void ui_spacer(f32 min, f32 pref, f32 max) {
    String8 string = {};
    UI_Widget *spacer = ui_init_widget(string, null);
    if(!spacer)
        return;
    
    ui_widget_add_property(spacer, UI_Widget_Property_Spacer);
    // TODO(Cian): @UI dislike having to do layouting checks here so this is a temp fix
    if(ui_widget_has_property(spacer->tree_parent, UI_Widget_Property_LayoutHorizontal)) {
        spacer->parameters[0].min = min;
        spacer->parameters[0].pref = pref;
        spacer->parameters[0].max = max;
        
        // NOTE(Cian): Bit of a hack but this prevents weird measuring behaviour with fillers
        spacer->parameters[1].min = 1;
        spacer->parameters[1].pref = 1;
        spacer->parameters[1].max = 1;
    } else {
        spacer->parameters[1].min = min;
        spacer->parameters[1].pref = pref;
        spacer->parameters[1].max = max;
        
        // NOTE(Cian): Bit of a hack but this prevents weird measuring behaviour with fillers
        spacer->parameters[0].min = 1;
        spacer->parameters[0].pref = 1;
        spacer->parameters[0].max = 1;
    }
    
    if(ui->parent_stack.current) {
        ui_measure_widget(spacer);
    }
}

internal void ui_begin_split_pane(V4 layout, b32 split_vertical, f32 min_size_1, V2 *size_pos_1, f32 min_size_2, V2 *size_pos_2, u32 options, char *string...) {
    String8 split_string = {};
    MAKE_FORMAT_STRING(split_string, string);
    
    b32 newly_created = false;
    UI_Widget *split_pane = ui_init_widget(split_string, &newly_created);
    ui_widget_add_property(split_pane, UI_Widget_Property_RenderSplit);
    ui_widget_add_property(split_pane, UI_Widget_Property_InstantLayout);
    ui_push_parent(split_pane);
    
    split_pane->curr_layout = layout;
    V4 parent_layout = split_pane->window_parent->old_layout;
    split_pane->old_layout = {layout.x + parent_layout.x, layout.y + parent_layout.y, layout.width, layout.height};
    
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
        
        splitter_rect->x = split_pane->old_layout.x + size_pos_1->size + 8.0f;
        splitter_rect->y = split_pane->old_layout.y; // might add some padding to this
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
        
        splitter_rect->y = split_pane->old_layout.y + size_pos_1->size + 8.0f;
        splitter_rect->x = split_pane->old_layout.x; // might add some padding to this
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
        if(*is_open == false) {
            ui->active_window = null;
            return;
        }
    }
    
    String8 window_string = {};
    MAKE_FORMAT_STRING(window_string, title);
    
    b32 newly_created = false;
    UI_Widget *window = ui_init_widget(window_string, &newly_created);
    ui_widget_add_property(window, UI_Widget_Property_Container);
    ui_widget_add_property(window, UI_Widget_Property_RenderBackground);
    ui_widget_add_property(window, UI_Widget_Property_LayoutVertical);
    ui_widget_add_property(window, UI_Widget_Property_InstantLayout);
    
    if(~options & UI_ContainerOptions_NoTitle)
        ui_widget_add_property(window, UI_Widget_Property_RenderTitleBar);
    
    if(~options & UI_ContainerOptions_NoMove)
        ui_widget_add_property(window, UI_Widget_Property_Draggable);
    
    if(~options & UI_ContainerOptions_NoResize) {
        ui_widget_add_property(window, UI_Widget_Property_RenderBorderHot);
        ui_widget_add_property(window, UI_Widget_Property_Resizable);
    }
    
    if(is_open) {
        ui_widget_add_property(window, UI_Widget_Property_RenderCloseButton);
    }
    
    if(newly_created || (options & (UI_ContainerOptions_NoMove | UI_ContainerOptions_NoResize))) {
        // TODO(Cian): @UI @Window need to figure out how relative positioning and immediate input work, something like adding to parents old_layout or something
        
        window->curr_layout = layout;
        V4 parent_layout = window->window_parent->old_layout;
        window->old_layout = {layout.x + parent_layout.x, layout.y + parent_layout.y, layout.width, layout.height};
        
        
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
    
    //necessary mouse info
    V2 mouse_pos = os->mouse_pos;
    V2 mouse_delta = {};
    
    b32 mouse_move = os_sum_mouse_moves(&mouse_delta);
    
    OS_Event *mouse_down_event = null;
    OS_Event *mouse_up_event = null;
    b32 mouse_down = os_peek_mouse_button_event(&mouse_down_event, OS_Event_Type_MouseDown, OS_Mouse_Button_Left);
    b32 mouse_up = os_peek_mouse_button_event(&mouse_up_event, OS_Event_Type_MouseUp, OS_Mouse_Button_Left);
    
    //window dimensions and rect stuff
    f32 title_height = window->style->title_height;
    f32 border_thickness = window->style->border_thickness;
    
    V4 title_rect = v4(window->old_layout.x, window->old_layout.y, window->old_layout.width, title_height);
    f32 r = window->style->title_height / 5;
    V4 close_button_rect = v4(window->old_layout.x + window->old_layout.width - window->style->padding.x1 - (r / 2) - r, window->old_layout.y + (window->style->title_height / 2) - r, 2 * r, 2 * r);
    V4 border_left = v4(window->old_layout.x, window->old_layout.y - title_height, border_thickness,  window->old_layout.height - title_height);
    V4 border_right = v4(window->old_layout.x + ( window->old_layout.width - border_thickness), window->old_layout.y + title_height, border_thickness,  window->old_layout.height - title_height);
    V4 border_bottom = v4(window->old_layout.x, window->old_layout.y + ( window->old_layout.height - border_thickness), window->old_layout.width,  border_thickness);
    
    
    f32 vertical_min_scroll_offset = -CLAMP_MIN(window->child_parameters_sum[1].min - UI_AVAILABLE_HEIGHT(window), 0);
    
    V4 vertical_scroll_rect = {};
    
    vertical_scroll_rect.width =  UI_SEC_SCROLL_SIZE;
    vertical_scroll_rect.x = window->old_layout.x + window->old_layout.width - window->style->border_thickness - vertical_scroll_rect.width;
    
    f32 max_scrollbar_height = window->old_layout.height - (2*window->style->border_thickness) - window->style->title_height;
    vertical_scroll_rect.height = CLAMP_MIN(UI_AVAILABLE_HEIGHT(window) / window->child_parameters_sum[1].min * max_scrollbar_height, UI_MIN_SCROLL_MAIN); 
    
    f32 min_scroll_center_vertical = window->old_layout.y + window->old_layout.height - window->style->border_thickness - (vertical_scroll_rect.height / 2);
    
    f32 max_scroll_center_vertical = window->old_layout.y + window->style->title_height + window->style->border_thickness + (vertical_scroll_rect.height/2);
    
    f32 mapped_scroll_center_vertical = (window->scroll_offset_y - vertical_min_scroll_offset) * ((max_scroll_center_vertical -  min_scroll_center_vertical) /  (-vertical_min_scroll_offset)) + min_scroll_center_vertical;
    
    vertical_scroll_rect.y =  mapped_scroll_center_vertical - (vertical_scroll_rect.height / 2);
    
    f32 horizontal_min_scroll_offset = -CLAMP_MIN(window->child_parameters_sum[0].min - UI_AVAILABLE_WIDTH(window), 0);
    V4 horizontal_scroll_rect = {};
    
    horizontal_scroll_rect.height =  UI_SEC_SCROLL_SIZE;
    horizontal_scroll_rect.y = window->old_layout.y + window->old_layout.height - window->style->border_thickness - horizontal_scroll_rect.height;
    
    f32 max_scrollbar_width = window->old_layout.width - (2 * window->style->border_thickness);
    horizontal_scroll_rect.width = CLAMP_MIN(UI_AVAILABLE_WIDTH(window) / window->child_parameters_sum[0].min * max_scrollbar_width, UI_MIN_SCROLL_MAIN);
    
    f32 min_scroll_center_horizontal = window->old_layout.x + window->old_layout.width - window->style->border_thickness - (horizontal_scroll_rect.width / 2);
    
    f32 max_scroll_center_horizontal = window->old_layout.x + window->style->border_thickness + (horizontal_scroll_rect.width/2);
    
    if(ui_widget_has_property(window, UI_Widget_Property_ScrollVertical)) {
        max_scrollbar_width -= UI_SEC_SCROLL_SIZE;
        min_scroll_center_horizontal -= UI_SEC_SCROLL_SIZE;
        horizontal_scroll_rect.width = CLAMP_MIN(UI_AVAILABLE_WIDTH(window) / window->child_parameters_sum[0].min * max_scrollbar_width, UI_MIN_SCROLL_MAIN); 
    }
    
    f32 mapped_scroll_center_horizontal = (window->scroll_offset_x - horizontal_min_scroll_offset) * ((max_scroll_center_horizontal -  min_scroll_center_horizontal) /  (-horizontal_min_scroll_offset)) + min_scroll_center_horizontal;
    
    horizontal_scroll_rect.x = mapped_scroll_center_horizontal - (horizontal_scroll_rect.width / 2);
    
    //initial input info
    b32 mouse_is_over_title  = false;
    b32 mouse_is_over_right  = false;
    b32 mouse_is_over_left   = false;
    b32 mouse_is_over_bottom = false;
    b32 mouse_is_over_close  = false;
    b32 mouse_is_over_vertical_scroll = false;
    b32 mouse_is_over_horizontal_scroll = false;
    
    b32 dragging_title  = ui_widget_has_property(window, UI_Widget_Property_DraggingTitle);
    b32 dragging_left   = ui_widget_has_property(window, UI_Widget_Property_ResizeLeft);
    b32 dragging_right  = ui_widget_has_property(window, UI_Widget_Property_ResizeRight);
    b32 dragging_bottom = ui_widget_has_property(window, UI_Widget_Property_ResizeBottom);
    b32 dragging_vertical_scroll = false;
    b32 dragging_horizontal_scroll      = false;
    
    if(!(options & UI_ContainerOptions_NoMove))
        mouse_is_over_title = math_point_in_rect(title_rect, mouse_pos);
    if(!(options & UI_ContainerOptions_NoResize)) {
        mouse_is_over_right = math_point_in_rect(border_right, mouse_pos);
        mouse_is_over_left = math_point_in_rect(border_left, mouse_pos);
        mouse_is_over_bottom = math_point_in_rect(border_bottom, mouse_pos);
    }
    if(is_open) {
        mouse_is_over_close = math_point_in_rect(close_button_rect, mouse_pos);
    }
    
    //overflow scrolling data setup
    if(!newly_created){
        //check for overflow
        if(window->child_parameters_sum[0].min > UI_AVAILABLE_WIDTH(window)) {
            ui_widget_add_property(window, UI_Widget_Property_ScrollHorizontal);
            
            window->scroll_offset_x = CLAMP(window->scroll_offset_x, horizontal_min_scroll_offset,0);
            
            dragging_horizontal_scroll = ui_widget_has_property(window, UI_Widget_Property_DraggingHorizontalScroll);
            mouse_is_over_horizontal_scroll = math_point_in_rect(horizontal_scroll_rect, mouse_pos);
        } else {
            ui_widget_remove_property(window, UI_Widget_Property_ScrollHorizontal);
        }
        
        //another q, once scrolling do reset the widgets layout to full size? This sounds complex, but maybe not? Maybe just check for it in layout and alter accordingly
        if(window->child_parameters_sum[1].min > UI_AVAILABLE_HEIGHT(window)) {
            ui_widget_add_property(window, UI_Widget_Property_ScrollVertical);
            
            s32 mouse_scroll = 0;
            os_sum_mouse_scroll(&mouse_scroll);
            
            //quick test
            if(ui->clickable_window == window) {
                window->scroll_offset_y += mouse_scroll / 3;
            }
            
            window->scroll_offset_y = CLAMP(window->scroll_offset_y, vertical_min_scroll_offset,0);
            
            dragging_vertical_scroll = ui_widget_has_property(window, UI_Widget_Property_DraggingVerticalScroll);
            mouse_is_over_vertical_scroll = math_point_in_rect(vertical_scroll_rect, mouse_pos);
        } else {
            ui_widget_remove_property(window, UI_Widget_Property_ScrollVertical);
        }
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
            
            if(!(mouse_is_over_close || mouse_is_over_vertical_scroll || mouse_is_over_horizontal_scroll || mouse_is_over_title || mouse_is_over_left || mouse_is_over_right || mouse_is_over_bottom)) {
                ui->hot = ui_null_id();
            }
            
            if(is_open && mouse_is_over_close) {
                *is_open = false;
                window->last_frame--;
            }
            os_take_event(mouse_up_event);
        } else {
            if(mouse_move) {
                
                if(dragging_title) {
                    window->curr_layout.x += mouse_delta.x;
                    window->curr_layout.y += mouse_delta.y;
                } else if(dragging_left) {
                    f32 delta_x = mouse_delta.x;
                    if(delta_x > window->curr_layout.width) {
                        delta_x = window->curr_layout.width;
                    }
                    window->curr_layout.x += delta_x;
                    window->curr_layout.width -= delta_x;
                    
                } else if(dragging_right) {
                    if(window->curr_layout.x < (window->curr_layout.x + window->curr_layout.width + mouse_delta.x) || mouse_delta.x > 0) {
                        window->curr_layout.width += mouse_delta.x;
                    }
                } else if(dragging_bottom) {
                    if((window->curr_layout.y + title_height) < (window->curr_layout.y + window->curr_layout.height + mouse_delta.y) || mouse_delta.y > 0) {
                        window->curr_layout.height += mouse_delta.y;
                    }
                } else if(dragging_vertical_scroll) {
                    f32 max_delta = max_scroll_center_vertical - min_scroll_center_vertical;
                    f32 scroll_delta = (mouse_delta.y - max_delta) * ((-vertical_min_scroll_offset)/(-max_delta)) + vertical_min_scroll_offset;
                    window->scroll_offset_y = CLAMP(window->scroll_offset_y - scroll_delta, vertical_min_scroll_offset, 0);
                } else if(dragging_horizontal_scroll) {
                    f32 max_delta = max_scroll_center_horizontal - min_scroll_center_horizontal;
                    f32 scroll_delta = (mouse_delta.x - max_delta) * ((-horizontal_min_scroll_offset)/(-max_delta)) + horizontal_min_scroll_offset;
                    window->scroll_offset_x = CLAMP(window->scroll_offset_x - scroll_delta, horizontal_min_scroll_offset, 0);
                }
            }
        }
    } else if(ui_is_id_equal(ui->hot, window->id)) { 
        if(mouse_down) {
            //this only works if mouse is over window controls, to catch fall-through clicks end-window must do the same check
            ui_bring_to_front(window);
            ui->active = window->id;
            os_take_event(mouse_down_event);
        } else if(!(mouse_is_over_close || mouse_is_over_vertical_scroll || mouse_is_over_horizontal_scroll || mouse_is_over_title || mouse_is_over_left || mouse_is_over_right || mouse_is_over_bottom)) {
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
            } else if(mouse_is_over_close) {
                ui->hot = window->id;
            } else if(mouse_is_over_vertical_scroll) {
                ui->hot = window->id;
                ui_widget_add_property(window,  UI_Widget_Property_DraggingVerticalScroll);
            } else if(mouse_is_over_horizontal_scroll) {
                ui->hot = window->id;
                ui_widget_add_property(window, UI_Widget_Property_DraggingHorizontalScroll);
            }
        }
        
    }
    
    if(!(ui_is_id_equal(ui->hot, window->id) || ui_is_id_equal(ui->active, window->id))) {
        ui_widget_remove_property(window, UI_Widget_Property_DraggingTitle);
        ui_widget_remove_property(window,  UI_Widget_Property_ResizeLeft);
        ui_widget_remove_property(window,  UI_Widget_Property_ResizeRight);
        ui_widget_remove_property(window,  UI_Widget_Property_ResizeBottom);
        ui_widget_remove_property(window,  UI_Widget_Property_DraggingHorizontalScroll);
        ui_widget_remove_property(window,  UI_Widget_Property_DraggingVerticalScroll);
    }
    
    // TODO(Cian): @UI @Windows add some way of having a min/max width/height for windows when resizing, maybe use last frames info on the childrens size? Could be stored in the windows size_parameters, then we should also have some default min/max size
    
    ui_push_parent(window);
    ui_push_window(window);
    
}

internal void ui_end_window() {
    UI_Widget *window = ui->active_window;
    if(window) {
        V2 mouse_pos = os->mouse_pos;
        OS_Event *event = null;
        b32 mouse_down = os_peek_mouse_button_event(&event, OS_Event_Type_MouseDown, OS_Mouse_Button_Left);
        
        if(mouse_down && ui->clickable_window == window && ui_widget_has_property(window, UI_Widget_Property_Draggable)) {
            ui_bring_to_front(window);
            os_take_event(event);
        }
        
        ui_pop_parent();
        ui_pop_window();
    } else {
        ui->active_window = ui->window_stack.current;
    }
}


internal b32 ui_button(char *format...) {
    String8 string = {};
    MAKE_FORMAT_STRING(string, format);
    b32 newly_created = false;
    UI_Widget *button = ui_init_widget(string, &newly_created);
    if(!button)
        return false;
    button->style = &widget_style_table[UI_Widget_Style_DefaultButton];
    ui_widget_add_property(button, UI_Widget_Property_RenderBackground);
    ui_widget_add_property(button, UI_Widget_Property_RenderActive);
    ui_widget_add_property(button, UI_Widget_Property_RenderHot);
    ui_widget_add_property(button, UI_Widget_Property_RenderText);
    ui_widget_add_property(button, UI_Widget_Property_Clickable);
    
    if(!newly_created) {
        //do input only when we have layout from previous frame
    }
    
    if(ui->parent_stack.current) {
        ui_measure_widget(button);
    }
    
    return false; //temporary until we get our input testing function done
}

internal b32 ui_button(UI_Widget_Style *style, char *title...) {
    
}

internal void ui_test_box(NVGcolor color, char *format,...) {
    String8 string = {};
    MAKE_FORMAT_STRING(string, format);
    UI_Widget *test_box = ui_init_widget(string, null);
    // TODO(Cian): @UI create a ui style table so that new colors etc can easily be added
    //test_box->color = color;
    ui_widget_add_property(test_box, UI_Widget_Property_RenderBackground);
}


