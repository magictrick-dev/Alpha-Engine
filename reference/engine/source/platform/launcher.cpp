#include "platform/events/engine_events.hpp"
#include <platform/utilities.hpp>
#include <platform/filesystem.hpp>
#include <platform/launcher.hpp>
#include <platform/window.hpp>
#include <platform/events.hpp>
#include <platform/input.hpp>
#include <platform/registry.hpp>
#include <platform/renderer/context.hpp>
#include <platform/renderer/shader.hpp>
#include <platform/renderer/texture.hpp>
#include <platform/renderer/mesh.hpp>
#include <platform/renderer/framebuffer.hpp>
#include <application/model.hpp>
#include <application/layers/game.hpp>
#include <application/layers/editor.hpp>

#include <print>

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

// --- Application ---------------------------------------------------------------------------------
//
// Generally, you will write most of the application level code here since it isolates any sort
// of engine specific ordering behaviors to more directly defined functions.
//
// NOTE(Chris): During fixed update intervals, you want to do any non-input based updates, such as AI
//              updates, or anything that modifies visual state in fixed intervals. Since input states,
//              such as is_pressed() and is_released() is recorded per-frame, you will miss these events
//              in fixed update if they aren't pressed on the frame that a fixed update occurs.
//
// -------------------------------------------------------------------------------------------------

static EditorLayer editor_layer;
static GameLayer game_layer;
static bool show_editor = true;

static inline int32_t
initialize(int argc, const char **argv)
{

    EntityRegistry::Initialize();
    EntityRegistry::RegisterComponent<Metadata>();
    EntityRegistry::RegisterComponent<Transform>();
    EntityRegistry::RegisterComponent<Material>();
    EntityRegistry::RegisterComponent<Mesh>();
    EntityRegistry::RegisterComponent<Grid>();
    EntityRegistry::RegisterComponent<DirectionLight>();
    EntityRegistry::RegisterComponent<PointLight>();

    if (!game_layer.mount()) return -1;
    if (!editor_layer.mount()) return -1;

    return 0;

}

static inline void
update(float delta_time)
{

    if (pde_input_is_input_pressed(InputType_F12)) show_editor = !show_editor;
    game_layer.update(delta_time);
    editor_layer.update(delta_time);

}

static inline void
fixed_update(float delta_time)
{

    game_layer.fixed_update(delta_time);
    editor_layer.fixed_update(delta_time);

}

static inline void
render()
{

    game_layer.render();
    if (show_editor) editor_layer.render();

}

// --- Main Thread ---------------------------------------------------------------------------------
//
// The main thread is responsible for the larger context of the application, including rendering and
// updating the runtime state. Most of your work occurs here.
//
// -------------------------------------------------------------------------------------------------

int32_t 
pde_launcher_thread_main(int32_t argc, const char **argv)
{

    pde_logging_engine_info("Primary engine thread has successfully launched.");

    if (!pde_renderer_create_context())
    {

        PlatformEvent event = {};
        event.type = PlatformEventType_Exit;
        event.exit.code = -1;

        pde_events_push_platform_event(event);
        return -1;

    }

    if (initialize(argc, argv) != 0)
    {

        PlatformEvent event = {};
        event.type = PlatformEventType_Exit;
        event.exit.code = -1;

        pde_events_push_platform_event(event);
        return -1;

    }

    glm::vec2 framebuffer_size;
    framebuffer_size.x = pde_window_get_framebuffer_width();
    framebuffer_size.y = pde_window_get_framebuffer_height();

    FramebufferSpecification specification = {};
    specification.color_format = FramebufferColorFormatType_Float16;
    specification.attachment_format = FramebufferAttachmentFormatType_Depth24_Stencil8;
    specification.multisample_type = FramebufferMultisampleType_MSAA4;
    specification.target = NULL; // Default framebuffer.
    specification.width = pde_window_get_framebuffer_width();
    specification.height = pde_window_get_framebuffer_height();

    framebuffer_t primary_framebuffer = pde_renderer_create_framebuffer(&specification);
    if (primary_framebuffer == PDE_INVALID_FRAMEBUFFER)
    {

        PlatformEvent event = {};
        event.type = PlatformEventType_Exit;
        event.exit.code = -1;

        pde_events_push_platform_event(event);
        return -1;

    }

    std::string mesh_vertex_path = pde_filesystem_resolve("./resources/shaders/mesh.vertex.glsl");
    std::string mesh_fragment_path = pde_filesystem_resolve("./resources/shaders/mesh.fragment.glsl");
    {
        ResourceEvent resource_event = {};
        resource_event.type = ResourceEventType_MonitorAdd;
        strcpy_s(resource_event.monitor_add.path, mesh_vertex_path.c_str());
        pde_events_push_resource_event(resource_event);
    }
    {
        ResourceEvent resource_event = {};
        resource_event.type = ResourceEventType_MonitorAdd;
        strcpy_s(resource_event.monitor_add.path, mesh_fragment_path.c_str());
        pde_events_push_resource_event(resource_event);
    }

    float delta_time = 1.0f / 60.0f;
    float accumulator = 0.0f;
    uint64_t frame_count = 0;
    constexpr float fixed_step = 1.0f / 20.0f;
    timestamp_t frame_begin = pde_utilities_get_timestamp();

    while (!pde_window_should_close())
    {

        EngineEvent event;
        pde_events_swap_engine_event_queue();
        pde_input_swap_input_buffers();
        while (pde_events_pop_engine_event(&event))
        {

            switch (event.type)
            {

                case EngineEventType_WindowFramebufferResize:
                {
                    int32_t width = event.window_framebuffer_resize.width;
                    int32_t height = event.window_framebuffer_resize.height;
                    framebuffer_size.x = width;
                    framebuffer_size.y = height;
                } break;

                case EngineEventType_ResourceMonitorUpdate:
                {
                    std::string path = event.resource_monitor_update.path;
                    if (event.resource_monitor_update.update_type == ResourceMonitorUpdateType_Updated)
                    {
                        pde_logging_engine_info(std::format("Resource {} has updated.", path));
                    }
                } break;

                case EngineEventType_KeyInput:
                {
                    pde_input_set_input((InputType)event.key_input.code, 
                            event.key_input.down);
                } break;

                case EngineEventType_MouseMove:
                {
                    pde_input_set_mouse_position(event.mouse_move.x, 
                            event.mouse_move.y);
                } break;

                case EngineEventType_MouseButton:
                {
                    pde_input_set_input((InputType)event.mouse_button.code,
                            event.mouse_button.down);
                } break;

                case EngineEventType_MouseScroll:
                {
                    pde_input_set_mouse_wheel(event.mouse_scroll.x,
                            event.mouse_scroll.y);
                } break;

                case EngineEventType_MouseCapture:
                {
                    pde_input_set_mouse_capture(event.mouse_capture.captured);
                } break;

                case EngineEventType_MouseView:
                {
                    pde_input_set_mouse_visibility(event.mouse_view.inside);
                } break;

                case EngineEventType_ControllerConnection:
                {
                    pde_input_set_controller_connection(event.controller_connection.index,
                            event.controller_connection.connected);
                    pde_logging_engine_info(std::format("Controller {} status: {}",
                                event.controller_connection.index,
                                event.controller_connection.connected));
                } break;

                default: break;
            }

        }

        update(delta_time);
        if (accumulator >= fixed_step)
        {
            fixed_update(accumulator);
            accumulator -= fixed_step;
        }

        if (pde_window_get_framebuffer_width() != 0 && pde_window_get_framebuffer_height() != 0)
        {

            // NOTE(Chris): The framebuffer is potentially out of date, we will need to check
            //              and rebuild the framebuffer with the new size.
            if (pde_window_get_framebuffer_width() != specification.width ||
                pde_window_get_framebuffer_height() != specification.height)
            {

                pde_renderer_destroy_framebuffer(primary_framebuffer);

                specification.width = pde_window_get_framebuffer_width();
                specification.height = pde_window_get_framebuffer_height();
                primary_framebuffer = pde_renderer_create_framebuffer(&specification);

                if (primary_framebuffer == PDE_INVALID_FRAMEBUFFER)
                {

                    PlatformEvent event = {};
                    event.type = PlatformEventType_Exit;
                    event.exit.code = -1;

                    pde_events_push_platform_event(event);
                    return -1;

                }

            }

            pde_renderer_begin_frame(framebuffer_size, { 0.105f, 0.105f, 0.1f, 1.0f });
            pde_renderer_bind_framebuffer(primary_framebuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            glClearColor(0.105f, 0.105f, 0.1f, 1.0f);

            render();

            pde_renderer_blit_framebuffer_to_target_framebuffer(primary_framebuffer); // To screen.
            pde_renderer_bind_framebuffer(NULL);
            pde_renderer_finalize_frame();
            pde_renderer_swap_buffers();

        }

        timestamp_t frame_end = pde_utilities_get_timestamp();
        float frame_time = pde_utilities_get_time_difference(frame_begin, frame_end); 
        delta_time = frame_time;
        accumulator += frame_time;
        frame_begin = frame_end;

        pde_events_wait_for_engine_events();

    }

    pde_renderer_destroy_context();

    return 0;

}

// --- Resource Thread -----------------------------------------------------------------------------
//
// The resource thread is responsible for handling any dispatched resource requests and feeding them
// back to the engine.
//
// -------------------------------------------------------------------------------------------------

struct FileMonitorStatus
{

    std::string file_path;
    std::size_t file_time;

    inline void check()
    {
        if (pde_filesystem_file_is_ready(file_path))
        {
            size_t new_time = pde_filesystem_file_time(file_path);
            if (new_time > file_time)
            {

                this->file_time = new_time;

                EngineEvent event;
                event.type = EngineEventType_ResourceMonitorUpdate;
                event.resource_monitor_update.update_type = ResourceMonitorUpdateType_Updated;
                strcpy_s(event.resource_monitor_update.path, file_path.c_str());
                pde_events_push_engine_event(event);

            }
        }
    }

};

std::unordered_map<std::string, FileMonitorStatus> file_monitors;

int32_t 
pde_launcher_thread_resource()
{

    pde_logging_resource_info("Resource thread has successfully launched.");

    while (!pde_window_should_close())
    {

        ResourceEvent event;
        pde_events_swap_resource_event_queue();
        while (pde_events_pop_resource_event(&event))
        {

            switch (event.type)
            {
                case ResourceEventType_MonitorAdd:
                {
                    if (file_monitors.contains(event.monitor_add.path)) break;

                    std::string path = event.monitor_add.path;
                    if (pde_filesystem_path_exists_as_file(path))
                    {
                        FileMonitorStatus status;
                        status.file_path = path;
                        status.file_time = pde_filesystem_file_time(path);

                        file_monitors[event.monitor_add.path] = status;
                        pde_logging_resource_info(std::format("Monitoring file {}", path));

                    }
                    else
                    {
                        pde_logging_resource_error(std::format("Unable to monitor {}, file does not exist.", path));
                    }

                } break;

                case ResourceEventType_MonitorRemove:
                {
                    if (file_monitors.contains(event.monitor_remove.path)) break;
                    std::string path = event.monitor_remove.path;
                    file_monitors.erase(path);
                    pde_logging_resource_info(std::format("Unmonitoring file {}", path));
                } break;

                case ResourceEventType_MonitorQuery:
                {
                    const bool result = file_monitors.contains(event.monitor_query.path);
                    EngineEvent engine_event;
                    engine_event.type = EngineEventType_ResourceMonitorUpdate;
                    engine_event.resource_monitor_update.update_type = (result) ? 
                        ResourceMonitorUpdateType_Monitoring :
                        ResourceMonitorUpdateType_NotMonitoring;
                    strcpy_s(engine_event.resource_monitor_update.path, event.monitor_query.path);
                    pde_events_push_engine_event(engine_event);
                };

                default: break;
            }

        }

        for (auto &monitored_resource : file_monitors)
            monitored_resource.second.check();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

    }


    return 0;
}

// --- Audio Thread --------------------------------------------------------------------------------
//
// The audio thread is responsible for handling and playing audio.
//
// -------------------------------------------------------------------------------------------------

int32_t 
pde_launcher_thread_audio()
{

    pde_logging_audio_info("Audio thread has successfully launched.");

    while (!pde_window_should_close())
    {

        AudioEvent event;
        pde_events_swap_audio_event_queue();
        while (pde_events_pop_audio_event(&event))
        {

            switch (event.type)
            {
                case AudioEventType_Exit: return 0;
                default: break;
            }

        }

        pde_events_wait_for_audio_events();

    }

    std::println("Leaving thread audio.");   

    return 0;
}
