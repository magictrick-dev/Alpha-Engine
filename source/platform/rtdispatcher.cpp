#include <platform/rtdispatcher.hpp>
#include <mutex>
#include <queue>

static std::mutex event_queue_mtx;
static std::queue<RTEvent> event_queues[2];
static std::queue<RTEvent> *read_queue = &event_queues[0];
static std::queue<RTEvent> *write_queue = &event_queues[1];



