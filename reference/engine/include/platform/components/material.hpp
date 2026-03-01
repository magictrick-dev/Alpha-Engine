#pragma once
#include <platform/registry.hpp>
#include <platform/renderer/texture.hpp>
#include <glad/glad.h>

struct Material : public ComponentInterface
{

    texture_t diffuse_texture = pde_renderer_get_default_texture();
    texture_t specular_texture = pde_renderer_get_default_void_texture();
    texture_t normal_texture = pde_renderer_get_default_void_texture();
    texture_t height_texture = pde_renderer_get_default_void_texture();
    glm::vec4 color_overlay = { 0.0f, 0.0f, 0.0f, 0.0f };
    float ambient_coefficient   = 0.10f;
    float diffuse_coefficient   = 0.70f;
    float specular_coefficient  = 1.0f;
    float specular_strength     = 64.0f;

    inline void display() override
    {

        ImGui::SeparatorText("Material");

        ImGui::DragFloat("Ambient Coefficient", &ambient_coefficient, 0.01f);
        ImGui::DragFloat("Diffuse Coefficient", &diffuse_coefficient, 0.01f);
        ImGui::DragFloat("Specular Coefficient", &specular_coefficient, 0.01f);
        ImGui::DragFloat("Specular Strength", &specular_strength, 0.01f);
        ImGui::DragFloat4("Color##overlay", glm::value_ptr(color_overlay), 0.01f);
        ImGui::ColorPicker4("##color_overlay", glm::value_ptr(color_overlay), 
                ImGuiColorEditFlags_HDR | 
                ImGuiColorEditFlags_NoLabel |
                ImGuiColorEditFlags_Float |
                ImGuiColorEditFlags_AlphaBar);

        if (ImGui::TreeNodeEx("Diffuse Texture", ImGuiTreeNodeFlags_FramePadding))
        {

            void* handle = pde_renderer_get_raw_texture_handle(diffuse_texture);
            float width = ImGui::GetContentRegionAvail().x;
            ImGui::Image(handle, { width, width });
            ImGui::TreePop();

        }

        if (ImGui::TreeNodeEx("Specular Texture", ImGuiTreeNodeFlags_FramePadding))
        {

            void* handle = pde_renderer_get_raw_texture_handle(specular_texture);
            float width = ImGui::GetContentRegionAvail().x;
            ImGui::Image(handle, { width, width });
            ImGui::TreePop();

        }

        if (ImGui::TreeNodeEx("Normal Texture", ImGuiTreeNodeFlags_FramePadding))
        {

            void* handle = pde_renderer_get_raw_texture_handle(normal_texture);
            float width = ImGui::GetContentRegionAvail().x;
            ImGui::Image(handle, { width, width });
            ImGui::TreePop();

        }

        if (ImGui::TreeNodeEx("Height Texture", ImGuiTreeNodeFlags_FramePadding))
        {

            void* handle = pde_renderer_get_raw_texture_handle(height_texture);
            float width = ImGui::GetContentRegionAvail().x;
            ImGui::Image(handle, { width, width });
            ImGui::TreePop();

        }

    }

};

