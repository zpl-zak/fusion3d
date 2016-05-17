// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#include "freeMove.h"
	
void FreeMove::ProcessInput(const Input& input, float delta)
{
	float movAmt = m_speed * delta;

	if (input.GetKey(m_shiftKey))
		movAmt *= 10.0f;

	if(input.GetKey(m_forwardKey))
		Move(GetTransform()->GetRot()->GetForward(), movAmt);
	if(input.GetKey(m_backKey))
		Move(GetTransform()->GetRot()->GetBack(), movAmt);
	if(input.GetKey(m_leftKey))
		Move(GetTransform()->GetRot()->GetLeft(), movAmt);
	if(input.GetKey(m_rightKey))
		Move(GetTransform()->GetRot()->GetRight(), movAmt);
}

void FreeMove::DebugDrawUI()
{
	ImGui::InputFloat("Speed", &m_speed);
}

void FreeMove::Move(const Vector3f& direction, float amt)
{
	GetTransform()->SetPos(*GetTransform()->GetPos() + (direction * amt));
}
