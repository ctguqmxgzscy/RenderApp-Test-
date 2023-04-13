#include"Material.h"


void Material::setShader(Shader shader)
{
	shader.setVec4("material.ambient", Ambient);
	shader.setVec4("material.diffuse", Diffuse);
	shader.setVec4("material.specular", Specular);
	shader.setFloat("material.shininess", Shininess);
}