/*
 * Copyright (C) 2015-2016 Dominik "ZaKlaus" Madarasz
 * Copyright (C) 2014 Benny Bobaganoosh
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

#include "mesh.h"

#include "../core/profiling.h"
#include "../core/util.h"

#include <GL/glew.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::map<std::string, MeshData*> Mesh::s_resourceMap;
std::map<std::string, btTriangleMesh*> Mesh::s_resourceColMap;

bool IndexedModel::IsValid() const
{
	return m_positions.size() == m_texCoords.size()
		&& m_texCoords.size() == m_normals.size()
		&& m_normals.size() == m_tangents.size();
}

void IndexedModel::AddVertex(const Vector3f& vert)
{
	m_positions.push_back(vert);
}

void IndexedModel::AddTexCoord(const Vector2f& texCoord)
{
	m_texCoords.push_back(texCoord);
}

void IndexedModel::AddNormal(const Vector3f& normal)
{
	m_normals.push_back(normal);
}
	
void IndexedModel::AddTangent(const Vector3f& tangent)
{
	m_tangents.push_back(tangent);
}

void IndexedModel::CalcBBoxNCenter()
{
	Vector3f BMin, BMax, Center; // ja som K0K0T!!!!!!!!!!
	for (size_t i = 0; i < m_positions.size(); i++)
	{
		if (BMin.GetX() < m_positions[i].GetX())BMin.SetX(m_positions[i].GetX());
		if (BMin.GetY() < m_positions[i].GetY())BMin.SetY(m_positions[i].GetY());
		if (BMin.GetZ() < m_positions[i].GetZ())BMin.SetZ(m_positions[i].GetZ());

		if (BMax.GetX() >= m_positions[i].GetX())BMax.SetX(m_positions[i].GetX());
		if (BMax.GetY() >= m_positions[i].GetY())BMax.SetY(m_positions[i].GetY());
		if (BMax.GetZ() >= m_positions[i].GetZ())BMax.SetZ(m_positions[i].GetZ());


	}

	Center.SetX(0.5f * (BMax.GetX() + BMin.GetX()));
	Center.SetY(0.5f * (BMax.GetY() + BMin.GetY()));
	Center.SetZ(0.5f * (BMax.GetZ() + BMin.GetZ()));

	//(t.GetBoundMin().Abs() + t.GetBoundMax().Abs()

	m_boundMax = BMax; m_boundMin = BMin; m_center = Center;
	m_boundSize = (BMin.Abs() + BMax.Abs()).Length();
	m_boundSize = fabsf(m_boundSize);
}

IndexedModel IndexedModel::Finalize(bool normalsFlat)
{
	CalcBBoxNCenter();
	
	if(IsValid())
	{
		return *this;
	}
	
	if(m_texCoords.size() == 0)
	{
		for(unsigned int i = m_texCoords.size(); i < m_positions.size(); i++)
		{
			m_texCoords.push_back(Vector2f(0.0f, 0.0f));
		}
	}
	
	if(m_normals.size() == 0)
	{
		CalcNormals(normalsFlat);
	}
	
	if(m_tangents.size() == 0)
	{
		CalcTangents();
	}


	
	return *this;
}

void IndexedModel::AddFace(unsigned int vertIndex0, unsigned int vertIndex1, unsigned int vertIndex2)
{
	m_indices.push_back(vertIndex0);
	m_indices.push_back(vertIndex1);
	m_indices.push_back(vertIndex2);
}

int getSimilarIndex (const Vector3f& vertex, const std::vector<Vector3f>& out_verts, int& found)
{
	auto i = std::find (out_verts.begin (), out_verts.end (), vertex);

	if (i != out_verts.end ())
	{
		found = i - out_verts.begin ();
		return 1;
	}
	return 0;
}

void IndexedModel::CalcNormals(bool flat) // toto musim prerobit na priemer normalov,
{
	m_normals.clear();
	m_normals.reserve(m_positions.size());
	
	for(unsigned int i = 0; i < m_positions.size(); i++)
		m_normals.push_back(Vector3f(0,0,0));

	
	for (unsigned int i = 0; i < m_indices.size (); i += 3)
	{
		int i0 = m_indices[i];
		int i1 = m_indices[i + 1];
		int i2 = m_indices[i + 2];

		Vector3f v1 = m_positions[i1] - m_positions[i0];
		Vector3f v2 = m_positions[i2] - m_positions[i0];

		Vector3f normal = v1.Cross (v2).Normalized ();

		m_normals[i0] = m_normals[i0] + normal;
		m_normals[i1] = m_normals[i1] + normal;
		m_normals[i2] = m_normals[i2] + normal;
	}

	if (!flat)
	{
		std::vector<Vector3f> normals;
		Vector3f nNormal;
		int shared = 0;

		for (size_t i = 0; i < m_positions.size (); i++)
		{
			/*for (int j = 0; j < m_vertslots[i].size(); j ++)
			{
				nNormal += m_normals[m_indices[m_vertslots[i][j]]];
				shared++;
			}*/

			nNormal /= (float)shared;
			nNormal = nNormal.Normalized ();
			normals.push_back (nNormal);

			nNormal = Vector3f ();
			shared = 0;
		}
		m_normals.clear ();
		m_normals = normals;
	}
}

void IndexedModel::CalcTangents()
{
	m_tangents.clear();
	m_tangents.reserve(m_positions.size());
	
	for(unsigned int i = 0; i < m_positions.size(); i++)
		m_tangents.push_back(Vector3f(0,0,0));
		
	for(unsigned int i = 0; i < m_indices.size(); i += 3)
    {
		int i0 = m_indices[i];
		int i1 = m_indices[i + 1];
		int i2 = m_indices[i + 2];
    
        Vector3f edge1 = m_positions[i1] - m_positions[i0];
        Vector3f edge2 = m_positions[i2] - m_positions[i0];
        
        float deltaU1 = m_texCoords[i1].GetX() - m_texCoords[i0].GetX();
        float deltaU2 = m_texCoords[i2].GetX() - m_texCoords[i0].GetX();
        float deltaV1 = m_texCoords[i1].GetY() - m_texCoords[i0].GetY();
        float deltaV2 = m_texCoords[i2].GetY() - m_texCoords[i0].GetY();
        
        float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
        float f = dividend == 0.0f ? 0.0f : 1.0f/dividend;
        
        Vector3f tangent = Vector3f(0,0,0);
        
        tangent.SetX(f * (deltaV2 * edge1.GetX() - deltaV1 * edge2.GetX()));
        tangent.SetY(f * (deltaV2 * edge1.GetY() - deltaV1 * edge2.GetY()));
        tangent.SetZ(f * (deltaV2 * edge1.GetZ() - deltaV1 * edge2.GetZ()));

//Bitangent example, in Java
//		Vector3f bitangent = new Vector3f(0,0,0);
//		
//		bitangent.setX(f * (-deltaU2 * edge1.getX() - deltaU1 * edge2.getX()));
//		bitangent.setX(f * (-deltaU2 * edge1.getY() - deltaU1 * edge2.getY()));
//		bitangent.setX(f * (-deltaU2 * edge1.getZ() - deltaU1 * edge2.getZ()));

		m_tangents[i0] += tangent;
		m_tangents[i1] += tangent;
		m_tangents[i2] += tangent;	
    }

    for(unsigned int i = 0; i < m_tangents.size(); i++)
		m_tangents[i] = m_tangents[i].Normalized();
}


MeshData::MeshData(const IndexedModel& model, int materialIndex) :
    ReferenceCounter(),
    m_drawCount(model.GetIndices().size()),
    m_materialIndex(materialIndex),
	m_model(model)
{
	if(!model.IsValid())
	{
		std::cout << "Error: Invalid mesh! Must have same number of positions, texCoords, normals, and tangents! "
			<< "(Maybe you forgot to Finalize() your IndexedModel?)" << std::endl;
		assert(0 != 0);
	}

	m_materialArray.push_back(m_materialIndex);

	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.GetPositions().size() * sizeof(model.GetPositions()[0]), &model.GetPositions()[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.GetTexCoords().size() * sizeof(model.GetTexCoords()[0]), &model.GetTexCoords()[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.GetNormals().size() * sizeof(model.GetNormals()[0]), &model.GetNormals()[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TANGENT_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.GetTangents().size() * sizeof(model.GetTangents()[0]), &model.GetTangents()[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.GetIndices().size() * sizeof(model.GetIndices()[0]), &model.GetIndices()[0], GL_STATIC_DRAW);
}

MeshData::~MeshData() 
{	
	glDeleteBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void MeshData::Draw() const
{
	glBindVertexArray(m_vertexArrayObject);
	
	#if PROFILING_DISABLE_MESH_DRAWING == 0
		glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	#endif
}


Mesh::Mesh(const std::string& meshName, const IndexedModel& model) :
	m_fileName(meshName)
{
	std::map<std::string, MeshData*>::const_iterator it = s_resourceMap.find(meshName);
	if(it != s_resourceMap.end())
	{
		std::cout << "Error adding mesh " << meshName << ": Mesh already exists by the same name!" << std::endl;
		assert(0 != 0);
	}
	else
	{
		m_meshData = new MeshData(model, 0);
		s_resourceMap.insert(std::pair<std::string, MeshData*>(meshName, m_meshData));
	}
}

Mesh::Mesh(const std::string& fileName) :
	m_fileName(fileName),
	m_meshData(0)
{
	std::map<std::string, MeshData*>::const_iterator it = s_resourceMap.find(fileName);
	if(it != s_resourceMap.end())
	{
		m_meshData = it->second;
		m_meshData->AddReference();
	}
	else
	{
		auto name = Util::split(fileName, '.')[0];
		Util::CreateDir((Util::ResourcePath() + "models/" + name), NULL);
		m_meshData = Mesh::ImportMeshData(fileName, 0).at(0);
	}
}

Mesh::Mesh(const std::string& fileName, MeshData * data) :
    m_fileName(fileName),
    m_meshData(data)
{
    m_meshData->AddReference();
}

Mesh::Mesh(const Mesh& mesh) :
	m_fileName(mesh.m_fileName),
	m_meshData(mesh.m_meshData)
{
	m_meshData->AddReference();
}

std::vector<MeshData*> Mesh::ImportMeshData(const std::string & fileName, int mode)
{
	std::vector<MeshData*> outData;
#if 1
	auto fname = Util::split(fileName, '.');
	auto cachedName = Util::ResourcePath() + "models/" + fname[0] + "/" + fname[0] + "_cached_0.zdl";

	auto name = Util::split(fileName, '.')[0];
	Util::CreateDir((Util::ResourcePath() + "models/" + name), NULL);

	if (FILE *file = fopen(cachedName.c_str(), "rb")) {
		fclose(file);

		int nums = 1;
		FILE * num = fopen((Util::ResourcePath() + "models/" + fname[0] + "/nums").c_str(), "rb");
		fread(&nums, sizeof(int), 1, num);
		
		for (int i = 0; i < nums; i++)
		{
			auto x = MeshData::LoadCachedModel(fileName, i);
			outData.push_back(x);
			s_resourceMap.insert(std::pair<std::string, MeshData*>(fileName + "_" + Util::to_string(i), outData.at(outData.size() - 1)));
		}
	}
	else 
#endif
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile((Util::ResourcePath() + "models/" + fileName).c_str(),
			(fileName == "plane.obj") ? (aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				// aiProcess_OptimizeGraph |
				//aiProcess_PreTransformVertices |
				//aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_FixInfacingNormals |
				aiProcess_FindInvalidData |
				//aiProcess_ValidateDataStructure |
				aiProcess_CalcTangentSpace) : (
					aiProcess_CalcTangentSpace |
					aiProcess_Triangulate |
					//aiProcess_RemoveComponent |
					//aiProcess_OptimizeGraph |
					aiProcess_PreTransformVertices |
					//aiProcess_OptimizeMeshes |
					aiProcess_SplitLargeMeshes |
					aiProcess_JoinIdenticalVertices |
					aiProcess_ImproveCacheLocality));
		if (!scene)
		{
			std::cout << "Mesh load failed!: " << fileName << "|" << std::endl;
			scene = importer.ReadFile(Util::ResourcePath() + "models/error.obj",
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				// aiProcess_OptimizeGraph |
				//aiProcess_PreTransformVertices |
				//aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_FixInfacingNormals |
				aiProcess_FindInvalidData |
				//aiProcess_ValidateDataStructure |
				aiProcess_CalcTangentSpace);
		}
		unsigned int start = mode;
		unsigned int end = start + 1;

		if (mode == -1)
		{
			end = scene->mNumMeshes;
			start = 0;
		}
		int lastIndexOffset = 0;
		for (unsigned int i = start; i < end; i++)
		{
			std::vector<Vector3f> positions;
			std::vector<Vector2f> texCoords;
			std::vector<Vector3f> normals;
			std::vector<Vector3f> tangents;
			std::vector<unsigned int> indices;

			const aiMesh* model = scene->mMeshes[i];
			
			const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
			for (unsigned int j = 0; j < model->mNumVertices; j++)
			{
				aiVector3D pos = model->HasPositions() ? model->mVertices[j] : aiZeroVector;
				aiVector3D normal = model->mNormals[j];
				aiVector3D texCoord = model->HasTextureCoords(0) ? model->mTextureCoords[0][j] : aiZeroVector;
				aiVector3D tangent = model->mTangents[j];

				positions.push_back(Vector3f(pos.x, pos.y, pos.z));
				texCoords.push_back(Vector2f(texCoord.x, texCoord.y));
				normals.push_back(Vector3f(normal.x, normal.y, normal.z));
				tangents.push_back(Vector3f(tangent.x, tangent.y, tangent.z));
			}

			for (unsigned int j = 0; j < model->mNumFaces; j++)
			{
				const aiFace& face = model->mFaces[j];
				//assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0] - lastIndexOffset);
				indices.push_back(face.mIndices[1] - lastIndexOffset);
				indices.push_back(face.mIndices[2] - lastIndexOffset);
			}
			auto mdata = new MeshData(IndexedModel(indices, positions, texCoords, normals, tangents).Finalize(), model->mMaterialIndex);

			mdata->CacheModel(fileName, i);
			
			outData.push_back(mdata);
			s_resourceMap.insert(std::pair<std::string, MeshData*>(fileName + "_" + Util::to_string(i), outData.at(outData.size() - 1)));
		}
	}

    return outData;
}

btTriangleMesh* Mesh::ImportColData(const std::string & fileName)
{
	auto name = Util::split(fileName, '.')[0];
	Util::CreateDir((Util::ResourcePath() + "models/" + name), NULL);

	auto data = Mesh::ImportMeshData(fileName);

	btTriangleMesh* mesh = new btTriangleMesh();

	for (unsigned int i = 0; i < 1; i++)
	{
		const IndexedModel& model = data.at(i)->GetModel();

		for (unsigned int j = 0; j < model.GetPositions().size(); j++)
		{

			mesh->findOrAddVertex(model.GetPositions()[j].GetBT(), false);
		}

		for (unsigned int j = 0; j < model.GetIndices().size(); j+=3)
		{
			mesh->addTriangleIndices(model.GetIndices()[j + 0], model.GetIndices()[j + 1], model.GetIndices()[j+2]);
		}

	}

	return mesh;
}

std::vector<Material*> Mesh::ImportMeshMaterial(const std::string & fileName)
{
	std::vector<Material*> outData;

#if 1
	auto fname = Util::split(fileName, '.');
	auto cachedName = Util::ResourcePath() + "models/" + fname[0] + "/" + fname[0] + "_cached_0.zml";

	auto name = Util::split(fileName, '.')[0];
	Util::CreateDir((Util::ResourcePath() + "models/" + name), NULL);

	if (FILE *file = fopen(cachedName.c_str(), "rb")) {
		fclose(file);

		int nums = 1;
		FILE * num = fopen((Util::ResourcePath() + "models/" + fname[0] + "/numsM").c_str(), "rb");
		fread(&nums, sizeof(int), 1, num);

		for (int i = 0; i < nums; i++)
		{
			auto x = Material::LoadCachedMaterial(fileName, i);
			outData.push_back(x);
			//s_resourceMap.insert(std::pair<std::string, MeshData*>(fileName + "_" + Util::to_string(i), outData.at(outData.size() - 1)));
			//Material::s_resourceMap.insert(std::make_pair(fileName + "_" + Util::to_string(i), outData.at(outData.size() - 1)));
		}
	}
	else 
#endif
	{

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile((Util::ResourcePath() + "models/" + fileName).c_str(),
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			// aiProcess_OptimizeGraph |
			//aiProcess_PreTransformVertices |
			//aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_FixInfacingNormals |
			aiProcess_FindInvalidData |
			//aiProcess_ValidateDataStructure |
			aiProcess_CalcTangentSpace);

		if (!scene)
		{
			std::cout << "Mesh load failed!: " << fileName << std::endl;
			scene = importer.ReadFile(Util::ResourcePath() + "models/error.obj",
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				// aiProcess_OptimizeGraph |
				//aiProcess_PreTransformVertices |
				//aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				aiProcess_FixInfacingNormals |
				aiProcess_FindInvalidData |
				//aiProcess_ValidateDataStructure |
				aiProcess_CalcTangentSpace);
		}

		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			aiString matName;
			aiString texName, normName, dispName;
			float shine = 0.0;
			aiColor3D matColorAi;

			scene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
			scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &texName);
			scene->mMaterials[i]->GetTexture(aiTextureType_HEIGHT, 0, &normName);
			scene->mMaterials[i]->GetTexture(aiTextureType_DISPLACEMENT, 0, &dispName);
			scene->mMaterials[i]->Get(AI_MATKEY_SHININESS, shine);
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, matColorAi);

			Vector3f color = Vector3f(matColorAi.r, matColorAi.g, matColorAi.b);

			//std::cout << matName.C_Str() << ":::" << texName.C_Str();

			std::string sTexName = "DefaultTexture.png";
			if (texName.length > 0)
			{
				sTexName = texName.C_Str();
			}

			std::string sNormName = "default_normal.jpg";
			if (normName.length > 0)
			{
				sNormName = normName.C_Str();
			}

			std::string sDispName = "default_disp.png";
			if (dispName.length > 0)
			{
				sDispName = dispName.C_Str();
			}

			Material * mat = new Material(matName.C_Str(), sTexName, 0.4f, 0.8f, sNormName, sDispName, 0, 0, color);
			mat->CacheMaterial(fileName, i);
			outData.push_back(mat);
		}
	}
    
    
    return outData;
}

std::vector<Mesh*> Mesh::ImportMesh(const std::string & fileName)
{
    std::vector<MeshData*> data = Mesh::ImportMeshData(fileName);
    std::vector<Mesh*> meshes;

	auto prepath = Util::ResourcePath() + "models/" + Util::split(fileName, '.')[0];
	prepath = prepath.substr(2);
	

	Util::CreateDir(prepath, NULL);

    for (size_t i = 0; i < data.size(); i++)
    {
		auto x = new Mesh(fileName + "_" + Util::to_string(i), data.at(i));
        meshes.push_back(x);

		//x->CacheModel(i);
    }
    return meshes;
}

btTriangleMesh* Mesh::ImportCollision(const std::string & fileName)
{
	return Mesh::ImportColData(fileName);
	std::map<std::string, btTriangleMesh*>::const_iterator it = s_resourceColMap.find(fileName+"_col");

	auto prepath = Util::ResourcePath() + "models/" + Util::split(fileName, '.')[0];
	prepath = prepath.substr(2);

	Util::CreateDir(prepath, NULL);

	btTriangleMesh* mesh;
	if (it != s_resourceColMap.end())
	{
		mesh = it->second;
	}
	else
	{
		mesh = Mesh::ImportColData(Util::split(fileName, '.')[0]);
	}
	return mesh;
}

MeshData * MeshData::LoadCachedModel(const std::string & fileName, int index)
{
	MeshData* m;
	std::string name = Util::split(fileName, '.')[0];

	std::string fname = Util::ResourcePath() + "models/" + name + "/" + name + "_cached_" + std::to_string(index) + ".zdl";

	FILE * file = fopen(fname.c_str(), "rb");


	char magic[2] = { 0 };

	fread(magic, sizeof(char), 2, file);

	if (magic[0] != 'Z' || magic[1] != 'F')
		assert(!"Invalid cached model!");

	int drawCount = 0;
	fread(&drawCount, sizeof(int), 1, file);

	int matID = 0;
	fread(&matID, sizeof(int), 1, file);

	int indSize = 0;
	fread(&indSize, sizeof(int), 1, file);
	std::vector<unsigned int> indices;
	
	indices.resize(indSize);
	fread((char*)&indices[0], sizeof(indices[0]), indSize, file);

	int posSize = 0;
	fread(&posSize, sizeof(int), 1, file);
	std::vector<Vector3f> positions;
	positions.resize(posSize);
	fread((char*)&positions[0], sizeof(positions[0]), posSize, file);

	int norSize = 0;
	fread(&norSize, sizeof(int), 1, file);
	std::vector<Vector3f> normals;
	normals.resize(norSize);
	fread((char*)&normals[0], sizeof(normals[0]), norSize, file);

	int uvSize = 0;
	fread(&uvSize, sizeof(int), 1, file);
	std::vector<Vector2f> uvs;
	uvs.resize(uvSize);
	fread((char*)&uvs[0], sizeof(uvs[0]), uvSize, file);


	fclose(file);

	m = new MeshData(IndexedModel(indices, positions, uvs, normals).Finalize(), matID);
	//s_resourceMap.insert(std::make_pair(fileName + "_" + Util::to_string(index), m));

	return m;
}

void MeshData::CacheModel(const std::string& fileName, int index)
{
	static char magic[] = "ZF";
	std::string index_ = std::to_string(index);
	auto path = Util::split(fileName, '.');
	std::string newFileName = path[0] + "_cached_"+ index_ +".zdl";
	auto name = (Util::ResourcePath() + "models/" + path[0] + "/" + newFileName);

	FILE * file = fopen(name.c_str(), "wb");

	if (!file)
		assert(!"Can't open handle!");

	fwrite(magic, sizeof(char), 2, file);

	size_t size = GetModel().GetIndices().size();
	int matID = GetMaterialIndex();

	fwrite(&size, sizeof(size_t), 1, file);
	fwrite(&matID, sizeof(int), 1, file);

	auto mdl = GetModel();

	fwrite(&size, sizeof(size_t), 1, file);

	for (auto x : mdl.GetIndices())
	{
		fwrite(&x, sizeof(size_t), 1, file);
	}

	auto posSize = mdl.GetPositions().size();
	fwrite(&posSize, sizeof(size_t), 1, file);
	for (auto x : mdl.GetPositions())
	{
		fwrite(&x, sizeof(Vector3f), 1, file);
	}

	auto norSize = mdl.GetNormals().size();
	fwrite(&norSize, sizeof(size_t), 1, file);
	for (auto x : mdl.GetNormals())
	{
		fwrite(&x, sizeof(Vector3f), 1, file);
	}

	auto uvSize = mdl.GetTexCoords().size();
	fwrite(&uvSize, sizeof(size_t), 1, file);
	for (auto x : mdl.GetTexCoords())
	{
		fwrite(&x, sizeof(Vector2f), 1, file);
	}

	FILE * num = fopen((Util::ResourcePath() + "models/" + path[0] + "/nums").c_str(), "wb");
	int id = 1 + index;
	fwrite(&id, sizeof(int), 1, num);

	fclose(num);

	fclose(file);
}

Mesh::~Mesh()
{
	if(m_meshData && m_meshData->RemoveReference())
	{
		if(m_fileName.length() > 0)
		{
			s_resourceMap.erase(m_fileName);
		}
			
		delete m_meshData;
	}
}

void Mesh::Draw() const
{
	m_meshData->Draw();
}
