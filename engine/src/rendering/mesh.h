/*
 * Copyright (C) 2015 Subvision Studio
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef MESH_H
#define MESH_H

#include "../core/math3d.h"
#include "../rendering/material.h"
#include "../core/referenceCounter.h"

#include <string>
#include <vector>
#include <map>
#include <GL/glew.h>

static bool IsEngineComponent = true;

/**
 * Represents engine-compatible model used by renderer.
 */
class IndexedModel
{
public:
	IndexedModel() {}
	IndexedModel(const std::vector<unsigned int> indices, const std::vector<Vector3f>& positions, const std::vector<Vector2f>& texCoords,
		const std::vector<Vector3f>& normals = std::vector<Vector3f>(), const std::vector<Vector3f>& tangents = std::vector<Vector3f>()) :
			m_indices(indices),
			m_positions(positions),
			m_texCoords(texCoords),
			m_normals(normals),
			m_tangents(tangents) {}

	bool IsValid() const;
	void CalcNormals(bool flat=true);
	void CalcTangents();

	IndexedModel Finalize(bool normalsFlat=true);

	void AddVertex(const Vector3f& vert);
	inline void AddVertex(float x, float y, float z) { AddVertex(Vector3f(x, y, z)); }
	
	void AddTexCoord(const Vector2f& texCoord);
	inline void AddTexCoord(float x, float y) { AddTexCoord(Vector2f(x, y)); }
	
	void AddNormal(const Vector3f& normal);
	inline void AddNormal(float x, float y, float z) { AddNormal(Vector3f(x, y, z)); }
	
	void AddTangent(const Vector3f& tangent);
	inline void AddTangent(float x, float y, float z) { AddTangent(Vector3f(x, y, z)); }

//	inline void AddVertexSlot(const std::vector<unsigned int> slot) { m_vertslots.push_back(slot); }

	inline void AddVertices (const std::vector<Vector3f> verts) { for (auto x : verts) { AddVertex (x); } };
	inline void AddTexCoords (const std::vector<Vector2f> coords) { for (auto x : coords) { AddTexCoord (x); } };
	inline void AddNormals (const std::vector<Vector3f> normals) { for (auto x : normals) { AddNormal (x); } };
	inline void AddTangents (const std::vector<Vector3f> tangents) { for (auto x : tangents) { AddTangent (x); } };

	void AddFace(unsigned int vertIndex0, unsigned int vertIndex1, unsigned int vertIndex2);
	void CalcBBoxNCenter();

	inline const std::vector<unsigned int>& GetIndices() const { return m_indices; }
	//inline const std::vector<std::vector<unsigned int>>& GetVertexSlot() const { return m_vertslots; }
	inline const std::vector<Vector3f>& GetPositions()   const { return m_positions; }
	inline const std::vector<Vector2f>& GetTexCoords()   const { return m_texCoords; }
	inline const std::vector<Vector3f>& GetNormals()     const { return m_normals; }
	inline const std::vector<Vector3f>& GetTangents()    const { return m_tangents; }
	inline const Vector3f& GetBoundMin()    const { return m_boundMin; }
	inline const Vector3f& GetBoundMax()    const { return m_boundMax; }
	inline const float& GetBoundSize()    const { return m_boundSize; }
	inline const Vector3f& GetCenter()    const { return m_center; }

private:
	std::vector<unsigned int> m_indices;
	//std::vector<std::vector<unsigned int>> m_vertslots;
    std::vector<Vector3f> m_positions;
    std::vector<Vector2f> m_texCoords;
    std::vector<Vector3f> m_normals;
    std::vector<Vector3f> m_tangents;  
	Vector3f m_boundMin, m_boundMax;
	Vector3f m_center;
	float m_boundSize;
};

/**
 * Contains pointers for various vertex information inside renderer.
 */
class MeshData : public ReferenceCounter
{
public:
	MeshData(const IndexedModel& model, int materialIndex);
	virtual ~MeshData();
	
	void Draw() const;
    int GetMaterialIndex() { return m_materialIndex; }
	static MeshData* LoadCachedModel(const std::string& fileName, int index);
	void CacheModel(const std::string& fileName, int index);
	const IndexedModel& GetModel() { return m_model; }
protected:	
private:
	MeshData(MeshData& other) {}
	void operator=(MeshData& other) {}

	enum
	{
		POSITION_VB,
		TEXCOORD_VB,
		NORMAL_VB,
		TANGENT_VB,
		
		INDEX_VB,
		
		NUM_BUFFERS
	};
	
	GLuint m_vertexArrayObject;
	GLuint m_vertexArrayBuffers[NUM_BUFFERS];
	int m_drawCount;
    int m_materialIndex;
	const IndexedModel m_model;
};

class Mesh
{
public:
	Mesh(const std::string& fileName = "cube.obj");
    Mesh(const std::string& fileName, MeshData * data);
	Mesh(const std::string& meshName, const IndexedModel& model);
	Mesh(const Mesh& mesh);
    static std::vector<MeshData*> ImportMeshData(const std::string& fileName, int mode = -1);
	static btTriangleMesh*  ImportColData(const std::string& fileName);
    static std::vector<Material*> ImportMeshMaterial(const std::string& fileName);
    static std::vector<Mesh*> ImportMesh(const std::string& fileName);
	static btTriangleMesh* ImportCollision(const std::string& fileName);
	
	virtual ~Mesh();

	void Draw() const;
    MeshData* GetMeshData() const { return m_meshData; }
	void operator=(Mesh& mesh) {}
	static std::map<std::string, MeshData*> s_resourceMap;
	static std::map<std::string, btTriangleMesh*> s_resourceColMap;
protected:
private:
	

	std::string m_fileName;
	MeshData* m_meshData;
	
	
};

#endif
