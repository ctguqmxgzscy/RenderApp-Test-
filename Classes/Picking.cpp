#include "Picking.h"

int RenderItem::count = 0;

PickingTexture::PickingTexture()
{
    this->m_fbo = 0;
    this->m_depthTexture = 0;
    this->m_pickingTexture = 0;
}

bool PickingTexture::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    //create a color attachment texture 
    glGenTextures(1, &m_pickingTexture);
    glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WindowWidth, WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);
    //create a renderbuffer object for depth and stencil attachment
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    //now that we actually create the freambuffer and added all attachements we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is no complete!" << std::endl;
        return false;
    }    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return true;
}

PickingTexture::PixelInfo PickingTexture::ReadPixel(unsigned int x, unsigned int y)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    PixelInfo Pixel;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &Pixel);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    return Pixel;
}

RenderItem::RenderItem(const char* str)
{
    char name[20];
    m_Model_ = new Model(str); 
    count++;
    sprintf_s(name, "object_%d", count);
    _name = std::string(name);
    _shader = new Shader("Shaders/simple_color.vert", "Shaders/simple_color.frag");
}

RenderItem::RenderItem()
{
    char name[20];
    count++;
    sprintf_s(name, "object_%d", count);
    _name = std::string(name);
    _shader = new Shader("Shaders/simple_color.vert", "Shaders/simple_color.frag");
}

RenderItem::~RenderItem()
{
    this->children.clear();
    count--;
    delete this->m_Model_;
}

void RenderItem::Draw() {
    if (this->m_Model_) {
        if (_is_shader_loaded) {
            _shader->use();
            _shader->setMat4("model", this->transform.getModelMat());
            this->m_Model_->Draw(*_shader);
        }
        else {
            _shader->use();
            _shader->setMat4("model", this->transform.getModelMat());
            this->m_Model_->Draw_DefaultEffects(*_shader);
        }
    }
    else
         return;
}

void RenderItem::EnablePicking()
{

    if (this->m_Model_)
        this->m_Model_->EnablePicking();
    else
        return;
}

void RenderItem::DisablePicking()
{
    if (this->m_Model_)
        this->m_Model_->DisablePicking();
    else
        return;
}


void RenderItem::Draw_Mesh_Onclicked(Shader _exclude_shader, Shader _simple_color_shader, unsigned int drawIndex)
{
    _exclude_shader.use();
    this->m_Model_->Draw(_exclude_shader, drawIndex);
    _simple_color_shader.use();
    this->m_Model_->get_Meshes()[drawIndex].Draw_Mesh_SimpleColor(_simple_color_shader);
}

void RenderItem::Draw_Triangle_Onclicked(Shader _exclude_shader, Shader _simple_color_shader, unsigned int drawIndex, unsigned int primitiveIndex)
{
    //启用正常的模型着色器，把第drawIndex个网格除外的网格全部绘制出来
    _exclude_shader.use();
    this->m_Model_->Draw(_exclude_shader, drawIndex);
    //绘制第drawIndex个网格的第PrimitiveIndex个片元（这里是三角形）
    this->m_Model_->get_Meshes()[drawIndex].Draw_Triangle_SimpleColor(_exclude_shader,_simple_color_shader, primitiveIndex);
}
