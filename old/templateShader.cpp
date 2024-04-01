#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <math.h>

int main() {
    GLFWwindow *window;

    const GLchar *vertex120 = R"END(
    #version 120
    attribute vec4 inPosition;
    attribute vec3 inColor;

    varying vec4 outColor;

    void main() {
        outColor = vec4(inColor,1);
        gl_Position = inPosition;
    }
    )END";

    const GLchar *fragment120 = R"END(
    #version 120
    varying vec4 outColor;

    void main() {
        gl_FragColor = outColor;
    }
    )END";

    if (!glfwInit()) {
        std::cout << "Init error";
        return -1;
    }

    window = glfwCreateWindow(800, 800, "Hello", 0, 0);
    if (!window) {
        std::cout << "Window creation error";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD init error";
        return -1;
    }

    // compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex120, 0);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment120, 0);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        int length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        char *log = new char[length];
        glGetProgramInfoLog(shaderProgram, length, &length, log);
        std::cout << "Shader program linking failed\n" << log << std::endl;
    }

    // attributes
    GLint attribPos = glGetAttribLocation(shaderProgram, "inPosition");
    GLint attribColor = glGetAttribLocation(shaderProgram, "inColor");

    // vertices
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    // colors
    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    GLuint vao, vbo, cbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(attribPos);
    glVertexAttribPointer(attribPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &cbo);
    glBindBuffer(GL_ARRAY_BUFFER, cbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glEnableVertexAttribArray(attribColor);
    glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    const GLuint nVertices = sizeof(vertices) / sizeof(vertices[0]) / 3;

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, nVertices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}