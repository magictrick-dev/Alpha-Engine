#pragma once
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

typedef void* mesh_t;
#define PDE_INVALID_MESH NULL

enum DrawMeshFormatType : uint32_t
{
    DrawMeshFormatType_Points,
    DrawMeshFormatType_Lines,
    DrawMeshFormatType_Triangles,
};

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
};

typedef uint32_t MeshIndex;

struct MeshSpecification
{
    std::vector<MeshVertex> vertices;
    std::vector<MeshIndex> indices;
};

mesh_t pde_renderer_create_mesh(MeshSpecification *specification);
void pde_renderer_destroy_mesh(mesh_t handle);
void pde_renderer_draw_mesh(mesh_t handle, DrawMeshFormatType draw_type = DrawMeshFormatType_Triangles);

mesh_t pde_renderer_get_sphere_mesh();
mesh_t pde_renderer_get_cube_mesh();
mesh_t pde_renderer_get_quad_mesh();

