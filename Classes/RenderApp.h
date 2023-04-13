#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "GeometryGenerator.h"
#include "Picking.h"
#include "Utility.h"

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
#include"../glm/glm.hpp"
#include"../glm/gtc/matrix_transform.hpp"
#include"../glm/gtc/type_ptr.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include "../imgui/ImGuizmo.h"
#include "../imgui/imgui_internal.h"

glm::vec3 lightPos(10.0f, 10.0f, 10.0f);
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
float camDistance = 8.f;
float matrix[16] = {
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };

float identity_matrix[16] = { 1.f, 0.f, 0.f, 0.f,
                            0.f, 1.f, 0.f, 0.f,
                            0.f, 0.f, 1.f, 0.f,
                            0.f, 0.f, 0.f, 1.f };
//float camera_view[16] = { 1.f, 0.f, 0.f, 0.f,
//                      0.f, 1.f, 0.f, 0.f,
//                      0.f, 0.f, 1.f, 0.f,
//                      0.f, 0.f, 0.f, 1.f };
//float camera_projection[16];

class RenderApp
{
public:
    RenderApp();
    ~RenderApp();
public:
    int Run();
    int Init();
public:

    void processInput(GLFWwindow* window);
    unsigned int loadTexture(char const* path);
    void gui_setting(GLFWwindow* window);
    void viewport_setting(GLFWwindow* window);
    void ui_rendering(GLFWwindow* window);

private:
    GLFWwindow* window;
    std::vector<RenderItem*> items;
    Camera* cur_camera = new Camera();
    RenderItem* cur_item;
    RenderItem* empty_item;
    
public:
    GLFWwindow* getWindow() { return this->window; }
    Camera* getCamera() { return this->cur_camera; }
    std::vector<RenderItem*> getItems(){ return this->items; }
    RenderItem* getCurRenderItem() { return this->cur_item; }
    ImVec2 viewport_pos, viewport_size;
public:
    int SCR_WIDTH = 1920;
    int SCR_HEIGHT = 1080;
    float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;

    bool firstMouse = true;
    bool hiddenMosue = false;
    bool isPressed = false;

    float deltaTime = 0.0f; // 当前帧与上一帧的时间差
    float lastFrame = 0.0f; // 上一帧的时间

};

RenderApp::RenderApp()
{
    if (!RenderApp::Init()) {
        std::cout << "Init Failure!" << std::endl;
    }
    empty_item = new RenderItem();
}
RenderApp::~RenderApp()
{
}

int RenderApp::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RenderApp", NULL, NULL);

    GLFWimage images[1];
    images[0].pixels = stbi_load("Resources/Icon/icon.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

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
}
int RenderApp::Run() {

    items.push_back(new RenderItem("Resources/Nanosuit/nanosuit.obj"));
    //Shader shader = Shader("Shaders/Model.vert", "Shaders/Model.frag");
    //items[0]->setShader(shader);
    cur_camera->Position.z = 10.0f;
    ImGuiIO& io = ImGui::GetIO();
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //处理输入，清除颜色缓存和深度缓存
        processInput(window);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        ui_rendering(window);
        viewport_setting(window);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        //向shader传送所有的需要的uniform变量资源
        glm::mat4 projection = cur_camera->getProjectionMatrix();
        glm::mat4 view = cur_camera->getViewMatrix();

        items[0]->getShader().use();
        items[0]->getShader().setMat4("projection", projection);
        items[0]->getShader().setMat4("view", view);
        //shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        //shader.setVec3("viewPos", myCamera->Position);
        //shader.setVec3("dirLight.ambient", 0.51f, 0.51f, 0.5f);
        //shader.setVec3("dirLight.diffuse", 0.99f, 0.99f, 0.99f);
        //shader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        //shader.setFloat("material.shininess", 32.0f);

        for (unsigned int i = 0; i < items.size(); i++) {
            if (!items[i]->isDisabled())
                items[i]->Draw();
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();  

    glfwTerminate();
    return 0;
}

unsigned int RenderApp::loadTexture(char const* path)
{
    //定义并生成纹理对象
    unsigned int textureID;
    glGenTextures(1, &textureID);
    //通过stbi库加载贴图
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
        //将纹理对象绑定到2D纹理缓存中去，然后绑定纹理数据data，并由OpenGL生成贴图
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        //设置纹理环绕方式为重复，过滤方式为线性过滤和线性MipMap过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //绑定完毕后释放data，减少内存占用
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}
void RenderApp::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cur_camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cur_camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cur_camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cur_camera->ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cur_camera->MovementSpeed = 4.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        cur_camera->MovementSpeed = 2.5f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        hiddenMosue = !hiddenMosue;
}
void RenderApp::gui_setting(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(NULL);
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.Fonts->AddFontFromFileTTF("Resources/Fonts/kaiu.ttf", 24, NULL, io.Fonts->GetGlyphRangesChineseFull());

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiViewportFlags_NoDecoration;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiCol_DockingEmptyBg;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.37f, 0.36f, 0.36f, 102.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 171.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 255.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.45f, 0.45f, 0.45f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.55f, 0.55f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.46f, 0.46f, 0.46f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.17f, 0.17f, 0.17f, 0.95f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.78f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.45f, 0.45f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.51f, 0.51f, 0.51f, 0.70f);
    colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 0.97f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.40f, 0.13f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.45f, 1.00f, 0.85f, 0.35f);

    style.WindowRounding = 4;
    style.FrameRounding = 4;
    style.GrabRounding = 4;
    style.ScrollbarSize = 7;
    style.ScrollbarRounding = 0;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}
void RenderApp::viewport_setting(GLFWwindow* window) {

    int main_window_x, main_window_y;
    glfwGetWindowPos(window, &main_window_x, &main_window_y);

    ImGuiIO& io = ImGui::GetIO();
    static ImGuiWindowFlags gizmoWindowFlags = 0;
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;
        
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    //3D Gizmo
    ImGui::Begin("viewport_window", 0, gizmoWindowFlags);
    ImGuizmo::SetDrawlist();
    float windowWidth = (float)ImGui::GetWindowWidth();
    float windowHeight = (float)ImGui::GetWindowHeight();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

    viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
    viewManipulateTop = ImGui::GetWindowPos().y;
    ImGuiWindow* viewport_window = ImGui::GetCurrentWindow();
    gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(viewport_window->InnerRect.Min, viewport_window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
    viewport_size = ImGui::GetWindowSize();
    viewport_pos = ImGui::GetWindowPos();

    if (ImGui::IsKeyPressed(ImGuiKey_T))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
        mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_S))
        useSnap = !useSnap;
    ImGui::Checkbox("##UseSnap", &useSnap);
    ImGui::SameLine();

    switch (mCurrentGizmoOperation)
    {
    case ImGuizmo::TRANSLATE:
        ImGui::InputFloat3("Snap", &snap[0]);
        break;
    case ImGuizmo::ROTATE:
        ImGui::InputFloat("Angle Snap", &snap[0]);
        break;
    case ImGuizmo::SCALE:
        ImGui::InputFloat("Scale Snap", &snap[0]);
        break;
    }
    ImGui::Checkbox("Bound Sizing", &boundSizing);
    if (boundSizing)
    {
        ImGui::PushID(3);
        ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
        ImGui::SameLine();
        ImGui::InputFloat3("Snap", boundsSnap);
        ImGui::PopID();
    }
    auto camera_view = (float*)glm::value_ptr(cur_camera->getViewMatrix());
    auto camera_projection = (float*)glm::value_ptr(cur_camera->getProjectionMatrix());
    ImGuizmo::DrawGrid(camera_view, camera_projection, identity_matrix, 100.f);

    if (cur_item) {

        glm::rotate();
        cur_item->updateModelMat();
        ImGuizmo::Manipulate(camera_view, camera_projection, mCurrentGizmoOperation, mCurrentGizmoMode, (float*)glm::value_ptr(cur_item->model_mat),
            NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
        ImGuizmo::ViewManipulate(camera_view, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

        //cur_item->translation[0] = cur_item->model_mat_value[12];
        //cur_item->translation[1] = cur_item->model_mat_value[13];
        //cur_item->translation[2] = cur_item->model_mat_value[14];

        //cur_item->scale[0] = cur_item->model_mat_value[0];
        //cur_item->scale[1] = cur_item->model_mat_value[5];
        //cur_item->scale[2] = cur_item->model_mat_value[10];
    }  
    ImGui::End();

    viewport_pos.x -= main_window_x;
    viewport_pos.y = SCR_HEIGHT - viewport_size.y - (viewport_pos.y - main_window_y);

    glViewport((int)viewport_pos.x, (int)viewport_pos.y, viewport_size.x, viewport_size.y);
  
}
void RenderApp::ui_rendering(GLFWwindow* window) {
    //Detail Window
    ImGui::Begin("Detail");
    if (cur_item) 
    {

        ImGuizmo::DecomposeMatrixToComponents((float*)glm::value_ptr(cur_item->model_mat), 
            cur_item->translation, cur_item->rotation, cur_item->scale);

        ImGui::Text("Position");
        ImGui::PushItemWidth(130.0f);
        ImGui::InputFloat("P.x", &cur_item->translation[0], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("P.y", &cur_item->translation[1], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("P.z", &cur_item->translation[2], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();

        ImGui::NewLine();
        ImGui::Text("Rotation");
        ImGui::InputFloat("R.x", &cur_item->rotation[0], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("R.y", &cur_item->rotation[1], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("R.z", &cur_item->rotation[2], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();

        ImGui::NewLine();
        ImGui::Text("Scale");
        ImGui::InputFloat("S.x", &cur_item->scale[0], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("S.y", &cur_item->scale[1], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("S.z", &cur_item->scale[2], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::PopItemWidth();

        ImGui::NewLine(); ImGui::NewLine();
        ImGui::Checkbox("Disabled", cur_item->getDisabled());
    }
    else 
    {
        ImGui::Text("Position");
        ImGui::PushItemWidth(130.0f);
        ImGui::InputFloat("P.x", &empty_item->translation[0], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("P.y", &empty_item->translation[1], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("P.z", &empty_item->translation[2], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();

        ImGui::NewLine();
        ImGui::Text("Rotation");
        ImGui::InputFloat("R.x", &empty_item->rotation[0], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("R.y", &empty_item->rotation[1], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("R.z", &empty_item->rotation[2], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();

        ImGui::NewLine();
        ImGui::Text("Scale");
        ImGui::InputFloat("S.x", &empty_item->scale[0], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("S.y", &empty_item->scale[1], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
        ImGui::InputFloat("S.z", &empty_item->scale[2], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
        ImGui::PopItemWidth();
    }

    ImGui::NewLine();
    ImGui::Text(u8"绘制模式");
    const char* mode_items[] = { "Fill_Mode","WireFrame_Mode","Point_Mode" };
    static const char* current_item = mode_items[0];
    static int current_index = 0;
    if (ImGui::BeginCombo("##render_combo", current_item)) 
    {

        for (unsigned int i = 0; i < IM_ARRAYSIZE(mode_items); i++)
        {
            bool is_selected = (current_item == mode_items[i]);
            if (ImGui::Selectable(mode_items[i], is_selected))
                current_item = mode_items[current_index = i];
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        switch (current_index)
        {
        case 0:glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
        case 1:glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
        case 2:glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); glPointSize(3.0f); break;
        }
        ImGui::EndCombo();
    }
    ImGui::End();

    //ObjectLists Window
    ImGui::Begin(u8"物体列表", 0, ImGuiWindowFlags_NoCollapse);
    auto items = this->getItems();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    for (unsigned int i = 0; i < items.size(); i++) {

        ImGui::PushID(1);
        ImGui::AlignTextToFramePadding();
        bool node_open = ImGui::TreeNode("Object", "%s", items[i]->getName().c_str());
        if (node_open)
        {
            cur_item = items[i];
            ImGui::TreePop();
        }
        else
            cur_item = NULL;
        ImGui::PopID();

    }
    ImGui::PopStyleVar();
    ImGui::End();
}

