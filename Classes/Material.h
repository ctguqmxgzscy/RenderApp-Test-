#pragma once
#include <cstdint>
#include <vector>
#include "Shader.h"
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define MAX_TEXTURE_COUNT 200

static size_t texture_ref_count[MAX_TEXTURE_COUNT];

struct Texture
{
	unsigned int id;
	std::string type;
	aiString path;
	Texture() {};
	~Texture()
	{
		if (texture_ref_count[id] == 1)
			glDeleteTextures(1, &id);
		else if (texture_ref_count[id] != 0)
			texture_ref_count[id]--;

		this->id = 0;
		type.clear();
		path.Clear();
	}
	Texture& operator=(const Texture& rhs)
	{
		id = rhs.id;
		texture_ref_count[id]++;
		type = rhs.type;
		path = rhs.path;
		return *this;
	}
	Texture(const Texture& rhs)
	{
		id = rhs.id;
		texture_ref_count[id]++;
		type = rhs.type;
		path = rhs.path;
	}

	Texture& operator=(Texture&& rhs)
	{
		if (this != &rhs)
		{
			if (texture_ref_count[id] == 1)
				glDeleteTextures(1, &id);
			else
				texture_ref_count[id]--;
			id = 0;
			std::swap(id, rhs.id);
			type = rhs.type;
			rhs.type = "";
			path = rhs.path;
			rhs.path = "";
		}
		return *this;
	}
	Texture(Texture&& rhs) noexcept
	{
		id = rhs.id;
		rhs.id = 0;
		type = rhs.type;
		rhs.type = "";
		path = rhs.path;
		rhs.path = "";
	}
};

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
	void setAlbedoMap(std::shared_ptr<Texture> albedoMap);
	void setNormalMap(std::shared_ptr<Texture> normalMap);
	void setMetallicMap(std::shared_ptr<Texture> metallicMap);
	void setRoughnessMap(std::shared_ptr<Texture> roughnessMap);
	void setAoMap(std::shared_ptr<Texture> aoMap);
	void setDisplacementMap(std::shared_ptr<Texture> displacementMap);

private:
	std::shared_ptr<Texture> mAlbedoMap = nullptr;
	std::shared_ptr<Texture> mNormalMap = nullptr;
	std::shared_ptr<Texture> mMetallicMap = nullptr;
	std::shared_ptr<Texture> mRoughnessMap = nullptr;
	std::shared_ptr<Texture> mAoMap = nullptr;
	std::shared_ptr<Texture> mDisplacementMap = nullptr;
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
	void setDiffuseMap(std::shared_ptr<Texture> diffuseMap);
	void setNormalMap(std::shared_ptr<Texture> normalMap);
	void setSpecularMap(std::shared_ptr<Texture> specularMap);

private:
	std::shared_ptr<Texture> mDiffuseMap = nullptr;
	std::shared_ptr<Texture> mSpecularMap = nullptr;
	std::shared_ptr<Texture> mNormalMap = nullptr;
};
