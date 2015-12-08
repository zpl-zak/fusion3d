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

#include "entity.h"
#include "entityComponent.h"
#include "coreEngine.h"

	 Entity::Entity(const Vector3f & pos, const Quaternion & rot, float scale) :
		 m_transform(pos, rot, scale),
		 m_coreEngine(0),
		 m_displayName("undefined"),
		 m_parent(0),
		 m_destroy(false)
 {

 }

 Entity::~Entity()
{
	for(unsigned int i = 0; i < m_components.size(); i++)
	{
		if(m_components[i])
		{	
			delete m_components[i];
		}
	}
	
	for(unsigned int i = 0; i < m_children.size(); i++)
	{
		if(m_children[i]) 
		{
			delete m_children[i];
		}
	}
	delete m_shape;
	delete m_picker;
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
	component->Init();
	return this;
}

Entity * Entity::AddComponents(std::vector<EntityComponent*> components)
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

	for(unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] != NULL)
			m_children[i]->ProcessInputAll(input, delta);
	}
}

void Entity::UpdateAll(float delta)
{
	Update(delta);
	for(unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i]->m_destroy)
		{
			delete m_children[i];
			m_children[i] = NULL;
			continue;
		}
		if (m_children[i] != NULL)
			m_children[i]->UpdateAll(delta);
	}
}

void Entity::RenderAll(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
{
	Render(shader, renderingEngine, camera);

	for(unsigned int i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] != NULL)
			m_children[i]->RenderAll(shader, renderingEngine, camera);
	}
}

void Entity::PostRenderAll(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
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
	m_shape = new btBoxShape(btVector3(1, 1, 1));
	m_shape->calculateLocalInertia(0, btVector3(0, 0, 0));

	btRigidBody::btRigidBodyConstructionInfo CI
		(0.0f, new btDefaultMotionState(GetTransform()->GetBT()), m_shape, btVector3(0, 0, 0));

	m_picker = new btRigidBody(CI);
	m_picker->setUserPointer((void*)m_picker);
	m_picker->setCollisionFlags(m_picker->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	CoreEngine::GetCoreEngine()->GetPhysicsEngine()->GetWorld()->addRigidBody(m_picker);
}

void Entity::ProcessInput(const Input& input, float delta)
{
	m_transform.Update();

	for(unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->ProcessInput(input, delta);
	}
}

void Entity::Update(float delta)
{
	for(unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->Update(delta);
	}
}

void Entity::Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
{
	for(unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->Render(shader, renderingEngine, camera);
	}
}

void Entity::PostRender(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		m_components[i]->PostRender(shader, renderingEngine, camera);
	}
}

void Entity::SetEngine(CoreEngine* engine)
{
	if(m_coreEngine != engine)
	{
		m_coreEngine = engine;
		
		for(unsigned int i = 0; i < m_components.size(); i++)
		{
			m_components[i]->AddToEngine(engine);
		}

		for(unsigned int i = 0; i < m_children.size(); i++)
		{
			m_children[i]->SetEngine(engine);
		}
	}
}

std::vector<Entity*> Entity::GetAllAttached()
{
	std::vector<Entity*> result;
	
	for(unsigned int i = 0; i < m_children.size(); i++)
	{
		std::vector<Entity*> childObjects = m_children[i]->GetAllAttached();
		result.insert(result.end(), childObjects.begin(), childObjects.end());
	}
	
	result.push_back(this);
	return result;
}

EntityComponent* Entity::GetComponentByType(const std::string& name)
{
	return (EntityComponent*)g_factory.m_classes[name];
}