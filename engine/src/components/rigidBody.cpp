// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.


#include "rigidBody.h"

RigidBody::RigidBody(): m_body(nullptr), m_shape(nullptr), m_state(nullptr), m_mass(0)
{ }

RigidBody::RigidBody(btCollisionShape* shape, float mass, bool calcInertia, Vector3f inertia): m_body(nullptr),
                                                                                               m_shape (shape), m_state(nullptr),
                                                                                               m_mass (mass)
{
	if (calcInertia)
	{
		m_shape->calculateLocalInertia (m_mass, m_inertia);
	}
}
