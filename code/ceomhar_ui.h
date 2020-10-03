#define UI_DEFAULT_DENSITY 96.00f
#define UI_HASH_SIZE 256
#define UI_ID_ESCAPE '@'
#define UI_PARENT 'p'

// TODO(Cian): Pull this out into it's DataDesk generated ceomhar_closure.h
#define MAX_CLOSURE_ARGS 16

/* 
     * NOTE(Cian): Every UI Item needs it's x, y, width and height to be defined in some way
* before we can render it, therefore, use macro flags to indicate which of these have been set
* and ASSERT whether the required information has been passed. Once a Constraint method is
* completed it will set the relevant flags e.g UI_X_SET etc
*/

// NOTE(Cian): UI runs twice, once to calculate layout based on constraints. This step
// builds a tree of the UI along with a HashMap for lookup. On the second run through,
// each UI call simply looks up it's layout from the hash map, now we can
// process Input. If the result of Input results in the creation of a UI element we 
// may have to do some very basic single pass layout stuff. Initially, to keep things simple
// I'm just going to do the HashMap part.

enum UI_LayoutFlags {
    UI_WIDTH_SET = 1 << 0,
    UI_HEIGHT_SET = 1 << 1,
    UI_X0_SET = 1 << 2,
    UI_Y0_SET = 1 << 3,
    UI_X1_SET = 1 << 4,
    UI_Y1_SET = 1 << 5,
    UI_W_H_X0_Y0 = UI_WIDTH_SET | UI_HEIGHT_SET | UI_X0_SET | UI_Y0_SET,
    UI_W_H_X1_Y1 = UI_WIDTH_SET | UI_HEIGHT_SET | UI_X1_SET | UI_Y1_SET,
    UI_W_H_X1_Y0 = UI_WIDTH_SET | UI_HEIGHT_SET | UI_X1_SET | UI_Y0_SET,
    UI_W_H_X0_Y1 = UI_WIDTH_SET | UI_HEIGHT_SET | UI_X0_SET | UI_Y1_SET,
    UI_H_X0_X1_Y0 = UI_HEIGHT_SET | UI_X0_SET | UI_X1_SET | UI_Y0_SET,
    UI_H_X0_X1_Y1 = UI_HEIGHT_SET | UI_X0_SET | UI_X1_SET | UI_Y1_SET,
    UI_W_X0_Y0_Y1 = UI_WIDTH_SET  | UI_X0_SET | UI_Y0_SET | UI_Y1_SET,
    UI_W_X1_Y0_Y1 = UI_WIDTH_SET  | UI_X1_SET | UI_Y0_SET | UI_Y1_SET,
    UI_X0_X1_Y0_Y1 = UI_X0_SET | UI_X1_SET | UI_Y0_SET | UI_Y1_SET,
    UI_LAYOUT_END = 14
};

enum UI_ConstraintFlags {
    UI_MIN_HEIGHT = 1 << 0,
    UI_MAX_HEIGHT = 1 << 1,
    UI_MIN_WIDTH = 1 << 2,
    UI_MAX_WIDTH = 1 << 3,
    UI_NUM_CONSTRAINTS = 6,
};

enum UI_Orientation {
    UI_VERTICAL = 1 << 0,
    UI_HORIZONTAL = 1 << 0,
};

enum UI_ItemTypes {
    UI_LIST,
};

// TODO(Cian): Pull this out into it's DataDesk generated ceomhar_closure.h
struct Closure {
    void (*call)(Closure *);
    void *args[MAX_CLOSURE_ARGS];
};


// TODO(Cian): same as above

struct CodeView {
    // TODO(Cian): Same as above
    Closure closure_register[128];
    u32 front, rear, size;
};

// TODO(Cian): UI state and other stuff needs to be moved into a "core" megastruct
struct UI_Item {
    char *id;
    u32 layout_flags;
    u32 constraints_list[(UI_NUM_CONSTRAINTS * 2) + 1];
    u32 ui_item_type; // TODO(Cian): Do we need this??
    f32 width;
    f32 height;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
    // TODO(Cian): This is redundant, in future we'll probably move to a deferred ui system
    CodeView code_view;
    UI_Item *hash_next;
    UI_Item *stack_next;
};

struct UI_State {
    // NOTE(Cian):  *parent is a stack DS, current is just the current UI_Item in context
    UI_Item *parent;
    UI_Item current;
    // NOTE(Cian): This needs to be ^2 for now
    UI_Item *ui_items_hash[UI_HASH_SIZE];
};

GLOBAL UI_State *ui_state = 0;

#define UI_LAYOUT_COMPLETE UI_WIDTH_SET | UI_HEIGHT_SET | UI_X0_SET | UI_Y0_SET | UI_X1_SET | UI_Y1_SET

INTERNAL UI_State *UI_InitState();

// NOTE(Cian): UI Layout Utilities
INTERNAL void UI_BeginWindow(char *id);
INTERNAL void UI_BeginNavMenu(char *id, u32 orientation, f32 width, f32 height, f32 margin);
INTERNAL void UI_EndWindow(char *id);
INTERNAL void UI_EndLayout();

INTERNAL b32 UI_IsAllFlagsSet(u32 flags);
INTERNAL b32 UI_DoLayout(UI_Item *current);

// NOTE(Cian): UI constraints
INTERNAL void UI_StartToStartConstraint(char *id, f32 offset);
INTERNAL void UI_StartToEndConstraint(char *id, f32 offset);
INTERNAL void UI_EndToEndConstraint(char *id, f32 offset);
INTERNAL void UI_BottomToBottomConstraint(char *id, f32 offset);
INTERNAL void UI_TopToTopConstraint(char *id, f32 offset);

INTERNAL void UI_Width(f32 width);
INTERNAL void UI_FillWidth(f32 offset);
INTERNAL void UI_Height(f32 width);
INTERNAL void UI_FillHeight(f32 offset);
INTERNAL void UI_SetX(f32 x);
INTERNAL void UI_SetY(f32 y);

INTERNAL void UI_MaxHeight(f32 max);
INTERNAL void UI_MinHeight(f32 min);
INTERNAL void UI_MaxWidth(f32 max);
INTERNAL void UI_MinWidth(f32 min);
INTERNAL void UI_CenterX(f32 offset);
INTERNAL void UI_CenterY(f32 offset);


INTERNAL char *UI_Parent();

// NOTE(Cian): UI Elements
INTERNAL void UI_Panel(char *id, NVGcolor color);
INTERNAL void UI_PanelClosure(char *id, NVGcolor);
INTERNAL void UI_Text(char *id, char* text, f32 font_size,  NVGcolor color);
INTERNAL void UI_Text_Closure(Closure *closure);

// NOTE(Cian): UI Utilities
INTERNAL f32 PixelsToDIP(f32 pixels);
INTERNAL f32 DIPToPixels(f32 dp);

// NOTE(Cian): UI Item Hash Lookup
INTERNAL UI_Item *GetUIItem(char *key);
INTERNAL UI_Item *AddUIItem(char *key, UI_Item item);
INTERNAL void RemoveUIItem(char *key);

// NOTE(Cian): UI Parent Stack Methods - Used for keeping track of current parent
INTERNAL void PushUIParent(UI_Item *parent);
INTERNAL UI_Item *PopUIParent();
INTERNAL UI_Item *ViewUIParent();
INTERNAL UI_Item PeekUIParent();

// NOTE(Cian): UI closure queue methods to defer things
// TODO(Cian): Move this to it's own closure file once DataDesk is setup
INTERNAL void QueueClosure(Closure closure);
INTERNAL Closure *TakeClosure();