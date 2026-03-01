#pragma once
#include <platform/events.hpp>
#include <platform/input.hpp>

class LayerInterface
{

    public:
        inline virtual bool mount() = 0;
        inline virtual bool unmount() = 0;

        inline virtual void update(float delta_time) = 0;
        inline virtual void fixed_update(float delta_time) = 0;
        inline virtual void render() = 0;

        inline void cursor_lock();
        inline void cursor_unlock();
        inline void cursor_toggle_lock();

};

void LayerInterface::
cursor_lock()
{
    PlatformEvent event;
    event.type = PlatformEventType_CursorCapture;
    event.cursor_capture.capture = true;
    pde_events_push_platform_event(event);
}

void LayerInterface::
cursor_unlock()
{
    PlatformEvent event;
    event.type = PlatformEventType_CursorCapture;
    event.cursor_capture.capture = false;
    pde_events_push_platform_event(event);
}

void LayerInterface::
cursor_toggle_lock()
{
    if (pde_input_is_mouse_captured()) cursor_unlock();
    else cursor_lock();
}
