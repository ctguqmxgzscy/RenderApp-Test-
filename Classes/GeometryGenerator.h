#pragma once
#include<cstdint>
#include<vector>
#include"Shader.h"
#include"../glm/glm.hpp"
#include"../glm/gtc/matrix_transform.hpp"
#include"../glm/gtc/type_ptr.hpp"
#include"Material.h"

using namespace glm;

class GeometryGenerator {
public:

	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

	struct Vertex {
		Vertex() {}
		Vertex(const vec3& p, 
			const vec3& n,
			const vec3& t, 
			const vec2& uv) :
			Position(p),
			Normal(n),
			TangentU(t),
			TexC(uv) {}
		Vertex(float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) :
			Position(px, py, pz),
			Normal(nx, ny, nz),
			TangentU(tx, ty, tz),
			TexC(u, v) {}

		vec3 Position;
		vec3 Normal;
		vec3 TangentU;
		vec2 TexC;
	};

	struct MeshData {
		std::vector<Vertex>Vertices;
		std::vector<unsigned int> Indices32;

		std::vector<uint16>& GetIndices16() {
			if (mIndices16.empty()) {
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i) {
					mIndices16[i] = static_cast<uint16>(Indices32[i]);
				}
				return mIndices16;
			}
		}

	private:
		std::vector<uint16>mIndices16;
	};


	///<summary>
	/// Creates a box centered at the origin with the given dimensions, where each
	/// face has m rows and n columns of vertices.
	///</summary>
	MeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	MeshData CreateGeosphere(float radius, uint32 numSubdivisions);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
	MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	MeshData CreateGrid(float width, float depth, uint32 m, uint32 n);

	///<summary>
	/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
	///</summary>
	MeshData CreateQuad(float x, float y, float w, float h, float depth);

private:
	void Subdivide(MeshData& meshData);
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
		uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRaidus, float topRadius, float height,
		uint32 sliceCount, uint32 stackCount, MeshData& meshData);
};

class Node {

public:
	
	vec3 offset;
	vec3 center;
	float translation[3] = { 0.f,0.f,0.f };
	float rotation[3] = { 0.f,0.f,0.f };
	float scale[3] = { 1.0f,1.0f,1.0f };

	mat4 model_mat;
	float model_mat_value[16] = {
					1.f, 0.f, 0.f, 0.f,
					0.f, 1.f, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					0.f, 0.f, 0.f, 1.f };

	GeometryGenerator::MeshData meshData;
	Material material;
	Node() {
		this->material = Material();
		this->offset = glm::vec3(0.f);
		this->subling = NULL;
		this->child = NULL;
		this->parent = NULL;
		this->center = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	//void Draw(Shader shader);
	void updateModelMat();
	//void setupMeshData();
	//void cleanup();

	Node* subling;
	Node* child;
	Node* parent;
};