#include "devMode.h"

void DevMode::PostRender (const Shader & shader, const RenderingEngine & renderingEngine, const Camera & camera) const
{
	if (m_isDev)
	{
		btVector3 color;

		switch (m_handleMode)
		{
		case HANDLE_SELECT:
			color = btVector3 (0, 1, 0);
			break;
		case HANDLE_TRANSFORM:
			color = btVector3 (1, 0, 0);
			break;
		case HANDLE_ROTATE:
			color = btVector3 (1, 1, 0);
			break;
		case HANDLE_SCALE:
			color = btVector3 (0, 0, 1);
			break;
		}

		for (auto x : m_selected)
		{
			m_engine->GetPhysicsEngine ()->GetWorld ()->debugDrawObject (x->GetBody ()->getWorldTransform (), x->GetBody ()->getCollisionShape (), color);
		}
	}

	/// Interface
	{



		ImGui::SetNextWindowPos (ImVec2 (10, 80), ImGuiSetCond_FirstUseEver);
		ImGui::Begin ("Stats", (bool*)&m_statsWnd, ImGuiWindowFlags_NoResize);
		{
			ImGui::Text ("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO ().Framerate, ImGui::GetIO ().Framerate);
		}
		ImGui::End ();

		if (ImGui::BeginMainMenuBar ())
		{
			if (ImGui::BeginMenu ("Game"))
			{
				if (ImGui::MenuItem ("Entity Manager", "CTRL+E"))
				{
					ImGui::SetNextWindowPos (ImVec2 (320, 240), ImGuiSetCond_FirstUseEver);
					ImGui::Begin ("Entity Manager", (bool*)&show_entity_manager);
					{

					}
					ImGui::End ();
				}

				ImGui::EndMenu ();
			}

			ImGui::EndMainMenuBar ();
		}

	}
}