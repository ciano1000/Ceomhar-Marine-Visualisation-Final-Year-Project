
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
        // NOTE(Cian): BeginNavMenu pushes a closure, this closure will be called at UI_End, this closure will loop through the closures created in the block below and perform layout on them based on commands called here before UI_BeginNavMenu
        UI_BeginNavMenu("nav_menu",UI_VERTICAL, padding, margin);
        {
            PushClosure(UI_ImageButton("Home"));
            PushClosure(UI_ImageButton("Dashboards"));
        }
        UI_EndLayout();
        
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
        //UI_Height((f32)PeekUIParent.height);
        UI_BottomToBottomConstraint(PeekUIParent().id, 0);
        UI_TopToTopConstraint(PeekUIParent().id, 0);
        UI_Panel("nav_bar", nvgRGBA(40,40,40,255));
        
        
        UI_BeginWindow("main_window");
        UI_StartToStartConstraint(PeekUIParent().id,0);
        UI_Width(60);
        UI_BottomToBottomConstraint(PeekUIParent().id, 0);
        UI_TopToTopConstraint(PeekUIParent().id, 0);
        UI_BeginNavMenu("nav_menu", UI_VERTICAL, 60, 60, 10);
        {
            UI_PanelClosure("nav_1", nvgRGBA(89, 222, 195, 200));
            UI_PanelClosure("nav_2", nvgRGBA(89, 222, 195, 200));
            UI_PanelClosure("nav_3", nvgRGBA(89, 222, 195, 200));
        }
        UI_EndLayout();
        //main panel
        UI_StartToEndConstraint("nav_bar",0);
        UI_EndToEndConstraint(PeekUIParent().id,0);
        UI_BottomToBottomConstraint(PeekUIParent().id, 0);
        UI_TopToTopConstraint(PeekUIParent().id, 0);
        UI_Panel("main_panel",  nvgRGBA(80, 80, 80, 255));
        // TODO(Cian): Do FillHeight() and FillWidth
        //UI_FillHeight();
        // TODO(Cian): Make the main panel a BeginPanel() also create a BeginLayout() for when you don't want a background color for the layout.
        
        //Title Panel
        UI_StartToEndConstraint("nav_bar",0);
        UI_EndToEndConstraint(PeekUIParent().id,0);
        UI_TopToTopConstraint(PeekUIParent().id,0);
        UI_Height(PeekUIParent().height * 0.25f);
        UI_MinHeight(120);
        // TODO(Cian): add overload for MIN/MAX functions to be able to "fit content", this will probably require closures or some other form of deferral, so wait till we implement that for input before altering
        UI_Panel("title_panel",  nvgRGBA(150, 150, 150, 255));
        
        UI_StartToEndConstraint("nav_bar", 30);
        // TODO(Cian): CenterX/Y function to center in parent(with offset)
        UI_BottomToBottomConstraint("title_panel", 60);
        UI_Text("title_text", "Dashboard",  32,  nvgRGBA(255,255,255,255)); 
        UI_EndWindow();
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
        char *loop_id = "panel_item_";
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
            
            char id_buffer[14];
            
            snprintf(id_buffer, ArrayCount(id_buffer), "%s%d", loop_id, i);
            
            UI_SetX(x);
            UI_SetY(y);
            UI_Width(dashboard_item_width);
            UI_Height(dashboard_item_height);
            UI_Panel(id_buffer, nvgRGBA(200, 200, 200, 255));
            
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