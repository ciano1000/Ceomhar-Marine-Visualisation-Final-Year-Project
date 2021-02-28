#define OS_MAX_EVENTS 4096

#define OS_DEFAULT_DPI 96.0f
#define OS_DEFAULT_DISPLAY_X 2560
#define OS_DEFAULT_DISPLAY_Y 1440
#define OS_DEFAULT_DISPLAY_DENSITY 3686400.0f

#define PLATFORM_RESERVE_MEMORY(name) void *##name(u64 size)
typedef PLATFORM_RESERVE_MEMORY(Platform_Reserve_Memory);

#define PLATFORM_COMMIT_MEMORY(name) void name(void *memory, u64 size)
typedef PLATFORM_COMMIT_MEMORY(Platform_Commit_Memory);

#define PLATFORM_DECOMMIT_MEMORY(name) void name(void* memory, u64 size)
typedef PLATFORM_DECOMMIT_MEMORY(Platform_Decommit_Memory);

#define PLATFORM_RELEASE_MEMORY(name) void name(void* memory, u64 size)
typedef PLATFORM_RELEASE_MEMORY(Platform_Release_Memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) b32 name(char *file_path, OS::File_Read *file_read)

namespace OS {
    struct File_Read {
        char *data;
        u64 size;
    };
    typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(Debug_Platform_Read_Entire_File); 
    
    struct App_Display 
    {
        // TODO(Cian): Change this to float
        f32 width;
        f32 height;
        // TODO(Cian): split these into x & y
        u32 dpi;
        f32 pixel_ratio;
    };
    
    // TODO(Cian): Enum for different keys etc
    
    enum Mouse_Button {
        Mouse_Button_Left,
        Mouse_Button_Middle,
        Mouse_Button_Right,
    };
    
    enum Event_Type {
        Event_Type_Null,
        Event_Type_MouseMove,
        Event_Type_MouseDown,
        Event_Type_MouseUp,
        
        Event_Type_MAX
    };
    
    struct Event {
        Event_Type type;
        Mouse_Button mouse_button;
        // TODO(Cian): add keys etc
        V2 pos;
        V2 delta;
        V2 scroll;
    };
    
    
    /** NOTE(Cian): This struct should contain all OS specific info that the app layer needs
    *   e.g fps, fullscreen, input, exe folder as well as OS functions for RW files, memory management etc
    */
    struct State 
    {
        b32 running;
        App_Display display;
        u64 current_time;
        // TODO(Cian): File IO function pointers
        u32 event_count;
        Event events[OS_MAX_EVENTS];
        V2 mouse_pos;
        
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
    };
}

global OS::State *os = null;

#define APP_FUNCTION __declspec(dllexport)

// NOTE(Cian): Typedefs are for the platform(no declspec)
APP_FUNCTION void start(OS::State *os);
typedef void App_Start(OS::State *os);
internal void app_start_stub(OS::State *os);

APP_FUNCTION void update_and_render();
typedef void App_Update_And_Render();
internal void app_update_and_render_stub();

APP_FUNCTION void unload();
typedef void App_Unload();
internal void app_unload_stub(OS::State *os);

APP_FUNCTION void reload();
typedef void App_Reload();
internal void app_reload_stub(OS::State *os);



