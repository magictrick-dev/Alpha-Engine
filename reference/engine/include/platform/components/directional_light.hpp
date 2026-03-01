#pragma once
#include <platform/registry.hpp>

struct DirectionLight : public ComponentInterface
{

    glm::vec3 color = { 1.0f, 1.0f, 1.0f };

    inline virtual void display() override
    {

        ImGui::SeparatorText("Direction Light");
        ImGui::DragFloat3("Color", glm::value_ptr(color), 0.01f);
        ImGui::ColorPicker3("##Color", glm::value_ptr(color), 
                ImGuiColorEditFlags_HDR | 
                ImGuiColorEditFlags_NoLabel |
                ImGuiColorEditFlags_Float);

    }

};

