#ifndef FPSWALKER_PROG_H
#define FPSWALKER_PROG_H

#include "3DEngine.h"
#include "components/physicsObjectComponent.h"
#include "rendering/camera.h"
#include "components/freeLook.h"

class FPSWalker : public Program
{
public:
	FPSWalker(float aspect, Vector2f center, float sensitivity)
	{
		m_body = new RigidBody(
			new btCapsuleShape(1, 2),
			2,
			false
			);

		m_camera = new CameraComponent(Matrix4f().InitPerspective(
			ToRadians(70.0f), aspect, 0.1f, 1000.0f));

		m_look = new FreeLook(center, sensitivity);
	}

	virtual ~FPSWalker()
	{
		delete m_look;
	}

	virtual int ProcessInput(const Input& input, float delta)
	{
		Transform * t = GetParent()->GetTransform();
		float speed = 12;

		if (input.GetKey(Input::KEY_W))
		{
			m_body->GetBody()->setLinearVelocity(t->GetRot()->GetForward().GetXZ().GetBT() * speed + btVector3(0, m_body->GetBody()->getLinearVelocity().getY(), 0));
		}

		if (input.GetKey(Input::KEY_S))
		{
			m_body->GetBody()->setLinearVelocity(-t->GetRot()->GetForward().GetXZ().GetBT() * speed + btVector3(0, m_body->GetBody()->getLinearVelocity().getY(), 0));
		}

		if (input.GetKey(Input::KEY_A))
		{
			m_body->GetBody()->setLinearVelocity(t->GetRot()->GetLeft().GetBT() * speed + btVector3(0, m_body->GetBody()->getLinearVelocity().getY(), 0));
		}

		if (input.GetKey(Input::KEY_D))
		{
			m_body->GetBody()->setLinearVelocity(t->GetRot()->GetRight().GetBT() * speed + btVector3(0, m_body->GetBody()->getLinearVelocity().getY(), 0));
		}

		if (input.GetKeyDown(Input::KEY_SPACE))
		{
			m_body->GetBody()->setLinearVelocity(btVector3(0, 10, 0));
		}

		if (!input.GetKey(Input::KEY_W) && !input.GetKey(Input::KEY_S) && !input.GetKey(Input::KEY_A) && !input.GetKey(Input::KEY_D))
		{
			m_body->GetBody()->setLinearVelocity(btVector3(0,m_body->GetBody()->getLinearVelocity().getY(),0));
		}

		m_look->ProcessInput(input, delta);

		return 1;
	}

	virtual int Init()
	{
		m_look->SetParent(GetParent());

		GetParent()->AddComponent(m_body);
		GetParent()->AddComponent(m_camera);

		m_body->GetBody()->setAngularFactor(0);
		m_body->GetBody()->setActivationState(DISABLE_DEACTIVATION);
		m_body->GetBody()->setSleepingThresholds(0, 0);

		return 1;
	}

	virtual int Update(float delta)
	{
		return 1;
	}

	virtual int Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
	{
		return 1;
	}

private:
	RigidBody* m_body;
	CameraComponent* m_camera;
	FreeLook* m_look;
};

#endif
