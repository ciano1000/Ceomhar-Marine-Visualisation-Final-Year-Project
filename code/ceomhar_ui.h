#define UI_DEFAULT_DENSITY 96.00f
#define UI_HASH_SIZE 256
#define UI_MAX_WIDGETS 1024
#define UI_MAX_PANELS 24
#define UI_MAX_STACK  16

#define _UI_DEFER_LOOP(begin, end, var) for(int _##var##_ = (begin, 0); !_##var##_; ++_##var##_,end)
#define UI_BEGIN _UI_DEFER_LOOP(UI_Begin(), UI_End(), i)
#define UI_ROW _UI_DEFER_LOOP(UI_PushRow(), UI_PopRow(), j)
#define UI_PANEL _UI_DEFER_LOOP(UI_PushPanel(), UI_PopPanel(), k)
#define UI_X(x) _UI_DEFER_LOOP(UI_PushX(x), UI_PopX(), l)
#define UI_Y(y) _UI_DEFER_LOOP(UI_PushY(y), UI_PopY(), m)
#define UI_WIDTH(w) _UI_DEFER_LOOP(UI_PushWidth(w), UI_PopWidth(), n)
#define UI_WIDTH_FILL UI_WIDTH(ui_state->auto_layout_state.size_remaining.width)
#define UI_WIDTH_AUTO _UI_DEFER_LOOP(UI_PushWidthAuto(), UI_PopWidth(), p)
#define UI_HEIGHT(h) _UI_DEFER_LOOP(UI_PushHeight(h), UI_PopHeight(), o)
#define UI_HEIGHT_AUTO _UI_DEFER_LOOP(UI_PushHeightAuto(), UI_PopHeight(), q)
#define UI_HEIGHT_FILL UI_HEIGHT(ui_state->auto_layout_state.size_remaining.height)
#define UI_POS(pos) _UI_DEFER_LOOP(UI_PushPos(pos), UI_PopPos(), r)
#define UI_PAD(pad) _UI_DEFER_LOOP(UI_PushPadding(pad), UI_PopPad(), s)
#define UI_SIZE(s) _UI_DEFER_LOOP(UI_PushSize(s), UI_PopSize(), t)
// TODO(Cian):  Get Cols working #define UI_Column(pos) _UI_DEFER_LOOP(UI_PushPos(pos), UI_PopPos(), r)


// NOTE(Cian): Preliminary
enum UI_LayoutFlags {
    UI_ROW = 1 << 0,
    UI_COL = 1 << 1,
    UI_FLOW = 1 << 2,
    UI_AUTO_WIDTH = 1 << 3,
    UI_AUTO_HEIGHT = 1 << 4,
};

struct UI_Panel {
    V4 layout;
    V4 padding;
    b32 has_title;
    char title[64];
    // TODO(Cian): Eventually just use a V4 with our renderer
    NVGcolor main_color;
    NVGcolor border_color;
};

struct UI_Widget {
    u64 properties;
    V4 layout;
    // TODO(Cian): more stuff...
};

struct UI_State {
    // NOTE(Cian): Current in context widgets
    u32 widget_size;
    UI_Widget widgets[UI_MAX_WIDGETS];
    u32 panel_size;
    UI_Panel panels[UI_MAX_PANELS];
    
    
    // NOTE(Cian): AutoLayout vals
    struct {
        V2 pos;
        V2 size;
        V2 padding;
        // TODO(Cian): Handle this in our Push_DIMENSION functions
        V2 size_remaining;
        // TODO(Cian): maybe to save memory/cpu time, convert these to a single bit field
        b32 auto_height;
        b32 auto_width;
        b32 fill_width;
        b32 fill_height;
        b32 is_col;
        u32 start, end;
    } auto_layout_state;;
    
    // NOTE(Cian): Layout Stacks
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
            f32 width_remaining;
            b32 auto_width;
            b32 fill_width;
        } width_stack[UI_MAX_STACK];
    };
    
    struct {
        u32 height_size;
        struct {
            f32 height;
            f32 height_remaining;
            b32 auto_height;
            b32 fill_height;
        } height_stack[UI_MAX_STACK];
    };
    
    struct {
        u32 child_widgets_size;
        struct {
            u32 start;
            u32 end;
        } child_widgets_stack[UI_MAX_STACK];
    };
    struct{
        u32 group_mode_stack_size;
        struct {
            b32 is_column;
        }group_mode_stack[UI_MAX_STACK];
    };
    
    struct{
        u32 padding_stack_size;
        struct {
            V2 padding;
        }padding_stack[UI_MAX_STACK];
    };
    
};

GLOBAL UI_State *ui_state;

// NOTE(Cian): UI Utilities
INTERNAL f32 PixelsToDIP(f32 pixels);
INTERNAL f32 DIPToPixels(f32 dp);
#define UI_MIN_ROW_HEIGHT DIPToPixels(16);

INTERNAL void UI_Begin();
INTERNAL void UI_End();
INTERNAL void UI_PushRow();
INTERNAL void UI_PushRow(V2 size);
INTERNAL void UI_PushRow(V2 pos, V2 size);
INTERNAL void UI_PopRow();
INTERNAL void UI_PushPanel(V2 padding);
INTERNAL void UI_PopPanel();
INTERNAL void UI_PushWidth(f32 width, b32 auto_width);
INTERNAL void UI_PushWidth(f32 width);
INTERNAL void UI_PushWidthAuto();
INTERNAL void UI_PopWidth();
INTERNAL void UI_PushHeight(f32 width);
INTERNAL void UI_PushHeightAuto();
INTERNAL void UI_PopHeight();
INTERNAL void UI_PushPos(V2 pos);
INTERNAL void UI_PopPos();
INTERNAL void UI_PushSize(V2 size);
INTERNAL void UI_PopSize();
INTERNAL void UI_PushGroupMode(b32 column);
INTERNAL void UI_PopGroupMode();
INTERNAL void UI_PushButton();
INTERNAL void UI_PushPadding(V2 padding);
INTERNAL void UI_PopPadding();
INTERNAL void UI_PushWidgetIndices();
INTERNAL void UI_PopWidgetIndices();
INTERNAL void UI_IncrementWidgetIndices();




