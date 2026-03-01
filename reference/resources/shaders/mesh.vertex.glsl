#version 460 core
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texture;

layout (location = 0) out vec2 texture_coordinates;
layout (location = 1) out vec3 fragment_coordinates;
layout (location = 2) out vec3 normal_coordinates;

uniform mat4 u_model;
uniform mat4 u_model_inverse;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{

    texture_coordinates = v_texture;
    fragment_coordinates = vec3(u_model * vec4(v_position, 1.0f));
    normal_coordinates = normalize(vec3(u_model_inverse * vec4(v_normal, 1.0f)));
    gl_Position = u_projection * u_view * u_model * vec4(v_position, 1.0f);

}
