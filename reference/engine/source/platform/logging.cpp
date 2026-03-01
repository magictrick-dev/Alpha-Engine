#include <platform/logging.hpp>
#include <deque>
#include <mutex>

static uint32_t log_limit = 256;
static std::deque<MessageLog> logs;
static std::mutex log_mutex;

void pde_logging_add_message(MessageLog log)
{
    std::scoped_lock lock(log_mutex);
    logs.push_back(log);
    while (logs.size() > log_limit) logs.pop_front();
}

void 
pde_logging_add_message(MessageOriginType origin, MessageSeverityType severity, std::string message)
{

    MessageLog log = {};
    log.origin = origin;
    log.severity = severity;
    log.message = message;

    pde_logging_add_message(log);

}

std::vector<MessageLog> 
pde_logging_get_logs()
{

    std::scoped_lock lock(log_mutex);
    std::vector<MessageLog> result(logs.begin(), logs.end());
    return std::move(result);

}

