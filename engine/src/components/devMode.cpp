/*
 * Copyright (C) 2015-2016 Dominik "ZaKlaus" Madarasz
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

#include "devMode.h"

bool general_settings = false;
Entity* general_settings_uid = 0;
DevMode* root_entity = 0;
std::vector<const char*> factory_class_names;

bool create_entity_window = false;
bool create_component_window = false;

bool component_settings = false;
EntityComponent* component_settings_uid = 0;

bool global_axis = true;

bool render_settings = false;

void CreateEntity()
{
	static char name[256] = {0};
	//Dialog
	{
		ImGui::InputText("Name", name, 256);
	}

	if (ImGui::Button("Cancel"))
	{
	clear:
		ZeroMemory(name, 256);
		create_entity_window = false;
	}

	if (ImGui::Button("Create"))
	{
		auto e = new Entity();
		e->SetDisplayName(name);
		e->SetTransform(*root_entity->GetTransform());

		root_entity->GetParent()->GetScene()->AddChild(e);
		general_settings = true;
		general_settings_uid = e;

		goto clear;
	}
}

void CreateComponent()
{
	static int selected_item = 0;
	//ImGui::ListBoxHeader("Components");
	{
		//for (size_t i = 0; i < factory_class_names.size(); i++)
		{
			ImGui::ListBox("Components", &selected_item, &factory_class_names[0], factory_class_names.size());
		}
	}
	//ImGui::ListBoxFooter();

	if (ImGui::Button("Create Component"))
	{
		auto x = (EntityComponent*)g_factory.construct(factory_class_names[selected_item]);
		general_settings_uid->AddComponent(x);
		selected_item = 0;
		create_component_window = false;

		component_settings = true;
		component_settings_uid = x;
	}
}

void ShowCompProps(EntityComponent* uid)
{
	ImGui::Text("Component: %s", uid->__ClassType__().c_str());

	ImGui::Separator();

	uid->DrawDebugUI();
}

void ShowObjectProps(Entity* uid)
{
	if (!uid)
		return;

	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	{
		ImGui::PushItemWidth(-1);
		{
			if (ImGui::TreeNode("Transform"))
			{
				ImGui::Columns(3);
				auto t = uid->GetTransform();

				Vector3f* p = t->GetPos();
				float x = p->GetX();
				float y = p->GetY();
				float z = p->GetZ();

				ImGui::DragFloat("X ##xp", &x, 0.1f);
				ImGui::DragFloat("Y ##yp", &y, 0.1f);
				ImGui::DragFloat("Z ##zp", &z, 0.1f);
				t->SetPos(Vector3f(x, y, z));

				ImGui::NextColumn();

				auto p2 = t->GetEulerAngles();

				float roll = ToDegrees(p2[0]);
				float pitch = ToDegrees(p2[1]);
				float yaw = ToDegrees(p2[2]);

				float old_roll = roll;
				float old_pitch = pitch;
				float old_yaw = yaw;

				ImGui::SliderFloat("Roll ##xr", &roll, -180, 180);
				ImGui::SliderFloat("Pitch ##yr", &pitch, -180, 180);
				ImGui::SliderFloat("Yaw ##zr", &yaw, -180, 180);
				//ImGui::SliderFloat("W ##wr", &w2, 0, 360);

				float ex = ToRadians(roll);
				float ey = ToRadians(pitch);
				float ez = ToRadians(yaw);

				t->SetEulerAngles(Vector3f(ex, ey, ez));

				ImGui::NextColumn();

				auto p3 = t->GetScale();
				float x3 = p3;

				ImGui::DragFloat("XYZ ##xs", &x3);

				t->SetScale(x3);

				ImGui::TreePop();
				ImGui::Columns(1);
			}
		}
		ImGui::PopItemWidth();
	}
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	ImGui::Separator();
	{
		if (ImGui::Button("Go To"))
		{
			Vector3f dir = uid->GetTransform()->GetPos() - root_entity->GetTransform()->GetPos();

			Vector3f newPos = *uid->GetTransform()->GetPos() + (dir.Normalized() * -5.0f);

			//root_entity->GetTransform()->LookAt((*uid->GetTransform()->GetPos()), Vector3f(0, 1, 0));
			root_entity->GetTransform()->SetPos(newPos);
		}

		if (ImGui::Button("Teleport To"))
		{
			//root_entity->GetTransform()->LookAt((*uid->GetTransform()->GetPos()), Vector3f(0, 1, 0));
			uid->GetTransform()->SetPos(*root_entity->GetTransform()->GetPos());
		}
	}
	ImGui::Separator();

	if (ImGui::TreeNode("Components"))
	{
		for (size_t i = 0; i < uid->GetAllComponents().size(); i++)
		{
			if (uid->GetAllComponents()[i] == nullptr)
				continue;

			ImGui::AlignFirstTextHeightToWidgets();
			// Here we use a Selectable (instead of Text) to highlight on hover
			//ImGui::Text("Field_%d", i);
			char label[64];
			sprintf_s(label, 64, "Component %s", uid->GetAllComponents()[i]->__ClassType__().c_str());
			ImGui::Bullet();
			if (ImGui::Selectable(label))
			{
				component_settings_uid = uid->GetAllComponents()[i];
				component_settings = true;
			}
			//ImGui::NextColumn();
			/*ImGui::PushItemWidth(-1);
			ImGui::PopItemWidth();*/
		}
		ImGui::TreePop();
	}

	if (ImGui::Button("Create Component"))
	{
		create_component_window = true;
	}
}

void ShowObject(const char* prefix, Entity* uid)
{
	if (uid == nullptr)
		return;

	ImGui::PushID(uid); // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
	//ImGui::AlignFirstTextHeightToWidgets();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
	bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);

	if (node_open)
	{
		if (ImGui::Selectable("General", &general_settings))
		{
			general_settings = true;
			general_settings_uid = uid;
		}
		for (size_t i = 0; i < uid->GetAllEntities().size(); i++)
		{
			ImGui::PushID(i); // Use field index as identifier.
			ShowObject(uid->GetAllEntities()[i]->GetDisplayName().c_str(), uid->GetAllEntities()[i]);
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

DevMode::DevMode(const Window& window, bool isDev)
	:
	m_window(&window),
	m_isUpdating(!isDev),
	m_isDev(isDev),
	m_isCmd(false),
	m_showCursor(false),
	m_devCamera(Matrix4f().InitPerspective(
		            ToRadians(70.0f), window.GetAspect(), 0.1f, 1000.0f), 0)

	, enable_physics(false)
{
	m_engine = CoreEngine::GetCoreEngine();
	m_move = new FreeMove(10.0f);
	m_look = new FreeLook(window.GetCenter());

	m_engine->GetPhysicsEngine()->SetSimulation(false);
	ProgramHoster::SetActive(m_isUpdating);
	m_gizmoSphere = (new Entity())->AddComponent(new MeshRenderer("arrows.obj"));
}

DevMode::~DevMode()
{
	delete m_look;
	delete m_move;
}

void DevMode::Init()
{
	m_move->SetParent(GetParent());
	m_look->SetParent(GetParent());
	m_devCamera.SetTransform(GetTransform());
	m_lastCamera = m_engine->GetRenderingEngine()->GetMainCamera();
	GetParent()->AddComponent(new PointLight(Vector3f(0.1f, 0.1f, 0.1f), 55.4f, Attenuation(6.5f, 7.7f, 23.2f), 42.f, 6, 1, 0.193f));

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

	m_engine->GetPhysicsEngine()->SetSimulation(false);
	ProgramHoster::SetActive(m_isUpdating);


	for (std::map<std::string, constructor_t>::iterator it = g_factory.m_classes.begin(); it != g_factory.m_classes.end(); ++it)
	{
		factory_class_names.push_back(it->first.c_str());
	}
}

void DevMode::ProcessInput(const Input& input, float delta)
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
					}
					else
						std::cout << (char)(input.GetLastKey() + 61);
					m_keySequence.push_back(input.GetLastKey());
				}

				if (input.GetKeyDown(Input::KEY_RETURN))
				{
					std::cout << " OK!" << std::endl;
					EvalKeySequence();
				}
			}

			if (input.GetMouseDown(Input::MOUSE_LEFT_BUTTON) && m_handleMode == HANDLE_SELECT)
			{
				Vector3f RayOrigin, RayDir;

				Util::ScreenToWorld(input.GetMousePosition().GetX(), input.GetMousePosition().GetY(), m_window->GetWidth(), m_window->GetHeight(), m_devCamera.GetViewProjectionWithoutTranslation(), RayOrigin, RayDir);
				RayOrigin = m_devCamera.GetTransform()->GetTransformedPos();

				Vector3f end = RayOrigin + RayDir * 1000.0f;

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
					Entity* hit = hitBody->GetParent();

					if (hit)
					{
						//std::cout << "Selected: " << hit->GetDisplayName() << "(" << hit << ")" << std::endl;
						//std::cout << hit->GetTransform ()->GetPos ()->GetX () << ", " << hit->GetTransform ()->GetPos ()->GetY () << ", " << hit->GetTransform ()->GetPos ()->GetZ () << std::endl;

						bool exist = false;
						for (size_t i = 0; i < m_selected.size(); i++)
						{
							if (m_selected.at(i) == hitBody)
							{
								m_selected.erase(m_selected.begin() + i);
								exist = true;
							}
						}

						if (input.GetKey(Input::KEY_LSHIFT) || input.GetKey(Input::KEY_RSHIFT))
						{
							if (!exist)
							{
								m_selected.push_back(hitBody);
							}
						}
						else
						{
							m_selected.clear();
							if (!exist)
							{
								m_selected.push_back(hitBody);
								general_settings = true;
								general_settings_uid = hitBody->GetParent();
							}
							else
							{
								general_settings = false;
								general_settings_uid = nullptr;
							}
						}

						//std::cout << "Selected Count: " << m_selected.size () << std::endl;
					}
				}
			}
			if (input.GetKey(Input::KEY_J) && m_handleMode == HANDLE_TRANSFORM)
			{
				for (size_t i = 0; i < m_selected.size(); i++)
				{
					auto selected = m_selected.at(i);
					auto pos = selected->GetTransform()->GetPos();

					if (global_axis)
					{
						selected->GetTransform()->SetPos(*pos + Vector3f(-1, 0, 0));
					}
					else
					{
						auto left = selected->GetTransform()->GetRot()->GetLeft();
						selected->GetTransform()->SetPos(*pos + left);
					}
				}
			}
			if (input.GetKey(Input::KEY_I) && m_handleMode == HANDLE_TRANSFORM)
			{
				for (size_t i = 0; i < m_selected.size(); i++)
				{
					auto selected = m_selected.at(i);
					auto pos = selected->GetTransform()->GetPos();

					if (global_axis)
					{
						selected->GetTransform()->SetPos(*pos + Vector3f(0, 0, 1));
					}
					else
					{
						auto forward = selected->GetTransform()->GetRot()->GetForward();
						selected->GetTransform()->SetPos(*pos + forward);
					}
				}
			}
			if (input.GetKey(Input::KEY_K) && m_handleMode == HANDLE_TRANSFORM)
			{
				for (size_t i = 0; i < m_selected.size(); i++)
				{
					auto selected = m_selected.at(i);
					auto pos = selected->GetTransform()->GetPos();

					if (global_axis)
					{
						selected->GetTransform()->SetPos(*pos + Vector3f(0, 0, -1));
					}
					else
					{
						auto backward = selected->GetTransform()->GetRot()->GetBack();
						selected->GetTransform()->SetPos(*pos + backward);
					}
				}
			}
			if (input.GetKey(Input::KEY_L) && m_handleMode == HANDLE_TRANSFORM)
			{
				for (size_t i = 0; i < m_selected.size(); i++)
				{
					auto selected = m_selected.at(i);
					auto pos = selected->GetTransform()->GetPos();

					if (global_axis)
					{
						selected->GetTransform()->SetPos(*pos + Vector3f(1, 0, 0));
					}
					else
					{
						auto right = selected->GetTransform()->GetRot()->GetRight();
						selected->GetTransform()->SetPos(*pos + right);
					}
				}
			}


			if (input.GetKeyDown(Input::KEY_Q))
			{
				m_handleMode = HANDLE_SELECT;
				std::cout << "Selection Mode" << std::endl;
			}
			if (input.GetKeyDown(Input::KEY_W))
			{
				m_handleMode = HANDLE_TRANSFORM;
				std::cout << "Transform Mode" << std::endl;
			}
			if (input.GetKeyDown(Input::KEY_E))
			{
				m_handleMode = HANDLE_ROTATE;
				std::cout << "Rotate Mode" << std::endl;
			}
			if (input.GetKeyDown(Input::KEY_R))
			{
				m_handleMode = HANDLE_SCALE;
				std::cout << "Scale Mode" << std::endl;
			}
			if (input.GetKeyDown(Input::KEY_G))
			{
				global_axis = !global_axis;
			}
		}
	}

	if (input.GetKeyDown(Input::KEY_F4))
	{
		m_isDev = !m_isDev;
		m_isUpdating = m_engine->GetPhysicsEngine()->GetSimulation();


		if (m_isDev)
		{
			std::cout << "Dev Mode" << std::endl;
			input.SetCursor(true);
			m_lastCamera = m_engine->GetRenderingEngine()->GetMainCamera();
			m_engine->GetRenderingEngine()->SetMainCamera(m_devCamera);

			m_engine->GetPhysicsEngine()->SetSimulation(false);
			ProgramHoster::SetActive(false);
		}
		else
		{
			std::cout << "Play Mode" << std::endl;
			input.SetCursor(!m_showCursor);
			m_engine->GetRenderingEngine()->SetMainCamera(*m_lastCamera);

			m_engine->GetPhysicsEngine()->SetSimulation(true);
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

void DevMode::Update(float delta)
{
	if (m_isDev)
	{
	}
}

void DevMode::PostRender(const Shader& shader, RenderingEngine& renderingEngine, const Camera& camera)
{
	root_entity = this;
	if (m_isDev)
	{
		btVector3 color;

		switch (m_handleMode)
		{
		case HANDLE_SELECT:
			color = btVector3(0, 1, 0);
			break;
		case HANDLE_TRANSFORM:
			color = btVector3(1, 0, 0);
			break;
		case HANDLE_ROTATE:
			color = btVector3(1, 1, 0);
			break;
		case HANDLE_SCALE:
			color = btVector3(0, 0, 1);
			break;
		}

		for (auto x : m_selected)
		{
			m_engine->GetPhysicsEngine()->GetWorld()->debugDrawObject(x->GetBody()->getWorldTransform(), x->GetBody()->getCollisionShape(), color);
		}
	}

	/// Interface
	{
		if (general_settings)
		{
			ImGui::SetNextWindowPos(ImVec2(320, 240), ImGuiSetCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Property Manager", &general_settings, ImGuiWindowFlags_AlwaysAutoResize);
			{
				ShowObjectProps(general_settings_uid);
				ImGui::End();
			}
		}

		if (create_entity_window)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("Create Entity", &create_entity_window, ImGuiWindowFlags_AlwaysAutoResize);
			{
				CreateEntity();
				ImGui::End();
			}
		}

		if (create_component_window)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("Create Component", &create_component_window, ImGuiWindowFlags_AlwaysAutoResize);
			{
				CreateComponent();
				ImGui::End();
			}
		}

		if (component_settings)
		{
			ImGui::Begin("Component Editor", &component_settings, ImGuiWindowFlags_AlwaysAutoResize);
			{
				ShowCompProps(component_settings_uid);
				ImGui::End();
			}
		}

		ImGui::SetNextWindowPos(ImVec2(320, 240), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Hierarchy Window", &show_entity_manager);
		{
			//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			//ImGui::Columns(2);
			ImGui::Separator();
			{
				for (auto x : GetParent()->GetScene()->GetAllEntities())
				{
					ShowObject(x->GetDisplayName().c_str(), x);
				}
			}
			ImGui::Columns(1);
			ImGui::Separator();

			if (ImGui::Button("Create Entity"))
			{
				create_entity_window = true;
			}

			//ImGui::PopStyleVar();
			ImGui::End();
		}

		ImGui::SetNextWindowPos(ImVec2(10, 80), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Stats", (bool*)&m_statsWnd, ImGuiWindowFlags_NoResize);
		{
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Axis mode: %s", (global_axis) ? "Global" : "Local");
			ImGui::End();
		}

		ImGui::SetNextWindowPos(ImVec2(0, 120), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Render Settings", (bool*)&render_settings, ImGuiWindowFlags_NoResize);
		{
			ImGui::Text("Ambient color: ");
			static const Vector3f colvec = renderingEngine.GetVector3f("ambient");
			static float col[3] = { colvec[0], colvec[1], colvec[2] };
			ImGui::ColorEdit3("Ambient Color", col);

			if (ImGui::Button("Update"))
			{
				const Vector3f newcol = Vector3f(col[0], col[1], col[2]);
				renderingEngine.SetVector3f("ambient", newcol);
			}
			
			ImGui::End();
		}


		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Game"))
			{
				if (ImGui::MenuItem("Entity Manager", "CTRL+E"))
				{
					show_entity_manager = !show_entity_manager;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("States"))
			{
				if (ImGui::MenuItem("Enable Physics", "CTRL+P", enable_physics))
				{
					enable_physics = !enable_physics;
					m_engine->GetPhysicsEngine()->SetSimulation(enable_physics);
				}

				if (ImGui::MenuItem("Render Settings", "CTRL+R"))
				{
					render_settings = !render_settings;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	///GIZMO
	if (general_settings_uid && m_isDev)
	{
		auto ot = general_settings_uid->GetTransform();
		Shader s = shader;
		//m_gizmoSphere->GetTransform()->SetPos(*ot->GetPos());
		m_gizmoSphere->SetTransform(*ot);

		if (global_axis)
			m_gizmoSphere->GetTransform()->SetEulerAngles(Vector3f());

		m_gizmoSphere->RenderAll(s, renderingEngine, camera);

		Vector3f pos = ot->GetTransformedPos();

		pos = (Vector3f)camera.GetViewProjection().Transform(pos);
		pos.SetX(pos.GetX() / pos.GetZ());
		pos.SetY(pos.GetY() / pos.GetZ());

		pos.SetX(renderingEngine.GetWindow()->GetWidth() * ((pos.GetX() + 1.0f) / 2.0f) + 0.05f);
		pos.SetY(renderingEngine.GetWindow()->GetHeight() * (1.0f - (pos.GetY() + 1.0f) / 2.0f) + 0.05f);

		if (pos.GetX() > renderingEngine.GetWindow()->GetWidth() ||
			pos.GetX() < 0 ||
			pos.GetY() > renderingEngine.GetWindow()->GetHeight() ||
			pos.GetY() < 0)
		{
			return;
		}

		/*ImGui::SetNextWindowPos(ImVec2(pos.GetX(), pos.GetY()));
		ImGui::Begin(general_settings_uid->GetDisplayName().c_str());
		{
		ImGui::End();
		}*/
	}
}

void DevMode::EvalKeySequence()
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
