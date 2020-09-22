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
    state->current.constraints_list[0] = 0;
    
    return state;
}

INTERNAL void UI_BeginWindow(char *id) {
    // NOTE(Cian): Init state
    UI_Item window = {};
    window.id = id;
    window.layout_flags = UI_LAYOUT_COMPLETE;;
    window.width = (f32)global_os->display.width;
    window.height = (f32)global_os->display.height;
    window.x0 = 0;
    window.y0 = 0;
    window.x1 = window.width;
    window.y1 = window.height;
    window.code_view = {};
    
    UI_Item *p_item = AddUIItem(id, window);
    
    PushUIParent(p_item);
}

INTERNAL void VerticalLinearLayout_Test(Closure *block) {
    f32 *width = (f32*)block->args[0];
    f32 height = (*width);
    
    u32 num_items = PeekUIParent().code_view.size;
    
    Closure *p_current = TakeClosure();
    ui_state->current = {};
    while(p_current) {
        if(p_current) {
            // TODO(Cian): Dynamically create id's for elements based on menu id
            UI_Width(*width);
            UI_Height(height);
            UI_StartToStartConstraint(PeekUIParent().id, 0);
            UI_EndToEndConstraint(PeekUIParent().id, 0);
            p_current->call(p_current);
            
            p_current = TakeClosure();
            ui_state->current = {};
        }
    }
}

INTERNAL void UI_BeginNavMenu(char *id, u32 orientation, f32 width, f32 height, f32 margin) {
    // TODO(Cian): Add more options for defining sizing(e.g 3 per row/column), margins(for left/top/right/bottom), making every n item x times bigger/smaller 
    UI_Item *menu = &ui_state->current;
    
    menu->id = id;
    
    b32 all_set = UI_DoLayout(menu);
    // TODO(Cian): not sure if deferring layout of menu itself will work so asserting for now
    assert(all_set);
    f32 *heap_width =  (f32*)Memory_ArenaPush(&global_os->frame_arena, sizeof(f32));
    *heap_width = ui_state->current.width; 
    
    Closure menu_closure = {};
    menu_closure.call = &VerticalLinearLayout_Test;
    menu_closure.args[0] = (void*)heap_width;
    menu = AddUIItem(id, *menu);
    PushUIParent(menu);
    QueueClosure(menu_closure);
    
}


INTERNAL void UI_EndWindow() {
    
    Closure *p_current = TakeClosure();
    
    while(p_current) {
        if(p_current) {
            p_current->call(p_current);
            p_current = TakeClosure();
        }
    }
    
    PopUIParent();
}

INTERNAL void UI_EndLayout() {
    
    Closure *p_current = TakeClosure();
    p_current->call(p_current);
    
    
    while(p_current) {
        if(p_current) {
            p_current->call(p_current);
            p_current = TakeClosure();
        }
    }
    
    PopUIParent();
}

#if 0 
INTERNAL b32 UI_IsAllFlagsSet(u32 flags) {
    // NOTE(Cian): This method determines whether enough layout info exists to do auto layout
    // TODO(Cian): Do this method
    return TRUE;
}
#endif

INTERNAL void UI_StartToStartConstraint(const char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    if(!relative) {
        
        char *heap_id = (char *)Memory_ArenaPush(&global_os->frame_arena, strlen(id) * sizeof(char));
        f32 *heap_offset = (f32 *)Memory_ArenaPush(&global_os->frame_arena, sizeof(f32));
        
        strcpy(heap_id, id);
        *heap_offset = offset;
        
        Closure closure = {};
        closure.call = &UI_StartToStartConstraint_Closure;
        closure.args[0] = (void *)heap_id;
        closure.args[1] = (void *)heap_offset;
        
        QueueClosure(closure);
        return;
    }
    
    ui_state->current.x0 = relative->x0 + offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X0_SET;
}

INTERNAL void UI_StartToStartConstraint_Closure(Closure *block) {
    char *id = (char*)block->args[0];
    f32 *offset = (f32*)block->args[1];
    
    UI_StartToStartConstraint(id, *offset);
}

INTERNAL void UI_StartToEndConstraint(const char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    // TODO(Cian): if GetUIItem returns NULL, maybe the UI_Item hasn't been created yet, defer layout till enclosing parent ends e.g add layout closure to register ---- do the same for all other id based layout methods below
    
    if(!relative) {
        char *heap_id = (char *)Memory_ArenaPush(&global_os->frame_arena, strlen(id) * sizeof(char));
        f32 *heap_offset = (f32 *)Memory_ArenaPush(&global_os->frame_arena, sizeof(f32));
        
        strcpy(heap_id, id);
        *heap_offset = offset;
        
        Closure closure = {};
        closure.call = &UI_StartToEndConstraint_Closure;
        closure.args[0] = (void *)heap_id;
        closure.args[1] = (void *)heap_offset;
        return;
    }
    
    ui_state->current.x0 = relative->x1 + offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X0_SET;
}

INTERNAL void UI_StartToEndConstraint_Closure(Closure *block) {
    char *id = (char*)block->args[0];
    f32 *offset = (f32*)block->args[1];
    
    UI_StartToEndConstraint(id, *offset);
}

INTERNAL void UI_EndToEndConstraint(const char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.x1 = relative->x1 - offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X1_SET;
}

#if 0
INTERNAL void UI_EndToStartConstraint(const char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.x1 = relative->x0 - offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X1_SET;
}
#endif
INTERNAL void UI_BottomToBottomConstraint(const char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    
    if(!relative) {
        char *heap_id = (char *)Memory_ArenaPush(&global_os->frame_arena, strlen(id) * sizeof(char));
        f32 *heap_offset = (f32 *)Memory_ArenaPush(&global_os->frame_arena, sizeof(f32));
        
        strcpy(heap_id, id);
        *heap_offset = offset;
        
        Closure closure = {};
        closure.call = &UI_BottomToBottomConstraint_Closure;;
        closure.args[0] = (void *)heap_id;
        closure.args[1] = (void *)heap_offset;
        
        QueueClosure(closure);
        return;
    }
    
    ui_state->current.y1 = relative->y1 - offset;;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_Y1_SET;
}


INTERNAL void UI_BottomToBottomConstraint_Closure(Closure *block) {
    char *id = (char*)block->args[0];
    f32 *offset = (f32*)block->args[1];
    
    UI_BottomToBottomConstraint(id, *offset);
}

INTERNAL void UI_TopToTopConstraint(const char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.y0 = relative->y0 + offset;;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_Y0_SET;
}


INTERNAL void UI_Width(f32 width) {
    ui_state->current.width = DIPToPixels(width);
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_WIDTH_SET;
}

INTERNAL void UI_FillWidth(f32 offset) {
    ui_state->current.width = PeekUIParent().width - offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_WIDTH_SET;
}


INTERNAL void UI_Height(f32 height) {
    ui_state->current.height = DIPToPixels(height);
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_HEIGHT_SET;
}

INTERNAL void UI_FillHeight(f32 offset) {
    ui_state->current.height = PeekUIParent().height -offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_HEIGHT_SET;
}

INTERNAL void UI_MinHeight(f32 min) {
    
    u32 i;
    
    for(i = 0; i < ArrayCount(ui_state->current.constraints_list) - 1;++i) {
        if(ui_state->current.constraints_list[i] == 0) {
            ui_state->current.constraints_list[i] = UI_MIN_HEIGHT;
            ui_state->current.constraints_list[i + 1] = (u32)min;
            
            break;
        }
    }
}

#if 0
INTERNAL void UI_MaxHeight(f32 max) {
    
    u32 i;
    
    for(i = 0; i < ArrayCount(ui_state->current.constraints_list) - 1;++i) {
        if(ui_state->current.constraints_list[i] == 0) {
            ui_state->current.constraints_list[i] = UI_MAX_HEIGHT;
            ui_state->current.constraints_list[i + 1] = (u32)max;
            
            break;
        }
    }
}

INTERNAL void UI_MinWidth(f32 min) {
    
    u32 i;
    
    for(i = 0; i < ArrayCount(ui_state->current.constraints_list) - 1;++i) {
        if(ui_state->current.constraints_list[i] == 0) {
            ui_state->current.constraints_list[i] = UI_MIN_WIDTH;
            ui_state->current.constraints_list[i + 1] = (u32)min;
            
            break;
        }
    }
}
INTERNAL void UI_MaxWidth(f32 max) {
    
    u32 i;
    
    for(i = 0; i < ArrayCount(ui_state->current.constraints_list) - 1;++i) {
        if(ui_state->current.constraints_list[i] == 0) {
            ui_state->current.constraints_list[i] = UI_MAX_WIDTH;
            ui_state->current.constraints_list[i + 1] = (u32)max;
            
            break;
        }
    }
}


#endif

INTERNAL b32 UI_DoLayout(UI_Item *current) {
    b32 all_set = TRUE;
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
        all_set = FALSE;
        break;
    }
    
    for(u32 i = 0; i < ArrayCount(ui_state->current.constraints_list) - 1;++i) {
        u32 current_val = current->constraints_list[i];
        u32 next_val = current->constraints_list[i + 1];
        
        switch(current_val) {
            case UI_MIN_HEIGHT: {
                current->height = CLAMP_MIN(current->height, (f32)next_val);
                current->y1 = ui_state->current.y0 + ui_state->current.height;
            } break;
            
            case UI_MAX_HEIGHT: {
                current->height = CLAMP_MAX(current->height, (f32)next_val);
                current->y1 = ui_state->current.y0 + ui_state->current.height;
            } break;
            
            case UI_MIN_WIDTH: {
                current->width = CLAMP_MIN(current->width, (f32)next_val);
                current->x1 = ui_state->current.x0 + ui_state->current.width;
            } break;
            
            case UI_MAX_WIDTH: {
                current->width = CLAMP_MAX(current->width, (f32)next_val);
                current->x1 = ui_state->current.x0 + ui_state->current.width;
            } break;
            default:
            break;
        }
        // NOTE(Cian): skip over value to get to next definition
        ++i;
    }
    
    return all_set;
    
}
INTERNAL void UI_Panel(const char *id, NVGcolor color) {
    // TODO(Cian): UI Element should check it's parent type to see if it should handle it's layout or it's parent should
    
    UI_Item *current = &ui_state->current;
    current->id = id;
    
    b32 all_set = UI_DoLayout(current);
    
    // TODO(Cian): Add Min/Max functions for width and height values that get applied here
    // TODO(Cian): Here is where we will check for unfinished constraints in the buffer and register closures that will be attempted later when necessary info is potentially available.
    if(all_set) {
        
    }
    
    AddUIItem(id, ui_state->current);
    
    nvgBeginPath(global_vg);
    nvgRect(global_vg,  ui_state->current.x0, ui_state->current.y0, ui_state->current.width, ui_state->current.height);
    nvgFillColor(global_vg, color);
    nvgFill(global_vg);
    
    // TODO(Cian): reset ui_state stuff
    ui_state->current = {};
}


INTERNAL void UI_Panel(Closure *block) {
    char *id = (char *)block->args[0];
    NVGcolor *color = (NVGcolor*)block->args[1];
    
    UI_Panel(id, *color);
}

INTERNAL void UI_Text(const char *id, const char *text, f32 font_size, NVGcolor color) {
    // TODO(Cian): Need to work out text wrapping and text spanning multiple rows
    
    font_size = DIPToPixels(font_size);
    
    UI_Item *current = &ui_state->current;
    current->id = id;
    UI_Height(font_size);
    UI_Width(0); // NOTE(Cian): Bit of a hack but needed due to how nanovg calculates text sizing
    b32 all_set = UI_DoLayout(current);
    
    if(!all_set) {
        char *heap_id = (char *)Memory_ArenaPush(&global_os->frame_arena, strlen(id) * sizeof(char));
        char *heap_text = (char *)Memory_ArenaPush(&global_os->frame_arena, strlen(text) * sizeof(char));
        f32 *heap_font_size = (f32 *)Memory_ArenaPush(&global_os->frame_arena, sizeof(f32));
        NVGcolor *heap_color = (NVGcolor *)Memory_ArenaPush(&global_os->frame_arena, sizeof(NVGcolor));
        // TODO(Cian): Pull layout stuff into it's own struct later, this is currently a bit wasteful lol
        UI_Item *heap_item = (UI_Item*)Memory_ArenaPush(&global_os->frame_arena, sizeof(UI_Item));
        
        strcpy(heap_id, id);
        strcpy(heap_text, text);
        *heap_font_size = font_size;
        *heap_color = color;
        *heap_item = ui_state->current;
        
        Closure closure = {};
        closure.call = &UI_Text_Closure;;
        closure.args[0] = (void *)heap_id;
        closure.args[1] = (void *)heap_text;
        closure.args[2] = (void *)heap_font_size;
        closure.args[3] = (void *)heap_color;
        closure.args[4] = (void *)heap_item;
        
        QueueClosure(closure);
        ui_state->current = {};
        
        return;
    }
    
    
    nvgFontSize(global_vg, current->height);
    nvgFontFace(global_vg, "roboto-bold");
    // NOTE(Cian): Aligning to the left/right means that the left/right (e.g beginning of text/ end of text) is positioned at the given coordinates
    nvgTextAlign(global_vg, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
    nvgFillColor(global_vg, color);
    f32 title_end_x = nvgText(global_vg, current->x0, current->y0, text, NULL);
    current->width = title_end_x - current->x0;
    current->x1 =  title_end_x;
    
    AddUIItem(id, ui_state->current);
    ui_state->current = {};
}

INTERNAL void UI_Text_Closure(Closure *block) {
    char *id = (char *)block->args[0];
    char *text = (char *)block->args[1];
    f32 *text_size = (f32 *)block->args[2];
    NVGcolor *color = (NVGcolor *)block->args[3];
    UI_Item *item = (UI_Item*)block->args[4];
    UI_Item *current = &ui_state->current;
    
    current->layout_flags = current->layout_flags | item->layout_flags;
    memcpy(current->constraints_list, item->constraints_list, ArrayCount(item->constraints_list) * sizeof(u32));
    current->width += item->width;
    current->height += item->height;
    current->x0 += item->x0;
    current->x1 += item->x1;
    current->y0 += item->y0;
    current->y1 += item->y1;
    UI_Text(id, text, *text_size, *color);
}



INTERNAL UI_Item *GetUIItem(const char *key) {
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

INTERNAL UI_Item *AddUIItem(const char *key, UI_Item item) {
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

INTERNAL UI_Item *PopUIParent() {
    UI_Item *popped = ui_state->parent;
    ui_state->parent = popped->stack_next;
    
    return popped;
}

// TODO(Cian): Change Peek to return a pointer and fix all occurences in code
INTERNAL UI_Item PeekUIParent() {
    UI_Item item = *(ui_state->parent);
    return item;
}

INTERNAL UI_Item *ViewUIParent() {
    UI_Item *item = ui_state->parent;
    return item;
}

INTERNAL void QueueClosure(Closure closure) {
    // NOTE(Cian): We should never run out of space for closures
    CodeView *code_view = &ViewUIParent()->code_view;
    assert(code_view->size <= ArrayCount(code_view->closure_register)); 
    
    code_view->closure_register[code_view->rear] = closure;
    code_view->rear = (code_view->rear + 1) % ArrayCount(code_view->closure_register);
    code_view->size += 1;
}

INTERNAL Closure *TakeClosure() {
    CodeView *code_view = &ViewUIParent()->code_view;
    Closure *result = NULL;
    if(code_view->size == 0) 
        return result;
    
    result = &code_view->closure_register[code_view->front];
    code_view->front = (code_view->front + 1) % ArrayCount(code_view->closure_register);
    code_view->size -= 1;
    
    return result;
}

#if 0
// TODO(Cian): Once we get a unit test system in place add this to it
INTERNAL void TestUIHash() {
    char *generated_strings[256];
    for(u32 i = 0; i < 256; ++i) {
        char rand_string[14];
        StringGenRandom(rand_string, 14);
        generated_strings[i] = (char *)Memory_ArenaPush(&global_os->frame_arena, sizeof(rand_string));
        strcpy(generated_strings[i], rand_string);
        UI_Item item = {};
        item.id = (char *)Memory_ArenaPush(&global_os->frame_arena, sizeof(rand_string));
        strcpy(item.id, rand_string);
        AddUIItem(rand_string, item);
    }
    
    for(u32 i =0; i< 256; ++i) {
        UI_Item *result = GetUIItem(generated_strings[i]);
        if(result == NULL) {
            printf("oops");
        }
        
    }
}
#endif