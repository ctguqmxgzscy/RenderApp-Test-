#pragma once
#include<cstdint>
#include<vector>
#include"Shader.h"
#include<unordered_map>

class Material
{
public:
	Material() {};
	Material(const Material& rhs);
	std::string Name;

	bool diffuseMapping = false;
	bool normalMapping = false;
	bool specularMapping = false;

	glm::vec3 Diffuse = { 1.0f,1.0f,1.0f };
	glm::vec3 Ambient = { 0.01f,0.01f,0.01f };
	glm::vec3 Specular = { 1.0f,1.0f,1.0 };
	float Shininess = 32.f;
public:
	void setShader(Shader shader);
};

