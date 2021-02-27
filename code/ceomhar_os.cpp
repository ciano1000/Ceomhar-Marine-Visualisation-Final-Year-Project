namespace OS{
    
    internal void push_event(OS_Event event) {
        if(global_os->event_count < OS_MAX_EVENTS) {
            global_os->events[global_os->event_count++] = event;
        }
    }
    
    internal b32 get_next_event(OS_Event **event) {
        b32 result = false;
        
        u32 start_idx = 0;
        
        OS_Event *new_e = null;
        
        if((*event)) {
            start_idx = (u32)(*event - global_os->events) + 1;
        }  
        for(u32 i = start_idx; i < OS_MAX_EVENTS; i++) {
            if(global_os->events[i].type != OS_EventType_Null) {
                new_e = global_os->events + i;
                break;
            }
        }
        
        *event = new_e;
        result = *event != null;
        return result;
    }
    
    internal void take_event(OS_Event *event) {
        event->type = OS_EventType_Null;
    }
    
    // NOTE(Cian): Finds a mouse move event but doesn't take it off the queue just yet, used for dragging
    internal b32 peek_mouse_move(OS_Event **event) {
        // TODO(Cian):  not sure would there be more than one mouse event per frame? Might be possible
        b32 result = false;
        for(OS_Event *new_event = 0; OS::get_next_event(&new_event);) {
            if(new_event->type == OS_EventType_MouseMove) {
                *event = new_event;
                result = true;
            }
        }
        
        return result;
    }
    
    internal b32 peek_mouse_button_event(OS_Event **event, OS_EventType type, OS_MouseButton button) {
        b32 result = false;
        // TODO(Cian):  not sure would there be more than one mouse event per frame? Might be possible
        for(OS_Event *new_event = 0; OS::get_next_event(&new_event);) {
            if(new_event->type == type) {
                if(new_event->mouse_button == button) {
                    *event = new_event;
                    result = true;
                }
            }
        }
        
        return result;
    }
    
    internal void flush_events() {
        for(u32 i = 0; i < global_os->event_count; i++) {
            global_os->events[i] = {};
        }
        
        global_os->event_count = 0;
    }
    
    internal OS_Event mouse_move_event(V2 position, V2 delta_pos) {
        OS_Event mouse_event = {};
        
        mouse_event.type = OS_EventType_MouseMove;
        mouse_event.pos = position;
        mouse_event.delta = delta_pos;
        
        return mouse_event;
    }
    
    internal OS_Event mouse_button_down_event(OS_MouseButton button, V2 position) {
        OS_Event mouse_event = {};
        
        mouse_event.type = OS_EventType_MouseDown;
        mouse_event.pos = position;
        mouse_event.mouse_button = button;
        
        return mouse_event;
    }
    
    internal OS_Event mouse_button_up_event(OS_MouseButton button, V2 position) {
        OS_Event mouse_event = {};
        
        mouse_event.type = OS_EventType_MouseUp;
        mouse_event.pos = position;
        mouse_event.mouse_button = button;
        
        return mouse_event;
    }
}