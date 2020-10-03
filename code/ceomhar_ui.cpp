#pragma warning(push)
#pragma warning(disable: 4505)


INTERNAL f32 PixelsToDIP(float pixels) {
    // TODO(Cian): Fix rendering so that rounding isn't necessary
    return F32_ROUND(pixels / (global_os->display.dpi / UI_DEFAULT_DENSITY));
}

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

// TODO(Cian): This is messy
INTERNAL void UI_BeginPanel(char *id, NVGcolor color) {
    UI_Panel(id, color);
    PushUIParent(GetUIItem(id));
}

INTERNAL void UI_EndPanel() {
    PopUIParent();
}

INTERNAL void UI_EndWindow() {
    PopUIParent();
    ui_state->current = {};
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

INTERNAL void UI_StartToStartConstraint(char *id, f32 offset) {
    
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.x0 = relative->x0 + offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X0_SET;
}

INTERNAL void UI_StartToEndConstraint(char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
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

INTERNAL void UI_EndToStartConstraint(char *id, f32 offset) {
    UI_Item *relative = GetUIItem(id);
    
    assert(relative);
    
    ui_state->current.x1 = relative->x0 - offset;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X1_SET;
}

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

INTERNAL void UI_SetY(f32 y) {
    
    ui_state->current.y0 = y;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_Y0_SET;
}

INTERNAL void UI_SetX(f32 x) {
    
    ui_state->current.x0 = x;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X0_SET;
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
// NOTE(Cian): this is all being redone
INTERNAL void UI_Button(char *id, NVGcolor color) {
    // TODO(Cian): UI Element should check it's parent type to see if it should handle it's layout or it's parent should
    
    UI_Item *current = &ui_state->current;
    current->id = id;
    
    b32 all_set = UI_DoLayout(current);
    
    // TODO(Cian): Add Min/Max functions for width and height values that get applied here
    // TODO(Cian): Here is where we will check for unfinished constraints in the buffer and register closures that will be attempted later when necessary info is potentially available.
    assert(all_set);
    
    NVGcolor curr_color = color;
    f32 mx = global_os->mouse_pos.x;
    f32 my = global_os->mouse_pos.y;
    
    f32 px0 = ui_state->current.x0;
    f32 px1 = ui_state->current.x1;
    f32 py0 = ui_state->current.y0;
    f32 py1 = ui_state->current.y1;
    if(mx > px0 && mx < px1 && my > py0 && my < py1) {
        curr_color = nvgRGBA(255,255,255,255);
    }
    AddUIItem(id, ui_state->current);
    
    nvgBeginPath(global_vg);
    nvgRect(global_vg,  ui_state->current.x0, ui_state->current.y0, ui_state->current.width, ui_state->current.height);
    nvgFillColor(global_vg, curr_color);
    nvgFill(global_vg);
    
    // TODO(Cian): reset ui_state stuff
    ui_state->current = {};
}
INTERNAL void UI_Button(Closure *block) {
    char *id = (char *)block->args[0];
    NVGcolor *color = (NVGcolor*)block->args[1];
    
    UI_Button(id, *color);
}

INTERNAL void UI_ButtonClosure(char *id, NVGcolor color) {
    char *heap_id = (char *)Memory_ArenaPush(&global_os->frame_arena, sizeof(char) * strlen(id));
    strcpy(heap_id, id);
    
    NVGcolor *heap_color = (NVGcolor*)Memory_ArenaPush(&global_os->frame_arena, sizeof(NVGcolor));
    *heap_color = color;
    
    Closure closure = {};
    closure.args[0] = (void*)id;
    closure.args[1] = (void*)heap_color;
    closure.call = &UI_Button;
    
    QueueClosure(closure);
}
INTERNAL void UI_Panel(char *id, NVGcolor color) {
    // TODO(Cian): UI Element should check it's parent type to see if it should handle it's layout or it's parent should
    
    UI_Item *current = &ui_state->current;
    current->id = id;
    
    b32 all_set = UI_DoLayout(current);
    
    // TODO(Cian): Add Min/Max functions for width and height values that get applied here
    // TODO(Cian): Here is where we will check for unfinished constraints in the buffer and register closures that will be attempted later when necessary info is potentially available.
    assert(all_set);
    
    NVGcolor curr_color = color;
    
    AddUIItem(id, ui_state->current);
    
    nvgBeginPath(global_vg);
    nvgRect(global_vg,  ui_state->current.x0, ui_state->current.y0, ui_state->current.width, ui_state->current.height);
    nvgFillColor(global_vg, curr_color);
    nvgFill(global_vg);
    
    // TODO(Cian): reset ui_state stuff
    ui_state->current = {};
}

INTERNAL void UI_Panel(Closure *block) {
    char *id = (char *)block->args[0];
    NVGcolor *color = (NVGcolor*)block->args[1];
    
    UI_Panel(id, *color);
}

INTERNAL void UI_PanelClosure(char *id, NVGcolor color) {
    char *heap_id = (char *)Memory_ArenaPush(&global_os->frame_arena, sizeof(char) * strlen(id));
    strcpy(heap_id, id);
    
    NVGcolor *heap_color = (NVGcolor*)Memory_ArenaPush(&global_os->frame_arena, sizeof(NVGcolor));
    *heap_color = color;
    
    Closure closure = {};
    closure.args[0] = (void*)id;
    closure.args[1] = (void*)heap_color;
    closure.call = &UI_Panel;
    
    QueueClosure(closure);
}

INTERNAL void UI_Text(char *id, char *text, f32 font_size, NVGcolor color) {
    // TODO(Cian): Need to work out text wrapping and text spanning multiple rows
    
    // TODO(Cian): Move font size into the ui_state and set like the other layout params
    font_size = DIPToPixels(font_size);
    
    UI_Item *current = &ui_state->current;
    current->id = id;
    
    UI_Height(font_size);
    nvgFontSize(global_vg, current->height);
    nvgTextAlign(global_vg, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
    f32 width = nvgTextBounds(global_vg, 0, 0,text,NULL, NULL);
    
    UI_Width(width); 
    b32 all_set = UI_DoLayout(current);
    
    assert(all_set);
    
    nvgFontFace(global_vg, "roboto-bold");
    nvgFillColor(global_vg, color);
    nvgText(global_vg,current->x0, current->y0, text, NULL);
    
    AddUIItem(id, ui_state->current);
    ui_state->current = {};
}

INTERNAL UI_Item *GetUIItem(char *key) {
    // NOTE(Cian): If the key is a parent escape sequence, simply return the current parent
    // TODO(Cian): Change to custom String library when done
    if(*key == UI_ID_ESCAPE) {
        if(*(key +1) == UI_PARENT)
            return ViewUIParent();
    }
    
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
        // TODO(Cian): All this memory copying is messy, keep note for UI rewrite
        p_item = (UI_Item *)Memory_ArenaPush(&global_os->frame_arena, sizeof(UI_Item)); 
        *p_item = item;
        p_item->id = (char*)Memory_ArenaPush(&global_os->frame_arena, (strlen(key)+1) * sizeof(char));
        strcpy(p_item->id,key);
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
    p_item = (UI_Item *)Memory_ArenaPush(&global_os->frame_arena, sizeof(UI_Item)); 
    *p_item = item;
    p_item->id = (char*)Memory_ArenaPush(&global_os->frame_arena, (strlen(key)+1) * sizeof(char));
    strcpy(p_item->id,key);
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

INTERNAL void UI_DrawGraph_Test() {
    
    f32 max_y = 10;
    f32 max_x = 50;
    
    f32 begin_x = PeekUIParent().x0 + 32;
    f32 end_x = PeekUIParent().x1 - 48;
    f32 begin_y = PeekUIParent().y0 + 16;
    f32 end_y = PeekUIParent().y1 - 32;
    
    f32 width = end_x - (begin_x - 16);
    
    f32 tick_length = 8;
    f32 tick_horizontal_x = begin_x - tick_length;
    for(u32 i = 0; i < 2; ++i) {
        nvgBeginPath(global_vg);
        nvgRect(global_vg, tick_horizontal_x,begin_y +(i * (end_y - begin_y)/2), tick_length, 2);
        nvgFillColor(global_vg, nvgRGBA(0,0,0,255));
        nvgFill(global_vg);
    }
    
    for(u32 i = 0; i < 2; ++i) {
        nvgBeginPath(global_vg);
        nvgRect(global_vg, tick_horizontal_x,begin_y +(i * (end_y - begin_y)/2), tick_length, 2);
        nvgFillColor(global_vg, nvgRGBA(0,0,0,255));
        nvgFill(global_vg);
    }
    
    // NOTE(Cian): Vertical graph bar
    nvgBeginPath(global_vg);
    nvgRect(global_vg, begin_x,begin_y, 2,PeekUIParent().height - 16);
    nvgFillColor(global_vg, nvgRGBA(0,0,0,255));
    nvgFill(global_vg);
    
    nvgBeginPath(global_vg);
    nvgRect(global_vg, begin_x - 16,end_y, PeekUIParent().width - 48, 2);
    nvgFillColor(global_vg, nvgRGBA(0,0,0,255));
    nvgFill(global_vg);
    
    // NOTE(Cian): Some random samples of a Sin wave
    NVGpaint bg;
	f32 samples[6];
	f32 sx[6], sy[6];
	f32 dx = width/5.0f;
    u32 i;
    f32 t = (f32)global_os->current_time;
    
    f32 h = end_y - begin_y;
    f32 x = begin_x - 16;
    f32 y = begin_y;
    f32 w = width;
    
	samples[0] = (1+sinf(t*1.2345f+cosf(t*0.33457f)*0.44f))*0.5f;
	samples[1] = (1+sinf(t*0.68363f+cosf(t*1.3f)*1.55f))*0.5f;
	samples[2] = (1+sinf(t*1.1642f+cosf(t*0.33457f)*1.24f))*0.5f;
	samples[3] = (1+sinf(t*0.56345f+cosf(t*1.63f)*0.14f))*0.5f;
	samples[4] = (1+sinf(t*1.6245f+cosf(t*0.254f)*0.3f))*0.5f;
	samples[5] = (1+sinf(t*0.345f+cosf(t*0.03f)*0.6f))*0.5f;
    
	for (i = 0; i < 6; i++) {
		sx[i] = begin_x+i*dx;
		sy[i] = begin_y +PeekUIParent().height*samples[i]*0.8f;
	}
    
#if 0
	// Graph line
	nvgBeginPath(global_vg);
	nvgMoveTo(global_vg, sx[0], sy[0]+2);
	for (i = 1; i < 6; i++)
		nvgBezierTo(global_vg, sx[i-1],sy[i-1], sx[i],sy[i], sx[i],sy[i]);
	nvgStrokeColor(global_vg, nvgRGBA(0,0,0,32));
	nvgStrokeWidth(global_vg, 3.0f);
	nvgStroke(global_vg);
#endif
    
	nvgBeginPath(global_vg);
	nvgMoveTo(global_vg, sx[0], sy[0]);
	for (i = 1; i < 6; i++)
		nvgBezierTo(global_vg, sx[i-1]+dx*0.5f,sy[i-1], sx[i]-dx*0.5f,sy[i], sx[i],sy[i]);
	nvgStrokeColor(global_vg, nvgRGBA(0,160,192,255));
	nvgStrokeWidth(global_vg, 3.0f);
	nvgStroke(global_vg);
    
	// Graph sample pos
    
    
	for (i = 0; i < 6; i++) {
		bg = nvgRadialGradient(global_vg, sx[i],sy[i]+2, 3.0f,8.0f, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,0));
		nvgBeginPath(global_vg);
		nvgRect(global_vg, sx[i]-10, sy[i]-10+2, 20,20);
		nvgFillPaint(global_vg, bg);
		nvgFill(global_vg);
	}
    
	nvgBeginPath(global_vg);
	for (i = 0; i < 6; i++)
		nvgCircle(global_vg, sx[i], sy[i], 4.0f);
	nvgFillColor(global_vg, nvgRGBA(0,160,192,255));
	nvgFill(global_vg);
	nvgBeginPath(global_vg);
	for (i = 0; i < 6; i++)
		nvgCircle(global_vg, sx[i], sy[i], 2.0f);
	nvgFillColor(global_vg, nvgRGBA(220,220,220,255));
	nvgFill(global_vg);
    
	nvgStrokeWidth(global_vg, 1.0f);
    
}

// TODO(Cian): Look into this again once we investigate autolayout

INTERNAL void VerticalLinearLayout_Test(Closure *block) {
    f32 *width = (f32*)block->args[0];
    f32 *height = (f32*)block->args[1];
    f32 *margin = (f32*)block->args[2];
    
    u32 num_items = PeekUIParent().code_view.size;
    f32 curr_y = PeekUIParent().y0;
    
    
    Closure *p_current = TakeClosure();
    ui_state->current = {};
    while(p_current) {
        if(p_current) {
            UI_Height(*height);
            UI_Width(*width);
            UI_SetY(curr_y);
            UI_StartToStartConstraint(PeekUIParent().id,0);
            p_current->call(p_current);
            
            curr_y += *height + *margin;
            p_current = TakeClosure();
            ui_state->current = {};
        }
    }
}

INTERNAL void UI_BeginNavMenu(char *id, u32 orientation, f32 width, f32 height, f32 margin) {
    UI_Item *menu = &ui_state->current;
    
    menu->id = id;
    
    b32 all_set = UI_DoLayout(menu);
    assert(all_set);
    f32 *heap_width =  (f32*)Memory_ArenaPush(&global_os->frame_arena, sizeof(f32));
    *heap_width = DIPToPixels(width); 
    f32 *heap_height =  (f32*)Memory_ArenaPush(&global_os->frame_arena, sizeof(f32));
    *heap_height = DIPToPixels(height); 
    f32 *heap_margin =  (f32*)Memory_ArenaPush(&global_os->frame_arena, sizeof(f32));
    *heap_margin = DIPToPixels(margin);; 
    
    Closure menu_closure = {};
    menu_closure.call = &VerticalLinearLayout_Test;
    menu_closure.args[0] = (void*)heap_width;
    menu_closure.args[1] = (void*)heap_height;
    menu_closure.args[2] = (void*)heap_margin;
    menu = AddUIItem(id, *menu);
    PushUIParent(menu);
    QueueClosure(menu_closure);
    
}



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
#pragma warning(pop)