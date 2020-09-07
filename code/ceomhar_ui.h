#define UI_DEFAULT_DENSITY 96.00f
#define UI_PARENT (ui_state->parent.id)

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
    UI_LAYOUT_END = 6
};

struct UI_Item {
    char id[250];
    u32 layout_flags;
    f32 width;
    f32 height;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
};

// TODO(Cian): maybe storing the entire UI in a tree structure would be better?
struct UI_State {
    UI_Item current;
    UI_Item parent;
    UI_Item siblings[50];
    // Closure stuff for automated layouts
};

GLOBAL UI_State *ui_state = 0;

// NOTE(Cian): UI Layout Utilities
INTERNAL void UI_BeginWindow();
INTERNAL void UI_EndWindow();

INTERNAL b32 UI_IsAllFlagsSet(u32 flags);

INTERNAL void UI_StartToStartConstraint(char *id, f32 offset);
INTERNAL void UI_BottomToBottomConstraint(char *id, f32 offset);

INTERNAL void UI_Width(f32 width);
INTERNAL void UI_Height(f32 width);

// NOTE(Cian): UI Elements
INTERNAL void UI_Panel();

// NOTE(Cian): UI Utilities
INTERNAL f32 PixelsToDIP(f32 pixels);
INTERNAL f32 DIPToPixels(f32 dp);
