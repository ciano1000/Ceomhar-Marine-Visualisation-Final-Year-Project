
#if 0
INTERNAL f32 PixelsToDIP(float pixels) {
    // TODO(Cian): Fix rendering so that rounding isn't necessary
    return F32_ROUND(pixels / (global_os->display.dpi / UI_DEFAULT_DENSITY));
}
#endif

INTERNAL f32 DIPToPixels(float dp) {
    return F32_ROUND (dp  * (global_os->display.dpi / UI_DEFAULT_DENSITY));
}

INTERNAL void UI_BeginWindow() {
    // NOTE(Cian): Init state
    // TODO(Cian): ID hashing and stuff
    UI_Item window = {
        "", UI_AllSet(), (f32)global_os->display.width, (f32)global_os->display.height,
        0, 0, (f32)global_os->display.width, (f32)global_os->display.height
    };
    ui_state->parent = window;
    ui_state->current = {};
    
}

INTERNAL u32 UI_AllSet() {
    return (1UL << UI_LAYOUT_END) - 1UL;
}

INTERNAL void UI_StartToStartConstraint(char *id, f32 offset) {
    // TODO(Cian): Add ability to constrain based on ID instead of just the parent
    ui_state->current.x0 = ui_state->parent.x0;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_X0_SET;
}

#if 0
INTERNAL void UI_BottomToBottomConstraint(char *id, f32 offset) {
    // TODO(Cian): Add ability to constrain based on ID instead of just the parent
    ui_state->current.y1 = ui_state->parent.y1;
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_Y1_SET;
}
#endif


INTERNAL void UI_Width(f32 width) {
    ui_state->current.width = DIPToPixels(width);
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_WIDTH_SET;
}

INTERNAL void UI_Height(f32 height) {
    ui_state->current.height = DIPToPixels(height);
    ui_state->current.layout_flags = ui_state->current.layout_flags | UI_HEIGHT_SET;
}

INTERNAL void UI_Panel() {
    // TODO(Cian): add safety checks to ensure all necessary layout flags are set
    // TODO(Cian): check flags to see what values are there
    nvgBeginPath(global_vg);
    nvgRect(global_vg,  ui_state->current.x0, ui_state->current.y0, ui_state->current.width, ui_state->current.height);
    nvgFillColor(global_vg, nvgRGBA(40,40,40,255));
    nvgFill(global_vg);
}