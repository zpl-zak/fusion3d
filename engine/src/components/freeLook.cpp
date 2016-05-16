// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#include "freeLook.h"
#include "../rendering/window.h"

void FreeLook::ProcessInput(const Input& input, float delta)
{
	if (input.GetKey (m_unlockMouseKey))
	{
		input.SetCursor (true);
		m_mouseLocked = false;
	}

	if(m_mouseLocked)
	{
		Vector2f deltaPos = input.GetMousePosition() - m_windowCenter;
		
		bool rotY = deltaPos.GetX() != 0;
		bool rotX = deltaPos.GetY() != 0;
			
		if(rotY)
		{
			GetTransform()->Rotate(Vector3f(0,1,0), ToRadians(deltaPos.GetX() * m_sensitivity));
		}
		if(rotX)
		{
			GetTransform()->Rotate(GetTransform()->GetRot()->GetRight(), ToRadians(deltaPos.GetY() * m_sensitivity));
		}
			
		if(rotY || rotX)
		{
			input.SetMousePosition(m_windowCenter);
		}
	}

	if (input.GetKey (m_lockMouseKey))
	{
		input.SetCursor (false);
		input.SetMousePosition (m_windowCenter);
		m_mouseLocked = true;
	}
}
