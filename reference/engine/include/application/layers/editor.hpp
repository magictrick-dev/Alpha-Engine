#pragma once
#include "platform/renderer/shader.hpp"
#include <application/layers/layer_interface.hpp>
#include <application/editor/scene_graph.hpp>
#include <application/editor/inspector.hpp>
#include <application/editor/console.hpp>
#include <platform/input.hpp>
#include <imgui/imgui.h>
#include <imgui/implot.h>

class EditorLayer : public LayerInterface
{

    public:
        inline virtual bool mount() override;
        inline virtual bool unmount() override;

        inline virtual void update(float delta_time) override;
        inline virtual void fixed_update(float delta_time) override;
        inline virtual void render() override;

    private:
        Entity editor_camera;
        Entity grid;

        shader_t grid_shader;

        bool camera_movement = false;
        bool debug = false;

        InspectorEditor inspector;
        SceneGraphEditor scene_graph;
        ConsoleEditor console;

};

bool EditorLayer::
mount()
{

    editor_camera = EntityRegistry::CreateEntity();
    editor_camera.attach<FreeflyCameraController>();
    editor_camera.get<Metadata>().name = "Editor Camera";
    editor_camera.get<Metadata>().ignore = true;

    grid = EntityRegistry::CreateEntity();
    grid.get<Metadata>().name = "Grid";
    grid.get<Metadata>().ignore = true;
    Material &grid_material = grid.attach<Material>();
    Transform &transform = grid.get<Transform>();
    Mesh &grid_mesh = grid.attach<Mesh>();
    grid_mesh.mesh = pde_renderer_get_quad_mesh();
    transform.position.y = -0.5f;
    transform.rotation.x = -90.0f;
    transform.scale = { 10.0f, 10.0f, 1.0f };

    std::string grid_vertex_path = pde_filesystem_resolve("./resources/shaders/grid.vertex.glsl");
    std::string grid_fragment_path = pde_filesystem_resolve("./resources/shaders/grid.fragment.glsl");
    if (grid_vertex_path.empty()) return false;
    if (grid_fragment_path.empty()) return false;
    grid_shader = pde_renderer_create_vertex_fragment_shader(grid_vertex_path, grid_fragment_path);
    if (grid_shader == PDE_INVALID_SHADER) return false;

    return true;

}

bool EditorLayer::
unmount()
{

    return true;
}

void EditorLayer::
update(float delta_time)
{

    if (pde_input_is_input_pressed(InputType_F1))
    {
        cursor_toggle_lock();
        camera_movement = !camera_movement;
    }
    if (pde_input_is_input_pressed(InputType_F2)) console.toggle_window();
    if (pde_input_is_input_pressed(InputType_F3)) inspector.toggle_window();
    if (pde_input_is_input_pressed(InputType_F4)) scene_graph.toggle_window();
    if (pde_input_is_input_pressed(InputType_F9)) debug = !debug;

    console.update();
    inspector.update();
    inspector.set_inspection(scene_graph.get_viewed_entity());
    scene_graph.update();

    editor_camera.get<FreeflyCameraController>().update(delta_time);

}

void EditorLayer::
fixed_update(float delta_time)
{

}

void EditorLayer::
render()
{

    if (camera_movement) ImGui::BeginDisabled();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 10.0f, 5.0f });

        if (ImGui::BeginMainMenuBar())
        {

            if (ImGui::BeginMenu("File"))
            {

                ImGui::MenuItem("Open Project", "Ctrl-o");
                ImGui::MenuItem("Save Project", "Crtl-s");
                ImGui::MenuItem("Save-as Project", "Shift Ctrl-s");
                ImGui::Separator();
                ImGui::MenuItem("About");
                ImGui::EndMenu();

            }

            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Inspector", "F2")) console.toggle_window();
                if (ImGui::MenuItem("Inspector", "F3")) inspector.toggle_window();
                if (ImGui::MenuItem("Scene Graph", "F4")) scene_graph.toggle_window();
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

    ImGui::PopStyleVar();

    console.show();
    inspector.show();
    scene_graph.show();

    if (debug)
    {
        ImPlot::ShowDemoWindow();
        ImGui::ShowDemoWindow(&debug);
    }
    if (camera_movement) ImGui::EndDisabled();

    {

        constexpr float grid_scale = 100.0f;
        glm::vec3 position = editor_camera.get<Transform>().position;
        int32_t chunk_x = (int32_t)((position.x) / grid_scale) - 2;
        int32_t chunk_z = (int32_t)((position.z) / grid_scale) - 2;

        pde_renderer_bind_shader(grid_shader);
        Transform grid_transform = grid.get<Transform>();
        Material grid_material = grid.get<Material>();
        Mesh grid_mesh = grid.get<Mesh>();
        grid_transform.scale.x = grid_scale;
        grid_transform.scale.y = grid_scale;

        auto &camera_controller = editor_camera.get<FreeflyCameraController>();
        auto &camera_transform = editor_camera.get<Transform>();

        glm::mat4 projection = camera_controller.get_projection_matrix();
        glm::mat4 view = camera_controller.get_view_matrix();

        pde_renderer_set_uniform(grid_shader, "u_projection", projection);
        pde_renderer_set_uniform(grid_shader, "u_view", view);
        pde_renderer_set_uniform(grid_shader, "u_camera", camera_transform.position);
        pde_renderer_set_uniform(grid_shader, "u_exposure", camera_controller.exposure);
        pde_renderer_set_uniform(grid_shader, "u_gamma", camera_controller.gamma);

        for (int32_t x = chunk_x; x <= chunk_x + 5; ++x)
        {
            grid_transform.position.x = x * grid_scale;
            for (int32_t z = chunk_z; z <= chunk_z + 5; ++z)
            {
                grid_transform.position.z = z * grid_scale;
                glm::mat4 model = grid_transform.compute_transform();
                glm::mat4 model_inverse = grid_transform.compute_inverse_transform();
                pde_renderer_set_uniform(grid_shader, "u_model", model);
                pde_renderer_set_uniform(grid_shader, "u_model_inverse", model_inverse);
                pde_renderer_draw_mesh(grid_mesh.mesh);
            }
        }

    }

}
