#version 330 core

struct PointLight {
    vec2 position;
    float height;
    vec3 color;
};

in vec2 v_UV;

out vec4 FragColor;

uniform sampler2D u_diffuse_texture;
uniform PointLight[MAX_POINT_LIGHT_COUNT] point_lights;

vec3 process_point_light(PointLight point_light);

void main()
{
    // Point lights
    vec3 light_value = vec3(0.0);
    for (int i = 0; i < MAX_POINT_LIGHT_COUNT; i++) {
        light_value += process_point_light(point_lights[i])
    }

    // Final
    FragColor = texture(u_diffuse_texture, v_UV) * vec4(light_value);
}


vec3 process_point_light(PointLight point_light)
{
    return point_light.color;
}
