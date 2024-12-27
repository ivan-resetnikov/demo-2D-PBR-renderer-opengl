#version 330 core

#define MAX_POINT_LIGHT_COUNT 32

struct PointLight {
    vec3 color;
    vec2 position;
    float energy;
    float height;

    float attenuation_linear;
    float attenuation_quadratic;
};

in vec2 v_UV;
in vec2 v_frag_pos;

out vec4 FragColor;

uniform sampler2D u_diffuse_texture;
uniform sampler2D u_normal_texture;
uniform sampler2D u_ao_texture;
uniform sampler2D u_roughness_texture;
uniform sampler2D u_light_mask;

uniform vec3 u_ambient_light;
uniform PointLight[MAX_POINT_LIGHT_COUNT] u_point_lights;

uniform vec2 u_camera_pos;
uniform vec2 u_viewport_size;

vec3 process_point_light(PointLight point_light, vec2 frag_normal, float ao_value);

void main()
{
    vec2 normal_value = texture(u_normal_texture, fract(v_UV * 4.0)).xy * 2.0 - 1.0;
    float ao_value = texture(u_ao_texture, fract(v_UV * 4.0)).r;

    // Point lights
    vec3 light_value = u_ambient_light;
    for (int i = 0; i < MAX_POINT_LIGHT_COUNT; i++) {
        light_value += process_point_light(u_point_lights[i], normal_value, ao_value);
    }

    // Final
    FragColor = texture(u_diffuse_texture, fract(v_UV * 4.0)) * vec4(light_value, 1.0);
}


vec3 process_point_light(PointLight point_light, vec2 frag_normal, float ao_value)
{
    // Attenuation
    float distance = length(v_frag_pos - point_light.position);
    // https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    float attenuation = 1.0 / (1.0 + point_light.attenuation_linear * distance + point_light.attenuation_quadratic * (distance * distance));

    // Normal map
    vec3 normal = vec3(frag_normal.xy, 1.0);
    vec3 light_dir = normalize(vec3(point_light.position, point_light.height) - vec3(v_frag_pos, 0.0));
    float normal_difference = max(dot(normal, light_dir), 0.0);

    // Light mask
    // vec2 mask_UV = (v_frag_pos - point_light.position + 512.0) / 512.0 * 0.5;
    // if (mask_UV.x < 0.0 || mask_UV.x > 1.0 || mask_UV.y < 0.0 || mask_UV.y > 1.0) return vec3(0.0);
    // vec3 mask_value = texture(u_light_mask, mask_UV).rgb;
    vec3 mask_value = vec3(1.0);

    vec3 view_dir = normalize(vec3(u_camera_pos.x + u_viewport_size.x * 0.5, u_camera_pos.y + u_viewport_size.y * 0.5, 128.0) - vec3(v_frag_pos, 0.0));
    vec3 reflecttion_dir = reflect(-light_dir, normal);
    float specular_factor = max(dot(view_dir, reflecttion_dir), 0.0);  // No `pow()` yet
    vec3 specular_value = specular_factor * point_light.color;

    return (point_light.color + specular_value) * point_light.energy * ao_value * normal_difference * mask_value * attenuation;

    // return vec3(mask_UV, 0.0);
}
