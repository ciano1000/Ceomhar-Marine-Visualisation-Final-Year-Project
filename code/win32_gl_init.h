#include <gl\gl.h>
#include "gl\wglext.h"
#include "gl\glext.h"

internal void *win32_load_opengl_proc(char *name);

#define GLPROC(name, type) PFNGL##type##PROC gl##name = 0;
#define WGLPROC(name, type) PFNWGL##type##EXTPROC wgl##name##EXT = 0;
#include "glProcs.inc"

internal void win32_load_all_gl_procs(void)
{
    //TODO: update this with platform struct to get loading function when loading game as DLL
#define STRING(s) #s
#define GLPROC(name,type) gl##name = (PFNGL##type##PROC)win32_load_opengl_proc("gl"#name);
#define WGLPROC(name, type) wgl##name##EXT = (PFNWGL##type##EXTPROC)win32_load_opengl_proc(STRING(wgl##name##EXT));
#include "glProcs.inc"
}

