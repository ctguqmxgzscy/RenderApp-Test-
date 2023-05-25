#include"Material.h"

Material::~Material()
{
	m_shader = nullptr;
}

BlingPhongMaterial::~BlingPhongMaterial()
{
}

BlingPhongMaterial::BlingPhongMaterial()
{
	this->Diffuse = { 1.0f,1.0f,1.0f };
	this->Ambient = { 0.01f,0.01f,0.01f };
	this->Specular = { 1.0f,1.0f,1.0 };
	this->type = BlingPhong;
	this->Shininess = 32.f;
	this->normalMapping = false;
	this->diffuseMapping = false;
	this->specularMapping = false;
}


BlingPhongMaterial::BlingPhongMaterial(const BlingPhongMaterial& rhs)
{
	this->Ambient = rhs.Ambient;
	this->Diffuse = rhs.Diffuse;
	this->type = BlingPhong;
	this->Shininess = rhs.Shininess;
	this->Specular = rhs.Specular;
	this->normalMapping = false;
	this->diffuseMapping = false;
	this->specularMapping = false;
	this->m_shader = rhs.m_shader;
}

void BlingPhongMaterial::setShaderProp()
{
	m_shader->use();
	m_shader->setVec3("material.ambient", Ambient);
	m_shader->setVec3("material.diffuse", Diffuse);
	m_shader->setVec3("material.specular", Specular);
	m_shader->setFloat("material.shininess", Shininess);
	m_shader->setBool("material.isNormalMapping", normalMapping);
	m_shader->setBool("material.isDiffuseMapping", diffuseMapping);
	m_shader->setBool("material.isSpecularMapping", specularMapping);
}

PBRMaterial::~PBRMaterial()
{
}

PBRMaterial::PBRMaterial()
{
	this->type = PBR;
	this->albedo = glm::vec3(0.5f, 0.0f, 0.0f);
	this->ao = 1.0f;
	this->metallic = 0.1f;
	this->roughness = 0.1f;
	this->albedoMapping = false;
	this->metallicMapping = false;
	this->normalMapping = false;
	this->roughnessMapping = false;
	this->aoMapping = false;
}

PBRMaterial::PBRMaterial(const PBRMaterial& rhs)
{
	this->albedo = rhs.albedo;
	this->ao = rhs.ao;
	this->metallic = rhs.metallic;
	this->roughness = rhs.roughness; 
	this->type = PBR;
	this->albedoMapping = false;
	this->metallicMapping = false;
	this->normalMapping = false;
	this->roughnessMapping = false;
	this->aoMapping = false;
	this->m_shader = rhs.m_shader;
}

void PBRMaterial::setShaderProp()
{
	m_shader->use();
	m_shader->setVec3("material.albedo", albedo);
	m_shader->setFloat("material.ao", ao);
	m_shader->setFloat("material.metallic", metallic);
	m_shader->setFloat("material.roughness", roughness);
	m_shader->setBool("material.normalMapping", normalMapping);
	m_shader->setBool("material.albedoMapping", albedoMapping);
	m_shader->setBool("material.metallicMapping", metallicMapping);
	m_shader->setBool("material.roughnessMapping", roughnessMapping);
	m_shader->setBool("material.aoMapping", aoMapping);
}
