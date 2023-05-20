#include"GeometryGenerator.h"
#include<algorithm>


GeometryGenerator::MeshData 
GeometryGenerator::CreateBox(float width, float height, float depth, uint32 numSubdivisions)
{
	Vertex v[24];
	MeshData meshData;

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

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

	 meshData.Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	uint32 i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.Indices32.assign(&i[0], &i[36]);

	// Put a cap on the number of subdivisions.
	numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

	for (uint32 i = 0; i < numSubdivisions; ++i)
		Subdivide(meshData);

	return meshData;
}

GeometryGenerator::MeshData
GeometryGenerator::CreateSphere(float radius, uint32 sliceCount, uint32 stackCount) {
	MeshData meshData;

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	meshData.Vertices.push_back(topVertex);

	float phiStep = glm::pi<float>() / stackCount;
	float thetaStep = 2.0f * glm::pi<float>() / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (uint32 i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (uint32 j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex v;

			// spherical to cartesian
			v.Position.x = radius * sinf(phi) * cosf(theta);
			v.Position.y = radius * cosf(phi);
			v.Position.z = radius * sinf(phi) * sinf(theta);

			// Partial derivative of P with respect to theta
			v.TangentU.x = -radius * sinf(phi) * sinf(theta);
			v.TangentU.y = 0.0f;
			v.TangentU.z = +radius * sinf(phi) * cosf(theta);

			v.TangentU = glm::normalize(v.TangentU);
			v.Normal = glm::normalize(v.Position);

			v.TexC.x = theta / glm::two_pi<float>();
			v.TexC.y = phi / glm::pi<float>();

			meshData.Vertices.push_back(v);
		}
	}

	meshData.Vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (uint32 i = 1; i <= sliceCount; ++i)
	{
		meshData.Indices32.push_back(0);
		meshData.Indices32.push_back(i + 1);
		meshData.Indices32.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	uint32 baseIndex = 1;
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 i = 0; i < stackCount - 2; ++i)
	{
		for (uint32 j = 0; j < sliceCount; ++j)
		{
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j);
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
			meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	uint32 southPoleIndex = (uint32)meshData.Vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(southPoleIndex);
		meshData.Indices32.push_back(baseIndex + i);
		meshData.Indices32.push_back(baseIndex + i + 1);
	}

	return meshData;
}

GeometryGenerator::MeshData 
GeometryGenerator::CreateGridLine(float width, float depth, uint32 m, uint32 n)
{
	MeshData meshData;

	uint32 vertexCount = m * n;
	uint32 lineCount = m + n;

	//
	// Create the vertices.
	//

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	meshData.Vertices.resize(vertexCount);
	for (uint32 i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32 j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			meshData.Vertices[i * n + j].Position = glm::vec3(x, 0.0f, z);
			meshData.Vertices[i * n + j].Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			meshData.Vertices[i * n + j].TangentU = glm::vec3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			meshData.Vertices[i * n + j].TexC.x = j * du;
			meshData.Vertices[i * n + j].TexC.y = i * dv;
		}
	}

	//
	// Create the indices.
	//
	meshData.Indices32.resize(lineCount * 2); // 3 indices per face

	// Iterate over each quad and compute indices.
	uint32 k = 0;
	for (uint32 i = 0; i < m; ++i)
	{
		meshData.Indices32[k] = i * n;
		meshData.Indices32[k + 1] = i * n + n - 1;
		k += 2;
	}
	for (uint32 j = 0; j < n; ++j)
	{
		meshData.Indices32[k] = j;
		meshData.Indices32[k + 1] = (m - 1) * n + j;
		k += 2;
	}

	return meshData;
}

GeometryGenerator::MeshData
GeometryGenerator::CreateGrid(float width, float depth, uint32 m, uint32 n)
{
	MeshData meshData;

	uint32 vertexCount = m * n;
	uint32 faceCount = (m - 1) * (n - 1) * 2;

	//
	// Create the vertices.
	//

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	meshData.Vertices.resize(vertexCount);
	for (uint32 i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint32 j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			meshData.Vertices[i * n + j].Position = glm::vec3(x, 0.0f, z);
			meshData.Vertices[i * n + j].Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			meshData.Vertices[i * n + j].TangentU = glm::vec3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			meshData.Vertices[i * n + j].TexC.x = j * du;
			meshData.Vertices[i * n + j].TexC.y = i * dv;
		}
	}

	//
	// Create the indices.
	//

	meshData.Indices32.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	uint32 k = 0;
	for (uint32 i = 0; i < m - 1; ++i)
	{
		for (uint32 j = 0; j < n - 1; ++j)
		{
			meshData.Indices32[k] = i * n + j;
			meshData.Indices32[k + 1] = i * n + j + 1;
			meshData.Indices32[k + 2] = (i + 1) * n + j;

			meshData.Indices32[k + 3] = (i + 1) * n + j;
			meshData.Indices32[k + 4] = i * n + j + 1;
			meshData.Indices32[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	return meshData;
}

void
GeometryGenerator::Subdivide(MeshData& meshData) {
	// Save a copy of the input geometry.
	MeshData inputCopy = meshData;

	meshData.Vertices.resize(0);
	meshData.Indices32.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	uint32 numTris = (uint32)inputCopy.Indices32.size() / 3;
	for (uint32 i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex m0 = MidPoint(v0, v1);
		Vertex m1 = MidPoint(v1, v2);
		Vertex m2 = MidPoint(v0, v2);

		//
		// Add new geometry.
		//

		meshData.Vertices.push_back(v0); // 0
		meshData.Vertices.push_back(v1); // 1
		meshData.Vertices.push_back(v2); // 2
		meshData.Vertices.push_back(m0); // 3
		meshData.Vertices.push_back(m1); // 4
		meshData.Vertices.push_back(m2); // 5

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
