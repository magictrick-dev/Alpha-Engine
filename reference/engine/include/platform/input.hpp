#pragma once
#include <cstdint>
#include <array>

enum ControllerInputType : uint32_t
{
    ControllerInputType_A,
    ControllerInputType_B,
    ControllerInputType_X,
    ControllerInputType_Y,
    ControllerInputType_LT,
    ControllerInputType_RT,
    ControllerInputType_LSB,
    ControllerInputType_RSB,
    ControllerInputType_Start,
    ControllerInputType_Select,
    ControllerInputType_DirLeft,
    ControllerInputType_DirRight,
    ControllerInputType_DirUp,
    ControllerInputType_DirDown,
    ControllerInputType_LeftAnalog,
    ControllerInputType_RightAnalog,
    ControllerInputType_LeftTrigger,
    ControllerInputType_RightTrigger,

    ControllerInputType_Unknown,
};

enum InputType : uint32_t
{
    // Mouse
    InputType_MouseLeft,
    InputType_MouseRight,
    InputType_MouseMiddle,
    InputType_MouseButton4,
    InputType_MouseButton5,
    InputType_MouseButton6,
    InputType_MouseButton7,
    InputType_MouseButton8,

    // Letters
    InputType_KeyA,
    InputType_KeyB,
    InputType_KeyC,
    InputType_KeyD,
    InputType_KeyE,
    InputType_KeyF,
    InputType_KeyG,
    InputType_KeyH,
    InputType_KeyI,
    InputType_KeyJ,
    InputType_KeyK,
    InputType_KeyL,
    InputType_KeyM,
    InputType_KeyN,
    InputType_KeyO,
    InputType_KeyP,
    InputType_KeyQ,
    InputType_KeyR,
    InputType_KeyS,
    InputType_KeyT,
    InputType_KeyU,
    InputType_KeyV,
    InputType_KeyW,
    InputType_KeyX,
    InputType_KeyY,
    InputType_KeyZ,

    // Numbers (top row)
    InputType_Key0,
    InputType_Key1,
    InputType_Key2,
    InputType_Key3,
    InputType_Key4,
    InputType_Key5,
    InputType_Key6,
    InputType_Key7,
    InputType_Key8,
    InputType_Key9,

    // Function keys
    InputType_F1,
    InputType_F2,
    InputType_F3,
    InputType_F4,
    InputType_F5,
    InputType_F6,
    InputType_F7,
    InputType_F8,
    InputType_F9,
    InputType_F10,
    InputType_F11,
    InputType_F12,

    // Control keys
    InputType_Escape,
    InputType_Tab,
    InputType_ShiftLeft,
    InputType_ShiftRight,
    InputType_ControlLeft,
    InputType_ControlRight,
    InputType_AltLeft,
    InputType_AltRight,
    InputType_Space,
    InputType_Enter,
    InputType_Backspace,
    InputType_Delete,
    InputType_Insert,
    InputType_Home,
    InputType_End,
    InputType_PageUp,
    InputType_PageDown,

    // Arrows
    InputType_ArrowUp,
    InputType_ArrowDown,
    InputType_ArrowLeft,
    InputType_ArrowRight,

    // Numpad
    InputType_Numpad0,
    InputType_Numpad1,
    InputType_Numpad2,
    InputType_Numpad3,
    InputType_Numpad4,
    InputType_Numpad5,
    InputType_Numpad6,
    InputType_Numpad7,
    InputType_Numpad8,
    InputType_Numpad9,
    InputType_NumpadAdd,
    InputType_NumpadSubtract,
    InputType_NumpadMultiply,
    InputType_NumpadDivide,
    InputType_NumpadDecimal,
    InputType_NumpadEnter,

    // Unknown / unmapped
    InputType_Unknown,

};



struct BinaryInput
{
    bool was_pressed;
    bool was_released;
    bool down;
};

struct AnalogInput
{
    double x;
    double y;
    double dx;
    double dy;
};

struct Controller
{

    bool active = false;
    bool was_connected = false;
    bool was_disconnected = false;

    std::array<BinaryInput, ControllerInputType_Unknown + 1> buttons;

    AnalogInput left_stick;
    AnalogInput right_stick;
    AnalogInput left_trigger;
    AnalogInput right_trigger;

};

struct InputFrame
{

    std::array<Controller, 4> controllers;
    std::array<BinaryInput, InputType_Unknown + 1> inputs;
    AnalogInput mouse_position;
    AnalogInput mouse_wheel;

    bool mouse_visible = true;
    bool mouse_captured = false;

};

void pde_input_swap_input_buffers();
void pde_input_set_controller_connection(uint32_t index, bool connected);
void pde_input_set_input(InputType type, bool down);
void pde_input_set_mouse_position(double x, double y);
void pde_input_set_mouse_wheel(double x, double y);
void pde_input_set_mouse_capture(bool captured);
void pde_input_set_mouse_visibility(bool visible);
bool pde_input_is_input_down(InputType type);
bool pde_input_is_input_up(InputType type);
bool pde_input_is_input_pressed(InputType type);
bool pde_input_is_input_released(InputType type);
double pde_input_get_mouse_cursor_x();
double pde_input_get_mouse_cursor_y();
double pde_input_get_mouse_cursor_dx();
double pde_input_get_mouse_cursor_dy();
double pde_input_get_mouse_wheel_dx();
double pde_input_get_mouse_wheel_dy();
bool pde_input_is_mouse_visible();
bool pde_input_is_mouse_captured();
bool pde_input_controller_is_connected(uint32_t index);
