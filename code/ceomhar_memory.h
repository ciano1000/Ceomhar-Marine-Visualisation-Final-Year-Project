struct MemoryArena 
{
	void *base;
    u64 size;
    u64 commit_pos;
    u64 allocation_pos;
};

INTERNAL MemoryArena Memory_ArenaInitialise(u64 size);
INTERNAL void * Memory_ArenaPush(MemoryArena *arena, u64 size);
INTERNAL void Memory_ArenaPop(MemoryArena *arena, u64 size);
INTERNAL void Memory_ArenaClear(MemoryArena *arena);