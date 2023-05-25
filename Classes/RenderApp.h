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
#include "../imgui/TextEditor.h"
#include "Utility.h"

float camDistance = 8.f;

class RenderApp
{
private:
    GLFWwindow* window;
    TextEditor editor;

    WindowResources* resources;
    WindowFlags* windowFlags;
    LightManager* lightManager;
    ShaderManager* shaderManager;
    GeometryGenerator* generator;

    PickingTexture* pickingTexture;
    LeftMouse* leftMouse;

    std::vector<RenderItem*> items;
    RenderItem* cur_item = nullptr;
    RenderItem* empty_item;
    Camera* cur_camera;

    //Skybox VAO,VBO and TextureID
    size_t skyboxVAO, skyboxVBO, skyboxTextureID;
    //ScreenTexture VAO,VBO
    size_t  screenVAO, screenVBO;
    size_t planeVAO, planeVBO;
    //Effecs Framebuffer and texture
    size_t screenFbo, screenTexture;
    //Custom Framebuffer and texture
    size_t m_fbo, m_texture;
    //MSAA Framebuffer and texture
    size_t m_fbo_msaa, m_texture_msaa;
    //PBR Textures
    size_t irradianceMap, prefilterMap, brdfLUTTexture, envCubemap, hdrTexture;
    //3D Gizmo Manipulation
    ImGuizmo::OPERATION mCurrentGizmoOperation;
    ImGuizmo::MODE mCurrentGizmoMode;
public:
    int SCR_WIDTH = 1920;
    int SCR_HEIGHT = 1080;
    float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;

    bool firstMouse = true;
    bool hiddenMosue = false;

    float deltaTime = 0.0f; // 当前帧与上一帧的时间差
    float lastFrame = 0.0f; // 上一帧的时间
public:
    RenderApp();
    ~RenderApp();
    int Run();
    void DrawUI(ImGuiIO& io);
    int Init();
public:
    void ProcessInput(GLFWwindow* window);
    void SetGUI();
    void SetViewport();
    void DrawObjectWindow();
    void DrawMainMenuWindow();
    void DrawToolWindow();
private:
    void LoadPBREnvMap(const char* path);
    void DrawCameraToolWindow();
    void DrawLightToolWindow();
    void DrawSkyboxToolWindow();
    void DrawEffectToolWindow();
    void ShowEditorWindow();
    void ShowExportWindow();
    void ShowGeometryCreateWindow();
    void ShowBlingPhongMaterial(int cur_index, Material* material, std::vector<Mesh>& meshes);
    void ProcessBlingPhongDialog(Material* material, std::vector<Texture>& textures);
    void ProcessPBRDialog(Material* material, std::vector<Texture>& textures);
    void ShowPBRMaterial(int cur_index, Material* material, std::vector<Mesh>& meshes);
private:
    void CreateFBO(int width, int height);
    void CreateMSAAFBO(int width, int height);
    void SaveImage(const char* path, int ex_width, int ex_height);
    //OpenGL DrawCallback
    void DrawGridLine(Mesh& mesh);
    void draw_callback(GLFWwindow* window);
public:
    GLFWwindow* getWindow() { return this->window; }
    Camera* getCamera() { return this->cur_camera; }
    LeftMouse* getLeftMouse() { return this->leftMouse; }
    RenderItem* getCurRenderItem() { return this->cur_item; }
    WindowFlags* getWindowFlags() { return this->windowFlags; }
    ImVec2 viewport_pos, viewport_size;
};

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
    //Load texture for each icon
    resources->trans_icon = LoadTexture("Resources/Icon/trans_icon.png");
    resources->rotate_icon = LoadTexture("Resources/Icon/rotate_icon.png");
    resources->scale_icon = LoadTexture("Resources/Icon/scale_icon.png");
    resources->camera_icon = LoadTexture("Resources/Icon/camera_icon.png");
    resources->light_icon = LoadTexture("Resources/Icon/light_icon.png");
    resources->skybox_icon = LoadTexture("Resources/Icon/skybox_icon.png");  
    resources->effect_icon = LoadTexture("Resources/Icon/effect_icon.png");
    resources->shader_icon = LoadTexture("Resources/Icon/shader_icon.png");
    resources->object_icon = LoadTexture("Resources/Icon/object.png");
    //Load texture for each face
    for (size_t i = 0; i < 6; i++)
        resources->skybox_textures[i] = LoadTexture(resources->skybox_faces[i].c_str());
    //Init picking texture
    pickingTexture->Init(SCR_WIDTH, SCR_HEIGHT);
    //Init custom framebuffer
    CreateFBO(SCR_WIDTH,SCR_HEIGHT);
    CreateMSAAFBO(SCR_WIDTH, SCR_HEIGHT);
}

int RenderApp::Run() {

#pragma region Init Some Varaibles
    // RenderItem PBR Material
    //items.push_back(new RenderItem());
    //Mesh mesh;
    //mesh.GetDataFrom(generator->CreateSphere(2, 32, 32));
    //delete mesh.material;
    //mesh.material = new PBRMaterial();
    //mesh.material->m_shader = &shaderManager->pbr_shader;
    //auto m = static_cast<PBRMaterial*>(mesh.material);
    //items[0]->setModel(&mesh);
    //GridLineData
    gridMesh.GetDataFrom(generator->CreateGridLine(24, 24, 12, 12));
    gridMesh.material->m_shader = new Shader("Shaders/gridline.vert", "Shaders/gridline.frag");
    //skybox shader,VAO,VBO
    shaderManager->skybox_shader.use();
    shaderManager->skybox_shader.setInt("skybox", 0);
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //ScreenTexture VAO,VBO
    shaderManager->inversion_shader.setUint("ScreenTexture", 0);
    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LESS);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    //Default shader    
    shaderManager->default_shader.use();
    shaderManager->default_shader.setFloat("material.shininess", 32.f);
    shaderManager->default_shader.setDirLight(*(lightManager->dirLight));
    shaderManager->default_shader.setPointLight(*(lightManager->pointLight));
    //PBR shader
    shaderManager->pbr_shader.use();
    shaderManager->pbr_shader.setPBRPointLight(*(lightManager->pbrPointLight));
    shaderManager->pbr_shader.setInt("irradianceMap", 0);
    shaderManager->pbr_shader.setInt("prefilterMap", 1);
    shaderManager->pbr_shader.setInt("brdfLUT", 2);
    shaderManager->pbr_shader.setInt("material.albedoMap", 3);
    shaderManager->pbr_shader.setInt("material.aoMap", 4);
    shaderManager->pbr_shader.setInt("material.metallicMap", 5);
    shaderManager->pbr_shader.setInt("material.normalMap", 6);
    shaderManager->pbr_shader.setInt("material.roughnessMap", 7);

    //Background shader
    shaderManager->backgroundShader.use();
    shaderManager->backgroundShader.setInt("environmentMap", 0);

    //skybox texture
    skyboxTextureID = loadCubemap(resources->skybox_faces);
    ImGuiIO& io = ImGui::GetIO();
    //ShaderTextEditor
    auto lang = TextEditor::LanguageDefinition::GLSL();
    editor.SetLanguageDefinition(lang);
    editor.SetPalette(TextEditor::GetDarkPalette());
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 projection = cur_camera->getProjectionMatrix();
    shaderManager->pbr_shader.use();
    shaderManager->pbr_shader.setMat4("projection", projection);
    shaderManager->backgroundShader.use();
    shaderManager->backgroundShader.setMat4("projection", projection);

    LoadPBREnvMap("Resources/newport_loft.hdr");

#pragma endregion
   
    //MainLoop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //process inputs, and clear the color buffer and depth buffer
        ProcessInput(window);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        DrawUI(io);
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

void RenderApp::DrawUI(ImGuiIO& io)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();

    DrawMainMenuWindow();
    DrawToolWindow();
    DrawObjectWindow();
    SetViewport();

    if (windowFlags->camera_window_open)
    {
        ImGui::SetNextWindowSize(ImVec2(600, 600));
        DrawCameraToolWindow();
    }

    if (windowFlags->light_window_open)
    {
        ImGui::SetNextWindowSize(ImVec2(800, 800));
        DrawLightToolWindow();
    }

    if (windowFlags->skybox_window_open)
    {
        DrawSkyboxToolWindow();
    }

    if (windowFlags->effect_window_open)
    {
        DrawEffectToolWindow();
    }

    if (windowFlags->shader_window_open) 
    {
        ShowEditorWindow();
    }

    if (windowFlags->should_export_open)
    {
        ShowExportWindow();
    }

    if (windowFlags->should_geometry_open)
    {
        ShowGeometryCreateWindow();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

inline void RenderApp::CreateFBO(int width, int height)
{
    //off-screen framebuffer
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

    //ScreenTexture
    glGenFramebuffers(1, &screenFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFbo);
    //create a color attachment texture 
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
    //create a renderbuffer object for depth and stencil attachment
    unsigned int screen_rbo;
    glGenRenderbuffers(1, &screen_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screen_rbo);
    //now that we actually create the freambuffer and added all attachements we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is no complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void RenderApp::CreateMSAAFBO(int width, int height)
{
    glGenFramebuffers(1, &m_fbo_msaa);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_msaa);
    // create a multisampled color attachment texture
    glGenTextures(1, &m_texture_msaa);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_texture_msaa);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_texture_msaa, 0);
    // create a (also multisampled) renderbuffer object for depth and stencil attachments
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::MSAA_FRAMEBUFFER:: MSAA_Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void RenderApp::DrawGridLine(Mesh& mesh)
{
    mesh.material->m_shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    mesh.material->m_shader->setMVP(model,
        cur_camera->getViewMatrix(), cur_camera->getProjectionMatrix());
    glBindVertexArray(mesh.get_VAO());
    /*GL_POINTS、GL_LINE_STRIP、GL_LINE_LOOP、GL_LINES、GL_TRIANGLE_STRIP、GL_TRIANGLE_FAN、GL_TRIANGLES、GL_QUAD_STRIP、GL_QUADS和GL_POLYGON。*/
    glDrawElements(GL_LINES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

inline void RenderApp::draw_callback(GLFWwindow* window)
{
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    //向shader传送所有的需要的uniform变量资源
    glm::mat4 projection = cur_camera->getProjectionMatrix();
    glm::mat4 view = cur_camera->getViewMatrix();

    //bind to picking texture and store index information
    pickingTexture->EnableWriting();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
    for (size_t i = 0; i < items.size(); i++)
    {
        //第i个模型未被禁用
        if (!items[i]->isDisabled())  
            for (size_t j = 0; j < items[i]->getModel().getMeshes().size(); j++)
            {
                Material& material = *(items[i]->getModel().getMeshes()[j].material);
                material.setShader(&shaderManager->picking_shader);
                material.m_shader->use();
                material.m_shader->setMVP(items[i]->transform.getModelMat(),
                    view, projection);
                material.m_shader->setUint("ModelIndex", i);
                material.m_shader->setUint("MeshSize", items[i]->getModel().getMeshes().size());
                items[i]->getModel().getMeshes()[j].Draw_PickingEffects(j);
            }     
    }
    pickingTexture->DisableWriting();
    // bind to custom framebuffer and draw scene as we normally would to color texture 
    if (windowFlags->isMSAAOn)
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_msaa);
    else
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!windowFlags->shouldExport)
        DrawGridLine(gridMesh);
    //Draw Object
    for (unsigned int i = 0; i < items.size(); i++)
    {
        if (!items[i]->isDisabled())
            for (size_t j = 0; j < items[i]->getModel().getMeshes().size(); j++)
            {
                Material& material = *(items[i]->getModel().getMeshes()[j].material);
                auto& pbr_texs = items[i]->getModel().getMeshes()[j].textures;
                if (material.type == BlingPhong)
                {
                    material.setShader(&shaderManager->default_shader);
                    material.m_shader->use();
                    material.m_shader->setDirLight(*(lightManager->dirLight));
                    material.m_shader->setPointLight(*(lightManager->pointLight));
                }
                else if (material.type == PBR)
                {
                    material.setShader(&shaderManager->pbr_shader);
                    PBRMaterial* m = static_cast<PBRMaterial*>(&material);
                    material.m_shader->use();
                    // bind pre-computed IBL data
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
                    material.m_shader->setPBRPointLight(*(lightManager->pbrPointLight));
                }
                material.m_shader->setMVP(items[i]->transform.getModelMat(), 
                    view, projection);
                material.m_shader->setVec3("viewPos", cur_camera->Position);
                items[i]->getModel().getMeshes()[j].Draw();
            }
    }
 
    //Skybox 
    if (windowFlags->isSkyboxOn)
    {
        glDisable(GL_CULL_FACE);
        //// render skybox (render as last to prevent overdraw)
        glDepthFunc(GL_LEQUAL);
        shaderManager->backgroundShader.use();
        shaderManager->backgroundShader.setMat4("view", view);
        shaderManager->backgroundShader.setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap); // display irradiance map
        //glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map
        //glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderCube();
        glDepthFunc(GL_LESS);
        //Bling-Phong Disabled
        //// change depth function so depth test passes when values are equal to depth buffer's content
        //glDepthFunc(GL_LEQUAL);
        //shaderManager->skybox_shader.use();
        ////remove translation from view matrix
        //view = glm::mat4(glm::mat3(cur_camera->getViewMatrix()));
        //shaderManager->skybox_shader.setMat4("view", view);
        //shaderManager->skybox_shader.setMat4("projection", projection);
        ////skybox cube
        //glBindVertexArray(skyboxVAO);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0);
        //glDepthFunc(GL_LESS);
    }
    if (windowFlags->isMSAAOn)
    {
        //// 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_msaa);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //Post-Processing
    if (windowFlags->isEffectOn)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, screenFbo);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderManager->cur_effect_shader->use();
        glBindVertexArray(screenVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}

inline void RenderApp::SaveImage(const char* path, int ex_width = 1920, int ex_height = 1080) 
{
    //申请颜色数据内存
    unsigned char* readColorBuffer = new unsigned char[SCR_WIDTH * SCR_HEIGHT * 3];
    //读取显存中的像素（注意这里的格式是 BGR）
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, readColorBuffer);
    //更改内存中的图像大小
    stbir_resize(readColorBuffer, SCR_WIDTH, SCR_HEIGHT, 0, readColorBuffer, ex_width, ex_height, 
        0, STBIR_TYPE_UINT8, 3, STBIR_ALPHA_CHANNEL_NONE, 0,
        STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
        STBIR_FILTER_BOX, STBIR_FILTER_BOX,
        STBIR_COLORSPACE_SRGB, nullptr);
    //将数据写入文件
    stbi_flip_vertically_on_write(true);
    stbi_write_png(path, ex_width, ex_height, 3, (void*)readColorBuffer, 0);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //清理颜色数据内存
    delete[] readColorBuffer;

}

RenderApp::RenderApp()
{
    windowFlags = new WindowFlags();
    resources = new WindowResources();
    pickingTexture = new PickingTexture();
    leftMouse = new LeftMouse();

    if (!RenderApp::Init()) {
        std::cout << "Init Failure!" << std::endl;
    }

    lightManager = new LightManager();
    shaderManager = new ShaderManager();
    generator = new GeometryGenerator();

    cur_camera = new Camera();
    empty_item = new RenderItem();
}

RenderApp::~RenderApp()
{
}

void RenderApp::ProcessInput(GLFWwindow* window)
{
    if (windowFlags->isViewportHover)
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
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (leftMouse->isPressed && state == GLFW_RELEASE)
    {
        leftMouse->isOnClicked = true;
        leftMouse->isPressed = false;
    }
    else
        leftMouse->isOnClicked = false;
}

void RenderApp::ShowEditorWindow()
{
    ImGui::Begin("TextEditor", &windowFlags->shader_window_open, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl-S")) {
                if (windowFlags->shader_flag == VERTEX)
                {
                    auto textToSave = editor.GetText();
                    if (cur_item)
                    {
                        windowFlags->cur_shader->vertexCode = { textToSave };
                        windowFlags->cur_shader->ReCompile();
                        windowFlags->cur_shader->use();
                        windowFlags->cur_shader->setFloat("material.shininess", 32.f);
                        windowFlags->cur_shader->setDirLight(*(lightManager->dirLight));
                        windowFlags->cur_shader->setPointLight(*(lightManager->pointLight));
                    }

                }
                else if (windowFlags->shader_flag == FRAGMENT)
                {
                    auto textToSave = editor.GetText();
                    if (cur_item)
                    {
                        windowFlags->cur_shader->fragmentCode = { textToSave };
                        windowFlags->cur_shader->ReCompile();
                        windowFlags->cur_shader->use();
                        windowFlags->cur_shader->setFloat("material.shininess", 32.f);
                        windowFlags->cur_shader->setDirLight(*(lightManager->dirLight));
                        windowFlags->cur_shader->setPointLight(*(lightManager->pointLight));
                    }
                }
            }
            if (ImGui::MenuItem("Quit", "Alt-F4"))
                ;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl-Z", nullptr, editor.CanUndo()))
                editor.Undo();
            if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, editor.CanRedo()))
                editor.Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                editor.Copy();
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, editor.HasSelection()))
                editor.Cut();
            if (ImGui::MenuItem("Delete", "Del", nullptr, editor.HasSelection()))
                editor.Delete();
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, ImGui::GetClipboardText() != nullptr))
                editor.Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", nullptr, nullptr))
                editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Dark palette"))
                editor.SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light palette"))
                editor.SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro blue palette"))
                editor.SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    editor.Render("TextEditor");
    ImGui::End();
}

void RenderApp::SetGUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(NULL);
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.Fonts->AddFontFromFileTTF("Resources/Fonts/kai.ttf", 24, NULL, io.Fonts->GetGlyphRangesChineseFull());

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
    float windowPosX = (float)ImGui::GetWindowPos().x;
    float windowPosY = (float)ImGui::GetWindowPos().y;

    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

    viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
    viewManipulateTop = ImGui::GetWindowPos().y;
    ImGuiWindow* viewport_window = ImGui::GetCurrentWindow();
    gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(viewport_window->InnerRect.Min, viewport_window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
    auto camera_view = (float*)glm::value_ptr(cur_camera->getViewMatrix());
    auto camera_projection = (float*)glm::value_ptr(cur_camera->getProjectionMatrix());

    float p_min_x = viewport_window->InnerRect.Max.x;
    float p_min_y = viewport_window->InnerRect.Max.y;
    float p_max_x = viewport_window->InnerRect.Min.x;
    float p_max_y = viewport_window->InnerRect.Min.y;
#pragma endregion

    //ImGuizmo::DrawGrid(camera_view, camera_projection, glm::value_ptr(glm::mat4(1.0f)), 100.f);
    if (windowFlags->isEffectOn)
    {
        ImGui::GetWindowDrawList()->AddImage((void*)screenTexture, ImVec2(p_max_x, p_min_y), ImVec2(p_min_x, p_max_y));
    }
    else
        ImGui::GetWindowDrawList()->AddImage((void*)m_texture,
            ImVec2(p_max_x, p_min_y), ImVec2(p_min_x, p_max_y));

    if (cur_item)
    {
        //Draw 3D Gizmo
        ImGuizmo::Manipulate(camera_view, camera_projection, mCurrentGizmoOperation, mCurrentGizmoMode,
            (float*)glm::value_ptr(cur_item->transform.getModelMat()), NULL, NULL, NULL, NULL);
        //Draw Manipulate View(3D Cube)
        ImGuizmo::ViewManipulate(camera_view, camDistance,
            ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
    }
    windowFlags->isViewportHover = ImGui::IsWindowHovered();
    style.WindowPadding = ImVec2(6, 6);

    if (leftMouse->isOnClicked)
    {
        double scale_x = SCR_WIDTH / windowWidth;
        double scale_y = SCR_HEIGHT / windowHeight;
        double read_x, read_y;
        read_x = ImGui::GetMousePos().x - windowPosX;
        read_y = ImGui::GetMousePos().y - windowPosY;
        read_x *= scale_x;
        read_y *= scale_y;

        PickingTexture::PixelInfo Pixel = pickingTexture->ReadPixel(read_x, SCR_HEIGHT - read_y - 1);
        if (Pixel.PrimID != 0 && items.size())
            cur_item = items[Pixel.ObjectID];
    }

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
        {
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
        }
        ImGui::NewLine();
        //More Details 
        if (cur_item)
        {
            ImVec2 fill_size = ImVec2(24, 30);
            if (cur_item->hasRenderData())
            {
                //Model Info
                if (ImGui::CollapsingHeader("Model Info"))
                {
                    ImGui::Dummy(fill_size);
                    ImGui::SameLine();
                    ImGui::LabelText("", "Mesh Size :  %u",
                        cur_item->getModel().getMeshes().size());

                    ImGui::Dummy(fill_size);
                    ImGui::SameLine();
                    ImGui::LabelText("", "Vertex Size :  %u",
                        cur_item->getModel().getVertexSum());

                    ImGui::Dummy(fill_size);
                    ImGui::SameLine();
                    ImGui::LabelText("", "Texture Size :  %u",
                        cur_item->getModel().getTextureSum());

                    ImGui::Dummy(fill_size);
                    ImGui::SameLine();
                    ImGui::LabelText("", "Index Size :  %u",
                        cur_item->getModel().getIndexSum());
                }

                //Material Info
                if (ImGui::CollapsingHeader("Material Info"))
                {
                    std::vector<Mesh>& meshes = cur_item->getModel().getMeshes();
                    static size_t current_index = 0;
                    static int e = 0;
                    if (MESH_MAX_SIZE >= meshes.size())
                    {
                        static const char* current_item = number[0];
                        ImGui::Dummy(fill_size);
                        ImGui::SameLine(); ImGui::SetNextItemWidth(150.f);
                        if (ImGui::BeginCombo("##Meshes", current_item))
                        {
                            for (unsigned int i = 0; i < meshes.size(); i++)
                            {
                                bool is_selected = (current_item == number[i]);
                                if (ImGui::Selectable(number[i], is_selected))
                                    current_item = number[current_index = i];
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }

                        //Shading Mode
                        const char* shading_mode[] = { "Bling-Phong","PBR"};
                        static const char* current_mode = (meshes[current_index].material->type == PBR) ? shading_mode[1] : shading_mode[0];
                        static int shading_cur_index = 0;
                        ImGui::Dummy(ImVec2(fill_size)); ImGui::SameLine();
                        ImGui::Text("Shading Mode");ImGui::SameLine(); 
                        ImGui::SetNextItemWidth(150.f);
                        if (ImGui::BeginCombo("##shading_mode_combo", current_mode))
                        {
                            for (unsigned int i = 0; i < IM_ARRAYSIZE(shading_mode); i++)
                            {
                                bool is_selected = (current_mode == shading_mode[i]);
                                if (ImGui::Selectable(shading_mode[i], is_selected))
                                {
                                    current_mode = shading_mode[shading_cur_index = i];
                                    switch (shading_cur_index)
                                    {
                                    case 0:
                                    {
                                        delete  meshes[current_index].material;
                                        meshes[current_index].material = new BlingPhongMaterial();
                                        meshes[current_index].material->setShader(&shaderManager->default_shader);
                                    }; break;
                                    case 1:
                                    {
                                        delete meshes[current_index].material;
                                        meshes[current_index].material = new PBRMaterial();
                                        meshes[current_index].material->setShader(&shaderManager->pbr_shader);
                                    }; break;
                                    default:
                                        break;
                                    }
                                }
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                        Material* material = meshes[current_index].material;
                        //Show Material        
                        if (current_index > meshes.size())
                            current_index = 0, current_item = number[0];    
                        
                        if (material->type == BlingPhong)
                        {
                            current_mode = shading_mode[0];
                            ShowBlingPhongMaterial(current_index, material, meshes);
                        }

                        else if (material->type == PBR)
                        {
                            current_mode = shading_mode[1];
                            ShowPBRMaterial(current_index, material, meshes);
                        }
                    }
                    //Material Texture-Mapping
                    if (meshes[current_index].material->type == BlingPhong)
                        ProcessBlingPhongDialog(meshes[current_index].material, meshes[current_index].textures);
                    else if (meshes[current_index].material->type == PBR)
                        ProcessPBRDialog(meshes[current_index].material, meshes[current_index].textures);
                }
            }
        }
        ImGui::End();
    }

    //ObjectLists Window
    {
        //Create a window
        ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoCollapse);
        //Set Style
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        //Draw ObjectLists
        for (size_t i = 0; i < items.size(); i++)
        {
            ImGui::AlignTextToFramePadding();
            float h = ImGui::GetTextLineHeight();
            ImGui::Dummy(ImVec2(h, h));
            ImGui::SameLine();
            size_t flag = 0;
            //Draw Object Icon
            ImGui::Image((void*)resources->object_icon, ImVec2(h, h));
            ImGui::SameLine();
            if (cur_item && cur_item == items[i])
                flag = ImGuiTreeNodeFlags_Selected;
            //Draw Object[i]
            bool expanded = ImGui::TreeNodeEx((void*)i, flag, "%s", items[i]->getName().c_str());
            //该结点若展开
            if (expanded && items[i] && items[i]->children.empty())
            {
                cur_item = items[i];
                ImGui::TreePop();
            }
            //按下鼠标右键后且悬停到对应的结点后
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Copy")) {
                    //copy selected item
                    items.push_back(new RenderItem(items[i]));
                }
                if (ImGui::MenuItem("Delete"))
                {
                    cur_item = nullptr;
                    delete items[i];
                    items[i] = nullptr;
                    items.erase(items.begin() + i);                  
                }

                ImGui::EndPopup();
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
            if (ImGui::MenuItem("Export PNG")) // <-- Append!
                windowFlags->should_export_open = true;

            if (ImGui::MenuItem("Load Model"))
            {
                ifd::FileDialog::Instance().Open("ModelFileDialog", "Open A Model Image",
                    "Model file (*.obj;*.3ds;*.blend;*.FBX;){.obj,.3ds,.blend,.FBX},.*", true);

            }
            if (ImGui::BeginMenu("Disabled", false)) // Disabled
            {
                IM_ASSERT(0);
            }
            if (ImGui::MenuItem("Checked", NULL, true)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Alt+F4"))
                glfwSetWindowShouldClose(window, GLFW_TRUE);

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
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Geometry"))
                windowFlags->should_geometry_open = true;
            if (ImGui::MenuItem("Light"))
                ;
            if (ImGui::MenuItem("Camera"))
                ;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        ImGui::End();
        //Process FileDialog Result
        if (ifd::FileDialog::Instance().IsDone("ModelFileDialog")) {
            if (ifd::FileDialog::Instance().HasResult()) {
                const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
                //old pic will be replaced with new pic
                if (res.size() == 1)
                {
                    items.push_back(new RenderItem(res[0].u8string().c_str()));
                }
            }
            ifd::FileDialog::Instance().Close();
        }
        if (ifd::FileDialog::Instance().IsDone("SaveImageFileDialog")) {
            if (ifd::FileDialog::Instance().HasResult()) {
                const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
                //old pic will be replaced with new pic
                if (res.size() == 1)
                {
                    SaveImage(res[0].u8string().c_str(), windowFlags->export_w, windowFlags->export_h);
                    if (windowFlags->isMSAAOn)
                        windowFlags->isMSAAOn = false;
                    if (windowFlags->shouldExport)
                        windowFlags->shouldExport = false;
                }
            }
            ifd::FileDialog::Instance().Close();
        }
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
            if (!leftMouse->isPressed)
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
            if (!leftMouse->isPressed)
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
            if (!leftMouse->isPressed)
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
    if (ImGui::BeginTable("Other_Icons Table", 10, flags, ImVec2(item_size.x * 12, item_size.y)))
    {
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //Camera Icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->camera_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            //if pressed, no bgcolor
            if (!leftMouse->isPressed)
            {
                ImGui::SetTooltip("Camera");
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
            }
            //if clicked, show camera window
            if (ImGui::IsItemClicked())
                windowFlags->camera_window_open = true;
        }
        //fill the gap between cameraIcon and lightIcon
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //Light Icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->light_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            if (!leftMouse->isPressed)
            {
                ImGui::SetTooltip("Light");
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
            }
            if (ImGui::IsItemClicked())
                windowFlags->light_window_open = true;
        }
        //fill the gap between lightIcon and skyboxIcon
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //Skybox Icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->skybox_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            if (!leftMouse->isPressed)
            {
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                ImGui::SetTooltip("Skybox");
            }
            if (ImGui::IsItemClicked())
                windowFlags->skybox_window_open = true;
        }
        //fill the gap between skyboxIcon and effectIcon
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //Effect Icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->effect_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            if (!leftMouse->isPressed)
            {
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                ImGui::SetTooltip("Effect");
            }
            if (ImGui::IsItemClicked())
                windowFlags->effect_window_open = true;
        }
        //fill the gap between effectIcon and shaderIcon
        ImGui::TableNextColumn();
        ImGui::Dummy(ImVec2(10, 10));
        //ShaderEidtor Icon
        ImGui::TableNextColumn();
        ImGui::Image((void*)resources->shader_icon, item_size);
        if (ImGui::IsItemHovered())
        {
            if (!leftMouse->isPressed)
            {
                ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.7f, 0.65f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
                ImGui::SetTooltip("Shader");
            }
            if (ImGui::IsItemClicked())
                windowFlags->shader_window_open = true;
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

inline void RenderApp::DrawCameraToolWindow()
{

    if (ImGui::Begin("Camera", &windowFlags->camera_window_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size);
        ImGui::SameLine();
        ImGui::Text("Camera Attribute");
        ImGui::Checkbox("Should Look At Current Item?", &windowFlags->shouldLookAtCurItem);
        if (!windowFlags->shouldLookAtCurItem)
        {
            ImGui::InputFloat3("Look At", glm::value_ptr(cur_camera->At), "%.1f");
        }
        else if (cur_item && windowFlags->shouldLookAtCurItem)
            cur_camera->At = cur_item->transform.getLocalPostion();
        ImGui::InputFloat("Radius", &cur_camera->Distance, 0.1f, 0.01f, "%.2f");
        ImGui::End();
    }
}

inline void RenderApp::DrawLightToolWindow()
{

    if (ImGui::Begin("Light", &windowFlags->light_window_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size);
        ImGui::SameLine();
        ImGui::Text("Light On/Off");
        ImGui::SameLine();
        ImGui::ToggleButton("toggle", &windowFlags->isLightOn);
        //Light Setting
        if (windowFlags->isLightOn)
        {
            if (ImGui::CollapsingHeader("BlingPhong-LightingModel"))
            {
                ImGui::Dummy(ImVec2(12,12));
                ImGui::SameLine();
                if (ImGui::CollapsingHeader("DirLight Atrribute"))
                {
                    ImGui::Dummy(fill_size);
                    ImGui::SameLine();
                    ImGui::Text("Directional Light On");
                    ImGui::SameLine();
                    ImGui::ToggleButton("DirLight Toggle", &windowFlags->isLightCastersOn[0]);
                    //Attribute Panel
                    if (windowFlags->isLightCastersOn[0])
                    {
                        ImGui::InputFloat3("DirLight Direction", glm::value_ptr(lightManager->dirLight->direction), "%.1f");
                        ImGui::ColorEdit3("DirLight Ambient", glm::value_ptr(lightManager->dirLight->ambient));
                        ImGui::ColorEdit3("DirLight Diffuse", glm::value_ptr(lightManager->dirLight->diffuse));
                        ImGui::ColorEdit3("DirLight Specular", glm::value_ptr(lightManager->dirLight->specular));
                        //Change the uniform value in default_shader
                        shaderManager->default_shader.use();
                        shaderManager->default_shader.setDirLight(*(lightManager->dirLight));
                    }
                }
                ImGui::Dummy(ImVec2(12, 12));
                ImGui::SameLine();
                if (ImGui::CollapsingHeader("PointLight Atrribute"))
                {
                    ImGui::Dummy(fill_size);
                    ImGui::SameLine();
                    ImGui::Text("Point Light On");
                    ImGui::SameLine();
                    ImGui::ToggleButton("PointLight Toggle", &windowFlags->isLightCastersOn[1]);

                    if (windowFlags->isLightCastersOn[1])
                    {
                        ImGui::InputFloat3("PointLight Position", glm::value_ptr(lightManager->pointLight->position), "%.1f");
                        ImGui::ColorEdit3("PointLight Ambient", glm::value_ptr(lightManager->pointLight->ambient));
                        ImGui::ColorEdit3("PointLight Diffuse", glm::value_ptr(lightManager->pointLight->diffuse));
                        ImGui::ColorEdit3("PointLight Specular", glm::value_ptr(lightManager->pointLight->specular));

                        shaderManager->default_shader.use();
                        shaderManager->default_shader.setPointLight(*(lightManager->pointLight));
                    }
                }
                ImGui::Dummy(ImVec2(12, 12));
                ImGui::SameLine();
                if (ImGui::CollapsingHeader("SpotLight Atrribute"))
                {
                    ImGui::Dummy(fill_size);
                    ImGui::SameLine();
                    ImGui::Text("Spot Light On");
                    ImGui::SameLine();
                    ImGui::ToggleButton("SpotLight Toggle", &windowFlags->isLightCastersOn[2]);

                    if (windowFlags->isLightCastersOn[2])
                    {
                        ImGui::InputFloat("SpotLight InnerCutOut", &lightManager->spotLight->innerCutOut, 0.01f, 0.01f, "%.3f");
                        ImGui::InputFloat("SpotLight OutterCutOut", &lightManager->spotLight->outterCutOut, 0.01f, 0.01f, "%.3f");

                        ImGui::InputFloat3("SpotLight Position", glm::value_ptr(lightManager->spotLight->position), "%.1f");
                        ImGui::InputFloat3("SpotLight Direction", glm::value_ptr(lightManager->spotLight->direction), "%.1f");

                        ImGui::ColorEdit3("SpotLight Ambient", glm::value_ptr(lightManager->spotLight->ambient));
                        ImGui::ColorEdit3("SpotLight Diffuse", glm::value_ptr(lightManager->spotLight->diffuse));
                        ImGui::ColorEdit3("SpotLight Specular", glm::value_ptr(lightManager->spotLight->specular));
                    }
                }
            }
            if (ImGui::CollapsingHeader("PBR-LightingModel"))
            {
                ImGui::Dummy(ImVec2(12, 12));
                ImGui::SameLine();
                if (ImGui::CollapsingHeader("PBR PointLight Atrribute"))
                {
                    ImGui::Dummy(fill_size);
                    ImGui::SameLine();
                    ImGui::Text("PBR-PointLight On");
                    ImGui::SameLine();
                    ImGui::ToggleButton("PBR-PointLight Toggle", &windowFlags->isLightCastersOn[1]);

                    if (windowFlags->isLightCastersOn[1])
                    {
                        ImGui::InputFloat3("PBR-PointLight Position", glm::value_ptr(lightManager->pbrPointLight->position), "%.1f");
                        ImGui::ColorEdit3("PBR-PointLight Color", glm::value_ptr(lightManager->pbrPointLight->lightColor));

                        shaderManager->pbr_shader.use();
                        shaderManager->pbr_shader.setPBRPointLight(*(lightManager->pbrPointLight));
                    }
                }
            }

        }
        ImGui::End();
    }
}

inline void RenderApp::DrawSkyboxToolWindow()
{
    if (ImGui::Begin("Skybox", &windowFlags->skybox_window_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size);
        ImGui::SameLine();
        ImGui::Text("Skybox/EnvMap HDR");
        ImGui::SameLine();
        ImGui::ToggleButton("Skybox Toggle", &windowFlags->isSkyboxOn);
        if (windowFlags->isSkyboxOn)
        {
            float w = ImGui::GetWindowWidth() / 4;
            auto flags = ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_NoHostExtendX |
                ImGuiTableFlags_SizingStretchSame;
            //Draw SkyboxTextures Region
            if (ImGui::CollapsingHeader("SkyBox"))
            {
                ImGui::Dummy(ImVec2(12, 12)); ImGui::SameLine;
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
                ImGui::Dummy(ImVec2(12, 12)); ImGui::SameLine;
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
        else
        {
            //HDR EnvMap
            if (ImGui::CollapsingHeader("EnvMap--HDR"))
            {
                ImGui::Dummy(ImVec2(100, 12)); ImGui::SameLine;
                ImGui::Image((void*)this->hdrTexture, ImVec2(200, 200));
                if (ImGui::Button("Import EnviromentMap .hdr"))
                {
                    ifd::FileDialog::Instance().Open("EnviromentMapping", "Open A HDR Image",
                        "Image file (*.hdr;){.hdr},.*", true);
                }

            }
            if (ifd::FileDialog::Instance().IsDone("EnviromentMapping"))
            {
                if (ifd::FileDialog::Instance().HasResult()) {
                    const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
                    if (res.size() == 1)
                    {
                        glDeleteTextures(1, &this->hdrTexture);
                        glDeleteTextures(1, &this->brdfLUTTexture);
                        glDeleteTextures(1, &this->irradianceMap);
                        glDeleteTextures(1, &this->envCubemap);
                        glDeleteTextures(1, &this->prefilterMap);
                        LoadPBREnvMap(res[0].u8string().c_str());
                    }
                }
                ifd::FileDialog::Instance().Close();
            }
        }

        ImGui::End();
    }
}

inline void RenderApp::DrawEffectToolWindow()
{

    if (ImGui::Begin("Effect", &windowFlags->effect_window_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size);
        ImGui::SameLine();
        ImGui::Text("Effect On/Off");
        ImGui::SameLine();
        ImGui::ToggleButton("effect toggle", &windowFlags->isEffectOn);
        //Effect Setting
        if (windowFlags->isEffectOn)
        {
            ImGui::NewLine();
            //Screen Effecs
            ImGui::Text(u8"屏幕后期特效");
            const char* effect_items[] = { "Inversion","Grayscale","Sharpen","Blur","Edge-Detection" };
            static const char* current_item = effect_items[0];
            static int current_index = 0;
            if (ImGui::BeginCombo("##effect_combo", current_item))
            {

                for (unsigned int i = 0; i < IM_ARRAYSIZE(effect_items); i++)
                {
                    bool is_selected = (current_item == effect_items[i]);
                    if (ImGui::Selectable(effect_items[i], is_selected))
                        current_item = effect_items[current_index = i];
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                switch (current_index)
                {
                case 0:windowFlags->effectFlags = INVERSION_EFFECT;
                    shaderManager->cur_effect_shader = &(shaderManager->inversion_shader); break;
                case 1:windowFlags->effectFlags = GRAYSCALE_EFFECT;
                    shaderManager->cur_effect_shader = &(shaderManager->grayscale_shader); break;
                case 2:windowFlags->effectFlags = SHARPEN_EFFECT;
                    shaderManager->cur_effect_shader = &(shaderManager->sharpen_shader); break;
                case 3:windowFlags->effectFlags = BLUR_EFFECT;
                    shaderManager->cur_effect_shader = &(shaderManager->blur_shader); break;
                case 4:windowFlags->effectFlags = EDGE_DETECTION_EFFECT;
                    shaderManager->cur_effect_shader = &(shaderManager->edge_detection_shader); break;
                }
                ImGui::EndCombo();
            }
        }
        ImGui::End();
    }
}

inline void RenderApp::ShowExportWindow()
{
    if (ImGui::Begin("Effect", &windowFlags->should_export_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size); ImGui::SameLine();
        ImGui::Text(u8"导出渲染结果");
        ImGui::Dummy(fill_size); ImGui::SameLine();
        ImGui::SetNextItemWidth(200.f);
        ImGui::InputInt("ExportWidth",&windowFlags->export_w);
        ImGui::Dummy(fill_size); ImGui::SameLine();
        ImGui::SetNextItemWidth(200.f);
        ImGui::InputInt("ExportHeight", &windowFlags->export_h);
        ImGui::Dummy(fill_size);ImGui::SameLine();
        ImGui::Checkbox(u8"MSAA 超级采样", &windowFlags->isMSAAOn);
        ImGui::Dummy(fill_size); ImGui::SameLine();
        if (ImGui::Button(u8"导出PNG"))
        {
            if (windowFlags->export_w && windowFlags->export_h)
            {
                ifd::FileDialog::Instance().Save("SaveImageFileDialog", "Export Image",
                    "Image file (*.png){.png}");
                windowFlags->shouldExport = true;
            }
            else
                ImGui::OpenPopup("Input Error");
        }
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Input Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"请确保导出图片尺寸不为0");
            ImGui::Separator();
            ImGui::Dummy(ImVec2(40,0));
            ImGui::SameLine();
            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SetItemDefaultFocus();
            ImGui::EndPopup();
        }
        ImGui::End();
    }
}

inline void RenderApp::ShowGeometryCreateWindow()
{
    if (ImGui::Begin("GeometryWindow", &windowFlags->should_export_open))
    {
        ImVec2 fill_size = ImVec2(24, 24);
        ImGui::Dummy(fill_size); ImGui::SameLine();
        ImGui::Text(u8"创建几何体");

        const char* mode_items[] = { "Box","Sphere","XXX" };
        static const char* current_item = mode_items[0];
        static int current_index = 0;
        if (ImGui::BeginCombo("##create_combo", current_item))
        {
            for (unsigned int i = 0; i < IM_ARRAYSIZE(mode_items); i++)
            {
                bool is_selected = (current_item == mode_items[i]);
                if (ImGui::Selectable(mode_items[i], is_selected))
                    current_item = mode_items[current_index = i];
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        static float w = 2, h = 2, d = 2, r = 2;
        static int n = 0, e = 0, slice = 10, stack = 10;
        static MaterialType t = BlingPhong;
 ;
        if (ImGui::RadioButton("Bling-Phong Material", &e, 0))
            t = BlingPhong;
        ImGui::SameLine();
        if (ImGui::RadioButton("PBR Material", &e, 1))
            t = PBR;
        ImGui::Dummy(fill_size); ImGui::SameLine();
        ImGui::SetNextItemWidth(40.0f);

        if (ImGui::Button(u8"创建"))
        {
            if (current_index == 0)
                ImGui::OpenPopup("Create Box");
            else if (current_index == 1)
                ImGui::OpenPopup("Create Sphere");
            else if (current_index == 2)
                ImGui::OpenPopup("Create XXX");
        }        
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Create Box", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"请输入Box的相关参数"); ImGui::Separator();
            ImGui::Dummy(ImVec2(40, 0)); ImGui::SameLine();
            ImGui::InputFloat(u8"width", &w, 0.1, 0.05, "%.3f");
            ImGui::Dummy(ImVec2(40, 0)); ImGui::SameLine();
            ImGui::InputFloat(u8"height", &h, 0.1, 0.05, "%.3f");
            ImGui::Dummy(ImVec2(40, 0)); ImGui::SameLine();
            ImGui::InputFloat(u8"depth", &d, 0.1, 0.05, "%.3f");
            ImGui::Dummy(ImVec2(40, 0)); ImGui::SameLine();
            ImGui::SliderInt(u8"细分次数", &n, 0, 10);

            ImGui::Dummy(ImVec2(60, 0)); ImGui::SameLine();
            if (ImGui::Button("OK", ImVec2(120, 0))) 
            {
                items.push_back(new RenderItem());
                RenderItem item;
                Mesh mesh(t);
                mesh.GetDataFrom(generator->CreateBox(w, h, d, n));
                items.back()->setModel(std::move(mesh));
                ImGui::CloseCurrentPopup(); 
            }
            ImGui::SetItemDefaultFocus(); ImGui::SameLine();
            ImGui::Dummy(ImVec2(100, 0)); ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Create Sphere", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"请输入Sphere的相关参数");
            ImGui::Dummy(ImVec2(40, 0)); ImGui::SameLine();
            ImGui::InputFloat(u8"radius", &r, 0.1, 0.05, "%.3f");
            ImGui::Dummy(ImVec2(40, 0)); ImGui::SameLine();
            ImGui::SliderInt(u8"sliceCount", &slice, 10, 30, "%d");
            ImGui::Dummy(ImVec2(40, 0)); ImGui::SameLine();
            ImGui::SliderInt(u8"stackCount", &stack, 10, 30, "%d");

            ImGui::Dummy(ImVec2(60, 0)); ImGui::SameLine();
            if (ImGui::Button("OK", ImVec2(120, 0))) { 
                items.push_back(new RenderItem());
                RenderItem item;
                Mesh mesh(t);
                mesh.GetDataFrom(generator->CreateSphere(r, slice, stack));
                items.back()->setModel(std::move(mesh));
                ImGui::CloseCurrentPopup(); 
            }
            ImGui::SetItemDefaultFocus(); ImGui::SameLine();
            ImGui::Dummy(ImVec2(100, 0)); ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        ImGui::End();
    }
}

inline void RenderApp::LoadPBREnvMap(const char* path)
{

    size_t captureFBO, captureRBO;
    // pbr: setup framebuffer
    // ----------------------
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // pbr: load the HDR environment map
    // ---------------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(path, &width, &height, &nrComponents, 0);
   
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    shaderManager->equirectangularToCubemapShader.use();
    shaderManager->equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    shaderManager->equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    for (unsigned int i = 0; i < 6; ++i)
    {
        shaderManager->equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    shaderManager->irradianceShader.use();
    shaderManager->irradianceShader.setInt("environmentMap", 0);
    shaderManager->irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        shaderManager->irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    shaderManager->prefilterShader.use();
    shaderManager->prefilterShader.setInt("environmentMap", 0);
    shaderManager->prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        shaderManager->prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            shaderManager->prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    glGenTextures(1, &brdfLUTTexture);

    // pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    shaderManager->brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
}

inline void RenderApp::ShowBlingPhongMaterial(int cur_index, Material* material, std::vector<Mesh>& meshes)
{
    size_t p = 0;
    ImVec2 fill_size = ImVec2(24, 24);
    auto m = static_cast<BlingPhongMaterial*>(material);
    //Ambient
    ImGui::Dummy(fill_size);
    ImGui::SameLine();
    ImGui::ColorEdit3("Ambient", glm::value_ptr(m->Ambient));
    //Diffuse Mapping
    ImGui::NewLine(); ImGui::Dummy(fill_size);
    ImGui::SameLine(); ImGui::SetNextItemWidth(250.0f);
    ImGui::ColorEdit3("Diffuse ", glm::value_ptr(m->Diffuse));
    ImGui::SameLine();
    ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint
    if (m->diffuseMapping)
    {
        if (ImGui::ImageButton("#Diffuse", (void*)meshes[cur_index].textures[p++].id, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open(material_dialogs[0], "Open A Diffuse Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    }
    else
        if (ImGui::ImageButton("#Diffuse", 0, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open(material_dialogs[0], "Open A Diffuse Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);

    //Specular Mapping
    ImGui::NewLine(); ImGui::Dummy(fill_size);
    ImGui::SameLine(); ImGui::SetNextItemWidth(250.0f);
    ImGui::ColorEdit3("Specular", glm::value_ptr(m->Specular));
    ImGui::SameLine();
    if (m->specularMapping)
    {
        if (ImGui::ImageButton("#Specular", (void*)meshes[cur_index].textures[p++].id, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open(material_dialogs[1], "Open A Specular Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    }
    else
        if (ImGui::ImageButton("#Specular", 0, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open(material_dialogs[1], "Open A Specular Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    //Normal Mapping
    ImGui::Dummy(fill_size); ImGui::SameLine();
    ImGui::Text("Normal Map"); ImGui::SameLine();
    if (m->normalMapping)
    {
        if (ImGui::ImageButton("#Normal", (void*)meshes[cur_index].textures[p++].id, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open(material_dialogs[2], "Open A Normal Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    }
    else
        if (ImGui::ImageButton("#Normal", 0, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open(material_dialogs[2], "Open A Normal Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    //Shininess
    ImGui::Dummy(fill_size); ImGui::SameLine();
    ImGui::SliderFloat("Shininess", &m->Shininess, 0, 128);

    ImGui::Dummy(fill_size);
    ImGui::SameLine();

    ImGui::Text(m->m_shader->vertexName.c_str());
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(24 * 3, 30));
    ImGui::SameLine();
    if (ImGui::Button("Open .vert", ImVec2(24 * 6, 30)))
    {
        windowFlags->shader_window_open = true;
        windowFlags->shader_flag = VERTEX;
        editor.SetText(m->m_shader->vertexCode);
    }

    ImGui::Dummy(fill_size);
    ImGui::SameLine();
    ImGui::Text(m->m_shader->fragmentName.c_str());
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(24 * 3, 30));
    ImGui::SameLine();
    if (ImGui::Button("Open .frag", ImVec2(24 * 6, 30)))
    {
        windowFlags->shader_window_open = true;
        windowFlags->shader_flag = FRAGMENT;
        windowFlags->cur_shader = m->m_shader;
        editor.SetText(m->m_shader->fragmentCode);
    }
}

inline void RenderApp::ProcessBlingPhongDialog(Material* material, std::vector<Texture>& textures)
{
    auto m = static_cast<BlingPhongMaterial*>(material);
    for (size_t i = 0; i < 3; i++)
    {
        if (ifd::FileDialog::Instance().IsDone(material_dialogs[i])) {
            if (ifd::FileDialog::Instance().HasResult()) {
                const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
                //old pic will be replaced with new pic
                if (res.size() == 1)
                {
                    switch (i)
                    {
                    case 0:
                    {
                        if (m->diffuseMapping == false)
                        {
                            Texture tex;
                            tex.path = res[0].u8string();
                            tex.id = LoadTexture(tex.path.C_Str(), true);
                            tex.type = "texture_diffuse";
                            textures.insert(textures.begin(), std::move(tex));
                        }
                        else
                        {
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type == "texture_diffuse")
                                {
                                    textures[j].path = res[0].u8string();
                                    glDeleteTextures(1, &textures[j].id);
                                    textures[j].id = LoadTexture(textures[j].path.C_Str(), true);
                                    textures[j].type = "texture_diffuse";
                                    break;
                                }
                            }
                        }
                        m->diffuseMapping = true;
                    }; break;
                    case 1:
                    {
                        bool k = false;
                        if (m->specularMapping == false)
                        {
                            Texture tex;
                            tex.path = res[0].u8string();
                            tex.id = LoadTexture(tex.path.C_Str());
                            tex.type = "texture_specular";
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type == "texture_diffuse")
                                {
                                    textures.insert(textures.begin() + j + 1, tex);
                                    k = true;
                                    break;
                                }
                            }
                            if (k)
                                break;
                            else
                                textures.insert(textures.begin(), std::move(tex));
                        }
                        else
                        {
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type == "texture_specular")
                                {
                                    textures[j].path = res[0].u8string();
                                    glDeleteTextures(1, &textures[j].id);
                                    textures[j].id = LoadTexture(textures[j].path.C_Str());
                                    textures[j].type = "texture_specular";
                                    break;
                                }
                            }
                        }
                        m->specularMapping = true;
                    }; break;
                    case 2:
                    {
                        bool k = false;
                        if (m->normalMapping == false)
                        {
                            Texture tex;
                            tex.path = res[0].u8string();
                            tex.id = LoadTexture(tex.path.C_Str());
                            tex.type = "texture_normal";
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type == "texture_specular")
                                {
                                    textures.insert(textures.begin() + j + 1, tex);
                                    k = true;
                                    break;
                                }
                            }
                            if (k)
                                break;
                            else
                                for (size_t j = 0; j < textures.size(); j++)
                                {
                                    if (textures[j].type == "texture_diffuse")
                                    {
                                        textures.insert(textures.begin() + j + 1, tex);
                                        k = true;
                                        break;
                                    }
                                }
                            if (k)
                                break;
                            else
                                textures.insert(textures.begin(), std::move(tex));
                        }
                        else
                        {
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type == "texture_normal")
                                {
                                    textures[j].path = res[0].u8string();
                                    glDeleteTextures(1, &textures[j].id);
                                    textures[j].id = LoadTexture(textures[j].path.C_Str());
                                    textures[j].type = "texture_normal";
                                    break;
                                }
                            }
                        }
                        m->normalMapping = true;
                    }; break;
                    default:break;
                    }
                }
            }
            ifd::FileDialog::Instance().Close();
        }
    }
}

inline void RenderApp::ShowPBRMaterial(int cur_index, Material* material, std::vector<Mesh>& meshes)
{
    size_t p = 0;
    ImVec2 fill_size = ImVec2(24, 24);
    ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

    auto m = static_cast<PBRMaterial*>(material);
    //Albedo
    ImGui::NewLine(); ImGui::Dummy(fill_size);
    ImGui::SameLine(); ImGui::SetNextItemWidth(250.0f);
    ImGui::ColorEdit3("Albedo", glm::value_ptr(m->albedo));
    ImGui::SameLine();
    if (m->albedoMapping)
    {
        if (ImGui::ImageButton("#Albedo", (void*)meshes[cur_index].textures[p++].id, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Albedo Mapping", "Open Albedo Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    }
    else
        if (ImGui::ImageButton("#Albedo", 0, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Albedo Mapping", "Open Albedo Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    //Ao
    ImGui::NewLine(); ImGui::Dummy(fill_size);
    ImGui::SameLine(); ImGui::SetNextItemWidth(250.0f);
    ImGui::SliderFloat("Ao", &m->ao, 1.0f, 2.0f, "%.2f");
    ImGui::SameLine();
    if (m->aoMapping)
    {
        if (ImGui::ImageButton("#Ao", (void*)meshes[cur_index].textures[p++].id, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Ao Mapping", "Open Ao Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.**", true);
    }
    else
        if (ImGui::ImageButton("#Ao", 0, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Ao Mapping", "Open Ao Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    //Metallic
    ImGui::NewLine(); ImGui::Dummy(fill_size);
    ImGui::SameLine(); ImGui::SetNextItemWidth(250.0f);
    ImGui::SliderFloat("Metallic", &m->metallic, 0.0f, 1.0f, "%.2f");
    ImGui::SameLine();
    if (m->metallicMapping)
    {
        if (ImGui::ImageButton("#Metallic", (void*)meshes[cur_index].textures[p++].id, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Metallic Mapping", "Open Metallic Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    }
    else
        if (ImGui::ImageButton("#Metallic", 0, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Metallic Mapping", "Open Metallic Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    //Normal
    ImGui::NewLine(); ImGui::Dummy(fill_size);
    ImGui::SameLine(); ImGui::Text("Normal");
    ImGui::SameLine();
    if (m->normalMapping)
    {
        if (ImGui::ImageButton("#Normal", (void*)meshes[cur_index].textures[p++].id, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Normal Mapping", "Open Normal Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    }
    else
        if (ImGui::ImageButton("#Normal", 0, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Normal Mapping", "Open Normal Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    //Roughness
    ImGui::NewLine(); ImGui::Dummy(fill_size);
    ImGui::SameLine(); ImGui::SetNextItemWidth(250.0f);
    ImGui::SliderFloat("Roughness", &m->roughness, 0.05f, 1.0f, "%.2f");
    ImGui::SameLine();
    if (m->roughnessMapping)
    {
        if (ImGui::ImageButton("#Roughness", (void*)meshes[cur_index].textures[p++].id, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Roughness Mapping", "Open Roughness Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);
    }
    else
        if (ImGui::ImageButton("#Roughness", 0, ImVec2(32, 32)))
            ifd::FileDialog::Instance().Open("PBR Roughness Mapping", "Open Roughness Map",
                "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga){.png,.jpg,.jpeg,.bmp,.tga},.*", true);

    ImGui::Dummy(fill_size);
    ImGui::SameLine();

    ImGui::Text(m->m_shader->vertexName.c_str());
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(24 * 3, 30));
    ImGui::SameLine();
    if (ImGui::Button("Open .vert", ImVec2(24 * 6, 30)))
    {
        windowFlags->shader_window_open = true;
        windowFlags->shader_flag = VERTEX;
        editor.SetText(m->m_shader->vertexCode);
    }

    ImGui::Dummy(fill_size);
    ImGui::SameLine();
    ImGui::Text(m->m_shader->fragmentName.c_str());
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(24 * 3, 30));
    ImGui::SameLine();
    if (ImGui::Button("Open .frag", ImVec2(24 * 6, 30)))
    {
        windowFlags->shader_window_open = true;
        windowFlags->shader_flag = FRAGMENT;
        windowFlags->cur_shader = m->m_shader;
        editor.SetText(m->m_shader->fragmentCode);
    }
}

inline void RenderApp::ProcessPBRDialog(Material* material, std::vector<Texture>& textures)
{
    auto m = static_cast<PBRMaterial*>(material);
    for (size_t i = 0; i < 5; i++)
    {
        if (ifd::FileDialog::Instance().IsDone(pbr_material_dialogs[i])) {
            if (ifd::FileDialog::Instance().HasResult()) {
                const std::vector<std::filesystem::path>& res = ifd::FileDialog::Instance().GetResults();
                //old pic will be replaced with new pic
                if (res.size() == 1)
                {
                    switch (i)
                    {
                    case 0:
                    {
                        if (m->albedoMapping == false)
                        { 
                            Texture tex;
                            tex.path = res[0].u8string();
                            tex.id = LoadTexture(tex.path.C_Str(), true);
                            tex.type = "texture_albedo";
                            textures.insert(textures.begin(), std::move(tex));
                            m->albedoMapping = true;
                        }
                        else
                        {
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_albedo") == 0)
                                {
                                    textures[j].path = res[0].u8string();
                                    glDeleteTextures(1, &textures[j].id);
                                    textures[j].id = LoadTexture(textures[j].path.C_Str(), true);
                                    textures[j].type = "texture_albedo";
                                    break;
                                }
                            }
                        }
                    }; break;
                    case 1:
                    {
                        bool k = false;
                        if (m->aoMapping == false)
                        {
                            Texture tex;
                            tex.path = res[0].u8string();
                            tex.id = LoadTexture(tex.path.C_Str());
                            tex.type = "texture_ao";
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_albedo") == 0)
                                {
                                    textures.insert(textures.begin() + j + 1, tex);
                                    k = true;
                                    break;
                                }
                            }
                            if (k)
                            {
                                m->aoMapping = true;
                                break;
                            }
                            else
                            {
                                textures.insert(textures.begin(), std::move(tex));
                                m->aoMapping = true;
                            }
                        }
                        else
                        {
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_ao") == 0)
                                {
                                    textures[j].path = res[0].u8string();
                                    glDeleteTextures(1, &textures[j].id);
                                    textures[j].id = LoadTexture(textures[j].path.C_Str());
                                    textures[j].type = "texture_ao";
                                    break;
                                }
                            }
                        }
                    }; break;
                    case 2:
                    {
                        bool k = false;
                        if (m->metallicMapping == false)
                        {
                            Texture tex;
                            tex.path = res[0].u8string();
                            tex.id = LoadTexture(tex.path.C_Str());
                            tex.type = "texture_metallic";
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_ao") == 0)
                                {
                                    textures.insert(textures.begin() + j + 1, tex);
                                    k = true;
                                    break;
                                }
                            }
                            if (k)
                            {
                                m->metallicMapping = true;
                                break;
                            }
                            else
                                for (size_t j = 0; j < textures.size(); j++)
                                {
                                    if (textures[j].type.compare("texture_albedo") == 0)
                                    {
                                        textures.insert(textures.begin() + j + 1, tex);
                                        k = true;
                                        break;
                                    }
                                }
                            if (k)
                            {
                                m->metallicMapping = true;
                                break;
                            }
                            else
                            {
                                textures.insert(textures.begin(), std::move(tex));
                                m->metallicMapping = true;
                            }
                        }
                        else
                        {
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_metallic") == 0)
                                {
                                    textures[j].path = res[0].u8string();
                                    glDeleteTextures(1, &textures[j].id);
                                    textures[j].id = LoadTexture(textures[j].path.C_Str());
                                    textures[j].type = "texture_metallic";
                                    break;
                                }
                            }
                        }
                    }; break;
                    case 3:
                    {
                        bool k = false;
                        if (m->normalMapping == false)
                        {
                            Texture tex;
                            tex.path = res[0].u8string();
                            tex.id = LoadTexture(tex.path.C_Str());
                            tex.type = "texture_normal";
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_metallic") == 0) 
                                {
                                    textures.insert(textures.begin() + j + 1, tex);
                                    k = true;
                                    break;
                                }
                            }
                            if (k)
                            {
                                m->normalMapping = true;
                                break;
                            }
                            else
                                for (size_t j = 0; j < textures.size(); j++)
                                {
                                    if (textures[j].type.compare("texture_ao") == 0)
                                    {
                                        textures.insert(textures.begin() + j + 1, tex);
                                        k = true;
                                        break;
                                    }
                                }
                            if (k)
                            {
                                m->normalMapping = true;
                                break;
                            }
                            else
                                for (size_t j = 0; j < textures.size(); j++)
                                {
                                    if (textures[j].type.compare("texture_albedo") == 0)
                                    {
                                        textures.insert(textures.begin() + j + 1, tex);
                                        k = true;
                                        break;
                                    }
                                }
                            if (k)
                            {
                                m->normalMapping = true;
                                break;
                            }
                            else
                            {
                                textures.insert(textures.begin(), std::move(tex));
                                m->normalMapping = true;
                            }
                        }
                        else
                        {
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_normal") == 0)
                                {
                                    textures[j].path = res[0].u8string();
                                    glDeleteTextures(1, &textures[j].id);
                                    textures[j].id = LoadTexture(textures[j].path.C_Str());
                                    textures[j].type = "texture_normal";
                                    break;
                                }
                            }
                        }
                    }; break;
                    case 4:
                    {
                        bool k = false;
                        if (m->roughnessMapping == false)
                        {
                            Texture tex;
                            tex.path = res[0].u8string();
                            tex.id = LoadTexture(tex.path.C_Str());
                            tex.type = "texture_roughness";
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_normal") == 0)
                                {
                                    textures.insert(textures.begin() + j + 1, tex);
                                    k = true;
                                    break;
                                }
                            }
                            if (k)
                            {
                                m->roughnessMapping = true;
                                break;
                            }
                            else
                                for (size_t j = 0; j < textures.size(); j++)
                                {
                                    if (textures[j].type.compare("texture_metallic") == 0)
                                    {
                                        textures.insert(textures.begin() + j + 1, tex);
                                        k = true;
                                        break;
                                    }
                                }
                            if (k)
                            {
                                m->roughnessMapping = true;
                                break;
                            }
                            else
                                for (size_t j = 0; j < textures.size(); j++)
                                {
                                    if (textures[j].type.compare("texture_ao") == 0)
                                    {
                                        textures.insert(textures.begin() + j + 1, tex);
                                        k = true;
                                        break;
                                    }
                                }
                            if (k)
                            {
                                m->roughnessMapping = true;
                                break;
                            }
                            else
                                for (size_t j = 0; j < textures.size(); j++)
                                {
                                    if (textures[j].type.compare("texture_albedo") == 0)
                                    {
                                        textures.insert(textures.begin() + j + 1, tex);
                                        k = true;
                                        break;
                                    }
                                }
                            if (k)
                            {
                                m->roughnessMapping = true;
                                break;
                            }
                            else
                            {
                                textures.insert(textures.begin(), std::move(tex));
                                m->roughnessMapping = true;
                            }
                        }
                        else
                        {
                            for (size_t j = 0; j < textures.size(); j++)
                            {
                                if (textures[j].type.compare("texture_roughness") == 0)
                                {
                                    textures[j].path = res[0].u8string();
                                    glDeleteTextures(1, &textures[j].id);
                                    textures[j].id = LoadTexture(textures[j].path.C_Str());
                                    textures[j].type = "texture_roughness";
                                    break;
                                }
                            }
                        }
                    }; break;
                    default:break;
                    }
                }
            }
            ifd::FileDialog::Instance().Close();
        }
    }
}