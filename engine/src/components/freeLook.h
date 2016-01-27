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
	
	virtual void ProcessInput(const Input& input, float delta);
protected:
private:
	float    m_sensitivity;
	bool     m_mouseLocked;
	int		 m_unlockMouseKey;
	int		 m_lockMouseKey;
	Vector2f m_windowCenter;
};

#endif // FREELOOK_H
