struct APP_Debug{
    OS::File_Read demo_read;
    // TODO(Cian): Temporary thing for demo
    Parsing::Measurement measurements[13735];
};
global APP_Debug *debug;
global NVGcontext *vg_context;