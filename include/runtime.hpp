#pragma once
#include <cstdint>
#include <platform/window.hpp>

enum RuntimeResult : uint32_t
{
    RuntimeResult_Success,
    RuntimeResult_Failure,
};

struct RuntimeState
{
    bool running = true;
    Window *window = NULL;
};

RuntimeResult runtime_init(RuntimeState *runtime_state);
RuntimeResult runtime_main(RuntimeState *runtime_state);
RuntimeResult runtime_exit(RuntimeState *runtime_state);

