#include <platform/input.hpp>

static InputFrame frame_a, frame_b;
static InputFrame *current = &frame_a;
static InputFrame *previous = &frame_b;

void 
pde_input_swap_input_buffers()
{

    std::swap(current, previous);
    *current = *previous;

    for (uint32_t i = 0; i < InputType_Unknown; ++i)
    {
        current->inputs[i].was_pressed = false;
        current->inputs[i].was_released = false;
    }

    current->mouse_position.dx = 0.0f;
    current->mouse_position.dy = 0.0f;
    current->mouse_wheel.x = 0.0f;
    current->mouse_wheel.y = 0.0f;
    current->mouse_wheel.dx = 0.0f;
    current->mouse_wheel.dy = 0.0f;

    for (uint32_t c = 0; c < 4; ++c)
    {

        if (!current->controllers[c].active) continue;
        for (uint32_t i = 0; i < ControllerInputType_Unknown; ++i)
        {
            current->controllers[c].buttons[i].was_pressed = false;
            current->controllers[c].buttons[i].was_released = false;
            current->controllers[c].was_connected = false;
            current->controllers[c].was_disconnected = false;
        }
        current->controllers[c].left_stick.dx = 0.0f;
        current->controllers[c].right_stick.dx = 0.0f;
        current->controllers[c].left_stick.dy = 0.0f;
        current->controllers[c].right_stick.dy = 0.0f;

    }

}

void 
pde_input_set_input(InputType type, bool down)
{

    if (down)
    {
        if (!previous->inputs[type].down) current->inputs[type].was_pressed = true;
        current->inputs[type].down = true;
    }
    else
    {
        if (previous->inputs[type].down) current->inputs[type].was_released = true;
        current->inputs[type].down = false;
    }

}

void 
pde_input_set_mouse_position(double x, double y)
{

    double dx = x - current->mouse_position.x;
    double dy = y - current->mouse_position.y;
    current->mouse_position.x = x;
    current->mouse_position.y = y;
    current->mouse_position.dx += dx;
    current->mouse_position.dy += dy;

}

void 
pde_input_set_mouse_wheel(double x, double y)
{

    current->mouse_wheel.dx += x;
    current->mouse_wheel.dy += y;

}

void 
pde_input_set_mouse_capture(bool captured)
{

    current->mouse_captured = captured;

}

void 
pde_input_set_mouse_visibility(bool visible)
{

    current->mouse_visible = visible;

}

bool 
pde_input_is_input_down(InputType type)
{
    return current->inputs[type].down;
}

bool 
pde_input_is_input_up(InputType type)
{
    return !current->inputs[type].down;
}

bool 
pde_input_is_input_pressed(InputType type)
{
    return current->inputs[type].was_pressed;
}

bool 
pde_input_is_input_released(InputType type)
{
    return current->inputs[type].was_released;
}

double 
pde_input_get_mouse_cursor_x()
{
    return current->mouse_position.x;
}

double 
pde_input_get_mouse_cursor_y()
{
    return current->mouse_position.y;
}

double 
pde_input_get_mouse_cursor_dx()
{
    return current->mouse_position.dx;
}

double 
pde_input_get_mouse_cursor_dy()
{
    return current->mouse_position.dy;
}

double 
pde_input_get_mouse_wheel_dx()
{
    return current->mouse_wheel.dx;
}

double 
pde_input_get_mouse_wheel_dy()
{
    return current->mouse_wheel.dy;
}

bool 
pde_input_is_mouse_visible()
{
    return current->mouse_visible;
}

bool 
pde_input_is_mouse_captured()
{
    return current->mouse_captured;
}

void 
pde_input_set_controller_connection(uint32_t index, bool connected)
{

    if (connected)
    {
        current->controllers[index].active = true;
        if (!previous->controllers[index].active) current->controllers[index].was_connected = true;
    }
    else
    {
        current->controllers[index].active = false;
        if (previous->controllers[index].active) current->controllers[index].was_connected = false;
    }

}

bool 
pde_input_controller_is_connected(uint32_t index)
{

    return current->controllers[index].active;

}

