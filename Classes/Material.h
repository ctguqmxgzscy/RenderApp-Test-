#pragma once
#include<cstdint>
#include<vector>
#include"Shader.h"
#include<unordered_map>

enum MaterialType
{
	BlingPhong,
	PBR
};

class Material
{
public:
	MaterialType type;
	Shader* m_shader;
	~Material();
	void setShader(Shader* shader) { this->m_shader = shader; }
};

class PBRMaterial :public Material
{
public:
	~PBRMaterial();
	PBRMaterial();
	PBRMaterial(const PBRMaterial& rhs);

public:
	bool albedoMapping = false;
	bool normalMapping = false;
	bool metallicMapping = false;
	bool roughnessMapping = false;
	bool aoMapping = false;

	glm::vec3  albedo;
	float metallic;
	float roughness;
	float ao;

public:
	void setShaderProp();
};

class BlingPhongMaterial : public Material
{
public:
	~BlingPhongMaterial();
	BlingPhongMaterial();
	BlingPhongMaterial(const BlingPhongMaterial& rhs);
public:

	bool diffuseMapping = false;
	bool normalMapping = false;
	bool specularMapping = false;

	glm::vec3 Diffuse;
	glm::vec3 Ambient;
	glm::vec3 Specular;
	float Shininess;
public:
	void setShaderProp();
};
