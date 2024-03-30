#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <math.h>

int main() {
    GLFWwindow *window;

    const GLchar* vertex120 = R"END(
    #version 120
    attribute vec3 inPosition;
    void main()
    {
        gl_Position = vec4(inPosition,1);
    }
    )END";

    const GLchar* raster120 = R"END(
    #version 120
    uniform float time;
    uniform vec2 resolution;
    void main()
    {
        vec2 centerPoint = resolution / 2.0;
        vec2 currentPoint = gl_FragCoord.xy;

        if(length(currentPoint - centerPoint) < 100.0){
            gl_FragColor = vec4(1.0,1.0,1.0,1.0);
        }
        else{
            gl_FragColor = vec4(0.0,0.0,0.0,1.0);
        }
        
        // float i = 1 - (gl_FragCoord.y / resolution.y);
        // gl_FragColor = vec4(i*abs(sin(time)),i*abs(sin(time*3.f)),i*abs(sin(time/2.f)), 1.0);
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

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed\n" << infoLog << std::endl;
        exit(1);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &raster120, 0);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed\n" << infoLog << std::endl;
        exit(1);
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader program linking failed\n" << infoLog << std::endl;
        exit(1);
    }

    glUseProgram(shaderProgram);

    // vbos
    GLfloat positions[] = {
        -1, -1, 0,
        -1, 1, 0,
        1, -1, 0,
        1, -1, 0,
        -1, 1, 0,
        1, 1, 0
    };
    
    GLuint positionsData;
    glGenBuffers(1, &positionsData);
    glBindBuffer(GL_ARRAY_BUFFER, positionsData);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    //attribs

    GLuint attribPosition;

    attribPosition = glGetAttribLocation(shaderProgram, "inPosition");
    glEnableVertexAttribArray(attribPosition);
    glBindBuffer(GL_ARRAY_BUFFER, positionsData);
    glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint res = glGetUniformLocation(shaderProgram, "resolution");
    glUniform2f(res, 800, 800);

    GLuint time = glGetUniformLocation(shaderProgram, "time");

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1f(time, glfwGetTime());

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}