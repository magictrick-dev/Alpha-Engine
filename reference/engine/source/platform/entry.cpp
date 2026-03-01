#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <platform/launcher.hpp>
#include <platform/window.hpp>
#include <platform/events.hpp>
#include <platform/controller.hpp>
#include <platform/logging.hpp>
#include <platform/input.hpp>

#if defined(_WIN32)
#include <windows.h>
#include <cstdio>
#include <conio.h>
#include <cstdint>
#include <thread>
#include <print>
#include <chrono>

static inline void
construct_cli_arguments(int *input_argc, const char ***input_argv)
{

    int argc = 0;
    LPWSTR *wide_argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    const char **argv = (const char**)malloc(sizeof(const char*)*argc);
    for (int i = 0; i < argc; ++i)
    {

        int required_size = WideCharToMultiByte(CP_ACP, 0, wide_argv[i], -1, NULL, 0, NULL, NULL);
        char *buffer = (char*)malloc(required_size);
        WideCharToMultiByte(CP_ACP, 0, wide_argv[i], -1, buffer, required_size, NULL, NULL);
        argv[i] = buffer;
        
    }

    *input_argc = argc;
    *input_argv = argv;

}

static inline void
deconstruct_cli_arguments(int argc, const char **argv)
{

    for (int i = 0; i < argc; ++i)
    {

        const char *string = argv[i];
        free((char*)string);

    }

    free(argv);

}

static inline void send_controller_button_update(uint32_t index, uint32_t button, bool down)
{
    EngineEvent e;
    e.type = EngineEventType_ControllerButton;
    e.controller_button.code = button;
    e.controller_button.down = down;
    pde_events_push_engine_event(e);
}

static inline void send_controller_analog_update(uint32_t index, uint32_t code, float x, float y, float dead_zone)
{

    EngineEvent e;
    e.type = EngineEventType_ControllerAnalog;
    e.controller_analog.code = code;
    e.controller_analog.x = x;
    e.controller_analog.y = y;
    e.controller_analog.default_dead_zone = dead_zone;
    pde_events_push_engine_event(e);

}

int WINAPI 
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

#   define PAPERDREAMS_ENABLE_CONSOLE 1
#   if PAPERDREAMS_ENABLE_CONSOLE == 1
        AllocConsole();
        freopen_s((FILE**)stdout,   "CONOUT$",  "w", stdout);
        freopen_s((FILE**)stderr,   "CONOUT$",  "w", stderr);
        freopen_s((FILE**)stdin,    "CONIN$",   "r", stdin);
#   endif

    int argc;
    const char **argv;
    construct_cli_arguments(&argc, &argv);

    if (!pde_window_create_window(1600, 900, "Paperdreams Engine", WindowFlags_ShowWindow))
    {
        std::println("Failed to create window.");
        char c = _getch();
        return -1;
    }

    std::thread main_thread(pde_launcher_thread_main, argc, argv);
    std::thread audio_thread(pde_launcher_thread_audio);
    std::thread resource_thread(pde_launcher_thread_resource);

    int32_t exit_status = 0;
    bool runtime_flag = true;
    while (runtime_flag && !pde_window_should_close())
    {

        pde_window_poll_events();

        PlatformEvent event;
        pde_events_swap_platform_event_queue();
        while (pde_events_pop_platform_event(&event))
        {

            switch (event.type)
            {

                case PlatformEventType_Exit:
                {

                    runtime_flag = false;
                    exit_status = event.exit.code;
                    std::println("Application forced closed with status: {}", exit_status);
                    pde_window_destroy_window();
                    char c = _getch();

                } break;

                case PlatformEventType_CursorCapture:
                {

                    pde_window_cursor_capture(event.cursor_capture.capture);

                } break;

            }

        }

        // NOTE(Chris): We're not actually checking is state changes occur here, so we're
        //              basically blasting the shit out of the event queue with controller updates
        //              as they're spooned in. The only real reason we do this is so that the engine
        //              thread isn't wasting time polling on something that is effectively streaming
        //              out updates.
        static ControllerInfo controller_info[4] = { };
        static bool controller_connections[4] = { false, false, false, false };
        for (uint32_t index = 0; index < 4; ++index)
        {

            pde_controller_get_info((ControllerIndex)index, &controller_info[index]);

            if (controller_info[index].connected && controller_connections[index] == false)
            {
                EngineEvent event;
                event.type = EngineEventType_ControllerConnection;
                event.controller_connection.connected = true;
                event.controller_connection.index = index;
                pde_events_push_engine_event(event);
                controller_connections[index] = true;
            }
            else if (!controller_info[index].connected && controller_connections[index] == true)
            {

                EngineEvent event;
                event.type = EngineEventType_ControllerConnection;
                event.controller_connection.connected = false;
                event.controller_connection.index = index;
                pde_events_push_engine_event(event);
                controller_connections[index] = false;

            }

            if (controller_info[index].connected)
            {
                send_controller_button_update(index, ControllerInputType_A, controller_info[index].button_a);
                send_controller_button_update(index, ControllerInputType_B, controller_info[index].button_b);
                send_controller_button_update(index, ControllerInputType_X, controller_info[index].button_x);
                send_controller_button_update(index, ControllerInputType_Y, controller_info[index].button_y);
                send_controller_button_update(index, ControllerInputType_DirUp, controller_info[index].direction_pad_up);
                send_controller_button_update(index, ControllerInputType_DirDown, controller_info[index].direction_pad_down);
                send_controller_button_update(index, ControllerInputType_DirLeft, controller_info[index].direction_pad_left);
                send_controller_button_update(index, ControllerInputType_DirRight, controller_info[index].direction_pad_right);
                send_controller_button_update(index, ControllerInputType_LSB, controller_info[index].left_shoulder);
                send_controller_button_update(index, ControllerInputType_RSB, controller_info[index].right_shoulder);
                send_controller_button_update(index, ControllerInputType_LT, controller_info[index].left_thumb);
                send_controller_button_update(index, ControllerInputType_RT, controller_info[index].right_thumb);
                send_controller_button_update(index, ControllerInputType_Start, controller_info[index].start);
                send_controller_button_update(index, ControllerInputType_Select, controller_info[index].back);

                send_controller_analog_update(index, ControllerInputType_LeftAnalog, 
                        controller_info[index].left_thumb_stick_x, 
                        controller_info[index].left_thumb_stick_y, 
                        controller_info[index].left_thumb_deadzone);

                send_controller_analog_update(index, ControllerInputType_RightAnalog, 
                        controller_info[index].right_thumb_stick_x, 
                        controller_info[index].right_thumb_stick_y, 
                        controller_info[index].right_thumb_deadzone);

                send_controller_analog_update(index, ControllerInputType_RightTrigger, 
                        controller_info[index].right_trigger, 
                        controller_info[index].right_trigger, 
                        controller_info[index].trigger_deadzone);

                send_controller_analog_update(index, ControllerInputType_LeftTrigger, 
                        controller_info[index].left_trigger, 
                        controller_info[index].left_trigger, 
                        controller_info[index].trigger_deadzone);
            }

        }
        

    }

    if (main_thread.joinable()) main_thread.join();

    if (audio_thread.joinable()) 
    {

        AudioEvent event;
        event.type = AudioEventType_Exit;
        pde_events_push_audio_event(event);

        audio_thread.join();
    }

    if (resource_thread.joinable()) 
    {

        ResourceEvent event;
        event.type = ResourceEventType_Exit;
        pde_events_push_resource_event(event);
        resource_thread.join();

    }

    deconstruct_cli_arguments(argc, argv);
    
    return exit_status;

}

#else
#   pragma error "Undefined platform entry point."
#endif
