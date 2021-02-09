
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
        
        // NOTE(Cian): Panels auto-create a Row container(for padding etc), it's row fills its width
        UI_Width(800, 0.8f) UI_HeightFill UI_Panel("Test_Panel", nvgRGBA(50, 54, 57, 255))
        {
            // NOTE(Cian): Blue
            UI_WidthFill UI_HeightAuto{
                UI_TestBox("Temp1", nvgRGBA(3, 219, 252, 255));
                // NOTE(Cian): Purple
                UI_TestBox("Temp2", nvgRGBA(252, 3, 152, 255));
                
                // NOTE(Cian): Yellow
                UI_TestBox("Temp3", nvgRGBA(252, 252, 3, 255));
            }
        }
        
        
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
