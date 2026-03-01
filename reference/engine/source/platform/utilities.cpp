#include <platform/utilities.hpp>
#include <chrono>

timestamp_t 
pde_utilities_get_timestamp()
{

    using namespace std::chrono;
    return duration_cast<nanoseconds>(
        steady_clock::now().time_since_epoch()
    ).count();

}

double 
pde_utilities_get_time_difference(timestamp_t start, timestamp_t end)
{

    int64_t delta_ns = static_cast<int64_t>(end) - static_cast<int64_t>(start);
    return static_cast<float>(delta_ns) * 1e-9f;

}

