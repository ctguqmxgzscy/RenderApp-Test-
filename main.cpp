#pragma once
#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Classes/Shader.h"

#include"Classes/RenderApp.h"

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

RenderApp* app;

int main()
{  
    app = new RenderApp();

    glfwSetScrollCallback(app->getWindow(), scroll_callback);
    glfwSetCursorPosCallback(app->getWindow(), mouse_callback);
    glfwSetMouseButtonCallback(app->getWindow(), mouse_button_callback);
    app->SetGUI();

    return app->Run();
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (app->firstMouse)
    {
        app->lastX = xpos;
        app->lastY = ypos;
        app->firstMouse = false;
    }

    float xoffset = xpos - app->lastX;
    float yoffset = app->lastY - ypos;

    app->lastX = xpos;
    app->lastY = ypos;

    if (!(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS))
        return;
    if (app->getLeftMouse()->isPressed)
        app->getCamera()->ProcessMouseMovement(-xoffset, -yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(app->getWindowFlags()->isViewportHover)
        app->getCamera()->ProcessMouseScroll(yoffset);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    auto mouse = app->getLeftMouse();
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        mouse->isPressed = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        mouse->x = xpos;
        mouse->y = ypos;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        mouse->isOnClicked = true;
    else
    {
        mouse->isOnClicked = false;
        mouse->isPressed = false;
    }
}
