
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui_state = (UI_State*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(UI_State));
    *ui_state = {};
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the row height, draw everything, and perform input
    
    glViewport( 0, 0, (u32)global_os->display.width, (u32)global_os->display.height);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);                                         
    nvgBeginFrame(global_vg, (f32)global_os->display.width,  (f32)global_os->display.height, 1);
    
    // NOTE(Cian): UI_Begin just creates a root Row container thats the same size as our display, containers can only have one child(for now maybe idk)
    UI_BeginUI
    {
        
        UI_WidthFill UI_HeightFill UI_Col {
            UI_Padding2(v2(10,10)) UI_HeightAuto UI_WidthFill UI_Panel(PRIMARY_COLOR_DARK, "Navigation") {
                UI_HeightAuto UI_WidthAuto {
                    UI_Button("Open###replacement_hash");
                    UI_Filler(1);
                    UI_Label("Dashboard Demo Monday 22 Feb");
                    UI_Filler(1);
                }
            }
            UI_Row {
                UI_Padding2(v2(10,10)) UI_HeightFill UI_WidthFill UI_Panel(PRIMARY_COLOR_LIGHT, "MainPanel##hash") {
                    
                }
                UI_Padding2(v2(10,10)) UI_HeightFill UI_Width(500,0.9f) UI_Panel(PRIMARY_COLOR, "SidePanel") {
                    
                }
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
