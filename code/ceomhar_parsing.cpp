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
   *       RelativeTime: 1/10s e.g. deciseconds, milliseconds are 1/1000s, e.g. x10 for conversion
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
    char meaure_id;
    b32 measurement_is_float;
    f32 measurement_f;
    u32 measurement_i;
    u8 measurement_quality; // 00 is worst 15 is best
    u8 checksum;
    
    // TODO(Cian): Could probably store these two timestamps as one but for clarity seperating them rn
    // NOTE(Cian): Unique to Sensor Measurement 1
    u32 relative_time_ms;
    
    // NOTE(Cian): Unique to Sensor Measurement 2
    u32 timestamp_ms;
    b32 status_is_a;
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
            MeasurementType curr_type;
            if(String_CompareStrings(measurement_tokens[MeasurementType_Sensor_1], curr_measurement.sensor_type)) {
                curr_type = MeasurementType_Sensor_1;
                // TODO(Cian): correctly parse Hex time into a timestamp in ms
            } else if(String_CompareStrings(measurement_tokens[MeasurementType_Sensor_2], curr_measurement.sensor_type)) {
                curr_type = MeasurementType_Sensor_2;
                // TODO(Cian): correctly parse hhmmss.ss to a timestamp in ms
                //status field
                String status_text = String_StringTokenizer(&global_os->scope_arena, file_string, ',', &bytes_read);
            } else {
                // NOTE(Cian): Ignore for now
                continue;
            }
            curr_measurement.type = curr_type;
        }
    }
}