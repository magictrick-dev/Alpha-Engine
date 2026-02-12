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

void rtdispatcher_swap_queues();
bool rtdispatcher_queue_is_empty();
RTEvent* rtdispatcher_get_current_event();

