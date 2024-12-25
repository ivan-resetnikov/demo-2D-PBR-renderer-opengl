#include "typedefs.h"

#include <vector>

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
#include "lights.h"

#define MAX_POINT_LIGHT_COUNT 32

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
    GLuint diffuse_texture = Engine::load_texture("../assets/textures/brick_00/diffuse.jpg", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB, GL_RGB, texture_info);
    GLuint normal_texture = Engine::load_texture("../assets/textures/brick_00/normal.jpg", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB, GL_RGB);
    GLuint ao_texture = Engine::load_texture("../assets/textures/brick_00/ao.jpg", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RED, GL_RED);
    GLuint roughness_texture = Engine::load_texture("../assets/textures/brick_00/roughness.jpg", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RED, GL_RED);

    GLuint light_mask = Engine::load_texture("../assets/light_masks/flashlight.png", GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_RGB, GL_RGB);

    // Camera
    glm::mat4 projection_matrix = glm::ortho(0.0f, (float)g_context.screen_size_x, (float)g_context.screen_size_y, 0.0f, -128.0f, 128.0f);

    // Lights
    std::vector<PointLight> point_lights;

    // point_lights.push_back(PointLight{
    //     glm::vec3(1.0f, 0.0f, 0.0f),
    //     glm::vec2(0.0f, 0.0f),
    //     2.5f,
    //     400.0f,
    // });
    // point_lights.push_back(PointLight{
    //     glm::vec3(0.0f, 1.0f, 0.0f),
    //     glm::vec2(0.0f, 0.0f),
    //     2.0f,
    //     256.0f
    // });
    // point_lights.push_back(PointLight{
    //     glm::vec3(0.0f, 0.0f, 1.0f),
    //     glm::vec2(0.0f, 0.0f),
    //     3.0f,
    //     512.0f,
    //     64.0f,
    // });

    for (int i = 0; i < 16; i++)
    {
        point_lights.push_back(PointLight{
            (i % 2 == 0) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2((float)g_context.screen_size_x / 15.0f * (float)i, abs(sin((float)i / 3.141f * 2.0f) * 256.0f)),
            1.0f,
            512.0f
        });
    }

    for (int i = 0; i < 16; i++)
    {
        point_lights.push_back(PointLight{
            glm::vec3(1.0f, 0.6078f, 0.0f),
            glm::vec2((float)g_context.screen_size_x / 16.0f * (float)i, (float)g_context.screen_size_y),
            1.0f,
            512.0f,
            256.0f,
            {
                0.00175f * 0.2f,
                0.00005f * 0.2f
            }
        });
    }
    
    

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

        // point_lights[0].position = glm::vec2(
        //     960.0f + sin((float)SDL_GetTicks() * 0.001f) * 128.0f,
        //     256.0f + cos((float)SDL_GetTicks() * 0.001f) * 256.0f
        // );

        // point_lights[1].position = glm::vec2(
        //     540.0f + 256.0f + sin((float)SDL_GetTicks() * 0.0015f) * 512.0f,
        //     540.0f - 256.0f + cos((float)SDL_GetTicks() * 0.0015f) * 128.0f
        // );

        // point_lights[2].position = glm::vec2(
        //     960.0f + 128.0f + sin((float)SDL_GetTicks() * 0.002f) * 512.0f,
        //     540.0f + 256.0f + cos((float)SDL_GetTicks() * 0.002f) * 65.0f
        // );

        for (int i = 0; i < 16; i++)
        {
            point_lights[16 + i].energy = (
                (sin(((float)SDL_GetTicks() + (float)i * 120.0f) * 0.002f) + 1.25) * 0.1f +
                (sin(((float)SDL_GetTicks() + (float)i * 300.0f) * 0.0025f) + 1.25) * 0.05f +
                (sin(((float)SDL_GetTicks() + (float)i * 60.0f) * 0.002f) + 1.25) * 0.15f +
                (sin(((float)SDL_GetTicks() + (float)i * 400.0f) * 0.001f) + 1.25) * 0.12f
            );
        }
        

        // Quad
        glUseProgram(shader_program);

        // Uniforms: Matrices
        glm::mat4 model_matrix(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3((float)texture_info.width, (float)texture_info.height, 0.0f) * 4.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_model_matrix"), 1, GL_FALSE, &model_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_view_matrix"), 1, GL_FALSE, &view_matrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "u_projection_matrix"), 1, GL_FALSE, &projection_matrix[0][0]);

        // Uniforms: Textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_texture);
        glUniform1i(glGetUniformLocation(shader_program, "u_diffuse_texture"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normal_texture);
        glUniform1i(glGetUniformLocation(shader_program, "u_normal_texture"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ao_texture);
        glUniform1i(glGetUniformLocation(shader_program, "u_ao_texture"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, roughness_texture);
        glUniform1i(glGetUniformLocation(shader_program, "u_roughness_texture"), 3);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, light_mask);
        glUniform1i(glGetUniformLocation(shader_program, "u_light_mask"), 4);

        // Uniforms: Light: Ambient
        glUniform3fv(glGetUniformLocation(shader_program, "u_ambient_light"), 1, &glm::vec3(0.059f, 0.055f, 0.09f)[0]);

        // Uniforms: Light: Point lights
        int i = 0;
        for (PointLight& point_light : point_lights) {
            if (i > MAX_POINT_LIGHT_COUNT) {
                log_warning("Point light buffer size exceeded MAX_POINT_LIGHT_COUNT value (" + std::to_string(MAX_POINT_LIGHT_COUNT) + ")");
                break;
            }

            const std::string uniform_prefix = "u_point_lights[" + std::to_string(i) + "]";

            glUniform3fv(glGetUniformLocation(shader_program, (uniform_prefix + ".color").c_str()), 1, &point_light.color[0]);
            glUniform2fv(glGetUniformLocation(shader_program, (uniform_prefix + ".position").c_str()), 1, &point_light.position[0]);
            glUniform1f(glGetUniformLocation(shader_program, (uniform_prefix + ".energy").c_str()), point_light.energy);
            glUniform1f(glGetUniformLocation(shader_program, (uniform_prefix + ".height").c_str()), point_light.height);
            glUniform1f(glGetUniformLocation(shader_program, (uniform_prefix + ".attenuation_linear").c_str()), point_light.attenuation.linear);
            glUniform1f(glGetUniformLocation(shader_program, (uniform_prefix + ".attenuation_quadratic").c_str()), point_light.attenuation.quadratic);
            i++;
        }

        // Uniforms: Misc
        glUniform2fv(glGetUniformLocation(shader_program, "u_camera_pos"), 1, &glm::vec2(0.0f, 0.0f)[0]);
        glUniform2fv(glGetUniformLocation(shader_program, "u_viewport_size"), 1, &glm::vec2((float)g_context.screen_size_x, (float)g_context.screen_size_y)[0]);

        // Draw
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
        SDL_GL_SwapWindow(g_context.window);
        SDL_Delay(16);
    }
    log_info("Exiting main loop");

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);
    glDeleteTextures(1, &diffuse_texture);
    glDeleteTextures(1, &normal_texture);
    glDeleteTextures(1, &ao_texture);
    glDeleteTextures(1, &roughness_texture);
    glDeleteTextures(1, &light_mask);
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
