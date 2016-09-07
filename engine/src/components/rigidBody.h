/*
 * Copyright (C) 2015-2016 Dominik "ZaKlaus" Madarasz
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

#ifndef PHYSICS_OBJECT_COMPONENT_INCLUDED_H
#define PHYSICS_OBJECT_COMPONENT_INCLUDED_H

#include <iostream>
#include <cassert>

#include "../core/coreEngine.h"
#include "../core/timing.h"
#include "../core/entityComponent.h"
#include "btBulletDynamicsCommon.h"


__declspec(align(16)) class RigidBody : public EntityComponent
{
public:
	FCLASS (RigidBody);

	RigidBody() : m_body(nullptr), m_shape(nullptr), m_state(nullptr), m_mass(0)
	{ }

	explicit RigidBody(btCollisionShape* shape, float mass = 0.0f, bool calcInertia = true, Vector3f inertia = Vector3f(0, 0, 0)) : m_body(nullptr),
		m_shape(shape), m_state(nullptr),
		m_mass(mass)
	{
		if (calcInertia)
		{
			m_shape->calculateLocalInertia(m_mass, m_inertia);
		}
	}

	virtual ~RigidBody();

	virtual void DebugDrawUI();

	void* operator new(size_t i);
	
	void operator delete(void* p);

	virtual void DataDeploy(tinyxml2::XMLElement* data);

	virtual void Init();

	virtual void Update(float delta);

	void Enter(RigidBody* hit);

	void Stay();

	void Exit();

	btRigidBody* GetBody();

	void UpdateTransform();

	void SetEnter(std::function<void(RigidBody* hit)> f);

	void SetStay(std::function<void(RigidBody* hit)> f);

	void SetExit(std::function<void(RigidBody* hit)> f);

private:
	btRigidBody* m_body;
	btCollisionShape* m_shape;
	btDefaultMotionState* m_state;
	float m_mass;
	btVector3 m_inertia;
	std::vector<std::pair<RigidBody*, int>> m_contacts;

	std::function<void (RigidBody* hit)> m_enter, m_stay, m_exit;
};


#endif
