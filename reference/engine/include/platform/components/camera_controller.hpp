#pragma once
#include <platform/registry.hpp>
#include <platform/input.hpp>
#include <platform/window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

struct FreeflyCameraController : public ComponentInterface
{

    glm::vec3 front        = { 0.0f, 0.0f, -1.0f };
    glm::vec3 up           = { 0.0f, 1.0f, 0.0f };
    glm::vec3 direction    = { 0.0f, 0.0f, 0.0f };

    float near      = 0.01f;
    float far       = 1000.0f;
    float yaw       = -90.0f;
    float pitch     = 0.0f;
    float aspect    = 16.0f / 9.0f;
    float fov       = 45.0f;
    float speed     = 2.5f;
    float sense     = 2.5f;
    float exposure  = 1.0f;
    float gamma     = 2.2f;

    bool active = false;

    inline void display() override
    {

        glm::vec3 position = self.get<Transform>().position;
        int32_t chunk_x = (int32_t)(position.x / 10.0f);
        int32_t chunk_z = (int32_t)(position.z / 10.0f);

        ImGui::SeparatorText("Camera Controller");
        ImGui::DragFloat("Gamma", &gamma, 0.01f);
        ImGui::DragFloat("Exposure", &exposure, 0.01f);
        ImGui::BeginDisabled();
            ImGui::InputInt("Chunk X", &chunk_x);
            ImGui::InputInt("Chunk Z", &chunk_z);
            ImGui::InputFloat3("Front", glm::value_ptr(front));
            ImGui::InputFloat3("Up", glm::value_ptr(up));
            ImGui::InputFloat3("Direction", glm::value_ptr(direction));
            ImGui::InputFloat("Pitch", &pitch);
            ImGui::InputFloat("Yaw", &yaw);
            ImGui::InputFloat("Aspect Ratio", &aspect);
        ImGui::EndDisabled();
        ImGui::DragFloat("FOV", &fov, 0.01f);
        ImGui::DragFloat("Speed", &speed, 0.01f);
        ImGui::DragFloat("Sensitivity", &sense, 0.01f);

    }

    inline void update(float delta_time) override
    {

        float width = (float)pde_window_get_framebuffer_width();
        float height = (float)pde_window_get_framebuffer_height();
        aspect = width / height;

        if (pde_input_is_input_pressed(InputType_F1)) active = !active;
        if (active == true)
        {

            float speed_modifier = 1.0f;
            if (pde_input_is_input_down(InputType_ShiftLeft)) speed_modifier = 3.2f;

            Transform &transform = self.get<Transform>();

            float speed = speed_modifier * this->speed * delta_time;
            if (pde_input_is_input_down(InputType_KeyW)) transform.position += speed * front;
            if (pde_input_is_input_down(InputType_KeyS)) transform.position -= speed * front;
            if (pde_input_is_input_down(InputType_KeyA)) transform.position -= speed * glm::cross(front, up);
            if (pde_input_is_input_down(InputType_KeyD)) transform.position += speed * glm::cross(front, up);

            float mouse_dx = pde_input_get_mouse_cursor_dx();
            float mouse_dy = pde_input_get_mouse_cursor_dy();
            float wheel = pde_input_get_mouse_wheel_dy();

            yaw += mouse_dx * sense * delta_time;
            pitch -= mouse_dy * sense * delta_time;

            pitch = std::clamp(pitch, -89.0f, 89.0f);
            fov = std::clamp(fov + wheel * delta_time * 20.0f, 30.0f, 179.0f);

            glm::vec3 direction;
            direction.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
            direction.y = sinf(glm::radians(pitch));
            direction.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
            front = glm::normalize(direction);

        }

    }

    inline glm::mat4 get_projection_matrix()
    {

        glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, near, far);
        return projection;

    }

    inline glm::mat4 get_view_matrix()
    {

        Transform &camera_transform = self.get<Transform>();

        glm::mat4 view = glm::lookAt(camera_transform.position, 
                camera_transform.position + front, up);

        return view;

    }

};
