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
	DevMode(const Window& window, bool isDev = true)
		:
		m_window(&window),
		m_isUpdating(!isDev),
		m_isDev(isDev),
		m_isCmd(false),
		m_showCursor(false),
		m_devCamera(Matrix4f().InitPerspective(
			ToRadians(70.0f), window.GetAspect(), 0.1f, 100000.0f), 0)

		, enable_physics(false)
	{
		m_engine = CoreEngine::GetCoreEngine();
		m_move = new FreeMove(10.0f);
		m_look = new FreeLook(window.GetCenter());

		m_engine->GetPhysicsEngine ()->SetSimulation(false);
		ProgramHoster::SetActive(m_isUpdating);
		m_gizmoSphere = (new Entity())->AddComponent(new MeshRenderer("arrows.dae"));
	}

	virtual ~DevMode()
	{
		delete m_look;
		delete m_move;
	}

	virtual void Init();

	virtual void ProcessInput(const Input& input, float delta)
	{
		if (m_isDev) 
		{
			if (input.GetMouse(Input::MOUSE_RIGHT_BUTTON))
			{
				m_move->ProcessInput(input, delta);
				m_look->ProcessInput(input, delta);
			}
			else
			{
				if (m_isCmd)
				{
					if (input.GetLastKey() != -1 && ((input.GetLastKey() > 3 && input.GetLastKey() < 40) || input.GetLastKey() == Input::KEY_SPACE))
					{
						if ((char)(input.GetLastKey() + 61) == 'i')
						{
							std::cout << ' ';
						} else
						std::cout << (char)(input.GetLastKey() + 61);
						m_keySequence.push_back(input.GetLastKey());
					}

					if (input.GetKeyDown(Input::KEY_RETURN))
					{
						std::cout << " OK!" << std::endl;
						EvalKeySequence();
					}
				}

				if (input.GetMouseDown (Input::MOUSE_LEFT_BUTTON) && m_handleMode == HANDLE_SELECT)
				{

					Vector3f RayOrigin, RayDir;

					Util::ScreenToWorld (input.GetMousePosition ().GetX (), input.GetMousePosition ().GetY (), m_window->GetWidth (), m_window->GetHeight (), m_devCamera.GetViewProjection ().Inverse (), RayOrigin, RayDir);


					Vector3f end = RayOrigin + RayDir*1000.0f;

					btCollisionWorld::ClosestRayResultCallback RayCallback(
						RayOrigin.GetBT(),
						end.GetBT()
						);

					CoreEngine::GetCoreEngine()->GetPhysicsEngine()->GetWorld()->rayTest(
						RayOrigin.GetBT(),
						end.GetBT(),
						RayCallback
						);

					if (RayCallback.hasHit())
					{
						RigidBody* hitBody = (RigidBody*)RayCallback.m_collisionObject->getUserPointer();
						Entity* hit = hitBody->GetParent ();

						if (hit)
						{
							//std::cout << "Selected: " << hit->GetDisplayName() << "(" << hit << ")" << std::endl;
							//std::cout << hit->GetTransform ()->GetPos ()->GetX () << ", " << hit->GetTransform ()->GetPos ()->GetY () << ", " << hit->GetTransform ()->GetPos ()->GetZ () << std::endl;

							bool exist = false;
							for (size_t i = 0; i < m_selected.size (); i++)
							{
								if (m_selected.at (i) == hitBody)
								{
									m_selected.erase (m_selected.begin () + i);
									exist = true;
								}
							}

							if (input.GetKey (Input::KEY_LSHIFT) || input.GetKey (Input::KEY_RSHIFT))
							{
								if (!exist)
								{
									m_selected.push_back (hitBody);
								}
							}
							else
							{
								m_selected.clear ();
								if (!exist)
								{
									m_selected.push_back (hitBody);
								}
							}

							//std::cout << "Selected Count: " << m_selected.size () << std::endl;
						}
					}
				}
				if (input.GetKeyDown (Input::KEY_Q))
				{
					m_handleMode = HANDLE_SELECT;
					std::cout << "Selection Mode" << std::endl;
				}
				if (input.GetKeyDown (Input::KEY_W))
				{
					m_handleMode = HANDLE_TRANSFORM;
					std::cout << "Transform Mode" << std::endl;
				}
				if (input.GetKeyDown (Input::KEY_E))
				{
					m_handleMode = HANDLE_ROTATE;
					std::cout << "Rotate Mode" << std::endl;
				}
				if (input.GetKeyDown (Input::KEY_R))
				{
					m_handleMode = HANDLE_SCALE;
					std::cout << "Scale Mode" << std::endl;
				}
			}
		}
		
		if (input.GetKeyDown(Input::KEY_F4))
		{
			m_isDev = !m_isDev;
			m_isUpdating = m_engine->GetPhysicsEngine ()->GetSimulation();
			

			if (m_isDev)
			{
				std::cout << "Dev Mode" << std::endl;
				input.SetCursor(true);
				m_lastCamera = m_engine->GetRenderingEngine()->GetMainCamera();
				m_engine->GetRenderingEngine()->SetMainCamera(m_devCamera);

				m_engine->GetPhysicsEngine ()->SetSimulation(false);
				ProgramHoster::SetActive(false);
			}
			else
			{
				std::cout << "Play Mode" << std::endl;
				input.SetCursor(!m_showCursor);
				m_engine->GetRenderingEngine()->SetMainCamera(*m_lastCamera);

				m_engine->GetPhysicsEngine ()->SetSimulation(true);
				ProgramHoster::SetActive(true);
			}

			
			m_showCursor = input.GetCursor();
		}

		if (input.GetKeyDown(Input::KEY_F3))
		{
			if (!m_isDev)return;
			
			m_isCmd = !m_isCmd;

			if (m_isCmd)
			{
				std::cout << "Command Mode" << std::endl;
			}
			else
			{
				std::cout << "Visual Mode" << std::endl;
			}
		}
	}

	virtual void Update(float delta)
	{
		if (m_isDev)
		{

		}
	}

	virtual void PostRender (const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);
	

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

	void EvalKeySequence()
	{
		std::string sequence;
		for (size_t i = 0; i < m_keySequence.size(); i++)
		{
			if ((char)(m_keySequence.at(i) + 61) == 'i')
			{
				sequence += ' ';
				continue;
			}
			sequence += (char)(m_keySequence.at(i) + 61);
		}

	
		std::string command = sequence.substr(0, sequence.find(" "));
		
		if (command == "IDKFA")
		{
			std::cout << "THIS IS NOT DOOM!" << std::endl;
		}
		if (command == "QUIT")
		{
			CoreEngine::GetCoreEngine()->Stop();
		}
		if (command == "SAY")
		{
			std::cout << "Player says: " << sequence.substr(sequence.find(" ") + 1, std::string::npos) << std::endl;
		}
		if (command == "L")
		{
			std::string type = sequence.substr(sequence.find(" ") + 1, std::string::npos);

			if (type == "A")
			{
				m_engine->GetRenderingEngine()->SetShader("forward-ambient");
				m_engine->GetRenderingEngine()->SetFullBright(false);
			}
			if (type == "F")
			{
				m_engine->GetRenderingEngine()->SetShader("forward-fullbright");
				m_engine->GetRenderingEngine()->SetFullBright(true);
			}
		}

		m_keySequence.clear();
	}
};

#endif