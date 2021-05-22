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

    int num_points_in_vert = 6;
    GLfloat vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 8 * num_points_in_vert];
    GLuint indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 6 * 2];
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
        unsigned char *data_texture = stbi_load("gml.jpg", &texture_width, &texture_height,
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
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * num_points_in_vert + 0] = -0.5 + step_w * j;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * num_points_in_vert + 2] = -0.5 + step_h * i;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * num_points_in_vert + 1] = 0.;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * num_points_in_vert + 3] = 0. + step_w * j / 3.;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * num_points_in_vert + 4] = 0. + step_h * i;
                vertices[(i * NUMBERS_POINT_WIDTH_MESH + j) * num_points_in_vert + 5] = 0. + step_w * j;
            }
        }
        // flagpole
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 0] = -0.55;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1] = 0.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3] = 1. / 3.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4] = 0;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 5] = 0;

        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 0 + num_points_in_vert] = -0.55;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + num_points_in_vert] = 0.5;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + num_points_in_vert] = 0.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3 + num_points_in_vert] = 1. / 3.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4 + num_points_in_vert] = 1;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 5 + num_points_in_vert] = 0;

        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 0 + 2 * num_points_in_vert] = -0.5;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + 2 * num_points_in_vert] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + 2 * num_points_in_vert] = 0.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3 + 2 * num_points_in_vert] = 2. / 3.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4 + 2 * num_points_in_vert] = 0;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 5 + 2 * num_points_in_vert] = 0;

        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 0 + 3 * num_points_in_vert] = -0.5;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + 3 * num_points_in_vert] = 0.5;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + 3 * num_points_in_vert] = 0.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3 + 3 * num_points_in_vert] = 2. / 3.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4 + 3 * num_points_in_vert] = 1;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 5 + 3 * num_points_in_vert] = 0;

        // base
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 0 + 4 * num_points_in_vert] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + 4 * num_points_in_vert] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + 4 * num_points_in_vert] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3 + 4 * num_points_in_vert] = 2. / 3.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4 + 4 * num_points_in_vert] = 0.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 5 + 4 * num_points_in_vert] = 0.;

        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 0 + 5 * num_points_in_vert] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + 5 * num_points_in_vert]  = 1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + 5 * num_points_in_vert] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3 + 5 * num_points_in_vert] = 2. / 3.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4 + 5 * num_points_in_vert] = 1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 5 + 5 * num_points_in_vert] = 0.;

        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 0 + 6 * num_points_in_vert] = 1;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + 6 * num_points_in_vert] = -1;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + 6 * num_points_in_vert] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3 + 6 * num_points_in_vert] = 1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4 + 6 * num_points_in_vert] = 0.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 5 + 6 * num_points_in_vert] = 0.;

        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 0 + 7 * num_points_in_vert] = 1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + 7 * num_points_in_vert] = 1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + 7 * num_points_in_vert] = -1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3 + 7 * num_points_in_vert] = 1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4 + 7 * num_points_in_vert] = 1.;
        vertices[num_points_in_vert * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 5 + 7 * num_points_in_vert] = 0.;

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

        //flagpole
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 0] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH;
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 1] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1;
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 2] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2;

        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 3] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1;
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 4] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2;
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 5] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3;

        // base
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 0 + 6] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 4;
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 1 + 6] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + 4;
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 2 + 6] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + 4;

        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 3 + 6] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 1 + 4;
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 4 + 6] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 2 + 4;
        indices[2 * 3 * (NUMBERS_POINT_HEIGHT_MESH - 1) * (NUMBERS_POINT_WIDTH_MESH - 1) + 5 + 6] = NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 3 + 4;


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
        int index = 0;
        float offset = 0.1f;
        for(int y = -10; y < 10; y += 2)
        {
            for(int x = -10; x < 10; x += 2)
            {
                translations[3 * index] = (float)x / 10.0f + offset;
                translations[3 * index + 1] = 1.0;
                translations[3 * index++ + 2] = (float)y / 10.0f + offset;
            }
        }

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, num_points_in_vert * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    // 5. Fill texture flag
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, num_points_in_vert * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 6. Fill distance from flagpole
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, num_points_in_vert * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 7. Отвязываем VAO (НЕ EBO)
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    add_shader_program(shaderProgram, "shaders/simple.vert", "shaders/simple.frag");

}

void Renderer::Render() {
    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), 0, 1, 0);

    TimePoint current_time = Clock::now();
    float dif_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / 1000.0;
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), dif_time);

    glUniform3fv(glGetUniformLocation(shaderProgram, "offsets"), 100, translations);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6 * NUMBERS_POINT_HEIGHT_MESH * NUMBERS_POINT_WIDTH_MESH + 12, GL_UNSIGNED_INT, 0, 100);
    glBindVertexArray(0);
}

void Renderer::Close() {
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}