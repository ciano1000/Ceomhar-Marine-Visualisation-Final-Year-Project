#define UI_DEFAULT_DENSITY 96.00f
#define UI_HASH_SIZE 256
#define UI_MAX_WIDGETS 1024
#define UI_MAX_STACK  16

// NOTE(Cian): Preliminary
enum UI_LayoutFlags {
    UI_ROW = 1 << 0,
    UI_COL = 1 << 1,
    UI_FLOW = 1 << 2,
    UI_AUTO_WIDTH = 1 << 3,
    UI_AUTO_HEIGHT = 1 << 4,
};

enum UI_SizingFlags {
}

struct UI_Widget {
    u64 properties;
    V4 layout;
    // TODO(Cian): more stuff...
};

struct UI_State {
    // NOTE(Cian): Current in context widgets
    u32 widget_size;
    UI_Widget widgets[UI_MAX_WIDGETS];
    
    // NOTE(Cian): AutoLayout vals
    struct {
        V2 pos;
        V2 size;
        V2 size_remaining;
        b32 auto_height;
        b32 auto_width;
        b32 fill_width;
        b32 fill_height;
        b32 is_col;
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
    u32 group_mode_stack_size;
    struct
    {
        b32 is_column;
    }
    group_mode_stack[UI_STACK_MAX];
};

GLOBAL UI_State *ui_state;

// NOTE(Cian): UI Utilities
INTERNAL f32 PixelsToDIP(f32 pixels);
INTERNAL f32 DIPToPixels(f32 dp);
#define UI_MIN_ROW_HEIGHT DIPToPixels(16);

INTERNAL void BeginUI();
INTERNAL void EndUI();
INTERNAL void BeginRow();
INTERNAL void EndRow();

#define UI_Begin _DEFER_LOOP(BeginUI(), EndUI())
#define UI_Row _DEFER_LOOP(BeginRow(), EndRow())



