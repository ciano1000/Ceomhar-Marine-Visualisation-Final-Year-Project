void AppStart(OS_State *os_state, NVGcontext *context);
void AppUpdateAndRender(OS_State *os_state, NVGcontext *context);
struct APP_Debug{
    OS_FileRead demo_read;
    // TODO(Cian): Temporary thing for demo
    Measurement measurements[13735];
};
global APP_Debug *debug;