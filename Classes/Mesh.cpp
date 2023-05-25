#include"Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = new BlingPhongMaterial();
	setupMesh();
}


Mesh Mesh::operator=(Mesh&& rhs) noexcept
{
	//ALWAYS check for self-assignment.
	if (this != &rhs)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &VBO);
		this->VAO = 0;
		this->EBO = 0;
		this->VBO = 0;		
		std::swap(this->VAO, rhs.VAO);
		std::swap(this->VBO, rhs.VBO);
		std::swap(this->EBO, rhs.EBO);

		delete this->material;
		this->material = rhs.material;
		rhs.material = nullptr;

		vertices.clear();
		textures.clear();
		indices.clear();
		this->vertices = std::move(rhs.vertices);
		this->textures = std::move(rhs.textures);
		this->indices = std::move(rhs.indices);

	}
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	this->VAO = 0;
	glDeleteBuffers(1, &EBO);
	this->EBO = 0;
	glDeleteBuffers(1, &VBO);
	this->VBO = 0;
	vertices.clear();
	textures.clear();
	indices.clear();
}

Mesh::Mesh(Mesh&& rhs) noexcept
{
	this->vertices = std::move(rhs.vertices);
	this->textures = std::move(rhs.textures);
	this->indices = std::move(rhs.indices);
	this->VAO = rhs.VAO;
	this->VBO = rhs.VBO;
	this->EBO = rhs.EBO;
	this->material = rhs.material;
	rhs.material = nullptr;
	rhs.EBO = 0;
	rhs.VAO = 0;
	rhs.VBO = 0;
}

Mesh::Mesh(MaterialType t)
{
	if (t == BlingPhong)
		material = new BlingPhongMaterial();
	else if (t == PBR)
		material = new PBRMaterial();
}

void Mesh::GetDataFrom(GeometryGenerator::MeshData data)
{
	for (size_t i = 0; i < data.Vertices.size(); i++)
	{
		Vertex vert;
		vert.Position = data.Vertices[i].Position;
		vert.Normal = data.Vertices[i].Normal;
		vert.Tangent = data.Vertices[i].TangentU;
		vert.TexCoord = data.Vertices[i].TexC;
		vertices.push_back(std::move(vert));
	}

	for (size_t i = 0; i < data.Indices32.size(); i++)
	{
		indices.push_back(data.Indices32[i]);
	}
	setupMesh();
}

void Mesh::Draw()
{
	material->m_shader->use();
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	if (material->type == BlingPhong)
	{
		for (int i = 0; i < textures.size(); i++) 
		{
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
			{
				if (material->type == BlingPhong)
				{
					auto m = static_cast<BlingPhongMaterial*>(material);
					m->diffuseMapping = true;
					number = std::to_string(diffuseNr++);
				}
			}
			else if (name == "texture_specular")
			{
				if (material->type == BlingPhong)
				{
					auto m = static_cast<BlingPhongMaterial*>(material);
					m->specularMapping = true;
					number = std::to_string(specularNr++);
				}
			}
			else if (name == "texture_normal")
			{
				if (material->type == BlingPhong)
				{
					auto m = static_cast<BlingPhongMaterial*>(material);
					m->normalMapping = true;
					number = std::to_string(normalNr++);
				}
			}
			else if (name == "texture_height")
				number = std::to_string(heightNr++);

			if (material->type == BlingPhong)
			{
				material->m_shader->setFloat(("material." + name + number).c_str(), i);
			}
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);
	}

	if(material->type == BlingPhong)
	{
		auto m = static_cast<BlingPhongMaterial*>(material);
		m->setShaderProp();
	}
	else if (material->type == PBR)
	{
		auto m = static_cast<PBRMaterial*>(material);
		size_t p = 0;
		if (m->albedoMapping)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, textures[p++].id);
		}
		if (m->aoMapping)
		{
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, textures[p++].id);
		}
		if (m->metallicMapping)
		{
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, textures[p++].id);
		}
		if (m->normalMapping)
		{
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, textures[p++].id);
		}
		if (m->roughnessMapping)
		{
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, textures[p++].id);
		}
		m->setShaderProp();
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::Draw_PickingEffects(unsigned int i)
{
	glBindVertexArray(VAO);
	material->m_shader->use();
	material->m_shader->setUint("MeshIndex", i);
	material->m_shader->setUint("IndicesSize", indices.size());
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::Draw_Mesh_SimpleColor()
{
	glBindVertexArray(VAO);
	material->m_shader->use();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
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
