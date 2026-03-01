#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui/imgui.h>
#include <platform/registry.hpp>

struct Transform : public ComponentInterface
{

    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    inline glm::mat4 compute_parent_transform()
    {

        glm::mat4 transformation(1.0f);
        Entity parent = self.get<Metadata>().parent;
        while (parent.tag != 0)
        {
            transformation *= parent.get<Transform>().compute_transform();
            parent = parent.get<Metadata>().parent;
        }

        return transformation;


    }

    inline glm::mat4 compute_transform() const
    {

        const glm::quat quaternion = glm::radians(rotation);

        const glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position);
        const glm::mat4 rotation_matrix = glm::mat4_cast(quaternion);
        const glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);
        const glm::mat4 result = translation_matrix * rotation_matrix * scale_matrix;
        return result;

    }

    inline glm::mat4 compute_inverse_transform() const
    {

        const glm::quat quaternion = glm::radians(rotation);

        const glm::mat4 rotation_matrix = glm::mat4_cast(quaternion);
        const glm::mat4 rotation_inverse = glm::transpose(glm::inverse(rotation_matrix));
        const glm::mat4 scale_inverse = glm::scale(glm::mat4(1.0f), 1.0f/(scale));

        const glm::mat4 result = rotation_inverse * scale_inverse;
        return result;

    }

    inline void display() override
    {

        ImGui::SeparatorText("Transform");
        ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.1f);
        ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f);

    }

};

