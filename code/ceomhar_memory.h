struct MemoryArena 
{
	void *base;
    u64 size;
    u64 commit_pos;
    u64 allocation_pos;
};