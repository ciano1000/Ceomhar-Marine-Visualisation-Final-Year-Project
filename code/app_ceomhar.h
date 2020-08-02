struct AppDisplay 
{
    u32 width;
    u32 height;
    f32 pixel_ratio;
};

struct AppState 
{
	AppDisplay display;
};

struct AppMemory 
{
	
};


void AppUpdateAndRender(AppState *app_state, AppMemory *app_memory);