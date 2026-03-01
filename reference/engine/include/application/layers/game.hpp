#pragma once
#include <platform/registry.hpp>
#include <platform/events.hpp>
#include <platform/renderer/context.hpp>
#include <platform/renderer/shader.hpp>
#include <platform/renderer/texture.hpp>
#include <platform/renderer/mesh.hpp>
#include <platform/renderer/framebuffer.hpp>

#include <application/layers/layer_interface.hpp>
#include <application/model.hpp>

#include <stack>
#include <queue>

class GameLayer : public LayerInterface
{

    public:
        inline virtual bool mount() override;
        inline virtual bool unmount() override;

        inline virtual void update(float delta_time) override;
        inline virtual void fixed_update(float delta_time) override;
        inline virtual void render() override;

    private:
        inline void attach_point_light(Entity entity, int32_t &index);
        inline void render_entity(Entity entity, 
                glm::mat4 parent_transform = glm::mat4(1.0f),
                glm::mat4 parent_inverse_transform = glm::mat4(1.0f));

    private:
        mesh_t mesh_shader;

};

bool GameLayer::
mount()
{

    mesh_shader = pde_renderer_create_vertex_fragment_shader(
            "./resources/shaders/mesh.vertex.glsl",
            "./resources/shaders/mesh.fragment.glsl");
    if (mesh_shader == PDE_INVALID_SHADER) return false;
    
    return true;

}

bool GameLayer::
unmount()
{

    return true;
}

void GameLayer::
update(float delta_time)
{

}

void GameLayer::
fixed_update(float delta_time)
{

}

void GameLayer::
attach_point_light(Entity entity, int32_t &index)
{

    if (index >= 16) return;

    Transform &transform = entity.get<Transform>();
    Metadata &meta = entity.get<Metadata>();

    glm::vec4 position = transform.compute_parent_transform() * glm::vec4(transform.position, 1.0f);

    PointLight &light = entity.get<PointLight>();
    pde_renderer_set_uniform(mesh_shader, std::format("point_lights[{}].position", index), glm::vec3(position));
    pde_renderer_set_uniform(mesh_shader, std::format("point_lights[{}].color", index), light.color);
    pde_renderer_set_uniform(mesh_shader, std::format("point_lights[{}].constant", index), light.constant);
    pde_renderer_set_uniform(mesh_shader, std::format("point_lights[{}].linear", index), light.linear);
    pde_renderer_set_uniform(mesh_shader, std::format("point_lights[{}].quadratic", index), light.quadratic);

    index++;

}

void GameLayer::
render_entity(Entity entity, glm::mat4 parent_transform, glm::mat4 parent_inverse_transform)
{


    Metadata &metadata = entity.get<Metadata>();
    Transform &transform = entity.get<Transform>();

    auto model = parent_transform * transform.compute_transform();
    auto model_inverse = parent_inverse_transform * transform.compute_inverse_transform();

    // Recursively render the children first with the parent transforms.
    for (auto child : metadata.children) render_entity(child, model, model_inverse);

    if (!entity.has<Material, Mesh>()) return;
    Material &material = entity.get<Material>();
    Mesh &mesh = entity.get<Mesh>();

    int32_t diffuse_slot = 0;
    int32_t specular_slot = 1;
    int32_t normal_slot = 2;

    pde_renderer_set_uniform(mesh_shader, "color_overlay", material.color_overlay);
    pde_renderer_set_uniform(mesh_shader, "ambient_coefficient", material.ambient_coefficient);
    pde_renderer_set_uniform(mesh_shader, "diffuse_coefficient", material.diffuse_coefficient);
    pde_renderer_set_uniform(mesh_shader, "specular_coefficient", material.specular_coefficient);
    pde_renderer_set_uniform(mesh_shader, "specular_strength", material.specular_strength);

    glActiveTexture(GL_TEXTURE0 + diffuse_slot);
    glBindTexture(GL_TEXTURE_2D, (GLuint)((uint64_t)pde_renderer_get_raw_texture_handle(material.diffuse_texture)));
    pde_renderer_set_uniform(mesh_shader, "texture_diffuse1", diffuse_slot);

    glActiveTexture(GL_TEXTURE0 + specular_slot);
    glBindTexture(GL_TEXTURE_2D, (GLuint)((uint64_t)pde_renderer_get_raw_texture_handle(material.specular_texture)));
    pde_renderer_set_uniform(mesh_shader, "texture_specular1", specular_slot);
    if (!pde_renderer_texture_is_defaulted(material.specular_texture))
        pde_renderer_set_uniform(mesh_shader, "use_specular_texture", 1);

    glActiveTexture(GL_TEXTURE0 + normal_slot);
    glBindTexture(GL_TEXTURE_2D, (GLuint)((uint64_t)pde_renderer_get_raw_texture_handle(material.normal_texture)));
    pde_renderer_set_uniform(mesh_shader, "texture_normals1", normal_slot);
    if (!pde_renderer_texture_is_defaulted(material.normal_texture))
        pde_renderer_set_uniform(mesh_shader, "use_normal_texture", 1);

    pde_renderer_set_uniform(mesh_shader, "u_model", model);
    pde_renderer_set_uniform(mesh_shader, "u_model_inverse", model_inverse);

    pde_renderer_draw_mesh(mesh.mesh);

}

void GameLayer::
render()
{

    // Bind the shader.
    pde_renderer_bind_shader(mesh_shader);

    // We need a camera, so we will grab the first freefly camera available.
    auto cameras = EntityRegistry::GetEntitiesWith<false, FreeflyCameraController>(false);
    if (cameras.size() <= 0) return;
    auto camera = cameras[0];

    // Fetching a direction light, if one exists.
    auto directional_lights = EntityRegistry::GetEntitiesWith<false, DirectionLight>();
    if (directional_lights.size() > 0)
    {
        Transform &transform = directional_lights[0].get<Transform>();
        DirectionLight &light = directional_lights[0].get<DirectionLight>();
        pde_renderer_set_uniform(mesh_shader, "direction_light_color", light.color);
        pde_renderer_set_uniform(mesh_shader, "direction_light_position", transform.position);
    }

    // Fetching point lights, of they exist.
    int32_t count = EntityRegistry::GetEntitiesWith<false, PointLight>().size();
    if (count > 16) count = 16;
    pde_renderer_set_uniform(mesh_shader, "point_light_count", count);

    auto point_lights = EntityRegistry::GetEntitiesWith<false, PointLight>();
    int32_t index = 0;
    for (auto light : point_lights) attach_point_light(light, index);

    // Fetching camera and controller to bind our uniforms.
    auto &camera_transform = camera.get<Transform>();
    auto &camera_controller = camera.get<FreeflyCameraController>();
    
    auto projection = camera_controller.get_projection_matrix();
    auto view = camera_controller.get_view_matrix();
    auto camera_position = camera_transform.position;
    auto camera_gamma = camera_controller.gamma;
    auto camera_exposure = camera_controller.exposure;

    pde_renderer_set_uniform(mesh_shader, "u_projection", projection);
    pde_renderer_set_uniform(mesh_shader, "u_view", view);
    pde_renderer_set_uniform(mesh_shader, "u_camera", camera_position);
    pde_renderer_set_uniform(mesh_shader, "u_gamma", camera_gamma);
    pde_renderer_set_uniform(mesh_shader, "u_exposure", camera_exposure);

    auto entities = EntityRegistry::GetEntitiesWith<true, Transform, Material, Mesh>();
    for (auto entity : entities) render_entity(entity);

    pde_renderer_bind_shader(NULL);

}
