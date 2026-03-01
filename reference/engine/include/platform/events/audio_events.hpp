#pragma once
#include <cstdint>
#include <platform/events/event_queue.hpp>

enum AudioEventType : uint32_t
{

    AudioEventType_Exit,
    AudioEventType_TrackPrepare,
    AudioEventType_TrackQuery,
    AudioEventType_TrackAdjust,
    AudioEventType_TrackPlay,
    AudioEventType_TrackPause,
    AudioEventType_TrackStop,

};

struct AudioEvent
{

    AudioEventType type;

};
