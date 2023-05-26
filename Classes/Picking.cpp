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
}

RenderItem::RenderItem(const RenderItem* rhs)
{
    this->_name = std::string(rhs->_name.c_str()).append("(copy)");
    if (rhs->m_Model_ != NULL)
    {
        if (!rhs->m_Model_->getPath().empty())
            this->m_Model_ = new Model(rhs->m_Model_->getPath().c_str());
        else 
        {
            this->m_Model_ = new Model(*(rhs->m_Model_));
        }
    }
    this->_is_shader_loaded = false;
    this->_is_diabled = false;
    this->parent = rhs->parent;
    this->transform = Transform(rhs->transform);
    count++;

    if (rhs->children.size() == 0)
        return;

    for (auto&& child : rhs->children)
    {
        auto copy = new RenderItem(child.get());
        this->children.push_back(std::make_unique<RenderItem>(copy));
        this->children.back()->parent = this;
    }
}

RenderItem::RenderItem()
{
    char name[20];
    count++;
    sprintf_s(name, "object_%d", count);
    _name = std::string(name);
}

RenderItem::~RenderItem()
{
    this->children.clear();
    count--;
    delete this->m_Model_;
    this->m_Model_ = nullptr;
}

void RenderItem::Draw() {
    if (this->m_Model_) 
        if (_is_shader_loaded) 
            this->m_Model_->Draw();
        else 
            this->m_Model_->Draw_DefaultEffects();
    else
         return;
}
