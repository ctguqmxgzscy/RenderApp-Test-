#include"GeometryGenerator.h"
#include<algorithm>


GeometryGenerator::MeshData 
GeometryGenerator::CreateBox(float width, float height, float depth, uint32 numSubdivisions)
{
	MeshData meshData;

	Vertex v[24];

	float w2 = 0.5 * width;
	float h2 = 0.5 * height;
	float d2 = 0.5 * depth;

#pragma region Create Vertex Data
	//Vertex Structure
	//			------postion------ -----normal-----  -----Tangent------- --TexCoord---
	//	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	//Front 
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//Back
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, +1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, +1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//Top
	v[8] = Vertex(-w2, +h2, -d2, 0.0f, +1.0f, 0.0f, +1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, +1.0f, 0.0f, +1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, +1.0f, 0.0f, +1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, +1.0f, 0.0f, +1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//Bottom						    
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//Left
	v[16] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	//Right
	v[20] = Vertex(+w2, -h2, -d2, +1.0f, 0.0f, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, +1.0f, 0.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, 1.0f);
	v[22] = Vertex(+w2, +h2, +d2, +1.0f, 0.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, +1.0f, 0.0f, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	//Copy Vertex Data
	meshData.Vertices.assign(&v[0], &v[24]);
#pragma endregion

#pragma region Create Index Data
	uint32 i[36];
	//Front Index
	i[0] = 0, i[1] = 1, i[2] = 2;
	i[3] = 0, i[4] = 2, i[5] = 3;
	//Back Index
	i[6] = 4, i[7] = 5, i[8] = 6;
	i[9] = 4, i[10] = 6, i[11] = 7;
	//Top Index
	i[12] = 8, i[13] = 9, i[14] = 10;
	i[15] = 8, i[16] = 10, i[17] = 11;
	//Bottom Index
	i[18] = 12, i[19] = 13, i[20] = 14;
	i[21] = 12, i[22] = 14, i[23] = 15;
	//Left Index
	i[24] = 16, i[25] = 17, i[26] = 18;
	i[27] = 16, i[28] = 18, i[29] = 19;
	//Right Index
	i[30] = 20, i[31] = 21, i[32] = 22;
	i[33] = 20, i[34] = 22, i[35] = 23;
	//Copy Index Data
	meshData.Indices32.assign(&i[0], &i[36]);
#pragma endregion

	//Put a cap on the number of subdivisions
	numSubdivisions = std::min<uint32>(numSubdivisions, 6u);
	for (uint32 i = 0; i < numSubdivisions; ++i) {
		Subdivide(meshData);
	}

	return meshData;
}

GeometryGenerator::MeshData
GeometryGenerator::CreateSphere(float radius, uint32 sliceCount, uint32 stackCount) {
	MeshData meshData;
#pragma region Create Vertex Data
	// Compute the vertices stating at the top pole and moving down the stacks.
	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	meshData.Vertices.push_back(topVertex);

	float phiStep = glm::pi<float>() / stackCount;
	float thetaStep = 2.0f * glm::pi<float>() / sliceCount;

	//Compute vertices for each stack ring(do ot count the poles as rings);
	for (uint32 i = 1; i <= stackCount - 1; ++i) {
		float phi = i * phiStep;

		//Vertices of ring.
		for (uint32 j = 0; j <= sliceCount; ++j) {
			float theta = j * thetaStep;
			Vertex v;

			//spherical to cartesian
			v.Position.x = radius * sinf(phi) * cosf(theta);
			v.Position.y = radius * cosf(phi);
			v.Position.z = radius * sinf(phi) * sinf(theta);

			//Partial derivative of P with respect to theta
			v.TangentU.x = -radius * sinf(phi) * sinf(theta);
			v.TangentU.y = 0.0f;
			v.TangentU.z = +radius * sinf(phi) * cosf(theta);

			v.TangentU = normalize(v.TangentU);
			v.Normal = normalize(v.Position);
			//x > 0 &x < 1   y > =0&&y < =1 
			v.TexC.x = theta / glm::pi<float>();
			v.TexC.y = phi / glm::pi<float>();

			meshData.Vertices.push_back(v);
		}
	}

	meshData.Vertices.push_back(bottomVertex);
#pragma endregion

#pragma region Create Index Data
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	for (uint32 i = 1; i <= sliceCount; ++i) {
		meshData.Indices32.push_back(0);
		meshData.Indices32.push_back(i + 1);
		meshData.Indices32.push_back(i);
	}
	// Compute indices for inner stacks (not connected to poles).
	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	uint32 baseIndex = 1;
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 i = 0; i < stackCount - 2; ++i) {
		for (uint32 j = 0; j < sliceCount; ++j) {
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j);
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	// South pole vertex was added last.
	uint32 southPoleIndex = (uint32)meshData.Vertices.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i) {
		meshData.Indices32.push_back(southPoleIndex);
		meshData.Indices32.push_back(baseIndex + i);
		meshData.Indices32.push_back(baseIndex + i + 1);
	}
#pragma endregion


	return meshData;
}

void
GeometryGenerator::Subdivide(MeshData& meshData) {
	MeshData copy = meshData;

	meshData.Vertices.reserve(0);
	meshData.Indices32.reserve(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	uint32 numTriangles = (uint32)meshData.Vertices.size() / 3;
	for (uint32 i = 0; i < numTriangles; ++i) {

		Vertex v0 = copy.Vertices[copy.Indices32[i * 3 + 0]];
		Vertex v1 = copy.Vertices[copy.Indices32[i * 3 + 1]];
		Vertex v2 = copy.Vertices[copy.Indices32[i * 3 + 2]];

		//Generate the midpoints
		Vertex m0 = MidPoint(v0, v1);
		Vertex m1 = MidPoint(v1, v2);
		Vertex m2 = MidPoint(v0, v2);

		meshData.Vertices.push_back(v0);//0
		meshData.Vertices.push_back(v1);//1
		meshData.Vertices.push_back(v2);//2
		meshData.Vertices.push_back(m0);//3
		meshData.Vertices.push_back(m1);//4
		meshData.Vertices.push_back(m2);//5

		meshData.Indices32.push_back(i * 6 + 0);
		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 5);
		
		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 4);
		meshData.Indices32.push_back(i * 6 + 5);

		meshData.Indices32.push_back(i * 6 + 5);
		meshData.Indices32.push_back(i * 6 + 4);
		meshData.Indices32.push_back(i * 6 + 2);

		meshData.Indices32.push_back(i * 6 + 3);
		meshData.Indices32.push_back(i * 6 + 1);
		meshData.Indices32.push_back(i * 6 + 4);
	}
}

GeometryGenerator::Vertex
GeometryGenerator::MidPoint(const Vertex& v0, const Vertex& v1) {
	Vertex v;
	v.Position = 0.5f * (v0.Position + v1.Position);
	v.Normal = normalize(0.5f * (v0.Normal + v1.Normal));
	v.TangentU = normalize(0.5f * (v0.TangentU + v1.TangentU));
	v.TexC = 0.5f * (v0.TexC + v1.TexC);
	return v;
}

void Node::updateModelMat()
{
	//如果该结点没有父节点，那么就对自己的model矩阵就行
	//先进行缩放、再进行旋转和位移的操作从而变换到世界坐标系中
	if (!this->parent) {
		this->model_mat = glm::mat4(1.0f);

		glm::vec3 trans_vec, rotate_vec, scale_vec;
		trans_vec = glm::make_vec3(translation);
		scale_vec = glm::make_vec3(scale);

		this->model_mat = glm::translate(model_mat, trans_vec);
		this->model_mat = glm::rotate(model_mat, glm::radians(this->rotation[0]), normalize(vec3(1.0f, 0.0f, 0.0f)));
		this->model_mat = glm::rotate(model_mat, glm::radians(this->rotation[1]), normalize(vec3(0.0f, 1.0f, 0.0f)));
		this->model_mat = glm::rotate(model_mat, glm::radians(this->rotation[2]), normalize(vec3(0.0f, 0.0f, 1.0f)));
		this->model_mat = glm::scale(model_mat, scale_vec);

	}
	else {
	//如果该结点有父节点，那么必须处理所有父节点的关系
	//先遍历该结点有多少个父节点
		Node* p = this;
		int cnt = 0, copy;
		this->model_mat = glm::mat4(1.0f);
		while (p->parent)
		{
			p = p->parent;
			cnt++;
		}
	//对父亲数目就行拷贝
		copy = cnt;
		p = this;
		while (cnt)
		{
			p = this;
			int k = cnt;
			while (k)
			{
				p = p->parent;
				k--;
			}
			if (cnt == copy) {

				glm::vec3 trans_vec, rotate_vec, scale_vec;
				trans_vec = glm::make_vec3(p->translation);
				rotate_vec = glm::make_vec3(p->rotation);
				scale_vec = glm::make_vec3(p->scale);

				//最后对根父节点进行旋转，位移操作，变换到世界坐标系的变换空间中
				this->model_mat = glm::translate(model_mat, trans_vec - p->center);
				this->model_mat = glm::rotate(model_mat, glm::radians(p->rotation[0]), normalize(vec3(1.0f, 0.0f, 0.0f)));
				this->model_mat = glm::rotate(model_mat, glm::radians(p->rotation[1]), normalize(vec3(0.0f, 1.0f, 0.0f)));
				this->model_mat = glm::rotate(model_mat, glm::radians(p->rotation[2]), normalize(vec3(0.0f, 0.0f, 1.0f)));
				this->model_mat = glm::translate(model_mat, p->center);
			}
			else {

				glm::vec3 trans_vec, rotate_vec, scale_vec;
				trans_vec = glm::make_vec3(p->translation);
				rotate_vec = glm::make_vec3(p->rotation);
				scale_vec = glm::make_vec3(p->scale);

				//如果不是根父节点，那么要变换到该层父节点的变换空间中
				this->model_mat = glm::translate(model_mat, p->offset - p->center);
				this->model_mat = glm::rotate(model_mat, glm::radians(p->rotation[0]), normalize(vec3(1.0f, 0.0f, 0.0f)));
				this->model_mat = glm::rotate(model_mat, glm::radians(p->rotation[1]), normalize(vec3(0.0f, 1.0f, 0.0f)));
				this->model_mat = glm::rotate(model_mat, glm::radians(p->rotation[2]), normalize(vec3(0.0f, 0.0f, 1.0f)));
				this->model_mat = glm::translate(model_mat, p->center);
			}
			cnt--;
		}
		//首先对自己就行变换，然后变换最近一层的父节点坐标系的变换空间中
		this->model_mat = glm::translate(model_mat, this->offset - this->center);
		this->model_mat = glm::rotate(model_mat, glm::radians(this->rotation[0]), normalize(vec3(1.0f, 0.0f, 0.0f))); 
		this->model_mat = glm::rotate(model_mat, glm::radians(this->rotation[1]), normalize(vec3(0.0f, 1.0f, 0.0f)));
		this->model_mat = glm::rotate(model_mat, glm::radians(this->rotation[2]), normalize(vec3(0.0f, 0.0f, 1.0f)));
		this->model_mat = glm::translate(model_mat, this->center);
		this->model_mat = glm::scale(model_mat, glm::make_vec3(scale));
		
	}
	//如果该结点还有孩子，还要更新孩子结点的model变换矩阵
	if (this->child) {
		this->child->updateModelMat();
	}
	//如果该结点有兄弟，兄弟结点也得更新model变换矩阵
	if (this->subling) {
		this->subling->updateModelMat();
	}
}

//void Node::setupMeshData()
//{
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, meshData.Vertices.size() * sizeof(GeometryGenerator::Vertex), 
//		&meshData.Vertices[0], GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.Indices32.size() * sizeof(unsigned int), 
//		&meshData.Indices32[0], GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GeometryGenerator::Vertex), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GeometryGenerator::Vertex), 
//		(void*)offsetof(GeometryGenerator::Vertex, Normal));
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GeometryGenerator::Vertex),
//		(void*)offsetof(GeometryGenerator::Vertex, TangentU));
//	glEnableVertexAttribArray(2);
//	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(GeometryGenerator::Vertex),
//		(void*)offsetof(GeometryGenerator::Vertex, TexC));
//	glEnableVertexAttribArray(3);
//}
//
//void Node::cleanup()
//{
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	glDeleteBuffers(1, &EBO);
//}

//void Node::Draw(Shader shader) {
//	shader.use();
//	shader.setMat4("model", this->model);
//
//	material.setShader(shader);
//
//	glBindVertexArray(VAO);
//	glDrawElements(GL_TRIANGLES, meshData.Indices32.size(), GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
//}
