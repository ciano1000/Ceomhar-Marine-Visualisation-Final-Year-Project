// TODO(Cian): @App Temporary for demo
struct APP_Debug{
    OS_FileRead demo_read;
    // TODO(Cian): Temporary thing for demo
    Measurement measurements[13735];
};
global APP_Debug *debug;
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui_state = (UI_State*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(UI_State));
    *ui_state = {};
    // TODO(Cian): @APP push this to debug arena
    debug = (APP_Debug*)Memory_ArenaPush(&global_os->permanent_arena, sizeof(APP_Debug));
    
    global_os->DebugReadEntireFile("D:\\dev\\fyp_ceomhar\\FSS_NMEA_SampleData.txt", &debug->demo_read);
    Parser_DebugParseMeasurements(debug->demo_read, debug->measurements);
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
        static b32 is_started = false;
        UI_WidthFill UI_HeightFill UI_Col {
            UI_Padding2(v2(10,10)) UI_HeightAuto UI_WidthFill UI_Panel(PRIMARY_COLOR_DARK, 
                                                                       "Navigation") {
                //Both of these buttons below are actually reference the same button since they use the same hash string
                UI_HeightAuto UI_WidthAuto {
                    if(!is_started){
                        if(UI_Button("Start###replacement_hash")) {
                            is_started = true;
                        }
                    }
                    else {
                        if(UI_Button("Stop###replacement_hash")) {
                            is_started = false;
                        }
                    }
                    UI_Filler(1);
                    UI_Label("Dashboard Demo Monday 22 Feb");
                    UI_Filler(1);
                }
            }
            UI_Row {
                UI_Padding2(v2(10,10)) UI_HeightFill UI_WidthFill UI_Panel(PRIMARY_COLOR_LIGHT, "MainPanel##hash") {
                    UI_HeightFill UI_WidthFill UI_Col {
                        UI_Filler(1);
                        UI_HeightAuto UI_Row {
                            UI_Height(500, 1.0f) UI_WidthRatio(0.32f, 1.0f) {
                                UI_TestBox(HIGHLIGHT_COLOR, "TestBox");
                                UI_Filler(1);
                                UI_TestBox(HIGHLIGHT_COLOR, "TestBox1");
                                UI_Filler(1);
                                UI_TestBox(HIGHLIGHT_COLOR, "TestBox2");
                                
                            }
                        }
                        UI_Filler(1);
                    }
                }
                UI_Padding2(v2(10,10)) UI_HeightFill UI_Width(450,0.9f) UI_Panel(PRIMARY_COLOR, "SidePanel") {
                    UI_HeightAuto UI_WidthFill UI_Col {
                        UI_Row {
                            UI_Filler(1);
                            UI_Label(20.0f, "Options");
                            UI_Filler(1);
                        }
                    }
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
