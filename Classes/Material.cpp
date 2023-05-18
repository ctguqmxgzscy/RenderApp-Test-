#include"Material.h"

Material::Material()
{
	this->Diffuse = { 1.0f,1.0f,1.0f };
	this->Ambient = { 0.01f,0.01f,0.01f };
	this->Specular = { 1.0f,1.0f,1.0 };
	this->Name = std::string("Default_Material");
	this->Shininess = 32.f;
	this->normalMapping = false;
	this->diffuseMapping = false;
	this->specularMapping = false;
}

Material::Material(const Material& rhs)
{
	this->Ambient = rhs.Ambient;
	this->Diffuse = rhs.Diffuse;
	this->Name = std::string(rhs.Name.c_str());
	this->Shininess = rhs.Shininess;
	this->Specular = rhs.Specular;
	this->normalMapping = false;
	this->diffuseMapping = false;
	this->specularMapping = false;

	this->m_shader = rhs.m_shader;
}

void Material::setShader(Shader* shader)
{
	m_shader = shader;
}

void Material::setShaderProp()
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