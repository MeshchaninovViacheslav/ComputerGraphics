#include "renderer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include <chrono>

int add_shader(std::string fileName, GLuint shaderProgram, GLenum shaderType) {
    char *src;
    int32_t size;

    //Читаем файл с кодом шейдера
    std::ifstream t;
    std::string fileContent;

    t.open(fileName);
    if (t.is_open()) {
        std::stringstream buffer;
        buffer << t.rdbuf();
        fileContent = buffer.str();
    } else std::cout << "File " << fileName << " opening failed" << std::endl;
    t.close();
    size = fileContent.length();  //Set the Size

    std::string result = fileContent;
    src = const_cast<char *>(result.c_str());

    int shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &src, &size);

    //компилируем шейдер
    glCompileShader(shaderID);
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (success) {
        glAttachShader(shaderProgram, shaderID);
    } else {
        //печатаем информацию об ошибках компиляции
        int m;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &m);
        char *l = new char[m];
        if (t) glGetShaderInfoLog(shaderID, m, &m, l);
        std::cout << "Compiler Error: " << l << std::endl;
        delete[] l;
    }
    return shaderID;
}

void add_shader_program(GLuint &shaderProgram, const std::string vs, const std::string fs) {
    //компилируем шейдеры и связываем их с программой
    shaderProgram = glCreateProgram();
    int vs_code = add_shader(vs, shaderProgram, GL_VERTEX_SHADER);
    int fs_code = add_shader(fs, shaderProgram, GL_FRAGMENT_SHADER);
    glLinkProgram(shaderProgram);

    //печатаем ошибки линковки
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        int m;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &m);
        char *l = new char[m];
        glGetProgramInfoLog(shaderProgram, m, &m, l);
        std::cout << "Linker Error: " << l << std::endl;
        delete[] l;
    }
    glDeleteShader(vs_code);
    glDeleteShader(fs_code);
}

void Renderer::Init(SDL_Window *_window, int w, int h) {
    window = _window;
    width = w;
    height = h;

    GLfloat vertices[5 * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH];
    GLuint indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1)];
    // Initialize texture flag
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        start_time = Clock::now();

        int texture_width, texture_height;
        unsigned char *data_texture = stbi_load("bochkarev.jpg", &texture_width, &texture_height,
                                                nullptr, 0);
        if (data_texture) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data_texture);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data_texture);
        } else {
            throw std::runtime_error("Failed to load texture");
        }


        for (int i = 0; i < NUMBERS_POINT_HEIGHT_MESH; ++i) {
            float step_h = 1. / (NUMBERS_POINT_HEIGHT_MESH - 1);
            for (int j = 0; j < NUMBERS_POINT_WIDTH_MESH; ++j) {
                float step_w = 1. / (NUMBERS_POINT_WIDTH_MESH - 1);
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * 5 + 1] = -0.5 + step_h * i;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * 5 + 0] = -0.5 + step_w * j;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * 5 + 2] = 0.;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * 5 + 4] = 0. + step_h * i;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * 5 + 3] = 0. + step_w * j;
            }
        }
        /*
        for (int i = 0; i < 5 * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH; i += 5) {
            for (int j = 0; j < 5; ++j) {
                std::cout << vertices[i + j] << " ";
            }
            std::cout << std::endl;
        }*/
        /*
        GLfloat vertices[] = {
                0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // Верхний правый угол
                0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Нижний правый угол
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Нижний левый угол
                -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // Верхний левый угол
        };
         */



        for (int i = 0; i < NUMBERS_POINT_HEIGHT_MESH - 1; ++i) {
            for (int j = 0; j < NUMBERS_POINT_WIDTH_MESH - 1; ++j) {

                indices[(i * (NUMBERS_POINT_WIDTH_MESH - 1) + j) * 6 + 0] = i * NUMBERS_POINT_WIDTH_MESH + j;
                indices[(i * (NUMBERS_POINT_WIDTH_MESH - 1) + j) * 6 + 1] = i * NUMBERS_POINT_WIDTH_MESH + j + 1;
                indices[(i * (NUMBERS_POINT_WIDTH_MESH - 1) + j) * 6 + 2] = (i + 1) * NUMBERS_POINT_WIDTH_MESH + j;
                indices[(i * (NUMBERS_POINT_WIDTH_MESH - 1) + j) * 6 + 3] = i * NUMBERS_POINT_WIDTH_MESH + j + 1;
                indices[(i * (NUMBERS_POINT_WIDTH_MESH - 1) + j) * 6 + 4] = (i + 1) * NUMBERS_POINT_WIDTH_MESH + j;
                indices[(i * (NUMBERS_POINT_WIDTH_MESH - 1) + j) * 6 + 5] = (i + 1) * NUMBERS_POINT_WIDTH_MESH + j + 1;
            }
        }
        /*
        for (auto x: indices) {
            std::cout << x << std::endl;
        }*/

        /*
        GLuint indices[] = {
                0, 1, 2,   // Первый треугольник
                1, 2, 3    // Второй треугольник
        };
        */
    }

    int numbers_point_width_mesh = 2;
    GLfloat vertices_flagpole[5 * NUMBERS_POINT_HEIGHT_MESH * numbers_point_width_mesh];
    GLuint indices_flagpole[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (numbers_point_width_mesh - 1)];
    // Initialize texture flagpole
    {
        glGenTextures(2, &texture_pole);
        glBindTexture(GL_TEXTURE_2D, texture_pole);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        start_time = Clock::now();

        int texture_width, texture_height;
        unsigned char *data_texture = stbi_load("wooden_container.jpg", &texture_width, &texture_height,
                                                nullptr, 0);
        if (data_texture) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data_texture);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data_texture);
        } else {
            throw std::runtime_error("Failed to load texture_pole");
        }


        for (int i = 0; i < NUMBERS_POINT_HEIGHT_MESH; ++i) {
            float step_h = 1. / (NUMBERS_POINT_HEIGHT_MESH - 1);
            for (int j = 0; j < numbers_point_width_mesh; ++j) {
                float step_w = 1. / (numbers_point_width_mesh - 1);
                vertices_flagpole[(i * numbers_point_width_mesh + j) * 5 + 1] = -0.5 + step_h * i;
                vertices_flagpole[(i * numbers_point_width_mesh + j) * 5 + 0] = -0.5 + step_w * j;
                vertices_flagpole[(i * numbers_point_width_mesh + j) * 5 + 2] = 0.;
                vertices_flagpole[(i * numbers_point_width_mesh + j) * 5 + 4] = 0. + step_h * i;
                vertices_flagpole[(i * numbers_point_width_mesh + j) * 5 + 3] = 0. + step_w * j;
            }
        }

        for (int i = 0; i < NUMBERS_POINT_HEIGHT_MESH - 1; ++i) {
            for (int j = 0; j < numbers_point_width_mesh - 1; ++j) {
                indices_flagpole[(i * (numbers_point_width_mesh - 1) + j) * 6 + 0] = i * numbers_point_width_mesh + j;
                indices_flagpole[(i * (numbers_point_width_mesh - 1) + j) * 6 + 1] = i * numbers_point_width_mesh + j + 1;
                indices_flagpole[(i * (numbers_point_width_mesh - 1) + j) * 6 + 2] = (i + 1) * numbers_point_width_mesh + j;
                indices_flagpole[(i * (numbers_point_width_mesh - 1) + j) * 6 + 3] = i * numbers_point_width_mesh + j + 1;
                indices_flagpole[(i * (numbers_point_width_mesh - 1) + j) * 6 + 4] = (i + 1) * numbers_point_width_mesh + j;
                indices_flagpole[(i * (numbers_point_width_mesh - 1) + j) * 6 + 5] = (i + 1) * numbers_point_width_mesh + j + 1;
            }
        }

    }

    // 1. Создаем буферы
    glGenBuffers(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // 2. Копируем наши вершины в буфер для OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. Копируем наши индексы в в буфер для OpenGL
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. Устанавливаем указатели на вершинные атрибуты
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    // 5. Fill texture flag
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 6. Fill texture flag_pole
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindTexture(GL_TEXTURE_2D, texture_pole);

    // 7. Отвязываем VAO (НЕ EBO)
    glBindVertexArray(0);

    add_shader_program(shaderProgram, "shaders/simple.vert", "shaders/simple.frag");
}

void Renderer::Render() {
    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0, 1, 0);

    TimePoint current_time = Clock::now();
    float dif_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / 1000.0;
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), dif_time);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6 * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::Close() {
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}