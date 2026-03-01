#pragma once
#include <platform/registry.hpp>
#include <platform/renderer/mesh.hpp>
#include <platform/renderer/shader.hpp>

struct Mesh : public ComponentInterface
{

    mesh_t mesh = pde_renderer_get_sphere_mesh();

    inline virtual void display() override
    {

        ImGui::SeparatorText("Mesh");
        if (ImGui::BeginCombo("Replace Mesh", "Select Mesh"))
        {
            if (ImGui::Selectable("Cube Mesh")) mesh = pde_renderer_get_cube_mesh();
            if (ImGui::Selectable("Sphere Mesh")) mesh = pde_renderer_get_sphere_mesh();
            if (ImGui::Selectable("Quad Mesh")) mesh = pde_renderer_get_quad_mesh();
            ImGui::EndCombo();
        }

    }

};

