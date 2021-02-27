// TODO(Cian): @App Temporary for demo
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui_state = (UI::State*)Memory::arena_push(&global_os->permanent_arena, sizeof(UI::State));
    *ui_state = {};
    // TODO(Cian): @APP push this to debug arena
    debug = (APP_Debug*)Memory::arena_push(&global_os->permanent_arena, sizeof(APP_Debug));
    
    global_os->debug_read_entire_file("D:\\dev\\fyp_ceomhar\\FSS_NMEA_SampleData.txt", &debug->demo_read);
    Parsing::debug_parse_measurements(debug->demo_read, debug->measurements);
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the ROW height, draw everything, and perform input
    
    glViewport( 0, 0, (u32)global_os->display.width, (u32)global_os->display.height);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);                                         
    nvgBeginFrame(global_vg, (f32)global_os->display.width,  (f32)global_os->display.height, 1);
    
    // NOTE(Cian): UI_Begin just creates a root Row container thats the same size as our display, containers can only have one child(for now maybe idk)
    using namespace UI;
    BEGIN_UI
    {
        static b32 is_started = false;
        WIDTH_FILL HEIGHT_FILL COL {
            PADDING2(v2(10,10)) HEIGHT_AUTO  WIDTH_FILL PANEL(PRIMARY_COLOR_DARK, 
                                                              "Navigation") {
                //Both of these buttons below are actually reference the same button since they use the same hash string
                HEIGHT_AUTO WIDTH_AUTO {
                    if(!is_started){
                        if(button("Start###replacement_hash")) {
                            is_started = true;
                        }
                    }
                    else {
                        if(button("Stop###replacement_hash")) {
                            is_started = false;
                        }
                    }
                    FILLER(1);
                    label("Dashboard Demo Monday 22 Feb");
                    FILLER(1);
                }
            }
            ROW {
                PADDING2(v2(10,10)) HEIGHT_FILL WIDTH_FILL PANEL(PRIMARY_COLOR_LIGHT, "MainPanel##hash") {
                    HEIGHT_FILL WIDTH_FILL COL {
                        FILLER(1);
                        HEIGHT_AUTO ROW {
                            HEIGHT(500, 1.0f) WIDTH_RATIO(0.32f, 1.0f) {
                                test_box(HIGHLIGHT_COLOR, "TestBox");
                                FILLER(1);
                                test_box(HIGHLIGHT_COLOR, "TestBox1");
                                FILLER(1);
                                test_box(HIGHLIGHT_COLOR, "TestBox2");
                            }
                        }
                        FILLER(1);
                    }
                }
                PADDING2(v2(10,10)) HEIGHT_FILL WIDTH(450,0.9f) PANEL(PRIMARY_COLOR, "SidePanel") {
                    HEIGHT_AUTO WIDTH_FILL COL {
                        ROW {
                            FILLER(1);
                            label(20.0f, "Options");
                            FILLER(1);
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
