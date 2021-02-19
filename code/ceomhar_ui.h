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

enum UI_WidgetProperty{
    UI_WidgetProperty_RenderBackground,
    UI_WidgetProperty_RenderBackgroundRounded,
    UI_WidgetProperty_Clickable,
    UI_WidgetProperty_RenderText,
    UI_WidgetProperty_RenderBorder,
    UI_WidgetProperty_RenderHot,
    UI_WidgetProperty_RenderActive,
    UI_WidgetProperty_Container,
    UI_WidgetProperty_LayoutHorizontal,
    UI_WidgetProperty_LayoutVertical,
    UI_WidgetProperty_MAX
};

enum UI_SizeParametersIndex {
    UI_ParameterIndex_Width,
    UI_ParameterIndex_Height
};

enum UI_LayoutIndices {
    UI_LayoutIndex_X,
    UI_LayoutIndex_Y,
    UI_LayoutIndex_Width,
    UI_LayoutIndex_Height
};

struct UI_SizeParameters {
    b32 is_ratio;
    union {
        f32 size;
        f32 ratio;
    };
    f32 strictness;
};

struct UI_ID {
    s32 hash;
    u32 table_pos;
};

struct UI_Widget{
    // NOTE(Cian): properties declare the type and functionality of the UI_Component, e.g layout-> vertical, horizontal etc, widget -> slider, button, text etc
    u64 properties[UI_WidgetProperty_MAX / 64 + 1];
    String string;
    UI_Widget *hash_next;
    UI_Widget *tree_next_sibling;
    UI_Widget *tree_prev_sibling;
    UI_Widget *tree_first_child;
    UI_Widget *tree_last_child;
    UI_Widget *tree_parent;
    UI_ID id;
    u64 last_frame;
    V4 curr_layout;
    V4 old_layout;
    UI_SizeParameters parameters[2];
    V4 padding;
    NVGcolor color;
    NVGcolor text_color;
    f32 font_size;
    
    f32 hot_transition;
    f32 active_transition;
};

struct UI_State {
    // NOTE(Cian): Current in context widgets
    u32 widget_size;
    UI_Widget *widgets[UI_MAX_WIDGETS];
    UI_Widget *root_widget;
    UI_Widget *prev_widget;
    u64 curr_frame;
    UI_ID hot;
    UI_ID active;
    u32 panel_size;
#define UI_STACK(name, type) \
struct { \
u32 size;\
type stack[256];\
type current;\
} name##_stack
    
    UI_STACK(parent, UI_Widget*);
    UI_STACK(width, UI_SizeParameters);
    UI_STACK(height, UI_SizeParameters);
    UI_STACK(padding, V4);
    u32 non_interactive_count;
};

struct UI_RecursiveQueue {
    u32 front;
    u32 rear;
    u32 size;
    UI_Widget *widget_queue[UI_MAX_WIDGETS];
};

// NOTE(Cian): Macro to handle varargs boilerplate
#define UI_MakeFormatString(string, format) \
va_list args;\
va_start(args, format);\
string = String_MakeString(&global_os->frame_arena, format, args);\
va_end(args);

global UI_State *ui_state;
#define _UI_DEFER_LOOP(begin, end, var) for(int var  = (begin, 0); !var; ++var,end)
#define UI_BeginUI _UI_DEFER_LOOP(UI_Begin(), UI_End(), UNIQUE_INT)
#define UI_Row _UI_DEFER_LOOP(UI_BeginRow(0), UI_EndRow(), UNIQUE_INT)
#define UI_Col _UI_DEFER_LOOP(UI_BeginColumn(0), UI_EndColumn(), UNIQUE_INT)
#define UI_Padding4(pad_v4)  _UI_DEFER_LOOP(UI_PushPadding(pad_v4), UI_PopPadding(), UNIQUE_INT)
#define UI_Padding2(pad_v2)  _UI_DEFER_LOOP(UI_PushPadding(v4(pad_v2.x, pad_v2.y, pad_v2.x, pad_v2.y)), UI_PopPadding(), UNIQUE_INT)
#define UI_X(x) _UI_DEFER_LOOP(UI_PushX(x), UI_PopX(), UNIQUE_INT)
#define UI_Y(y) _UI_DEFER_LOOP(UI_PushY(y), UI_PopY(), UNIQUE_INT)
#define UI_Width(size, strictness) _UI_DEFER_LOOP(UI_PushWidth(size, strictness), UI_PopWidth(), UNIQUE_INT)
#define UI_WidthRatio(ratio, strictness) _UI_DEFER_LOOP(UI_PushWidthRatio(ratio, strictness), UI_PopWidth(), UNIQUE_INT)
#define UI_WidthFill UI_Width(UI_MAX_SIZE, 0.0f)
#define UI_WidthAuto UI_Width(0.0f, 0.0f)
#define UI_Height(size, strictness) _UI_DEFER_LOOP(UI_PushHeight(size, strictness), UI_PopHeight(), UNIQUE_INT)
#define UI_HeightRatio(ratio, strictness) _UI_DEFER_LOOP(UI_PushHeightRatio(ratio, strictness), UI_PopHeight(), UNIQUE_INT)
#define UI_HeightAuto UI_Height(0.0f, 0.0f)
#define UI_HeightFill UI_Height(UI_MAX_SIZE, 0.0f)
#define UI_Pos(pos) _UI_DEFER_LOOP(UI_PushPos(pos), UI_PopPos(), UNIQUE_INT)
#define UI_Panel(panel_string, color) _UI_DEFER_LOOP(UI_BeginPanel(panel_string, color), UI_EndPanel(), UNIQUE_INT)
#define UI_Filler(factor) UI_Spacer(factor * UI_MAX_SIZE, 0.0f)


#define UI_MIN_ROW_HEIGHT DIPToPixels(16);

// NOTE(Cian): Functions, wish I didn't have to do this shit, hoping Jai comes out or any other viable replacement for C/C++
internal void UI_PushParent(UI_Widget *parent);
internal void UI_PopParent();
internal void UI_PushWidth(f32 size, f32 strictness);
internal void UI_PopWidth();
internal void UI_PushHeight(f32 size, f32 strictness);
internal void UI_PopHeight();
internal void UI_Enqueue(UI_RecursiveQueue *queue, UI_Widget *widget);
internal UI_Widget* UI_Dequeue(UI_RecursiveQueue *queue);




