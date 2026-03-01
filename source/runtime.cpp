#include <print>
#include <runtime.hpp>
#include <platform/window.hpp>
#include <platform/rtdispatcher.hpp>

RuntimeResult 
runtime_init(RuntimeState *runtime_state)
{

    runtime_state->window = new Window(1280, 720, "Alpha Engine");
    runtime_state->window->show();

    return RuntimeResult_Success;
}

RuntimeResult 
runtime_main(RuntimeState *runtime_state)
{

    while (runtime_state->running == true)
    {

        // Poll platform events.
        runtime_state->window->poll_events();
        if (runtime_state->window->should_close())
        {
            runtime_state->running = false;
            break;
        }

        RTDispatcher::swap_queues();
        while (!RTDispatcher::is_empty())
        {

            RTEvent *current_event = RTDispatcher::get_current_event();
            RTEventType event_type = current_event->type;
            switch (event_type)
            {

                case RTEventType_WindowResize:
                {
                    int32_t width = current_event->window_resize.width;
                    int32_t height = current_event->window_resize.height;
                    std::println("[Debug][Platform] Window Resize: {}, {}", width, height);
                } break;

                case RTEventType_WindowFramebufferResize:
                {
                    int32_t width = current_event->window_framebuffer_resize.width;
                    int32_t height = current_event->window_framebuffer_resize.height;
                    std::println("[Debug][Platform] Window Framebuffer Resize: {}, {}", width, height);
                } break;

                default:
                {
                    std::println("[Error][Platform] Uncaught event {}", uint32_t(event_type));
                } break;

            }
            RTDispatcher::pop_event();

        }

        runtime_state->window->swap_buffers();

    }

    return RuntimeResult_Success;
}

RuntimeResult 
runtime_exit(RuntimeState *runtime_state)
{

    runtime_state->window->close();

    return RuntimeResult_Success;
}



