#version 460 core
layout (location = 0) in vec2 texture_coordinates;
layout (location = 1) in vec3 fragment_coordinates;
layout (location = 2) in vec3 normal_coordinates;

layout (location = 0) out vec4 fragment;

uniform vec3 u_camera;
uniform float u_exposure = 1.0;
uniform float u_gamma = 2.2;

uniform vec4 color_overlay;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normals1;
uniform int use_specular_texture = 0;
uniform int use_normal_texture = 0;

uniform float ambient_coefficient = 0.10f;
uniform float diffuse_coefficient = 0.20f;
uniform float specular_coefficient = 0.50f;
uniform float specular_strength = 64.0f;

struct PointLight
{
    vec3 position;
    vec3 color;
    float constant;
    float linear;
    float quadratic;
};

// Default direction light is really far away.
uniform vec3 direction_light_color = { 0.0f, 0.0f, 0.0f };
uniform vec3 direction_light_position = { 1000.0f, 10000.0f, -2000.0f };
uniform PointLight point_lights[16];
uniform int point_light_count = 0;

vec4 tone_map(vec4 fragment_color)
{

    vec3 raw = fragment_color.rgb;
    vec3 mapped = pow((vec3(1.0f) - exp(-raw * u_exposure)), vec3(1.0f / u_gamma));
    return vec4(mapped, 1.0f);

}

vec4 calculate_direction_light(vec3 normal, vec3 view_direction, vec4 specular_sample)
{

    vec3 light_direction = normalize(direction_light_position - fragment_coordinates);
    vec3 reflection_direction = reflect(-light_direction, normal);
    vec3 diffuse_texture = vec3(texture(texture_diffuse1, texture_coordinates));

    vec3 ambient_color = diffuse_texture.rgb * ambient_coefficient;

    vec3 diffuse_color = max(dot(normal, light_direction), 0.0f) 
            * diffuse_texture.rgb 
            * direction_light_color
            * diffuse_coefficient;

    vec3 specular_color = pow(max(dot(view_direction, reflection_direction), 0.0f), specular_strength)
            * specular_coefficient
            * direction_light_color
            * specular_sample.rgb 
            * specular_sample.a;

    return vec4(ambient_color + diffuse_color + specular_color, 1.0f);

}

vec4 calculate_point_light(PointLight light, vec3 normal, vec3 view_direction, vec4 specular_sample)
{

    vec3 light_direction = normalize(light.position - fragment_coordinates);
    vec3 reflection_direction = reflect(-light_direction, normal);

    vec3 diffuse_texture = vec3(texture(texture_diffuse1, texture_coordinates));

    vec3 ambient_color = diffuse_texture.rgb * ambient_coefficient;

    vec3 diffuse_color = max(dot(normal, light_direction), 0.0f) 
            * diffuse_texture.rgb 
            * light.color
            * diffuse_coefficient;

    vec3 specular_color = pow(max(dot(view_direction, reflection_direction), 0.0f), specular_strength)
            * specular_coefficient
            * light.color
            * specular_sample.rgb 
            * specular_sample.a;

    float distance = length(light.position - fragment_coordinates);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient_color *= attenuation;
    diffuse_color *= attenuation;
    specular_color *= attenuation;

    return vec4(ambient_color + diffuse_color + specular_color, 1.0f);

}

void main()
{

    vec3 normal = normal_coordinates;
    vec4 specular_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    if (use_normal_texture == 1)
    {
        vec3(texture(texture_normals1, texture_coordinates)) * 2 - 1.0f;
    }

    if (use_specular_texture == 1) 
    {
        specular_color = texture(texture_specular1, texture_coordinates);
    }

    vec3 view_direction = normalize(u_camera - fragment_coordinates);
    vec4 direction_light = calculate_direction_light(normal, view_direction, specular_color);

    vec4 point_light = vec4(0.0f);
    for (int i = 0; i < point_light_count; ++i) 
    {
        point_light += calculate_point_light(point_lights[i], normal, view_direction, specular_color);
    }

    vec4 fragment_color = direction_light + point_light;

    fragment = vec4(mix(tone_map(fragment_color).rgb, color_overlay.rgb, color_overlay.a), 1.0f);

}

