#include"Material.h"


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
}

void Material::setShader(Shader shader)
{
	shader.setVec3("material.ambient", Ambient);
	shader.setVec3("material.diffuse", Diffuse);
	shader.setVec3("material.specular", Specular);
	shader.setFloat("material.shininess", Shininess);
	shader.setBool("material.isNormalMapping", normalMapping);
	shader.setBool("material.isDiffuseMapping", diffuseMapping);
	shader.setBool("material.isSpecularMapping", specularMapping);
}