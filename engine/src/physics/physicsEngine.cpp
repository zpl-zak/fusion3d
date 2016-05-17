// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#include "physicsEngine.h"
#include "../components/rigidBody.h"


void PhysicsEngine::Simulate(float delta)
{
	if (!m_simulation)return;

	m_world->stepSimulation(1 / 60.0f, 1);

	size_t numManifolds = GetWorld()->getDispatcher()->getNumManifolds ();
	for (size_t i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* c = GetWorld()->getDispatcher ()->getManifoldByIndexInternal (i);
		btCollisionObject* a = (btCollisionObject*) (c->getBody0 ());
		btCollisionObject* b = (btCollisionObject*) (c->getBody1 ());

		size_t co = c->getNumContacts ();
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
