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

#include "material.h"
#include <iostream>
#include <cassert>
#include "shader.h"
#include "../core/util.h"

std::map<std::string, Material*> Material::s_resourceMap;

Material::Material(const std::string& materialName) :
	m_materialName(materialName)
{
	if (materialName == "null")return;

	if (materialName.length() > 0)
	{
		std::map<std::string, Material*>::const_iterator it = s_resourceMap.find(materialName);
		if (it == s_resourceMap.end())
		{
			std::cerr << "Error: Material " << materialName << " has not been initialized!" << std::endl;
			assert(0 != 0);
		}

		m_materialData = it->second->GetData();
		m_materialData->AddReference();
	}

	m_shader = new Shader("forward-ambient");
}

Material::Material(const Material& other) :
	m_materialData(other.m_materialData),
	m_materialName(other.m_materialName),
	m_shader(other.m_shader)
{
	if (m_materialData)
		m_materialData->AddReference();
}

Material::~Material()
{
	if (m_materialName == "null")return;

	if (m_materialData && m_materialData->RemoveReference())
	{
		if (m_materialName.length() > 0)
		{
			s_resourceMap.erase(m_materialName);
		}

		delete m_materialData;
	}
}

Material::Material(const std::string& materialName, const Texture& diffuse, float specularIntensity, float specularPower,
                   const Texture& normalMap,
                   const Texture& dispMap, float dispMapScale, float dispMapOffset, Vector3f color) :
	m_materialName(materialName)
{
	m_materialData = new MaterialData();
	s_resourceMap[m_materialName] = this;

	m_materialData->SetTexture("diffuse", diffuse);
	m_materialData->SetFloat("specularIntensity", specularIntensity);
	m_materialData->SetFloat("specularPower", specularPower);
	m_materialData->SetTexture("normalMap", normalMap);
	m_materialData->SetTexture("dispMap", dispMap);
	m_materialData->SetVector3f("matColor", color);

	//HACK(zaklaus): Speed up for huge meshes
	m_materialData->Diffuse = diffuse;
	m_materialData->Normal = normalMap;
	m_materialData->Disp = dispMap;

	float baseBias = dispMapScale / 2.0f;
	m_materialData->SetFloat("dispMapScale", dispMapScale);
	m_materialData->SetFloat("dispMapBias", -baseBias + baseBias * dispMapOffset);

	m_shader = new Shader("forward-ambient");
}

Material* Material::LoadCachedMaterial(const std::string& fileName, int index)
{
	Material* m;
	std::string name = Util::split(fileName, '.')[0];

	std::string fname = Util::ResourcePath() + "models/" + name + "/" + name + "_cached_" + std::to_string(index) + ".zml";

	FILE* file = fopen(fname.c_str(), "rb");

	char magic[2] = {0};

	fread(magic, sizeof(char), 2, file);

	if (magic[0] != 'Z' || magic[1] != 'F')
	assert(!"Invalid cached model!");


	int matNameS = 0;
	fread(&matNameS, sizeof(int), 1, file);
	std::string matName = "";
	matName.reserve(matNameS);
	fread((char*)&matName[0], sizeof(char), matNameS + 1, file);

	// diffuse
	int diffS = 0;
	fread(&diffS, sizeof(int), 1, file);
	std::string diff = "";
	diff.reserve(diffS);
	fread((char*)&diff[0], sizeof(char), diffS + 1, file);

	// normal
	int nS = 0;
	fread(&nS, sizeof(int), 1, file);
	std::string n = "";
	n.reserve(nS);
	fread((char*)&n[0], sizeof(char), nS + 1, file);

	// displacement
	int dS = 0;
	fread(&dS, sizeof(int), 1, file);
	std::string d = "";
	d.reserve(dS);
	fread((char*)&d[0], sizeof(char), dS + 1, file);

	Vector3f matColor;
	fread(&matColor, sizeof(Vector3f), 1, file);


	fclose(file);
	int z = strlen("./data/textures/");
	m = new Material(matName.c_str(), std::string(diff.c_str() + z), 0.4f, 0.8f, std::string(n.c_str() + z), std::string(d.c_str() + z), 0, 0, matColor);

	return m;
}

void Material::CacheMaterial(const std::string& fileName, int index)
{
	static char magic[] = "ZF";
	std::string index_ = std::to_string(index);
	auto path = Util::split(fileName, '.');
	std::string newFileName = path[0] + "_cached_" + index_ + ".zml";
	auto name = (Util::ResourcePath() + "models/" + path[0] + "/" + newFileName);

	FILE* file = fopen(name.c_str(), "wb");

	if (!file)
	assert(!"Can't open handle!");

	char null = 0;

	fwrite(magic, sizeof(char), 2, file);

	int ms = m_materialName.size();
	fwrite(&ms, sizeof(int), 1, file);
	fwrite(&m_materialName[0], sizeof(char), ms, file);
	fwrite(&null, sizeof(char), 1, file);

	int dds = m_materialData->Diffuse.GetName().size();
	auto dd = m_materialData->Diffuse.GetName();
	fwrite(&dds, sizeof(int), 1, file);
	fwrite(&dd[0], sizeof(char), dds, file);
	fwrite(&null, sizeof(char), 1, file);

	int ns = m_materialData->Normal.GetName().size();
	auto n = m_materialData->Normal.GetName();
	fwrite(&ns, sizeof(int), 1, file);
	fwrite(&n[0], sizeof(char), ns, file);
	fwrite(&null, sizeof(char), 1, file);

	int ds = m_materialData->Disp.GetName().size();
	auto d = m_materialData->Disp.GetName();
	fwrite(&ds, sizeof(int), 1, file);
	fwrite(&d[0], sizeof(char), ds, file);
	fwrite(&null, sizeof(char), 1, file);

	Vector3f matColor = GetVector3f("matColor");
	fwrite(&matColor, sizeof(matColor), 1, file);

	FILE* num = fopen((Util::ResourcePath() + "models/" + path[0] + "/numsM").c_str(), "wb");
	int id = 1 + index;
	fwrite(&id, sizeof(int), 1, num);

	fclose(num);

	fclose(file);
}

void Material::DrawDebugUI()
{
	static char diffuse[256] = {0};
	static char normal[256] = {0};
	static char disp[256] = {0};
	static float dispScale, dispOffset = 0.0f;
	static float specularIntensity, specularPower = 0.0f;

	ImGui::Text("Texture Maps");
	ImGui::InputText("Diffuse", diffuse, 256);
	ImGui::InputText("Normal", normal, 256);
	ImGui::InputText("Displacement", disp, 256);

	ImGui::Text("Material Settings");
	ImGui::DragFloat("Specular Intensity", &specularIntensity, 0.1f);
	ImGui::DragFloat("Specular Power", &specularPower, 0.1f);
	ImGui::DragFloat("Displacement Scale", &dispScale, 0.1f);
	ImGui::DragFloat("Displacement Offset", &dispOffset, 0.1f);

	if (ImGui::Button("Update Material"))
	{
		float baseBias = dispScale / 2.0f;
		m_materialData->SetFloat("dispMapScale", dispScale);
		m_materialData->SetFloat("dispMapBias", -baseBias + baseBias * dispOffset);

		Texture dif, nrm, dis;

		dif = Texture(diffuse);
		nrm = Texture(normal);
		dis = Texture(disp);

		m_materialData->Diffuse = dif;
		m_materialData->Normal = nrm;
		m_materialData->Disp = dis;

		m_materialData->SetTexture("diffuse", dif);
		m_materialData->SetFloat("specularIntensity", specularIntensity);
		m_materialData->SetFloat("specularPower", specularPower);
		m_materialData->SetTexture("normalMap", nrm);
		m_materialData->SetTexture("dispMap", dis);
	}
}
