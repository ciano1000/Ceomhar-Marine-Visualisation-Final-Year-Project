void AppStart(OS_State *os_state, NVGcontext *context);
void AppUpdateAndRender(OS_State *os_state, NVGcontext *context);


GLOBAL OS_State *global_os = {};
// TODO(Cian): Move this into the Permanent Storage arena changing nanovg's memory management to our system
GLOBAL NVGcontext *global_vg = {};