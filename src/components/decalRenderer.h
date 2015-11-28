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

#ifndef DECALRENDERER_H_INCLUDED
#define DECALRENDERER_H_INCLUDED

#include "../core/entityComponent.h"
#include "../rendering/mesh.h"

COMPONENT(DecalRenderer)
public:
	
	DecalRenderer(Material* material)
	{
		m_material = material;
	}

	virtual void Init()
	{
		m_visible = true;
	}

	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
	{
		if (!m_visible) return;
		
		shader.Bind();
		shader.UpdateUniforms(GetTransform(),m_material, renderingEngine, camera);
		
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
	Material* m_material;
	bool			m_visible;
};

#endif // DECALRENDERER_H_INCLUDED
