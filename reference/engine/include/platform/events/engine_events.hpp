#pragma once
#include <cstdint>
#include <platform/events/event_queue.hpp>

enum EngineEventType : uint32_t
{

    EngineEventType_WindowFramebufferResize,
    EngineEventType_WindowResize,
    EngineEventType_WindowMove,
    EngineEventType_WindowFocus,
    EngineEventType_WindowMinimize,
    EngineEventType_WindowMaximize,
    EngineEventType_WindowRestore,
    EngineEventType_WindowClose,

    EngineEventType_KeyChar,
    EngineEventType_KeyInput,

    EngineEventType_MouseButton,
    EngineEventType_MouseScroll,
    EngineEventType_MouseMove,
    EngineEventType_MouseView,
    EngineEventType_MouseCapture,

    EngineEventType_ResourceMonitorUpdate,

    EngineEventType_ControllerConnection,
    EngineEventType_ControllerButton,
    EngineEventType_ControllerAnalog,

};

enum ResourceMonitorUpdateType : uint32_t
{
    ResourceMonitorUpdateType_Updated,
    ResourceMonitorUpdateType_Deleted,
    ResourceMonitorUpdateType_Monitoring,
    ResourceMonitorUpdateType_NotMonitoring,
};

struct EngineEvent
{

    EngineEventType type;

    union
    {

        struct { int32_t width; int32_t height; } window_resize;
        struct { int32_t width; int32_t height; } window_framebuffer_resize;
        struct { int32_t x; int32_t y; } window_move;
        struct { bool focused; } window_focus;
        struct { uint32_t codepoint; } key_char;
        struct { uint32_t code; int32_t down; } key_input;
        struct { uint32_t code; int32_t down; } mouse_button;
        struct { double x; double y; } mouse_scroll;
        struct { double x; double y; } mouse_move;
        struct { bool inside; } mouse_view;
        struct { bool captured; } mouse_capture;
        struct { uint32_t index; bool connected; } controller_connection;
        struct { uint32_t code; bool down; } controller_button;
        struct { uint32_t code; float x; float y; float default_dead_zone; } controller_analog;

        struct {
            char path[256];
            ResourceMonitorUpdateType update_type;
        } resource_monitor_update;

    };
    
};
