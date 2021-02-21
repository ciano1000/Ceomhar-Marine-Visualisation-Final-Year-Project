#define MEMORY_ARENA_MAX Gigabytes(4)
#define MEMORY_ARENA_COMMIT_SIZE Kilobytes(4)

struct MemoryArena 
{
	void *base;
    u64 max;
    u64 commit_pos;
    u64 allocation_pos;
};

internal MemoryArena Memory_ArenaInitialise();
internal void * Memory_ArenaPush(MemoryArena *arena, u64 size);
internal void Memory_ArenaPop(MemoryArena *arena, u64 size);
internal void Memory_ArenaClear(MemoryArena *arena);
internal void Memory_ArenaRelease(MemoryArena *arena);

#define Memory_ScopeBlock _DEFER_LOOP(null, Memory_ArenaClear(&global_os->scope_arena), UNIQUE_INT)
#define Memory_ScopePush(size) Memory_ArenaPush(&global_os->scope_arena, size)