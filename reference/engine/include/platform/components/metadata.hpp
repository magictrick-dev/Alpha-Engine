#pragma once
#include <platform/registry.hpp>

struct Metadata : public ComponentInterface
{

    Entity parent = { };
    std::vector<Entity> children;

    std::string name = "Unnamed Entity";
    std::vector<std::string> tags;
    bool ignore = false;

    inline virtual void display() override
    {

        ImGui::SeparatorText("Metadata");
        ImGui::InputText("Name", &name);

        ImGui::BeginDisabled();
            ImGui::InputScalar("Index", ImGuiDataType_U32, &self.index);
            ImGui::InputScalar("Generation", ImGuiDataType_U16, &self.generation);
        ImGui::EndDisabled();

    }

};
