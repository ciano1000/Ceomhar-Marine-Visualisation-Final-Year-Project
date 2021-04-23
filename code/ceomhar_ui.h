#define UI_OS_DEFAULT_DENSITY 96.00f

#define UI_WIDGET_TABLE_SIZE 1024
#define UI_MAX_CONTAINERS 256

#define UI_MAX_SIZE 1000000
#define UI_AUTO_SIZE 0

#define UI_MAKE_COLOR_STYLE(normal_back, normal_text, normal_border, hot_back, hot_text, hot_border, active_back, active_text, active_border) {{normal_back, normal_text, normal_border}, { hot_back, hot_text, hot_border}, {active_back, active_text, active_border}} 

#define UI_WHITE       nvgRGBA(225,225,225,255)
#define UI_DARKEST     nvgRGBA(0,0,0,255)
#define UI_DARKER      nvgRGBA(18,18,18,255)
#define UI_DARK        nvgRGBA(30,30,30,255)
#define UI_LIGHT       nvgRGBA(100,100,100,255)
#define UI_PURPLE      nvgRGBA(187,134,252,255)
#define UI_PINK        nvgRGBA(239,183,255,255)
#define UI_DARK_PURPLE nvgRGBA(136,88,200,255)
#define UI_AQUA        nvgRGBA(3,218,198,255)
#define UI_RED         nvgRGBA(207,102,121,255)

#define UI_WINDOW_FILL_WIDTH ui->window_stack.current->curr_layout.width
#define UI_WINDOW_FILL_HEIGHT ui->window_stack.current->curr_layout.height

#define UI_WINDOW_RATIO_HEIGHT(ratio) ui->window_stack.current->curr_layout.height * ratio
#define UI_WINDOW_RATIO_WIDTH(ratio) ui->window_stack.current->curr_layout.width * ratio

enum UI_Widget_Property{
    UI_Widget_Property_RenderBackground,
    UI_Widget_Property_RenderActive,
    UI_Widget_Property_RenderHot,
    UI_Widget_Property_RenderText,
    UI_Widget_Property_RenderBorder,
    UI_Widget_Property_RenderBorderHot,
    UI_Widget_Property_RenderCloseButton,
    UI_Widget_Property_RenderSplit,
    UI_Widget_Property_RenderTitleBar, //draggable containers must have this, static can also have them
    
    UI_Widget_Property_MainWindow,
    
    UI_Widget_Property_Clickable,
    UI_Widget_Property_Togglable,
    UI_Widget_Property_EditText,
    //Used for both lists and containers
    UI_Widget_Property_ScrollVertical, 
    UI_Widget_Property_ScrollHorizontal, 
    
    UI_Widget_Property_Container, // e.g. a window
    UI_Widget_Property_Container_SizeAuto_Width, // e.g. a window
    UI_Widget_Property_Container_SizeAuto_Height, // e.g. a window
    
    UI_Widget_Property_LayoutHorizontal,
    UI_Widget_Property_LayoutVertical,
    UI_Widget_Property_Spacer,
    
    UI_Widget_Property_InstantLayout,//for widgets that don't need to be laid at the end of the frame
    
    UI_Widget_Property_ResizeLeft,
    UI_Widget_Property_ResizeRight,
    UI_Widget_Property_ResizeBottom,
    UI_Widget_Property_DraggingTitle,
    UI_Widget_Property_CloseButtonHot,
    
    //~Container Options
    UI_Widget_Property_Draggable,
    UI_Widget_Property_Resizable,
    
    UI_Widget_Property_CustomUpdate,
    UI_Widget_Property_CustomLayout,
    UI_Widget_Property_CustomRender,
    UI_Widget_Property_MAX
};

#define UI_PARAM_IS_RATIO(size_parameters) (size_parameters.pref < 1 && size_parameters.pref > 0) ? true : false
#define UI_PARAM_IS_AUTO(size_parameters) (size_parameters.pref == 0 && size_parameters.min == 0 && size_parameters.max == 0) ? true : false

enum UI_Widget_Color_State {
    UI_ColorState_Normal,
    UI_ColorState_Hot,
    UI_ColorState_Active,
};

struct UI_Widget_Colors {
    NVGcolor background_color;
    NVGcolor text_color;
    NVGcolor border_color;
};
// TODO(Cian): @UI add styling functionality
struct UI_Widget_Style {
    UI_Widget_Colors colors[3];  // normal, hot, active
    f32 border_thickness;
    f32 radius;
    f32 font_size;
    V4 padding;
    f32 title_height;
};


enum UI_Widget_Styles {
    UI_Widget_Style_Default,
    UI_Widget_Style_DefaultButton,
    UI_Widget_Style_DefaultLayout,
    UI_Widget_Style_MAX
};

static UI_Widget_Style widget_style_table[UI_Widget_Style_MAX] = {
    {UI_MAKE_COLOR_STYLE(UI_DARK, UI_WHITE, UI_LIGHT, UI_LIGHT, UI_WHITE, UI_WHITE, UI_DARKEST, UI_WHITE, UI_LIGHT), 4.0f, 4.0f, 16.0f, {10.0f, 10.0f, 10.0f, 10.0f}, 30.0f},
    {UI_MAKE_COLOR_STYLE(UI_PURPLE, UI_WHITE, {}, UI_PINK, UI_WHITE, {}, UI_DARK_PURPLE, UI_WHITE, {}), 0.0f, 2.0f, 14.0f, {2.0f, 2.0f, 2.0f, 2.0f}, 0.0f},
    {UI_MAKE_COLOR_STYLE({}, {}, {}, {}, {}, {}, {}, {}, {}), 0.0f, 0.0f, 0.0f, {2.0f, 2.0f, 2.0f, 2.0f}, 0.0f},
};

enum UI_ID_Property {
    UI_ID_Property_Regular,
    UI_ID_Property_NonInteractable,
    UI_ID_Property_Null,
};

struct UI_ID {
    u32 hash;
    UI_ID_Property property;
};

// TODO(Cian): @UI implement all these options
enum UI_ContainerOptions {
    UI_ContainerOptions_NoResize = 1 << 0,
    UI_ContainerOptions_NoTitle = 1 << 1,
    UI_ContainerOptions_NoMove = 1 << 2,
    UI_ContainerOptions_Center = 1 << 4,
    UI_ContainerOptions_AlignRight = 1 << 5,
    UI_ContainerOptions_Popup = 1 << 6,
};

struct UI_Widget{
    // NOTE(Cian): properties declare the type and functionality of the UI_Component, e.g layout-> vertical, horizontal etc, widget -> slider, button, text etc
    u64 properties[UI_Widget_Property_MAX / 64 + 1];
    String8 string;
    //~ NOTE(Cian) we reuse the hash_next pointer as the free-list linked list pointer
    UI_Widget *hash_next;
    UI_Widget *tree_next_sibling;
    UI_Widget *tree_prev_sibling;
    UI_Widget *tree_first_child;
    UI_Widget *tree_last_child;
    UI_Widget *tree_parent;
    UI_Widget *window_parent;
    
    UI_Widget *next_sorted_container;
    UI_Widget *prev_sorted_container;
    UI_Widget *front_child;
    
    UI_ID id;
    u64 last_frame;
    V4 curr_layout;
    V4 old_layout;
    V3 parameters[2];
    UI_Widget_Style *style;
    
    String8 edit_text;
    // NOTE(Cian): The scroll offsets are for the containers(e.g. windows), they then apply these offsets to their childrens position
    f32 scroll_offset_x;
    f32 scroll_offset_y;
    V4 splitter_rect;
};

struct UI_Layout_Stack_Entry {
    UI_Widget *widget;
    UI_Widget *last_child;
    u32 num_expand;
    f32 offset;
    f32 sum_size;
    f32 min_sum_delta;
    f32 max_sum_delta;
};

struct UI_State {
    u32 widget_size;
    UI_Widget *widgets[UI_WIDGET_TABLE_SIZE];
    s32 widget_free_list_count;
    UI_Widget *widget_free_list;
    
    UI_Widget *sorted_containers_start;
    UI_Widget *sorted_containers_end;
    UI_Widget *active_window; //set when a window is created && reset when a window ends
    UI_Widget *clickable_window;
    
    UI_Widget *root_widget;
    UI_Widget *prev_widget;
    
    u64 curr_frame;
    
    UI_ID hot;
    UI_ID active;
    
#define UI_STACK(name, type) \
struct { \
u32 size;\
type stack[256];\
type current;\
} name##_stack
    
    UI_STACK(parent, UI_Widget*);
    UI_STACK(window, UI_Widget*);
    UI_STACK(width, V3);
    UI_STACK(height, V3);
    UI_STACK(padding, V4);
    // NOTE(Cian): Editable string stuff, the current in-focus string is referenced here and operated on at the start of each frame
    // TODO(Cian): @UI @String implement text boxes based on below
    struct {
        String8 *text;
        u32 max_length;
        u32 cursor_idx;
        u32 selected_offset;
    } focused_text; 
};

#define MAKE_FORMAT_STRING(string, format) \
va_list args;\
va_start(args, format);\
string = string_make(&os->frame_arena, format, args);\
va_end(args);
#define _UI_DEFER_LOOP(begin, end, var) for(int var  = (begin, 0); !var; ++var,end)
#define UI _UI_DEFER_LOOP(ui_begin(), ui_end(), UNIQUE_INT)
#define Row _UI_DEFER_LOOP(ui_begin_row(0), ui_end_row(), UNIQUE_INT)
#define Col _UI_DEFER_LOOP(ui_begin_column(0), ui_end_column(), UNIQUE_INT)
#define PADDING4(pad_v4)  _UI_DEFER_LOOP(ui_push_padding(pad_v4), ui_pop_padding(), UNIQUE_INT)
#define PADDING2(pad_v2)  _UI_DEFER_LOOP(ui_push_padding(v4(pad_v2.x, pad_v2.y, pad_v2.x, pad_v2.y)), ui_pop_padding(), UNIQUE_INT)
#define Width(min, pref, max) _UI_DEFER_LOOP(ui_push_width(min, pref, max), ui_pop_width(), UNIQUE_INT)
#define Width_Fill Width(0.0f, 0.0f, UI_MAX_SIZE)
#define Width_Auto Width(UI_AUTO_SIZE, UI_AUTO_SIZE, UI_AUTO_SIZE)
#define Height(min, pref, max) _UI_DEFER_LOOP(ui_push_height(min, pref, max), ui_pop_height(), UNIQUE_INT)
#define Height_Auto  Height(UI_AUTO_SIZE, UI_AUTO_SIZE, UI_AUTO_SIZE)
#define Height_Fill Height(0.0f, 0.0f, factor * UI_MAX_SIZE)
#define Filler(factor) ui_spacer(0.0f, 0.0f, factor * UI_MAX_SIZE)

// NOTE(Cian): Functions, wish I didn't have to do this shit, hoping Jai comes out or any other viable replacement for C/C++


// TODO(Cian): Organise this into some kinda "Core" struct
global UI_State *ui;