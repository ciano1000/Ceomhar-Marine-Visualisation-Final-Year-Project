#define _USE_MATH_DEFINES
#include <cmath>
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
#include "ceomhar_data.h"
#include "ceomhar_parsing.h"

global NVGcontext *vg_context;

#include "app_ceomhar.h"
global App_Core *core;
global App_Debug *debug;

#include "ceomhar_os.cpp"
#include "ceomhar_memory.cpp"
#include "ceomhar_string.cpp"
#include "ceomhar_ui.cpp"
#include "ceomhar_data.cpp"
#include "ceomhar_parsing.cpp"



// TODO(Cian): @App Temporary for demo
//void app_start(OS_State *state, NVGcontext *vg) {
external APP_START(app_start){
    os = state;
    
    core = (App_Core*)memory_arena_push(&os->permanent_arena, sizeof(App_Core));
    //init data table
    core->table.last_bucket =  (Data_Table_Bucket*)memory_arena_push(&os->permanent_arena, sizeof(Data_Table_Bucket));
    core->table.first_bucket = core->table.last_bucket; 
    
    vg_context = os->vg;
    ui = &core->ui;
    debug = &core->debug;
    
    os->debug_read_entire_file("D:\\dev\\fyp_ceomhar\\FSS_NMEA_SampleData.txt", &debug->scanmar_demo_read);
    os->debug_read_entire_file("D:\\dev\\fyp_ceomhar\\Marport_NMEA_SampleData.txt", &debug->marport_demo_read);
    parsing_debug_demo_data_to_lines(debug->scanmar_demo_read, debug->scanmar_lines);
    parsing_debug_demo_data_to_lines(debug->marport_demo_read, debug->marport_lines);
    
    
    //Data Table stuff
}

// TODO(Cian): How should we pass the vgContext???
external APP_UPDATE_AND_RENDER(app_update_and_render) {
    
    debug->time_since_measure_update += os->delta_time;
    
    if(debug->time_since_measure_update >= 1000 && debug->data_idx < SCANMAR_LINES) {
        u32 cap = CLAMP_MAX(debug->data_idx + 8, SCANMAR_LINES);
        
        for(;debug->data_idx < cap; debug->data_idx++) {
            u32 marport_idx = debug->data_idx % MARPORT_LINES;
            String8 marport_entry = debug->marport_lines[marport_idx];
            String8 scanmar_entry = debug->scanmar_lines[debug->data_idx];
            
            parsing_submit_data_entry(scanmar_entry);
            parsing_submit_data_entry(marport_entry);
        }
        
        debug->time_since_measure_update = 0;
    }
    // NOTE(Cian): Begin creates a blank panel and sets up UI, at every UI_End or UI_Pop we do some very simple auto layout, e.g fit everything to the the ROW height, draw everything, and perform input
    
    nvgBeginFrame(vg_context, (f32)os->display.width, (f32)os->display.height, os->display.pixel_ratio);
    
    UI {
        /* Target goal for tommorow, 2 windows with sorting order working, titles rendering, and close button working
UI_Window(pos, title) {

}

UI_Window(...) {

}
*/
        ui_begin_window(v4(0, 0, UI_WINDOW_RATIO_WIDTH(0.3f), 300), null, UI_ContainerOptions_NoResize | UI_ContainerOptions_NoMove,"Window");
        {
        }
        ui_end_window();
        static b32 open = true;
        ui_begin_window(v4(200, 200, 100, 300), &open, null,"Window Numero Dos");
        {
            /*ui_button("Test_Button");
            ui_button("Another_Button");
            Filler(1);
            ui_button("Test_Button_3");
            ui_button("Test_Button_4"); */
            Width(200, 400, UI_MAX_SIZE) Height(200, 400, UI_MAX_SIZE) {
                ui_begin_plot(UI_PlotType_Graph, "Time", true, "Trawl Speed", false, "Plot");
                static V2 point = {3600,20};
                ui_plot_points(UI_Plot_PointStyle_Points, &point, {1800,0}, {7200, 40}, 1);
                ui_end_plot();
            }
        }
        ui_end_window();
        
        static V2 size_pos_1 = {300, 0};
        static V2 size_pos_2 = {300, 0};
        ui_begin_split_pane(v4(1000, 100, 600, 800), true, 200, &size_pos_1, 200, &size_pos_2, null, "###Splitter");
        ui_begin_window(v4(size_pos_1.pos, 100, size_pos_1.size, 800), null, UI_ContainerOptions_NoResize | UI_ContainerOptions_NoMove, "Split_1");
        ui_end_window();
        ui_begin_window(v4(size_pos_2.pos, 100, size_pos_2.size, 800), null, UI_ContainerOptions_NoResize | UI_ContainerOptions_NoMove, "Split_2");
        ui_end_window();
        ui_end_split_pane();
    }
    
    
    nvgRestore(vg_context);
    
    nvgEndFrame(vg_context);
}
