void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    // NOTE(Cian): Stuff to go in UpdateAndRender
    {
        
        float *my_float = (float *)Memory_ArenaPush(&global_os->permanent_arena, sizeof(float));
        *my_float = 23.0f;
        
        glViewport( 0, 0, global_os->display.width, global_os->display.height);
        glClearColor(1.0f,1.0f,1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        
        nvgBeginFrame(global_vg, (f32)global_os->display.width,  (f32)global_os->display.height, global_os->display.pixel_ratio);
        
        // NOTE(Cian): NanoVG sample from Github, draws a RECT with a circle cut out
        {
            nvgBeginPath(global_vg);
            nvgRect(global_vg, 100,100, 120,30);
            nvgCircle(global_vg, 120,120, 5);
            nvgPathWinding(global_vg, NVG_HOLE);	// Mark circle as a hole.
            nvgFillColor(global_vg, nvgRGBA(255,192,0,255));
            nvgFill(global_vg);
        }
        
        // NOTE(Cian): Test Title element w/Nano
        nvgBeginPath(global_vg);
        //Just a background panel
        nvgRect(global_vg, 300, 50, 600, 600);
        nvgFillColor(global_vg, nvgRGBA(28, 30, 34, 192));
        nvgFill(global_vg);
        
        nvgFontSize(global_vg, 15.0f);
        nvgFontFace(global_vg, "sans-bold");
        // NOTE(Cian): Aligning to the left/right means that the left/right (e.g beginning of text/ end of text) is positioned at the given coordinates
        nvgTextAlign(global_vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
        nvgFillColor(global_vg, nvgRGBA(255,255,255,255));
        nvgText(global_vg, 600, 70, "This is a title", NULL);
        
        nvgRestore(global_vg);
        
        nvgEndFrame(global_vg);
    }
}