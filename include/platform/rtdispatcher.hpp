#pragma once
#include <utilities/definitions.hpp>

enum RTEventType : uint32_t
{
    RTEventType_WindowResize,
    RTEventType_WindowFramebufferResize,
    RTEventType_WindowClose,
    RTEventType_WindowDestroy,
    RTEventType_WindowFocus,
    RTEventType_WindowMinimize,
    RTEventType_WindowMaximize,
    RTEventType_WindowRestore,
};

struct RTEventWindowResize { int32_t width; int32_t height; };
struct RTEventWindowFramebufferResize { int32_t width; int32_t height; };
struct RTEventWindowClose { int32_t dummy; };
struct RTEventWindowDestroy { int32_t dummy; };
struct RTEventWindowFocus { int32_t focused; };
struct RTEventWindowMinimize { int32_t dummy; };
struct RTEventWindowMaximize { int32_t dummy; };
struct RTEventWindowRestore { int32_t dummy; };
struct RTEventKeyPress { uint32_t engine_code; int32_t is_down; };
struct RTEventKeyChar { uint32_t code_point; };


struct RTEvent
{

    RTEventType type;

    union
    {
        RTEventWindowResize window_resize;
        RTEventWindowFramebufferResize window_framebuffer_resize;
        RTEventWindowClose window_close;
        RTEventWindowDestroy window_destroy;
        RTEventWindowFocus window_focus;
        RTEventWindowMinimize window_minimize;
        RTEventWindowMaximize window_maximize;
        RTEventWindowRestore window_restore;
    };

};

// NOTE(Chris): Single reader, multiple writer event queue. The runtime thread 
//              is responsible for catching any events related to the platform,
//              such as window changes, input, and internal runtime threads.
//              Processing occurs at the start of the runtime loop, ensuring all
//              events update the state of the runtime environment before any
//              game-engine specific events occur.

class RTDispatcher
{
    public:
        static void swap_queues();
        static void pop_event();
        static void push_event(RTEvent event);
        static bool is_empty();
        static RTEvent* get_current_event();
};

