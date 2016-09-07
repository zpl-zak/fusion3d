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


#include "rigidBody.h"

RigidBody::~RigidBody()
{
	CoreEngine::GetCoreEngine()->GetPhysicsEngine()->GetWorld()->removeRigidBody(m_body);
}

void RigidBody::DebugDrawUI()
{

}

void * RigidBody::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void RigidBody::operator delete(void * p)
{
	_mm_free(p);
}

void RigidBody::DataDeploy(tinyxml2::XMLElement * data)
{
	btCollisionShape* shape = nullptr;
	float mass = 0.0f;
	bool calcInertia = false;
	Vector3f inertia = Vector3f(0, 0, 0);

	if (data->Attribute("shape"))
	{
		std::string s = data->Attribute("shape");

		if (s == "bvh")
		{
			shape = new btBvhTriangleMeshShape(Mesh::ImportCollision(data->GetText()), true);
		}
		else if (s == "convex")
		{
			shape = new btConvexTriangleMeshShape(Mesh::ImportCollision(data->GetText()), true);
		}
		else if (s == "box")
		{
			shape = new btBoxShape(Util::ParseVector3(data->GetText()).GetBT());
		}
		else if (s == "sphere")
		{
			shape = new btSphereShape(atof(data->GetText()));
		}
		else if (s == "capsule")
		{
			std::vector<std::string> p = Util::Split(data->GetText(), ';');
			shape = new btCapsuleShape(atof(p[0].c_str()), atof(p[1].c_str()));
		}
		else if (s == "cylinder")
		{
			shape = new btCylinderShape(Util::ParseVector3(data->GetText()).GetBT());
		}
		else
		{
			std::cerr << "Unsupported collision shape! " << s << std::endl;
			assert(1 == 0);
		}
	}
	else
	{
		std::cerr << "Unspecified collision shape! " << std::endl;
		assert(1 == 0);
	}

	if (data->Attribute("mass"))
	{
		mass = atof(data->Attribute("mass"));
	}

	if (data->Attribute("calcInertia"))
	{
		if (atoi(data->Attribute("calcInertia")) == 0)
		{
			calcInertia = false;
		}
		else
		{
			calcInertia = true;
		}
	}

	if (data->Attribute("inertia"))
	{
		inertia = Util::ParseVector3(data->Attribute("inertia"));
	}

	if (calcInertia)
	{
		shape->calculateLocalInertia(mass, inertia.GetBT());
	}

	m_shape = shape;
	m_mass = mass;
}

void RigidBody::Init()
{
	if (m_shape == nullptr) return;
	m_state = new btDefaultMotionState(GetTransform()->GetBT());

	btRigidBody::btRigidBodyConstructionInfo CI
	(m_mass, m_state, m_shape, m_inertia);

	m_body = new btRigidBody(CI);

	m_body->setUserPointer((void*)this);

	CoreEngine::GetCoreEngine()->GetPhysicsEngine()->GetWorld()->addRigidBody(m_body);
}

void RigidBody::Update(float delta)
{
	if (!CoreEngine::GetCoreEngine()->GetPhysicsEngine()->GetSimulation()) {
		m_body->getMotionState()->setWorldTransform(GetTransform()->GetBT());
		m_body->setWorldTransform(GetTransform()->GetBT());
		return;
	}

	btTransform bt;
	m_body->getMotionState()->getWorldTransform(bt);

	GetTransform()->SetPos(Vector3f::GetFT(bt.getOrigin()));

	m_shape->setLocalScaling(btVector3(GetTransform()->GetScale(), GetTransform()->GetScale(), GetTransform()->GetScale()));
	if (m_inertia != btVector3(0, 0, 0))
	{
		GetTransform()->SetRot(Quaternion::GetFT(bt.getRotation()));
	}

	Stay();
	Exit();
}

void RigidBody::Enter(RigidBody * hit)
{
	for (auto x : m_contacts)
	{
		if (x.first == hit)
		{
			if (x.second == 0)
			{
				if (m_enter != nullptr)
					m_enter(hit);
				x.second = 2;
			}
			else x.second++;
			return;
		}
	}

	m_contacts.push_back(std::pair<RigidBody*, int>(hit, 2));

	if (m_enter != nullptr)
		m_enter(hit);
}

void RigidBody::Stay()
{
	for (auto x : m_contacts)
	{
		if (x.second == 2)
		{
			if (m_stay != nullptr)
				m_stay(x.first);
		}
	}
}

void RigidBody::Exit()
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
				m_exit(x.first);
		}
	}
}

btRigidBody * RigidBody::GetBody() { return m_body; }

void RigidBody::UpdateTransform()
{
	delete m_state;
	m_state = new btDefaultMotionState(GetTransform()->GetBT());
	m_body->setMotionState(m_state);
	m_body->setLinearVelocity(btVector3());
}

void RigidBody::SetEnter(std::function<void(RigidBody*hit)> f)
{
	m_enter = f;
}

void RigidBody::SetStay(std::function<void(RigidBody*hit)> f)
{
	m_stay = f;
}

void RigidBody::SetExit(std::function<void(RigidBody*hit)> f)
{
	m_exit = f;
}
