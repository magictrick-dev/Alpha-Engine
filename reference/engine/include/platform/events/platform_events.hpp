#pragma once
#include <cstdint>
#include <platform/events/event_queue.hpp>

enum PlatformEventType : uint32_t
{

    PlatformEventType_Exit,
    PlatformEventType_CursorCapture,

};

struct PlatformEvent
{

    PlatformEventType type;

    union
    {

        struct { int32_t code; } exit;
        struct { bool capture; } cursor_capture;

    };

};

