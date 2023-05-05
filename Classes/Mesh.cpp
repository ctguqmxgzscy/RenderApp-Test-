#include"Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

//Mesh::~Mesh()
//{
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &EBO);
//	glDeleteBuffers(1, &VBO);
//
//	vertices.clear();
//	textures.clear();
//	indices.clear();
//}

void Mesh::Draw(Shader shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); 
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		shader.setFloat(("material."+name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
	shader.use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::Draw_PickingEffects(Shader shader,unsigned int i)
{
	glBindVertexArray(VAO);
	shader.use();
	shader.setUint("MeshIndex", i);
	shader.setUint("IndicesSize", indices.size());
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::Draw_Mesh_SimpleColor(Shader shader)
{
	glBindVertexArray(VAO);
	shader.use();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::Draw_Triangle_SimpleColor(Shader _exclude_shader,Shader _simple_color_shader, unsigned int primitiveIndex)
{
	//设定模型着色器的纹理贴图参数
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		_exclude_shader.setFloat(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);
	_exclude_shader.use();
	glBindVertexArray(VAO);
	//绘制第primitiveIndex 个三角形以前的所有三角形
	glDrawElements(GL_TRIANGLES, 3 * primitiveIndex, GL_UNSIGNED_INT, 0);
	//绘制第primitiveIndex 个三角形以后的所有三角形
	glDrawElements(GL_TRIANGLES, indices.size() - 3 * (primitiveIndex + 1), GL_UNSIGNED_INT,
		(const GLvoid*)((primitiveIndex + 1) * 3 * sizeof(GLuint)));
	glBindVertexArray(0);
	//绘制被选中的第primitiveIndex个三角形，必须是填充模式绘制，
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(VAO);
	_simple_color_shader.use();
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)(primitiveIndex * 3 * sizeof(GLuint)));
	glBindVertexArray(0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0],
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
	glEnableVertexAttribArray(2);
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// ids
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

	// weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
	glBindVertexArray(0);

}
