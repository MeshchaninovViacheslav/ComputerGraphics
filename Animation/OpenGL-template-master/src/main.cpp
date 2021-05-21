#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "renderer.h"

bool running = true;

void process_input(SDL_Window *window, SDL_Event &event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            running = false;
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    // Специальное значение SDL_WINDOWPOS_CENTERED для x и y заставит SDL2
    // разместить окно в центре монитора по осям x и y.
    // Здесь для примера используется 0, т.е. окно появится в левом верхнем углу экрана.
    // Для использования OpenGL вы ДОЛЖНЫ указать флаг SDL_WINDOW_OPENGL.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_Window *window = SDL_CreateWindow(
            "SDL2/OpenGL Demo", 0, 0, 640, 480, SDL_WINDOW_OPENGL);

    // Создаём контекст OpenGL, связанный с окном.
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    glewInit();
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    Renderer renderer{};
    renderer.Init(window, width, height);
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                process_input(window, event);
            }
        }
        // Заливка кадра чёрным цветом средствами OpenGL
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        // Обновление и рисование сцены
        renderer.Render();
        // В конце - вывод нарисованного кадра в окно на экране
        SDL_GL_SwapWindow(window);
    }
    renderer.Close();
    SDL_GL_DeleteContext(glcontext);
    return 0;
}