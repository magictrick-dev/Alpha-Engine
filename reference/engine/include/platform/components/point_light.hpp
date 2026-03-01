#pragma once
#include <platform/registry.hpp>

struct PointLight : public ComponentInterface
{

    glm::vec3 color     = { 1.0f, 1.0f, 1.0f };
    float constant      = 1.0f;
    float linear        = 0.09f;
    float quadratic     = 0.032f;

    inline virtual void display() override
    {

        ImGui::SeparatorText("Point Light");
        ImGui::DragFloat3("Color", glm::value_ptr(color), 0.01f);
        ImGui::DragFloat("Constant", &constant);
        ImGui::DragFloat("Linear", &linear);
        ImGui::DragFloat("Quadratic", &quadratic);
        ImGui::ColorPicker3("##Color", glm::value_ptr(color), 
                ImGuiColorEditFlags_HDR | 
                ImGuiColorEditFlags_NoLabel |
                ImGuiColorEditFlags_Float);

    }

};

