
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui_state = (UI_State*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(UI_State));
    *ui_state = {};
    
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the row height, draw everything, and perform input
    
    glViewport( 0, 0, global_os->display.width, global_os->display.height);
    glClearColor(0.5f,0.5f,0.5f,1.5f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);                                         
    nvgBeginFrame(global_vg, (f32)global_os->display.width,  (f32)global_os->display.height, 1);
    
    // NOTE(Cian): UI_Begin just creates a root Row container thats the same size as our display, containers can only have one child(for now maybe idk)
    UI_BeginUI
    {
        
        UI_WidthFill
        {
            UI_TestBox("Temp1", nvgRGBA(123, 210, 70, 255));
            UI_TestBox("Temp2", nvgRGBA(210, 123, 70, 255));
            UI_TestBox("Temp3", nvgRGBA(70, 250, 150, 255));
        }
        // NOTE(Cian): Panels auto-create a Row container(for padding etc), it's row fills its width
        /*UI_WidthRatio(0.4f, 1.0f) UI_Panel("Test_Panel") {
            
        }*/
    }
    
    /*// NOTE(Cian): Naive way we're gonna do layout and input first
 Our UI calls above will create a nested tree of UI commands
UI_Layout {
   UI_Layout
}
*/
    
    nvgRestore(global_vg);
    
    nvgEndFrame(global_vg);
}
