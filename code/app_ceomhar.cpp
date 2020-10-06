
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui_state = (UI_State*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(UI_State));
    *ui_state = {};
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the row height, draw everything, and perform input
    
    UI_Begin();
    {
        // TODO(Cian): Instead of hardcoding panels/rows to fill width, make it a method
        // TODO(Cian): Overloaded panel methods that take pos and sizing
        UI_PushPanel(v2(2,2));
        {
            
            // NOTE(Cian): Buttons dimensions aare determined by font size, text length & padding
            UI_PushButton();
            UI_PushButton();
            UI_PushButton();
            
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
        UI_PopPanel();
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
