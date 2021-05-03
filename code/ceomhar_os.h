#define OS_MAX_EVENTS 4096

#define OS_DEFAULT_DPI 96.0f
#define OS_DEFAULT_DISPLAY_X 2560
#define OS_DEFAULT_DISPLAY_Y 1440

#define PLATFORM_RESERVE_MEMORY(name) void *##name(u64 size)
typedef PLATFORM_RESERVE_MEMORY(Platform_Reserve_Memory);

#define PLATFORM_COMMIT_MEMORY(name) void name(void *memory, u64 size)
typedef PLATFORM_COMMIT_MEMORY(Platform_Commit_Memory);

#define PLATFORM_DECOMMIT_MEMORY(name) void name(void* memory, u64 size)
typedef PLATFORM_DECOMMIT_MEMORY(Platform_Decommit_Memory);

#define PLATFORM_RELEASE_MEMORY(name) void name(void* memory, u64 size)
typedef PLATFORM_RELEASE_MEMORY(Platform_Release_Memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) b32 name(char *file_path, String8 *file_read)

struct OS_File_Read {
    char *data;
    u64 size;
};
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(Debug_Platform_Read_Entire_File); 

struct OS_App_Display 
{
    // TODO(Cian): Change this to float
    f32 width;
    f32 height;
    // TODO(Cian): split these into x & y
    u32 dpi;
    f32 pixel_ratio;
};

// TODO(Cian): Enum for different keys etc

enum OS_Mouse_Button {
    OS_Mouse_Button_Left,
    OS_Mouse_Button_Middle,
    OS_Mouse_Button_Right,
};

enum OS_Event_Type {
    OS_Event_Type_Null,
    OS_Event_Type_MouseMove,
    OS_Event_Type_MouseDown,
    OS_Event_Type_MouseUp,
    OS_Event_Type_MouseScroll,
    OS_Event_Type_MAX
};

struct OS_Event {
    OS_Event_Type type;
    OS_Mouse_Button mouse_button;
    // TODO(Cian): add keys etc
    V2 pos;
    V2 delta;
    s16 scroll_delta;
};


/** NOTE(Cian): This struct should contain all OS specific info that the app layer needs
*   e.g fps, fullscreen, input, exe folder as well as OS functions for RW files, memory management etc
*/
struct OS_State 
{
    b32 running;
    OS_App_Display display;
    u64 current_time;
    // TODO(Cian): File IO function pointers
    u32 event_count;
    OS_Event events[OS_MAX_EVENTS];
    V2 mouse_pos;
    b32 mouse_off_screen;
    
    Platform_Reserve_Memory *reserve_memory;
    Platform_Commit_Memory *commit_memory;
    Platform_Decommit_Memory *decommit_memory;
    Platform_Release_Memory *release_memory;
    // TODO(Cian): @OS only include in build if running in debug
    Debug_Platform_Read_Entire_File *debug_read_entire_file;
    
    Memory_Arena frame_arena;
    Memory_Arena permanent_arena; 
    // NOTE(Cian): A scope arena has a lifetime of some local scope, useful for very temporary strings etc.
    Memory_Arena scope_arena;
    
#ifndef CEOMHAR_DATAGEN
#ifndef CEOMHAR_SERVER
    NVGcontext *vg;
#endif
#endif
};


global OS_State *os = null;

#define external extern "C" __declspec(dllexport)

#define APP_START(name) void name(OS_State *state)
typedef APP_START(App_Start);
APP_START(app_start_stub) {
    
}

#define APP_UPDATE_AND_RENDER(name) void name()
typedef APP_UPDATE_AND_RENDER(App_Update_And_Render);
APP_UPDATE_AND_RENDER(app_update_and_render_stub) {
    
}

