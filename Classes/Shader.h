#pragma once
#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

#include"../glm/glm.hpp"
#include"../glm/gtc/matrix_transform.hpp"
#include"../glm/gtc/type_ptr.hpp"

//平行光
struct DirectionalLight
{
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	DirectionalLight()
	{
		direction = glm::vec3(-0.2f, -1.0f, -0.3f);
		ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		specular = glm::vec3(0.5f, 0.5f, 0.5f);
	}
};
//点光源
struct PointLight
{
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	PointLight()
	{
		position = glm::vec3(0, 5.0f, 5.0f);
		ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
		specular = glm::vec3(1.0f, 1.0f, 1.0f);
		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;
	}
};
//聚光灯
struct SpotLight
{
	float innerCutOut;
	float outterCutOut;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	SpotLight()
	{
		position = glm::vec3(0, 10.0f, 0);
		direction = glm::vec3(0, -1.0f, 0);
		ambient = glm::vec3(0, 0, 0);
		diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		specular = glm::vec3(1.0f, 1.0f, 1.0f);

		constant = 10.0f;
		linear = 0.09f;
		quadratic = 0.032f;
		innerCutOut = glm::cos(glm::radians(12.5f));
		outterCutOut = glm::cos(glm::radians(15.0f));
	}
};


class LightManager
{
public:
	LightManager() 
	{
		dirLight = new DirectionalLight();
		spotLight = new SpotLight();
		pointLight = new PointLight();
	}
	~LightManager() {}


public:
	DirectionalLight* dirLight;
	SpotLight* spotLight;
	PointLight* pointLight;
};

class Shader {
public:
	unsigned int ID;
	Shader(const GLchar* vertexPath, const GLchar* fragment);
	Shader(){}
	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value)const;
	void setUint(const std::string& name, unsigned int value) const;
	void setFloat(const std::string& name, float value)const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w);
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	
	void setDirLight(const DirectionalLight& light) const;
	void setPointLight(const PointLight& light)const;
	void setSpotLight(const SpotLight& light)const;
};


struct ShaderManager
{
	Shader* picking_shader;
	Shader* default_shader;
	Shader* skybox_shader;
	//Effect Shader
	Shader* cur_effect_shader;
	Shader* inversion_shader;
	Shader* grayscale_shader;
	Shader* sharpen_shader;
	Shader* blur_shader;
	Shader* edge_detection_shader;
	ShaderManager()
	{
		//Skybox shader
		skybox_shader = new Shader("Shaders/skybox.vert", "Shaders/skybox.frag");
		//Default shader
		default_shader = new Shader("Shaders/model.vert", "Shaders/model.frag");
		//picking shader
		picking_shader = new Shader("Shaders/Picking.vert", "Shaders/Picking.frag");
		//effects shader
		//1st Inversion Effect
		inversion_shader = new Shader("Shaders/PostProcessing.vert", "Shaders/Inversion.frag");
		//2nd Grayscale Effect
		grayscale_shader = new Shader("Shaders/PostProcessing.vert", "Shaders/Grayscale.frag");
		//3rd Sharpen Effect
		sharpen_shader = new Shader("Shaders/PostProcessing.vert", "Shaders/Sharpen.frag");
		//4th Blur Effect
		blur_shader = new Shader("Shaders/PostProcessing.vert", "Shaders/Blur.frag");
		//5th Edge-Detection Effect
		edge_detection_shader = new Shader("Shaders/PostProcessing.vert", "Shaders/Edge-Detection.frag");
		cur_effect_shader = inversion_shader;
	}
};