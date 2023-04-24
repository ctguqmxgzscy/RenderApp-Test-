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

#include"../glm/glm.hpp"
#include"../glm/gtc/matrix_transform.hpp"
#include"../glm/gtc/type_ptr.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include "../imgui/imgui_internal.h"
#include "../imgui/ImGuizmo.h"
#include "../imgui/ImFileDialog.h"

float camDistance = 8.f;
std::string skybox_dialogs[6] =
{
    "SkyboxTopImageOpenDialog",
    "SkyboxLeftImageOpenDialog",
    "SkyboxFrontImageOpenDialog",
    "SkyboxRightImageOpenDialog",
    "SkyboxBackImageOpenDialog",
    "SkyboxBottomImageOpenDialog"
};

struct WindowResources
{
    unsigned int trans_icon;
    unsigned int rotate_icon;
    unsigned int scale_icon;
    unsigned int camera_icon;
    unsigned int light_icon;
    unsigned int skybox_icon;

    std::vector<std::string> skybox_faces
    {
        "Resources/skybox/right.jpg",
        "Resources/skybox/left.jpg",
        "Resources/skybox/top.jpg",
        "Resources/skybox/bottom.jpg",
        "Resources/skybox/front.jpg",
        "Resources/skybox/back.jpg"
    };

    unsigned int skybox_textures[6];
};

class RenderApp
{

private:
    GLFWwindow* window;
    WindowResources* resources;
    LightManager* lightManager;

    std::vector<RenderItem*> items;
    Camera* cur_camera;
    Shader* default_shader;
    RenderItem* cur_item;
    RenderItem* empty_item;
    //Skybox
    Shader* skybox_shader;
    size_t skyboxVAO, skyboxVBO, skyboxTextureID;
    //Custom Framebuffer
    size_t m_fbo, m_texture;
    //3D Gizmo Manipulation
    ImGuizmo::OPERATION mCurrentGizmoOperation;
    ImGuizmo::MODE mCurrentGizmoMode;
    //Module Window Flag
    bool camera_window_open = false;
    bool light_window_open = false;
    bool skybox_window_open = false;
    bool isSkyboxOn = false;
    bool isLightOn = true;
    bool isLightCastersOn[3] = { true,false,false };
    bool shouldLookAtCurItem = false;

public:
    int SCR_WIDTH = 1920;
    int SCR_HEIGHT = 1080;
    float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;

    bool firstMouse = true;
    bool hiddenMosue = false;

    bool isPressed = false;

    float deltaTime = 0.0f; // 当前帧与上一帧的时间差
    float lastFrame = 0.0f; // 上一帧的时间

public:
    RenderApp();
    ~RenderApp();
    int Run();
    int Init();

public:
    void ProcessInput(GLFWwindow* window);
    void SetGUI();
    void SetViewport();
    void DrawObjectWindow();
    void DrawMainMenuWindow();
    void DrawToolWindow();
private:
    void CreateFBO(int width, int height);
    void DrawCameraToolWindow();
    void DrawLightToolWindow();
    void DrawSkyboxToolWindow();
    //OpenGL DrawCallback
    void draw_callback(GLFWwindow* window);
public:
    GLFWwindow* getWindow() { return this->window; }
    Camera* getCamera() { return this->cur_camera; }
    std::vector<RenderItem*> getItems() { return this->items; }
    RenderItem* getCurRenderItem() { return this->cur_item; }
    ImVec2 viewport_pos, viewport_size;
};

RenderApp::RenderApp()
{

    resources = new WindowResources();
    lightManager = new LightManager();
    
    if (!RenderApp::Init()) {
        std::cout << "Init Failure!" << std::endl;
    }

    cur_camera = new Camera();
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

    resources->trans_icon = LoadTexture("Resources/Icon/trans_icon.png");
    resources->rotate_icon = LoadTexture("Resources/Icon/rotate_icon.png");
    resources->scale_icon = LoadTexture("Resources/Icon/scale_icon.png");
    resources->camera_icon = LoadTexture("Resources/Icon/camera_icon.png");
    resources->light_icon = LoadTexture("Resources/Icon/light_icon.png");
    resources->skybox_icon = LoadTexture("Resources/Icon/skybox_icon.png");
    
    //Load texture for each face
    for (size_t i = 0; i < 6; i++)
        resources->skybox_textures[i] = LoadTexture(resources->skybox_faces[i].c_str());
    

    CreateFBO(SCR_WIDTH,SCR_HEIGHT);
}
int RenderApp::Run() {

    items.push_back(new RenderItem("Resources/Nanosuit/nanosuit.obj"));
    items.push_back(new RenderItem());
    items.push_back(new RenderItem());  
    //skybox shader,VAO,VBO
    skybox_shader = new Shader("Shaders/skybox.vert", "Shaders/skybox.frag");
    skybox_shader->use();
    skybox_shader->setInt("skybox", 0);
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //Default shader
    default_shader = new Shader("Shaders/model.vert", "Shaders/model.frag");
    default_shader->use();
    default_shader->setFloat("material.shininess", 32.f);
    default_shader->setDirLight(*(lightManager->dirLight));
    default_shader->setPointLight(*(lightManager->pointLight));
    items[0]->setShader(default_shader);
    //skybox texture
    skyboxTextureID = loadCubemap(resources->skybox_faces);
    ImGuiIO& io = ImGui::GetIO();

    // ImFileDialog requires you to set the CreateTexture and DeleteTexture
    ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return (void*)tex;
    };
    ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
        GLuint texID = (GLuint)((uintptr_t)tex);
        glDeleteTextures(1, &texID);
    };


    //MainLoop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //处理输入，清除颜色缓存和深度缓存
        ProcessInput(window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();

        DrawMainMenuWindow();
        DrawToolWindow();
        DrawObjectWindow();
        SetViewport();

        if (camera_window_open)
        {
            ImGui::SetNextWindowSize(ImVec2(600, 600));
            DrawCameraToolWindow();
        }

        if (light_window_open)
        {
            ImGui::SetNextWindowSize(ImVec2(800, 800));
            DrawLightToolWindow();
        }

        // file dialogs
        if (ifd::FileDialog::Instance().IsDone("ShaderOpenDialog")) {
            if (ifd::FileDialog::Instance().HasResult()) {
                const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
                for (const auto& r : res) // ShaderOpenDialog supports multiselection
                    printf("OPEN[%s]\n", r.u8string().c_str());
            }
            ifd::FileDialog::Instance().Close();
        }

        if (skybox_window_open)
        {
            DrawSkyboxToolWindow();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        draw_callback(window);
       
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();  

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteFramebuffers(1, &m_fbo);

    glfwTerminate();
    return 0;
}

void RenderApp::ProcessInput(GLFWwindow* window)
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

void RenderApp::SetGUI() {
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

void RenderApp::SetViewport() {

#pragma region Init Some Variables
    ImGuiIO& io = ImGui::GetIO();
    static ImGuiWindowFlags gizmoWindowFlags = 0;
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;

    mCurrentGizmoMode = ImGuizmo::WORLD;
#pragma endregion
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(0, 0);
    ImGui::Begin("viewport_window", 0, gizmoWindowFlags);
#pragma region ViewportSetting

    ImGuizmo::SetDrawlist();
    float windowWidth = (float)ImGui::GetWindowWidth();
    float windowHeight = (float)ImGui::GetWindowHeight();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

    viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
    viewManipulateTop = ImGui::GetWindowPos().y;
    ImGuiWindow* viewport_window = ImGui::GetCurrentWindow();
    gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(viewport_window->InnerRect.Min, viewport_window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

#pragma endregion
    auto camera_view = (float*)glm::value_ptr(cur_camera->getViewMatrix());
    auto camera_projection = (float*)glm::value_ptr(cur_camera->getProjectionMatrix());

    float p_min_x = viewport_window->InnerRect.Max.x;
    float p_min_y = viewport_window->InnerRect.Max.y;
    float p_max_x = viewport_window->InnerRect.Min.x;
    float p_max_y = viewport_window->InnerRect.Min.y;
    //ImGuizmo::DrawGrid(camera_view, camera_projection, glm::value_ptr(glm::mat4(1.0f)), 100.f);
    ImGui::GetWindowDrawList()->AddImage((void*)m_texture, ImVec2(p_max_x, p_min_y), ImVec2(p_min_x, p_max_y));

    if (cur_item) 
    {
        //Draw 3D Gizmo
        ImGuizmo::Manipulate(camera_view, camera_projection, mCurrentGizmoOperation, mCurrentGizmoMode, 
            (float*)glm::value_ptr(cur_item->transform.getModelMat()), NULL, NULL, NULL, NULL);
        //Draw Manipulate View(3D Cube)
        ImGuizmo::ViewManipulate(camera_view, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
    }  
    ImGui::End();
    style.WindowPadding = ImVec2(6, 6);
}

void RenderApp::DrawObjectWindow() {
    //Detail Window
    {
        ImGui::Begin("Detail");
        //More Detail
        if (cur_item)
        {
            Transform& trans = cur_item->transform;
            float pos[3], eular[3], scale[3];
            ImGuizmo::DecomposeMatrixToComponents((float*)glm::value_ptr(trans.getModelMat()),
                pos, eular, scale);

            ImGui::Text("Position"); 
            {
                ImGui::PushItemWidth(130.0f);
                ImGui::InputFloat("P.x", &pos[0], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
                ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
                ImGui::InputFloat("P.y", &pos[1], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
                ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
                ImGui::InputFloat("P.z", &pos[2], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
            }

            ImGui::NewLine();
            ImGui::Text("Rotation");
            {
                ImGui::InputFloat("R.x", &eular[0], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
                ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
                ImGui::InputFloat("R.y", &eular[1], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
                ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
                ImGui::InputFloat("R.z", &eular[2], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
            }

            ImGui::NewLine();
            ImGui::Text("Scale");
            {
                ImGui::InputFloat("S.x", &scale[0], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
                ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
                ImGui::InputFloat("S.y", &scale[1], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
                ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
                ImGui::InputFloat("S.z", &scale[2], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
                ImGui::PopItemWidth();

            }

            trans.setNewPosition(glm::vec3(pos[0], pos[1], pos[2]));
            trans.setNewRotation(glm::vec3(eular[0], eular[1], eular[2]));
            trans.setNewScale(glm::vec3(scale[0], scale[1], scale[2]));
            cur_item->updateSelfAndChild();

            ImGui::NewLine(); ImGui::NewLine();
            ImGui::Checkbox("Disabled", cur_item->getDisabled());
        }
        else
        {
            float pos[3], eular[3], scale[3];
            ImGuizmo::DecomposeMatrixToComponents((float*)glm::value_ptr(empty_item->transform.getModelMat()),
                pos, eular, scale);

            ImGui::Text("Position");
            ImGui::PushItemWidth(130.0f);
            ImGui::InputFloat("P.x", &pos[0], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
            ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
            ImGui::InputFloat("P.y", &pos[1], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
            ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
            ImGui::InputFloat("P.z", &pos[2], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();

            ImGui::NewLine();
            ImGui::Text("Rotation");
            ImGui::InputFloat("R.x", &eular[0], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
            ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
            ImGui::InputFloat("R.y", &eular[1], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();
            ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
            ImGui::InputFloat("R.z", &eular[2], 1.0f, 0.1f, "%.1f"); ImGui::SameLine();

            ImGui::NewLine();
            ImGui::Text("Scale");
            ImGui::InputFloat("S.x", &scale[0], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
            ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
            ImGui::InputFloat("S.y", &scale[1], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
            ImGui::Dummy(ImVec2(4.0f, 0.0f)); ImGui::SameLine();
            ImGui::InputFloat("S.z", &scale[2], 0.1f, 0.1f, "%.1f"); ImGui::SameLine();
            ImGui::PopItemWidth();
        }
        ImGui::NewLine();
        //Draw Mode
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
    }

    //ObjectLists Window
    {
        //Create a window
        ImGui::Begin(u8"物体列表", 0, ImGuiWindowFlags_NoCollapse);
        //Get RenderItems
        auto items = this->getItems();
        //Set Style
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        //Draw ObjectLists
        for (size_t i = 0; i < items.size(); i++) 
        {
            ImGui::AlignTextToFramePadding();
            float h = ImGui::GetTextLineHeight();
            ImGui::Dummy(ImVec2(h, h));
            ImGui::SameLine();
            unsigned int textureId = LoadTexture("Resources/Icon/object.png");
            //Draw Object Icon
            ImGui::Image((void*)textureId, ImVec2(h, h));
            ImGui::SameLine();
            //Draw Object[i]
            bool expanded = ImGui::TreeNodeEx((void*)i, 0, "%s", items[i]->getName().c_str());
            //按下鼠标右键后且悬停到对应的结点后
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::BeginMenu("Option 1")) {
                    std::string str = std::to_string(i);
                    ImGui::MenuItem(str.c_str());
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Option 2")) ImGui::EndMenu();
                ImGui::EndPopup();
            }
            //该结点若展开
            if (expanded && items[i]->children.empty()) {
                cur_item = items[i];
                ImGui::TreePop();
            }
        }
        ImGui::PopStyleVar();
        ImGui::End();
    }

}

void RenderApp::DrawMainMenuWindow()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("(demo menu)", NULL, false, false);
            if (ImGui::MenuItem("New")) {}
            if (ImGui::MenuItem("Open", "Ctrl+O")) {}
            if (ImGui::BeginMenu("Open Recent"))
            {
                ImGui::MenuItem("fish_hat.c");
                ImGui::MenuItem("fish_hat.inl");
                ImGui::MenuItem("fish_hat.h");
                if (ImGui::BeginMenu("More.."))
                {
                    ImGui::MenuItem("Hello");
                    ImGui::MenuItem("Sailor");
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Save As..")) {}

            ImGui::Separator();
            if (ImGui::BeginMenu("Options"))
            {
                static bool enabled = true;
                ImGui::MenuItem("Enabled", "", &enabled);
                ImGui::BeginChild("child", ImVec2(0, 60), true);
                for (int i = 0; i < 10; i++)
                    ImGui::Text("Scrolling Text %d", i);
                ImGui::EndChild();
                static float f = 0.5f;
                static int n = 0;
                ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
                ImGui::InputFloat("Input", &f, 0.1f);
                ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Colors"))
            {
                float sz = ImGui::GetTextLineHeight();
                for (int i = 0; i < ImGuiCol_COUNT; i++)
                {
                    const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
                    ImVec2 p = ImGui::GetCursorScreenPos();
                    ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
                    ImGui::Dummy(ImVec2(sz, sz));
                    ImGui::SameLine();
                    ImGui::MenuItem(name);
                }
                ImGui::EndMenu();
            }

            // Here we demonstrate appending again to the "Options" menu (which we already created above)
            // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
            // In a real code-base using it would make senses to use this feature from very different code locations.
            if (ImGui::BeginMenu("Options")) // <-- Append!
            {
                static bool b = true;
                ImGui::Checkbox("SomeOption", &b);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Disabled", false)) // Disabled
            {
                IM_ASSERT(0);
            }
            if (ImGui::MenuItem("Checked", NULL, true)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Alt+F4")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }     
}

void RenderApp::DrawToolWindow()
{
    //Tool Window
    ImGui::Begin("Tool Window", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
    float half_h = ImGui::GetWindowHeight() / 2;
    float quater_h = ImGui::GetWindowWidth() / 4;
    auto flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody
        | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_NoHostExtendX;
    ImVec2 item_size = ImVec2(50, 50);

    //Draw Transformation Icons
    ImGui::Dummy(ImVec2(item_size.x * 2, item_size.y));
    ImGui::SameLine();
    if (ImGui::BeginTable("Transformation_Table", 5, flags, ImVec2(item_size.x * 5, item_size.y)))
    {
        //move icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->trans_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            //if pressed, no bgcolor
            if (!isPressed)
            {
                ImGui::SetTooltip("Move");
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
            }
            //if clicked, change gizmoOperation
            if (ImGui::IsItemClicked()) 
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        }
        //fill the gap between moveIcon and rotateIcon
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //rotate icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->rotate_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            if (!isPressed)
            {
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                ImGui::SetTooltip("Rotate");
            }
            if (ImGui::IsItemClicked())
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
        }
        //fill the gap between rotateIcon and scaleIcon
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //scale icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->scale_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            if (!isPressed)
            {
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                ImGui::SetTooltip("Scale");
            }
            if (ImGui::IsItemClicked())
                mCurrentGizmoOperation = ImGuizmo::SCALE;   
        }

        ImGui::EndTable();
    }
    ImGui::SameLine();


    //Draw Other Icons
    flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_ContextMenuInBody
        | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Borders;
    ImGui::Dummy(ImVec2(quater_h, 50));
    ImGui::SameLine();
    if (ImGui::BeginTable("Other_Icons Table", 6, flags, ImVec2(item_size.x * 10, item_size.y)))
    {
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //Camera Icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->camera_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            //if pressed, no bgcolor
            if (!isPressed)
            {
                ImGui::SetTooltip("Camera");
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
            }
            //if clicked, show camera window
            if (ImGui::IsItemClicked())
                camera_window_open = true;


        }
        //fill the gap between cameraIcon and lightIcon
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //Lgiht Icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->light_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            if (!isPressed)
            {
                ImGui::SetTooltip("Light");
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
            }
            if (ImGui::IsItemClicked())
                light_window_open = true;
        }
        //fill the gap between lightIcon and scaleIcon
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //Skybox Icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->skybox_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            if (!isPressed)
            {
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                ImGui::SetTooltip("Skybox");
            }
            if (ImGui::IsItemClicked())
                skybox_window_open = true;
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

inline void RenderApp::DrawCameraToolWindow()
{

    if (ImGui::Begin("Camera",&camera_window_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size);
        ImGui::SameLine();
        ImGui::Text("Camera Attribute");
        ImGui::Checkbox("Should Look At Current Item?", &shouldLookAtCurItem);
        if (!shouldLookAtCurItem)
        {
            cur_camera->At = glm::vec3(0, 0, 0);
            ImGui::InputFloat3("Look At", glm::value_ptr(cur_camera->At), "%.1f");
        }
        else if (cur_item && shouldLookAtCurItem)
            cur_camera->At = cur_item->transform.getLocalPostion();
        ImGui::InputFloat("Radius", &cur_camera->Distance, 0.1f, 0.01f, "%.2f");
        ImGui::End();
    }
}

inline void RenderApp::DrawLightToolWindow()
{

    if (ImGui::Begin("Light", &light_window_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size);
        ImGui::SameLine();
        ImGui::Text("Light On/Off");
        ImGui::SameLine();
        ImGui::ToggleButton("toggle",&isLightOn);
        //Light Setting
        if (isLightOn)
        {
            if (ImGui::CollapsingHeader("DirLight Atrribute")) 
            {
                ImGui::Dummy(fill_size);
                ImGui::SameLine();
                ImGui::Text("Directional Light On");
                ImGui::SameLine();
                ImGui::ToggleButton("DirLight Toggle", &isLightCastersOn[0]);
                //Attribute Panel
                if (isLightCastersOn[0])
                {
                    ImGui::InputFloat3("DirLight Direction", glm::value_ptr(lightManager->dirLight->direction), "%.1f");
                    ImGui::ColorEdit3("DirLight Ambient",glm::value_ptr(lightManager->dirLight->ambient));
                    ImGui::ColorEdit3("DirLight Diffuse", glm::value_ptr(lightManager->dirLight->diffuse));
                    ImGui::ColorEdit3("DirLight Specular", glm::value_ptr(lightManager->dirLight->specular));
                    //Change the uniform value in default_shader
                    default_shader->use();
                    default_shader->setDirLight(*(lightManager->dirLight));
                }

            }
            if (ImGui::CollapsingHeader("PointLight Atrribute"))
            {
                ImGui::Dummy(fill_size);
                ImGui::SameLine();
                ImGui::Text("Point Light On");
                ImGui::SameLine();
                ImGui::ToggleButton("PointLight Toggle", &isLightCastersOn[1]);

                if (isLightCastersOn[1])
                {
                    ImGui::InputFloat("PointLight Constant", &lightManager->pointLight->constant, 0.01f, 0.01f, "%.3f");
                    ImGui::InputFloat("PointLight Quadratic", &lightManager->pointLight->quadratic, 0.01f, 0.01f, "%.3f");
                    ImGui::InputFloat("PointLight Linear", &lightManager->pointLight->linear, 0.01f, 0.01f, "%.3f");
                    ImGui::InputFloat3("PointLight Position", glm::value_ptr(lightManager->pointLight->position), "%.1f");
                    ImGui::ColorEdit3("PointLight Ambient", glm::value_ptr(lightManager->pointLight->ambient));
                    ImGui::ColorEdit3("PointLight Diffuse", glm::value_ptr(lightManager->pointLight->diffuse));
                    ImGui::ColorEdit3("PointLight Specular", glm::value_ptr(lightManager->pointLight->specular));

                    default_shader->use();
                    default_shader->setPointLight(*(lightManager->pointLight));
                }
            }
            if (ImGui::CollapsingHeader("SpotLight Atrribute"))
            {
                ImGui::Dummy(fill_size);
                ImGui::SameLine();
                ImGui::Text("Spot Light On");
                ImGui::SameLine();
                ImGui::ToggleButton("SpotLight Toggle", &isLightCastersOn[2]);

                if (isLightCastersOn[2])
                {
                    ImGui::InputFloat("SpotLight Constant", &lightManager->spotLight->constant, 0.01f, 0.01f, "%.3f");
                    ImGui::InputFloat("SpotLight InnerCutOut", &lightManager->spotLight->innerCutOut, 0.01f, 0.01f, "%.3f");
                    ImGui::InputFloat("SpotLight OutterCutOut", &lightManager->spotLight->outterCutOut, 0.01f, 0.01f, "%.3f");
                    ImGui::InputFloat("SpotLight Quadratic", &lightManager->spotLight->quadratic, 0.01f, 0.01f, "%.3f");
                    ImGui::InputFloat("SpotLight Linear", &lightManager->spotLight->linear, 0.01f, 0.01f, "%.3f");

                    ImGui::InputFloat3("SpotLight Position", glm::value_ptr(lightManager->spotLight->position), "%.1f");
                    ImGui::InputFloat3("SpotLight Direction", glm::value_ptr(lightManager->spotLight->direction), "%.1f");

                    ImGui::ColorEdit3("SpotLight Ambient", glm::value_ptr(lightManager->spotLight->ambient));
                    ImGui::ColorEdit3("SpotLight Diffuse", glm::value_ptr(lightManager->spotLight->diffuse));
                    ImGui::ColorEdit3("SpotLight Specular", glm::value_ptr(lightManager->spotLight->specular));
                }
            }
        }
        ImGui::End();
    }
}

inline void RenderApp::DrawSkyboxToolWindow()
{
    if (ImGui::Begin("Skybox", &skybox_window_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size);
        ImGui::SameLine();
        ImGui::Text("Skybox On/Off");
        ImGui::SameLine();
        ImGui::ToggleButton("Skybox Toggle", &isSkyboxOn);
        if (isSkyboxOn)
        {
            float w = ImGui::GetWindowWidth() / 4;
            auto flags = ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_NoHostExtendX |
                ImGuiTableFlags_SizingStretchSame;
            //Draw SkyboxTextures Region
            if (ImGui::CollapsingHeader("Skybox Preview"))
            {
                if (ImGui::BeginTable("Skybox Preview Table", 4, flags, ImVec2(w * 4, w * 3)))
                {
                    //The First Row
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Image((void*)resources->skybox_textures[2], ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    //The Second Row
                    ImGui::TableNextColumn(); ImGui::Image((void*)resources->skybox_textures[1], ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Image((void*)resources->skybox_textures[4], ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Image((void*)resources->skybox_textures[0], ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Image((void*)resources->skybox_textures[5], ImVec2(w, w));
                    //The Third Row
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Image((void*)resources->skybox_textures[3], ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::EndTable();
                }
            }

            if (ImGui::CollapsingHeader("Skybox Setting"))
            {
                if (ImGui::BeginTable("Skybox Setting Table", 4, flags, ImVec2(w * 4, w * 3)))
                {
                    //The First Row
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Top", ImVec2(w, w)))
                        ifd::FileDialog::Instance().Open(skybox_dialogs[2], "Open A Top Image",
                            "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    //The Second Row
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Left", ImVec2(w, w)))
                        ifd::FileDialog::Instance().Open(skybox_dialogs[1], "Open A Left Image",
                            "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Front", ImVec2(w, w)))
                        ifd::FileDialog::Instance().Open(skybox_dialogs[4], "Open A Front Image",
                            "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Right", ImVec2(w, w)))
                        ifd::FileDialog::Instance().Open(skybox_dialogs[0], "Open A Right Image",
                            "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Back", ImVec2(w, w)))
                        ifd::FileDialog::Instance().Open(skybox_dialogs[5], "Open A Back Image",
                            "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
                    //The Third Row
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::TableNextColumn();
                    if (ImGui::Button("Bottom", ImVec2(w, w)))
                        ifd::FileDialog::Instance().Open(skybox_dialogs[3], "Open A Bottom Image",
                            "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::TableNextColumn(); ImGui::Dummy(ImVec2(w, w));
                    ImGui::EndTable();
                }
            }
            // file dialogs
            for (size_t i = 0; i < 6; i++)
                if (ifd::FileDialog::Instance().IsDone(skybox_dialogs[i])) {
                    if (ifd::FileDialog::Instance().HasResult()) {
                        const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
                        //old pic will be replaced with new pic
                        if (res.size() == 1)
                        {
                            resources->skybox_faces[i] = res[0].u8string();
                            resources->skybox_textures[i] = LoadTexture(resources->skybox_faces[i].c_str());
                            glDeleteTextures(1, &skyboxTextureID);
                            skyboxTextureID = loadCubemap(resources->skybox_faces);
                        }
                    }
                    ifd::FileDialog::Instance().Close();
                }

        }
        ImGui::End();
    }
}

inline void RenderApp::CreateFBO(int width, int height)
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    //create a color attachment texture 
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
    //create a renderbuffer object for depth and stencil attachment
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    //now that we actually create the freambuffer and added all attachements we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is no complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void RenderApp::draw_callback(GLFWwindow* window)
{
    // bind to custom framebuffer and draw scene as we normally would to color texture 
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //向shader传送所有的需要的uniform变量资源
    glm::mat4 projection = cur_camera->getProjectionMatrix();
    glm::mat4 view = cur_camera->getViewMatrix();

    items[0]->getShader()->use();
    items[0]->getShader()->setMat4("projection", projection);
    items[0]->getShader()->setMat4("view", view);
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
    //Skybox
    if (isSkyboxOn)
    {
        // change depth function so depth test passes when values are equal to depth buffer's content
        glDepthFunc(GL_EQUAL);
        skybox_shader->use();
        //remove translation from view matrix
        view = glm::mat4(glm::mat3(cur_camera->getViewMatrix()));
        skybox_shader->setMat4("view", view);
        skybox_shader->setMat4("projection", projection);
        //skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

}



