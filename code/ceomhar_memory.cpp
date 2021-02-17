#pragma warning(push)
#pragma warning(disable: 4505)
internal MemoryArena Memory_ArenaInitialise() {
    MemoryArena arena = {};
    arena.max = MEMORY_ARENA_MAX;
    arena.base = global_os->ReserveMemory(MEMORY_ARENA_MAX);
    return arena;
}

internal void * Memory_ArenaPush(MemoryArena *arena, u64 size) {
    void *memory = 0;
    
    if((arena->allocation_pos + size) > arena->commit_pos)
    {
        u64 required = size;
        required += MEMORY_ARENA_COMMIT_SIZE - 1;
        required -= required % MEMORY_ARENA_COMMIT_SIZE;
        
        global_os->CommitMemory((u8 *)arena->base + arena->commit_pos, required);
        arena->commit_pos += required;
    }
    memory = (u8 *)arena->base + arena->allocation_pos;
    arena->allocation_pos += size;
    return memory;
}

internal void Memory_ArenaPop(MemoryArena *arena, u64 size) {
    if(size > arena->allocation_pos)
    {
        size = arena->allocation_pos;
    }
    arena->allocation_pos -= size;
}

// TODO(Cian): @Memory need to think about a deallocation strategy, current strategy is probably fine for the permanent arena and the frame arena since memory use is always likely to be very consistent, however the scratch arena could vary wildly
internal void Memory_ArenaClear(MemoryArena *arena) {
    Memory_ArenaPop(arena, arena->allocation_pos);
}
internal void Memory_ArenaRelease(MemoryArena *arena) { 
    global_os->ReleaseMemory(arena->base, arena->commit_pos);
}
#pragma warning(pop)