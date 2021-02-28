namespace Parsing {
    
    internal u32 parse_explicit_time(Memory_Arena *arena, String8 time_string) {
        // NOTE(Cian): Time string is in the for HHMMSS.SS
        String8 hours_string = String::push_string(arena, 3);
        String::copy(hours_string, time_string, 0, 3);
        u32 hours = String::to_u32(hours_string);
        
        String8 minutes_string = String::push_string(arena, 3);
        String::copy(minutes_string, time_string, 2, 3);
        u32 minutes = String::to_u32(minutes_string);
        
        String8 seconds_string = String::push_string(arena, 3);
        String::copy(seconds_string, time_string, 4, 3);
        u32 seconds = String::to_u32(seconds_string);
        
        String8 centiseconds_string = String::push_string(arena, 3);
        String::copy(centiseconds_string, time_string, 7, 3);
        u32 centiseconds = String::to_u32(centiseconds_string);
        
        //convert to milliseconds
        return (hours * 60 * 60 * 1000) + (minutes * 60 * 1000) + (seconds * 1000) + (centiseconds * 10);
    }
    
    internal void debug_parse_measurements(OS::File_Read file, Measurement *measurements) {
        u32 bytes_read = 0;
        u32 curr_measure = 0;
        String8 file_string = {};
        file_string.data = file.data;
        file_string.size = (u32)file.size;
        
        while(bytes_read < (file.size - 1)) {
            char curr_char = file.data[bytes_read];
            Measurement *curr_measurement = &debug->measurements[curr_measure];
            
            
            Memory_ScopeBlock {
                curr_measurement->message_header = String::tokenizer(&os->permanent_arena, file_string, ',', &bytes_read);
                
                
                //find out what measurement we are currently parsing!
                if(String::compare(measurement_tokens[MeasurementType_Sensor_1], curr_measurement->message_header)) {
                    String8 time_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    curr_measurement->sensor_type = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 sensor_id_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    curr_measurement->measure_id = (char)(*String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read).data);
                    String8 measure_val_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 measurement_quality_string = String::tokenizer(&os->scope_arena, file_string, '*', &bytes_read);
                    String8 checksum_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    
                    curr_measurement->type = MeasurementType_Sensor_1;
                    // NOTE(Cian): @MarineInstitute how much time accuracy is needed? E.g. seconds, ms etc, is Sensor_1 even used?
                    curr_measurement->relative_time_ms = String::hex_string_to_u32(time_string, String::HexString8_BigEndian);
                    curr_measurement->sensor_id = (u8)String::to_u32(sensor_id_string);
                    
                    if(String::is_float(measure_val_string)) {
                        curr_measurement->measurement_is_float = true;
                        curr_measurement->measurement_f = String::to_f32(measure_val_string);
                    } else {
                        curr_measurement->measurement_is_float = false;
                        curr_measurement->measurement_i = String::to_u32(measure_val_string);
                    }
                    
                    curr_measurement->measurement_quality = (u8)String::to_u32(measurement_quality_string);
                    curr_measurement->checksum = (u8)String::to_u32(checksum_string);
                } else if(String::compare(measurement_tokens[MeasurementType_Sensor_2], curr_measurement->message_header)) {
                    String8 time_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 status_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 sensor_type_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 sensor_id_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 measure_id_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 measure_val_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 measurement_quality_string = String::tokenizer(&os->scope_arena, file_string, '*', &bytes_read);
                    String8 checksum_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    
                    
                    curr_measurement->type = MeasurementType_Sensor_2;
                    // TODO(Cian): Parser for hhmmss.ss
                    // NOTE(Cian): @MarineInstitute how much time accuracy is needed? E.g. seconds, ms etc, is Sensor_1 even used?
                    curr_measurement->timestamp_ms = parse_explicit_time(&os->scope_arena, time_string);
                    if(status_string.data[0] == 'A')
                        curr_measurement->status = StatusIndicator_NewMeasurement;
                    else
                        curr_measurement->status = StatusIndicator_ComputedMeasure;
                    curr_measurement->sensor_type = sensor_type_string;
                    curr_measurement->sensor_id = (u8)String::to_u32(sensor_id_string);
                    curr_measurement->measure_id = (char)(*measure_id_string.data);
                    
                    if(String::is_float(measure_val_string)) {
                        curr_measurement->measurement_is_float = true;
                        curr_measurement->measurement_f = String::to_f32(measure_val_string);
                    } else {
                        curr_measurement->measurement_is_float = false;
                        curr_measurement->measurement_i = String::to_u32(measure_val_string);
                    }
                    curr_measurement->measurement_quality = (u8)String::to_u32(measurement_quality_string);
                    curr_measurement->checksum = (u8)String::to_u32(checksum_string);
                } else if(String::compare(measurement_tokens[MeasurementType_GeographicPos], curr_measurement->message_header)){
                    String8 latitude_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 longitude_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 time_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 status_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 mode_string = String::tokenizer(&os->scope_arena, file_string, '*', &bytes_read);
                    String8 checksum_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    
                    curr_measurement->type = MeasurementType_GeographicPos;
                    curr_measurement->latitude = String::to_f32(latitude_string);
                    curr_measurement->longitude = String::to_f32(longitude_string);
                    curr_measurement->timestamp_ms = parse_explicit_time(&os->scope_arena, time_string);
                    if(status_string.data[0] == 'A')
                        curr_measurement->status = StatusIndicator_Valid;
                    else
                        curr_measurement->status = StatusIndicator_Invalid;
                    if(mode_string.data[0] == 'A')
                        curr_measurement->mode = ModeIndicator_Auto;
                    else
                        curr_measurement->mode = ModeIndicator_Manual;
                    curr_measurement->checksum = (u8)String::to_u32(checksum_string);
                }  else if(String::compare(measurement_tokens[MeasurementType_CourseSpeed], curr_measurement->message_header)){
                    String8 true_course_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 magnetic_course_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 knts_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 km_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 mode_string = String::tokenizer(&os->scope_arena, file_string, '*', &bytes_read);
                    String8 checksum_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    
                    curr_measurement->type = MeasurementType_CourseSpeed;
                    curr_measurement->true_course = String::to_f32(true_course_string);
                    curr_measurement->magnetic_course = String::to_f32(magnetic_course_string);
                    curr_measurement->ground_speed_knts = String::to_f32(knts_string);
                    curr_measurement->ground_speed_km = String::to_f32(km_string);
                    if(mode_string.data[0] == 'A')
                        curr_measurement->mode = ModeIndicator_Auto;
                    else
                        curr_measurement->mode = ModeIndicator_Invalid;
                    
                    curr_measurement->checksum = (u8)String::to_u32(checksum_string);
                }  else if(String::compare(measurement_tokens[MeasurementType_TimeDate], curr_measurement->message_header)){
                    String8 time_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 day_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 month_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 year_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 timezone_hours_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 timezone_minutes_string = String::tokenizer(&os->scope_arena, file_string, '*', &bytes_read);
                    String8 checksum_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    
                    curr_measurement->type = MeasurementType_TimeDate;
                    curr_measurement->timestamp_ms = parse_explicit_time(&os->scope_arena, time_string);
                    curr_measurement->day = (u8)String::to_u32(day_string);
                    curr_measurement->month = (u8)String::to_u32(month_string);
                    curr_measurement->year = (u16)String::to_u32(year_string);
                    
                    curr_measurement->timezone_hours = (s8)String::to_u32(timezone_hours_string);
                    curr_measurement->timezone_minutes = (s8)String::to_u32(timezone_hours_string);
                    curr_measurement->checksum = (u8)String::to_u32(checksum_string);
                } else if(String::compare(measurement_tokens[MeasurementType_Depth], curr_measurement->message_header)){
                    String8 depth_feet_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 depth_meters_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String8 depth_fathoms_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    String::tokenizer(&os->scope_arena, file_string, '*', &bytes_read);
                    String8 checksum_string = String::tokenizer(&os->scope_arena, file_string, ',', &bytes_read);
                    
                    curr_measurement->type = MeasurementType_Depth;
                    curr_measurement->depth_feet = String::to_f32(depth_feet_string);
                    curr_measurement->depth_meters = String::to_f32(depth_meters_string);
                    curr_measurement->depth_fathoms = String::to_f32(depth_fathoms_string);
                    curr_measurement->checksum = (u8)String::to_u32(checksum_string);
                }
                else {
                    assert(false);
                    continue;
                }
            }
            curr_measure++;
        }
    }
}