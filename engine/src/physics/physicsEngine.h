// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

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
		m_simulation = false;
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

	inline void SetSimulation (bool state) { m_simulation = state; }
	inline bool GetSimulation () { return m_simulation; }

private:
	btBroadphaseInterface* m_broadphase;
	btDefaultCollisionConfiguration* m_configuration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_world;
	bool m_simulation;
};

#endif
