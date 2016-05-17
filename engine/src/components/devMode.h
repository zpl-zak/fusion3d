/*
* Copyright (C) 2015-2016 Subvision Studio
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

#ifndef DEBUG_PROG_H
#define DEBUG_PROG_H

#include "../core/coreEngine.h"
#include "../components/rigidBody.h"
#include "../components/ProgramComponent.h"
#include "../components/freeMove.h"
#include "../components/freeLook.h"
#include "../components/meshRenderer.h"
#include "../core/util.h"
#include <vector>

#include <Windows.h>

#include "../staticLibs/imgui.h"

class DevMode : public EntityComponent
{
public:
	FCLASS (DevMode);
	DevMode() {}
	DevMode(const Window& window, bool isDev = true);

	virtual ~DevMode();

	virtual void Init();

	virtual void ProcessInput(const Input& input, float delta);

	virtual void Update(float delta);

	virtual void PostRender(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);
	

private:
	CoreEngine* m_engine;
	FreeMove* m_move;
	FreeLook* m_look;
	const Window* m_window;
	double speed;
	bool m_isUpdating;
	bool m_isDev, m_isCmd;
	bool m_showCursor;
	std::vector<char> m_keySequence;
	std::vector<RigidBody*> m_selected;
	char m_handleMode;

	Entity* m_gizmoSphere;

	bool m_statsWnd;

	bool show_entity_manager;

	bool enable_physics;

	enum HANDLE_STATE {
		HANDLE_SELECT,
		HANDLE_TRANSFORM,
		HANDLE_ROTATE,
		HANDLE_SCALE
	};

	Camera m_devCamera;
	const Camera* m_lastCamera;

	void EvalKeySequence();
};

#endif