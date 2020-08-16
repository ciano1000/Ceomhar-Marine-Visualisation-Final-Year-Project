/* TODO(Cian): Determine how to handle global state so that the platform
* function pointers don't need to be passed like this
*/
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