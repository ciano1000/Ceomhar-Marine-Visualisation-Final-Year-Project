#include <gl\gl.h>
#include "gl\wglext.h"
#include "gl\glext.h"

INTERNAL void * Win32LoadOpenGLProc(char *name);

#define GLPROC(name, type) PFNGL##type##PROC gl##name = 0;
#include "glProcs.inc"

INTERNAL void LoadAllGLProcs(void)
{
    //TODO: update this with platform struct to get loading function when loading game as DLL
#define GLPROC(name,type) gl##name = (PFNGL##type##PROC)Win32LoadOpenGLProc("gl"#name);
#include "glProcs.inc"
}
