#include"Model.h"
#include "stb_image.h"

Model::Model(const char* path)
{
	loadModel(path);
}

Model::Model(Mesh mesh)
{
	this->meshes.push_back(std::move(mesh)); 
	this->directory = "";
	this->path = "";
}

Model::Model(const Model& rhs)
{
	if (!rhs.directory.empty())
		this->directory = std::string(rhs.directory);
	if (!rhs.path.empty())
		this->path = std::string(rhs.path);
	this->meshes = std::vector<Mesh>(rhs.meshes);
	this->isPicking = rhs.isPicking;
	this->textures_loaded = std::vector<Texture>(rhs.textures_loaded);
}

Model::~Model()
{
	this->directory.clear();
	this->meshes.clear();
	this->textures_loaded.clear();
}

void Model::Draw()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		if (this->isPicking) {
			meshes[i].Draw_PickingEffects(i);
		}
		else
			meshes[i].Draw();

	}
}

void Model::Draw_DefaultEffects() 
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
			meshes[i].Draw_Mesh_SimpleColor();
	}
}

void Model::Draw(unsigned int excluded_index)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		if (i != excluded_index)
			meshes[i].Draw();
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
		aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	this->path = path;
	directory = path.substr(0, path.find_last_of('/'));
	//absolute path
	if (directory == path)
		directory = path.substr(0, path.find_last_of('\\'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	//一个节点可拥有多套网格数据
	//因为这个节点可能本身也是一个由多个子网格组成的模型(父网格)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(std::move(processMesh(mesh, scene)));
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
		//检测网格是否拥有纹理坐标,有的话只获得第一组纹理坐标
		if (mesh->mTextureCoords[0]) {
			vertex.TexCoord = glm::vec2(mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y);
		}
		else
			vertex.TexCoord = glm::vec2(0.f, 0.f);
		if (mesh->mTangents)
		{
			vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y,
				mesh->mTangents[i].z);

		}
		if (mesh->mBitangents)
		{
			vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y,
				mesh->mBitangents[i].z);
		}
		vertices.push_back(vertex);
	}
	//一个网格由很多面组成
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		//一个面由多个顶点+索引绘制而成
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	
	//判断该网格是用到了某些材质
	if (mesh->mMaterialIndex >= 0) {

		//通过获得loadMaterialTextures获取该网格所需要的所有diffuse和specular贴图
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture>diffuseMaps = loadMaterialTextures(material,
			aiTextureType_AMBIENT, "texture_diffuse");
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

size_t Model::getVertexSum()
{
	size_t sum = 0;
	for (size_t i = 0; i < meshes.size(); i++)
	{
		sum += meshes[i].vertices.size();
	}
	return sum;
}

size_t Model::getTextureSum()
{
	size_t sum = 0;
	for (size_t i = 0; i < meshes.size(); i++)
	{
		sum += meshes[i].textures.size();
	}
	return sum;
}

size_t Model::getIndexSum()
{
	size_t sum = 0;
	for (size_t i = 0; i < meshes.size(); i++)
	{
		sum += meshes[i].indices.size();
	}
	return sum;
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
			//如果没加载过该纹理，加载它
			Texture texture;
			if (typeName == "texture_diffuse")
				texture.id = TextureFromFile(str.C_Str(), directory, true);
			else
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
		GLenum internalFormat;
		GLenum format;
		if (nrComponents == 1)
			internalFormat = format = GL_RED;
		else if (nrComponents == 3)
		{
			format = GL_RGB;
			internalFormat = gamma ? GL_SRGB : GL_RGB;
		}

		else if (nrComponents == 4)
		{
			internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
	texture_ref_count[textureID]++;
	return textureID;
}
