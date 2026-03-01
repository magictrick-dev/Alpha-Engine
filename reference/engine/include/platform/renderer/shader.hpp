#pragma once
#include <platform/filesystem.hpp>

#include <string>
#include <cstdint>
#include <glm/glm.hpp>

typedef void* shader_t;
#define PDE_INVALID_SHADER NULL

enum ShaderSourceType : uint32_t
{
    ShaderSourceType_None       = 0,
    ShaderSourceType_Vertex     = (1 << 0),
    ShaderSourceType_Control    = (1 << 1),
    ShaderSourceType_Evaluation = (1 << 2),
    ShaderSourceType_Geomeotry  = (1 << 3),
    ShaderSourceType_Fragment   = (1 << 4),
    ShaderSourceType_Compute    = (1 << 5),
};

struct ShaderSource
{
    const char *source;
    ShaderSourceType type;
};

struct ShaderSpecification
{
    ShaderSource *sources;
    uint32_t source_count;
};

shader_t pde_renderer_create_shader(ShaderSpecification *specification);
void pde_renderer_destroy_shader(shader_t handle);
void pde_renderer_bind_shader(shader_t handle);

bool pde_renderer_uniform_location_exists(shader_t handle, std::string location);
void pde_renderer_set_uniform(shader_t handle, std::string uniform, int32_t value);
void pde_renderer_set_uniform(shader_t handle, std::string uniform, uint32_t value);
void pde_renderer_set_uniform(shader_t handle, std::string uniform, float value);
void pde_renderer_set_uniform(shader_t handle, std::string uniform, glm::vec2 value);
void pde_renderer_set_uniform(shader_t handle, std::string uniform, glm::vec3 value);
void pde_renderer_set_uniform(shader_t handle, std::string uniform, glm::vec4 value);
void pde_renderer_set_uniform(shader_t handle, std::string uniform, glm::mat4 value);

inline shader_t
pde_renderer_create_vertex_fragment_shader(std::string vertex_shader_path, std::string fragment_shader_path)
{

    std::string vertex_path = pde_filesystem_resolve(vertex_shader_path);
    std::string fragment_path = pde_filesystem_resolve(fragment_shader_path);

    auto vertex_source = pde_filesystem_read_entire_file(vertex_path);
    auto fragment_source = pde_filesystem_read_entire_file(fragment_path);

    vertex_source.push_back('\0');
    fragment_source.push_back('\0');

    if (vertex_source.size() == 0 || fragment_source.size() == 0) return PDE_INVALID_SHADER;

    ShaderSource sources[2];
    sources[0].source = (char*)vertex_source.data();
    sources[0].type = ShaderSourceType_Vertex;

    sources[1].source = (char*)fragment_source.data();
    sources[1].type = ShaderSourceType_Fragment;

    ShaderSpecification shader_specification = {};
    shader_specification.sources = sources;
    shader_specification.source_count = 2;

    return pde_renderer_create_shader(&shader_specification);


}
