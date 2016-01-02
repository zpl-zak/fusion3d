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
			auto material = m_material.at (m_mesh.at (i).GetMeshData ()->GetMaterialIndex ());
			auto _shader = (shader.GetName() == "forward-ambient") ? material.GetShader () : &shader;
            _shader->Bind();
            _shader->UpdateUniforms(GetTransform(), material, renderingEngine, camera);
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

class MeshSequence : public MeshRenderer
{
public:
	MeshSequence()
	: m_step(-1),
	m_play(true)
	{}

	MeshSequence(const std::string& fileName, int stepSize)
		:
		m_step(-1),
		m_play(true)
	{
		LoadData(fileName, stepSize);
	}

	void Update(float delta) 
	{
		if (!m_play) return;

		m_step++;

		if (m_step >= m_meshes.size())
		{
			m_step = 0;
		}
	}

	void Render(const Transform& transform, const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
	{
		if (shader.GetName() == "shadowMapGenerator" && !m_shadows) return;
		if (!m_visible) return;
		int step = (m_step < 0) ? 0 : m_step;

		shader.Bind();
		for (size_t j = 0; j < m_meshes.at(step).size(); j++)
		{
			shader.UpdateUniforms(transform, m_materials.at(m_meshes.at(step).at(j).GetMeshData()->GetMaterialIndex()), renderingEngine, camera);
			m_meshes.at(step).at(j).Draw();
		}
	}

	void DataDeploy(tinyxml2::XMLElement* data)
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
		std::vector<std::vector<Mesh*>> meshes_old;
		std::vector<Material*> materials;

		for (size_t i = 0; i < files.size(); i++)
		{
			meshes_old.push_back(Mesh::ImportMesh(filename + "/" + files[i]));
		}
		materials = Mesh::ImportMeshMaterial(filename + "/" + files[0]);

		for (auto x : materials)
		{
			m_materials.push_back(*x);
		}
		
		
		std::vector<std::vector<Mesh>> meshg;
		for (size_t i = 0; i < meshes_old.size(); i++)
		{
			if (i + 1 == meshes_old.size())break;
			
			for (size_t l = 0; l < stepSize; l++)
			{
				std::vector<Mesh> mesh;
				for (size_t j = 0; j < meshes_old.at(i).size(); j++)
				{
					MeshData* meshData;
					std::map<std::string, MeshData*>::const_iterator it = Mesh::s_resourceMap.find(filename + (char)(j + i + l + 40));
					if (it != Mesh::s_resourceMap.end())
					{
						meshData = it->second;
						meshData->AddReference();
					}
					else
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

						meshData = new MeshData(IndexedModel(
							meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetIndices(),
							pos,
							meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetTexCoords(),
							nor,
							tan
							), meshes_old.at(i).at(j)->GetMeshData()->GetMaterialIndex());
						Mesh::s_resourceMap.insert(std::pair<std::string, MeshData*>(filename + (char)(j + i + l + 40), meshData));
					}
						mesh.push_back(Mesh(filename + (char)(j + i + l + 40), meshData));
					}
					meshg.push_back(mesh);
			}
			
		}
		m_meshes = meshg;

		for (auto mesh : meshes_old)
		{
			for (auto x : mesh)
			{
				delete x;
			}
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

	std::string GetName() 
	{
		return m_name;
	}

	void SetStep(size_t step)
	{
		m_step = step;
	}

	void SetPlay(bool play)
	{
		m_play = play;
	}

protected:
private:
	std::vector<std::vector<Mesh>> m_meshes;
	std::vector<Material> m_materials;
	std::string m_name;
	bool m_play;
	size_t m_step;
	size_t m_skip;
	DelayTimer m_timer;
};

class AnimMeshRenderer : public EntityComponent
{
public:
	AnimMeshRenderer()
		:
		m_anim(0)
	{}

	AnimMeshRenderer(const std::string& fileName, int stepSize)
		:
		m_anim(0)
	{
		LoadData(fileName, stepSize);
	}

	virtual ~AnimMeshRenderer()
	{
		for (auto x : m_anims)
		{
			delete x;
		}
	}

	virtual void Update(float delta)
	{
		if (m_anims.size() == 0)return;
		m_anims.at(m_anim)->Update(delta);
	}

	void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
	{
		if (m_anims.size() == 0)return;
		m_anims.at(m_anim)->Render(GetTransform(), shader, renderingEngine, camera);
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

	void LoadData(std::string filename, int stepSize)
	{
		std::string filelist = Util::ExecuteTask("listdir_dir", (Util::ResourcePath() + "models/" + filename));
		std::vector<std::string> files = Util::Split(filelist, '@');
		
		for (auto x : files)
		{
			m_anims.push_back(new MeshSequence(filename + "/" + x, stepSize));
		}

		for (size_t i = 0; i < files.size(); i++)
		{
			m_anims.push_back(new MeshSequence(filename + "/" + files[i], stepSize));
		}
	}

	void SetAnimState(std::string anim)
	{
		int i = 0;
		for (auto x : m_anims)
		{
			if (x->GetName() == anim)
			{
				m_anim = i;
				m_anims.at(i)->SetStep(-1);
				return;
			}
			++i;
		}
	}

	std::string GetAnimState() 
	{
		return m_anims.at(m_anim)->GetName();
	}

	void SetPlayState(bool play)
	{
		m_anims.at(m_anim)->SetPlay(play);
	}

protected:
private:
	std::vector<MeshSequence*> m_anims;
	int m_anim;
};

#endif // MESHRENDERER_H_INCLUDED
