#if CEOMHAR_INTERNAL
internal u32 parsing_time_string_to_seconds(Memory_Arena *arena, String8 time_string) {
    // NOTE(Cian): Time string is in the for HHMMSS.SS
    String8 hours_string = string_push(arena, 3);
    string_copy(hours_string, time_string, 0, 3);
    u32 hours = string_to_u32(hours_string);
    
    String8 minutes_string = string_push(arena, 3);
    string_copy(minutes_string, time_string, 2, 3);
    u32 minutes = string_to_u32(minutes_string);
    
    String8 seconds_string = string_push(arena, 3);
    string_copy(seconds_string, time_string, 4, 3);
    u32 seconds = string_to_u32(seconds_string);
    
    //ignoring the centiseconds as the sensors never seem to use them anyways
    //convert to seconds
    return (hours * 3600) + (minutes * 60) + (seconds);
}

internal void parsing_submit_data_entry(String8 entry_string) {
    
    Data_Manufacturer manufacturer = {};
    Data_Key key = {};
    Data_Value_Entry entry = {};
    
    Memory_ScopeBlock {
        u32 bytes_read = 0;
        String8 manufacturer_sentence_format = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
        
        if(string_contains(manufacturer_sentence_format, parsing_sentence_format_tokens[Data_Manufacturer_Marport])) {
            
            //ignoring type of gear, we dont need it
            string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            String8 sensor_location_string = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            //don't need the marport sensor node id
            string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            String8 data_type_string = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            //we will implicitly get the unit string by finding the data type
            string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            String8 value_string = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            
            Data_Sensor_Position pos = Data_Sensor_Position_MAX;
            for (u32 i = 0; i < Data_Sensor_Position_MAX; i += 1){
                String8 token = parsing_marport_sensor_position_tokens[i];
                if(string_compare(token, sensor_location_string)) {
                    pos = (Data_Sensor_Position)i;
                    break;
                }
            }
            
            Data_Value_Type type = Data_Value_Type_MAX; // by default just in case we dont match anything
            for (u32 i = 0; i < Data_Value_Type_MAX; i += 1){
                String8 token = parsing_marport_data_type_tokens[i];
                if(token.data == null)
                    continue;
                if(string_compare(token, data_type_string)) {
                    type = (Data_Value_Type)i;
                    break;
                }
            }
            
            manufacturer = Data_Manufacturer_Marport;
            key.value_type = type;
            key.marport_sensor_pos = pos;
            entry.unit = data_value_units[type];
            //for now just having all entries be f32
            entry.f_value = string_to_f32(value_string);
            
            data_add_entry(manufacturer, key, entry);
            
        } else { // only have two manufacturers
            
            manufacturer = Data_Manufacturer_Scanmar;
            // TODO(Cian): rememeber to return on ZDD event, we don't wish to push the time as data
            Parsing_Sentence_Format format = Parsing_Sentence_Format_MAX;
            for (u32 i = 0; i < Parsing_Sentence_Format_MAX; i += 1){
                String8 token = parsing_sentence_format_tokens[i];
                if(string_contains(manufacturer_sentence_format, token)) {
                    format = (Parsing_Sentence_Format)i;
                    break;
                }
            }
            
            String8 s2 = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            String8 s3 = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            String8 s4 = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            String8 s5 = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
            
            switch(format) {
                //sensor measurement
                case Parsing_Sentence_Format_SM2: {
                    String8 s6 = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
                    String8 s7 = string_tokenizer(&os->scope_arena, entry_string, ',', &bytes_read);
                    
                    //update the current time with this
                    u32 time = parsing_time_string_to_seconds(&os->scope_arena, s2);
                    
                    //ignoring s3
                    //s5 is sensor id
                    u32 sensor_id = string_to_u32(s5);
                    
                    // s4 & s6 is SensorType and Measure ID, used to decide what Data Value this is
                    Data_Value_Type type = Data_Value_Type_MAX;
                    for (u32 i = 0; i < Data_Value_Type_MAX; i += 1){
                        String8 sensor_type_token = parsing_scanmar_sensor_type_tokens[i];
                        char measure_type_token = parsing_scanmar_measure_tokens[i];
                        if(sensor_type_token.data == null)
                            continue;
                        
                        if(measure_type_token) {
                            if(string_compare(sensor_type_token, s4) && measure_type_token == s6.data[0]) {
                                type = (Data_Value_Type)i;
                                break;
                            }
                        } else {
                            if(string_compare(sensor_type_token, s4)) {
                                type = (Data_Value_Type)i;
                                break;
                            }
                        }
                    }
                    key.value_type = type;
                    key.scanmar_sensor_id = sensor_id;
                    entry.unit = data_value_units[type];
                    //for now just having all entries be f32
                    entry.f_value = string_to_f32(s7);
                    
                    //update the timestamp
                    core->table.current_timestamp = time;
                    
                    data_add_entry(manufacturer, key, entry);
                    
                } break;
                //course and speed
                case Parsing_Sentence_Format_VTG: {
                    key.value_type = Data_Value_Type_Course_True;
                    key.scanmar_sensor_id = 100; //no id for this data type, sensors id is in range 0-99 so no chance of a clash here
                    entry.unit = data_value_units[key.value_type];
                    //for now just having all entries be f32
                    entry.f_value = string_to_f32(s2);
                    
                    data_add_entry(manufacturer, key, entry);
                    
                    key.value_type = Data_Value_Type_Course_Magnetic;
                    entry.unit = data_value_units[key.value_type];
                    //for now just having all entries be f32
                    entry.f_value = string_to_f32(s3);
                    
                    data_add_entry(manufacturer, key, entry);
                    
                    key.value_type = Data_Value_Type_Speed;
                    entry.unit = data_value_units[key.value_type];
                    //for now just having all entries be f32
                    entry.f_value = string_to_f32(s5);
                    
                    data_add_entry(manufacturer, key, entry);
                } break;
                //Time and Date
                case Parsing_Sentence_Format_ZDA: {
                    // TODO(Cian): Use more date info like dd/mm/yy and timezone, time only is fine for now
                    u32 time = parsing_time_string_to_seconds(&os->scope_arena, s2);
                    core->table.current_timestamp = time;
                } break;
                //Seabed Depth
                case Parsing_Sentence_Format_DBS: {
                    key.value_type = Data_Value_Type_Depth;
                    key.scanmar_sensor_id = 100; //no id for this data type, sensors id is in range 0-99 so no chance of a clash here
                    entry.unit = data_value_units[key.value_type];
                    //for now just having all entries be f32
                    entry.f_value = string_to_f32(s3);
                    
                    data_add_entry(manufacturer, key, entry);
                } break;
                default: {
                    
                } break;
            }
        }
    }
}

internal void parsing_debug_demo_data_to_lines(String8 file, String8 *measurements) {
    u32 bytes_read = 0;
    u32 curr_measure = 0;
    String8 file_string = {};
    file_string.data = file.data;
    file_string.size = (u32)file.size;
    
    
    while(bytes_read < (file.size - 1)) {
        
        // NOTE(Cian): String tokenizer auto tokenizes the null terminator and line endings
        measurements[curr_measure] = string_tokenizer(&os->permanent_arena, file_string, null, &bytes_read);
        char curr_char = file.data[bytes_read];
        
        string_tokenizer(&os->permanent_arena, file_string, null, &bytes_read);
        
        curr_measure++;
    }
}
#endif