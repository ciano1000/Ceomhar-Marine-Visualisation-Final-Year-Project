#include <winsock2.h>
#include <windows.h>
#include <assert.h>
#include <stdio.h>
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
#define MAX_INCOMING_BYTES 52

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
    
    if(h_com_port == INVALID_HANDLE_VALUE) {
        printf("Error opening COM port: %d\n", GetLastError());
        // TODO(Cian): @Server call shutdown logic here maybe
        return 1;
    }
    
    //DCB - Device Control Block: is just a settings struct for serial communications devices
    DCB com_serial_params = {};
    com_serial_params.DCBlength = sizeof(DCB);
    b32 com_status = GetCommState(h_com_port, &com_serial_params);
    
    if(com_status == false) {
        printf("Error in COM state: %d\n", GetLastError());
        // TODO(Cian): @Server call shutdown logic here maybe
        return 1;
    }
    
    com_serial_params.BaudRate = BAUD_RATE;
    com_serial_params.fBinary = true;
    com_serial_params.Parity = NOPARITY;
    com_serial_params.ByteSize = 8;
    com_serial_params.StopBits = ONESTOPBIT; //stop-bits signify the end of a char, think of it like a null-terminator
    //just a function to apply our settings to our device
    
    com_status = SetCommState(h_com_port, &com_serial_params);
    
    if(com_status == false) {
        printf("Error setting COM state: %d\n", GetLastError());
        // TODO(Cian): @Server call shutdown logic here maybe
        return 1;
    }
    
    //Struct for the timeout settings of a device
    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
    com_status = SetCommTimeouts(h_com_port, &timeouts);
    
    if(com_status == false) {
        printf("Error setting COM timeout: %d\n", GetLastError());
        // TODO(Cian): @Server call shutdown logic here maybe
        return 1;
    }
    
    com_status = SetCommMask(h_com_port, EV_RXCHAR); //Sets which device events should be monitored for, EV_RXCHAR will monitor for when a char is received
    
    if(com_status == false) {
        printf("Error setting COM mask: %d\n", GetLastError());
        // TODO(Cian): @Server call shutdown logic here maybe
        return 1;
    }
    
    WSADATA data = {};
    s32 winsock_result = WSAStartup(MAKEWORD(2,2), &data);
    
    if(winsock_result != NO_ERROR) {
        printf("WSAStartup failed: %d", winsock_result);
        return 1;
    }
    
    SOCKADDR_IN send_addr = {0};
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(3490);
    send_addr.sin_addr.s_addr = inet_addr("127.0.0.11");
    
    SOCKET socket_out = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (socket_out == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    DWORD event_mask;
    
    char buffer[MAX_INCOMING_BYTES];
    u32 char_count = 0;
    u32 bytes_to_read = 0;
    // TODO(Cian): @Server change this to some kind of "running" variable
    while(true) {
        com_status = WaitCommEvent(h_com_port, &event_mask, null); //waits for a character to be received TODO  @Server this will block, should eventually be on seperate thread to server stuff
        if(com_status == false) {
            printf("Error waiting for COM event: %d\n", GetLastError());
            // TODO(Cian): @Server call shutdown logic here maybe
            return 1;
        }
        
        // NOTE(Cian): @Server Plan moving forward
        //Need to learn/decide how best to share the incoming strings with multiple threads
        //will probably have at least two threads:
        //Server thread: UDP socket stuff, maybe some serialisation & compression
        //SQL thread: thread for the SQL queries to load data into database
        //Data Processing thread: might want to do some Data processing from on the server side so that we can
        //save the results to the database, replacing some of the SQL stuff that's currently being done, and/or relieve some processing responsiblities from the client - this won't be done unless we have some tim at the end of the project 
        
        do {
            char read_char;
            b32 read_success = ReadFile(h_com_port, &read_char, sizeof(char), &(DWORD)bytes_to_read, null);
            if(read_success == false) {
                printf("Error reading from COM port: %d\n", GetLastError());
                // TODO(Cian): @Server call shutdown logic here maybe
                return 1;
            }
            
            buffer[char_count] = read_char;
            
            char_count++;
            if(read_char == null) {
                printf("%s \n", buffer);
                
                u32 total_bytes_sent = 0;
                
                do{
                    s32 bytes_sent = sendto(socket_out, buffer, sizeof(buffer), null, (SOCKADDR*) &send_addr, sizeof(send_addr));
                    
                    if (socket_out == INVALID_SOCKET) {
                        printf("Sending to socket failed: %d\n", WSAGetLastError());
                        WSACleanup();
                        return 1;
                    }
                    total_bytes_sent += bytes_sent;
                } while(total_bytes_sent < char_count);
                
                char_count = 0;
            }
        } while(bytes_to_read > 0);
    }
    
    WSACleanup();
    return 0;
}