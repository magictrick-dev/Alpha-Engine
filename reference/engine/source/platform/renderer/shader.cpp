#include <platform/renderer/shader.hpp>
#include <cassert>
#include <print>
#include <stack>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <glad/glad.h>

static const char * screen_vertex_shader = R"""(#version 460 core
layout (location = 0) in vec2 v_offset;
layout (location = 1) in vec2 v_texture;
layout (location = 0) out vec2 texture_coordinates;

void main()
{

    gl_Position = vec4(v_offset, 0.0f, 1.0f);
    texture_coordinates = v_texture;

}

)""";

static const char * screen_fragment_shader = R"""(#version 460 core
layout (location = 0) in vec2 texture_coordinates;
layout (location = 0) out vec4 fragment;

uniform sampler2D screen_texture;

void main()
{
    fragment = texture(screen_texture, texture_coordinates);
}

)""";

struct Shader
{
    GLuint program;
    std::unordered_map<std::string, GLint> uniforms;
};

static inline GLuint 
convert_to_type(ShaderSourceType type)
{

    switch (type)
    {

        case ShaderSourceType_Vertex: return GL_VERTEX_SHADER;
        case ShaderSourceType_Control: return GL_TESS_CONTROL_SHADER;
        case ShaderSourceType_Evaluation: return GL_TESS_EVALUATION_SHADER;
        case ShaderSourceType_Geomeotry: return GL_GEOMETRY_SHADER;
        case ShaderSourceType_Fragment: return GL_FRAGMENT_SHADER;
        case ShaderSourceType_Compute: return GL_COMPUTE_SHADER;
        default: assert(!"Unimplemented shader source type.");

    }

    return 0;

}

static inline GLuint 
compile_shader(const char *source, GLuint type)
{

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char log[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {

        glGetShaderInfoLog(shader, 512, NULL, log);
        std::println("{}", log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;

}

shader_t 
pde_renderer_create_shader(ShaderSpecification *specification)
{

    if (specification == NULL) return PDE_INVALID_SHADER;
    if (specification->source_count <= 0) return PDE_INVALID_SHADER;

    GLuint program = glCreateProgram();
    std::stack<GLuint> shaders;

    for (uint32_t index = 0; index < specification->source_count; ++index)
    {

        const char *source = specification->sources[index].source;
        GLuint type = convert_to_type(specification->sources[index].type);
        GLuint shader = compile_shader(source, type);
        if (shader == NULL)
        {
            glDeleteProgram(program);
            return PDE_INVALID_SHADER;
        }

        glAttachShader(program, shader);

    }

    glLinkProgram(program);
    
    while (!shaders.empty())
    {
        glDeleteShader(shaders.top());
        shaders.pop();
    }

    int success;
    char log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) 
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        std::println("{}", log);
        glDeleteProgram(program);
        return PDE_INVALID_SHADER;
    }

#if 0
    GLint count = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

    for (GLint i = 0; i < count; i++) {
        char name[256];
        GLsizei length;
        GLint size;
        GLenum type;
        glGetActiveUniform(program, i, sizeof(name), &length, &size, &type, name);
        std::cout << i << ": " << name << " (size=" << size << ")\n";
    }
#endif

    Shader *shader = new Shader();
    shader->program = program;
    return shader;

}

void 
pde_renderer_destroy_shader(shader_t handle)
{

    Shader *shader = (Shader*)handle;
    glDeleteProgram(shader->program);
    delete shader;

}

void 
pde_renderer_bind_shader(shader_t handle)
{

    if (handle == NULL)
    {
        glUseProgram(NULL);
        return;
    }

    Shader *shader = (Shader*)handle;
    glUseProgram(shader->program);

}

bool 
pde_renderer_uniform_location_exists(shader_t handle, std::string location)
{

    Shader *shader = (Shader*)handle;
    if (shader->uniforms.find(location) != shader->uniforms.end()) 
    {
        return shader->uniforms[location];
    }

    GLint result = glGetUniformLocation(shader->program, location.c_str());
    shader->uniforms[location] = result;

    if (result == -1)
    {
        std::println("Failed to find uniform: {}", location);
        return false;
    }

    return true;

}

GLint
pde_renderer_get_uniform_location(shader_t handle, std::string location)
{

    Shader *shader = (Shader*)handle;
    GLint result = glGetUniformLocation(shader->program, location.c_str());
    return result;

}

void 
pde_renderer_set_uniform(shader_t handle, std::string uniform, int32_t value)
{

    //if (!pde_renderer_uniform_location_exists(handle, uniform)) return;

    Shader *shader = (Shader*)handle;
    GLint location = pde_renderer_get_uniform_location(handle, uniform);
    glUniform1iv(location, 1, &value);
    
}

void 
pde_renderer_set_uniform(shader_t handle, std::string uniform, uint32_t value)
{

    //if (!pde_renderer_uniform_location_exists(handle, uniform)) return;

    Shader *shader = (Shader*)handle;
    GLint location = pde_renderer_get_uniform_location(handle, uniform);
    glUniform1uiv(location, 1, &value);

}

void 
pde_renderer_set_uniform(shader_t handle, std::string uniform, float value)
{

    //if (!pde_renderer_uniform_location_exists(handle, uniform)) return;

    Shader *shader = (Shader*)handle;
    GLint location = pde_renderer_get_uniform_location(handle, uniform);
    glUniform1fv(location, 1, &value);

}

void 
pde_renderer_set_uniform(shader_t handle, std::string uniform, glm::vec2 value)
{

    //if (!pde_renderer_uniform_location_exists(handle, uniform)) return;

    Shader *shader = (Shader*)handle;
    GLint location = pde_renderer_get_uniform_location(handle, uniform);
    glUniform2fv(location, 1, glm::value_ptr(value));

}

void 
pde_renderer_set_uniform(shader_t handle, std::string uniform, glm::vec3 value)
{

    //if (!pde_renderer_uniform_location_exists(handle, uniform)) return;

    Shader *shader = (Shader*)handle;
    GLint location = pde_renderer_get_uniform_location(handle, uniform);
    glUniform3fv(location, 1, glm::value_ptr(value));

}

void 
pde_renderer_set_uniform(shader_t handle, std::string uniform, glm::vec4 value)
{

    //if (!pde_renderer_uniform_location_exists(handle, uniform)) return;

    Shader *shader = (Shader*)handle;
    GLint location = pde_renderer_get_uniform_location(handle, uniform);
    glUniform4fv(location, 1, glm::value_ptr(value));

}

void 
pde_renderer_set_uniform(shader_t handle, std::string uniform, glm::mat4 value)
{

    //if (!pde_renderer_uniform_location_exists(handle, uniform)) return;

    Shader *shader = (Shader*)handle;
    GLint location = pde_renderer_get_uniform_location(handle, uniform);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));

}

const char * 
get_screen_vertex_shader()
{
    return screen_vertex_shader;
}

const char * 
get_screen_fragment_shader()
{
    return screen_fragment_shader;
}

