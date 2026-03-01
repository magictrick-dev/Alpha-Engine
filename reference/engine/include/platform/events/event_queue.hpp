#pragma once
#include <cstdint>
#include <queue>
#include <mutex>
#include <semaphore>

template <typename T, bool stall = false>
struct EventQueue
{

    std::queue<T> event_queue_a, event_queue_b;
    std::queue<T> *read_queue = &event_queue_a;
    std::queue<T> *write_queue = &event_queue_b;
    std::mutex event_mutex;

    std::binary_semaphore event_semaphore { 0 };

    inline void swap_queues()
    {
        std::scoped_lock lock(event_mutex);
        std::swap(read_queue, write_queue);
    }

    inline void push_event(T event)
    {
        std::scoped_lock lock(event_mutex);
        write_queue->push(event);

        if constexpr (stall)
        {
            event_semaphore.release();
        }

    }

    inline size_t pop_event(T *event)
    {
        if (read_queue->empty()) return 0;
        size_t current = read_queue->size();
        *event = read_queue->front();
        read_queue->pop();
        return current;
    }

    inline void wait_for_events()
    {
        if constexpr (stall)
        {
            event_semaphore.acquire();
        }
    }
    
};

