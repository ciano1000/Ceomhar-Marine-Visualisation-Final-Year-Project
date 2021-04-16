#pragma warning(push)
#pragma warning(disable: 4505)
internal Memory_Arena memory_arena_initialise() {
    Memory_Arena arena = {};
    arena.max = MEMORY_ARENA_MAX;
    arena.base = os->reserve_memory(MEMORY_ARENA_MAX);
    return arena;
}

internal void * memory_arena_push(Memory_Arena *arena, u64 size) {
    void *memory = 0;
    
    if((arena->allocation_pos + size) > arena->commit_pos)
    {
        u64 required = size;
        required += MEMORY_ARENA_COMMIT_SIZE - 1;
        required -= required % MEMORY_ARENA_COMMIT_SIZE;
        
        os->commit_memory((u8 *)arena->base + arena->commit_pos, required);
        arena->commit_pos += required;
    }
    memory = (u8 *)arena->base + arena->allocation_pos;
    arena->allocation_pos += size;
    return memory;
}

internal void memory_arena_pop(Memory_Arena *arena, u64 size) {
    if(size > arena->allocation_pos)
    {
        size = arena->allocation_pos;
    }
    arena->allocation_pos -= size;
}

internal b32 memory_move(void *dest, void *src, u32 size) {
    b32 res = true;
    
    u8 *curr_dest = (u8*)dest;
    u8 *curr_src = (u8*)src;
    
    if(curr_dest == null || curr_src == null || (curr_dest + size - 1) == null || (curr_src + (size - 1)) == null) {
        res = false;
        return res;
    } else {
        //overlapping - copy backwards
        if(curr_src < curr_dest && curr_dest < (curr_src + size - 1)) {
            for (u32 i = (size - 1); i >= 0; i--){
                
                *(curr_dest + i) = *(curr_src + i);
            }
        } else { //copy forwards
            for (u32 i = 0; i < size; i += 1){
                
                *(curr_dest + i) = *(curr_src + i);
            }
        }
    }
    
    return res;
}

// TODO(Cian): @Memory need to think about a deallocation strategy, current strategy is probably fine for the permanent arena and the frame arena since memory use is always likely to be very consistent, however the scratch arena could vary wildly
internal void memory_arena_clear(Memory_Arena *arena) {
    memory_arena_pop(arena, arena->allocation_pos);
}
internal void memory_arena_release(Memory_Arena *arena) { 
    os->release_memory(arena->base, arena->commit_pos);
}
#pragma warning(pop)