#pragma warning(push)
#pragma warning(disable: 4505)
namespace Memory {
    internal Memory_Arena arena_initialise() {
        Memory_Arena arena = {};
        arena.max = MEMORY_ARENA_MAX;
        arena.base = os->reserve_memory(MEMORY_ARENA_MAX);
        return arena;
    }
    
    internal void * arena_push(Memory_Arena *arena, u64 size) {
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
    
    internal void arena_pop(Memory_Arena *arena, u64 size) {
        if(size > arena->allocation_pos)
        {
            size = arena->allocation_pos;
        }
        arena->allocation_pos -= size;
    }
    
    // TODO(Cian): @Memory need to think about a deallocation strategy, current strategy is probably fine for the permanent arena and the frame arena since memory use is always likely to be very consistent, however the scratch arena could vary wildly
    internal void arena_clear(Memory_Arena *arena) {
        arena_pop(arena, arena->allocation_pos);
    }
    internal void arena_release(Memory_Arena *arena) { 
        os->release_memory(arena->base, arena->commit_pos);
    }
}
#pragma warning(pop)