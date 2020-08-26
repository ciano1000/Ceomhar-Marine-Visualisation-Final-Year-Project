#define MEMORY_ARENA_MAX Gigabytes(4)
#define MEMORY_ARENA_COMMIT_SIZE Kilobytes(4)

struct MemoryArena 
{
	void *base;
    u64 max;
    u64 commit_pos;
    u64 allocation_pos;
};

INTERNAL MemoryArena Memory_ArenaInitialise();
INTERNAL void * Memory_ArenaPush(MemoryArena *arena, u64 size);
INTERNAL void Memory_ArenaPop(MemoryArena *arena, u64 size);
INTERNAL void Memory_ArenaClear(MemoryArena *arena);
INTERNAL void Memory_ArenaRelease(MemoryArena *arena);