#define UI_DEFAULT_DENSITY 96.00f
#define UI_HASH_SIZE 256
#define UI_MAX_WIDGETS 1024
#define UI_MAX_PANELS 24
#define UI_MAX_STACK  16

struct UI_Widget{
    // NOTE(Cian): properties declare the type and functionality of the UI_Component, e.g layout-> vertical, horizontal etc, widget -> slider, button, text etc
    u64 properties;
    V4 layout;
    V3 width_constraints;
    V3 height_constraints;
    NVGcolor color;
    // NOTE(Cian): index for the components in this layout
    struct{
        u32 start;
        u32 end;
    }children;
};

struct UI_State {
    // NOTE(Cian): Current in context widgets
    u32 widget_size;
    UI_Widget widgets[UI_MAX_WIDGETS];
    //UI_Panel panels[UI_MAX_PANELS];
    u32 panel_size;
    
    // NOTE(Cian): AutoLayout vals
    struct {
        V2 pos;
        V2 size;
        V3 width_constraints;
        V3 height_constraints;
        UI_Widget *parent;
    } auto_layout_state;
    
    struct {
        u32 x_pos_size;
        struct {
            f32 x;
        } x_pos_stack[UI_MAX_STACK];
    };
    struct {
        u32 y_pos_size;
        struct {
            f32 y;
        } y_pos_stack[UI_MAX_STACK];
    };
    
    struct {
        u32 width_size;
        struct {
            f32 width;
        } width_stack[UI_MAX_STACK];
    };
    
    struct {
        u32 height_size;
        struct {
            f32 height;
        } height_stack[UI_MAX_STACK];
    };
    struct {
        u32 pref_width_size;
        struct {
            f32 min, pref, max;
        } pref_width_stack[UI_MAX_STACK];
    };
    
    struct {
        u32 pref_height_size;
        struct {
            f32 min, pref, max;
        } pref_height_stack[UI_MAX_STACK];
    };
    struct {
        u32 parent_stack_size;
        struct{
            UI_Widget *widget;
        }parent_stack[UI_MAX_STACK];
    };
    
};

GLOBAL UI_State *ui_state;
#define TOKEN_PASTE(x, y) x##y
#define CAT(x,y) TOKEN_PASTE(x,y)
#define UNIQUE_INT CAT(prefix, __COUNTER__)
#define _UI_DEFER_LOOP(begin, end, var) for(int var  = (begin, 0); !var; ++var,end)
#define UI_BEGIN _UI_DEFER_LOOP(UI_Begin(), UI_End(), UNIQUE_INT)
#define UI_Row _UI_DEFER_LOOP(UI_PushRow(), UI_PopRow(), UNIQUE_INT)
#define UI_P(padding, fill_color, border_color)  _UI_DEFER_LOOP(UI_PushPanel(padding, fill_color, border_color), UI_PopPanel(), k)
#define UI_X(x) _UI_DEFER_LOOP(UI_PushX(x), UI_PopX(), UNIQUE_INT)
#define UI_Y(y) _UI_DEFER_LOOP(UI_PushY(y), UI_PopY(), UNIQUE_INT)
#define UI_Width(min,pref, max) _UI_DEFER_LOOP(UI_PushWidthConstraints(min,pref,max), UI_PopWidthConstraints(), UNIQUE_INT)
#define UI_WidthFill UI_Width(0,0, 1000000)
#define UI_WidthAuto UI_Width(0,0,0)
#define UI_Height(min,pref, max) _UI_DEFER_LOOP(UI_PushHeightConstraints(min,pref,max), UI_PopHeightConstraints(), UNIQUE_INT)
#define UI_HeightAuto UI_Height(0,0,0)
#define UI_HeightFill UI_HEIGHT(0,0,1000000)
#define UI_Pos(pos) _UI_DEFER_LOOP(UI_PushPos(pos), UI_PopPos(), UNIQUE_INT)

// NOTE(Cian): UI Utilities
INTERNAL f32 PixelsToDIP(f32 pixels);
INTERNAL f32 DIPToPixels(f32 dp);
#define UI_MIN_ROW_HEIGHT DIPToPixels(16);

INTERNAL void UI_Begin();
INTERNAL void UI_End();
INTERNAL void UI_PushWidthConstraints(f32 min, f32 pref, f32 max);
INTERNAL void UI_PopWidthConstraints();
INTERNAL void UI_PushHeightConstraints(f32 min, f32 pref, f32 max);
INTERNAL void UI_PopHeightConstraints();
INTERNAL void UI_TestBox(NVGcolor c);
INTERNAL void UI_PushRow();
INTERNAL void UI_PopRow();
INTERNAL void UI_PushPanel(V2 padding, NVGcolor fill_color, NVGcolor border_color);
INTERNAL void UI_PopPanel();
INTERNAL void UI_PushWidth(f32 width);
INTERNAL void UI_PushWidth(f32 width);
INTERNAL void UI_PushWidthAuto();
INTERNAL void UI_PopWidth();
INTERNAL void UI_PushHeight(f32 width);
INTERNAL void UI_PushHeightAuto();
INTERNAL void UI_PopHeight();
INTERNAL void UI_PushX(f32 x);
INTERNAL void UI_PopX();
INTERNAL void UI_PushPos(V2 pos);
INTERNAL void UI_PopPos();
INTERNAL void UI_PushButton();





