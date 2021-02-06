#define UI_DEFAULT_DENSITY 96.00f
#define UI_HASH_SIZE 256
#define UI_MAX_WIDGETS 1024
#define UI_MAX_PANELS 24
#define UI_MAX_STACK  16

#define UI_MAX_SIZE 1000000
#define UI_MIN_SIZE 0

enum UI_WidgetProperty{
    UI_WidgetProperty_RenderBackground,
    UI_WidgetProperty_Container,
    UI_WidgetProperty_LayoutHorizontal,
    UI_WidgetProperty_LayoutVertical,
    UI_WidgetProperty_MAX
};

struct UI_SizeParameters {
    b32 is_ratio;
    union {
        f32 size;
        f32 ratio;
    };
    f32 strictness;
};

struct UI_Widget{
    // NOTE(Cian): properties declare the type and functionality of the UI_Component, e.g layout-> vertical, horizontal etc, widget -> slider, button, text etc
    u64 properties[UI_WidgetProperty_MAX / 64 + 1];
    // TODO(Cian): Need to do a proper UI id thingy
    char *string;
    UI_Widget *hash_next;
    UI_Widget *tree_next_sibling;
    UI_Widget *tree_prev_sibling;
    UI_Widget *tree_first_child;
    UI_Widget *tree_last_child;
    UI_Widget *tree_parent;
    V4 curr_layout;
    V4 old_layout;
    UI_SizeParameters widths;
    UI_SizeParameters heights;
    NVGcolor color;
};

struct UI_State {
    // NOTE(Cian): Current in context widgets
    u32 widget_size;
    UI_Widget *widgets[UI_MAX_WIDGETS];
    UI_Widget *root_widget;
    UI_Widget *prev_widget;
    //UI_Panel panels[UI_MAX_PANELS];
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
};

struct UI_RecursiveQueue {
    u32 front;
    u32 rear;
    u32 size;
    UI_Widget *widget_queue[UI_MAX_WIDGETS];
};

global UI_State *ui_state;
#define TOKEN_PASTE(x, y) x##y
#define CAT(x,y) TOKEN_PASTE(x,y)
#define UNIQUE_INT CAT(prefix, __COUNTER__)

#define _UI_DEFER_LOOP(begin, end, var) for(int var  = (begin, 0); !var; ++var,end)
#define UI_BeginUI _UI_DEFER_LOOP(UI_Begin(), UI_End(), UNIQUE_INT)
#define UI_Row _UI_DEFER_LOOP(UI_PushRow(), UI_PopRow(), UNIQUE_INT)
#define UI_P(padding, fill_color, border_color)  _UI_DEFER_LOOP(UI_PushPanel(padding, fill_color, border_color), UI_PopPanel(), k)
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
#define UI_Panel(panel_string) _UI_DEFER_LOOP(UI_BeginPanel(panel_string), UI_EndPanel(), UNIQUE_INT)

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




