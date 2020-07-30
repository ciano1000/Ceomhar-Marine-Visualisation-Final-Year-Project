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
LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
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
            result = DefWindowProc(windowHandle, message, wParam, lParam);
        }
        break;
    }
    
    return result;
}

int main() {
    WNDCLASS windowClass = {};
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpszClassName = "UITests";
    windowClass.lpfnWndProc = WindowProc;
    
    HINSTANCE instance = GetModuleHandle(NULL);
    
    windowClass.hInstance = instance;
    
    HWND windowHandle = {};
    HDC deviceContext = {};
    
    if(RegisterClassA(&windowClass))
    {
        windowHandle = CreateWindowExA(0, windowClass.lpszClassName,"UITests", WS_OVERLAPPEDWINDOW
                                       | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,
                                       CW_USEDEFAULT,CW_USEDEFAULT,0,0,instance,0);
        
        deviceContext = GetDC(windowHandle);
    }
    
    if(windowHandle)
    {
        Win32InitGL(windowHandle,deviceContext);
        LoadAllGLProcs();
        ReleaseDC(windowHandle,deviceContext);
        ShowWindow(windowHandle, SW_SHOWDEFAULT);
        MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
        
        
        //nanovg init
        NVGcontext *vgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
        Running = true;
        
        // NOTE(Cian): Load font(s)
        // TODO(Cian): Get the CWD instead of hardcoding location
        char *fontLocation = "D:\\dev\\nanovg_tests\\code\\fonts\\Roboto-Bold.ttf";
        nvgCreateFont(vgContext,"sans-bold", fontLocation);
        
        while(Running)
        {
            //Main game loop
            MSG message; 
            
            // TODO(Cian): ReCalculate window RECT on size change instead of every frame
            RECT clientRect;
            GetClientRect(windowHandle,&clientRect);
            float pxRatio =(f32)(clientRect.right-clientRect.left) / (f32)(clientRect.bottom-clientRect.top);
            glViewport( 0, 0, clientRect.right-clientRect.left, clientRect.bottom-clientRect.top );
            
            glClearColor(1.0f,1.0f,1.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
            
            nvgBeginFrame(vgContext, (f32)(clientRect.right-clientRect.left),  (f32)(clientRect.bottom-clientRect.top), pxRatio);
            
            // NOTE(Cian): NanoVG sample from Github, draws a RECT with a circle cut out
            {
                nvgBeginPath(vgContext);
                nvgRect(vgContext, 100,100, 120,30);
                nvgCircle(vgContext, 120,120, 5);
                nvgPathWinding(vgContext, NVG_HOLE);	// Mark circle as a hole.
                nvgFillColor(vgContext, nvgRGBA(255,192,0,255));
                nvgFill(vgContext);
            }
            
            // NOTE(Cian): Test Title element w/Nano
            nvgBeginPath(vgContext);
            //Just a background panel
            nvgRect(vgContext, 300, 50, 600, 600);
            nvgFillColor(vgContext, nvgRGBA(28, 30, 34, 192));
            nvgFill(vgContext);
            
            nvgFontSize(vgContext, 15.0f);
            nvgFontFace(vgContext, "sans-bold");
            // NOTE(Cian): Aligning to the left/right means that the left/right (e.g beginning of text/ end of text) is positioned at the given coordinates
            nvgTextAlign(vgContext, NVG_ALIGN_CENTER|NVG_ALIGN_MIDDLE);
            nvgFillColor(vgContext, nvgRGBA(255,255,255,255));
            nvgText(vgContext, 600, 70, "This is a title", NULL);
            
            nvgRestore(vgContext);
            
            nvgEndFrame(vgContext);
            
            HDC dc = GetDC(windowHandle);
            SwapBuffers(dc);
            ReleaseDC(windowHandle, deviceContext);
            
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