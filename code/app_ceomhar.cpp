
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui_state = (UI_State*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(UI_State));
    *ui_state = {};
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the row height, draw everything, and perform input
    
    glViewport( 0, 0, global_os->display.width, global_os->display.height);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);                                         
    nvgBeginFrame(global_vg, (f32)global_os->display.width,  (f32)global_os->display.height, 1);
    
    // NOTE(Cian): UI_Begin just creates a root Row container thats the same size as our display, containers can only have one child(for now maybe idk)
    UI_BeginUI
    {
        
        // NOTE(Cian): Panels auto-create a Row container(for padding etc), it's row fills its width
        UI_Width(800, 0.8f) UI_HeightFill UI_Padding2(v2(20, 20)) UI_Panel(PRIMARY_COLOR, "Test_Panel##test_append")
        {
            // NOTE(Cian): Blue
            UI_Padding2(v2(0,0))UI_WidthAuto UI_Height(100,1.0f){
                UI_HeightAuto UI_Row{
                    UI_Filler(1);
                    UI_Button("Open");
                    UI_Spacer(10.0f, 0.8f);
                    UI_Button("Open##2");
                    UI_Filler(1);
                }
                // NOTE(Cian): Purple
                UI_Filler(1);
                UI_TestBox(PRIMARY_COLOR_LIGHT, "Temp2");
                // NOTE(Cian): Yellow
                UI_TestBox(SECONDARY_COLOR, "Temp3");
                UI_TestBox(HIGHLIGHT_COLOR, "Temp4");
                UI_Filler(1);
            }
            
            /*UI_Height(400, 0.5f) UI_Row {
                UI_Width(100, 0.8f) UI_HeightAuto UI_Col {
                    UI_TestBox(nvgRGBA(152, 252, 3, 255), "Temp3");
                    UI_TestBox(nvgRGBA(52, 78, 3, 255), "Temp4");
                    UI_TestBox(nvgRGBA(152, 252, 67, 255), "Temp5");
                }
            }*/
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
