#include"Model.h"
#include "stb_image.h"

Model::Model(const char* path)
{
	loadModel(path);
}

Model::~Model()
{
	this->directory.clear();
	this->meshes.clear();
	this->textures_loaded.clear();
}

void Model::Draw(Shader shader)
{
	if (this->isPicking)
	{
		shader.use();
		shader.setFloat("MeshSize", this->meshes.size());
	}

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		if (this->isPicking) {
			meshes[i].Draw_PickingEffects(shader, i);
		}
		else
			meshes[i].Draw(shader);

	}
}

void Model::Draw_DefaultEffects(Shader shader) 
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
			meshes[i].Draw_Mesh_SimpleColor(shader);
	}
}

void Model::Draw(Shader shader, unsigned int excluded_index)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		if (i != excluded_index)
			meshes[i].Draw(shader);
	}
}

void Model::EnablePicking()
{
	this->isPicking = true;
}

void Model::DisablePicking()
{

	this->isPicking = false;
}

void Model::loadModel(std::string const &path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
		aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	//absolute path
	if (directory == path)
		directory = path.substr(0, path.find_last_of('\\'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	//һ���ڵ��ӵ�ж�����������
	//��Ϊ����ڵ���ܱ���Ҳ��һ���ɶ����������ɵ�ģ��(������)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<unsigned int>indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y,
			mesh->mVertices[i].z);
		if (mesh->HasNormals()) {
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y,
				mesh->mNormals[i].z);
		}
		else {
			printf("There is anymore normal!\n");
		}

		//��������Ƿ�ӵ����������,�еĻ�ֻ��õ�һ����������
		if (mesh->mTextureCoords[0]) {
			vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y);
		}
		else
			vertex.TexCoord = glm::vec2(0.f, 0.f);
		vertices.push_back(vertex);
	}

	//һ�������ɺܶ������
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		//һ�����ɶ������+�������ƶ���
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	//�жϸ��������õ���ĳЩ����
	if (mesh->mMaterialIndex >= 0) {
		//ͨ�����loadMaterialTextures��ȡ����������Ҫ������diffuse��specular��ͼ
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture>diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture>specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture>normalMaps = loadMaterialTextures(material,
			aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> heightMaps = loadMaterialTextures(material, 
			aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	return Mesh(vertices,indices,textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			//���û���ع�������������
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format == GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}
