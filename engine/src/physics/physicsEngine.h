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

/* * @file
 * @author Subvision Studio 
 * @section LICENSE
 *
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
#ifndef PHYSICS_ENGINE_INCLUDED_H
#define PHYSICS_ENGINE_INCLUDED_H

#include "btBulletDynamicsCommon.h"
#include "../rendering/renderingEngine.h"
#include <vector>

/**
 * The PhysicsEngine encapsulates all the functions and information necessary
 * to perform a physics simulation.
 */
class PhysicsEngine
{
public:
	/** 
	 * Creates a PhysicsEngine in a usable state.
	 */
	PhysicsEngine() 
	{
		m_broadphase = new btDbvtBroadphase();
		m_configuration = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher(m_configuration);
		m_solver = new btSequentialImpulseConstraintSolver;
		m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_configuration);
		m_world->setGravity(btVector3(0, -9.81f, 0));
	}

	virtual ~PhysicsEngine()
	{
		delete m_world;
		delete m_solver;
		delete m_dispatcher;
		delete m_configuration;
		delete m_broadphase;
	}
	

	void Simulate(float delta);

	inline btDiscreteDynamicsWorld* GetWorld()
	{
		return m_world;
	}

private:
	btBroadphaseInterface* m_broadphase;
	btDefaultCollisionConfiguration* m_configuration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_world;
};

#endif
