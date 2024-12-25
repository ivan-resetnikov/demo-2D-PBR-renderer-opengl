#include "shader_utils.h"

namespace Engine
{
    GLuint create_generic_shader(const char* vertex_shader_source, const char* fragment_shader_source)
    {
        if (strlen(vertex_shader_source) == 0) log_warning("[SHADER] Vertex shader source is empty!");
        if (strlen(vertex_shader_source) == 0) log_warning("[SHADER] Fragment shader source is empty!");

        int success;
        char info_log[512];
        
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
        glCompileShader(vertex_shader);
        // Check for errors
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
            log_error("[SHADER] Failed to compile the vertex shader!\n" + (std::string)info_log);
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragment_shader_source, NULL);
        glCompileShader(fragmentShader);
        // Check for errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, info_log);
            log_error("[SHADER] Failed to compile the vertex shader!\n" + (std::string)info_log);
        }

        GLuint shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragmentShader);
        glLinkProgram(shader_program);
        // Check for errors
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_program, 512, NULL, info_log);
            log_error("[SHADER] Failed to link the vertex and fragment shaders!\n" + (std::string)info_log);
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragmentShader);

        return shader_program;
    }
}