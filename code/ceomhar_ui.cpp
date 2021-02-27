#pragma warning(push)
#pragma warning(disable: 4505)

// TODO(Cian): @UI @Important Having some perf issues since latest UI changes, not sure why, after demo implement some basic profiling and will probably have to rewrite the UI system with those insights in mind

namespace UI{
    internal f32 pixels_to_dp(float pixels) {
        return F32_ROUND(pixels / (global_os->display.dpi / UI_DEFAULT_DENSITY));
    }
    
    internal f32 dp_to_pixels(float dp) {
        return F32_ROUND (dp  * (global_os->display.dpi / UI_DEFAULT_DENSITY));
    }
    
    internal void widget_add_property(Widget *widget, Widget_Property property) {
        u32 idx = property / 64;
        widget->properties[idx] |= ((u64)1 << (property % 64));
    }
    
    internal b32 widget_has_property(Widget *widget, Widget_Property property) {
        u32 idx = property / 64;
        // NOTE(Cian): Can think of !! as a clamp between {0,1}, it's just 2 NOTS together, not really required here but kinda cool
        return !!((u64)widget->properties[idx] & ((u64)1 <<  (property % 64)));
    }
    
    internal void widget_remove_property(Widget *widget, Widget_Property property) {
        u32 idx = property / 64;
        // NOTE(Cian): ~ is the complement of a series of bits, e.g. flips the bits
        widget->properties[idx] &= (~((u64)1 << (property % 64)));
    }
    
    internal b32 widgets_compare_property(Widget *widget_1, Widget *widget_2, Widget_Property property) {
        b32 result = false;
        
        if(widget_has_property(widget_1, property) && widget_has_property(widget_2, property)) {
            result = true;
        }
        
        return result;
    }
    
    internal b32 widgets_share_one_property(Widget *widget_1, Widget *widget_2, Widget_Property property_1, Widget_Property property_2) {
        return widgets_compare_property(widget_1, widget_2, property_1) || widgets_compare_property(widget_1, widget_2, property_2);
    }
    
    internal void push_parent(Widget *parent) {
        u32 size = ui_state->parent_stack.size;
        if(size > 0)
            ui_state->parent_stack.stack[size - 1] = ui_state->parent_stack.current;
        ui_state->parent_stack.current = parent;
        ui_state->parent_stack.size++;
    }
    
    internal void pop_parent() {
        //when a parent ends, it needs to be the previous widget, not its last child, so its siblings can correctly reference it
        ui_state->prev_widget = ui_state->parent_stack.current;
        ui_state->parent_stack.size--;
        u32 size = ui_state->parent_stack.size;
        ui_state->parent_stack.current = ui_state->parent_stack.stack[size - 1];
    }
    
    internal void push_width_ratio(f32 ratio, f32 strictness) {
        u32 *size = &ui_state->width_stack.size;
        ui_state->width_stack.stack[*size] = ui_state->width_stack.current;
        (*size)++;
        
        ui_state->width_stack.current.is_ratio = true;
        ui_state->width_stack.current.ratio = ratio;
        ui_state->width_stack.current.strictness = strictness;
    }
    
    internal void push_width(f32 size, f32 strictness) {
        u32 *s_size = &ui_state->width_stack.size;
        ui_state->width_stack.stack[*s_size] = ui_state->width_stack.current;
        (*s_size)++;
        ui_state->width_stack.current.is_ratio = false;
        ui_state->width_stack.current.size = size;
        ui_state->width_stack.current.strictness = strictness;
    }
    
    internal void pop_width() {
        u32 *size = &ui_state->width_stack.size;
        (*size)--;
        ui_state->width_stack.current = ui_state->width_stack.stack[*size];
    }
    
    internal void push_padding(V4 padding) {
        u32 *s_size = &ui_state->padding_stack.size;
        ui_state->padding_stack.stack[*s_size] = ui_state->padding_stack.current;
        (*s_size)++;
        
        ui_state->padding_stack.current = padding;
    }
    
    internal void pop_padding() {
        u32 *size = &ui_state->padding_stack.size;
        (*size)--;
        ui_state->padding_stack.current = ui_state->padding_stack.stack[*size];
    }
    
    internal void push_height_ratio(f32 ratio, f32 strictness) {
        u32 *size = &ui_state->height_stack.size;
        ui_state->height_stack.stack[*size] = ui_state->height_stack.current;
        (*size)++;
        
        ui_state->height_stack.current.is_ratio = true;
        ui_state->height_stack.current.ratio = ratio;
        ui_state->height_stack.current.strictness = strictness;
    }
    
    internal void push_height(f32 size, f32 strictness) {
        u32 *s_size = &ui_state->height_stack.size;
        ui_state->height_stack.stack[*s_size] = ui_state->height_stack.current;
        (*s_size)++;
        ui_state->height_stack.current.is_ratio = false;
        ui_state->height_stack.current.size = size;
        ui_state->height_stack.current.strictness = strictness;
    }
    
    internal void pop_height() {
        u32 *size = &ui_state->height_stack.size;
        (*size)--;
        ui_state->height_stack.current = ui_state->height_stack.stack[*size];
    }
    
    internal Widget* init_widget(String8 string) {
        Widget *widget = null;
        
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
                hash_string.data = (char*)Memory::arena_push(&global_os->frame_arena, hash_string.size);
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
                hash_string.data = (char*)Memory::arena_push(&global_os->frame_arena, hash_string.size);
                
                stbsp_snprintf(hash_string.data, hash_string.size, string.data + src_offset);
                
                //Remove appended piece from main string;
                string.data[first_pound_loc] = null;
                string.size = first_pound_loc + 1;
            } else {
                hash_string = string;
            }
            u32 seed = 0;
            
            // TODO(Cian): Was being stupid here and forgot that the positions of parents in table isn't guaranteed, need a better way of seeding the hash 
            if(ui_state->parent_stack.current) 
                seed = ui_state->parent_stack.current->id.hash;
            
            u32 hash = String::string_to_crc32(hash_string.data, seed);
            u32 idx = hash % UI_MAX_WIDGETS;
            
            widget = ui_state->widgets[idx];
            if(widget == null) {
                widget = (Widget*)Memory::arena_push(&global_os->permanent_arena, sizeof(Widget));
                ui_state->widgets[idx] = widget;
            } else {
                
                Widget *previous = null;
                while(widget) {
                    if(widget->id.hash == (s32)hash)
                        break;
                    previous = widget;
                    widget = widget->hash_next;
                }
                
                if(widget == null) {
                    widget = (Widget*)Memory::arena_push(&global_os->permanent_arena, sizeof(Widget));
                }
                if(previous) 
                    previous->hash_next = widget;
            }
            widget->id.hash = hash;
            widget->id.table_pos = idx;
        } else {
            u32 idx = 0;
            
            u64 valid_frame = (ui_state->curr_frame == 0) ? 0 : ui_state->curr_frame - 1;
            
            for(idx; idx < UI_MAX_WIDGETS; idx++) {
                widget = ui_state->widgets[idx];
                
                if(widget == null || widget->last_frame < valid_frame || (widget->id.hash == UI_NON_INTERACTABLE_ID && widget->last_frame < valid_frame)) {
                    break;
                }
            }
            
            if(widget == null) {
                widget = (Widget*)Memory::arena_push(&global_os->permanent_arena, sizeof(Widget));
                ui_state->widgets[idx] = widget;
                ui_state->non_interactive_count++;
            }
            Widget *hash_next = widget->hash_next;
            *widget = {};
            widget->hash_next = hash_next;
            
            widget->id.hash = UI_NON_INTERACTABLE_ID;
            widget->id.table_pos = idx;
            
            string = String::string_from_cstring("non_interactable");
        }
        widget->string = string;
        
        widget->old_layout = widget->curr_layout;
        widget->curr_layout = {0};
        widget->last_frame = ui_state->curr_frame;
        
        widget->parameters[0] = ui_state->width_stack.current;
        widget->parameters[1] = ui_state->height_stack.current;
        widget->padding = ui_state->padding_stack.current;
        
        if(ui_state->parent_stack.current) {
            widget->tree_parent = ui_state->parent_stack.current;
            if(ui_state->prev_widget == widget->tree_parent) {
                ui_state->prev_widget->tree_first_child = widget;
            } else if(ui_state->prev_widget->tree_parent == widget->tree_parent){
                ui_state->prev_widget->tree_next_sibling = widget;
                widget->tree_prev_sibling = ui_state->prev_widget;
            }
            
            widget->tree_parent->tree_last_child = widget;
        }
        
        ui_state->prev_widget = widget;
        ui_state->widget_size++;
        
        return widget;
    }
    
    internal void begin() {
        // NOTE(Cian): Create the main window and add as parent
        // TODO(Cian):  @UI Make a Widget init function once we have more knowledge about different use cases
        f32 display_width = (f32)global_os->display.width;
        f32 display_height = (f32)global_os->display.height;
        
        push_width(display_width, 1.0f);
        push_height(display_height, 1.0f);
        // NOTE(Cian): Some non-interactable widgets will still be explicitly named for debugging purposes
        // TODO(Cian): @UI might reqork this so it only does this in debug mode?
        String8 main_row_string = String::string_from_cstring("main");
        Widget *main_row_container = init_widget(main_row_string);
        pop_width();
        pop_height();
        
        // TODO(Cian): @UI Push size auto for both dimensions, e.g. by default widgets placed directly in the main_container are size auto
        
        ui_state->root_widget = main_row_container;
        push_parent(main_row_container);
        widget_add_property(main_row_container, Widget_Property_Container);
        widget_add_property(main_row_container, Widget_Property_LayoutHorizontal);
        
    }
    
    internal void begin_row(char *string) {
        String8 widget_string = String::string_from_cstring(string);
        Widget *row = init_widget(widget_string);
        push_parent(row);
        widget_add_property(row, Widget_Property_Container);
        widget_add_property(row, Widget_Property_LayoutHorizontal);
    }
    
    internal void end_row() {
        pop_parent();
    }
    
    internal void begin_column(char *string) {
        String8 widget_string = String::string_from_cstring(string);
        Widget *col = init_widget(widget_string);
        push_parent(col);
        widget_add_property(col, Widget_Property_Container);
        widget_add_property(col, Widget_Property_LayoutVertical);
    }
    
    internal void end_column() {
        pop_parent();
    }
    
    internal void begin_panel(NVGcolor color, char *format,...) {
        String8 string = {};
        MAKE_FORMAT_STRING(string, format);
        Widget *panel_container = init_widget(string);
        widget_add_property(panel_container, Widget_Property_Container);
        widget_add_property(panel_container, Widget_Property_RenderBackground);
        panel_container->color = color;
        push_parent(panel_container);
        // NOTE(Cian): The row inside of the panel shouldn't receive any of the panels padding or sizes, maybe should change this
        WIDTH_AUTO HEIGHT_AUTO PADDING2(v2(0,0))
            begin_row(0);
    }
    
    internal void end_panel() {
        end_row();
        pop_parent();
    }
    
    internal void test_box(NVGcolor color, char *format,...) {
        String8 string = {};
        MAKE_FORMAT_STRING(string, format);
        Widget *test_box = init_widget(string);
        test_box->color = color;
        widget_add_property(test_box, Widget_Property_RenderBackground);
    }
    
    internal void spacer(f32 size, f32 strictness) {
        String8 string = {};
        Widget *spacer = init_widget(string);
        // TODO(Cian): @UI dislike having to do layouting checks here so this is a temp fix
        if(widget_has_property(spacer->tree_parent, Widget_Property_LayoutHorizontal)) {
            spacer->parameters[UI_ParameterIndex_Width].size = size;
            spacer->parameters[UI_ParameterIndex_Width].strictness = strictness;
            spacer->parameters[UI_ParameterIndex_Width].is_ratio = false;
        } else {
            spacer->parameters[UI_ParameterIndex_Height].size = size;
            spacer->parameters[UI_ParameterIndex_Height].strictness = strictness;
            spacer->parameters[UI_ParameterIndex_Height].is_ratio = false;
        }
    }
    
    internal void button_helper(Widget *button, NVGcolor color, NVGcolor text_color, f32 font_size) {
        // TODO(Cian): @UI add parameters for different functionality as needed
        button->color = color;
        button->text_color = text_color;
        button->font_size = font_size;
        widget_add_property(button, Widget_Property_RenderBackgroundRounded);
        widget_add_property(button, Widget_Property_Clickable);
        widget_add_property(button, Widget_Property_RenderText);
        widget_add_property(button, Widget_Property_RenderHot);
        widget_add_property(button, Widget_Property_RenderActive);
        widget_add_property(button, Widget_Property_RenderBorder);
    }
    
    internal b32 button(char *format, ...) {
        String8 string = {};
        MAKE_FORMAT_STRING(string, format);
        Widget *button = init_widget(string);
        button_helper(button, SECONDARY_COLOR, DEFAULT_TEXT_COLOR, UI_DEFAULT_FONT_SIZE);
        
        // NOTE(Cian): Input Experiments, will be pulled out later
        b32 result = false;
        if(ui_state->curr_frame == 0) {
            return result;
        } else {
            OS_Event *mouse_down_event = null;
            OS_Event *mouse_up_event = null;
            b32 mouse_down = OS::peek_mouse_button_event(&mouse_down_event, OS_EventType_MouseDown, OS_MouseButton_Left);
            b32 mouse_up = OS::peek_mouse_button_event(&mouse_up_event, OS_EventType_MouseUp, OS_MouseButton_Left);
            // TODO(Cian): Maybe uuid comparison functions
            // this is the active widget
            
            V2 mouse_pos = global_os->mouse_pos;
            V4 button_rect = button->old_layout;
            
            b32 mouse_is_over = mouse_pos.x > button_rect.x &&
                mouse_pos.x < button_rect.x + button_rect.width &&
                mouse_pos.y > button_rect.y &&
                mouse_pos.y < button_rect.y + button_rect.height;
            
            if(ui_state->active.hash == button->id.hash) {
                if(mouse_up) {
                    
                    result = mouse_is_over;
                    ui_state->active = UI_NullID;
                    
                    if(!mouse_is_over){
                        ui_state->hot = UI_NullID;
                    }
                    OS::take_event(mouse_up_event);
                }
            } else if(ui_state->hot.hash == button->id.hash) {
                if(mouse_down) {
                    ui_state->active = button->id;
                    OS::take_event(mouse_down_event);
                } else if(!mouse_is_over) {
                    ui_state->hot = UI_NullID;
                }
            } else {
                if(mouse_is_over && !mouse_down && (ui_state->hot.hash == 0))
                    ui_state->hot = button->id;
            }
            
        }
        
        return result;
    }
    
    internal b32 button(NVGcolor background_color, char *format, ...) {
        String8 string = {};
        MAKE_FORMAT_STRING(string, format);
        Widget *button = init_widget(string);
        button_helper(button, background_color, DEFAULT_TEXT_COLOR, UI_DEFAULT_FONT_SIZE);
        
        return true;
    }
    
    internal b32 button(NVGcolor background_color, NVGcolor text_color, char *format, ...) {
        String8 string = {};
        MAKE_FORMAT_STRING(string, format);
        Widget *button = init_widget(string);
        button_helper(button, background_color, text_color, UI_DEFAULT_FONT_SIZE);
        return true;
    }
    
    internal void label(char *format, ...) {
        String8 string = {};
        MAKE_FORMAT_STRING(string, format);
        Widget *label = init_widget(string);
        widget_add_property(label, Widget_Property_RenderText);
        label->text_color = DEFAULT_TEXT_COLOR;
        label->font_size = UI_DEFAULT_FONT_SIZE;
    }
    
    internal void label(f32 font_size, char *format, ...) {
        String8 string = {};
        MAKE_FORMAT_STRING(string, format);
        Widget *label = init_widget(string);
        widget_add_property(label, Widget_Property_RenderText);
        label->text_color = DEFAULT_TEXT_COLOR;
        label->font_size = font_size;
    }
    
    internal inline f32 get_size(Widget *widget, u32 axis) {
        f32 result = 0;
        
        if(widget->parameters[axis].is_ratio) {
            f32 parent_size = widget->tree_parent->curr_layout.elements[axis + 2] - (widget->tree_parent->padding.elements[axis] + widget->tree_parent->padding.elements[axis + 2]);
            result = widget->parameters[axis].ratio * parent_size;
        }else {
            result = widget->parameters[axis].size; 
        }
        
        return result;
    }
    
    internal inline b32 is_auto_size(Widget *widget, u32 axis) {
        // TODO(Cian): @UI Maybe a better way of deciding this
        if(widget->parameters[axis].size == 0 && widget->parameters[axis].strictness == 0) {
            return true;
        }
        
        return false;
    }
    
    internal void measure_widget(Widget *widget, b32 is_tiled, u32 axis) {
        if(widget_has_property(widget, Widget_Property_Container)){
            f32 size = 0;
            f32 strictness = 0;
            
            Widget *curr = widget->tree_first_child;
            if(is_tiled) {
                f32 min_sum = 0;
                f32 sum = 0;
                
                while(curr) {
                    // NOTE(Cian): Child widget may not share same layout direction
                    if(is_auto_size(curr, axis)) {
                        if(widgets_share_one_property(widget, curr, Widget_Property_LayoutHorizontal, Widget_Property_LayoutVertical)){
                            measure_widget(curr, is_tiled, axis);
                        } else {
                            if(widget_has_property(widget, Widget_Property_Container)) {
                                measure_widget(curr, is_tiled, axis);
                            } else {
                                measure_widget(curr, !is_tiled, axis);
                            }
                        }
                    }
                    
                    sum += curr->parameters[axis].size;
                    min_sum += curr->parameters[axis].size * curr->parameters[axis].strictness;
                    
                    curr = curr->tree_next_sibling;
                }
                size = sum +  (widget->padding.elements[axis] + widget->padding.elements[axis + 2]);
                // TODO(Cian): @UI was having some issues with auto-sized strictness, keep in mind for rewrite
                strictness =  (min_sum +  (widget->padding.elements[axis] + widget->padding.elements[axis + 2])) / size;
            } else {
                f32 max_pref_size = 0;
                f32 max_min_size = 0;
                
                while(curr) {
                    // TODO(Cian): @UI Could probably simplify this, may as well check in an if this info, ok for now
                    // NOTE(Cian): Child widget may not share same layout direction
                    if(is_auto_size(curr, axis)) {
                        if(widgets_share_one_property(widget, curr, Widget_Property_LayoutHorizontal, Widget_Property_LayoutVertical)){
                            measure_widget(curr, is_tiled, axis);
                        } else {
                            if(widget_has_property(widget, Widget_Property_Container)) {
                                measure_widget(curr, is_tiled, axis);
                            } else {
                                measure_widget(curr, !is_tiled, axis);
                            }
                        }
                    }
                    
                    f32 pref_size = curr->parameters[axis].size;
                    f32 min_size = pref_size * curr->parameters[axis].strictness;
                    
                    if(pref_size > max_pref_size)
                        max_pref_size = pref_size;
                    if(min_size > max_min_size)
                        max_min_size = min_size;
                    
                    curr = curr->tree_next_sibling;
                }
                size = max_pref_size +  (widget->padding.elements[axis] + widget->padding.elements[axis + 2]);
                strictness = (max_min_size + (widget->padding.elements[axis] + widget->padding.elements[axis + 2])) / size;
                // TODO(Cian): @UI was having some issues with auto-sized strictness, keep in mind for rewrite
                
            }
            // TODO(Cian): @UI padding stuff is a bit crude here
            widget->parameters[axis].size = size;
            widget->parameters[axis].strictness = strictness;
        } else {
            // TODO(Cian): @UI Handle things like widgets that have text etc, if a widget doesn't fall into any category here just set some default size
            nvgSave(global_vg);
            if(widget_has_property(widget, Widget_Property_RenderText)) {
                nvgTextAlign(global_vg, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
                nvgFontFace(global_vg, "roboto-medium");
                nvgFontSize(global_vg, widget->font_size);
                f32 bounds[4];
                f32 width = nvgTextBounds(global_vg, 0, 0, widget->string.data, null, bounds);
                f32 height = bounds[3] - bounds[1];
                
                if(axis == UI_ParameterIndex_Width) {
                    f32 padding_x0 = UI_DEFAULT_TEXT_PADDING_X;
                    f32 padding_x1 = UI_DEFAULT_TEXT_PADDING_X;
                    
                    if(widget->padding.x0 != 0 || widget->padding.x1 != 0) {
                        padding_x0 = widget->padding.x0;
                        padding_x1 = widget->padding.x1;
                    }
                    
                    // TODO(Cian): add padding and appropriate strictness so that there is always enough room for text
                    widget->parameters[axis].size = width + (padding_x0 + padding_x1);
                    // TODO(Cian): @UI setting text strictness to 1 to solve some headaches right now, will fix in rewrite
                    //widget->parameters[axis].strictness = (width + (2 * UI_MIN_TEXT_PADDING_X)) / widget->parameters[axis].size;
                    widget->parameters[axis].strictness = 1.0f;
                } else {
                    f32 padding_y0 = UI_DEFAULT_TEXT_PADDING_Y;
                    f32 padding_y1 = UI_DEFAULT_TEXT_PADDING_Y;
                    
                    if(widget->padding.y0 != 0 || widget->padding.y1 != 0) {
                        padding_y0 = widget->padding.y0;
                        padding_y1 = widget->padding.y1;
                    }
                    
                    widget->parameters[axis].size = height + (padding_y0 + padding_y1);
                    //widget->parameters[axis].strictness = (height + (2 * UI_MIN_TEXT_PADDING_Y)) / widget->parameters[axis].size;
                    widget->parameters[axis].strictness = 1.0f;
                }
                
            } else {
                // TODO(Cian): @UI can probably remove this or only enable it in certain debug situations?
                if(axis == UI_ParameterIndex_Width){
                    widget->parameters[axis].size = 10;
                    widget->parameters[axis].strictness = 1.0f;
                } else {
                    widget->parameters[axis].size = 10;
                    widget->parameters[axis].strictness = 1.0f;
                }
            }
            nvgRestore(global_vg);
            
        }
    }
    // TODO(Cian): @UI Both of these layout functions loop through the children, maybe these loops could occur at the same time? Doubt it really matters for performance whatsoever but it's kinda bothering my OCD
    // NOTE(Cian): Lays out the widgets in the direction of the Container e.g. if a row it will lay out/re-size horizontally
    internal void layout_in_flow(Widget *first_child, f32 available, f32 initial_offset, u32 axis) {
        f32 sum_delta = 0;
        f32 sum = 0;
        
        f32 offset = initial_offset;
        Widget *curr = first_child;
        while(curr) {
            // TODO(Cian): @UI This is pretty piggy, if both sizes are auto we will measure children 2 times for each axis
            if(is_auto_size(curr, axis)) {
                // TODO(Cian): @UI Temporary fix for layouting, simpler rewrite needed later
                if((axis == UI_ParameterIndex_Width && widget_has_property(curr, Widget_Property_LayoutHorizontal))
                   || (axis == UI_ParameterIndex_Height && widget_has_property(curr, Widget_Property_LayoutVertical))) {
                    measure_widget(curr, true, axis);
                } else {
                    measure_widget(curr, false, axis);
                }
            }
            
            f32 pref_size = get_size(curr, axis);
            
            sum_delta +=  (pref_size * (1 - curr->parameters[axis].strictness));
            sum += pref_size;
            
            // NOTE(Cian): Attempt to layout with the preferred size
            curr->curr_layout.elements[axis] = offset;
            curr->curr_layout.elements[axis + 2] = pref_size;
            offset += curr->curr_layout.elements[axis + 2];
            
            curr = curr->tree_next_sibling;
        }
        
        offset = initial_offset;
        if(sum > available && sum_delta != 0) {
            
            curr = first_child;
            while(curr) {
                
                f32 pref_size = get_size(curr, axis);
                f32 factor = (pref_size * (1 - curr->parameters[axis].strictness)) / sum_delta; 
                
                curr->curr_layout.elements[axis] = offset;
                curr->curr_layout.elements[axis + 2] -= (factor * (sum - available)); 
                offset += curr->curr_layout.elements[axis + 2];
                
                curr = curr->tree_next_sibling;
            }
        } 
    }
    
    // NOTE(Cian): Lays out the widgets in the non-flow axis e.g. if a row it will do layout/re-sizing on the height, simply "centers" the widget on this axis
    internal void layout_non_flow(Widget *first_child, f32 available, f32 initial_offset, u32 axis) {
        Widget *curr = first_child;
        while(curr) {
            if(is_auto_size(curr, axis)) {
                // TODO(Cian): @UI Temporary fix for layouting, simpler rewrite needed later
                if((axis == UI_ParameterIndex_Width && widget_has_property(curr, Widget_Property_LayoutHorizontal))
                   || (axis == UI_ParameterIndex_Height && widget_has_property(curr, Widget_Property_LayoutVertical))) {
                    measure_widget(curr, true, axis);
                } else {
                    measure_widget(curr, false, axis);
                }
            }
            // TODO(Cian): @UI Maybe do fancy alignment stuff?
            
            f32 size = 0;
            
            f32 pref_size = get_size(curr, axis);
            f32 delta_size = pref_size * (1 - curr->parameters[axis].strictness);
            
            f32 factor = (pref_size - available) / delta_size;
            if(pref_size > available && factor <= 1) {
                size = pref_size - (factor * delta_size);
            } else {
                size = pref_size;
            }
            // NOTE(Cian): Center the widget 
            // TODO(Cian): @UI Might wanna use ABS here, also might wanna round but whatever
            f32 pos_offset = (available - size) / 2;
            
            // TODO(Cian): @UI need to sort out alignment later
            //The offset...
            curr->curr_layout.elements[axis] = initial_offset;
            //The size...
            curr->curr_layout.elements[axis + 2] = size;
            
            curr = curr->tree_next_sibling;
        }
    }
    
    // NOTE(Cian): 
    internal void do_layout(Widget *root) {
        // TODO(Cian): @UI Take padding into account here;
        f32 parent_width = root->curr_layout.width - (root->padding.x1 + root->padding.x0);
        f32 parent_height = root->curr_layout.height - (root->padding.x1 + root->padding.x0);
        
        f32 initial_offset_x = root->curr_layout.x + root->padding.x0;
        f32 initial_offset_y = root->curr_layout.y + root->padding.y0;
        
        if(widget_has_property(root, Widget_Property_RenderBorder)) {
            parent_width -= 2 * UI_BORDER_SIZE;
            parent_height -= 2 * UI_BORDER_SIZE;
            
            initial_offset_x += UI_BORDER_SIZE;
            initial_offset_y += UI_BORDER_SIZE;
        }
        
        if(widget_has_property(root, Widget_Property_LayoutHorizontal)) {
            layout_in_flow(root->tree_first_child, parent_width, initial_offset_x, 0);
            layout_non_flow(root->tree_first_child, parent_height, initial_offset_y,1);
        } else if (widget_has_property(root, Widget_Property_LayoutVertical)) {
            layout_in_flow(root->tree_first_child, parent_height, initial_offset_y,1);
            layout_non_flow(root->tree_first_child, parent_width, initial_offset_x,0);
        } else {
            layout_non_flow(root->tree_first_child, parent_width, initial_offset_x,0);
            layout_non_flow(root->tree_first_child, parent_height, initial_offset_y,1);
        }
        
        // TODO(Cian): @UI Lil annoying that I have a third loop here, need to try and clean this up once I get things working
        Widget *curr = root->tree_first_child;
        while(curr) {
            if(widget_has_property(curr, Widget_Property_Container)) {
                do_layout(curr);
            }
            curr = curr->tree_next_sibling;
        }
        
    }
    
    internal void render(Widget *root) {
        Widget *curr = root->tree_first_child;
        
        while(curr) {
            f32 x = curr->curr_layout.x;
            f32 y = curr->curr_layout.y;
            
            // TODO(Cian): @UI Very temporary very hacky, just to get things working, should use the transition effects to overlay a growing transparent rectangle that darkens/lightens the active/hot element(this wiill allow it to work with any color) 
            if(widget_has_property(curr, Widget_Property_RenderHot)) {
                if(curr->id.hash == ui_state->hot.hash)
                    curr->color = SECONDARY_COLOR_LIGHT;
            }
            
            if(widget_has_property(curr, Widget_Property_RenderActive)) {
                if(curr->id.hash == ui_state->active.hash)
                    curr->color = SECONDARY_COLOR_DARK;
            }
            
            if(widget_has_property(curr, Widget_Property_RenderBackground)){
                
                nvgBeginPath(global_vg);
                nvgRect(global_vg, x, y, curr->curr_layout.width,  curr->curr_layout.height);
                nvgFillColor(global_vg, curr->color);
                nvgFill(global_vg);
            } else if(widget_has_property(curr, Widget_Property_RenderBackgroundRounded)) {
                
                nvgBeginPath(global_vg);
                nvgRoundedRect(global_vg, x, y, curr->curr_layout.width,  curr->curr_layout.height, DEFAULT_ROUNDNESS);
                nvgFillColor(global_vg, curr->color);
                nvgFill(global_vg);
            }
            
            if(widget_has_property(curr, Widget_Property_RenderText)) {
                nvgTextAlign(global_vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
                nvgFontFace(global_vg, "roboto-medium");
                nvgFontSize(global_vg, curr->font_size);
                nvgFillColor(global_vg, curr->text_color);
                nvgText(global_vg, x + (curr->curr_layout.width / 2), y + (curr->curr_layout.height / 2), curr->string.data, null);
            }
            
            if(widget_has_property(curr, Widget_Property_Container)) {
                
                render(curr);
            }
            curr = curr->tree_next_sibling;
        }
    }
    
    internal void end() {
        // TODO(Cian): @UI Need to either clear autolayout state here or, make the State as part of the frame_arena and Widgets be maintained seperately in permanent arena
        ui_state->parent_stack.size = 0;
        ui_state->width_stack.size = 0;
        ui_state->height_stack.size = 0;
        
        ui_state->prev_widget = null;
        ui_state->parent_stack.current = null;
        ui_state->curr_frame++;
        //Autolayout for rendering and next frame goes here
        /* NOTE(Cian): Auto-layout works as follows:
        *  Traverse downwards through the tree, for every parent, measure it's children, this may
    *  need to be recursive, we add childrens 
    */
        Widget *root = ui_state->root_widget;
        // NOTE(Cian): Roots incoming constraints will always be tight so just set it's layout to be that
        root->curr_layout = {0, 0, root->parameters[0].size, root->parameters[1].size};
        do_layout(root);
        render(ui_state->root_widget);
    }
}