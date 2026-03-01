#include <platform/renderer/mesh.hpp>
#include <glad/glad.h>
#include <cassert>
#include <glm/gtc/constants.hpp>

struct Mesh
{
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    size_t count;
};

mesh_t 
pde_renderer_create_mesh(MeshSpecification *specification)
{

    if (specification == NULL) return PDE_INVALID_MESH;
    if (!specification->vertices.size()) return PDE_INVALID_MESH;
    if (!specification->indices.size()) return PDE_INVALID_MESH;

    Mesh *mesh = new Mesh();

    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->IBO);

    glBindVertexArray(mesh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(MeshVertex) * specification->vertices.size(),
            specification->vertices.data(),
            GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(offsetof(MeshVertex, texture_coordinates)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
            sizeof(MeshIndex) * specification->indices.size(),
            specification->indices.data(),
            GL_STATIC_DRAW);

    glBindVertexArray(NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);

    mesh->count = specification->indices.size();
    return mesh;

}

void 
pde_renderer_destroy_mesh(mesh_t handle)
{

    Mesh *mesh = (Mesh*)handle;
    glDeleteBuffers(1, &mesh->VBO);
    glDeleteBuffers(1, &mesh->IBO);
    glDeleteVertexArrays(1, &mesh->VAO);
    delete mesh;
    
}

void 
pde_renderer_draw_mesh(mesh_t handle, DrawMeshFormatType draw_type)
{

    Mesh *mesh = (Mesh*)handle;

    glBindVertexArray(mesh->VAO);
    
    GLuint style;
    switch (draw_type)
    {
        case DrawMeshFormatType_Points: style = GL_POINTS; break;
        case DrawMeshFormatType_Lines: style = GL_LINES; break;
        case DrawMeshFormatType_Triangles: style = GL_TRIANGLES; break;
        default: assert(!"Undefined draw format.");
    }

    glDrawElements(GL_TRIANGLES, mesh->count, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(NULL);

}

mesh_t
pde_renderer_get_sphere_mesh()
{

    static mesh_t cached_mesh = PDE_INVALID_MESH;
    if (cached_mesh != PDE_INVALID_MESH) return cached_mesh;

    MeshSpecification spec;

    const unsigned int slices = 48;
    const unsigned int stacks = 24;
    const float radius = 0.5f;

    for (unsigned int stack = 0; stack <= stacks; stack++)
    {
        float phi = glm::pi<float>() * (float)stack / (float)stacks; // latitude
        float y = cosf(phi);
        float r = sinf(phi);

        for (unsigned int slice = 0; slice <= slices; slice++)
        {
            float theta = 2.0f * glm::pi<float>() * (float)slice / (float)slices; // longitude
            float x = r * cosf(theta);
            float z = r * sinf(theta);

            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            MeshVertex vert;
            vert.position = normal * radius;
            vert.normal = normal;
            vert.texture_coordinates = glm::vec2(
                (float)slice / (float)slices,
                1.0f - (float)stack / (float)stacks
            );

            spec.vertices.push_back(vert);
        }
    }

    // Generate indices
    for (unsigned int stack = 0; stack < stacks; stack++)
    {
        for (unsigned int slice = 0; slice < slices; slice++)
        {
            unsigned int first  = (stack * (slices + 1)) + slice;
            unsigned int second = first + slices + 1;

            spec.indices.push_back(first);
            spec.indices.push_back(second);
            spec.indices.push_back(first + 1);

            spec.indices.push_back(second);
            spec.indices.push_back(second + 1);
            spec.indices.push_back(first + 1);
        }
    }

    cached_mesh = pde_renderer_create_mesh(&spec);
    return cached_mesh;

}

mesh_t
pde_renderer_get_cube_mesh()
{

    static mesh_t cached_mesh = PDE_INVALID_MESH;
    if (cached_mesh != PDE_INVALID_MESH) return cached_mesh;

    MeshSpecification spec = {};

    glm::vec3 positions[24] =
    {
        {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f},
        { 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f},
        { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f}
    };

    glm::vec3 normals[24] =
    {
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f,-1.0f}, {0.0f, 0.0f,-1.0f}, {0.0f, 0.0f,-1.0f}, {0.0f, 0.0f,-1.0f},
        {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},
        {0.0f,-1.0f, 0.0f},  {0.0f,-1.0f, 0.0f},  {0.0f,-1.0f, 0.0f},  {0.0f,-1.0f, 0.0f}
    };

    glm::vec2 uvs[24];
    for (int f = 0; f < 6; f++)
    {
        uvs[f * 4 + 0] = glm::vec2(0.0f, 0.0f);
        uvs[f * 4 + 1] = glm::vec2(1.0f, 0.0f);
        uvs[f * 4 + 2] = glm::vec2(1.0f, 1.0f);
        uvs[f * 4 + 3] = glm::vec2(0.0f, 1.0f);
    }

    for (int i = 0; i < 24; i++)
    {
        MeshVertex mv;
        mv.position = positions[i];
        mv.normal = normals[i];
        mv.texture_coordinates = uvs[i];
        spec.vertices.push_back(mv);
    }

    for (uint32_t face = 0; face < 6; face++)
    {
        uint32_t base = face * 4;
        spec.indices.push_back(base + 0);
        spec.indices.push_back(base + 1);
        spec.indices.push_back(base + 2);
        spec.indices.push_back(base + 2);
        spec.indices.push_back(base + 3);
        spec.indices.push_back(base + 0);
    }

    cached_mesh = pde_renderer_create_mesh(&spec);
    return cached_mesh;

}

mesh_t
pde_renderer_get_quad_mesh()
{

    static mesh_t cached_mesh = PDE_INVALID_MESH;
    if (cached_mesh != PDE_INVALID_MESH) return cached_mesh;
    
    MeshSpecification spec;

    MeshVertex v0 { {-0.5f, -0.5f, 0.0f}, {0, 0, 1}, {0, 0} };
    MeshVertex v1 { { 0.5f, -0.5f, 0.0f}, {0, 0, 1}, {1, 0} };
    MeshVertex v2 { { 0.5f,  0.5f, 0.0f}, {0, 0, 1}, {1, 1} };
    MeshVertex v3 { {-0.5f,  0.5f, 0.0f}, {0, 0, 1}, {0, 1} };

    spec.vertices = {v0, v1, v2, v3};
    spec.indices  = {0, 1, 2, 2, 3, 0};

    cached_mesh = pde_renderer_create_mesh(&spec);

    return cached_mesh;

}

