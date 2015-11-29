/*
* Copyright (C) 2015 Dominik Madarasz
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
#include "../components/programComponent.h"
#include "../components/freeMove.h"
#include "../components/freeLook.h"
#include <vector>

COMPONENT(DevMode)
public:
	DevMode(const Window& window, bool isDev = true)
		:
		m_window(&window),
		m_isUpdating(!isDev),
		m_isDev(isDev),
		m_isCmd(false),
		m_showCursor(false),
		m_devCamera(Matrix4f().InitPerspective(
			ToRadians(70.0f), window.GetAspect(), 0.1f, 1000.0f), 0)
	{
		m_engine = CoreEngine::GetCoreEngine();
		m_move = new FreeMove(10.0f);
		m_look = new FreeLook(window.GetCenter());

		m_engine->SetSimulation(m_isUpdating);
		ProgramComponent::SetActive(m_isUpdating);

	}

	virtual ~DevMode()
	{
		delete m_look;
		delete m_move;
	}

	virtual void Init()
	{
		m_move->SetParent(GetParent());
		m_look->SetParent(GetParent());
		m_devCamera.SetTransform(GetTransform());
		m_lastCamera = m_engine->GetRenderingEngine()->GetMainCamera();

		if (m_isDev)
		{
			m_lastCamera = m_engine->GetRenderingEngine()->GetMainCamera();
			m_engine->GetRenderingEngine()->SetMainCamera(m_devCamera);
		}
		else
		{
			if (m_lastCamera != NULL)
				m_engine->GetRenderingEngine()->SetMainCamera(*m_lastCamera);
		}

		m_engine->SetSimulation(m_isUpdating);
		ProgramComponent::SetActive(m_isUpdating);
	}

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

				if (input.GetMouseDown(Input::MOUSE_LEFT_BUTTON))
				{
					Vector3f rayStart(
						((float)input.GetMousePosition().GetX() / (float)m_window->GetWidth() - 0.5f) * 2.0f,
						((float)input.GetMousePosition().GetY() / (float)m_window->GetHeight() - 0.5f) * 2.0f,
						-1.0f
						);

					Vector3f rayEnd(
						((float)input.GetMousePosition().GetX() / (float)m_window->GetWidth() - 0.5f) * 2.0f,
						((float)input.GetMousePosition().GetY() / (float)m_window->GetHeight() - 0.5f) * 2.0f,
						0.0f
						);

					Matrix4f M = m_devCamera.GetViewProjection().Inverse();

					Vector3f rayS = (Vector3f)M.Transform(rayStart);
					Vector3f rayE = (Vector3f)M.Transform(rayEnd);

					Vector3f rayDir(rayS - rayE);
					rayDir = rayDir.Normalized();

					Vector3f end = rayS + rayDir*1000.0f;

					btCollisionWorld::ClosestRayResultCallback RayCallback(
						rayS.GetBT(),
						rayDir.GetBT()
						);

					CoreEngine::GetCoreEngine()->GetPhysicsEngine()->GetWorld()->rayTest(
						rayS.GetBT(),
						rayDir.GetBT(),
						RayCallback
						);

					if (RayCallback.hasHit())
					{
						Entity* hit = (Entity*)RayCallback.m_collisionObject->getUserPointer();

						if (hit)
						{
							std::cout << "Selected: " << hit->GetDisplayName() << "(" << hit << ")" << std::endl;
						}
					}
				}
			}
			
		}
		
		if (input.GetKeyDown(Input::KEY_F4))
		{
			m_isDev = !m_isDev;
			m_isUpdating = m_engine->GetSimulation();
			

			if (m_isDev)
			{
				std::cout << "Dev Mode" << std::endl;
				input.SetCursor(true);
				m_lastCamera = m_engine->GetRenderingEngine()->GetMainCamera();
				m_engine->GetRenderingEngine()->SetMainCamera(m_devCamera);

				m_engine->SetSimulation(false);
				ProgramComponent::SetActive(false);
			}
			else
			{
				std::cout << "Play Mode" << std::endl;
				input.SetCursor(!m_showCursor);
				m_engine->GetRenderingEngine()->SetMainCamera(*m_lastCamera);

				m_engine->SetSimulation(true);
				ProgramComponent::SetActive(true);
			}

			
			m_showCursor = input.GetCursor();
		}

		if (input.GetKeyDown(Input::KEY_F3))
		{
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

	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
	{
		
	}

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