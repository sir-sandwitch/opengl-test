#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <math.h>
#include <bmpread.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


int main() {
    GLFWwindow *window;

    const GLchar* vertex140 = R"END(
    #version 140
    attribute vec3 inPosition;
    attribute vec3 inColor;
    attribute vec2 inUvs;
    uniform mat4 matrix[100]; // Array of transformation matrices, one for each instance
    uniform mat4 projection;
    uniform float time;
    varying vec3 outColor;
    varying vec2 outUvs;
    void main()
    {
        int instanceId = gl_InstanceID; // Get the instance ID
        float theta = time; // Use the time offset for this instance
        float c = cos(theta);
        float s = sin(theta);
        mat4 rotationY = mat4(
            c, 0, s, 0,
            0, 1, 0, 0,
            -s, 0, c, 0,
            0, 0, 0, 1
        );
        mat4 rotationX = mat4(
            1, 0, 0, 0,
            0, c, -s, 0,
            0, s, c, 0,
            0, 0, 0, 1
        );
        mat4 rotationZ = mat4(
            c, -s, 0, 0,
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );

        outUvs = inUvs;
        outColor = inColor;
        gl_Position = projection * matrix[instanceId] * rotationY * rotationX * vec4(inPosition,1);
    }
    )END";

    const GLchar* raster120 = R"END(
    #version 120
    varying vec3 outColor;
    varying vec2 outUvs;
    uniform sampler2D tex;
    void main()
    {
        gl_FragColor = /*vec4(outColor,1.0) /2f +*/ vec4(texture2D(tex, outUvs).rgb, 1.0);
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
    glShaderSource(vertexShader, 1, &vertex140, 0);
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

    GLfloat vertices[] = {
        // Front face
        -1.0f, -1.0f,  1.0f,  // 0
        1.0f, -1.0f,  1.0f,  // 1
        1.0f,  1.0f,  1.0f,  // 2
        -1.0f,  1.0f,  1.0f,  // 3

        // Right face
        1.0f, -1.0f,  1.0f,  // 4
        1.0f, -1.0f, -1.0f,  // 5
        1.0f,  1.0f, -1.0f,  // 6
        1.0f,  1.0f,  1.0f,  // 7

        // Back face
        1.0f, -1.0f, -1.0f,  // 8
        -1.0f, -1.0f, -1.0f,  // 9
        -1.0f,  1.0f, -1.0f,  // 10
        1.0f,  1.0f, -1.0f,  // 11

        // Left face
        -1.0f, -1.0f, -1.0f,  // 12
        -1.0f, -1.0f,  1.0f,  // 13
        -1.0f,  1.0f,  1.0f,  // 14
        -1.0f,  1.0f, -1.0f,  // 15

        // Top face
        -1.0f,  1.0f,  1.0f,  // 16
        1.0f,  1.0f,  1.0f,  // 17
        1.0f,  1.0f, -1.0f,  // 18
        -1.0f,  1.0f, -1.0f,  // 19

        // Bottom face
        -1.0f, -1.0f, -1.0f,  // 20
        1.0f, -1.0f, -1.0f,  // 21
        1.0f, -1.0f,  1.0f,  // 22
        -1.0f, -1.0f,  1.0f   // 23
    };

    GLubyte indices[] = {
        0,  1,  2,  0,  2,  3,  // Front face
        4,  5,  6,  4,  6,  7,  // Right face
        8,  9, 10,  8, 10, 11,  // Back face
        12, 13, 14, 12, 14, 15, // Left face
        16, 17, 18, 16, 18, 19, // Top face
        20, 21, 22, 20, 22, 23  // Bottom face
    };

    GLfloat colors[] = {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,

        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,

        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,

        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,

        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,

        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
    };

    GLuint verticesBuf;
    glGenBuffers(1, &verticesBuf);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint indicesBuf;
    glGenBuffers(1, &indicesBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint colorsBuf;
    glGenBuffers(1, &colorsBuf);
    glBindBuffer(GL_ARRAY_BUFFER, colorsBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glm::mat4 matrix[100];
    for(int i = 0; i < 100; i++) {
        //random positions for the cubes
        matrix[i] = glm::translate(glm::mat4(1.0f), glm::vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5));

        //random rotations for the cubes
        matrix[i] = glm::rotate(matrix[i], (float)(rand() % 360), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix[i] = glm::rotate(matrix[i], (float)(rand() % 360), glm::vec3(0.0f, 1.0f, 0.0f));

        //random scales for the cubes
        GLfloat scale = (rand() % 10) / 10.0f;
        matrix[i] = glm::scale(matrix[i], glm::vec3(scale, scale, scale));
    }

    GLuint attribPosition;
    attribPosition = glGetAttribLocation(shaderProgram, "inPosition");
    glEnableVertexAttribArray(attribPosition);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBuf);
    glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint attribColor;
    attribColor = glGetAttribLocation(shaderProgram, "inColor");
    glEnableVertexAttribArray(attribColor);
    glBindBuffer(GL_ARRAY_BUFFER, colorsBuf);
    glVertexAttribPointer(attribColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint attribMatrix;
    attribMatrix = glGetUniformLocation(shaderProgram, "matrix");
    glUniformMatrix4fv(attribMatrix, 100, GL_FALSE, glm::value_ptr(matrix[0]));

    GLuint attribTime;
    attribTime = glGetUniformLocation(shaderProgram, "time");

    bmpread_t bitmap;

    if(!bmpread("jason(3).bmp", 0, &bitmap)) {
        std::cout << "Error reading texture" << std::endl;
        return -1;
    }

    GLuint texid;
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.data);

    GLuint attribTex;
    attribTex = glGetUniformLocation(shaderProgram, "tex");
    glUniform1i(attribTex, 0);

    // texture attribs

    GLfloat uvs[] = {
        // Front face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // Right face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // Back face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // Left face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // Top face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        // Bottom face
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };

    GLuint uvsData;
    glGenBuffers(1, &uvsData);
    glBindBuffer(GL_ARRAY_BUFFER, uvsData);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

    GLuint attribUvs;
    attribUvs = glGetAttribLocation(shaderProgram, "inUvs");
    glEnableVertexAttribArray(attribUvs);
    glBindBuffer(GL_ARRAY_BUFFER, uvsData);
    glVertexAttribPointer(attribUvs, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::translate(scaleMatrix, glm::vec3(0.0f, 0.0f, -2.0f));

    glm::mat4 projectionMatrix = glm::perspective(glm::radians(120.0f), 1.0f, 0.f, 10.0f) * scaleMatrix;
    
    GLint uniformProj = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(uniformProj, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glEnable(GL_CULL_FACE);

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glUniform1f(attribTime, glfwGetTime());

        // glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_BYTE, 0);

        glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_BYTE, 0, 100);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}