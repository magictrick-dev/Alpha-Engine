#version 460 core
layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_texture;
layout (location = 0) out vec2 texture_coordinates;

void main()
{

    texture_coordinates = v_texture;
    gl_Position = vec4(v_position, 0.0f, 1.0f);

}
