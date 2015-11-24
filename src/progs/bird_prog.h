#ifndef BIRD_PROG_H
#define BIRD_PROG_H

#include "3DEngine.h"
#include "components/physicsObjectComponent.h"
#include "rendering/camera.h"
#include "components/freeLook.h"

PROGRAM(Pipe)
public:
	Pipe()
	{
		m_timer.SetDelay(6);
	}

	virtual int Update(float delta)
	{
		if (m_timer.HasElapsed())
		{
			
		}

		return 1;
	}

private:

	DelayTimer m_timer;
	bool once = false;
};

PROGRAM(Bird)
public:
	Bird(float aspect, Vector2f center, float sensitivity) :
		m_count(1),
		m_pause(true)
	{
		m_body = new PhysicsObjectComponent(
			new btSphereShape(1),
			2
			);
	}

	virtual ~Bird()
	{

	}

	virtual int ProcessInput(const Input& input, float delta)
	{
		Transform * t = GetParent()->GetTransform();
		
		if (input.GetKeyDown(Input::KEY_SPACE))
		{
			m_body->GetBody()->setLinearVelocity(btVector3(m_body->GetBody()->getLinearVelocity().getX(), 10, m_body->GetBody()->getLinearVelocity().getZ()));
		}

		if (input.GetKeyDown(Input::KEY_RETURN))
		{
			m_pause = false;
			CoreEngine::GetCoreEngine()->SetSimulation(true);
		}

		return 1;
	}

	virtual int Init()
	{
		GetParent()->AddComponent(m_body);

		m_body->GetBody()->setAngularFactor(0);
		m_body->GetBody()->setActivationState(DISABLE_DEACTIVATION);
		m_body->GetBody()->setSleepingThresholds(0, 0);

		m_timer.SetDelay(0);
		std::cout << "PRESS ENTER TO PLAY" << std::endl;


		for (size_t i = 0; i < 10; i++)
		{
			GetParent()->GetScene()->AddChild((new Entity(
				Vector3f(0, (rand() % 5) - 5, m_count * 15 + 15)
				))
				->AddComponent(new MeshRenderer("pipe.obj"))
				->AddComponent(new ProgramComponent(new Pipe()))
				->AddComponent(new PhysicsObjectComponent(
					new btBvhTriangleMeshShape(Mesh::ImportCollision("pipe.obj"), true)
					))
				);
			++m_count;
		}

		CoreEngine::GetCoreEngine()->SetSimulation(false);
		return 1;
	}

	virtual int Update(float delta)
	{
		if (!m_pause)
		{
			btDiscreteDynamicsWorld* world = CoreEngine::GetCoreEngine()->GetPhysicsEngine()->GetWorld();
			if (m_timer.HasElapsed())
			{
				m_timer.SetDelay(3);
				GetParent()->GetScene()->AddChild((new Entity(
					Vector3f(0, (rand() % 5) - 5, m_count * 15 + 15)
					))
					->AddComponent(new MeshRenderer("pipe.obj"))
					->AddComponent(new ProgramComponent(new Pipe()))
					->AddComponent(new PhysicsObjectComponent(
						new btBvhTriangleMeshShape(Mesh::ImportCollision("pipe.obj"), true)
						))
					);
				++m_count;
			}
			m_body->GetBody()->setLinearVelocity(btVector3(0, m_body->GetBody()->getLinearVelocity().getY(), 5));

			int numManifolds = world->getDispatcher()->getNumManifolds();
			for (size_t i = 0; i < numManifolds; i++)
			{
				btPersistentManifold* c = world->getDispatcher()->getManifoldByIndexInternal(i);
				btCollisionObject* a = (btCollisionObject*)(c->getBody0());
				btCollisionObject* b = (btCollisionObject*)(c->getBody1());

				int co = c->getNumContacts();
				for (size_t j = 0; j < co; j++)
				{
					btManifoldPoint& pt = c->getContactPoint(j);
					if (pt.getDistance() < 0.f || GetParent()->GetTransform()->GetPos()->GetY() < 10)
					{
						GetParent()->GetTransform()->SetPos(Vector3f(0, 0, m_count * 7.5 + 3.25));
						m_body->UpdateTransform();

						std::cout << "GAME OVER!" << std::endl;
						std::cout << "PRESS ENTER TO PLAY" << std::endl;
						m_pause = true;
						CoreEngine::GetCoreEngine()->SetSimulation(false);
					}
				}
			}
		}
		return 1;
	}

	virtual int Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
	{
		
		return 1;
	}

private:
	DelayTimer m_timer;
	int m_count;
	PhysicsObjectComponent* m_body;
	bool m_pause;
};

PROGRAM(BirdCubemap)
public:
	BirdCubemap() {}

	virtual int Update(float delta)
	{
		m_time += delta * 0.05;

		GetParent()->GetTransform()->SetRot(Quaternion(Vector3f(1,0,0), m_time));

		return 1;
	}

private:
	float m_time;
};
#endif
