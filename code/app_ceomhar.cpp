
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui_state = (UI_State*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(ui_state));
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    // NOTE(Cian): Simplified UI
    // NOTE(Cian): Creates the main panel and inits all necessary stuff
#if 0
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the row height, draw everything, and perform input
    
    UI_Begin();
    {
        // NOTE(Cian): Rows start left-right by default, pushing a row creates a new row in the parent panel, panel and row height are determined by it's children if none specified
        UI_PushRow();
        {
            UI_PushPanel();// NOTE(Cian): Fills row with a panel, also pushes panel padding
            
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
            UI_PopPanel();
        }
        UI_PopRow();
    }
    UI_End();
    
    
#endif
    
    
    
    /*// NOTE(Cian): Naive way we're gonna do layout and input first
 Our UI calls above will create a nested tree of UI commands
UI_Layout {
   UI_Layout
}
*/
    
    nvgRestore(global_vg);
    
    nvgEndFrame(global_vg);
}
