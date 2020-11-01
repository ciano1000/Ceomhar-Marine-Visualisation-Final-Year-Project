
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui_state = (UI_State*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(UI_State));
    *ui_state = {};
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the row height, draw everything, and perform input
    
    glViewport( 0, 0, global_os->display.width, global_os->display.height);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);                                         
    nvgBeginFrame(global_vg, (f32)global_os->display.width,  (f32)global_os->display.height, 1);
    
    UI_Begin();
    {
        UI_WidthFill UI_Row {
            UI_WidthFill
                UI_TestBox(nvgRGBA(50, 168, 82,255));
            UI_Width(50, 200, 400)
                UI_TestBox(nvgRGBA(58, 50, 168,255));
            UI_WidthAuto
                UI_TestBox(nvgRGBA(168, 50, 137,255));
        }
        
        /*UI_Panel(v4(0, 0, ui_state->display.width, ui_state->display.height), nvgRGBA(20,22,25,255), nvgRGBA(108,170,33,255));
        {
            
            UI_FillWidth UI_Row {
                UI_PushButton("Test1");
                UI_PushButton("Dashboard");
                UI_PushButton("Test3");
                
                UI_Center {
                    // NOTE(Cian): Title that takes a va_arg
                    UI_Title("Some text %i", 12);
                }
                
                UI_Right {
                    UI_PushButton();
                    UI_PushButton();
                }
            }
        }
        UI_HEIGHT((f32)global_os->display.height - 64) UI_WIDTH_FILL UI_POS(v2(0,62)) UI_PANEL(v2(6,6), nvgRGBA(11,12,14,255), nvgRGBA(56,162,214,255))
        {
            
        }
*/
    }
    UI_End();
    
    /*// NOTE(Cian): Naive way we're gonna do layout and input first
 Our UI calls above will create a nested tree of UI commands
UI_Layout {
   UI_Layout
}
*/
    
    nvgRestore(global_vg);
    
    nvgEndFrame(global_vg);
}
