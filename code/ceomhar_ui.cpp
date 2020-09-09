
#if 0
INTERNAL f32 PixelsToDIP(float pixels) {
    // TODO(Cian): Fix rendering so that rounding isn't necessary
    return F32_ROUND(pixels / (global_os->display.dpi / UI_DEFAULT_DENSITY));
}
#endif

INTERNAL f32 DIPToPixels(float dp) {
    return F32_ROUND (dp  * (global_os->display.dpi / UI_DEFAULT_DENSITY));
}

INTERNAL UI_State *UI_InitState() {
    UI_State *state = (UI_State *)Memory_ArenaPush(&global_os->frame_arena, sizeof(UI_State));
    
    // NOTE(Cian): Init pointers to null
    for(u32 i=0;i< UI_HASH_SIZE; i++) {
        state->ui_items_hash[i] = NULL;
    }
    
    state->parent = NULL;
    
    return state;
}

INTERNAL void UI_BeginWindow(char *id) {
    // NOTE(Cian): Init state
    // TODO(Cian): ID hashing and stuff
    UI_Item window = {
        id, UI_LAYOUT_COMPLETE, (f32)global_os->display.width, (f32)global_os->display.height,
        0, 0, (f32)global_os->display.width, (f32)global_os->display.height
    };
    
    UI_Item *p_item = AddUIItem(id, window);
    
    PushUIParent(p_item);
}

#if 0 
INTERNAL b32 UI_IsAllFlagsSet(u32 flags) {
    // NOTE(Cian): This method determines whether enough layout info exists to do auto layout
    // TODO(Cian): Do this method
    return TRUE;
}

INTERNAL void UI_StartToStartConstraint(char *id, f32 offset) {
    // TODO(Cian): Add ability to constrain based on ID instead of just the parent
    ui_state->current->x0 = ui_state->parent->x0;
    ui_state->current->layout_flags = ui_state->current->layout_flags | UI_X0_SET;
}

#if 0
INTERNAL void UI_BottomToBottomConstraint(char *id, f32 offset) {
    // TODO(Cian): Add ability to constrain based on ID instead of just the parent
    ui_state->current.y1 = ui_state->parent.y1;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_Y1_SET;
}
#endif


INTERNAL void UI_Width(f32 width) {
    ui_state->current->width = DIPToPixels(width);
    ui_state->current->layout_flags = ui_state->current->layout_flags | UI_WIDTH_SET;
}

INTERNAL void UI_Height(f32 height) {
    ui_state->current->height = DIPToPixels(height);
    ui_state->current->layout_flags = ui_state->current->layout_flags | UI_HEIGHT_SET;
}

// TODO(Cian): add an id argument here used for looking up this UI element
INTERNAL void UI_Panel(char *id) {
    // TODO(Cian): add safety checks to ensure all necessary layout flags are set
    // TODO(Cian): check flags to see what values are there
    nvgBeginPath(global_vg);
    nvgRect(global_vg,  ui_state->current->x0, ui_state->current->y0, ui_state->current->width, ui_state->current->height);
    nvgFillColor(global_vg, nvgRGBA(40,40,40,255));
    nvgFill(global_vg);
    
    // TODO(Cian): reset ui_state stuff
}
#endif

INTERNAL UI_Item *GetUIItem(char *key) {
    u32 hash_value = StringToCRC32(key);
    u32 hash_index = hash_value & (UI_HASH_SIZE - 1);
    
    UI_Item *item = ui_state->ui_items_hash[hash_index];
    
    if(item) {
        do {
            if(strcmp(item->id, key) == 0) {
                break;
            } else {
                item = item->hash_next;
            }
        } while(item);
    } else {
        return NULL;
    }
    
    return item;
}

INTERNAL UI_Item *AddUIItem(char *key, UI_Item item) {
    u32 hash_value = StringToCRC32(key);
    u32 hash_index = hash_value & (UI_HASH_SIZE - 1);
    
    // TODO(Cian): re-use "deleted" UI_Item pointers that have been cleared?? Maybe
    UI_Item *p_item = ui_state->ui_items_hash[hash_index];
    
    if(p_item == NULL) {
        p_item = (UI_Item *)Memory_ArenaPush(&global_os->frame_arena, sizeof(UI_Item)); 
        *p_item = item;
        p_item->hash_next = NULL;
        ui_state->ui_items_hash[hash_index] = p_item;
        return p_item;
    }
    
    UI_Item *prev = 0;
    while(p_item) {
        
        if(strcmp(p_item->id, key) == 0) {
            item.hash_next = p_item->hash_next;
            *p_item = item;
            return p_item;
        }
        prev = p_item;
        p_item = p_item->hash_next;
    }
    
    prev->hash_next = (UI_Item *)Memory_ArenaPush(&global_os->frame_arena, sizeof(UI_Item));
    *(prev->hash_next) = item;
    
    return prev->hash_next;
}

INTERNAL void PushUIParent(UI_Item *parent) {
    parent->stack_next = ui_state->parent;
    ui_state->parent = parent;
}

INTERNAL UI_Item *PopUIParent() {
    UI_Item *popped = ui_state->parent;
    ui_state->parent = popped->stack_next;
    
    return popped;
}

INTERNAL UI_Item PeekUIParent() {
    UI_Item item = *(ui_state->parent);
    return item;
}


