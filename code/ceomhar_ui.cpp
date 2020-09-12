
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
    state->current = {};
    
    return state;
}

INTERNAL void UI_BeginWindow(char *id) {
    // NOTE(Cian): Init state
    UI_Item window = {
        id, UI_LAYOUT_COMPLETE,0 ,(f32)global_os->display.width, (f32)global_os->display.height,
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
#endif

INTERNAL void UI_StartToStartConstraint(char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    // TODO(Cian): if GetUIItem returns NULL, maybe the UI_Item hasn't been created yet, defer layout till enclosing parent ends e.g add layout closure to register ---- do the same for all other id based layout methods below
    
    assert(relative);
    
    ui_state->current.x0 = relative->x0 + offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X0_SET;
}

INTERNAL void UI_StartToEndConstraint(char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    // TODO(Cian): if GetUIItem returns NULL, maybe the UI_Item hasn't been created yet, defer layout till enclosing parent ends e.g add layout closure to register ---- do the same for all other id based layout methods below
    
    assert(relative);
    
    ui_state->current.x0 = relative->x1 + offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X0_SET;
}

INTERNAL void UI_EndToEndConstraint(char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.x1 = relative->x1 - offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X1_SET;
}

#if 0
INTERNAL void UI_EndToStartConstraint(char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.x1 = relative->x0 - offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X1_SET;
}
#endif
INTERNAL void UI_BottomToBottomConstraint(char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.y1 = relative->y1 - offset;;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_Y1_SET;
}


INTERNAL void UI_TopToTopConstraint(char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.y0 = relative->y0 + offset;;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_Y0_SET;
}


INTERNAL void UI_Width(f32 width) {
    ui_state->current.width = DIPToPixels(width);
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_WIDTH_SET;
}

#if 0
INTERNAL void UI_Height(f32 height) {
    ui_state->current.height = DIPToPixels(height);
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_HEIGHT_SET;
}
#endif
INTERNAL void UI_Panel(char *id, NVGcolor color) {
    b32 all_set = TRUE;
    
    UI_Item *current = &ui_state->current;
    current->id = id;
    switch(current->layout_flags) {
        case UI_W_H_X0_Y0:
        current->x1 = current->x0 + current->width;
        current->y1 = current->y0 + current->height;
        break;
        
        case UI_W_H_X1_Y1:
        current->x0 = current->x1 - current->width;
        current->y0 = current->y1 - current->height;
        break;
        
        case UI_W_H_X1_Y0:
        current->x0 = current->x1 - current->width;
        current->y1 = current->y0 + current->height;
        break;
        
        case UI_W_H_X0_Y1:
        current->x1 = current->x0 + current->width;
        current->y0 = current->y1 - current->height;
        break;
        
        case UI_H_X0_X1_Y0:
        current->width = current->x1 - current->x0;
        current->y1 = current->y0 + current->height;
        break;
        
        case UI_H_X0_X1_Y1:
        current->y0 = current->y1 - current->height;
        break;
        
        case UI_W_X0_Y0_Y1:
        current->height = current->y1 - current->y0;
        current->x1 = current->x0 + current->width;
        break;
        
        case UI_W_X1_Y0_Y1:
        current->height = current->y1 - current->y0;
        current->x0 = current->x1 - current->width;
        break;
        
        case UI_X0_X1_Y0_Y1:
        current->width = current->x1 - current->x0;
        current->height = current->y1 - current->y0;
        break;
        
        default:
        // TODO(Cian): Here is where we will check for unfinished constraints in the buffer and register closures that will be attempted later when necessary info is potentially available.
        all_set = FALSE;
    }
    
    // TODO(Cian): Add Min/Max functions for width and height values that get applied here
    
    assert(all_set);
    
    AddUIItem(id, ui_state->current);
    
    nvgBeginPath(global_vg);
    nvgRect(global_vg,  ui_state->current.x0, ui_state->current.y0, ui_state->current.width, ui_state->current.height);
    nvgFillColor(global_vg, color);
    nvgFill(global_vg);
    
    // TODO(Cian): reset ui_state stuff
    ui_state->current = {};
}


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
    prev->hash_next->hash_next = NULL;
    return prev->hash_next;
}

INTERNAL void PushUIParent(UI_Item *parent) {
    parent->stack_next = ui_state->parent;
    ui_state->parent = parent;
}

#if 0
INTERNAL UI_Item *PopUIParent() {
    UI_Item *popped = ui_state->parent;
    ui_state->parent = popped->stack_next;
    
    return popped;
}

#endif
INTERNAL UI_Item PeekUIParent() {
    UI_Item item = *(ui_state->parent);
    return item;
}

