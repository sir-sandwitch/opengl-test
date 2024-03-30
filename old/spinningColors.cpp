#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

const GLchar* vertex120 = R"END(
#version 120
attribute vec4 inColor;
attribute vec4 inPosition;
uniform mat4 matrix;
varying vec4 outColor;
void main()
{
    outColor = inColor;
    gl_Position = matrix * inPosition;
}
)END";

const GLchar* raster120 = R"END(
#version 120
varying vec4 outColor;
void main()
{
    gl_FragColor = outColor;
}
)END";

const GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
    -1.0f,  -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f
};

const GLfloat colors[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};

int main() {
    GLFWwindow *window;

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
    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "GLAD init error";
        return -1;
    }

    // std::cout << "Compiling shaders... (0/2) (vertex)" << std::endl;

    //vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex120, 0);
    glCompileShader(vertexShader);

    // std::cout << "\rCompiling shaders... (0/2) (error checking)" << std::endl;

    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLchar info[512];
        glGetShaderInfoLog(vertexShader, 512, 0, info);
        std::cout << "\nVertex shader error: " << info << std::endl;
        exit(1);
    }

    // std::cout << "\rCompiling shaders... (1/2) (fragment)";

    //fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &raster120, 0);
    glCompileShader(fragmentShader);

    // std::cout << "\rCompiling shaders... (1/2) (error checking)" << std::endl;

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLchar info[512];
        glGetShaderInfoLog(fragmentShader, 512, 0, info);
        std::cout << "\nFragment shader error: " << info << std::endl;
        exit(1);
    }

    // std::cout << "\rCompiling shaders... (2/2)" << std::endl;

    //shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLchar info[512];
        glGetProgramInfoLog(shaderProgram, 512, 0, info);
        std::cout << "Shader program error: " << info << std::endl;
        exit(1);
    }

    //use program
    glUseProgram(shaderProgram);

    // std::cout << "Setting up VBO..." << std::endl;

    //VBO setup
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    GLuint colorsBuffer;
    glGenBuffers(1, &colorsBuffer);

    //send data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    //get attribute and uniform locations
    GLint attribPosition = glGetAttribLocation(shaderProgram, "inPosition");
    glEnableVertexAttribArray(attribPosition);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint attribColor = glGetAttribLocation(shaderProgram, "inColor");
    glEnableVertexAttribArray(attribColor);
    glBindBuffer(GL_ARRAY_BUFFER, colorsBuffer);
    glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint attributeMatrix = glGetUniformLocation(shaderProgram, "matrix");

    float alpha = 0;

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        float sa = 0.5 * sin(alpha);
        float ca = 0.5 * cos(alpha);
        alpha += 0.01;

        const GLfloat matrix[] = {
            ca, -sa, 0, 0,
            sa, ca, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        glUniformMatrix4fv(attributeMatrix, 1, GL_FALSE, matrix);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}