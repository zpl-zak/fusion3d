/*
 * Copyright (C) 2015 Dominik Madarasz
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

#ifndef MESHRENDERER_H_INCLUDED
#define MESHRENDERER_H_INCLUDED

#include "../core/entityComponent.h"
#include "../core/timing.h"
#include "../rendering/mesh.h"

class MeshRenderer : public EntityComponent
{
public:
	MeshRenderer(){}
    MeshRenderer(std::vector<Mesh*> meshes, std::vector<Material*> materials)
    {
		LoadData(meshes, materials);
    }

    MeshRenderer(const std::string& fileName) :
        MeshRenderer(Mesh::ImportMesh(fileName), Mesh::ImportMeshMaterial(fileName))
    {}

    MeshRenderer(Mesh mesh, const Material& material)
    {
        m_mesh.push_back(mesh);
        m_material.push_back(material);
    }

	virtual void Init()
	{
		m_visible = true;
		m_shadows = true;
	}

	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
	{
		if (shader.GetName() == "shadowMapGenerator" && !m_shadows) return;
		if (!m_visible) return;
        for (size_t i = 0; i < m_mesh.size(); i++)
        {
            shader.Bind();
            shader.UpdateUniforms(GetTransform(), m_material.at(m_mesh.at(i).GetMeshData()->GetMaterialIndex()), renderingEngine, camera);
            m_mesh.at(i).Draw();
        }
	}

	virtual void DataDeploy(tinyxml2::XMLElement* data)
	{
		std::vector<Mesh*> meshes = Mesh::ImportMesh(data->GetText());
		std::vector<Material*> materials = Mesh::ImportMeshMaterial(data->GetText());
		LoadData(meshes, materials);
	}

	EntityComponent* SetVisible(bool visible)
	{
		m_visible = visible;

		return this;
	}

	bool IsVisible()
	{
		return m_visible;
	}

	void LoadData(std::vector<Mesh*> meshes, std::vector<Material*> materials)
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			m_mesh.push_back(*(meshes.at(i)));
		}

		for (size_t i = 0; i < materials.size(); i++)
		{
			m_material.push_back(*(materials.at(i)));
		}
	}

	EntityComponent* SetShadows(bool shadows)
	{
		m_shadows = shadows;

		return this;
	}

	bool HasShadows()
	{
		return m_shadows;
	}

protected:
	std::vector<Mesh> m_mesh;
	std::vector<Material> m_material;
	bool			m_visible;
	bool			m_shadows;
private:
};

class MorphMeshRenderer : public MeshRenderer
{
public:
	MorphMeshRenderer()
	: m_step(-1)
	{}

	MorphMeshRenderer(const std::string& fileName, int stepSize)
		:
		m_step(-1)
	{
		LoadData(fileName, stepSize);
	}

	virtual void Update(float delta) 
	{
		if (m_timer.HasElapsed())
		{
			
		}
		m_step++;

		if (m_step >= m_meshes.size())
		{
			m_step = 0;
		}
	}

	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
	{
		if (shader.GetName() == "shadowMapGenerator" && !m_shadows) return;
		if (!m_visible) return;

		shader.Bind();
		for (size_t j = 0; j < m_meshes.at(m_step).size(); j++)
		{
			shader.UpdateUniforms(GetTransform(), m_materials.at(m_meshes.at(m_step).at(j).GetMeshData()->GetMaterialIndex()), renderingEngine, camera);
			m_meshes.at(m_step).at(j).Draw();
		}
	}

	virtual void DataDeploy(tinyxml2::XMLElement* data)
	{
		int stepSize = 1;
		if (data->Attribute("step"))
		{
			stepSize = atoi(data->Attribute("step"));
		}
		LoadData(data->GetText(), stepSize);
	}

	EntityComponent* SetVisible(bool visible)
	{
		m_visible = visible;

		return this;
	}

	bool IsVisible()
	{
		return m_visible;
	}


	void LoadData(std::string filename, int stepSize)
	{
		m_step = -1;
		
		std::string filelist = Util::ExecuteTask("listdir", (Util::ResourcePath() + "models/" + filename + " .obj"));
		std::vector<std::string> files = Util::Split(filelist, '@');
		m_timer.SetDelay(1);
		std::vector<std::vector<Mesh*>> meshes_old;
		std::vector<Material*> materials;

		for (size_t i = 0; i < files.size() - 1; i++)
		{
			meshes_old.push_back(Mesh::ImportMesh(filename + "/" + files[i]));
		}
		materials = Mesh::ImportMeshMaterial(filename + "/" + files[0]);

		std::vector<std::vector<Mesh>> meshg;
		for (size_t i = 0; i < meshes_old.size(); i++)
		{
			if (i + 1 == meshes_old.size())break;
			
			for (size_t l = 0; l < stepSize; l++)
			{
				std::vector<Mesh> mesh;
				for (size_t j = 0; j < meshes_old.at(i).size(); j++)
				{
				
						std::vector<Vector3f> pos;
						std::vector<Vector3f> nor;
						std::vector<Vector3f> tan;

						for (size_t k = 0; k < meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetPositions().size(); k++)
						{
							pos.push_back((Vector3f)(meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetPositions().at(k)).Lerp(meshes_old.at(i + 1).at(j)->GetMeshData()->GetModel().GetPositions().at(k), (float)l / (float)stepSize));
							nor.push_back((Vector3f)(meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetNormals().at(k)).Lerp(meshes_old.at(i + 1).at(j)->GetMeshData()->GetModel().GetNormals().at(k), (float)l / (float)stepSize));
							tan.push_back((Vector3f)(meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetTangents().at(k)).Lerp(meshes_old.at(i + 1).at(j)->GetMeshData()->GetModel().GetTangents().at(k), (float)l / (float)stepSize));
						}


						mesh.push_back(Mesh(filename + (char)(j + 40), new MeshData(IndexedModel(
							meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetIndices(),
							pos,
							meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetTexCoords(),
							nor,
							tan
							), meshes_old.at(i).at(j)->GetMeshData()->GetMaterialIndex())));
					}
					meshg.push_back(mesh);
			}
			
		}
		m_meshes = meshg;

		for (auto mat : materials)
		{
			m_materials.push_back(*mat);
		}

		for (auto mesh : meshes_old)
		{
			for (auto x : mesh)
			{
				delete x;
			}
		}

		for (auto mat : materials)
		{
			delete mat;
		}
	}


	EntityComponent* SetShadows(bool shadows)
	{
		m_shadows = shadows;

		return this;

	}

	bool HasShadows()
	{
		return m_shadows;
	}

protected:
private:
	std::vector<std::vector<Mesh>> m_meshes;
	std::vector<Material> m_materials;
	size_t m_step;
	size_t m_skip;
	DelayTimer m_timer;
};
#endif // MESHRENDERER_H_INCLUDED
