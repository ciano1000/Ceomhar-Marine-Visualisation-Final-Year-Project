
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
        // TODO(Cian): Instead of hardcoding panels/rows to fill width, make it a method
        // TODO(Cian): Overloaded panel methods that take pos and sizing
        UI_WIDTH_FILL UI_HEIGHT(60) UI_PANEL(v2(6,6), nvgRGBA(20,22,25,255), nvgRGBA(108,170,33,255))
        {
            
            // NOTE(Cian): Buttons dimensions aare determined by font size, text length & padding
            UI_WIDTH_AUTO {
                UI_PushButton("Test1");
                UI_PushButton("Dashboard");
                UI_PushButton("Test3");
            }
#if 0
            UI_Center {
                UI_Label();
            }
            UI_Right {
                UI_PushButton();
                UI_PushButton();
            }
#endif
        }
        UI_HEIGHT((f32)global_os->display.height - 64) UI_WIDTH_FILL UI_POS(v2(0,62)) UI_PANEL(v2(6,6), nvgRGBA(11,12,14,255), nvgRGBA(56,162,214,255))
        {
            
        }
        
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
