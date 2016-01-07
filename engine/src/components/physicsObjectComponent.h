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
#ifndef PHYSICS_OBJECT_COMPONENT_INCLUDED_H
#define PHYSICS_OBJECT_COMPONENT_INCLUDED_H

#include <iostream>
#include <cassert>

#include "../core/coreEngine.h"
#include "../core/timing.h"
#include "../core/entityComponent.h"
#include "btBulletDynamicsCommon.h"


class RigidBody : public EntityComponent
{
public:
	FCLASS (RigidBody);
	RigidBody () { }
	RigidBody (btCollisionShape* shape, float mass = 0.0f, bool calcInertia = true, Vector3f inertia = Vector3f (0, 0, 0)) :
		m_shape (shape),
		m_mass (mass)
	{
		if (calcInertia)
		{
			m_shape->calculateLocalInertia (m_mass, m_inertia);
		}

		CoreEngine::GetCoreEngine ()->SetSimulation (true);
	}

	virtual ~RigidBody ()
	{
		CoreEngine::GetCoreEngine ()->GetPhysicsEngine ()->GetWorld ()->removeRigidBody (m_body);
	}

	virtual void DataDeploy (tinyxml2::XMLElement* data)
	{
		btCollisionShape* shape = nullptr;
		float mass = 0.0f;
		bool calcInertia = false;
		Vector3f inertia = Vector3f (0, 0, 0);

		if (data->Attribute ("shape"))
		{
			std::string s = data->Attribute ("shape");

			if (s == "bvh")
			{
				shape = new btBvhTriangleMeshShape (Mesh::ImportCollision (data->GetText ()), true);
			}
			else if (s == "convex")
			{
				shape = new btConvexTriangleMeshShape (Mesh::ImportCollision (data->GetText ()), true);
			}
			else if (s == "box")
			{
				shape = new btBoxShape (Util::ParseVector3 (data->GetText ()).GetBT ());
			}
			else if (s == "sphere")
			{
				shape = new btSphereShape (atof (data->GetText ()));
			}
			else if (s == "capsule")
			{
				std::vector<std::string> p = Util::Split (data->GetText (), ';');
				shape = new btCapsuleShape (atof (p[0].c_str ()), atof (p[1].c_str ()));
			}
			else if (s == "cylinder")
			{
				shape = new btCylinderShape (Util::ParseVector3 (data->GetText ()).GetBT ());
			}
			else
			{
				std::cerr << "Unsupported collision shape! " << s << std::endl;
				assert (1 == 0);
			}
		}
		else
		{
			std::cerr << "Unspecified collision shape! " << std::endl;
			assert (1 == 0);
		}

		if (data->Attribute ("mass"))
		{
			mass = atof (data->Attribute ("mass"));
		}

		if (data->Attribute ("calcInertia"))
		{
			if (atoi (data->Attribute ("calcInertia")) == 0)
			{
				calcInertia = false;
			}
			else
			{
				calcInertia = true;
			}
		}

		if (data->Attribute ("inertia"))
		{
			inertia = Util::ParseVector3 (data->Attribute ("inertia"));
		}

		if (calcInertia)
		{
			shape->calculateLocalInertia (mass, inertia.GetBT ());
		}

		m_shape = shape;
		m_mass = mass;

		CoreEngine::GetCoreEngine ()->SetSimulation (true);
	}

	virtual void Init ()
	{
		if (m_shape == nullptr) return;
		m_state = new btDefaultMotionState (GetTransform ()->GetBT ());

		btRigidBody::btRigidBodyConstructionInfo CI
			(m_mass, m_state, m_shape, m_inertia);

		m_body = new btRigidBody (CI);

		m_body->setUserPointer ((void*)this);

		CoreEngine::GetCoreEngine ()->GetPhysicsEngine ()->GetWorld ()->addRigidBody (m_body);
	}

	virtual void Update (float delta)
	{
		btTransform bt;
		m_body->getMotionState ()->getWorldTransform (bt);
		GetTransform ()->SetPos (Vector3f::GetFT (bt.getOrigin ()));
		m_shape->setLocalScaling (btVector3 (GetTransform ()->GetScale (), GetTransform ()->GetScale (), GetTransform ()->GetScale ()));
		if (m_inertia != btVector3 (0, 0, 0))
		{
			GetTransform ()->SetRot (Quaternion::GetFT (bt.getRotation ()));
		}

		Stay ();
		Exit ();
	}

	void Enter (RigidBody* hit)
	{
		for (auto x : m_contacts)
		{
			if (x.first == hit)
			{
				if (x.second == 0)
				{
					if (m_enter != nullptr)
						m_enter (hit);
					x.second = 2;
				}
				else x.second++;
				return;
			}
		}

		m_contacts.push_back (std::pair<RigidBody*, int> (hit, 2));

		if (m_enter != nullptr)
			m_enter (hit);
	}

	void Stay ()
	{
		for (auto x : m_contacts)
		{
			if (x.second == 2)
			{
				if (m_stay != nullptr)
					m_stay (x.first);
			}
		}
	}

	void Exit ()
	{
		for (auto x : m_contacts)
		{
			if (x.second > 0)
			{
				x.second--;
			}

			if (x.second == 0)
			{
				if (m_exit != nullptr)
					m_exit (x.first);
			}
		}
	}

	inline btRigidBody* GetBody () { return m_body; }

	inline void UpdateTransform ()
	{
		delete m_state;
		m_state = new btDefaultMotionState (GetTransform ()->GetBT ());
		m_body->setMotionState (m_state);
		m_body->setLinearVelocity (btVector3 ());
	}

	void SetEnter (std::function<void (RigidBody* hit)> f)
	{
		m_enter = f;
	}

	void SetStay (std::function<void (RigidBody* hit)> f)
	{
		m_stay = f;
	}

	void SetExit (std::function<void (RigidBody* hit)> f)
	{
		m_exit = f;
	}

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
