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

#ifndef ENTITYCOMPONENT_H_INCLUDED
#define ENTITYCOMPONENT_H_INCLUDED "EntityComponent"

#include "../reflection.h"
#include "../staticLibs/tinyxml2.h"
#include "../staticLibs/imgui.h"
#include "../reflection.h"

#include "transform.h"
#include "entity.h"
#include "input.h"
class RenderingEngine;
class Shader;

/**
 * Represents functional component attached to entity.
*/
class EntityComponent{
public:

	FCLASS (EntityComponent);

	EntityComponent () :
		m_parent (0) {}
	virtual ~EntityComponent() {}

	virtual std::string _GetClassName_() { return "EntityComponent"; }
	virtual void Init() { }
	virtual void ProcessInput(const Input& input, float delta) {}
	virtual void Update(float delta) {}
	virtual void Render(Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) {}
	virtual void PostRender(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const {}
	virtual void PostRender (const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) {}
	virtual void DataDeploy(tinyxml2::XMLElement* data) {}
	virtual void AddToEngine(CoreEngine* engine) { }
	virtual void DrawDebugUI() {}
	
	inline Transform* GetTransform()             { return m_parent->GetTransform(); }
	inline const Transform& GetTransform() const { return *m_parent->GetTransform(); }
	
	virtual void SetParent(Entity* parent) { m_parent = parent; }
	inline Entity* GetParent() { return m_parent; }
private:
	Entity* m_parent;
	
	EntityComponent(const EntityComponent& other) {}
	void operator=(const EntityComponent& other) {}
};

#endif // ENTITYCOMPONENT_H_INCLUDED
