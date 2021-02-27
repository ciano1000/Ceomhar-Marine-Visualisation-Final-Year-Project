namespace UI {
    
#define UI_DEFAULT_DENSITY 96.00f
    
    
#define UI_HASH_SIZE 256
#define UI_NON_INTERACTABLE_ID -2
#define UI_NullID {0,0}
    
#define UI_MAX_WIDGETS 1024
#define UI_MAX_PANELS 24
#define UI_MAX_STACK  16
    
#define UI_MAX_SIZE 1000000
#define UI_MIN_SIZE 0
    
#define UI_DEFAULT_TEXT_PADDING_X 16.0f
#define UI_DEFAULT_TEXT_PADDING_Y 10.0f
    
#define UI_MIN_TEXT_PADDING_X 4.0f
#define UI_MIN_TEXT_PADDING_Y 4.0f
    
#define UI_BORDER_SIZE 2.0f
    
#define UI_DEFAULT_FONT_SIZE 16.0f
#define UI_LARGE_FONT_SIZE 24.0f
    
#define DEFAULT_TEXT_COLOR nvgRGBA(225,225,225,255)
#define DARK_TEXT_COLOR nvgRGBA(0,0,0,255)
#define PRIMARY_COLOR nvgRGBA(18,18,18,255)
#define PRIMARY_COLOR_DARK nvgRGBA(0,0,0,255)
#define PRIMARY_COLOR_LIGHT nvgRGBA(50,50,50,255)
#define SECONDARY_COLOR nvgRGBA(187,134,252,255)
#define SECONDARY_COLOR_LIGHT nvgRGBA(239,183,255,255)
#define SECONDARY_COLOR_DARK nvgRGBA(136,88,200,255)
#define HIGHLIGHT_COLOR nvgRGBA(3,218,198,255)
#define HIGHLIGHT_COLOR_2 nvgRGBA(207,102,121,255)
    
#define DEFAULT_ROUNDNESS 4
    
    enum Widget_Property{
        Widget_Property_RenderBackground,
        Widget_Property_RenderBackgroundRounded,
        Widget_Property_Clickable,
        Widget_Property_RenderText,
        Widget_Property_RenderBorder,
        Widget_Property_RenderHot,
        Widget_Property_RenderActive,
        Widget_Property_Container,
        Widget_Property_LayoutHorizontal,
        Widget_Property_LayoutVertical,
        // TODO(Cian): @UI These are temporary properties until custom update & render is working
        Widget_Property_Togglable,
        Widget_Property_Scrollable,
        
        Widget_Property_CustomUpdate,
        Widget_Property_CustomRender,
        Widget_Property_MAX
    };
    
    enum Size_Parameters_Idx {
        UI_ParameterIndex_Width,
        UI_ParameterIndex_Height
    };
    
    enum Layout_Idx {
        UI_LayoutIndex_X,
        UI_LayoutIndex_Y,
        UI_LayoutIndex_Width,
        UI_LayoutIndex_Height
    };
    
    struct Size_Parameters {
        b32 is_ratio;
        union {
            f32 size;
            f32 ratio;
        };
        f32 strictness;
    };
    
    struct ID {
        s32 hash;
        u32 table_pos;
    };
    
    struct Widget{
        // NOTE(Cian): properties declare the type and functionality of the UI_Component, e.g layout-> vertical, horizontal etc, widget -> slider, button, text etc
        u64 properties[Widget_Property_MAX / 64 + 1];
        String8 string;
        Widget *hash_next;
        Widget *tree_next_sibling;
        Widget *tree_prev_sibling;
        Widget *tree_first_child;
        Widget *tree_last_child;
        Widget *tree_parent;
        ID id;
        u64 last_frame;
        V4 curr_layout;
        V4 old_layout;
        Size_Parameters parameters[2];
        V4 padding;
        NVGcolor color;
        NVGcolor text_color;
        f32 font_size;
    };
    struct State {
        // NOTE(Cian): Current in context widgets
        u32 widget_size;
        Widget *widgets[UI_MAX_WIDGETS];
        Widget *root_widget;
        Widget *prev_widget;
        u64 curr_frame;
        ID hot;
        ID active;
        u32 panel_size;
#define UI_STACK(name, type) \
struct { \
u32 size;\
type stack[256];\
type current;\
} name##_stack
        
        UI_STACK(parent, Widget*);
        UI_STACK(width, Size_Parameters);
        UI_STACK(height, Size_Parameters);
        UI_STACK(padding, V4);
        u32 non_interactive_count;
    };
    
#define MAKE_FORMAT_STRING(string, format) \
va_list args;\
va_start(args, format);\
string = String::make(&global_os->frame_arena, format, args);\
va_end(args);
#define _UI_DEFER_LOOP(begin, end, var) for(int var  = (begin, 0); !var; ++var,end)
#define BEGIN_UI _UI_DEFER_LOOP(begin(), end(), UNIQUE_INT)
#define ROW _UI_DEFER_LOOP(begin_row(0), end_row(), UNIQUE_INT)
#define COL _UI_DEFER_LOOP(begin_column(0), end_column(), UNIQUE_INT)
#define PADDING4(pad_v4)  _UI_DEFER_LOOP(push_padding(pad_v4), pop_padding(), UNIQUE_INT)
#define PADDING2(pad_v2)  _UI_DEFER_LOOP(push_padding(v4(pad_v2.x, pad_v2.y, pad_v2.x, pad_v2.y)), pop_padding(), UNIQUE_INT)
#define WIDTH(size, strictness) _UI_DEFER_LOOP(push_width(size, strictness), pop_width(), UNIQUE_INT)
#define WIDTH_RATIO(ratio, strictness) _UI_DEFER_LOOP(push_width_ratio(ratio, strictness), pop_width(), UNIQUE_INT)
#define WIDTH_FILL WIDTH(UI_MAX_SIZE, 0.0f)
#define WIDTH_AUTO WIDTH(0.0f, 0.0f)
#define HEIGHT(size, strictness) _UI_DEFER_LOOP(push_height(size, strictness), pop_height(), UNIQUE_INT)
#define HEIGHT_RATIO(ratio, strictness) _UI_DEFER_LOOP(push_height_ratio(ratio, strictness), pop_height(), UNIQUE_INT)
#define HEIGHT_AUTO  HEIGHT(0.0f, 0.0f)
#define HEIGHT_FILL HEIGHT(UI_MAX_SIZE, 0.0f)
#define PANEL(panel_string, color) _UI_DEFER_LOOP(begin_panel(panel_string, color), end_panel(), UNIQUE_INT)
#define FILLER(factor) spacer(factor * UI_MAX_SIZE, 0.0f)
    
    
#define UI_MIN_ROW_HEIGHT DIPToPixels(16);
    
    // NOTE(Cian): Functions, wish I didn't have to do this shit, hoping Jai comes out or any other viable replacement for C/C++
    internal void push_parent(Widget *parent);
    internal void pop_parent();
    internal void push_width(f32 size, f32 strictness);
    internal void pop_width();
    internal void push_height(f32 size, f32 strictness);
    internal void pop_height();
}
// TODO(Cian): Organise this into some kinda "Core" struct
global UI::State *ui_state;