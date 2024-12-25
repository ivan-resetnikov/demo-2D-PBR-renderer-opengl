#include <cstdint>

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "logging.h"

typedef unsigned int uintM_t;

namespace Engine
{
    struct Context {
        uintM_t screen_size_x = -1;  // Start unresolved
        uintM_t screen_size_y = -1;
        SDL_Window* window;
        SDL_GLContext gl_context;
    } g_context;

    inline void initContext();
    inline void mainLoop();
    inline void terminateContext();
}

inline void Engine::initContext()
{
    logInfo("Creating engine context");

    // Initializing SDL
    logDebug("Initializing SDL2");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        logCritical("Failed to initialize SDL\n SDL Error: " + (std::string)SDL_GetError());
        exit(1);
    }

    // Creating Window
    SDL_DisplayMode main_screen_mode;
    SDL_GetCurrentDisplayMode(0, &main_screen_mode);  // Assume i=0
    g_context.screen_size_x = (uintM_t)main_screen_mode.w;
    g_context.screen_size_y = (uintM_t)main_screen_mode.h;

    logDebug("Creating window");
    g_context.window = SDL_CreateWindow(
        "Engine", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        g_context.screen_size_x, g_context.screen_size_y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
    if (!g_context.window) {
        logCritical("Failed to create SDL window\n SDL Error: " + (std::string)SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_SetWindowResizable(g_context.window, SDL_FALSE);
    SDL_SetWindowIcon(g_context.window, SDL_LoadBMP("icon.bmp"));

    // OpenGL context
    logDebug("Creating OpenGL context");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    g_context.gl_context = SDL_GL_CreateContext(g_context.window);
    if (!g_context.gl_context) {
        logCritical("Failed to create SDL OpenGL context\n SDL Error: " + (std::string)SDL_GetError());
        SDL_DestroyWindow(g_context.window);
        SDL_Quit();
        exit(1);
    }

    logDebug("Calling OpenGL loader (GLAD)");
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        logCritical("Failed to initialize GLAD OpenGL loader!");
        SDL_GL_DeleteContext(g_context.gl_context);
        SDL_DestroyWindow(g_context.window);
        SDL_Quit();
        exit(1);
    }
}

inline void Engine::mainLoop()
{
    logInfo("Entering main loop");
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    
        SDL_GL_SwapWindow(g_context.window);
    }
    logInfo("Exiting main loop");
}

inline void Engine::terminateContext()
{
    logInfo("Terminating engine context");
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