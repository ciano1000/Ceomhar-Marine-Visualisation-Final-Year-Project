namespace OS{
    
    internal void push_event(Event event) {
        if(os->event_count < OS_MAX_EVENTS) {
            os->events[os->event_count++] = event;
        }
    }
    
    internal b32 get_next_event(Event **event) {
        b32 result = false;
        
        u32 start_idx = 0;
        
        Event *new_e = null;
        
        if((*event)) {
            start_idx = (u32)(*event - os->events) + 1;
        }  
        for(u32 i = start_idx; i < OS_MAX_EVENTS; i++) {
            if(os->events[i].type != Event_Type_Null) {
                new_e = os->events + i;
                break;
            }
        }
        
        *event = new_e;
        result = *event != null;
        return result;
    }
    
    internal void take_event(Event *event) {
        event->type = Event_Type_Null;
    }
    
    // NOTE(Cian): Finds a mouse move event but doesn't take it off the queue just yet, used for dragging
    internal b32 peek_mouse_move(Event **event) {
        // TODO(Cian):  not sure would there be more than one mouse event per frame? Might be possible
        b32 result = false;
        for(Event *new_event = 0; OS::get_next_event(&new_event);) {
            if(new_event->type == Event_Type_MouseMove) {
                *event = new_event;
                result = true;
            }
        }
        
        return result;
    }
    
    internal b32 peek_mouse_button_event(Event **event, Event_Type type, Mouse_Button button) {
        b32 result = false;
        // TODO(Cian):  not sure would there be more than one mouse event per frame? Might be possible
        for(Event *new_event = 0; OS::get_next_event(&new_event);) {
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
        for(u32 i = 0; i < os->event_count; i++) {
            os->events[i] = {};
        }
        
        os->event_count = 0;
    }
    
    internal Event mouse_move_event(V2 position, V2 delta_pos) {
        Event mouse_event = {};
        
        mouse_event.type = Event_Type_MouseMove;
        mouse_event.pos = position;
        mouse_event.delta = delta_pos;
        
        return mouse_event;
    }
    
    internal Event mouse_button_down_event(Mouse_Button button, V2 position) {
        Event mouse_event = {};
        
        mouse_event.type = Event_Type_MouseDown;
        mouse_event.pos = position;
        mouse_event.mouse_button = button;
        
        return mouse_event;
    }
    
    internal Event mouse_button_up_event(Mouse_Button button, V2 position) {
        Event mouse_event = {};
        
        mouse_event.type = Event_Type_MouseUp;
        mouse_event.pos = position;
        mouse_event.mouse_button = button;
        
        return mouse_event;
    }
}