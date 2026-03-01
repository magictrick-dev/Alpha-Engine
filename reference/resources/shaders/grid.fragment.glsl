#version 460 core
layout (location = 0) in vec2 texture_coordinates;
layout (location = 1) in vec3 fragment_coordinates; // world position
layout (location = 2) in vec3 normal_coordinates;

layout (location = 0) out vec4 fragment;

uniform vec3 u_camera; // camera world position
uniform float u_gridSpacing   = 1.0;   // spacing between minor lines
uniform float u_thickSpacing  = 2.0;   // spacing for major lines
uniform float u_lineWidth     = 1.0;   // base thickness of lines
uniform float u_fadeDistance  = 100.0; // distance at which grid mostly fades
uniform float u_fadeExponent  = 2.0;   // higher = faster falloff

uniform vec3 u_gridColor      = vec3(0.6, 0.6, 0.6);  // minor grid
uniform vec3 u_majorColor     = vec3(1.0, 1.0, 1.0);  // major grid
uniform vec3 u_axisXColor     = vec3(1.0, 0.2, 0.2);  // X axis
uniform vec3 u_axisZColor     = vec3(0.2, 0.2, 1.0);  // Z axis

// Anti-aliased grid function using screen-space derivatives
float gridLineAA(vec2 coord, float spacing, float lineWidth)
{
    // distance to nearest line along X and Z
    vec2 modCoord = mod(coord, spacing) - spacing * 0.5;
    vec2 d = abs(modCoord);

    // approximate derivatives in screen space for smooth AA
    vec2 f = fwidth(coord);
    float dist = min(d.x / f.x, d.y / f.y);

    // smoothstep for anti-aliasing
    return 1.0 - smoothstep(0.0, lineWidth, dist);
}

void main()
{
    vec2 gridCoord = fragment_coordinates.xz;
    vec4 result = vec4(0.0); // fully transparent by default

    // Exponential fade based on distance
    float distance = length(fragment_coordinates - u_camera);
    float fade = pow(clamp(1.0 - (distance / u_fadeDistance), 0.0, 1.0), u_fadeExponent);

    // Minor lines
    float minor = gridLineAA(gridCoord, u_gridSpacing, u_lineWidth) * fade;
    if (minor > 0.0)
        result = vec4(u_gridColor, minor);

    // Major lines (stronger, override minor)
    float major = gridLineAA(gridCoord, u_thickSpacing, u_lineWidth) * fade;
    if (major > 0.0)
        result = vec4(u_majorColor, major);

    // Axis highlights (fully visible, ignore fade)
    if (abs(fragment_coordinates.x) < u_lineWidth / 100.0f) result = vec4(u_axisZColor, 1.0);
    if (abs(fragment_coordinates.z) < u_lineWidth / 100.0f) result = vec4(u_axisXColor, 1.0);

    fragment = result;

}
