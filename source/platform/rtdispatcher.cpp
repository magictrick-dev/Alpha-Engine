#include <platform/rtdispatcher.hpp>
#include <mutex>
#include <queue>

static std::mutex event_queue_mtx;
static std::queue<RTEvent> event_queues[2];
static std::queue<RTEvent> *read_queue = &event_queues[0];
static std::queue<RTEvent> *write_queue = &event_queues[1];

void RTDispatcher::
swap_queues()
{

    std::scoped_lock lock(event_queue_mtx);
    std::swap(read_queue, write_queue);
    while (!write_queue->empty()) write_queue->pop();

}

bool RTDispatcher::
is_empty()
{

    return read_queue->empty();

}

RTEvent* RTDispatcher::
get_current_event()
{

    if (read_queue->empty()) return nullptr;
    return &read_queue->front();

}

void RTDispatcher::
pop_event()
{
    if (!read_queue->empty())
        read_queue->pop();
}

void RTDispatcher::
push_event(RTEvent event)
{
    std::scoped_lock lock(event_queue_mtx);
    write_queue->push(event);
}

