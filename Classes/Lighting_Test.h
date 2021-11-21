#pragma once
#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Shader.h"
#include"Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include"../glm/glm.hpp"
#include"../glm/gtc/matrix_transform.hpp"
#include"../glm/gtc/type_ptr.hpp"
#include"Picking.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;
bool firstMouse = true;

Camera* myCamera = new Camera();
struct LeftMouse {
    int x;
    int y;
    bool isPressed;
};
LeftMouse leftMouse;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//鼠标位置回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//键盘输入回调函数
void processInput(GLFWwindow* window);
//帧缓存回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//窗口回调函数
void window_size_callback(GLFWwindow* window, int width, int height);
//鼠标按钮回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//纹理加载函数
unsigned int loadTexture(char const* path);

int LightTest() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3DGraphicWindow", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
#pragma region Data
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
#pragma endregion

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);

    unsigned int texture1,texture2,texture3;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("Resources/container2.png",
        &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("Resources/container2_specular.png",
        &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("Resources/matrix.jpg",
        &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    auto myShader = new Shader("Shaders/DiffuseMap.vert", "Shaders/DiffuseMap.frag");
    auto myLightShadfer = new Shader("Shaders/Light.vert", "Shaders/Light.frag");
    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);

        glm::mat4 rotate;
        lightPos.x = sin(glfwGetTime()) * 5.f;
        lightPos.z = cos(glfwGetTime()) * 5.f;
        myShader->use();
        glm::mat4 model;
        glm::mat4 view = myCamera->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(myCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);          
        myShader->setMat4("view", view);
        myShader->setMat4("projection", projection);
        myShader->setMat4("model", model);
   

        myShader->setInt("material.diffuse", 0);
        myShader->setInt("material.specular", 1);
        myShader->setInt("material.emmission", 2);
        myShader->setVec3("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
        myShader->setFloat("material.shininess", 32.0f);

        myShader->setVec3("light.ambient", 0.f, .5f, .5f);
        myShader->setVec3("light.diffuse", 1.f, .0f, .0f);
        myShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        myShader->setVec3("lightPos", lightPos);
        myShader->setVec3("viewPos", myCamera->Position);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        myLightShadfer->use();
        glm::mat4 lightModel;
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(.2f));
        myLightShadfer->setMat4("model", lightModel);
        myLightShadfer->setMat4("view", view);
        myLightShadfer->setMat4("projection", projection);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
int LightCastersTest() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3DGraphicWindow", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

#pragma region Data
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
    };
#pragma endregion

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);

    unsigned int diffuseMap = loadTexture("Resources/container2.png");
    unsigned int specularMap= loadTexture("Resources/container2_specular.png");
    
    auto myShader = new Shader("Shaders/Light_Casters.vert", "Shaders/Light_Casters.frag");
    auto myLightShadfer = new Shader("Shaders/Light.vert", "Shaders/Light.frag");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        myShader->use();
        glm::mat4 rotate;
        glm::mat4 view = myCamera->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(myCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        myShader->setMat4("view", view);
        myShader->setMat4("projection", projection);
        glm::mat4 model = glm::mat4(1.0f);
        myShader->setMat4("model", model);
        myShader->setVec3("viewPos", myCamera->Position);
        myShader->setInt("material.diffuse", 0);
        myShader->setInt("material.specular", 1);
        myShader->setFloat("material.shininess", 32.0f);

        myShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        myShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        myShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        myShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        myShader->setVec3("pointLights[0].position", pointLightPositions[0]);
        myShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        myShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        myShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        myShader->setFloat("pointLights[0].constant", 1.0f);
        myShader->setFloat("pointLights[0].linear", 0.09);
        myShader->setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        myShader->setVec3("pointLights[1].position", pointLightPositions[1]);
        myShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        myShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        myShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        myShader->setFloat("pointLights[1].constant", 1.0f);
        myShader->setFloat("pointLights[1].linear", 0.09);
        myShader->setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        myShader->setVec3("pointLights[2].position", pointLightPositions[2]);
        myShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        myShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        myShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        myShader->setFloat("pointLights[2].constant", 1.0f);
        myShader->setFloat("pointLights[2].linear", 0.09);
        myShader->setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        myShader->setVec3("pointLights[3].position", pointLightPositions[3]);
        myShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        myShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        myShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        myShader->setFloat("pointLights[3].constant", 1.0f);
        myShader->setFloat("pointLights[3].linear", 0.09);
        myShader->setFloat("pointLights[3].quadratic", 0.032);

        myShader->setFloat("spotLight.constant", 1.0f);
        myShader->setFloat("spotLight.linear", 0.09);
        myShader->setFloat("spotLight.qudratic", 0.032);
        myShader->setFloat("spotLight.innerCutOut", glm::cos(glm::radians(12.5f)));
        myShader->setFloat("spotLight.outterCutOut", glm::cos(glm::radians(17.5f)));
        myShader->setVec3("spotLight.position", myCamera->Position);
        myShader->setVec3("spotLight.center", myCamera->Front);
        myShader->setVec3("spotLight.ambient", 0, 0, 0);
        myShader->setVec3("spotLight.diffuse", .8f, 0.8f, 0.0f);
        myShader->setVec3("spotLight.specular", 0.8f, 0.8f, 0.0f);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            myShader->setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        myLightShadfer->use();
        glm::mat4 lightModel;
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(.2f));
        myLightShadfer->setMat4("model", lightModel);
        myLightShadfer->setMat4("view", view);
        myLightShadfer->setMat4("projection", projection);
        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            myLightShadfer->setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}
//与此次实验内容相关函数
int ModelLoaded() {
    //初始化GLFW窗口并且指派OpenGL的版本为3.3
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3DGraphicWindow", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2);
    glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2);
    //glViewport(SCR_WIDTH / 4, SCR_HEIGHT / 4, SCR_WIDTH / 2, SCR_HEIGHT / 2);
    //glViewport(100, 50, 300, 300);
    /*glViewport(SCR_WIDTH / 2 - 300 / 2, SCR_HEIGHT / 2 - 300 / 2, 300, 300) ;*/
    //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    //注册各种回调函数
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    stbi_set_flip_vertically_on_load(true);
    //启用深度测试
    glEnable(GL_DEPTH_TEST);
    //创建三种着色器 
    Shader myFrameBufferShader("Shaders/Picking.vert", "Shaders/Picking.frag");
    Shader myShader("Shaders/Model.vert", "Shaders/Model.frag");
    Shader mySimpleShader("Shaders/simple_color.vert", "Shaders/simple_color.frag");
    //自定义的渲染项
    std::vector<RenderItem*> items;
    items.push_back(new RenderItem("Resources/Nanosuit/nanosuit.obj"));
    //与拾取有关的类（GPU方法拾取）
    PickingTexture* pickingTexture = new PickingTexture();

    if (!pickingTexture->Init(SCR_WIDTH,SCR_HEIGHT)) {
        return false;
    }
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        //启用自定义的帧缓存，接下来的操作都在自定义的帧缓存中进行
        pickingTexture->EnableWriting();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //启用自定义帧缓存着色器，设置MVP矩阵
        myFrameBufferShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(myCamera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = myCamera->getViewMatrix();
        myFrameBufferShader.setMat4("projection", projection);
        myFrameBufferShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
        myFrameBufferShader.setMat4("model", model);
        //遍历渲染项
        for (unsigned int i = 0; i < items.size(); i++) {
            //设置该渲染项的索引
            myFrameBufferShader.setUint("ModelIndex", i);
            //启用拾取绘制，内容会被绘制到自定义帧缓存中，并且颜色缓存的值为Vec3(模型索引，网格索引，图元索引)
            //图元索引在片元着色器中通过GLSL的内建变量gl_PrimitiveID获取
            items[i]->EnablePicking();
            items[i]->Draw(myFrameBufferShader);
            items[i]->DisablePicking();
        }
        //再切换为系统默认帧缓存
        pickingTexture->DisableWriting();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //启用模型着色器，设置MVP矩阵和平行光的方向、环境光、高光和漫反射的数值
        myShader.use();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        myShader.setMat4("model", model);
        myShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        myShader.setVec3("viewPos", myCamera->Position);
        // light properties
        myShader.setVec3("dirLight.ambient", 0.51f, 0.51f, 0.5f);
        myShader.setVec3("dirLight.diffuse", 0.99f, 0.99f, 0.99f);
        myShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        // 材质的光泽度
        myShader.setFloat("material.shininess", 32.0f);
        //GLFW检测到鼠标左键被按下
        if (leftMouse.isPressed) {
            //再次切换到自定义帧缓存，并根据鼠标位置读取对应的像素值到Pixel结构体中，读完之后自动切换回默认帧缓存
            PickingTexture::PixelInfo Pixel = pickingTexture->ReadPixel(leftMouse.x, SCR_HEIGHT - leftMouse.y - 1);
            printf("ObjectID:%u---DrawIndex:%u---PrimatyID:%u\n", (unsigned int)Pixel.ObjectID,
                (unsigned int)Pixel.DrawID, (unsigned int)Pixel.PrimID);
            //如果点到的不是背景
            if (Pixel.PrimID != 0) {
                //启用三角形绘制着色器，设置对应的MVP
                mySimpleShader.use();
                mySimpleShader.setMat4("projection", projection);
                mySimpleShader.setMat4("view", view);
                mySimpleShader.setMat4("model", model);
                //一下两个绘制函数可以启用一个，一个是使得被选中的网格体变成高亮，另一个是被选中的三角形变成高亮
                items[Pixel.ObjectID]->Draw_Mesh_Onclicked(myShader, mySimpleShader, Pixel.DrawID);
                //Pixel中存放着被选中的物体模型、物体模型的网格、对应的绘制图元索引，这些像素值保存在用户自定义的颜色缓存中
                //myFrameBufferShader的片元着色器负责绘制这些像素（FragColor=（ModelIndex，MeshIndex，gl_PrimitiveID+1))
                /*items[Pixel.ObjectID]->Draw_Triangle_Onclicked(myShader, mySimpleShader, Pixel.DrawID, Pixel.PrimID - 1);*/
            }
        }
        //如果鼠标左键没有点击，那么就默认渲染
        else
            for (unsigned int i = 0; i < items.size(); i++) {
                items[i]->Draw(myShader);
            }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    printf("XPOS:%lf\tYPOS:%lf\n", xpos, ypos);
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if (!(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS))
        return;
    myCamera->ProcessMouseMovement(xoffset, yoffset);
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        myCamera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        myCamera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        myCamera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        myCamera->ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        myCamera->MovementSpeed = 4.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        myCamera->MovementSpeed = 2.5f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        printf("WIDTH:%d,HEIGHT:%d\n", width, height);
        glfwSetWindowSize(window, width + 10, height + 10);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        printf("WIDTH:%d,HEIGHT:%d\n", width, height);
        glfwSetWindowSize(window, width - 10, height - 10);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    myCamera->ProcessMouseScroll(yoffset);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftMouse.isPressed = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        leftMouse.x = xpos;
        leftMouse.y = ypos;
    }
    else
        leftMouse.isPressed = false;

}