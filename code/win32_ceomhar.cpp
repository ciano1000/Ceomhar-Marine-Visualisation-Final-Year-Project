// TODO(Cian): Replace some of these C std libs with our own
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

#define STB_SPRINTF_IMPLEMENTATION
#include "stb\stb_sprintf.h"

#include <windows.h>
#include <Shellscalingapi.h>
#include "utils.h"
#include "win32_gl_init.h"
#include "win32_gl_init.cpp"
#include "win32_ceomhar.h"

#include "ceomhar_math.h"
#include "ceomhar_memory.h"
#include "ceomhar_os.h"

#include "nano\nanovg.h"
#include "nano\nanovg.c"
#define NANOVG_GL3_IMPLEMENTATION
#include "nano\nanovg_gl.h"

// TODO(Cian): Standardise where globals are defined because I found it hard to find where this is, maybe also, we should just have one single global struct pointer that contains everything we need?
global NVGcontext *global_vg = {};
global OS_State *global_os = {};

#include "ceomhar_memory.cpp"
#include "ceomhar_string.h"
#include "ceomhar_string.cpp"
#include "ceomhar_os.cpp"
#include "ceomhar_ui.h"
#include "ceomhar_ui.cpp"
#include "ceomhar_parsing.h"
#include "app_ceomhar.h"
#include "ceomhar_parsing.cpp"
#include "app_ceomhar.cpp"

namespace Win32 {
    PLATFORM_RESERVE_MEMORY(reserve_memory) {
        void * memory = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
        return memory;
    }
    
    PLATFORM_COMMIT_MEMORY(commit_memory) {
        VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
    }
    
    PLATFORM_DECOMMIT_MEMORY(decommit_memory) {
        VirtualFree(memory, size, MEM_DECOMMIT);
    }
    
    PLATFORM_RELEASE_MEMORY(release_memory) {
        VirtualFree(memory, size, MEM_RELEASE);
    }
    
    DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_read_entire_file) {
        b32 result = false;
        
        HANDLE file_handle = CreateFileA((LPCSTR)file_path, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(file_handle != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER file_size_struct = {};
            
            if(GetFileSizeEx(file_handle, &file_size_struct)) {
                u64 file_size = (u64)file_size_struct.QuadPart + 1;
                file_read->size = file_size;
                
                // TODO(Cian): @Win32 have some kind of debug arena
                void *file_buffer = Memory::arena_push(&global_os->permanent_arena, file_size);
                file_read->data = (char *)file_buffer;
                
                u32 bytes_read = 0;
                if(ReadFile(file_handle, file_buffer, (DWORD)file_size, (LPDWORD)&bytes_read, NULL)) {
                    result = true;
                    file_read->data[file_read->size] = '\0';
                }
            }
            CloseHandle(file_handle);
        }
        
        return result;
    } 
    
    
    void get_screen_info(HWND window_handle, AppDisplay  *screen_dimension) {
        RECT window_rect = {};
        
        GetClientRect(window_handle, &window_rect);
        screen_dimension->width = (f32)(window_rect.right - window_rect.left);
        screen_dimension->height = (f32)(window_rect.bottom - window_rect.top);
        
        screen_dimension->pixel_ratio = ((f32)screen_dimension->width) / ((f32)screen_dimension->height);
        
        HDC hdc = GetDC(window_handle);
        //screen_dimension->dpi = GetDpiForWindow(window_handle);
        // TODO(Cian): Should only redo this if monitor changes
        u32 monitor_x = GetDeviceCaps(hdc, HORZRES);
        u32 monitor_y = GetDeviceCaps(hdc, VERTRES);
        u32 monitor_product = monitor_x * monitor_y;
        screen_dimension->dpi = (u32)((f32)(monitor_product / OS_DEFAULT_DISPLAY_DENSITY) * OS_DEFAULT_DPI);
    }
}

using namespace Win32;

global b32 Running;
LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    static b32 weird_windows_mouse_tracking = false;
    
    LRESULT result = 0;
    switch (message)
    {
        //Input stuff
        //TODO: Create input system that will be called here
        case WM_SIZE:
        {
            // TODO(Cian): Again, some cleanup needs to be done with how we store and call certain things
            if(Running)
            {
                get_screen_info(window_handle, &global_os->display);
                AppUpdateAndRender();
                HDC dc = GetDC(window_handle);
                SwapBuffers(dc);
                ReleaseDC(window_handle, dc);
            }
        }break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            //handle input stuff
        }
        break;
        //Resize event
        case WM_QUIT:
        case WM_DESTROY:
        case WM_CLOSE:
        {
            Running = false;
        } break;
        
        case WM_LBUTTONDOWN:
        {
            OS::push_event(OS::mouse_button_down_event(OS_MouseButton_Left, global_os->mouse_pos));
        } break;
        case WM_LBUTTONUP:
        {
            OS::push_event(OS::mouse_button_up_event(OS_MouseButton_Left, global_os->mouse_pos));
        } break;
        case WM_RBUTTONDOWN:
        {
            OS::push_event(OS::mouse_button_down_event(OS_MouseButton_Right, global_os->mouse_pos));
        } break;
        case WM_RBUTTONUP:
        {
            OS::push_event(OS::mouse_button_up_event(OS_MouseButton_Right, global_os->mouse_pos));
        } break;
        case WM_MOUSEMOVE:
        {
            u16 x = LOWORD(l_param);
            u16 y = HIWORD(l_param);
            POINT mouse_pos;
            GetCursorPos(&mouse_pos);
            ScreenToClient(window_handle, &mouse_pos);
            V2 pos = {(f32)mouse_pos.x, (f32)mouse_pos.y};
            
            V2 last_pos = global_os->mouse_pos;
            global_os->mouse_pos = pos;
            V2 delta_pos = v2(global_os->mouse_pos.x - last_pos.x, global_os->mouse_pos.y - last_pos.y);
            
            OS::push_event(OS::mouse_move_event(pos, delta_pos));
            
            if(weird_windows_mouse_tracking == false) {
                weird_windows_mouse_tracking = true;
                
                TRACKMOUSEEVENT track_event = {};
                track_event.cbSize = sizeof(track_event);
                track_event.dwFlags = TME_LEAVE;
                track_event.hwndTrack = window_handle;
                track_event.dwHoverTime = HOVER_DEFAULT;
                TrackMouseEvent(&track_event);
            }
        } break;
        case WM_MOUSELEAVE:
        {
            weird_windows_mouse_tracking = false;
        } break;
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)l_param;
            lpMMI->ptMinTrackSize.x = 600;
            lpMMI->ptMinTrackSize.y = 600;
        }
        break;
        default:
        {
            result = DefWindowProc(window_handle, message, w_param, l_param);
        }
        break;
    }
    
    return result;
}

int main(u32 argc, char **argv) {
    for(u32 i = 0; i<argc; i++) {
        printf("Command %i: %s",i,argv[i]);
    }
    
    LARGE_INTEGER win_perf_frequency_large;
    QueryPerformanceFrequency(&win_perf_frequency_large);
    u64 perf_frequency = (u64)win_perf_frequency_large.QuadPart;
    
    HINSTANCE instance = GetModuleHandle(NULL);
    
    WNDCLASS window_class = {};
    
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = "UITests";
    window_class.lpfnWndProc = WindowProc;
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hInstance = instance;
    
    
    
    HWND window_handle = {};
    HDC device_context = {};
    
    if(RegisterClassA(&window_class))
    {
        window_handle = CreateWindowExA(0, window_class.lpszClassName,"Ceomhar Trawl Analysis: FYP", WS_OVERLAPPEDWINDOW
                                        | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,
                                        CW_USEDEFAULT,CW_USEDEFAULT,0,0,instance,0);
        
        device_context = GetDC(window_handle);
    }
    
    if(window_handle)
    {
        init_gl(window_handle,device_context);
        load_all_gl_procs();
        ReleaseDC(window_handle,device_context);
        ShowWindow(window_handle, SW_SHOWDEFAULT);
        //MessageBoxA(0,(char*)glGetString8(GL_VERSION), "OPENGL VERSION",0);
        
        
        //nanovg init
        // TODO(Cian): Rework so that vgcontext is stored in an arena
        global_vg = nvgCreateGL3( NVG_STENCIL_STROKES|NVG_ANTIALIAS| NVG_DEBUG );
        Running = true;
        
        // NOTE(Cian): Load font(s)
        // TODO(Cian): Get the CWD instead of hardcoding location
        char *fontLocation_1 = "D:\\dev\\fyp_ceomhar\\code\\fonts\\Roboto-Bold.ttf";
        nvgCreateFont(global_vg,"roboto-bold", fontLocation_1);
        char *fontLocation_2 = "D:\\dev\\fyp_ceomhar\\code\\fonts\\Roboto-Medium.ttf";
        nvgCreateFont(global_vg,"roboto-medium", fontLocation_2);
        
        
        OS_State os_state = {};
        global_os  = &os_state;
        // TODO(Cian):  push this onto a memory arena
        global_os->reserve_memory = &reserve_memory;
        global_os->commit_memory = &commit_memory;
        global_os->decommit_memory = &decommit_memory;
        global_os->release_memory = &release_memory;
        
        global_os->permanent_arena = Memory::arena_initialise();
        global_os->frame_arena = Memory::arena_initialise();
        global_os->scope_arena = Memory::arena_initialise();
        
        global_os->debug_read_entire_file = &debug_read_entire_file;
        
        // TODO(Cian): ReCalculate window RECT on size change and notify game layer
        AppDisplay screen_dimension = {};
        get_screen_info(window_handle, &screen_dimension);
        global_os->display = screen_dimension;
        
        AppStart(global_os, global_vg);
        
        wglSwapIntervalEXT(1);
        
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        while(Running)
        {
            LARGE_INTEGER win_perf_counter_large;
            QueryPerformanceCounter(&win_perf_counter_large);
            
            u64 initial_perf_counter = (u64)win_perf_counter_large.QuadPart;
            
            global_os->current_time = initial_perf_counter / perf_frequency;
            //Main game loop
            MSG message; 
            get_screen_info(window_handle, &screen_dimension);
            global_os->display = screen_dimension;
            AppUpdateAndRender();
            
            // TODO(Cian): @Platform When seperating app and platform layer, different app entry/exit points and functions need to be specified, e.g. here a function to clean up necessary platform stuff would occur
            OS::flush_events();
            
            HDC dc = GetDC(window_handle);
            SwapBuffers(dc);
            ReleaseDC(window_handle, device_context);
            
            while(PeekMessage(&message,0,0,0,PM_REMOVE|PM_NOYIELD))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
                
            }
            
            Memory::arena_clear(&global_os->frame_arena);
        }
        Memory::arena_release(&global_os->permanent_arena);
        Memory::arena_release(&global_os->frame_arena);
        Memory::arena_release(&global_os->scope_arena);
        
        // TODO(Cian): Clean up contexts and memory arenas
    }
    
    
    return 0;
}