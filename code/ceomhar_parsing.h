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

enum ModeIndicator {
    ModeIndicator_Auto,
    ModeIndicator_Manual,
    ModeIndicator_Invalid,
};

enum StatusIndicator {
    StatusIndicator_Valid,
    StatusIndicator_Invalid,
    StatusIndicator_NewMeasurement,
    StatusIndicator_ComputedMeasure,
};
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
    u32 relative_time_ms;
    
    // NOTE(Cian): Shared below
    StatusIndicator status;
    
    // NOTE(Cian): Unique to Sensor Measurement 2
    u32 timestamp_ms;
    
    
    // NOTE(Cian): Shared below
    ModeIndicator mode;
    
    // NOTE(Cian): Unique to GLL
    f32 latitude;
    f32 longitude;
    
    // NOTE(Cian): Unique to VTG
    f32 true_course;
    f32 magnetic_course;
    f32 ground_speed_knts;
    f32 ground_speed_km;
    
    // NOTE(Cian): Unique to ZDA
    u8 day;
    u8 month;
    u16 year;
    s8 timezone_hours;
    s8 timezone_minutes;
    
    // NOTE(Cian): DBS
    f32 depth_feet;
    f32 depth_meters;
    f32 depth_fathoms;
};
internal void Parser_DebugParseMeasurements(OS_FileRead file, Measurement *measurements);