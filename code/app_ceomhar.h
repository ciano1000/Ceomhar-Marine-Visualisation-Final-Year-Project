struct App_Debug{
    String8 demo_read;
    // TODO(Cian): Temporary thing for demo
    Measurement measurements[13735];
};
global App_Debug *debug;
global NVGcontext *vg_context;