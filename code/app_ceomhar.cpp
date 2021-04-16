#include "utils.h"

#include "nano\nanovg.h"
#include "nano\nanovg.c"
#define STB_SPRINTF_IMPLEMENTATION
#include "stb\stb_sprintf.h"
#include "ceomhar_math.h"
#include "ceomhar_memory.h"
#include "ceomhar_string.h"
#include "ceomhar_os.h"

#include "ceomhar_ui.h"
#include "ceomhar_parsing.h"
#include "app_ceomhar.h"


#include "ceomhar_os.cpp"
#include "ceomhar_memory.cpp"
#include "ceomhar_string.cpp"
#include "ceomhar_ui.cpp"
#include "ceomhar_parsing.cpp"


// TODO(Cian): @App Temporary for demo
//void app_start(OS_State *state, NVGcontext *vg) {
external APP_START(app_start){
    os = state;
    
    vg_context = os->vg;
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    ui = (UI_State*)memory_arena_push(&os->permanent_arena, sizeof(UI_State));
    *ui = {};
    // TODO(Cian): @APP push this to debug arena
    debug = (App_Debug*)memory_arena_push(&os->permanent_arena, sizeof(App_Debug));
    
    //os->debug_read_entire_file("D:\\dev\\fyp_ceomhar\\FSS_NMEA_SampleData.txt", &debug->demo_read);
    //parsing_debug_parse_measurements(debug->demo_read, debug->measurements);
}

// TODO(Cian): How should we pass the vgContext???
external APP_UPDATE_AND_RENDER(app_update_and_render) {
    
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the ROW height, draw everything, and perform input
    
    nvgBeginFrame(vg_context, (f32)os->display.width,  (f32)os->display.height, os->display.pixel_ratio);
    
    UI {
        /* Target goal for tommorow, 2 windows with sorting order working, titles rendering, and close button working
UI_Window(pos, title) {

}

UI_Window(...) {

}
*/
        ui_begin_window(v4(100, 100, 200, 200), null, "Window");
        
        ui_end_window();
        
        ui_begin_window(v4(200, 200, 200, 200), null, "Window Numero Dos");
        
        ui_end_window();
    }
    
    
    nvgRestore(vg_context);
    
    nvgEndFrame(vg_context);
}
