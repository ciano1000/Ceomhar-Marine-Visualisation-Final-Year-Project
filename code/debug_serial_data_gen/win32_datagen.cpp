#include <windows.h>
#include <assert.h>

#define STB_SPRINTF_IMPLEMENTATION
#include "..\stb\stb_sprintf.h"

#include "..\utils.h"
#include "..\ceomhar_math.h"
#include "..\ceomhar_memory.h"
#include "..\ceomhar_string.h"
#include "..\ceomhar_os.h"
#include "..\ceomhar_memory.cpp"
#include "..\ceomhar_string.cpp"
#include "..\win32_platform.cpp"

#define STRING_EXPANSION(str) #str
#define NUM_EXAMPLE_ROWS 13735
#define MAX_DATA_STRING_LENGTH 52
#define COM_PORT STRING_EXPANSION(COM4)
#define BAUD_RATE 9600

global String8 g_strings[NUM_EXAMPLE_ROWS];

u32 main(u32 argc, char **argv) {
    
    // TODO(Cian): Using this to save time but it is a bit messy
    OS_State state = {};
    os = &state;
    state.running = true;
    state.reserve_memory = &reserve_memory;
    state.commit_memory = &commit_memory;
    state.decommit_memory = &decommit_memory;
    state.release_memory = &release_memory;
    
    state.permanent_arena = memory_arena_initialise();
    state.frame_arena = memory_arena_initialise();
    state.scope_arena = memory_arena_initialise();
    
    state.debug_read_entire_file = &debug_read_entire_file;
    
    String8 file_dump = {}; 
    debug_read_entire_file("D:\\dev\\fyp_ceomhar\\FSS_NMEA_SampleData.txt", &file_dump);
    
    u32 bytes_read = 0;
    u32 string_idx = 0;
    
    while(bytes_read < (file_dump.size - 1)) {
        //setting the delimiter to null essentially makes this read the file line by line
        g_strings[string_idx] = string_tokenizer(&os->permanent_arena, file_dump, null, &bytes_read);
        
        string_idx++;
    }
    
    // TODO(Cian): @DATAGEN Do error checking on the windows file/device functions used below
    //open handle to the COM port
    HANDLE h_com_port = CreateFileA(COM_PORT, GENERIC_WRITE, null, null, OPEN_EXISTING, null, null);
    
    //DCB - Device Control Block: is just a settings struct for serial communications devices
    DCB com_serial_params = {};
    com_serial_params.DCBlength = sizeof(DCB);
    com_serial_params.BaudRate = BAUD_RATE;
    com_serial_params.fBinary = true;
    com_serial_params.Parity = NOPARITY;
    com_serial_params.ByteSize = 8;
    com_serial_params.StopBits = ONESTOPBIT; //stop-bits signify the end of a char, think of it like a null-terminator
    //just a function to apply our settings to our device
    SetCommState(h_com_port, &com_serial_params);
    
    //Struct for the timeout settings of a device
    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
    SetCommTimeouts(h_com_port, &timeouts);
    
    for (u32 i = 0; i < NUM_EXAMPLE_ROWS; i += 1){
        u32 bytes_written = 0;
        do {
            WriteFile(h_com_port, g_strings[i].data, g_strings[i].size, &(DWORD)bytes_written, null);
        } while(bytes_written < g_strings[i].size);
        Sleep(1000);
    }
    
    
    
    CloseHandle(h_com_port);
    return 0;
}