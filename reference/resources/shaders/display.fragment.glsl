#version 460 core
layout (location = 0) in vec2 texture_coordinates;
layout (location = 0) out vec4 fragment;

uniform sampler2D u_texture;
uniform float u_exposure = 1.0f;
uniform float u_gamma = 2.2f;

void main()
{

    vec3 raw_fragment = texture(u_texture, texture_coordinates).rgb;
    vec3 mapped = vec3(1.0f) - exp(-raw_fragment * u_exposure);
    mapped = pow(mapped, vec3(1.0f / gamma);

    fragment = vec4(mapped, 1.0f);

}
