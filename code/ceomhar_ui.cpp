#pragma warning(push)
#pragma warning(disable: 4505)


INTERNAL f32 PixelsToDIP(float pixels) {
    // TODO(Cian): Fix rendering so that rounding isn't necessary
    return F32_ROUND(pixels / (global_os->display.dpi / UI_DEFAULT_DENSITY));
}

INTERNAL f32 DIPToPixels(float dp) {
    return F32_ROUND (dp  * (global_os->display.dpi / UI_DEFAULT_DENSITY));
}

INTERNAL void UI_Begin() {
    V2 pos = {
        0,0
    };
    V2 size = {
        global_os->display.width,
        global_os->display.height
    };
    
    PushPos(pos);
    PushSize(size);
    PushLayout(UI_FLOW_LAYOUT);
}

INTERNAL void UI_End() {
    // NOTE(Cian): Loop through everything, calculate layout and then process input
}

INTERNAL void UI_PushRow() {
    f32 init_width = 0;
    
    if(ui_state->AutoLayoutState.fill_width == TRUE){
        init_width = ui_state->AutoLayoutState.width;
    } else {
        ui_state->AutoLayoutState.auto_width = TRUE;
    }
    
    V2 init_size = {
        init_width,
        0
    };
    UI_PushPos(ui_state->AutoLayoutState.pos);
    UI_PushLayout(FALSE);
    UI_PushSize(init_size);
}

INTERNAL void UI_PushRow(V2 size) {
    UI_PushPos(ui_state->AutoLayoutState.pos);
    UI_PushLayout(UI_ROW);
    UI_PushSize(size);
}

INTERNAL void UI_PopRow() {
    // NOTE(Cian): At this point we now know the height of the row, loop through child layouts and adjust accordingly
    //Loop through Widgets in the current context and adjust their heights if the CurrLayout contains UI_AUTO
    V2 widgets_indices = GetChildIndices();
    
    for(u32 i = widgets_indices.start; i < widgets_indices.end; ++i) {
        
    }
}

INTERNAL void UI_PushPanel() {
    
}

INTERNAL void PushX(f32 x) {
    assert(ui_state->x_pos_size < UI_STACK_MAX);
    ui_state->x_pos_stack[ui_state->x_pos_size++].x = ui_state->auto_layout_state.pos.x;
    ui_state->auto_layout_state.pos.x += x;
}

INTERNAL void PopX() {
    assert(ui_state->x_pos_size > 0);
    --ui_state->x_pos_size;
    ui_state->auto_layout_state.pos.x = ui_state->x_pos_stack[ui_state->x_pos_size].x;
}

INTERNAL void PushY(f32 y) {
    assert(ui_state->y_pos_size < UI_STACK_MAX);
    ui_state->y_pos_stack[ui_state->y_pos_size++].y = ui_state->auto_layout_state.pos.y;;
    ui_state->auto_layout_state.pos.y += y;
}

INTERNAL void PopX() {
    assert(ui_state->y_pos_size > 0);
    --ui_state->y_pos_size;
    ui_state->auto_layout_state.pos.y = ui_state->y_pos_stack[ui_state->y_pos_size].y;
}

INTERNAL void PushWidth(f32 width, b32 auto) {
    assert(ui_state->width_size < UI_STACK_MAX);
    ui_state->width_stack[ui_state->width_size].width = ui_state->auto_layout_state.size.width;
    ui_state->width_stack[ui_state->width_size].auto_width = ui_state->auto_layout_state.auto_width;
    ui_state->width_stack[ui_state->width_size].width_remaining = ui_state->auto_layout_state.size_remaining.width;
    ui_state->auto_layout_state.size.width = width;
    ui_state->auto_layout_state.auto_width = auto;
    ui_state->auto_layout_state.size_remaining.width = width;
}

INTERNAL void PushWidth(f32 width) {
    PushWidth(width, FALSE);
}

INTERNAL void PushWidthAuto() {
    PushWidth(0, TRUE);
}

INTERNAL void PopWidth() {
    assert(ui_state->x_pos_size > 0);
    --ui_state->x_pos_size;
    ui_state->auto_layout_state.size.width = ui_state->width_stack[ui_state->width_size].width;
    ui_state->auto_layout_state.auto_width = ui_state->width_stack[ui_state->width_size].auto_width;
    ui_state->auto_layout_state.size_remaining.width = ui_state->width_stack[ui_state->width_size].width_remaining;
}

INTERNAL void PushHeight(f32 height, b32 auto) {
    assert(ui_state->height_size < UI_STACK_MAX);
    ui_state->height_stack[ui_state->height_size].height = ui_state->auto_layout_state.size.height;
    ui_state->height_stack[ui_state->height_size].auto_height = ui_state->auto_layout_state.size.auto_height;
    ui_state->height_stack[ui_state->height_size].height_remaining = ui_state->auto_layout_state.size_remaining.height;
    ui_state->auto_layout_state.size.height = height;
    ui_state->auto_layout_state.size.auto_height = auto;
    ui_state->auto_layout_state.size_remaining.height = height;
}

INTERNAL void PushHeight(f32 height) {
    PushWidth(height, FALSE);
}

INTERNAL void PushHeightAuto() {
    PushHeight(0, TRUE);
}

INTERNAL void PopHeight() {
    assert(ui_state->x_pos_size > 0);
    --ui_state->x_pos_size;
    ui_state->auto_layout_state.size.height = ui_state->height_stack[ui_state->height_size].height;
    ui_state->auto_layout_state.size.auto_height = ui_state->height_stack[ui_state->height_size].auto_height;
    ui_state->auto_layout_state.size_remaining.height = ui_state->height_stack[ui_state->height_size].height_remaining;
}

INTERNAL void PushPos(V2 pos) {
    PushX(pos.x);
    PushY(pos.y);
}

INTERNAL void PopPos() {
    PopX();
    PopY();
}

INTERNAL void PushGroupMode(b32 column) {
    assert(ui_state->group_mode_stack_size < UI_STACK_MAX);
    ui_state->group_mode_stack[ui_state->group_mode_stack_size] = ui_state->auto_layout_state.is_col;
    ui_state->auto_layout_state.is_col = column;
}

INTERNAL void PopGroupMode() {
    assert(ui_state->layout_flags_size > 0);
    ui_state->layout_flags_size--;
    ui_state->auto_layout_state.is_col = ui_state->group_mode_stack[ui_state->group_mode_stack_size];
}

INTERNAL void PushSize(V2 size) {
    PushWidth(size.width);
    PushHeight(size.height);
}

INTERNAL void PopSize() {
    PopWidth();
    PopHeight();
}
#pragma warning(pop)