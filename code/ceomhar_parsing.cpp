internal u32 Parser_ParseExplicitTime(MemoryArena *arena, String time_string) {
    // NOTE(Cian): Time string is in the for HHMMSS.SS
    String hours_string = String_PushString(arena, 3);
    String_StringCopy(hours_string, time_string, 0, 3);
    u32 hours = String_DecimalStringToU32(hours_string);
    
    String minutes_string = String_PushString(arena, 3);
    String_StringCopy(minutes_string, time_string, 2, 3);
    u32 minutes = String_DecimalStringToU32(minutes_string);
    
    String seconds_string = String_PushString(arena, 3);
    String_StringCopy(seconds_string, time_string, 4, 3);
    u32 seconds = String_DecimalStringToU32(seconds_string);
    
    String centiseconds_string = String_PushString(arena, 3);
    String_StringCopy(centiseconds_string, time_string, 7, 3);
    u32 centiseconds = String_DecimalStringToU32(centiseconds_string);
    
    //convert to milliseconds
    return (hours * 60 * 60 * 1000) + (minutes * 60 * 1000) + (seconds * 1000) + (centiseconds * 10);
}

internal void Parser_DebugParseMeasurements(OS_FileRead file, Measurement *measurements) {
    u32 bytes_read = 0;
    u32 curr_measure = 0;
    String file_string = {};
    file_string.data = file.data;
    file_string.size = (u32)file.size;
    
    while(bytes_read < (file.size - 1)) {
        char curr_char = file.data[bytes_read];
        Measurement *curr_measurement = &debug->measurements[curr_measure];
        
        
        Memory_ScopeBlock {
            curr_measurement->message_header = String_StringTokenizer(&global_os->permanent_arena, file_string, ',', &bytes_read);
            
            
            //find out what measurement we are currently parsing!
            if(String_CompareStrings(measurement_tokens[MeasurementType_Sensor_1], curr_measurement->message_header)) {
                String time_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                curr_measurement->sensor_type = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String sensor_id_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                curr_measurement->measure_id = (char)(*String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read).data);
                String measure_val_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String measurement_quality_string = String_StringTokenizer(&global_os->scope_arena, file_string, '*', &bytes_read);
                String checksum_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                
                curr_measurement->type = MeasurementType_Sensor_1;
                // NOTE(Cian): @MarineInstitute how much time accuracy is needed? E.g. seconds, ms etc, is Sensor_1 even used?
                curr_measurement->relative_time_ms = String_HexStringToU32(time_string, HexString_BigEndian);
                curr_measurement->sensor_id = (u8)String_DecimalStringToU32(sensor_id_string);
                
                if(String_IsFloat(measure_val_string)) {
                    curr_measurement->measurement_is_float = true;
                    curr_measurement->measurement_f = String_FloatStringToF32(measure_val_string);
                } else {
                    curr_measurement->measurement_is_float = false;
                    curr_measurement->measurement_i = String_DecimalStringToU32(measure_val_string);
                }
                
                curr_measurement->measurement_quality = (u8)String_DecimalStringToU32(measurement_quality_string);
                curr_measurement->checksum = (u8)String_DecimalStringToU32(checksum_string);
            } else if(String_CompareStrings(measurement_tokens[MeasurementType_Sensor_2], curr_measurement->message_header)) {
                String time_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String status_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String sensor_type_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String sensor_id_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String measure_id_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String measure_val_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String measurement_quality_string = String_StringTokenizer(&global_os->scope_arena, file_string, '*', &bytes_read);
                String checksum_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                
                
                curr_measurement->type = MeasurementType_Sensor_2;
                // TODO(Cian): Parser for hhmmss.ss
                // NOTE(Cian): @MarineInstitute how much time accuracy is needed? E.g. seconds, ms etc, is Sensor_1 even used?
                curr_measurement->timestamp_ms = Parser_ParseExplicitTime(&global_os->scope_arena, time_string);
                if(status_string.data[0] == 'A')
                    curr_measurement->status = StatusIndicator_NewMeasurement;
                else
                    curr_measurement->status = StatusIndicator_ComputedMeasure;
                curr_measurement->sensor_type = sensor_type_string;
                curr_measurement->sensor_id = (u8)String_DecimalStringToU32(sensor_id_string);
                curr_measurement->measure_id = (char)(*measure_id_string.data);
                
                if(String_IsFloat(measure_val_string)) {
                    curr_measurement->measurement_is_float = true;
                    curr_measurement->measurement_f = String_FloatStringToF32(measure_val_string);
                } else {
                    curr_measurement->measurement_is_float = false;
                    curr_measurement->measurement_i = String_DecimalStringToU32(measure_val_string);
                }
                curr_measurement->measurement_quality = (u8)String_DecimalStringToU32(measurement_quality_string);
                curr_measurement->checksum = (u8)String_DecimalStringToU32(checksum_string);
            } else if(String_CompareStrings(measurement_tokens[MeasurementType_GeographicPos], curr_measurement->message_header)){
                String latitude_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String longitude_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String time_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String status_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String mode_string = String_StringTokenizer(&global_os->scope_arena, file_string, '*', &bytes_read);
                String checksum_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                
                curr_measurement->type = MeasurementType_GeographicPos;
                curr_measurement->latitude = String_FloatStringToF32(latitude_string);
                curr_measurement->longitude = String_FloatStringToF32(longitude_string);
                curr_measurement->timestamp_ms = Parser_ParseExplicitTime(&global_os->scope_arena, time_string);
                if(status_string.data[0] == 'A')
                    curr_measurement->status = StatusIndicator_Valid;
                else
                    curr_measurement->status = StatusIndicator_Invalid;
                if(mode_string.data[0] == 'A')
                    curr_measurement->mode = ModeIndicator_Auto;
                else
                    curr_measurement->mode = ModeIndicator_Manual;
                curr_measurement->checksum = (u8)String_DecimalStringToU32(checksum_string);
            }  else if(String_CompareStrings(measurement_tokens[MeasurementType_CourseSpeed], curr_measurement->message_header)){
                String true_course_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String magnetic_course_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String knts_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String km_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String mode_string = String_StringTokenizer(&global_os->scope_arena, file_string, '*', &bytes_read);
                String checksum_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                
                curr_measurement->type = MeasurementType_CourseSpeed;
                curr_measurement->true_course = String_FloatStringToF32(true_course_string);
                curr_measurement->magnetic_course = String_FloatStringToF32(magnetic_course_string);
                curr_measurement->ground_speed_knts = String_FloatStringToF32(knts_string);
                curr_measurement->ground_speed_km = String_FloatStringToF32(km_string);
                if(mode_string.data[0] == 'A')
                    curr_measurement->mode = ModeIndicator_Auto;
                else
                    curr_measurement->mode = ModeIndicator_Invalid;
                
                curr_measurement->checksum = (u8)String_DecimalStringToU32(checksum_string);
            }  else if(String_CompareStrings(measurement_tokens[MeasurementType_TimeDate], curr_measurement->message_header)){
                String time_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String day_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String month_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String year_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String timezone_hours_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String timezone_minutes_string = String_StringTokenizer(&global_os->scope_arena, file_string, '*', &bytes_read);
                String checksum_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                
                curr_measurement->type = MeasurementType_TimeDate;
                curr_measurement->timestamp_ms = Parser_ParseExplicitTime(&global_os->scope_arena, time_string);
                curr_measurement->day = (u8)String_DecimalStringToU32(day_string);
                curr_measurement->month = (u8)String_DecimalStringToU32(month_string);
                curr_measurement->year = (u16)String_DecimalStringToU32(year_string);
                
                curr_measurement->timezone_hours = (s8)String_DecimalStringToU32(timezone_hours_string);
                curr_measurement->timezone_minutes = (s8)String_DecimalStringToU32(timezone_hours_string);
                curr_measurement->checksum = (u8)String_DecimalStringToU32(checksum_string);
            } else if(String_CompareStrings(measurement_tokens[MeasurementType_Depth], curr_measurement->message_header)){
                String depth_feet_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String depth_meters_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String depth_fathoms_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String_StringTokenizer(&global_os->scope_arena, file_string, '*', &bytes_read);
                String checksum_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                
                curr_measurement->type = MeasurementType_Depth;
                curr_measurement->depth_feet = String_FloatStringToF32(depth_feet_string);
                curr_measurement->depth_meters = String_FloatStringToF32(depth_meters_string);
                curr_measurement->depth_fathoms = String_FloatStringToF32(depth_fathoms_string);
                curr_measurement->checksum = (u8)String_DecimalStringToU32(checksum_string);
            }
            else {
                assert(false);
                continue;
            }
        }
        curr_measure++;
    }
}