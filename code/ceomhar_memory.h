#define MEMORY_ARENA_MAX Gigabytes(4)
#define MEMORY_ARENA_COMMIT_SIZE Kilobytes(4)

struct Memory_Arena 
{
	void *base;
    u64 max;
    u64 commit_pos;
    u64 allocation_pos;
};

#define Memory_ScopeBlock _DEFER_LOOP(null, Memory::arena_clear(&global_os->scope_arena), UNIQUE_INT)
#define Memory_ScopePush(size) Memory::arena_push(&global_os->scope_arena, size)