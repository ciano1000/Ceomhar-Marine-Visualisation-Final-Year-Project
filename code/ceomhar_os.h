#define OS_MAX_EVENTS 4096

#define OS_DEFAULT_DPI 96.0f
#define OS_DEFAULT_DISPLAY_X 2560
#define OS_DEFAULT_DISPLAY_Y 1440
#define OS_DEFAULT_DISPLAY_DENSITY 3686400.0f

#define PLATFORM_RESERVE_MEMORY(name) void *##name(u64 size)
typedef PLATFORM_RESERVE_MEMORY(platform_reserve_memory);

#define PLATFORM_COMMIT_MEMORY(name) void name(void *memory, u64 size)
typedef PLATFORM_COMMIT_MEMORY(platform_commit_memory);

#define PLATFORM_DECOMMIT_MEMORY(name) void name(void* memory, u64 size)
typedef PLATFORM_DECOMMIT_MEMORY(platform_decommit_memory);

#define PLATFORM_RELEASE_MEMORY(name) void name(void* memory, u64 size)
typedef PLATFORM_RELEASE_MEMORY(platform_release_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) b32 name(char *file_path, OS_FileRead *file_read)

struct OS_FileRead {
    char *data;
    u64 size;
};
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file); 

struct AppDisplay 
{
    // TODO(Cian): Change this to float
    f32 width;
    f32 height;
    // TODO(Cian): split these into x & y
    u32 dpi;
    f32 pixel_ratio;
};

// TODO(Cian): Enum for different keys etc

enum OS_MouseButton {
    OS_MouseButton_Left,
    OS_MouseButton_Middle,
    OS_MouseButton_Right,
};

enum OS_EventType {
    OS_EventType_Null,
    OS_EventType_MouseMove,
    OS_EventType_MouseDown,
    OS_EventType_MouseUp,
    
    OS_EventType_MAX
};

struct OS_Event {
    OS_EventType type;
    OS_MouseButton mouse_button;
    // TODO(Cian): add keys etc
    V2 pos;
    V2 delta;
    V2 scroll;
};


/** NOTE(Cian): This struct should contain all OS specific info that the app layer needs
*   e.g fps, fullscreen, input, exe folder as well as OS functions for RW files, memory management etc
*/
struct OS_State 
{
	AppDisplay display;
    u64 current_time;
    // TODO(Cian): File IO function pointers
    u32 event_count;
    OS_Event events[OS_MAX_EVENTS];
    V2 mouse_pos;
    
    platform_reserve_memory *ReserveMemory;
    platform_commit_memory *CommitMemory;
    platform_decommit_memory *DecommitMemory;
    platform_release_memory *ReleaseMemory;
    // TODO(Cian): @OS only include in build if running in debug
    debug_platform_read_entire_file *DebugReadEntireFile;
    
    MemoryArena frame_arena;
    MemoryArena permanent_arena; 
    // NOTE(Cian): A scope arena has a lifetime of some local scope, useful for very temporary strings etc.
    MemoryArena scope_arena; 
};
