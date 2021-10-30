#include "Picking.h"

PickingTexture::PickingTexture()
{
    this->m_fbo = 0;
    this->m_depthTexture = 0;
    this->m_pickingTexture = 0;
}

bool PickingTexture::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{   // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Create the texture object for the primitive information buffer
    glGenTextures(1, &m_pickingTexture);
    glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);

    // Create the texture object for the depth buffer
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Verify that the FBO is correct
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    // Restore the default framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
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


void RenderItem::EnablePicking()
{

    if (this->m_Model_)
        this->m_Model_->EnablePicking();
    else
        printf("Your RenderItem have not any date to use!\n");
}

void RenderItem::DisablePicking()
{
    if (this->m_Model_)
        this->m_Model_->DisablePicking();
    else
        printf("Your RenderItem have not any date to use!\n");
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
    _exclude_shader.use();
    this->m_Model_->Draw(_exclude_shader, drawIndex);
    this->m_Model_->get_Meshes()[drawIndex].Draw_Triangle_SimpleColor(_exclude_shader,_simple_color_shader, primitiveIndex);
}
