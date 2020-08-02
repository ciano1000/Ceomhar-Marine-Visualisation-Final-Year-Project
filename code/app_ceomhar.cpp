// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender(AppState *app_state, AppMemory *app_memory, NVGcontext *vg) {
    // NOTE(Cian): Stuff to go in UpdateAndRender
    {
        glClearColor(1.0f,1.0f,1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        
        nvgBeginFrame(vg, (f32)app_state->display.width,  (f32)app_state->display.height, app_state->display.pixel_ratio);
        
        // NOTE(Cian): NanoVG sample from Github, draws a RECT with a circle cut out
        {
            nvgBeginPath(vg);
            nvgRect(vg, 100,100, 120,30);
            nvgCircle(vg, 120,120, 5);
            nvgPathWinding(vg, NVG_HOLE);	// Mark circle as a hole.
            nvgFillColor(vg, nvgRGBA(255,192,0,255));
            nvgFill(vg);
        }
        
        // NOTE(Cian): Test Title element w/Nano
        nvgBeginPath(vg);
        //Just a background panel
        nvgRect(vg, 300, 50, 600, 600);
        nvgFillColor(vg, nvgRGBA(28, 30, 34, 192));
        nvgFill(vg);
        
        nvgFontSize(vg, 15.0f);
        nvgFontFace(vg, "sans-bold");
        // NOTE(Cian): Aligning to the left/right means that the left/right (e.g beginning of text/ end of text) is positioned at the given coordinates
        nvgTextAlign(vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
        nvgFillColor(vg, nvgRGBA(255,255,255,255));
        nvgText(vg, 600, 70, "This is a title", NULL);
        
        nvgRestore(vg);
        
        nvgEndFrame(vg);
    }
}