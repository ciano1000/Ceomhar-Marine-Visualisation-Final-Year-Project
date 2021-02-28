namespace Win32 {
    struct App_Code {
        App_Start *app_start;
        App_Update_And_Render *app_update_and_render;
        App_Unload *app_unload;
        App_Reload *app_reload;
        HMODULE app_dll;
        FILETIME last_open;
    };
}