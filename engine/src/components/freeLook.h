// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef FREELOOK_H
#define FREELOOK_H "Freelook"

#include "../core/math3d.h"
#include "../core/entityComponent.h"

class FreeLook : public EntityComponent
{
public:
	FCLASS (FreeLook);
	FreeLook() {}
	FreeLook(const Vector2f& windowCenter, float sensitivity = 0.5f, int unlockMouseKey = Input::KEY_ESCAPE, int lockMouseKey = Input::KEY_RETURN) :
		m_sensitivity(sensitivity),
		m_mouseLocked(true),
		m_unlockMouseKey(unlockMouseKey),
		m_lockMouseKey (lockMouseKey),
		m_windowCenter(windowCenter) {}

	void ProcessInput(const Input& input, float delta) override;

	virtual void DebugDrawUI()
	{
		ImGui::InputFloat("Sensitivity", &m_sensitivity);
	}

protected:
private:
	float    m_sensitivity;
	bool     m_mouseLocked;
	int		 m_unlockMouseKey;
	int		 m_lockMouseKey;
	Vector2f m_windowCenter;
};

#endif // FREELOOK_H
