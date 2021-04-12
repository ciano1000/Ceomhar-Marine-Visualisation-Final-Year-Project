PLATFORM_RESERVE_MEMORY(reserve_memory) {
    void * memory = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return memory;
}

PLATFORM_COMMIT_MEMORY(commit_memory) {
    VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
}

PLATFORM_DECOMMIT_MEMORY(decommit_memory) {
    VirtualFree(memory, size, MEM_DECOMMIT);
}

PLATFORM_RELEASE_MEMORY(release_memory) {
    VirtualFree(memory, size, MEM_RELEASE);
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_read_entire_file) {
    b32 result = false;
    
    HANDLE file_handle = CreateFileA((LPCSTR)file_path, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file_handle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER file_size_struct = {};
        
        if(GetFileSizeEx(file_handle, &file_size_struct)) {
            u64 file_size = (u64)file_size_struct.QuadPart + 1;
            file_read->size = (u32)file_size;
            
            // TODO(Cian): @Win32 have some kind of debug arena
            void *file_buffer = memory_arena_push(&os->permanent_arena, file_size);
            file_read->data = (char *)file_buffer;
            
            u32 bytes_read = 0;
            if(ReadFile(file_handle, file_buffer, (DWORD)file_size, (LPDWORD)&bytes_read, NULL)) {
                result = true;
                file_read->data[file_read->size] = '\0';
            }
        }
        CloseHandle(file_handle);
    }
    
    return result;
} 

// TODO(Cian): Messy way of excluding this from datagen, should probably make this its own thing
#ifndef CEOMHAR_DATAGEN
#ifndef CEOMHAR_SERVER

void get_screen_info(HWND window_handle, OS_App_Display *screen_dimension) {
    RECT window_rect = {};
    
    GetClientRect(window_handle, &window_rect);
    screen_dimension->width = (f32)(window_rect.right - window_rect.left);
    screen_dimension->height = (f32)(window_rect.bottom - window_rect.top);
    
    screen_dimension->pixel_ratio = ((f32)screen_dimension->width) / ((f32)screen_dimension->height);
    
    HDC hdc = GetDC(window_handle);
    //screen_dimension->dpi = GetDpiForWindow(window_handle);
    // TODO(Cian): Should only redo this if monitor changes
    u32 monitor_x = GetDeviceCaps(hdc, HORZRES);
    u32 monitor_y = GetDeviceCaps(hdc, VERTRES);
    u32 monitor_product = monitor_x * monitor_y;
    //screen_dimension->dpi = (u32)((f32)(monitor_product / OS_DEFAULT_DISPLAY_DENSITY) * OS_DEFAULT_DPI);
}
#endif
#endif