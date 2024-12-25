#include "typedefs.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "logging.h"
#include "shader_utils.h"
#include "file_utils.h"
#include "texture_utils.h"

namespace Engine
{
    struct Context {
        uintmax_t screen_size_x = -1;  // Start unresolved
        uintmax_t screen_size_y = -1;
        SDL_Window* window;
        SDL_GLContext gl_context;
    } g_context;

    inline void initContext();
    inline void mainLoop();
    inline void terminateContext();
}

inline void Engine::initContext()
{
    log_info("Creating engine context");

    // Initializing SDL
    log_debug("Initializing SDL2");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        log_critical("Failed to initialize SDL\n SDL Error: " + (std::string)SDL_GetError());
        exit(1);
    }

    // Creating Window
    SDL_DisplayMode main_screen_mode;
    SDL_GetCurrentDisplayMode(0, &main_screen_mode);  // Assume i=0
    g_context.screen_size_x = (uintmax_t)main_screen_mode.w;
    g_context.screen_size_y = (uintmax_t)main_screen_mode.h;

    log_debug("Creating window");
    g_context.window = SDL_CreateWindow(
        "Engine", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        (int)g_context.screen_size_x, (int)g_context.screen_size_y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    if (!g_context.window) {
        log_critical("Failed to create SDL window\n SDL Error: " + (std::string)SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_SetWindowResizable(g_context.window, SDL_FALSE);
    SDL_SetWindowIcon(g_context.window, SDL_LoadBMP("icon.bmp"));

    // OpenGL context
    log_debug("Creating OpenGL context");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    g_context.gl_context = SDL_GL_CreateContext(g_context.window);
    if (!g_context.gl_context) {
        log_critical("Failed to create SDL OpenGL context\n SDL Error: " + (std::string)SDL_GetError());
        SDL_DestroyWindow(g_context.window);
        SDL_Quit();
        exit(1);
    }

    log_debug("Calling OpenGL loader (GLAD)");
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        log_critical("Failed to initialize GLAD OpenGL loader!");
        SDL_GL_DeleteContext(g_context.gl_context);
        SDL_DestroyWindow(g_context.window);
        SDL_Quit();
        exit(1);
    }
}

inline void Engine::mainLoop()
{
    // Mesh
    GLsizei vertex_size = (2 + 2) * sizeof(float);
    float vertices[] = {
        1.0f, 0.0f,  1.0f, 0.0f,  // top right
        1.0f, 1.0f,  1.0f, 1.0f,  // bottom right
        0.0f, 1.0f,  0.0f, 1.0f,  // bottom left
        0.0f, 0.0f,  0.0f, 0.0f,  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Shader
    GLuint shader_program = create_generic_shader(
        Engine::read_text_file("../resources/shaders/generic.vs").c_str(),
        Engine::read_text_file("../resources/shaders/generic.fs").c_str());

    // Texture
    Engine::TextureInfo texture_info;
    GLuint texture = Engine::load_texture("../assets/textures/brick_00/diffuse.jpg", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB, GL_RGB, texture_info);

    glm::mat4 projection_matrix = glm::ortho(0.0f, (float)g_context.screen_size_x, (float)g_context.screen_size_y, 0.0f, -128.0f, 128.0f);

    log_info("Entering main loop");
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 view_matrix(1.0f);

        // Quad
        glUseProgram(shader_program);

        // Uniforms: Matrices
        glm::mat4 model_matrix(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(256.0f, 256.0f, 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3((float)texture_info.width, (float)texture_info.height, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_model_matrix"), 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_view_matrix"), 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_projection_matrix"), 1, GL_FALSE, &projection_matrix[0][0]);

        // Uniforms: Textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Uniforms: Point lights
        glUniform3fv(glGetUniformLocation(shader_program, "u_ambient_light"), 1, &glm::vec3(0.059f, 0.055f, 0.09f)[0]);

        glUniform1f(glGetUniformLocation(shader_program, "u_point_lights[0].height"), 0.0f);
        glUniform2fv(glGetUniformLocation(shader_program, "u_point_lights[0].position"), 1, &glm::vec2(512.0f, 512.0f)[0]);
        glUniform3fv(glGetUniformLocation(shader_program, "u_point_lights[0].color"), 1, &glm::vec3(1.0f, 0.0f, 0.0f)[0]);
        glUniform1f(glGetUniformLocation(shader_program, "u_point_lights[0].radius"), 256.0f);

        glUniform1f(glGetUniformLocation(shader_program, "u_point_lights[1].height"), 0.0f);
        glUniform2fv(glGetUniformLocation(shader_program, "u_point_lights[1].position"), 1, &glm::vec2(512.0f + 256.0f, 512.0f - 256.0f)[0]);
        glUniform3fv(glGetUniformLocation(shader_program, "u_point_lights[1].color"), 1, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);
        glUniform1f(glGetUniformLocation(shader_program, "u_point_lights[1].radius"), 256.0f);

        glUniform1f(glGetUniformLocation(shader_program, "u_point_lights[2].height"), 0.0f);
        glUniform2fv(glGetUniformLocation(shader_program, "u_point_lights[2].position"), 1, &glm::vec2(512.0f + 256.0f, 512.0f + 256.0f)[0]);
        glUniform3fv(glGetUniformLocation(shader_program, "u_point_lights[2].color"), 1, &glm::vec3(0.0f, 0.0f, 1.0f)[0]);
        glUniform1f(glGetUniformLocation(shader_program, "u_point_lights[2].radius"), 256.0f);

        // Draw
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
        SDL_GL_SwapWindow(g_context.window);
    }
    log_info("Exiting main loop");

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);
    glDeleteTextures(1, &texture);
}

inline void Engine::terminateContext()
{
    log_info("Terminating engine context");
    SDL_GL_DeleteContext(g_context.gl_context);
    SDL_DestroyWindow(g_context.window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    Engine::initContext();
    Engine::mainLoop();
    Engine::terminateContext();
}
