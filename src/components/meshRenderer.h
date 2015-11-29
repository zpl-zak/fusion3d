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
#include "../rendering/mesh.h"

COMPONENT(MeshRenderer)
public:
    MeshRenderer(std::vector<Mesh*> meshes, std::vector<Material*> materials)
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
		for (size_t i = 0; i < meshes.size(); i++)
		{
			m_mesh.push_back(*(meshes.at(i)));
		}

		std::vector<Material*> materials = Mesh::ImportMeshMaterial(data->GetText());
		for (size_t i = 0; i < materials.size(); i++)
		{
			m_material.push_back(*(materials.at(i)));
		}
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
	std::vector<Mesh> m_mesh;
    std::vector<Material> m_material;
	bool			m_visible;
	bool			m_shadows;
};

#endif // MESHRENDERER_H_INCLUDED
