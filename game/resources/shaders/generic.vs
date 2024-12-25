#version 330 core

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_UV;

out vec2 v_UV;
out vec2 v_frag_pos;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;

void main()
{
    v_UV = a_UV;
    
    v_frag_pos = (u_view_matrix * u_model_matrix * vec4(a_pos.x, a_pos.y, 0.0, 1.0)).xy;

    gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(a_pos.x, a_pos.y, 0.0, 1.0);
}
