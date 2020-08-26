f32 PixelsToDIP(float pixels) {
    // TODO(Cian): Fix rendering so that rounding isn't necessary
    return F32_ROUND(pixels / (global_os->display.dpi / UI_DEFAULT_DENSITY));
}

f32 DIPToPixels(float dp) {
    return F32_ROUND (dp  * (global_os->display.dpi / UI_DEFAULT_DENSITY));
}