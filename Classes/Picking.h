#pragma once
#include"Model.h"
class RenderItem {

private:
	Model* m_Model_;
public:
	RenderItem(const char* str) { m_Model_=new Model(str); }
public:
    void EnablePicking();
    void DisablePicking();
    //绘制函数，可以使用EnablePicking()和DisablePicking()来控制是绘制到自定义的帧缓存还是系统帧缓存的颜色缓存中
    void Draw(Shader shader) { this->m_Model_->Draw(shader); }
    void Draw_Mesh_Onclicked(Shader _exclude_shader, Shader _simple_color_shader, unsigned int drawIndex);
    void Draw_Triangle_Onclicked(Shader _exclude_shader, Shader _simple_color_shader, 
        unsigned int drawIndex,unsigned int primitiveIndex);
    
public:

	Model& get_Model() { return *m_Model_; }
	void set_Model(Model* mesh) { this->m_Model_ = mesh; }

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