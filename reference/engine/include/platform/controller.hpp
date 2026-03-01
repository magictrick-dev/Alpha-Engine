#pragma once
#include <cstdint>

enum ControllerIndex : uint32_t
{
    ControllerIndex_Primary = 0,
    ControllerIndex_Controller1 = 0,
    ControllerIndex_Controller2 = 1,
    ControllerIndex_Controller3 = 2,
    ControllerIndex_Controller4 = 3,
};

struct ControllerInfo
{

    bool connected;

    bool direction_pad_up;
    bool direction_pad_down;
    bool direction_pad_left;
    bool direction_pad_right;
    bool start;
    bool back;
    bool left_thumb;
    bool right_thumb;
    bool left_shoulder;
    bool right_shoulder;
    bool button_a;
    bool button_b;
    bool button_x;
    bool button_y;

    float left_trigger;         // Normalized.
    float right_trigger;        // Normalized.

    float left_thumb_stick_x;   // Normalized.
    float left_thumb_stick_y;   // Normalized.
    float right_thumb_stick_x;  // Normalized.
    float right_thumb_stick_y;  // Normalized.

    float trigger_deadzone;     // Normalized.
    float left_thumb_deadzone;  // Normalized.
    float right_thumb_deadzone; // Normalized.

};

void pde_controller_get_info(ControllerIndex index, ControllerInfo *info);
bool pde_controller_is_active(ControllerIndex index);

