#pragma once
#include"Model.h"
#include "GeometryGenerator.h"

class RenderItem : public Node{

private:
	Model* m_Model_;
    std::string _name;
    Shader _shader; 
private:
    bool _is_shader_loaded = false;
    bool _is_diabled = false;
public:
    static int count;
public:
    RenderItem(const char* str);
    RenderItem();
public:
    void EnablePicking();
    void DisablePicking();
    //绘制函数，可以使用EnablePicking()和DisablePicking()来控制是绘制到自定义的帧缓存还是系统帧缓存的颜色缓存中
    void Draw(); 
    void Draw_Mesh_Onclicked(Shader _exclude_shader, Shader _simple_color_shader, unsigned int drawIndex);
    void Draw_Triangle_Onclicked(Shader _exclude_shader, Shader _simple_color_shader, 
        unsigned int drawIndex,unsigned int primitiveIndex);
    
public:
	Model& getModel() { return *m_Model_; }
	void setModel(Model* mesh) { this->m_Model_ = mesh; }
    std::string getName() { return _name; }
    void setName(std::string name) {this->_name = name;}
    bool isDisabled() { return this->_is_diabled; }
    bool* getDisabled() { return &_is_diabled; }
    void setShader(const Shader& shader) { _shader = shader; _is_shader_loaded = true; }
    Shader getShader() { return this->_shader; }
};

class PickingTexture {
public:
    PickingTexture();
    ~PickingTexture();
    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
    void EnableWriting() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo); }
    void DisableWriting() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); }
    struct PixelInfo {
        float ObjectID;
        float DrawID;
        float PrimID;
        PixelInfo() {
            ObjectID = 0;
            DrawID = 0;
            PrimID = 0;
        }
    };
    PixelInfo ReadPixel(unsigned int x, unsigned int y);
private:
    unsigned int m_fbo;
    unsigned int m_pickingTexture;
    unsigned int m_depthTexture;
};