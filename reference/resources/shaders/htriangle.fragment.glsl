#version 460 core
layout (location = 0) in vec2 texture_coordinates;
layout (location = 1) in vec3 fragment_coordinates;
layout (location = 2) in vec3 normal_coordinates;

layout (location = 0) out vec4 fragment;

uniform vec4 u_overlay;
uniform vec3 u_camera;
uniform float u_exposure = 1.0;
uniform float u_gamma = 2.2;

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float shine;
};

uniform Material u_material;

struct PointLight
{
    vec3 position;
    vec3 color;
};

uniform PointLight u_point_lights[16];
uniform int u_point_light_count;

vec4 point_light_diffuse()
{

    vec3 result = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < u_point_light_count; ++i)
    {

        vec3 light_color = u_point_lights[i].color;
        vec3 light_coordinates = u_point_lights[i].position;

        vec3 light_direction = normalize(light_coordinates - fragment_coordinates);          
        vec3 diffuse = (max(dot(normal_coordinates, light_direction), 0.0f)) * light_color;

        result += diffuse;

    }

    return vec4(result, 1.0f);
    //return vec4(0.0f);

}

vec4 point_light_specular()
{

    float specular_strength = 1.0f;

    vec3 result = { 0.0f, 0.0f, 0.0f };
    for (int i = 0; i < u_point_light_count; ++i)
    {

        vec3 light_color = u_point_lights[i].color;
        vec3 light_coordinates = u_point_lights[i].position;
        
        vec3 light_direction = normalize(light_coordinates - fragment_coordinates);          
        vec3 view_direction = normalize(u_camera - fragment_coordinates);
        vec3 reflection_direction = reflect(-light_direction, normal_coordinates);

        float spec = pow(max(dot(view_direction, reflection_direction), 0.0f), u_material.shine);
        vec3 specular = spec * light_color;

        result += specular;

    }

    return vec4(result, 1.0f) * specular_strength;
    //return vec4(0.0f);

}

vec4 tone_map(vec4 fragment_color)
{

    vec3 raw = fragment_color.rgb;
    vec3 mapped = pow((vec3(1.0f) - exp(-raw * u_exposure)), vec3(1.0f / u_gamma));
    return vec4(mapped, 1.0f);

}

void main()
{

    vec4 diffuse_color = texture(u_material.diffuse, texture_coordinates);
    vec4 specular_color = texture(u_material.specular, texture_coordinates);

    vec4 ambient = diffuse_color * 0.05f;
    vec4 diffuse = point_light_diffuse() * diffuse_color;
    vec4 specular = point_light_specular() * specular_color;

    vec4 fragment_color = (ambient + diffuse + specular);
    fragment = tone_map(specular + diffuse + ambient);
    //fragment = fragment_color;

}
