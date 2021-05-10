/* date = May 10th 2021 10:53 am */

#ifndef CEOMHAR_DATA_H
#define CEOMHAR_DATA_H

#define DATA_MANUFACTURER_LIST \
ADD_MANUFACTURER(Marport, "$MP")\
ADD_MANUFACTURER(Scanmar, "$PSCM")

enum Data_Manufacturer {
#define ADD_MANUFACTURER(manufacturer, token) Data_Manufacturer_##manufacturer,
    DATA_MANUFACTURER_LIST
#undef ADD_MANUFACTURER
    Data_Manufacturer_MAX
};

static String8 data_manufacturer_names[Data_Manufacturer_MAX] {
#define ADD_MANUFACTURER(manufacturer, token) {#manufacturer, sizeof(#manufacturer)},
    DATA_MANUFACTURER_LIST
#undef ADD_MANUFACTURER
};

#define DATA_TABLE_BUCKET_SIZE 4096

// enum name,
#define DATA_UNITS_LIST \
ADD_UNIT(Meter, "m")\
ADD_UNIT(Kilo, "kg")\
ADD_UNIT(Kilometer, "km/h")\
ADD_UNIT(Non_SI, null)\
ADD_UNIT(Boolean, null)\
ADD_UNIT(Speed, "m/s")\
ADD_UNIT(Celcius, " degrees")\
ADD_UNIT(Angle, " degrees")\
ADD_UNIT(Percentage, "%")\

enum Data_Unit {
#define ADD_UNIT(unit, string) Data_Unit_##unit,
    DATA_UNITS_LIST
#undef ADD_UNIT
    Data_Unit_MAX
};

static String8 data_unit_strings[Data_Unit_MAX]{
#define ADD_UNIT(unit, string) {string, sizeof(string)},
    DATA_UNITS_LIST
#undef ADD_UNIT
};

// TODO(Cian): Add some of the non-sensor specific Scanmar data like GPS, Seabed Depth, Speed/Heading etc
// TODO(Cian): Might add manufacturer types here so I can filter selection drop downs based on it, leave it for now
// NOTE(Cian): This isn't all the possible value types, only included ones that were in the demo data 
#define DATA_VALUE_TYPE_LIST \
ADD_VALUE_TYPE(Distance, "Distance", Data_Unit_Meter, "DST", null, null) \
ADD_VALUE_TYPE(Trawl_Eye_Height, "Trawl Eye Height", Data_Unit_Meter, "TEY", 'H', null) \
ADD_VALUE_TYPE(Trawl_Eye_Opening, "Trawl Eye Opening", Data_Unit_Meter, "TEY", 'O', null) \
ADD_VALUE_TYPE(Trawl_Eye_Clearance, "Trawl Eye Clearance", Data_Unit_Meter, "TEY", 'C', null) \
ADD_VALUE_TYPE(Trawl_Eye_Fish_Density, "Trawl Eye Fish Density", Data_Unit_Non_SI, "TEY", 'F', null) \
ADD_VALUE_TYPE(Trawl_Sounder_Openining, "Trawl Sounder Opening", Data_Unit_Meter,  "TS", 'O', null) \
ADD_VALUE_TYPE(Trawl_Sounder_Clearance, "Trawl Sounder Clearance", Data_Unit_Meter, "TS", 'C', null) \
ADD_VALUE_TYPE(Trawl_Sounder_Fish_Density, "Trawl Sounder Fish Density", Data_Unit_Non_SI, "TS", 'F', null) \
ADD_VALUE_TYPE(Trawl_Speed_Across, "Across Trawl Speed", Data_Unit_Speed, "TSP", 'X', null) \
ADD_VALUE_TYPE(Trawl_Speed_Along, "Along Trawl Speed", Data_Unit_Speed, "TSP", 'Y', null) \
ADD_VALUE_TYPE(Depth, "Depth", Data_Unit_Meter, null, null, "DPT") \
ADD_VALUE_TYPE(Catch, "Catch", Data_Unit_Percentage, null, null, "CAT") \
ADD_VALUE_TYPE(Pitch, "Pitch", Data_Unit_Angle, null, null, "PIT") \
ADD_VALUE_TYPE(Roll, "Roll", Data_Unit_Angle, null, null, "ROL") \
ADD_VALUE_TYPE(Temperature, "Temperature", Data_Unit_Celcius, null, null, "TMP") \
ADD_VALUE_TYPE(Spread_Starboard, "Spread Starboard", Data_Unit_Meter, null, null, "XST") \
ADD_VALUE_TYPE(Spread_Clump, "Spread Clump", Data_Unit_Meter, null, null, "XCL") \
ADD_VALUE_TYPE(Spread_Port, "Spread Port", Data_Unit_Meter, null, null, "XPT") \
ADD_VALUE_TYPE(Battery, "Batery", Data_Unit_Percentage, null, null, "BAT") \
ADD_VALUE_TYPE(Speed_Along, "Speed Along", Data_Unit_Speed, null, null, "SPL") \
ADD_VALUE_TYPE(Speed_Across, "Speed Across", Data_Unit_Speed, null, null, "SPX") \
ADD_VALUE_TYPE(Distance_To_Bottom, "Seabed Distance", Data_Unit_Meter, null, null, "DTB") \
ADD_VALUE_TYPE(Opening, "Vertical/Horizontal Opening", Data_Unit_Meter, null, null, "OPN") \
ADD_VALUE_TYPE(Clearance, "Seabed Clearance", Data_Unit_Meter, null, null, "CLR") \
ADD_VALUE_TYPE(Slant_Distance, "Hydrophone Distance", Data_Unit_Meter, null, null, "SLD") \
ADD_VALUE_TYPE(Relative_Bearing, "Ship-Sensor Bearing Angle", Data_Unit_Angle, null, null, "RBR") \
ADD_VALUE_TYPE(True_Bearing, "True Ship-Sensor Bearing Angle", Data_Unit_Angle, null, null, "TBR") \
ADD_VALUE_TYPE(Course_True, "True Ship Course", Data_Unit_Angle, null, null, null) \
ADD_VALUE_TYPE(Course_Magnetic, "Magnetic Ship Course", Data_Unit_Angle, null, null, null) \
ADD_VALUE_TYPE(Speed, "Ship Speed Knots", Data_Unit_Kilometer, null, null, null) \

enum Data_Value_Type {
#define ADD_VALUE_TYPE(name, string, unit, scanmar_sensor, scanmar_measure, marport_token) Data_Value_Type_##name,
    DATA_VALUE_TYPE_LIST
#undef ADD_VALUE_TYPE
    Data_Value_Type_MAX
};

static String8 data_value_type_strings[Data_Value_Type_MAX]{
#define ADD_VALUE_TYPE(name, string, unit, scanmar_sensor, scanmar_measure, marport_token) {string, sizeof(string)},
    DATA_VALUE_TYPE_LIST
#undef ADD_VALUE_TYPE
};

static Data_Unit data_value_units[Data_Value_Type_MAX]{
#define ADD_VALUE_TYPE(name, string, unit, scanmar_sensor, scanmar_measure, marport_token) unit,
    DATA_VALUE_TYPE_LIST
#undef ADD_VALUE_TYPE
};

#define DATA_MARPORT_SENSOR_LOCATIONS \
ADD_LOC(Port_Door, "Port Door", "PD") \
ADD_LOC(Starboard_Door, "Starboard Door", "SD") \
ADD_LOC(Port_Wing, "Port Wing", "PW") \
ADD_LOC(Starboard_Wing, "Starboard Wing", "SW") \
ADD_LOC(Headrope, "Headrope", "HR") \
ADD_LOC(Footrope, "Footrope", "FR") \
ADD_LOC(Body, "Body", "BO") \
ADD_LOC(Cod_End, "Cod-End", "CE") 

enum Data_Sensor_Position {
#define ADD_LOC(position, string, token) Data_Value_Type_##position,
    DATA_MARPORT_SENSOR_LOCATIONS
#undef ADD_LOC
    Data_Sensor_Position_MAX
};

static String8 data_marport_sensor_position_string[Data_Sensor_Position_MAX] {
#define ADD_LOC(position, string, token) {string, sizeof(string)},
    DATA_MARPORT_SENSOR_LOCATIONS
#undef ADD_LOC
};
// NOTE(Cian): Scanmar Key
//sensor type - identifying text string
//sensor id - integer between 0-99
//MeasID - single char that identifies the Measurements name and unit type - replace this with our generalised list of measurement types
//lets just have it so that our key is the Value_Type we mapped the raw data too during parsing combined with the sensor id

// NOTE(Cian): Marport Key
//sensor location - 2 char string
// gear position - only one gear in demo data so not needed
//sensor id -  not necessary to uniquely identify entries
//lets just have it so that our key is the Value_Type we mapped the raw data too during parsing combined with the sensor location

struct Data_Key {
    Data_Value_Type value_type;
    union {
        // TODO(Cian): Marport - sensor location enum and backing strings - should come from Parsing file as I'll need this stuff for that
        Data_Sensor_Position marport_sensor_pos;
        u32 scanmar_sensor_id; //Scanmar
    };
};

struct Data_Value_Entry {
    //don't *need* this here but it's a nice to have
    Data_Unit unit;
    union {
        f32 f_value;
        s32 i_value;
    };
};

//when all values in a bucket are this much time behind the latest time we clear this bucket and add it to a free list
#define DATA_BUCKET_EXPIRATION 28800

struct Data_Table_Bucket {
    u32 num_entries;
    u32 latest_timestamp;
    u32 first_timestamp; //use this to help determine if this bucket should be added to the free list
    
    u32 timestamps[DATA_TABLE_BUCKET_SIZE];
    Data_Manufacturer manufacturers[DATA_TABLE_BUCKET_SIZE];
    Data_Key keys[DATA_TABLE_BUCKET_SIZE];
    Data_Value_Entry entries[DATA_TABLE_BUCKET_SIZE];
    
    Data_Table_Bucket *next_bucket;
};

struct Data_Table {
    u32 current_timestamp;
    u32 total_entries;
    Data_Table_Bucket *first_bucket;
    Data_Table_Bucket *last_bucket;
    Data_Table_Bucket *bucket_free_list;
};

#endif //CEOMHAR_DATA_H