
void AppStart(OS_State *state, NVGcontext *vg) {
    // TODO(Cian): when app and platform are split into seperate TU's, put OS_State stuff here
    
}

// TODO(Cian): How should we pass the vgContext???
void AppUpdateAndRender() {
    
    ui_state = UI_InitState();
    
    // NOTE(Cian): Trying out UI stuff
#if 0
    {
        UI_Begin(); // sets up viewport, clears screen,
        // NOTE(Cian): UI_PARENT is a macro that resolves to some special value e.g -1 that marks this id to point to whatever parent is in context
        UI_StartToStartConstraint(UI_PARENT, 16);
        UI_Width(30);
        UI_TopToTopConstraint(UI_PARENT, 16);
        // NOTE(Cian): BeginNavMenu pushes a closure, this closure will be called at UI_End, this closure will loop through the closures created in the block below and perform layout on them based on commands called here before UI_BeginNavMenu
        UI_BeginLinearLayout("nav_menu",UI_VERTICAL, padding, margin);
        {
            UI_ImageButton("Home", UI_OpenHomeClosure());
            UI_ImageButton("Dashboards", UI_OpenDashboardClosure());
            UI_ImageButton("Data", UI_OpenDataClosure());
        }
        UI_EndLayout();
        
        UI_CenterX(0);
        UI_CenterY(0);
        UI_WrapContent();
        UI_Text("Some Title##title_id", 32, nvgRGBA(23,243,12,233)); 
        
        UI_Workspace("main_panel", menu_item_closure, menu_item_list));
        
        UI_EndToEndConstraint(UI_ID, offset);
        UI_EndToStartConstraint(UI_PARENT, offset);
        UI_TopToTopConstraint(...);
        UI_CenterX(offset);
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
        
        // TODO(Cian): Instead of constantly looking up the same id, is there a better way?
        UI_BeginWindow("main_window");
        
        UI_StartToStartConstraint("@p",0);
        UI_Width(60);
        UI_BottomToBottomConstraint("@p", 0);
        UI_TopToTopConstraint("@p", 0);
        UI_Panel("nav_bar", nvgRGBA(40,40,40,255));
        
        u32 choice = 0;
        char *menu_labels[] = {
            "Home","Dashboards","Widgets"
        };
        
        // TODO(Cian): Having to do this id trick makes a case for having menu items be managed by another widget
        char constraint_id[8];
        strcpy(constraint_id, "@p");
        for(u32 i = 0; i < ArrayCount(menu_labels); ++i) {
            char *id_base = "test_%d";
            char id_buffer[8];
            snprintf(id_buffer,8,id_base, i);
            
            // TODO(Cian): This is a case where centering would be nice (-:
            // TODO(Cian): Font size and width should be separate, should have commands like UI_FontSize(14), UI_FitContent(), UI_WrapContent
            UI_StartToStartConstraint("@p",0);
            UI_Width(60);
            UI_Height(60);
            UI_SetY((f32)i*60);
            UI_Button(id_buffer,nvgRGBA(0,255,255,255));
            
            strcpy(constraint_id, id_buffer);
        }
        
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
            UI_BeginPanel(id_buffer, nvgRGBA(200, 200, 200, 255));
            {
                UI_DrawGraph_Test();
            }
            UI_EndPanel();
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