#pragma once
#include<cstdint>
#include<vector>
#include"Shader.h"
#include<unordered_map>

class Material
{
public:
	Material();
	Material(const Material& rhs);
public:
	Shader* m_shader;
	std::string Name;
	bool diffuseMapping = false;
	bool normalMapping = false;
	bool specularMapping = false;

	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;
	float Shininess;
public:
	void setShader(Shader* shader);
	void setShaderProp();
};

