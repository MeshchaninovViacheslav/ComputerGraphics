#include "common.h"
#include "Image.h"
#include "Player.h"
#include "Level.h"

#include <unistd.h>
#include <chrono>
#include <thread>

#define GLFW_DLL

#include <GLFW/glfw3.h>


constexpr int numTilesInScreen = 40, numTilesInBackground = 50;
constexpr GLsizei WINDOW_WIDTH = numTilesInScreen * 16, WINDOW_HEIGHT = numTilesInScreen * 16;

struct InputState {
    bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
    GLfloat lastX = 400, lastY = 300; //исходное положение мыши
    bool firstMouse = true;
    bool captureMouse = true;  // Мышка захвачена нашим приложением или нет?
    bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow *window, int key, int scancode, int action, int mode) {
    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_1:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case GLFW_KEY_2:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        default:
            if (action == GLFW_PRESS)
                Input.keys[key] = true;
            else if (action == GLFW_RELEASE)
                Input.keys[key] = false;
    }
}

void processPlayerMovement(Player &player) {
    if (Input.keys[GLFW_KEY_W])
        player.ProcessInput(MovementDir::UP);
    else if (Input.keys[GLFW_KEY_S])
        player.ProcessInput(MovementDir::DOWN);
    if (Input.keys[GLFW_KEY_A])
        player.ProcessInput(MovementDir::LEFT);
    else if (Input.keys[GLFW_KEY_D])
        player.ProcessInput(MovementDir::RIGHT);
}

void processPlayerMovement(Level &level) {

    if (Input.keys[GLFW_KEY_W])
        level.ProcessPlayerMovement(MovementDir::UP);
    else if (Input.keys[GLFW_KEY_S])
        level.ProcessPlayerMovement(MovementDir::DOWN);
    else if (Input.keys[GLFW_KEY_A])
        level.ProcessPlayerMovement(MovementDir::LEFT);
    else if (Input.keys[GLFW_KEY_D])
        level.ProcessPlayerMovement(MovementDir::RIGHT);

    static bool hold = false;
    if (Input.keys[GLFW_KEY_E]) {
        if (!hold) {
            level.SwitchDoor();
        }
        hold = true;
    } else {
        hold = false;
    }
}

void OnMouseButtonClicked(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        Input.captureMouse = !Input.captureMouse;

    if (Input.captureMouse) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Input.capturedMouseJustNow = true;
    } else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow *window, double xpos, double ypos) {
    if (Input.firstMouse) {
        Input.lastX = float(xpos);
        Input.lastY = float(ypos);
        Input.firstMouse = false;
    }

    GLfloat xoffset = float(xpos) - Input.lastX;
    GLfloat yoffset = Input.lastY - float(ypos);

    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow *window, double xoffset, double yoffset) {
    // ...
}


int initGL() {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::cout << "Controls: " << std::endl;
    std::cout << "press right mouse button to capture/release mouse cursor  " << std::endl;
    std::cout << "W, A, S, D - movement  " << std::endl;
    std::cout << "press ESC to exit" << std::endl;

    return 0;
}

void FadeIn(GLFWwindow *window, Level *level) {
    for (double alpha = 0; alpha <= 1; alpha += 0.01) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK_ERRORS;
        glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE,  level->FadeIn(alpha).Data());
        GL_CHECK_ERRORS;

        glfwSwapBuffers(window);
    }
}

void FadeOut(GLFWwindow *window, Level *level) {
    for (double alpha = 1; alpha >= 0; alpha -= 0.01) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK_ERRORS;
        glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE,  level->FadeIn(alpha).Data());
        GL_CHECK_ERRORS;

        glfwSwapBuffers(window);
    }
}

int main(int argc, char **argv) {

    if (!glfwInit())
        return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, OnKeyboardPressed);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
    glfwSetScrollCallback(window, OnMouseScroll);

    if (initGL() != 0)
        return -1;

    //Reset any OpenGL errors which could be present for some reason
    GLenum gl_error = glGetError();
    while (gl_error != GL_NO_ERROR)
        gl_error = glGetError();

    // Create game map
    std::vector<std::string> levelsName{"../tests/1.txt", "../tests/2.txt", "../tests/3.txt"};
    int numLevel = 0;
    Level *level = new Level(levelsName[numLevel++], numTilesInBackground, numTilesInBackground, numTilesInScreen);
    FadeIn(window, level);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    GL_CHECK_ERRORS;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GL_CHECK_ERRORS;


    //game loop

    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        processPlayerMovement(*level);
        level->Draw();


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK_ERRORS;
        glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, level->Data());
        GL_CHECK_ERRORS;

        glfwSwapBuffers(window);

        if (level->PlayerIsDead()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            break;
        }

        if (level->PlayerIsWin() && Input.keys[GLFW_KEY_ENTER]) {
            FadeOut(window, level);
            delete level;
            if (numLevel == levelsName.size()) {
                break;
            }
            level = new Level(levelsName[numLevel++], numTilesInBackground, numTilesInBackground, numTilesInScreen);
            if (numLevel == levelsName.size()) {
                level->MakeLast();
            }
            FadeIn(window, level);
        }
    }
    glfwTerminate();

    return 0;

}
