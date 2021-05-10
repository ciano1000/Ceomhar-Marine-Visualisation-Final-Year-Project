internal void data_add_entry(Data_Manufacturer manufacturer, Data_Key key, Data_Value_Entry entry) {
    Data_Table_Bucket *bucket = core->table.last_bucket;
    
    if(bucket->num_entries >= DATA_TABLE_BUCKET_SIZE) {
        //create a new bucket
        // TODO(Cian): check the free list first for a free bucket
        // TODO(Cian): allocate off a data table memory arena
        bucket->next_bucket = (Data_Table_Bucket *)memory_arena_push(&os->permanent_arena, sizeof(Data_Table_Bucket));
        core->table.first_bucket = bucket->next_bucket;
        bucket = bucket->next_bucket;
    }
    
    u32 bucket_idx = bucket->num_entries;
    u32 timestamp = core->table.current_timestamp;
    bucket->timestamps[bucket_idx] = timestamp; 
    bucket->latest_timestamp = timestamp;
    if(bucket->num_entries == 0)
        bucket->first_timestamp = timestamp;
    bucket->manufacturers[bucket_idx] = manufacturer; 
    bucket->keys[bucket_idx] = key; 
    bucket->entries[bucket_idx] = entry; 
    bucket->num_entries++; 
    core->table.total_entries++;
}