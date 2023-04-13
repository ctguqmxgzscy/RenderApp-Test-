#pragma once
#include<cstdint>
#include<vector>
#include"Shader.h"
#include<unordered_map>

class Material
{
public:

	std::string Name;
	glm::vec4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	glm::vec4 Ambient = { 0.01f,0.01f,0.01f,1.0f };
	glm::vec4 Specular = { 1.0f,1.0f,1.0,1.0f };
	float Shininess = 0.25f;
	glm::mat4 MatTransform = glm::mat4(1.0f);
public:
	void setShader(Shader shader);
};

