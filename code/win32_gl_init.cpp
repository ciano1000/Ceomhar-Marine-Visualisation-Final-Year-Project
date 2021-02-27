
namespace Win32 {
    internal void * load_opengl_proc(char *name)
    {
        void *p = (void *)wglGetProcAddress(name);
        
        //On failure, p can either be NULL(!p), or an address equalling 1, 2 or 3 according to the windows DOCS
        if (!p || p == (void *)0x1 || p == (void *)0x2 || p == (void *)0x3 || p == (void *)-1)
        {
            return 0;
        }
        else
        {
            return p;
        }
    }
    
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB;
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
    
    internal void load_wgl_functions()
    {
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)load_opengl_proc("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)load_opengl_proc("wglCreateContextAttribsARB");
        wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)load_opengl_proc("wglMakeContextCurrentARB");
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)load_opengl_proc("wglSwapIntervalEXT");
    }
    
    internal HGLRC init_gl(HWND windowHandle, HDC deviceContext)
    {
        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
            PFD_TYPE_RGBA,                                              // The kind of framebuffer. RGBA or palette.
            32,                                                         // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24, // Number of bits for the depthbuffer
            8,  // Number of bits for the stencilbuffer
            0,  // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };
        
        //Returns 0 if couldn't find a format given our pfd TODO: if statement for this
        s32 pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
        SetPixelFormat(deviceContext, pixelFormat, &pfd);
        
        HGLRC dummyContext = wglCreateContext(deviceContext);
        wglMakeCurrent(deviceContext, dummyContext);
        
        load_wgl_functions();
        
        //Setup real format
        const s32 attribList[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0
        };
        
        UINT numFormats = 0;
        wglChoosePixelFormatARB(deviceContext, attribList, 0, 1, &pixelFormat, &numFormats);
        
        //TODO: Again check if pixelFormat is null or not
        
        //Request a certain GL Context version
        const int contextAttrib[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            0
        };
        
        HGLRC context = wglCreateContextAttribsARB(deviceContext,dummyContext,contextAttrib);
        //TODO: check if context has successfully been created
        
        wglMakeCurrent(deviceContext, 0);
        wglDeleteContext(dummyContext);
        wglMakeCurrent(deviceContext,context);
        wglSwapIntervalEXT(0);
        
        return context;
    }
    
    void delete_gl_context(HGLRC glContext, HDC deviceContext)
    {
        wglMakeCurrent(deviceContext, NULL);
        wglDeleteContext(glContext);
    }
}