#pragma once
#include"Mesh.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
class Model {
public:
	Model(const char* path);
	Model(Mesh mesh);
	~Model();
	//��������������
	void Draw(Shader shader);
	void Draw_DefaultEffects(Shader shader);
	//���Ƴ�excluded_index֮�����������
	void Draw(Shader shader,unsigned int excluded_index);
public:
	std::vector<Mesh>& get_Meshes() { return this->meshes; }
	std::string getDirectory() { return this->directory; }
	std::string	getPath() { return this->path; }
	size_t getVertexSum();
	size_t getTextureSum();
	size_t getIndexSum();
public:
	void EnablePicking(); 
	void DisablePicking();
	
private:
	
	bool isPicking = false;

	std::vector<Mesh> meshes;
	std::string directory;
	std::string path;
	//�붥�㸴�����ƣ���ֹ������
	std::vector<Texture> textures_loaded;
private:

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
		aiTextureType type, std::string typeName);
	void loadModel(std::string const &path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

};
