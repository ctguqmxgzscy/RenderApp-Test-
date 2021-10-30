#pragma once
#include"Mesh.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
class Model {
public:
	Model(const char* path);
	void Draw(Shader shader);
	void Draw(Shader shader,unsigned int excluded_index);
public:
	std::vector<Mesh>& get_Meshes() { return this->meshes; }
public:
	void EnablePicking(); 
	void DisablePicking();
	
private:
	
	bool isPicking = false;

	std::vector<Mesh> meshes;
	std::string directory;
	//与顶点复用相似，防止纹理复用
	std::vector<Texture> textures_loaded;
private:

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
		aiTextureType type, std::string typeName);
	void loadModel(std::string const &path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
