#define SCANMAR_LINES 13736
#define MARPORT_LINES 2925

struct App_Debug{
    String8 scanmar_demo_read;
    String8 marport_demo_read;
    String8 scanmar_lines[SCANMAR_LINES];
    String8 marport_lines[MARPORT_LINES];
    
    u32 data_idx;
    u32 time_since_measure_update;
};

struct App_Core {
    App_Debug debug;
    Data_Table table;
    UI_State ui;
};