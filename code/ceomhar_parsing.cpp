enum MeasurementType {
#define MeasurementTypeDef(type, token) MeasurementType_##type,
#include "MeasurementTypes.inc"
#undef MeasurementTypeDef
    MeasurementType_Max
};

static String measurement_tokens[MeasurementType_Max] = {
#define MeasurementTypeDef(type, token) {token, sizeof(token)},
#include "MeasurementTypes.inc"
#undef MeasurementTypeDef
};

// NOTE(Cian): @NMEA NMEA Spec Conventions
/*
  *    x.x    Variable length integer or float
*    xx_    Mandatory length deicmal digit, each x is a digit, e.g. xx -> 02, 10, 09 etc
*    hh_    Fixed length hex, leftmost is the MSB
*    c-c   Variable length text field, delimited by ,
*    cc_    Fixed text field, e.g. each c is a mandatory character
*/

// NOTE(Cian): @NMEA SensorMeasurement Spec
/*
   *       RelativeTime: 1/10s e.g. deciseconds, gonna store in seconds for now
$PSCMSM,hhhhhhhh,c-c,xx,c,x.x,xx*hh
Example: $PSCMSM, 00314A29,SYM,02,,0.2,12*hh<CR><LF>

$PSCMSM2,hhmmss.ss,A,c-c,xx,c,x.x,xx*hh<CR><LF>
 *           |      |
*           |      V
*           |      Status: A = new measurement, V = computed measure
*           V
*           Time of message transmission
*/
// TODO(Cian): @Parsing Gonna start with the "megastruct" approach at first and then pull things out into more efficient structures as needed
struct Measurement {
    MeasurementType type;
    String message_header;
    
    // NOTE(Cian): Sensor Measurement
    String sensor_type;
    u8 sensor_id;
    char measure_id;
    b32 measurement_is_float;
    f32 measurement_f;
    u32 measurement_i;
    u8 measurement_quality; // 00 is worst 15 is best
    u8 checksum;
    
    // TODO(Cian): Could probably store these two timestamps as one but for clarity seperating them rn
    // NOTE(Cian): Unique to Sensor Measurement 1
    u32 relative_time_deciseconds;
    
    // NOTE(Cian): Unique to Sensor Measurement 2
    u32 timestamp_ms;
    b32 status_new_measurement;
};

internal void Parser_DebugParseMeasurements(OS_FileRead file, Measurement *measurements) {
    u32 bytes_read = 0;
    u32 curr_measure = 0;
    String file_string = {};
    file_string.data = file.data;
    file_string.size = (u32)file.size;
    
    while(bytes_read < file.size) {
        char curr_char = file.data[bytes_read];
        Measurement curr_measurement = {};
        
        
        Memory_ScopeBlock {
            curr_measurement.message_header = String_StringTokenizer(&global_os->permanent_arena, file_string, ',', &bytes_read);
            String time_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
            
            //find out what measurement we are currently parsing!
            if(String_CompareStrings(measurement_tokens[MeasurementType_Sensor_1], curr_measurement.message_header)) {
                curr_measurement.type = MeasurementType_Sensor_1;
                
                // NOTE(Cian): @MarineInstitute how much time accuracy is needed? E.g. seconds, ms etc, is Sensor_1 even used?
                curr_measurement.relative_time_deciseconds = String_HexStringToU32(time_string, HexString_BigEndian);
                
                curr_measurement.sensor_type = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String sensor_id_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                curr_measurement.sensor_id = (u8)String_DecimalStringToU32(sensor_id_string);
                curr_measurement.measure_id = (char)(*String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read).data);
                
                String measure_val_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                if(String_IsFloat(measure_val_string)) {
                    curr_measurement.measurement_is_float = true;
                    curr_measurement.measurement_f = String_FloatStringToF32(measure_val_string);
                } else {
                    curr_measurement.measurement_is_float = false;
                    curr_measurement.measurement_i = String_DecimalStringToU32(measure_val_string);
                }
                
                String measurement_quality_string = String_StringTokenizer(&global_os->scope_arena, file_string, '*', &bytes_read);
                curr_measurement.measurement_quality = (u8)String_DecimalStringToU32(measurement_quality_string);
                
                
                String checksum_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                curr_measurement.checksum = (u8)String_DecimalStringToU32(checksum_string);
                
            } else if(String_CompareStrings(measurement_tokens[MeasurementType_Sensor_2], curr_measurement.message_header)) {
                curr_measurement.type = MeasurementType_Sensor_1;
                
                // TODO(Cian): Parser for hhmmss.ss
                // NOTE(Cian): @MarineInstitute how much time accuracy is needed? E.g. seconds, ms etc, is Sensor_1 even used?
                //curr_measurement.relative_time_deciseconds = String_HexStringToU32(time_string, HexString_BigEndian);
                
                String status_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                if(status_string.data[0] == 'A')
                    curr_measurement.status_new_measurement = true;
                else
                    curr_measurement.status_new_measurement = false;
                
                
                curr_measurement.sensor_type = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                String sensor_id_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                curr_measurement.sensor_id = (u8)String_DecimalStringToU32(sensor_id_string);
                curr_measurement.measure_id = (char)(*String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read).data);
                
                String measure_val_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                if(String_IsFloat(measure_val_string)) {
                    curr_measurement.measurement_is_float = true;
                    curr_measurement.measurement_f = String_FloatStringToF32(measure_val_string);
                } else {
                    curr_measurement.measurement_is_float = false;
                    curr_measurement.measurement_i = String_DecimalStringToU32(measure_val_string);
                }
                
                String measurement_quality_string = String_StringTokenizer(&global_os->scope_arena, file_string, '*', &bytes_read);
                curr_measurement.measurement_quality = (u8)String_DecimalStringToU32(measurement_quality_string);
                
                
                String checksum_string = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
                curr_measurement.checksum = (u8)String_DecimalStringToU32(checksum_string);
            } else {
                // NOTE(Cian): Ignore for now, just skipping to the end of the line till we handle these cases
                String_StringTokenizer(&global_os->scope_arena, file_string, '\n', &bytes_read);
                continue;
            }
        }
    }
}