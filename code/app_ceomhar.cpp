void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    
    ui_state = UI_InitState();
    
    // NOTE(Cian): Trying out UI stuff
#if 0
    {
        UI_BeginWindow(LAYOUT_TYPE); // sets up viewport, clears screen,
        UI_StartToStartConstraint(UI_PARENT, 0);
        UI_Width(30);
        UI_BeginNavMenu(ORIENTATION, );
        
        UI_EndNavMenu();
        
        UI_EndToEndConstraint(UI_ID, offset);
        UI_EndToStartConstraint(UI_PARENT, offset);
        UI_TopToTopConstraint(...);
        UI_CenterXConstraint(offset);
        //etc.. Once a ui element is created the state resets the constraints
        UI_Panel(x, y, width, height);
        //when using constraints for those axes, x & y will offset those constraints
        UI_EndWindow();
    }
#endif
#if 0
    // NOTE(Cian): Test hash
    {
        char *generated_strings[256];
        for(u32 i = 0; i < 256; ++i) {
            char rand_string[14];
            StringGenRandom(rand_string, 14);
            generated_strings[i] = (char *)Memory_ArenaPush(&global_os->frame_arena, sizeof(rand_string));
            strcpy(generated_strings[i], rand_string);
            UI_Item item = {};
            item.id = (char *)Memory_ArenaPush(&global_os->frame_arena, sizeof(rand_string));
            strcpy(item.id, rand_string);
            AddUIItem(rand_string, item);
        }
        
        for(u32 i =0; i< 256; ++i) {
            UI_Item *result = GetUIItem(generated_strings[i]);
            if(result == NULL) {
                printf("oops");
            }
        }
        // TODO(Cian): PushString method
    }
#endif
    
    // NOTE(Cian): Stuff to go in UpdateAndRender
    {
        
        f32 *my_float = (f32 *)Memory_ArenaPush(&global_os->permanent_arena, sizeof(float));
        *my_float = 23.0f;
        
        glViewport( 0, 0, global_os->display.width, global_os->display.height);
        glClearColor(1.0f,1.0f,1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        
        nvgBeginFrame(global_vg, (f32)global_os->display.width,  (f32)global_os->display.height, global_os->display.pixel_ratio);
        
        f32 nav_width = DIPToPixels(60);
        f32 main_panel_width = global_os->display.width - nav_width;
        //For example, this will be the same as a "StartToStartConstraint" 
        f32 title_pos_x = nav_width + DIPToPixels(30);
        f32 title_pos_y = DIPToPixels(60);
        f32 title_size = DIPToPixels(32);
        f32 title_panel_height = global_os->display.height * 0.25f;
        f32 title_min_height = title_pos_y + title_size + DIPToPixels(30);
        if(title_panel_height <= title_min_height)
        {
            title_panel_height = title_min_height;
        }
        
        f32 dashboard_item_width = DIPToPixels(400);
        f32 dashboard_item_height = DIPToPixels(300);
        f32 dashboard_item_margin = DIPToPixels(20);
        u32 dashboard_num_items = 4;
        
        UI_BeginWindow("main_window");
        // TODO(Cian): better way of getting parent instead of looking up twice?
        UI_StartToStartConstraint(PeekUIParent().id,0);
        UI_Width(60);
        // TODO(Cian): Also implement option to use TopToTop && BottomToBottom to calculate height, vice versa for width too
        //UI_Height((f32)PeekUIParent.height);
        UI_BottomToBottomConstraint(PeekUIParent().id, 0);
        UI_TopToTopConstraint(PeekUIParent().id, 0);
        UI_Panel("nav_bar");
#if 0
        //main panel
        UI_StartToStartConstraint("nav_bar",0);
        UI_EndToEndConstraint("nav_bar",0);//another option here is a UI_FillWidth method?
        UI_FillHeight();
        nvgBeginPath(global_vg);
        nvgRect(global_vg, nav_width,0,main_panel_width ,(f32)global_os->display.height);
        nvgFillColor(global_vg, nvgRGBA(80, 80, 80, 255));
        nvgFill(global_vg);
#endif
        
        //Title Panel
        nvgBeginPath(global_vg);
        nvgRect(global_vg, nav_width, 0, main_panel_width, title_panel_height);
        nvgFillColor(global_vg, nvgRGBA(150, 150, 150, 255));
        nvgFill(global_vg);
        
        //Title
        nvgFontSize(global_vg, title_size);
        nvgFontFace(global_vg, "roboto-bold");
        // NOTE(Cian): Aligning to the left/right means that the left/right (e.g beginning of text/ end of text) is positioned at the given coordinates
        nvgTextAlign(global_vg, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
        nvgFillColor(global_vg, nvgRGBA(255,255,255,255));
        f32 title_end_x = nvgText(global_vg, title_pos_x, title_pos_y, "Dashboards", NULL);
        
        //menu items
        f32 remaining_width = main_panel_width;
        f32 curr_row = 0;
        f32 pos_in_row = 0;
        f32 start_x = nav_width;
        f32 start_y = title_panel_height + dashboard_item_margin;
        //test centering items
#if 1   
        f32 item_size = (2 * dashboard_item_margin) + dashboard_item_width;
        u32 max_items_in_row = (u32)(remaining_width / item_size);
        
        f32 remaining = (f32)((u32)remaining_width % (u32)item_size);
        start_x = start_x + (remaining/2);
        remaining_width -= remaining;
        
        if(max_items_in_row < 1)
        {
            max_items_in_row = 1;
            start_x = nav_width;
        }
        
#endif  
        for (u32 i = 0;i < dashboard_num_items; i++){
            if(item_size > remaining_width && pos_in_row !=0)
            {
                curr_row++;
                pos_in_row = 0;
                remaining_width = main_panel_width - remaining;
            }
            
            f32 x = start_x + dashboard_item_margin + (pos_in_row * (dashboard_item_width + (2*dashboard_item_margin)));
            f32 y = start_y +dashboard_item_margin + (curr_row *(dashboard_item_height + dashboard_item_margin));
            f32 from_start = x - nav_width;
            f32 from_end = global_os->display.width - x;
            
            nvgBeginPath(global_vg);
            nvgRect(global_vg, x, y, dashboard_item_width, dashboard_item_height);
            nvgFillColor(global_vg, nvgRGBA(200, 200, 200, 255));
            nvgFill(global_vg);
            
            pos_in_row++;
            remaining_width -= dashboard_item_width + (2 * dashboard_item_margin); 
        }
#if 0
        // NOTE(Cian): NanoVG sample from Github, draws a RECT with a circle cut out
        {
            nvgBeginPath(global_vg);
            nvgRect(global_vg, 100,100, 120,30);
            nvgCircle(global_vg, 120,120, 5);
            nvgPathWinding(global_vg, NVG_HOLE);	// Mark circle as a hole.
            nvgFillColor(global_vg, nvgRGBA(255,192,0,255));
            nvgFill(global_vg);
        }
#endif
        nvgRestore(global_vg);
        
        nvgEndFrame(global_vg);
    }
}