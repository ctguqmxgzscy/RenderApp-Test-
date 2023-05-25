#pragma once
#include"Model.h"
#include<cstdint>
#include<vector>
#include <list>

#include"../glm/glm.hpp"
#include"../glm/gtc/matrix_transform.hpp"
#include"../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"	

class Transform {

protected:
	//Local Space
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale ;
	//Global Space
	glm::mat4 model_mat;
	//Dirty Flag
	bool m_isDirty;

protected:
	glm::mat4 getLocalSpaceMat()
	{

		glm::quat quaternion = glm::quat(glm::radians(rotation));

		const glm::mat4 rotation_mat = glm::toMat4(quaternion);

		return glm::translate(glm::mat4(1.0f), position) *
			rotation_mat *
			glm::scale(glm::mat4(1.0f), scale);
	}

public:
	Transform()
	{
		this->position = glm::vec3(0.f, 0.f, 0.f);
		this->rotation = glm::vec3(0.f, 0.f, 0.f);
		this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
		this->model_mat = glm::mat4(1.0f);
		this->m_isDirty = true;
	}

	Transform(const Transform& rhs)
	{ 
		this->position = glm::vec3(rhs.position);
		this->rotation = glm::vec3(rhs.rotation);
		this->scale = glm::vec3(rhs.scale);
		this->model_mat = glm::mat4(rhs.model_mat);
		this->m_isDirty = rhs.m_isDirty;
	}

public:
	void computeModelMat()
	{
		model_mat = getLocalSpaceMat();
		m_isDirty = false;
	}

	void computeModelMat(const glm::mat4& parentGlobalModelMat)
	{
		model_mat = parentGlobalModelMat * getLocalSpaceMat();
		m_isDirty = false;
	}

	void setNewPosition(const glm::vec3& newPostion)
	{
		position = newPostion;
		m_isDirty = true;
	}
	
	void setNewRotation(const glm::vec3& newRotation)
	{
		rotation = newRotation;
		m_isDirty = true;
	}
	void setNewScale(const glm::vec3& newScale)
	{
		scale = newScale;
		m_isDirty = true;
	}

	const glm::vec3& getLocalPostion()
	{
		return position;
	}

	const glm::vec3& getRotation() 
	{
		return rotation;
	}

	const glm::vec3& getScale()
	{
		return scale;
	}

	const glm::mat4& getModelMat()
	{
		return model_mat;
	}

	bool isDirty()
	{
		return m_isDirty;
	}
};

class RenderItem{

private:
	Model* m_Model_;
    std::string _name;
private:
    bool _is_shader_loaded = false;
    bool _is_diabled = false;
public:
    static int count;
public:
    RenderItem(const char* str);
	RenderItem(const RenderItem* rhs);
    RenderItem();
	~RenderItem();
public:
    //绘制函数，可以使用EnablePicking()和DisablePicking()来控制是绘制到自定义的帧缓存还是系统帧缓存的颜色缓存中
    void Draw(); 
	bool hasRenderData() { if (this->m_Model_) return true; return false; }
public:
	//Scene Graph
	std::list<std::unique_ptr<RenderItem>> children;
	RenderItem* parent = nullptr;
	//Space Information
	Transform transform;

	template<typename... TArgs>
	void addChild(const TArgs&... args)
	{
		children.emplace_back(std::make_unique<RenderItem>(args...));
		children.back()->parent = this;
	}

	//Update transform if it was changed
	void updateSelfAndChild()
	{
		if (transform.isDirty())
		{
			forceUpdateSelfAndChild();
			return;
		}

		for (auto&& child : children)
		{
			child->updateSelfAndChild();
		}
	}

	//Force update of transform even if local space don't change
	void forceUpdateSelfAndChild()
	{
		if (parent)
			transform.computeModelMat(parent->transform.getModelMat());
		else
			transform.computeModelMat();

		for (auto&& child : children)
		{
			child->updateSelfAndChild();
		}
	}

public:
	Model& getModel() { return *m_Model_; }
	void setModel(Model* model) { this->m_Model_ = model; }
	void setModel(Mesh&& mesh) { if (this->m_Model_==NULL) this->m_Model_ = new Model(std::move(mesh)); }
    std::string getName() { return _name; }
    void setName(std::string name) {this->_name = name;}
    bool isDisabled() { return this->_is_diabled; }
    bool* getDisabled() { return &_is_diabled; }
};

class PickingTexture {
public:
    PickingTexture();
	~PickingTexture() { glDeleteFramebuffers(1, &m_fbo); };
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
	unsigned int getPickingTexture() { return this->m_pickingTexture; }
private:
    unsigned int m_fbo;
    unsigned int m_pickingTexture;
    unsigned int m_depthTexture;
};
