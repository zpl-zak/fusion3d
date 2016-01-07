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

#include "physicsEngine.h"
#include "../components/physicsObjectComponent.h"


void PhysicsEngine::Simulate(float delta)
{
	m_world->stepSimulation(1 / 60.0f, 1);

	int numManifolds = GetWorld()->getDispatcher()->getNumManifolds ();
	for (size_t i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* c = GetWorld()->getDispatcher ()->getManifoldByIndexInternal (i);
		btCollisionObject* a = (btCollisionObject*) (c->getBody0 ());
		btCollisionObject* b = (btCollisionObject*) (c->getBody1 ());

		int co = c->getNumContacts ();
		for (size_t j = 0; j < co; j++)
		{
			btManifoldPoint& pt = c->getContactPoint (j);
			if (pt.getDistance () < 0.f)
			{
				//TODO: Implement collision list instead of calling OnCollide for each contact.
				RigidBody* ah = (RigidBody*) a->getUserPointer ();
				RigidBody* bh = (RigidBody*) b->getUserPointer ();

				ah->Enter (bh);
			}
		}
	}
}
