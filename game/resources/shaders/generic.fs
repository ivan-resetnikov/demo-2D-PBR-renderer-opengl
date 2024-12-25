#version 330 core

#define MAX_POINT_LIGHT_COUNT 16

struct PointLight {
    vec3 color;

    vec2 position;
    float height;

    float radius;
};

in vec2 v_UV;
in vec2 v_frag_pos;

out vec4 FragColor;

uniform sampler2D u_diffuse_texture;
uniform vec3 u_ambient_light;
uniform PointLight[MAX_POINT_LIGHT_COUNT] u_point_lights;

vec3 process_point_light(PointLight point_light);

void main()
{
    // Point lights
    vec3 light_value = u_ambient_light;
    for (int i = 0; i < MAX_POINT_LIGHT_COUNT; i++) {
        light_value += process_point_light(u_point_lights[i]);
    }

    // Final
    FragColor = texture(u_diffuse_texture, v_UV) * vec4(light_value, 1.0);
}


vec3 process_point_light(PointLight point_light)
{
    float distance = length(v_frag_pos - point_light.position);

    float radius = point_light.radius;
    float attenuation = 1.0 - (distance / radius);
    attenuation = clamp(attenuation, 0.0, 1.0);

    return point_light.color * attenuation;
}
