#pragma once
#include <platform/registry.hpp>
#include <platform/renderer/mesh.hpp>
#include <platform/renderer/shader.hpp>

struct Grid : public ComponentInterface
{

    mesh_t mesh = pde_renderer_get_quad_mesh();

    inline virtual void display() override
    {

        ImGui::SeparatorText("Grid");

    }

};

