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
    ui_state->widget_size = 0;
    ui_state->auto_layout_state.pos = {0,0};
    ui_state->auto_layout_state.size = {0,0};
    // NOTE(Cian): Create the main window and add as parent
    // TODO(Cian): Make a Widget init function once we have more knowledge about different use cases
    UI_Widget *main_container = &ui_state->widgets[ui_state->widget_size++];
    main_container->layout = v4(0,0,(f32)global_os->display.width, (f32)global_os->display.height);
    main_container->color = nvgRGBA(255,255,255,255);
    
    
}

INTERNAL void UI_End() {
    // NOTE(Cian): Loop through everything, calculate layout and then process input
    *ui_state = {};
}

INTERNAL void UI_PushWidthConstraints(f32 min, f32 pref, f32 max) {
    u32 *size = &ui_state->pref_width_size;
    ui_state->pref_width_stack[*size].min = ui_state->auto_layout_state.width_constraints.min;
    ui_state->pref_width_stack[*size].pref = ui_state->auto_layout_state.width_constraints.pref;
    ui_state->pref_width_stack[*size].max = ui_state->auto_layout_state.width_constraints.max;
    *size++;
    ui_state->auto_layout_state.width_constraints.min = min;
    ui_state->auto_layout_state.width_constraints.pref = pref;
    ui_state->auto_layout_state.width_constraints.max = max;
}

INTERNAL void UI_PopWidthConstraints() {
    u32 *size = &ui_state->pref_width_size;
    ui_state->auto_layout_state.width_constraints.min = ui_state->pref_width_stack[*size].min;
    ui_state->auto_layout_state.width_constraints.pref = ui_state->pref_width_stack[*size].pref;
    ui_state->auto_layout_state.width_constraints.max = ui_state->pref_width_stack[*size].max;
    *size--;
}

INTERNAL void UI_PushRow() {
    
}

INTERNAL void UI_PopRow() {
    
}

INTERNAL void UI_PushPanel(V2 padding, NVGcolor fill_color, NVGcolor border_color) {
    
}

INTERNAL void UI_PushPanel(char *title, V2 padding) {
    
}

INTERNAL void UI_PopPanel() {
}
// TODO(Cian): Closures for button clicks
INTERNAL void UI_PushButton(char *label) {
    
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

INTERNAL void UI_PushPos(V2 pos) {
    UI_PushX(pos.x);
    UI_PushY(pos.y);
}

INTERNAL void UI_PopPos() {
    UI_PopX();
    UI_PopY();
}


INTERNAL void UI_TestBox(NVGcolor color) {
    // TODO(Cian): Once properties are properly implemented, use them here
    UI_Widget *widget = &ui_state->widgets[ui_state->widget_size];
    widget->width_constraints = ui_state->auto_layout_state.width_constraints;
    widget->height_constraints = ui_state->auto_layout_state.height_constraints;
    widget->color = color;
    ui_state->widget_size++;
}
#pragma warning(pop)