
INTERNAL MemoryArena Memory_ArenaInitialise() {
    MemoryArena arena = {};
    arena.max = MEMORY_ARENA_MAX;
    arena.base = global_os->ReserveMemory(MEMORY_ARENA_MAX);
    return arena;
}

INTERNAL void * Memory_ArenaPush(MemoryArena *arena, u64 size) {
    void *memory = 0;
    
    if((arena->allocation_pos + size) > arena->commit_pos)
    {
        u64 required = (arena->allocation_pos + size) - arena->commit_pos;
        required += MEMORY_ARENA_COMMIT_SIZE - 2;
        required += (required / MEMORY_ARENA_COMMIT_SIZE);
        required -= required % MEMORY_ARENA_COMMIT_SIZE;
        
        global_os->CommitMemory((u8 *)arena->base + arena->commit_pos, required);
        arena->commit_pos += required;
    }
    memory = (u8 *)arena->base + arena->allocation_pos;
    arena->allocation_pos += size;
    return memory;
}

INTERNAL void Memory_ArenaPop(MemoryArena *arena, u64 size) {
    if(size > arena->allocation_pos)
    {
        size = arena->allocation_pos;
    }
    arena->allocation_pos -= size;
}

INTERNAL void Memory_ArenaClear(MemoryArena *arena) {
    Memory_ArenaPop(arena, arena->allocation_pos);
}
INTERNAL void Memory_ArenaRelease(MemoryArena *arena) { 
    global_os->ReleaseMemory(arena->base, arena->commit_pos);
}