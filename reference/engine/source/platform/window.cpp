#define GLFW_INCLUDE_NONE
#include <platform/window.hpp>
#include <platform/events.hpp>
#include <platform/input.hpp>
#include <platform/filesystem.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <print>

struct WindowState
{
    GLFWwindow *window = NULL;
    int32_t width;
    int32_t height;
    int32_t framebuffer_width;
    int32_t framebuffer_height;
    int32_t close_requests;
    bool is_visible;
    bool is_focused;
    bool is_minimized;
    bool is_maximized;
    bool is_resizable;
    bool is_cursor_captured;
};

static WindowState window_state = { };

static inline void window_size_callback(GLFWwindow *window, int width, int height);
static inline void window_maximize_callback(GLFWwindow *window, int maximized);
static inline void window_minimize_callback(GLFWwindow *window, int minimized);
static inline void window_focus_callback(GLFWwindow *window, int focused);
static inline void window_close_callback(GLFWwindow *window);
static inline void window_framebuffer_resize_callback(GLFWwindow *window, int width, int height);
static inline void window_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static inline void window_char_callback(GLFWwindow *window, uint32_t code_point);
static inline void window_cursor_move_callback(GLFWwindow *window, double x, double y);
static inline void window_cursor_scroll_callback(GLFWwindow *window, double x, double y);
static inline void window_cursor_enter_callback(GLFWwindow *window, int entered);
static inline void window_cursor_button_callback(GLFWwindow *window, int button, int action, int mods);
static inline uint32_t convert_input(int code);

bool 
pde_window_create_window(int32_t width, int32_t height, const char *name, WindowFlags flags)
{
  
    if (window_state.window != NULL) return true; // Window was already created, just exit.

    glfwInit();

    if (flags & WindowFlags_DisableOpenGL)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
    else
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *window = window_state.window = glfwCreateWindow(width, height, name, NULL, NULL); 
    if (window == NULL) return false;

    std::string icon_location = pde_filesystem_resolve("./resources/app_icon.png");
    if (icon_location == "")
    {
        glfwDestroyWindow(window);
        std::println("Failed to find app icon.");
        return false;
    }

    int32_t icon_width, icon_height, icon_channels;
    uint8_t *data = stbi_load(icon_location.c_str(), &icon_width, &icon_height, &icon_channels, 4);
    if (data == NULL)
    {
        glfwDestroyWindow(window);
        std::println("Failed to load app icon.");
        return false;
    }

    GLFWimage image;
    image.width = icon_width;
    image.height = icon_height;
    image.pixels = data;

    glfwSetWindowIcon(window, 1, &image);
    
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, window_framebuffer_resize_callback);
    glfwSetWindowIconifyCallback(window, window_minimize_callback);
    glfwSetWindowMaximizeCallback(window, window_maximize_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);
    glfwSetKeyCallback(window, window_key_callback);
    glfwSetCharCallback(window, window_char_callback);
    glfwSetCursorPosCallback(window, window_cursor_move_callback);
    glfwSetCursorEnterCallback(window, window_cursor_enter_callback);
    glfwSetScrollCallback(window, window_cursor_scroll_callback);
    glfwSetMouseButtonCallback(window, window_cursor_button_callback);

    if (flags & WindowFlags_DisableResize) glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
    if (flags & WindowFlags_ShowWindow) glfwShowWindow(window);
    if (flags & WindowFlags_CaptureCursor) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwGetFramebufferSize(window, &window_state.framebuffer_width, &window_state.framebuffer_height);
    glfwGetWindowSize(window, &window_state.width, &window_state.height);
    window_state.is_visible = glfwGetWindowAttrib(window, GLFW_VISIBLE);
    window_state.is_focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
    window_state.is_resizable = glfwGetWindowAttrib(window, GLFW_RESIZABLE);
    window_state.is_maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
    window_state.is_cursor_captured = (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED);
    window_state.is_minimized = false;
    window_state.close_requests = 0;

    return true;

}

void 
pde_window_destroy_window()
{

    if (window_state.window == NULL) return; // No window to destroy.
    glfwDestroyWindow(window_state.window);
    window_state.window = NULL;
    
}

void 
pde_window_poll_events()
{
    
    glfwPollEvents();
    
}

void 
pde_window_cursor_capture(bool enabled)
{

    if (window_state.window == NULL) return;

    uint32_t mode = (enabled) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
    glfwSetInputMode(window_state.window, GLFW_CURSOR, mode);

    EngineEvent event = {};
    event.type = EngineEventType_MouseCapture;
    event.mouse_capture.captured = enabled;

    pde_events_push_engine_event(event);

}

void 
show(bool enabled)
{

    if (window_state.window == NULL) return;

    glfwSetWindowAttrib(window_state.window, GLFW_VISIBLE, (enabled) ? GLFW_TRUE : GLFW_FALSE);

}

int32_t 
pde_window_get_width()
{

    if (window_state.window == NULL) return 0;
    return window_state.width;

}

int32_t 
pde_window_get_height()
{

    if (window_state.window == NULL) return 0;
    return window_state.height;

}

int32_t 
pde_window_get_framebuffer_width()
{

    if (window_state.window == NULL) return 0;
    return window_state.framebuffer_width;

}

int32_t 
pde_window_get_framebuffer_height()
{

    if (window_state.window == NULL) return 0;
    return window_state.framebuffer_height;

}

int32_t 
pde_window_get_close_requests()
{

    if (window_state.window == NULL) return 0;
    return window_state.close_requests;

}

bool 
pde_window_is_minimized()
{

    if (window_state.window == NULL) return false;
    return window_state.is_minimized;

}

bool 
pde_window_is_maximized()
{

    if (window_state.window == NULL) return false;
    return window_state.is_maximized;

}

bool 
pde_window_is_focused()
{

    if (window_state.window == NULL) return false;
    return window_state.is_focused;

}

bool 
pde_window_is_visible()
{

    if (window_state.window == NULL) return false;
    return window_state.is_visible;

}

bool 
pde_window_should_close()
{

    if (window_state.window == NULL) return true;
    return (window_state.close_requests > 0);

}

void* 
pde_window_get_window_handle()
{

    return window_state.window;

}

// --- Window Callbacks ----------------------------------------------------------------------------
//
// The following callbacks are primarily used to forward events to the front-end API.
//

static inline void 
window_size_callback(GLFWwindow *window, int width, int height)
{

    window_state.width = width;
    window_state.height = height;

    EngineEvent event;
    event.type = EngineEventType_WindowResize;
    event.window_resize.width = width;
    event.window_resize.height = height;

    pde_events_push_engine_event(event);

}

static inline void 
window_maximize_callback(GLFWwindow *window, int maximized)
{

    window_state.is_maximized = maximized;

    EngineEvent event;
    event.type = EngineEventType_WindowMaximize;

    pde_events_push_engine_event(event);

}

static inline void 
window_minimize_callback(GLFWwindow *window, int minimized)
{

    window_state.is_minimized = minimized;
    if (minimized == false) window_state.is_visible = false;

    EngineEvent event;
    event.type = (minimized) ? EngineEventType_WindowMinimize : EngineEventType_WindowRestore;

    pde_events_push_engine_event(event);

}

static inline void 
window_focus_callback(GLFWwindow *window, int focused)
{

    window_state.is_focused = focused;

    EngineEvent event;
    event.type = EngineEventType_WindowFocus;
    event.window_focus.focused = focused;

    pde_events_push_engine_event(event);

}

static inline void 
window_close_callback(GLFWwindow *window)
{

    window_state.close_requests++;

    EngineEvent event;
    event.type = EngineEventType_WindowClose;

    pde_events_push_engine_event(event);

}

static inline void 
window_framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{

    window_state.framebuffer_width = width;
    window_state.framebuffer_height = height;

    EngineEvent event;
    event.type = EngineEventType_WindowFramebufferResize;
    event.window_framebuffer_resize.width = width;
    event.window_framebuffer_resize.height = height;

    pde_events_push_engine_event(event);

}

static inline void 
window_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

    uint32_t converted_code = convert_input(key);

    EngineEvent event;
    event.type = EngineEventType_KeyInput;
    event.key_input.code = converted_code;
    event.key_input.down = action;

    pde_events_push_engine_event(event);


}

static inline void 
window_char_callback(GLFWwindow *window, uint32_t code_point)
{

    EngineEvent event;
    event.type = EngineEventType_KeyChar;
    event.key_char.codepoint = code_point;

    pde_events_push_engine_event(event);

}

static inline void 
window_cursor_move_callback(GLFWwindow *window, double x, double y)
{

    EngineEvent event;
    event.type = EngineEventType_MouseMove;
    event.mouse_move.x = x;
    event.mouse_move.y = y;

    pde_events_push_engine_event(event);

}

static inline void 
window_cursor_scroll_callback(GLFWwindow *window, double x, double y)
{

    EngineEvent event;
    event.type = EngineEventType_MouseScroll;
    event.mouse_scroll.x = x;
    event.mouse_scroll.y = y;

    pde_events_push_engine_event(event);

}

static inline void 
window_cursor_enter_callback(GLFWwindow *window, int entered)
{

    EngineEvent event;
    event.type = EngineEventType_MouseView;
    event.mouse_view.inside = entered;

    pde_events_push_engine_event(event);

}

static inline void 
window_cursor_button_callback(GLFWwindow *window, int button, int action, int mods)
{

    uint32_t converted_code = convert_input(button);

    EngineEvent event;
    event.type = EngineEventType_MouseButton;
    event.mouse_button.code = converted_code;
    event.mouse_button.down = action;

    pde_events_push_engine_event(event);

}

static inline uint32_t 
convert_input(int32_t code)
{

    switch (code)
    {

        // Mouse
        case GLFW_MOUSE_BUTTON_LEFT:   return InputType_MouseLeft;
        case GLFW_MOUSE_BUTTON_RIGHT:  return InputType_MouseRight;
        case GLFW_MOUSE_BUTTON_MIDDLE: return InputType_MouseMiddle;
        case GLFW_MOUSE_BUTTON_4:      return InputType_MouseButton4;
        case GLFW_MOUSE_BUTTON_5:      return InputType_MouseButton5;
        case GLFW_MOUSE_BUTTON_6:      return InputType_MouseButton6;
        case GLFW_MOUSE_BUTTON_7:      return InputType_MouseButton7;
        case GLFW_MOUSE_BUTTON_8:      return InputType_MouseButton8;

        // Letters
        case GLFW_KEY_A: return InputType_KeyA;
        case GLFW_KEY_B: return InputType_KeyB;
        case GLFW_KEY_C: return InputType_KeyC;
        case GLFW_KEY_D: return InputType_KeyD;
        case GLFW_KEY_E: return InputType_KeyE;
        case GLFW_KEY_F: return InputType_KeyF;
        case GLFW_KEY_G: return InputType_KeyG;
        case GLFW_KEY_H: return InputType_KeyH;
        case GLFW_KEY_I: return InputType_KeyI;
        case GLFW_KEY_J: return InputType_KeyJ;
        case GLFW_KEY_K: return InputType_KeyK;
        case GLFW_KEY_L: return InputType_KeyL;
        case GLFW_KEY_M: return InputType_KeyM;
        case GLFW_KEY_N: return InputType_KeyN;
        case GLFW_KEY_O: return InputType_KeyO;
        case GLFW_KEY_P: return InputType_KeyP;
        case GLFW_KEY_Q: return InputType_KeyQ;
        case GLFW_KEY_R: return InputType_KeyR;
        case GLFW_KEY_S: return InputType_KeyS;
        case GLFW_KEY_T: return InputType_KeyT;
        case GLFW_KEY_U: return InputType_KeyU;
        case GLFW_KEY_V: return InputType_KeyV;
        case GLFW_KEY_W: return InputType_KeyW;
        case GLFW_KEY_X: return InputType_KeyX;
        case GLFW_KEY_Y: return InputType_KeyY;
        case GLFW_KEY_Z: return InputType_KeyZ;

        // Numbers
        case GLFW_KEY_0: return InputType_Key0;
        case GLFW_KEY_1: return InputType_Key1;
        case GLFW_KEY_2: return InputType_Key2;
        case GLFW_KEY_3: return InputType_Key3;
        case GLFW_KEY_4: return InputType_Key4;
        case GLFW_KEY_5: return InputType_Key5;
        case GLFW_KEY_6: return InputType_Key6;
        case GLFW_KEY_7: return InputType_Key7;
        case GLFW_KEY_8: return InputType_Key8;
        case GLFW_KEY_9: return InputType_Key9;

        // Function keys
        case GLFW_KEY_F1:  return InputType_F1;
        case GLFW_KEY_F2:  return InputType_F2;
        case GLFW_KEY_F3:  return InputType_F3;
        case GLFW_KEY_F4:  return InputType_F4;
        case GLFW_KEY_F5:  return InputType_F5;
        case GLFW_KEY_F6:  return InputType_F6;
        case GLFW_KEY_F7:  return InputType_F7;
        case GLFW_KEY_F8:  return InputType_F8;
        case GLFW_KEY_F9:  return InputType_F9;
        case GLFW_KEY_F10: return InputType_F10;
        case GLFW_KEY_F11: return InputType_F11;
        case GLFW_KEY_F12: return InputType_F12;

        // Control keys
        case GLFW_KEY_ESCAPE:      return InputType_Escape;
        case GLFW_KEY_TAB:         return InputType_Tab;
        case GLFW_KEY_LEFT_SHIFT:  return InputType_ShiftLeft;
        case GLFW_KEY_RIGHT_SHIFT: return InputType_ShiftRight;
        case GLFW_KEY_LEFT_CONTROL:return InputType_ControlLeft;
        case GLFW_KEY_RIGHT_CONTROL:return InputType_ControlRight;
        case GLFW_KEY_LEFT_ALT:    return InputType_AltLeft;
        case GLFW_KEY_RIGHT_ALT:   return InputType_AltRight;
        case GLFW_KEY_SPACE:       return InputType_Space;
        case GLFW_KEY_ENTER:       return InputType_Enter;
        case GLFW_KEY_BACKSPACE:   return InputType_Backspace;
        case GLFW_KEY_DELETE:      return InputType_Delete;
        case GLFW_KEY_INSERT:      return InputType_Insert;
        case GLFW_KEY_HOME:        return InputType_Home;
        case GLFW_KEY_END:         return InputType_End;
        case GLFW_KEY_PAGE_UP:     return InputType_PageUp;
        case GLFW_KEY_PAGE_DOWN:   return InputType_PageDown;

        // Arrows
        case GLFW_KEY_UP:    return InputType_ArrowUp;
        case GLFW_KEY_DOWN:  return InputType_ArrowDown;
        case GLFW_KEY_LEFT:  return InputType_ArrowLeft;
        case GLFW_KEY_RIGHT: return InputType_ArrowRight;

        // Numpad
        case GLFW_KEY_KP_0:        return InputType_Numpad0;
        case GLFW_KEY_KP_1:        return InputType_Numpad1;
        case GLFW_KEY_KP_2:        return InputType_Numpad2;
        case GLFW_KEY_KP_3:        return InputType_Numpad3;
        case GLFW_KEY_KP_4:        return InputType_Numpad4;
        case GLFW_KEY_KP_5:        return InputType_Numpad5;
        case GLFW_KEY_KP_6:        return InputType_Numpad6;
        case GLFW_KEY_KP_7:        return InputType_Numpad7;
        case GLFW_KEY_KP_8:        return InputType_Numpad8;
        case GLFW_KEY_KP_9:        return InputType_Numpad9;
        case GLFW_KEY_KP_ADD:      return InputType_NumpadAdd;
        case GLFW_KEY_KP_SUBTRACT: return InputType_NumpadSubtract;
        case GLFW_KEY_KP_MULTIPLY: return InputType_NumpadMultiply;
        case GLFW_KEY_KP_DIVIDE:   return InputType_NumpadDivide;
        case GLFW_KEY_KP_DECIMAL:  return InputType_NumpadDecimal;
        case GLFW_KEY_KP_ENTER:    return InputType_NumpadEnter;
    }

    return InputType_Unknown;

}
