#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#define PI 3.1415926
const int steps = 100;
const float angle = PI * 2.f / steps;

void drawCircle(float red, float green, float blue){
    float radius = 1.0f;
    float prevX = 0;
    float prevY = 1.0f;
    for (int i=0; i <= steps; i++){
        glBegin(GL_TRIANGLES);

        float newX = radius * sin(angle*i);
        float newY = -radius * cos(angle*i);
        glColor3f(red,green,blue);
        glVertex3f(0.f,0.f,0.f);
        glVertex3f(prevX,prevY,0.f);
        glVertex3f(newX,newY,0.f);

        glEnd();

        prevX = newX;
        prevY = newY;
    }
}

int main(){
    if(!glfwInit()){
        std::cout << "Init error";
        return -1;
    }
    
    GLFWwindow *window;

    window = glfwCreateWindow(800,800,"Hello",0,0);
    if(!window){
        std::cout << "Window creation error";
        glfwTerminate();
        return -1;
    }

    // render loop
    glfwMakeContextCurrent(window);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(0.1,0.1,0.1);

    float angle = 0;

    float angleMoon = 0;

    while(!glfwWindowShouldClose(window)){
        angle += 1;
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);

        drawCircle(1,1,0);

        {
            glPushMatrix();
            glRotatef(angle,0,0,1);
            glTranslatef(0,5,0);
            glScalef(0.6,0.6,1);
            drawCircle(0,0.3,1);

            {
                glPushMatrix();
                glRotatef(angleMoon,0,0,1);
                glTranslatef(0,3,0);
                glScalef(0.5,0.5,1);
                drawCircle(0.5,0.5,0.5);
                glPopMatrix();
                angleMoon += 3;
            }

            glPopMatrix();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}