#pragma once
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

#ifndef GUIWINDOW_H_INCLUDED
#define GUIWINDOW_H_INCLUDED "EntityComponent"

#include "../core/entityComponent.h"
#include "../rendering/mesh.h"
#include "../rendering/shader.h"

class GUIWindow : public EntityComponent
{
public:
	GUIWindow(std::vector<Mesh*> meshes, std::vector<Material*> materials)
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

	GUIWindow(const std::string& gui) :
		GUIWindow(Mesh::ImportMesh(gui + ".obj"), Mesh::ImportMeshMaterial(gui + ".obj"))
	{}

	virtual ~GUIWindow()
	{
	}

	virtual void Init() 
	{
		m_visible = true;
	}

	virtual void ProcessInput(const Input& input, float delta) {}
	
	virtual void Update(float delta)
	{
	}

	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
	{
		if (shader.GetName() == "shadowMapGenerator") return;
		if (!m_visible) return;
		for (size_t i = 0; i < m_mesh.size(); i++)
		{
			shader.Bind();
			shader.UpdateUniforms(GetTransform(), m_material.at(m_mesh.at(i).GetMeshData()->GetMaterialIndex()), renderingEngine, camera);
			m_mesh.at(i).Draw();
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

protected:
private:
	std::vector<Mesh> m_mesh;
	std::vector<Material> m_material;
	bool			m_visible;
	Transform		m_t;
};

#endif // GUIWINDOW_H_INCLUDED
