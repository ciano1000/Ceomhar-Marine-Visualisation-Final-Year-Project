// TODO(Cian): Replace some of these C std libs with our own
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <windows.h>
#include <Shellscalingapi.h>
#include "utils.h"
#include "win32_gl_init.h"
#include "win32_gl_init.cpp"
#include "win32_ceomhar.h"

#include "ceomhar_memory.h"
#include "ceomhar_platform.h"

#include "nano\nanovg.h"
#include "nano\nanovg.c"
#define NANOVG_GL3_IMPLEMENTATION
#include "nano\nanovg_gl.h"

GLOBAL NVGcontext *global_vg = {};
GLOBAL OS_State *global_os = {};
#include "ceomhar_string.h"
#include "ceomhar_string.cpp"
#include "ceomhar_ui.h"
#include "ceomhar_ui.cpp"
#include "app_ceomhar.h"
#include "ceomhar_memory.cpp"
#include "app_ceomhar.cpp"


PLATFORM_RESERVE_MEMORY(Win32ReserveMemory) {
    void * memory = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return memory;
}

PLATFORM_COMMIT_MEMORY(Win32CommitMemory) {
    VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
}

PLATFORM_DECOMMIT_MEMORY(Win32DecommitMemory) {
    VirtualFree(memory, size, MEM_DECOMMIT);
}

PLATFORM_RELEASE_MEMORY(Win32ReleaseMemory) {
    VirtualFree(memory, size, MEM_RELEASE);
}

void Win32GetScreenInfo(HWND window_handle, AppDisplay  *screen_dimension) {
    RECT window_rect = {};
    
    GetClientRect(window_handle, &window_rect);
    screen_dimension->width = window_rect.right - window_rect.left;
    screen_dimension->height = window_rect.bottom - window_rect.top;
    
    screen_dimension->pixel_ratio = ((f32)screen_dimension->width) / ((f32)screen_dimension->height);
    
    // TODO(Cian): Look at doing this when WM_DPICHANGED received
    HDC hdc = GetDC(window_handle);
    screen_dimension->dpi = GetDpiForWindow(window_handle);
}

GLOBAL b32 Running;
LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
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
                Win32GetScreenInfo(window_handle, &global_os->display);
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
        window_handle = CreateWindowExA(0, window_class.lpszClassName,"UITests", WS_OVERLAPPEDWINDOW
                                        | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,
                                        CW_USEDEFAULT,CW_USEDEFAULT,0,0,instance,0);
        
        device_context = GetDC(window_handle);
    }
    
    if(window_handle)
    {
        Win32InitGL(window_handle,device_context);
        LoadAllGLProcs();
        ReleaseDC(window_handle,device_context);
        ShowWindow(window_handle, SW_SHOWDEFAULT);
        //MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
        
        
        //nanovg init
        // TODO(Cian): Rework so that vgcontext is stored in an arena
        global_vg = nvgCreateGL3( NVG_STENCIL_STROKES);
        Running = true;
        
        // NOTE(Cian): Load font(s)
        // TODO(Cian): Get the CWD instead of hardcoding location
        char *fontLocation = "D:\\dev\\nanovg_tests\\code\\fonts\\Roboto-Bold.ttf";
        nvgCreateFont(global_vg,"roboto-bold", fontLocation);
        
        
        OS_State os_state;
        global_os  = &os_state;
        // TODO(Cian):  push this onto a memory arena
        global_os->ReserveMemory = &Win32ReserveMemory;
        global_os->CommitMemory = &Win32CommitMemory;
        global_os->DecommitMemory = &Win32DecommitMemory;
        global_os->ReleaseMemory = &Win32ReleaseMemory;
        
        global_os->permanent_arena = Memory_ArenaInitialise();
        global_os->frame_arena = Memory_ArenaInitialise();
        
        // TODO(Cian): ReCalculate window RECT on size change and notify game layer
        AppDisplay screen_dimension = {};
        Win32GetScreenInfo(window_handle, &screen_dimension);
        global_os->display = screen_dimension;
        
        AppStart(global_os, global_vg);
        
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        while(Running)
        {
            
            LARGE_INTEGER win_perf_counter_large;
            QueryPerformanceCounter(&win_perf_counter_large);
            
            u64 initial_perf_counter = (u64)win_perf_counter_large.QuadPart;
            
            global_os->current_time = initial_perf_counter / perf_frequency;
            //Main game loop
            MSG message; 
            Win32GetScreenInfo(window_handle, &screen_dimension);
            global_os->display = screen_dimension;
            AppUpdateAndRender();
            
            HDC dc = GetDC(window_handle);
            SwapBuffers(dc);
            ReleaseDC(window_handle, device_context);
            
            while(PeekMessage(&message,0,0,0,PM_REMOVE|PM_NOYIELD))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
                
            }
            
            Memory_ArenaClear(&global_os->frame_arena);
        }
        Memory_ArenaRelease(&global_os->permanent_arena);
        Memory_ArenaRelease(&global_os->frame_arena);
        
        // TODO(Cian): Clean up contexts and memory arenas
    }
    
    
    return 0;
}