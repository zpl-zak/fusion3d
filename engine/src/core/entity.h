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

#ifndef ENTITYOBJECT_H
#define ENTITYOBJECT_H

#include <vector>
#include "../reflection.h"
#include "transform.h"
#include "input.h"

class Camera;
class CoreEngine;
class EntityComponent;
class RigidBody;
class Shader;
class RenderingEngine;

/**
 * Represents game object with it's components and transformation. This is the base unit of scene graph.
 */
class Entity
{
public:
	Entity(const Vector3f& pos = Vector3f(0, 0, 0), const Quaternion& rot = Quaternion(0, 0, 0, 1), float scale = 1.0f);
		
	virtual ~Entity();
	
	Entity* AddChild(Entity* child);
	Entity* AddComponent(EntityComponent* component);
    Entity* AddComponents(std::vector<EntityComponent*> components);
	
	void InitAll();
	void ProcessInputAll(const Input& input, float delta);
	void UpdateAll(float delta);
	void RenderAll( Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);
	void PostRenderAll(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const;
	
	/// Returns all components attached to entity.
	std::vector<Entity*> GetAllAttached();
	inline std::vector<Entity*> GetAllEntities() { return m_children; }
	inline std::vector<EntityComponent*> GetAllComponents() { return m_components; }

	inline Transform* GetTransform() { return &m_transform; }
	inline Entity* GetParent() { return m_parent; }
	inline const std::string& GetDisplayName() const { return m_displayName; }
	
	inline Entity* SetDisplayName (const std::string& displayName) { m_displayName = displayName; return this; }
	inline void SetTransform(Transform transform) { m_transform = transform; }
	void SetEngine (CoreEngine* engine);
	inline void SetParent (Entity* parent) { m_parent = parent; }

	inline void Destroy () { m_destroy = true; }

	/// Retrieves our scene graph.
	Entity* GetScene ();
	Entity* GetChildByName (const std::string& name);
	
	Entity* GetChildByIndex (unsigned int ID);
	std::vector<EntityComponent*> GetComponentsByType(const std::string& name);

protected:
private:
	Entity*							m_parent;
	std::vector<Entity*>          m_children;
	std::vector<EntityComponent*> m_components;
	std::string						m_displayName;
	Transform                     m_transform;
	CoreEngine*                   m_coreEngine;
	RigidBody*					 m_picker;
	bool						m_destroy;

	void Init();
	void ProcessInput(const Input& input, float delta);
	void Update(float delta);
	void Render( Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);
	void PostRender(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const;
	
	Entity(const Entity& other) {}
	void operator=(const Entity& other) {}
};

#endif // GAMEOBJECT_H
