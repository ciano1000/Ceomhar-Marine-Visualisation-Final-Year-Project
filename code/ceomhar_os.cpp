internal void os_push_event(OS_Event event) {
    if(os->event_count < OS_MAX_EVENTS) {
        os->events[os->event_count++] = event;
    }
}

internal void os_flush_events() {
    for(u32 i = 0; i < os->event_count; i++) {
        os->events[i] = {};
    }
    
    os->event_count = 0;
}

internal OS_Event os_mouse_move_event(V2 position, V2 delta_pos) {
    OS_Event mouse_event = {};
    
    mouse_event.type = OS_Event_Type_MouseMove;
    mouse_event.pos = position;
    mouse_event.delta = delta_pos;
    
    return mouse_event;
}

internal OS_Event os_mouse_button_down_event(OS_Mouse_Button button, V2 position) {
    OS_Event mouse_event = {};
    
    mouse_event.type = OS_Event_Type_MouseDown;
    mouse_event.pos = position;
    mouse_event.mouse_button = button;
    
    return mouse_event;
}

internal OS_Event os_mouse_button_up_event(OS_Mouse_Button button, V2 position) {
    OS_Event mouse_event = {};
    
    mouse_event.type = OS_Event_Type_MouseUp;
    mouse_event.pos = position;
    mouse_event.mouse_button = button;
    
    return mouse_event;
}

// TODO(Cian): @OS @Input add scroll modifiers here, e.g. for zooming when CTRL is held
internal OS_Event os_mouse_scroll_event(f32 scroll_delta) {
    OS_Event scroll_event = {};
    scroll_event.scroll_delta = scroll_delta;
    
    return scroll_event;
}


#ifdef CEOMHAR_APP
internal b32 os_get_next_event(OS_Event **event) {
    b32 result = false;
    
    u32 start_idx = 0;
    
    OS_Event *new_e = null;
    
    if((*event)) {
        start_idx = (u32)(*event - os->events) + 1;
    }  
    for(u32 i = start_idx; i < OS_MAX_EVENTS; i++) {
        if(os->events[i].type != OS_Event_Type_Null) {
            new_e = os->events + i;
            break;
        }
    }
    
    *event = new_e;
    result = *event != null;
    return result;
}

internal void os_take_event(OS_Event *event) {
    event->type = OS_Event_Type_Null;
}

// NOTE(Cian): Finds a mouse move event but doesn't take it off the queue just yet, used for dragging
internal b32 os_peek_mouse_move(OS_Event **event) {
    // TODO(Cian):  not sure would there be more than one mouse event per frame? Might be possible
    b32 result = false;
    for(OS_Event *new_event = 0; os_get_next_event(&new_event);) {
        if(new_event->type == OS_Event_Type_MouseMove) {
            *event = new_event;
            result = true;
        }
    }
    
    return result;
}

internal b32 os_sum_mouse_moves(V2 *delta_sum) {
    b32 result = false;
    for(OS_Event *new_event = 0; os_get_next_event(&new_event);) {
        if(new_event->type == OS_Event_Type_MouseMove) {
            result = true;
            
            delta_sum->x += new_event->delta.x;
            delta_sum->y += new_event->delta.y;
        }
    }
    
    return true;
}

internal b32 os_peek_mouse_button_event(OS_Event **event, OS_Event_Type type, OS_Mouse_Button button) {
    b32 result = false;
    // TODO(Cian):  not sure would there be more than one mouse event per frame? Might be possible
    for(OS_Event *new_event = 0; os_get_next_event(&new_event);) {
        if(new_event->type == type) {
            if(new_event->mouse_button == button) {
                *event = new_event;
                result = true;
            }
        }
    }
    
    return result;
}

#endif