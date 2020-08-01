#include <windows.h>
#include "utils.h"
#include "win32_gl_init.h"
#include "win32_gl_init.cpp"
// TODO(Cian): Load OpenGl functions that NanoVG needs

#include "nano\nanovg.h"
#include "nano\nanovg.c"
#define NANOVG_GL3_IMPLEMENTATION
#include "nano\nanovg_gl.h"


GLOBAL b32 Running;
LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;
    switch (message)
    {
        //Input stuff
        //TODO: Create input system that will be called here
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            //handle input stuff
        }
        break;
        //Resize event
        case WM_SIZE:
        {
            
        }
        break;
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

int main() {
    WNDCLASS window_class = {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = "UITests";
    window_class.lpfnWndProc = WindowProc;
    
    HINSTANCE instance = GetModuleHandle(NULL);
    
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
        MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
        
        
        //nanovg init
        NVGcontext *vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
        Running = true;
        
        // NOTE(Cian): Load font(s)
        // TODO(Cian): Get the CWD instead of hardcoding location
        char *fontLocation = "D:\\dev\\nanovg_tests\\code\\fonts\\Roboto-Bold.ttf";
        nvgCreateFont(vg,"sans-bold", fontLocation);
        
        while(Running)
        {
            //Main game loop
            MSG message; 
            
            // TODO(Cian): ReCalculate window RECT on size change instead of every frame
            RECT client_rect;
            GetClientRect(window_handle,&client_rect);
            float px_ratio =(f32)(client_rect.right-client_rect.left) / (f32)(client_rect.bottom-client_rect.top);
            
            // NOTE(Cian): Stuff to go in UpdateAndRender
            {
                glViewport( 0, 0, client_rect.right-client_rect.left, client_rect.bottom-client_rect.top );
                
                glClearColor(1.0f,1.0f,1.0f,1.0f);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
                
                nvgBeginFrame(vg, (f32)(client_rect.right-client_rect.left),  (f32)(client_rect.bottom-client_rect.top), px_ratio);
                
                // NOTE(Cian): NanoVG sample from Github, draws a RECT with a circle cut out
                {
                    nvgBeginPath(vg);
                    nvgRect(vg, 100,100, 120,30);
                    nvgCircle(vg, 120,120, 5);
                    nvgPathWinding(vg, NVG_HOLE);	// Mark circle as a hole.
                    nvgFillColor(vg, nvgRGBA(255,192,0,255));
                    nvgFill(vg);
                }
                
                // NOTE(Cian): Test Title element w/Nano
                nvgBeginPath(vg);
                //Just a background panel
                nvgRect(vg, 300, 50, 600, 600);
                nvgFillColor(vg, nvgRGBA(28, 30, 34, 192));
                nvgFill(vg);
                
                nvgFontSize(vg, 15.0f);
                nvgFontFace(vg, "sans-bold");
                // NOTE(Cian): Aligning to the left/right means that the left/right (e.g beginning of text/ end of text) is positioned at the given coordinates
                nvgTextAlign(vg, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
                nvgFillColor(vg, nvgRGBA(255,255,255,255));
                nvgText(vg, 600, 70, "This is a title", NULL);
                
                nvgRestore(vg);
                
                nvgEndFrame(vg);
            }
            
            HDC dc = GetDC(window_handle);
            SwapBuffers(dc);
            ReleaseDC(window_handle, device_context);
            
            while(PeekMessage(&message,0,0,0,PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
        
        // TODO(Cian): Clean up contexts
    }
    
    
    
    
    return 0;
}