static String8 parsing_manufacturing_tokens[Data_Manufacturer_MAX] = {
#define ADD_MANUFACTURER(type, token) {token, sizeof(token)},
    DATA_MANUFACTURER_LIST
#undef ADD_MANUFACTURER
};

#define PARSING_SENTENCE_FORMAT_LIST \
ADD_SF(MSD)\
ADD_SF(SM2)\
ADD_SF(GLL)\
ADD_SF(VTG)\
ADD_SF(ZDA)\
ADD_SF(DBS)

enum Parsing_Sentence_Format {
#define ADD_SF(type) Parsing_Sentence_Format_##type,
    PARSING_SENTENCE_FORMAT_LIST
#undef ADD_SF
    Parsing_Sentence_Format_MAX
};

static String8 parsing_sentence_format_tokens[Parsing_Sentence_Format_MAX] {
#define ADD_SF(type) {#type, sizeof(#type)},
    PARSING_SENTENCE_FORMAT_LIST
#undef ADD_SF
};

static String8 parsing_marport_sensor_position_tokens[Data_Sensor_Position_MAX] {
#define ADD_LOC(position, string, token) {token, sizeof(token)},
    DATA_MARPORT_SENSOR_LOCATIONS
#undef ADD_LOC
};

static String8 parsing_marport_data_type_tokens[Data_Value_Type_MAX] {
#define ADD_VALUE_TYPE(name, string, unit, scanmar_sensor, scanmar_measure, marport_token) {marport_token, sizeof(marport_token)},
    DATA_VALUE_TYPE_LIST
#undef ADD_VALUE_TYPE
};

static String8 parsing_scanmar_sensor_type_tokens[Data_Value_Type_MAX] {
#define ADD_VALUE_TYPE(name, string, unit, scanmar_sensor, scanmar_measure, marport_token) {scanmar_sensor, sizeof(scanmar_sensor)},
    DATA_VALUE_TYPE_LIST
#undef ADD_VALUE_TYPE
};

static char parsing_scanmar_measure_tokens[Data_Value_Type_MAX] {
#define ADD_VALUE_TYPE(name, string, unit, scanmar_sensor, scanmar_measure, marport_token) scanmar_measure,
    DATA_VALUE_TYPE_LIST
#undef ADD_VALUE_TYPE
};
