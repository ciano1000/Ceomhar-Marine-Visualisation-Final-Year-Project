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
#include "ceomhar_string.h"
#include "ceomhar_os.h"
#include "ceomhar_memory.cpp"
#include "ceomhar_os.cpp"
#include "ceomhar_string.cpp"
#include "win32_platform.cpp"
#include "win32_app_loading.cpp"

// TODO(Cian): @Win32 win32 layer should have nothing to do with Opengl/nanovg context, should be in a seperate thread eventually
#include "nano\nanovg.h"
#include "nano\nanovg.c"
#define NANOVG_GL3_IMPLEMENTATION
#include "nano\nanovg_gl.h"
global NVGcontext *vg_context = {};

global OS_State global_os;

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
                get_screen_info(window_handle, &global_os.display);
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
            os_push_event(os_mouse_button_down_event(OS_Mouse_Button_Left, global_os.mouse_pos));
        } break;
        case WM_LBUTTONUP:
        {
            os_push_event(os_mouse_button_up_event(OS_Mouse_Button_Left, global_os.mouse_pos));
        } break;
        case WM_RBUTTONDOWN:
        {
            os_push_event(os_mouse_button_down_event(OS_Mouse_Button_Right, global_os.mouse_pos));
        } break;
        case WM_RBUTTONUP:
        {
            os_push_event(os_mouse_button_up_event(OS_Mouse_Button_Right, global_os.mouse_pos));
        } break;
        case WM_MOUSEMOVE:
        {
            u16 x = LOWORD(l_param);
            u16 y = HIWORD(l_param);
            POINT mouse_pos;
            GetCursorPos(&mouse_pos);
            ScreenToClient(window_handle, &mouse_pos);
            V2 pos = {(f32)mouse_pos.x, (f32)mouse_pos.y};
            
            V2 last_pos = global_os.mouse_pos;
            global_os.mouse_pos = pos;
            V2 delta_pos = v2(global_os.mouse_pos.x - last_pos.x, global_os.mouse_pos.y - last_pos.y);
            
            os_push_event(os_mouse_move_event(pos, delta_pos));
            
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
    //NVGcontext *vg_context = {};
    //OS_State *os = {};
    
    
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
        win32_init_gl(window_handle,device_context);
        win32_load_all_gl_procs();
        ReleaseDC(window_handle,device_context);
        ShowWindow(window_handle, SW_SHOWDEFAULT);
        
        vg_context= nvgCreateGL3( NVG_STENCIL_STROKES|NVG_ANTIALIAS| NVG_DEBUG );
        
        // NOTE(Cian): Load font(s)
        // TODO(Cian): Get the CWD instead of hardcoding location
        char *fontLocation_1 = "D:\\dev\\fyp_ceomhar\\code\\fonts\\Roboto-Bold.ttf";
        nvgCreateFont(vg_context,"roboto-bold", fontLocation_1);
        char *fontLocation_2 = "D:\\dev\\fyp_ceomhar\\code\\fonts\\Roboto-Medium.ttf";
        nvgCreateFont(vg_context,"roboto-medium", fontLocation_2);
        
        os = &global_os;
        global_os.running = true;
        global_os.reserve_memory = &reserve_memory;
        global_os.commit_memory = &commit_memory;
        global_os.decommit_memory = &decommit_memory;
        global_os.release_memory = &release_memory;
        
        global_os.permanent_arena = memory_arena_initialise();
        global_os.frame_arena = memory_arena_initialise();
        global_os.scope_arena = memory_arena_initialise();
        
        global_os.debug_read_entire_file = &debug_read_entire_file;
        
        // TODO(Cian): ReCalculate window RECT on size change and notify game layer
        OS_App_Display screen_dimension = {};
        get_screen_info(window_handle, &screen_dimension);
        global_os.display = screen_dimension;
        
        Win32_App_Code code = {};
        // NOTE(Cian): Load app code
        HMODULE app_dll = LoadLibraryA("Ceomhar_app.dll");
        code.app_start = (App_Start*)GetProcAddress(app_dll,"update_and_render"); 
        code.app_update_and_render = (App_Update_And_Render*)GetProcAddress(app_dll,"update_and_render"); 
        
        code.app_start(os);
        
        wglSwapIntervalEXT(1);
        
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        while(global_os.running)
        {
            LARGE_INTEGER win_perf_counter_large;
            QueryPerformanceCounter(&win_perf_counter_large);
            
            u64 initial_perf_counter = (u64)win_perf_counter_large.QuadPart;
            
            os->current_time = initial_perf_counter / perf_frequency;
            //Main game loop
            MSG message; 
            get_screen_info(window_handle, &screen_dimension);
            os->display = screen_dimension;
            
            glViewport( 0, 0, (u32)os->display.width, (u32)os->display.height);
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT); 
            code.app_update_and_render();
            
            // TODO(Cian): @Platform When seperating app and platform layer, different app entry/exit points and functions need to be specified, e.g. here a function to clean up necessary platform stuff would occur
            os_flush_events();
            
            HDC dc = GetDC(window_handle);
            SwapBuffers(dc);
            ReleaseDC(window_handle, device_context);
            
            while(PeekMessage(&message,0,0,0,PM_REMOVE|PM_NOYIELD))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
                
            }
            
            memory_arena_clear(&os->frame_arena);
        }
        memory_arena_release(&os->permanent_arena);
        memory_arena_release(&os->frame_arena);
        memory_arena_release(&os->scope_arena);
        
        // TODO(Cian): Clean up contexts and memory arenas
    }
    
    
    return 0;
}