#include <platform/controller.hpp>
#include <cassert>

#if defined(_WIN32)
#include <windows.h>
#include <Xinput.h>

void 
pde_controller_get_info(ControllerIndex index, ControllerInfo *info)
{

    constexpr uint32_t max_index = XUSER_MAX_COUNT;
    if (index > max_index) return;
    assert(info != NULL);

    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    DWORD dwResult = XInputGetState(index, &state);

    if (dwResult == ERROR_SUCCESS)
    {

        info->connected = true;
        info->direction_pad_up = state.Gamepad.wButtons & 0x0001;
        info->direction_pad_down = state.Gamepad.wButtons & 0x0002;
        info->direction_pad_left = state.Gamepad.wButtons & 0x0004;
        info->direction_pad_right = state.Gamepad.wButtons & 0x0008;
        info->start = state.Gamepad.wButtons & 0x0010;
        info->back = state.Gamepad.wButtons & 0x0020;
        info->left_thumb = state.Gamepad.wButtons & 0x0040;
        info->right_thumb = state.Gamepad.wButtons & 0x0080;
        info->left_shoulder = state.Gamepad.wButtons & 0x0100;
        info->right_shoulder = state.Gamepad.wButtons & 0x0200;
        info->button_a = state.Gamepad.wButtons & 0x1000;
        info->button_b = state.Gamepad.wButtons & 0x2000;
        info->button_x = state.Gamepad.wButtons & 0x4000;
        info->button_y = state.Gamepad.wButtons & 0x8000;

        info->left_trigger = (float)state.Gamepad.bLeftTrigger / 255.0f;
        info->right_trigger = (float)state.Gamepad.bRightTrigger / 255.0f;

        float left_x = ((float)(state.Gamepad.sThumbLX + 32768) / 65535.0f) * 2.0f - 1.0f;
        float left_y = ((float)(state.Gamepad.sThumbLY + 32768) / 65535.0f) * 2.0f - 1.0f;
        float right_x = ((float)(state.Gamepad.sThumbRX + 32768) / 65535.0f) * 2.0f - 1.0f;
        float right_y = ((float)(state.Gamepad.sThumbRX + 32768) / 65535.0f) * 2.0f - 1.0f;
        info->left_thumb_stick_x = left_x;
        info->right_thumb_stick_x = right_x;
        info->left_thumb_stick_y = left_y;
        info->right_thumb_stick_y = right_y;
        info->left_thumb_deadzone = (float)XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / 65535.0f;
        info->right_thumb_deadzone = (float)XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / 65535.0f;
        info->trigger_deadzone = (float)XINPUT_GAMEPAD_TRIGGER_THRESHOLD / 255.0f;

    }
    else
    {
        info->connected = false;
        return;
    }

}

bool 
pde_controller_is_active(ControllerIndex index)
{

    constexpr uint32_t max_index = XUSER_MAX_COUNT;
    if (index > max_index) return false;

    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    DWORD dwResult = XInputGetState(index, &state);
    const bool result = dwResult == ERROR_SUCCESS; // What the fuck, Microsoft?
    return result;

}

#else
#   pragma error "Unimplemented platform implementation."
#endif

