// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef FREEMOVE_H
#define FREEMOVE_H "Freemove"

#include "../core/math3d.h"
#include "../core/entityComponent.h"

class FreeMove : public EntityComponent
{
public:
	FCLASS (FreeMove);
	FreeMove() {}
	FreeMove(float speed, int forwardKey = Input::KEY_W, int backKey = Input::KEY_S, int leftKey = Input::KEY_A, int rightKey = Input::KEY_D, int shiftKey = Input::KEY_LSHIFT)  :
		m_speed(speed),
		m_forwardKey(forwardKey),
		m_backKey(backKey),
		m_leftKey(leftKey),
		m_rightKey(rightKey),
		m_shiftKey(shiftKey) {}
	
	virtual void ProcessInput(const Input& input, float delta);

	virtual void DebugDrawUI();

protected:
private:
	void Move(const Vector3f& direction, float amt);

	float m_speed;
	int m_forwardKey;
	int m_backKey;
	int m_leftKey;
	int m_rightKey;
	int m_shiftKey;
};

#endif // FREEMOVE_H
