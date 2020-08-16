#define PLATFORM_RESERVE_MEMORY(name) void *##name(u64 size)
typedef PLATFORM_RESERVE_MEMORY(platform_reserve_memory);

#define PLATFORM_COMMIT_MEMORY(name) void name(void *memory, u64 size)
typedef PLATFORM_COMMIT_MEMORY(platform_commit_memory);

#define PLATFORM_DECOMMIT_MEMORY(name) void name(void* memory, u64 size)
typedef PLATFORM_DECOMMIT_MEMORY(platform_decommit_memory);

#define PLATFORM_RELEASE_MEMORY(name) void name(void* memory, u64 size)
typedef PLATFORM_RELEASE_MEMORY(platform_release_memory);

struct AppDisplay 
{
    u32 width;
    u32 height;
    f32 dpi_scale_x;
    f32 dpi_scale_y;
    f32 pixel_ratio;
};

struct AppState 
{
};

// TODO(Cian): Look at pulling this out to an os.h file later
/** NOTE(Cian): This struct should contain all OS specific info that the app layer needs
*   e.g fps, fullscreen, input, exe folder as well as OS functions for RW files, memory management etc
*/
struct OS_State 
{
	AppDisplay display;
    
    // TODO(Cian): File IO function pointers
    
    platform_reserve_memory *ReserveMemory;
    platform_commit_memory *CommitMemory;
    platform_decommit_memory *DecommitMemory;
    platform_release_memory *ReleaseMemory;
    
    MemoryArena frame_arena;
    MemoryArena permanent_arena; 
};
