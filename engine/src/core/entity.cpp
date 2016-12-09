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

#include "../3DEngine.h"
#include "entity.h"
#include "entityComponent.h"
#include "coreEngine.h"
#include "../components/rigidBody.h"

Entity::Entity(const Vector3f& pos, const Quaternion& rot, float scale) :
	m_transform(pos, rot, scale),
	m_coreEngine(0),
	m_displayName("undefined"),
	m_parent(0),
	m_destroy(false)
{
}

Entity::~Entity()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i])
		{
			delete m_components[i];
		}
	}

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i])
		{
			delete m_children[i];
		}
	}
	//                            	delete m_picker;
}

Entity* Entity::AddChild(Entity* child)
{
	m_children.push_back(child);
	child->GetTransform()->SetParent(&m_transform);
	child->SetParent(this);
	child->SetEngine(m_coreEngine);
	return this;
}

Entity* Entity::AddComponent(EntityComponent* component)
{
	m_components.push_back(component);
	component->SetParent(this);

	if (m_coreEngine && m_coreEngine->GetRenderingEngine() && m_coreEngine->GetPhysicsEngine())
		component->AddToEngine(m_coreEngine);

	component->Init();
	return this;
}

Entity* Entity::AddComponents(std::vector<EntityComponent*> components)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		AddComponent(components.at(i));
	}

	return this;
}

void Entity::InitAll()
{
	Init();

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] != NULL)
			m_children[i]->Init();
	}
}

void Entity::ProcessInputAll(const Input& input, float delta)
{
	ProcessInput(input, delta);

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] != NULL)
			m_children[i]->ProcessInputAll(input, delta);
	}
}

void Entity::UpdateAll(float delta)
{
	Update(delta);
	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i]->m_destroy)
		{
			delete m_children[i];
			m_children.erase(m_children.begin() + i);
			--i;
		}
		else m_children[i]->UpdateAll(delta);
	}
}

void Entity::RenderAll(Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
{
	Render(shader, renderingEngine, camera);

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] != NULL)
			m_children[i]->RenderAll(shader, renderingEngine, camera);
	}
}

void Entity::PostRenderAll(const Shader& shader, RenderingEngine& renderingEngine, const Camera& camera) const
{
	PostRender(shader, renderingEngine, camera);

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] != NULL)
			m_children[i]->PostRenderAll(shader, renderingEngine, camera);
	}
}

void Entity::Init()
{
	auto scale = m_transform.GetScale();
	m_picker = new RigidBody(new btBoxShape(btVector3(scale, scale, scale)), 0, false);

	m_picker->SetParent(this);
	m_picker->Init();

	m_picker->GetBody()->setCollisionFlags(m_picker->GetBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void Entity::ProcessInput(const Input& input, float delta)
{
	m_transform.Update();

	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->ProcessInput(input, delta);
	}
}

void Entity::Update(float delta)
{
	//SCALL (m_picker, m_picker->Update (delta));

	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->Update(delta);
	}
}

void Entity::Render(Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->Render(shader, renderingEngine, camera);
	}
}

void Entity::PostRender(const Shader& shader, RenderingEngine& renderingEngine, const Camera& camera) const
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->PostRender(shader, renderingEngine, camera);
	}
}

void Entity::SetEngine(CoreEngine* engine)
{
	if (m_coreEngine != engine)
	{
		m_coreEngine = engine;

		for (unsigned int i = 0; i < m_components.size(); i++)
		{
			m_components[i]->AddToEngine(engine);
		}

		for (unsigned int i = 0; i < m_children.size(); i++)
		{
			m_children[i]->SetEngine(engine);
		}
	}
}

Entity* Entity::GetScene()
{
	Entity* parent = m_parent;
	Entity* last_parent = parent;

	while (parent != 0)
	{
		last_parent = parent;
		parent = parent->GetParent();
	}

	return last_parent;
}

Entity* Entity::GetChildByName(const std::string& name)
{
	Entity* result = 0;

	for (size_t i = 0; i < m_children.size(); i++)
	{
		if (m_children.at(i)->GetDisplayName() == name)
		{
			result = m_children.at(i);
			break;
		}
		else
		{
			result = m_children.at(i)->GetChildByName(name);
			if (result != 0) break;
		}
	}

	return result;
}

Entity* Entity::GetChildByIndex(unsigned int ID)
{
	if (ID < 0 || ID > m_children.size() + 1) return nullptr;

	return m_children.at(ID);
}

std::vector<Entity*> Entity::GetAllAttached()
{
	std::vector<Entity*> result;

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		std::vector<Entity*> childObjects = m_children[i]->GetAllAttached();
		result.insert(result.end(), childObjects.begin(), childObjects.end());
	}

	result.push_back(this);
	return result;
}

std::vector<EntityComponent*> Entity::GetComponentsByType(const std::string& name)
{
	std::vector<EntityComponent*> components;

	for (auto x : m_components)
	{
		if (x->__ClassType__() == name)
			components.push_back(x);
	}

	return components;
}
