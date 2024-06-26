#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <math.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
    GLFWwindow *window;

    const GLchar *vertex120 = R"END(
    #version 120
    //position
    attribute vec4 inPosition;
    //color
    attribute vec3 inColor;
    //normal
    attribute vec3 inNormal;
    
    //time for rotation
    uniform float time;

    //surface -> light vector
    uniform vec3 light;

    //light color
    uniform vec3 lightColor;

    //diffuse color
    uniform vec3 diffuseColor;

    //model view projection matrix
    uniform mat4 mvp;

    //distance from surface to light
    attribute float distance;

    //output color
    varying vec4 outColor;

    //light power
    uniform float power;

    void main() {
        // float theta = time;
        // float c = cos(theta);
        // float s = sin(theta);
        // mat4 rotationY = mat4(
        //     c, 0, s, 0,
        //     0, 1, 0, 0,
        //     -s, 0, c, 0,
        //     0, 0, 0, 1
        // );
        // mat4 rotationX = mat4(
        //     1, 0, 0, 0,
        //     0, c, -s, 0,
        //     0, s, c, 0,
        //     0, 0, 0, 1
        // );

        // outColor = vec4(inColor,1);
        // gl_Position = mvp * rotationX * rotationY * inPosition;

        vec3 ambientColor = lightColor * 0.1;

        vec3 n = normalize(inNormal);

        vec3 l = normalize(light);

        float cosTheta = clamp(dot(n, l), 0, 1);

        outColor = vec4(ambientColor + diffuseColor * inColor * lightColor * power * cosTheta / (distance * distance), 1);
        gl_Position = mvp * inPosition;

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

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    std::string warn;

    if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "jason.obj")) {
        std::cout << "Failed to load obj file\n";
        return -1;
    }

    if(!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    if(!err.empty()) {
        std::cout << "Error: " << err << std::endl;
        return -1;
    }

    // vertices
    GLfloat vertices[attrib.vertices.size()];
    for(size_t i = 0; i < attrib.vertices.size(); i++) {
        vertices[i] = attrib.vertices[i];
    }

    // colors
    GLfloat colors[attrib.vertices.size()];
    for(size_t i = 0; i < attrib.vertices.size(); i++) {
        colors[i] = 1;
    }

    // indices
    GLuint indices[shapes[0].mesh.indices.size()];
    for(size_t i = 0; i < shapes[0].mesh.indices.size(); i++) {
        indices[i] = shapes[0].mesh.indices[i].vertex_index;
    }

    // normals
    GLfloat normals[attrib.normals.size()];
    for(size_t i = 0; i < attrib.normals.size(); i++) {
        normals[i] = attrib.normals[i];
    }

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

    GLuint indicesBuf;
    glGenBuffers(1, &indicesBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint normalsBuf;
    glGenBuffers(1, &normalsBuf);
    glBindBuffer(GL_ARRAY_BUFFER, normalsBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    glm::mat4 model = glm::mat4(0.5f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
    glm::mat4 projection = glm::perspective(glm::radians(240.0f), 1.0f, 0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;

    // for(int i = 0; i < sizeof(vertices)/sizeof(GLfloat); i++) {
    //     std::cout << vertices[i] << std::endl;
    // }

    GLuint attribMvp;
    attribMvp = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(attribMvp, 1, GL_FALSE, glm::value_ptr(mvp));

    GLuint attribTime;
    attribTime = glGetUniformLocation(shaderProgram, "time");

    glm::vec3 light = glm::vec3(0, 10, 10);

    GLuint attribLight;
    attribLight = glGetUniformLocation(shaderProgram, "light");
    glUniform3f(attribLight, light.x, light.y, light.z);

    GLuint attribLightColor;
    attribLightColor = glGetUniformLocation(shaderProgram, "lightColor");
    glUniform3f(attribLightColor, 1, 1, 1);
    GLuint attribDiffuseColor;
    attribDiffuseColor = glGetUniformLocation(shaderProgram, "diffuseColor");
    glUniform3f(attribDiffuseColor, 1, 1, 1);

    GLuint attribDistance;
    attribDistance = glGetAttribLocation(shaderProgram, "distance");
    GLfloat distances[attrib.vertices.size()];
    //calculate distance between each vertex and light
    for(size_t i = 0; i < attrib.vertices.size(); i+=3) {
        distances[i/3] = sqrt(pow(attrib.vertices[i] - light.x, 2) + pow(attrib.vertices[i + 1] - light.y, 2) + pow(attrib.vertices[i + 2] - light.z, 2));
    }
    GLuint distancesBuf;
    glGenBuffers(1, &distancesBuf);
    glBindBuffer(GL_ARRAY_BUFFER, distancesBuf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(distances), distances, GL_STATIC_DRAW);
    glVertexAttribPointer(attribDistance, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribDistance);

    GLuint attribPower;
    attribPower = glGetUniformLocation(shaderProgram, "power");
    glUniform1f(attribPower, 100);

    const GLuint nVertices = sizeof(vertices) / sizeof(vertices[0]) / 3;

    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform1f(attribTime, glfwGetTime());

        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}