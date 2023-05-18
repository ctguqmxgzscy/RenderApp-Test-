#pragma once
#include<iostream>
#include<string>
#include<vector>

#include"Shader.h"
#include"GeometryGenerator.h"
#include"../glm/glm.hpp"
#include"../glm/gtc/matrix_transform.hpp"
#include"../glm/gtc/type_ptr.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define MAX_BONE_INFLUENCE 4
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};
struct Texture
{
	unsigned int id;
	std::string type;
	aiString path;

	~Texture()
	{
		type.clear();
		path.Clear();
	}
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures; 
	Material material;
public:

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		std::vector<Texture> textures);
	Mesh() { this->material = Material(); };
	//~Mesh();
	 unsigned int& get_VAO() { return this->VAO; }
	 unsigned int& get_VBO() { return this->VBO; }
	 unsigned int& get_EBO() { return this->EBO; }

	 void GetDataFrom(GeometryGenerator::MeshData data);
	void Draw();
	void Draw_PickingEffects(unsigned int i);
	void Draw_Mesh_SimpleColor();
private:
	bool isPicking;
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};

