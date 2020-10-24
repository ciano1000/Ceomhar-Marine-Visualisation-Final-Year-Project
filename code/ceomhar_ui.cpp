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
    ui_state->auto_layout_state.pos = {0,0};
    ui_state->auto_layout_state.size = {0,0};;
    ui_state->auto_layout_state.size_remaining = {(f32)global_os->display.width,
        (f32)global_os->display.height};
}

INTERNAL void UI_End() {
    // NOTE(Cian): Loop through everything, calculate layout and then process input
    *ui_state = {};
}

INTERNAL void UI_PushRow() {
    UI_PushPos(ui_state->auto_layout_state.pos);
    UI_PushGroupMode(FALSE);
}

INTERNAL void UI_PushRow(V2 size) {
    UI_PushPos(ui_state->auto_layout_state.pos);
    UI_PushGroupMode(FALSE);
    UI_PushSize(size);
}

INTERNAL void UI_PushRow(V2 pos, V2 size) {
    UI_PushPos(pos);
    UI_PushGroupMode(FALSE);
    UI_PushSize(size);
}

INTERNAL void UI_PopRow() {
    // NOTE(Cian): At this point we now know the height of the row, loop through child layouts and adjust accordingly
    //Loop through Widgets in the current context and adjust their heights if the CurrLayout contains UI_AUTO
    u32 start = ui_state->auto_layout_state.start;
    u32 end = ui_state->auto_layout_state.end;
    
    for(u32 i = start; i < end; ++i) {
        
    }
    
    UI_PopWidgetIndices();
}

INTERNAL void UI_PushPanel(V2 padding, NVGcolor fill_color, NVGcolor border_color) {
    V2 pos = {
        ui_state->auto_layout_state.pos.x + padding.x,
        ui_state->auto_layout_state.pos.y + padding.y
    };
    
    nvgBeginPath(global_vg);
    nvgRect(global_vg, ui_state->auto_layout_state.pos.x, ui_state->auto_layout_state.pos.y, ui_state->auto_layout_state.size.width, ui_state->auto_layout_state.size.height);
    nvgFillColor(global_vg, fill_color);
    nvgFill(global_vg);
    
    nvgBeginPath(global_vg);
    nvgRect(global_vg, ui_state->auto_layout_state.pos.x, ui_state->auto_layout_state.pos.y, ui_state->auto_layout_state.size.width, ui_state->auto_layout_state.size.height);
    nvgRect(global_vg, ui_state->auto_layout_state.pos.x + 2, ui_state->auto_layout_state.pos.y + 2, ui_state->auto_layout_state.size.width - 4, ui_state->auto_layout_state.size.height - 4);
    nvgPathWinding(global_vg, NVG_HOLE);
    nvgFillColor(global_vg, border_color);
    nvgFill(global_vg);
    
    UI_PushPos(pos);
    UI_PushGroupMode(FALSE); 
    UI_PushPadding(padding);
}

INTERNAL void UI_PushPanel(char *title, V2 padding) {
    
}

INTERNAL void UI_PopPanel() {
    // NOTE(Cian):  If auto_height/auto_width, each widget will calculate it's required height, if smaller than current height it will extend to match, if larger it will add to the global height. If not auto_height/auto_width, the widget will simply match the given dimensions
    u32 start = ui_state->auto_layout_state.start;
    u32 end = ui_state->auto_layout_state.end;
    
    for(u32 i = start; i < end; ++i) {
        
    }
    UI_PopRow();
    UI_PopPadding();
    UI_PopPos();
    // NOTE(Cian): Down here, update the current autolayout values with our panels final width, height whatever
}
// TODO(Cian): Closures for button clicks
INTERNAL void UI_PushButton(char *label) {
    V2 button_size = ui_state->auto_layout_state.size;
    V2 button_pos = ui_state->auto_layout_state.pos;
    // TODO(Cian): make this customizable
    V2 button_padding = {
        8,4
    };
    
    f32 font_size = DIPToPixels(16);
    nvgFontSize(global_vg, font_size);
    nvgTextAlign(global_vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    V2 text_size = {
        nvgTextBounds(global_vg, 0, 0, label, NULL, NULL),
        font_size
    };
    // TODO(Cian): make a PushFontSize method
    if(ui_state->auto_layout_state.auto_width) {
        button_size.width = text_size.width + (button_padding.x*2);
    }
    
    f32 text_x = ((button_size.width / 2) + button_pos.x) - (text_size.width / 2);
    f32 text_y = (button_size.y / 2);
    
    nvgFontFace(global_vg, "roboto-bold");
    nvgFillColor(global_vg, nvgRGBA(255,255,255,255));
    nvgText(global_vg,text_x,text_y, label, NULL);
    
    ui_state->auto_layout_state.pos.x += button_size.x;
}

INTERNAL void UI_PushX(f32 x) {
    assert(ui_state->x_pos_size < UI_MAX_STACK);
    ui_state->x_pos_stack[ui_state->x_pos_size++].x = ui_state->auto_layout_state.pos.x;
    ui_state->auto_layout_state.pos.x += x;
}

INTERNAL void UI_PopX() {
    assert(ui_state->x_pos_size > 0);
    --ui_state->x_pos_size;
    ui_state->auto_layout_state.pos.x = ui_state->x_pos_stack[ui_state->x_pos_size].x;
}

INTERNAL void UI_PushY(f32 y) {
    assert(ui_state->y_pos_size < UI_MAX_STACK);
    ui_state->y_pos_stack[ui_state->y_pos_size++].y = ui_state->auto_layout_state.pos.y;;
    ui_state->auto_layout_state.pos.y += y;
}

INTERNAL void UI_PopY() {
    assert(ui_state->y_pos_size > 0);
    --ui_state->y_pos_size;
    ui_state->auto_layout_state.pos.y = ui_state->y_pos_stack[ui_state->y_pos_size].y;
}

INTERNAL void UI_PushWidth(f32 width, b32 auto_width) {
    assert(ui_state->width_size < UI_MAX_STACK);
    ui_state->width_stack[ui_state->width_size].width = ui_state->auto_layout_state.size.width;
    ui_state->width_stack[ui_state->width_size].auto_width = ui_state->auto_layout_state.auto_width;
    ui_state->width_stack[ui_state->width_size].width_remaining = ui_state->auto_layout_state.size_remaining.width;
    ui_state->width_size++;
    
    ui_state->auto_layout_state.size.width = width;
    ui_state->auto_layout_state.auto_width = auto_width;
    ui_state->auto_layout_state.size_remaining.width = width;
}

INTERNAL void UI_PushWidth(f32 width) {
    UI_PushWidth(width, FALSE);
}

INTERNAL void UI_PushWidthAuto() {
    UI_PushWidth(0, TRUE);
}

INTERNAL void UI_PopWidth() {
    assert(ui_state->width_size > 0);
    --ui_state->width_size;
    ui_state->auto_layout_state.size.width = ui_state->width_stack[ui_state->width_size].width;
    ui_state->auto_layout_state.auto_width = ui_state->width_stack[ui_state->width_size].auto_width;
    ui_state->auto_layout_state.size_remaining.width = ui_state->width_stack[ui_state->width_size].width_remaining;
}

INTERNAL void UI_PushHeight(f32 height, b32 auto_height) {
    assert(ui_state->height_size < UI_MAX_STACK);
    ui_state->height_stack[ui_state->height_size].height = ui_state->auto_layout_state.size.height;
    ui_state->height_stack[ui_state->height_size].auto_height = ui_state->auto_layout_state.auto_height;
    ui_state->height_stack[ui_state->height_size].height_remaining = ui_state->auto_layout_state.size_remaining.height;
    ui_state->height_size++;
    
    ui_state->auto_layout_state.size.height = height;
    ui_state->auto_layout_state.auto_height = auto_height;
    ui_state->auto_layout_state.size_remaining.height = height;
}

INTERNAL void UI_PushHeight(f32 height) {
    UI_PushHeight(height, FALSE);
}

INTERNAL void UI_PushHeightAuto() {
    UI_PushHeight(0, TRUE);
}

INTERNAL void UI_PopHeight() {
    assert(ui_state->height_size > 0);
    --ui_state->height_size;
    ui_state->auto_layout_state.size.height = ui_state->height_stack[ui_state->height_size].height;
    ui_state->auto_layout_state.auto_height = ui_state->height_stack[ui_state->height_size].auto_height;
    ui_state->auto_layout_state.size_remaining.height = ui_state->height_stack[ui_state->height_size].height_remaining;
}

INTERNAL void UI_PushPos(V2 pos) {
    UI_PushX(pos.x);
    UI_PushY(pos.y);
}

INTERNAL void UI_PopPos() {
    UI_PopX();
    UI_PopY();
}

INTERNAL void UI_PushGroupMode(b32 column) {
    assert(ui_state->group_mode_stack_size < UI_MAX_STACK);
    ui_state->group_mode_stack[ui_state->group_mode_stack_size++].is_column = ui_state->auto_layout_state.is_col;
    ui_state->auto_layout_state.is_col = column;
}

INTERNAL void UI_PopGroupMode() {
    assert(ui_state->group_mode_stack_size > 0);
    ui_state->group_mode_stack_size--;
    ui_state->auto_layout_state.is_col = ui_state->group_mode_stack[ui_state->group_mode_stack_size].is_column;
}

INTERNAL void UI_PushSize(V2 size) {
    UI_PushWidth(size.width);
    UI_PushHeight(size.height);
}

INTERNAL void UI_PopSize() {
    UI_PopWidth();
    UI_PopHeight();
}

INTERNAL void UI_PushPadding(V2 padding) {
    assert(ui_state->padding_stack_size < UI_MAX_STACK);
    ui_state->padding_stack[ui_state->padding_stack_size++].padding = ui_state->auto_layout_state.padding;
    ui_state->auto_layout_state.padding = padding;
}

INTERNAL void UI_PopPadding() {
    assert(ui_state->padding_stack_size > 0);
    --ui_state->padding_stack_size;
    ui_state->auto_layout_state.padding = ui_state->padding_stack[ui_state->padding_stack_size].padding;
}

INTERNAL void UI_PushWidgetIndices() {
    assert(ui_state->child_widgets_size < UI_MAX_STACK);
    ui_state->child_widgets_stack[ui_state->child_widgets_size].start = ui_state->auto_layout_state.start;
    ui_state->child_widgets_stack[ui_state->child_widgets_size].end = ui_state->auto_layout_state.end;
    ui_state->child_widgets_size++;
    
    ui_state->auto_layout_state.start = ui_state->auto_layout_state.end;
}

INTERNAL void UI_IncrementWidgetIndices() {
    ui_state->auto_layout_state.end++;
}

INTERNAL void UI_PopWidgetIndices(){
    ui_state->group_mode_stack_size--;
    ui_state->auto_layout_state.start = ui_state->child_widgets_stack[ui_state->child_widgets_size].start;
    ui_state->auto_layout_state.end = ui_state->child_widgets_stack[ui_state->child_widgets_size].end;
}
#pragma warning(pop)