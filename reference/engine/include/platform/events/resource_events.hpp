#pragma once
#include <cstdint>
#include <platform/events/event_queue.hpp>

enum ResourceEventType : uint32_t
{

    ResourceEventType_Exit,
    ResourceEventType_MonitorAdd,
    ResourceEventType_MonitorRemove,
    ResourceEventType_MonitorQuery,

};

struct ResourceEvent
{

    ResourceEventType type;

    union
    {
        struct { char path[256]; } monitor_add;
        struct { char path[256]; } monitor_remove;
        struct { char path[256]; } monitor_query;
    };

};

