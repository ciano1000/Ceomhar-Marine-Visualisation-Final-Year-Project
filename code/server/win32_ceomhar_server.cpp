#define _WINSOCKAPI_//needed to stop windows.h including the older version of winsock
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <Winsock2.h>
#include <Ws2tcpip.h>

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
#define COM_PORT STRING_EXPANSION(COM5) // TODO(Cian): @Server Temporary, this should be configurable in app/server
#define BAUD_RATE 9600

u32 main() {
    //setup os stuff for memory, strings etc
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
    
    // TODO(Cian): @Server Need to handle all possible errors below
    
    HANDLE h_com_port = CreateFileA(COM_PORT, GENERIC_READ, null, null, OPEN_EXISTING, null, null);
    
    //DCB - Device Control Block: is just a settings struct for serial communications devices
    DCB com_serial_params = {};
    com_serial_params.DCBlength = sizeof(DCB);
    b32 com_status = GetCommState(h_com_port, &com_serial_params);
    
    if(com_status == false) {
        printf("Error in com state\n");
        return 0;//should print the error here
    }
    
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
    
    SetCommMask(h_com_port, EV_RXCHAR); //Sets which device events should be monitored for, EV_RXCHAR will monitor for when a char is received
    
    DWORD event_mask;
    
    // TODO(Cian): Standin string thingy just for testing
    char read_data[50][52];
    u32 string_count = 0;
    u32 char_count = 0;
    u32 bytes_to_read = 0;
    // TODO(Cian): @Server change this to some kind of "running" variable
    while(true) {
        WaitCommEvent(h_com_port, &event_mask, null); //waits for a character to be received TODO  @Server this will block, should eventually be on seperate thread to server stuff
        
        do {
            char read_char;
            ReadFile(h_com_port, &read_char, sizeof(char), &(DWORD)bytes_to_read, null);
            read_data[string_count][char_count] = read_char;
            
            char_count++;
            if(read_char == null) {
                printf("%s \n", read_data[string_count]);
                
                string_count++;
                char_count = 0;
            }
        } while(bytes_to_read > 0);
    }
}