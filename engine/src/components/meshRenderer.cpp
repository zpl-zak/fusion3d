/*
 * Copyright (C) 2015-2016 Dominik "ZaKlaus" Madarasz
 * Copyright (C) 2014 Benny Bobaganoosh
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

#include "meshRenderer.h"

MeshRenderer::MeshRenderer() {}

MeshRenderer::MeshRenderer(std::vector<Mesh*> meshes, std::vector<Material*> materials)
{
	LoadData(meshes, materials);
}

MeshRenderer::MeshRenderer(const std::string & fileName) :
	MeshRenderer(Mesh::ImportMesh(fileName), Mesh::ImportMeshMaterial(fileName))
{}

MeshRenderer::MeshRenderer(Mesh mesh, const Material & material)
{
	m_mesh.push_back(mesh);
	m_material.push_back(material);
}

void MeshRenderer::Init()
{
	m_visible = true;
	m_shadows = true;

	for (size_t i = 0; i < m_material.size(); i++)
	{
		m_matNames.push_back(m_material[i].m_materialName.c_str());
	}
}

void MeshRenderer::Render(Shader & shader, const RenderingEngine & renderingEngine, const Camera & camera)
{
	if (m_mesh.size() == 0)return;

	if (shader.GetName() == "shadowMapGenerator" && !m_shadows) return;
	if (!m_visible) return;

	auto material = &m_material.at(m_mesh.at(0).GetMeshData()->GetMaterialIndex());
	auto _shader = (shader.GetName() == "forward-ambient") ? material->GetShader() : &shader;
	if (!_shader) _shader = &shader;

	_shader->Bind();
	_shader->UpdateUniforms(*GetTransform(), *material, renderingEngine, camera);
	//auto b = camera.GetViewProjection();
	for (size_t i = 0; i < m_mesh.size(); i++)
	{
		// DISTANCE CHECK
		{
			const IndexedModel& t = m_mesh.at(i).GetMeshData()->GetModel();
			//constexpr float DIST =   1.2f;

			//float dz = (t.GetCenter().GetZ()) - (camera.GetTransform().GetPos().GetZ()); dz = fabsf(dz);
			//float dy = (t.GetCenter().GetY()) - (camera.GetTransform().GetPos().GetY()); dy = fabsf(dy);
			//float dx = (t.GetCenter().GetX()) - (camera.GetTransform().GetPos().GetX()); dx = fabsf(dx);
			//float z = sqrtf((dy*dy) + (dx*dx) + (dz*dz));

			//float ad = t.GetBoundSize();// / logf(t.GetBoundSize()); // ??????????????????????????????????? //logf(t.GetBoundSize() + 1.0f) * DIST + 200;// +50;
			//if (z >= ad)
			//	continue;

			/*Vector3f c = b.Transform(t.GetCenter());
			float r = t.GetBoundSize() / 2;

			if (camera.GetFrustum().SphereInFrustum(c, r) == Frustum::OUTSIDE)
			continue;*/
		}

		if (shader.GetName() != "shadowMapGenerator")
		{
			auto* material = &m_material.at(m_mesh.at(i).GetMeshData()->GetMaterialIndex());

			//IMPORTANT(zaklaus): Make sure your shader uses following order of texture maps!!

			/*material.GetTexture("diffuse").Bind(0);
			material.GetTexture("normalMap").Bind(1);
			material.GetTexture("dispMap").Bind(2);*/

			material->GetData()->Diffuse.Bind(0);
			material->GetData()->Normal.Bind(1);
			material->GetData()->Disp.Bind(2);

			//_shader->SetUniformi("diffuse", 0);
		}
		m_mesh.at(i).Draw();
	}
}

void MeshRenderer::DataDeploy(tinyxml2::XMLElement * data)
{
	std::vector<Mesh*> meshes = Mesh::ImportMesh(data->GetText());
	std::vector<Material*> materials = Mesh::ImportMeshMaterial(data->GetText());
	LoadData(meshes, materials);
}

void MeshRenderer::DrawDebugUI()
{
	//INFO
	{
		ImGui::Text("Visible: %d", m_visible);
		ImGui::Text("Cast Shadows: %d", m_shadows);
		ImGui::Text("Number of meshes: %d", m_mesh.size());
		ImGui::Text("Number of linked materials: %d", m_material.size());
	}

	//CTRL
	{
		ImGui::Checkbox("Visible", &m_visible);
		ImGui::Checkbox("Cast Shadows", &m_shadows);
	}

	static char meshName[256] = { 0 };
	ImGui::InputText("Mesh Name", meshName, 256);
	if (ImGui::Button("Load Mesh"))
	{
		std::vector<Mesh*> meshes = Mesh::ImportMesh(meshName);
		std::vector<Material*> materials = Mesh::ImportMeshMaterial(meshName);
		LoadData(meshes, materials);

		m_matNames.clear();
		for (size_t i = 0; i < m_material.size(); i++)
		{
			m_matNames.push_back(m_material[i].m_materialName.c_str());
		}
	}

	ImGui::Separator();

	//Material Editor
	if (m_matNames.size() > 0)
	{
		static int selected_item = 0;

		static int selected_mesh = 0;
		ImGui::InputInt("Mesh", &selected_mesh);

		Clamp<int>(selected_mesh, 0, m_mesh.size() - 1);

		std::vector<Material*> mats;
		std::vector<const char*> matNames;

		mats.clear();
		matNames.clear();
		for (size_t i = 0; i < m_mesh[selected_mesh].GetMeshData()->m_materialArray.size(); i++)
		{
			auto x = m_mesh[selected_mesh].GetMeshData()->m_materialArray;
			mats.push_back(&m_material[x[i]]);
			matNames.push_back(mats[i]->m_materialName.c_str());
		}

		//ImGui::ListBoxHeader("Material Editor");
		//for (size_t i = 0; i < m_material.size(); i++)
		{
			ImGui::ListBox("Materials", &selected_item, &matNames[0], matNames.size());
		}
		//ImGui::ListBoxFooter();

		static bool new_material_dialog = false;

		if (new_material_dialog)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("New Material", &new_material_dialog, ImGuiWindowFlags_AlwaysAutoResize);
			{
				static char name[256] = { 0 };
				static char diffuse[256] = { 0 };
				static char normal[256] = { 0 };
				static char disp[256] = { 0 };
				static float dispScale, dispOffset = 0.0f;
				static float specularIntensity, specularPower = 0.0f;

				ImGui::InputText("Material Name", name, 256);

				ImGui::Text("Texture Maps");
				ImGui::InputText("Diffuse", diffuse, 256);
				ImGui::InputText("Normal", normal, 256);
				ImGui::InputText("Displacement", disp, 256);

				ImGui::Text("Material Settings");
				ImGui::DragFloat("Specular Intensity", &specularIntensity, 0.1f);
				ImGui::DragFloat("Specular Power", &specularPower, 0.1f);
				ImGui::DragFloat("Displacement Scale", &dispScale, 0.1f);
				ImGui::DragFloat("Displacement Offset", &dispOffset, 0.1f);

				if (ImGui::Button("Cancel"))
				{
					new_material_dialog = false;
				}

				if (ImGui::Button("Create"))
				{
					auto x = Material(name, Texture(diffuse), specularIntensity, specularPower, Texture(normal), Texture(disp), dispScale, dispOffset);
					//						m_material.erase(m_material.begin() + selected_item);
					//						m_material.insert(m_material.begin() + selected_item, x);
					m_material.push_back(x);

					m_mesh[selected_mesh].GetMeshData()->m_materialArray.push_back(m_material.size() - 1);

					//m_matNames[selected_item] = m_material[selected_item].m_materialName.c_str();
					new_material_dialog = false;
				}
				ImGui::End();
			}
		}

		if (ImGui::Button("Create Material"))
		{
			new_material_dialog = true;
		}

		if (ImGui::Button("Assign Material"))
		{
			m_mesh[selected_mesh].GetMeshData()->SetMaterialIndex(selected_item);
		}

		ImGui::Separator();

		m_material[selected_item].DrawDebugUI();
	}
}

EntityComponent * MeshRenderer::SetVisible(bool visible)
{
	m_visible = visible;

	return this;
}

bool MeshRenderer::IsVisible()
{
	return m_visible;
}

void MeshRenderer::LoadData(std::vector<Mesh*> meshes, std::vector<Material*> materials)
{
	m_mesh.clear();
	m_material.clear();

	for (size_t i = 0; i < meshes.size(); i++)
	{
		m_mesh.push_back(*(meshes.at(i)));
	}

	for (size_t i = 0; i < materials.size(); i++)
	{
		m_material.push_back(*(materials.at(i)));
	}
}

EntityComponent * MeshRenderer::SetShadows(bool shadows)
{
	m_shadows = shadows;

	return this;
}

bool MeshRenderer::HasShadows()
{
	return m_shadows;
}

Material & MeshRenderer::GetMaterial(int ID) { return m_material.at(ID); }

Mesh & MeshRenderer::GetMesh(int ID) { return m_mesh.at(ID); }

MeshSequence::MeshSequence()
	: m_step(-1),
	m_play(true)
{}

MeshSequence::MeshSequence(const std::string & fileName, int stepSize)
	:
	m_step(-1),
	m_play(true)
{
	LoadData(fileName, stepSize);
}

void MeshSequence::Update(float delta)
{
	if (!m_play) return;

	m_step++;

	if (m_step >= m_meshes.size())
	{
		m_step = 0;
	}
}

void MeshSequence::Render(const Transform & transform, Shader & shader, const RenderingEngine & renderingEngine, const Camera & camera)
{
	if (shader.GetName() == "shadowMapGenerator" && !m_shadows) return;
	if (!m_visible) return;
	int step = (m_step < 0) ? 0 : m_step;

	shader.Bind();
	for (size_t j = 0; j < m_meshes.at(step).size(); j++)
	{
		shader.UpdateUniforms(transform, m_materials.at(m_meshes.at(step).at(j).GetMeshData()->GetMaterialIndex()), renderingEngine, camera);
		m_meshes.at(step).at(j).Draw();
	}
}

void MeshSequence::DataDeploy(tinyxml2::XMLElement * data)
{
	int stepSize = 1;
	if (data->Attribute("step"))
	{
		stepSize = atoi(data->Attribute("step"));
	}
	LoadData(data->GetText(), stepSize);
}

EntityComponent * MeshSequence::SetVisible(bool visible)
{
	m_visible = visible;

	return this;
}

bool MeshSequence::IsVisible()
{
	return m_visible;
}

void MeshSequence::LoadData(std::string filename, size_t stepSize)
{
	m_step = -1;

	std::string filelist = Util::ExecuteTask("listdir", (Util::ResourcePath() + "models/" + filename + " .obj"));
	std::vector<std::string> files = Util::Split(filelist, '@');
	std::vector<std::vector<Mesh*>> meshes_old;
	std::vector<Material*> materials;

	for (size_t i = 0; i < files.size(); i++)
	{
		meshes_old.push_back(Mesh::ImportMesh(filename + "/" + files[i]));
	}
	materials = Mesh::ImportMeshMaterial(filename + "/" + files[0]);

	for (auto x : materials)
	{
		m_materials.push_back(*x);
	}


	std::vector<std::vector<Mesh>> meshg;
	for (size_t i = 0; i < meshes_old.size(); i++)
	{
		if (i + 1 == meshes_old.size())break;

		for (size_t l = 0; l < stepSize; l++)
		{
			std::vector<Mesh> mesh;
			for (size_t j = 0; j < meshes_old.at(i).size(); j++)
			{
				MeshData* meshData;
				std::map<std::string, MeshData*>::const_iterator it = Mesh::s_resourceMap.find(filename + (char)(j + i + l + 40));
				if (it != Mesh::s_resourceMap.end())
				{
					meshData = it->second;
					meshData->AddReference();
				}
				else
				{
					std::vector<Vector3f> pos;
					std::vector<Vector3f> nor;
					std::vector<Vector3f> tan;

					for (size_t k = 0; k < meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetPositions().size(); k++)
					{
						pos.push_back((Vector3f)(meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetPositions().at(k)).Lerp(meshes_old.at(i + 1).at(j)->GetMeshData()->GetModel().GetPositions().at(k), (float)l / (float)stepSize));
						nor.push_back((Vector3f)(meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetNormals().at(k)).Lerp(meshes_old.at(i + 1).at(j)->GetMeshData()->GetModel().GetNormals().at(k), (float)l / (float)stepSize));
						tan.push_back((Vector3f)(meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetTangents().at(k)).Lerp(meshes_old.at(i + 1).at(j)->GetMeshData()->GetModel().GetTangents().at(k), (float)l / (float)stepSize));
					}

					meshData = new MeshData(IndexedModel(
						meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetIndices(),
						pos,
						meshes_old.at(i).at(j)->GetMeshData()->GetModel().GetTexCoords(),
						nor,
						tan
					), meshes_old.at(i).at(j)->GetMeshData()->GetMaterialIndex());
					Mesh::s_resourceMap.insert(std::pair<std::string, MeshData*>(filename + (char)(j + i + l + 40), meshData));
				}
				mesh.push_back(Mesh(filename + (char)(j + i + l + 40), meshData));
			}
			meshg.push_back(mesh);
		}

	}
	m_meshes = meshg;

	for (auto mesh : meshes_old)
	{
		for (auto x : mesh)
		{
			delete x;
		}
	}
}

EntityComponent * MeshSequence::SetShadows(bool shadows)
{
	m_shadows = shadows;

	return this;

}

bool MeshSequence::HasShadows()
{
	return m_shadows;
}

std::string MeshSequence::GetName()
{
	return m_name;
}

void MeshSequence::SetStep(size_t step)
{
	m_step = step;
}

void MeshSequence::SetPlay(bool play)
{
	m_play = play;
}

AnimMeshRenderer::AnimMeshRenderer()
	:
	m_anim(0)
{}

AnimMeshRenderer::AnimMeshRenderer(const std::string & fileName, int stepSize)
	:
	m_anim(0)
{
	LoadData(fileName, stepSize);
}

AnimMeshRenderer::~AnimMeshRenderer()
{
	for (auto x : m_anims)
	{
		delete x;
	}
}

void AnimMeshRenderer::Update(float delta)
{
	if (m_anims.size() == 0)return;
	m_anims.at(m_anim)->Update(delta);
}

void AnimMeshRenderer::Render(Shader & shader, const RenderingEngine & renderingEngine, const Camera & camera) const
{
	if (m_anims.size() == 0)return;
	m_anims.at(m_anim)->Render(GetTransform(), shader, renderingEngine, camera);
}

void AnimMeshRenderer::DataDeploy(tinyxml2::XMLElement * data)
{
	int stepSize = 1;
	if (data->Attribute("step"))
	{
		stepSize = atoi(data->Attribute("step"));
	}
	LoadData(data->GetText(), stepSize);
}

void AnimMeshRenderer::LoadData(std::string filename, int stepSize)
{
	std::string filelist = Util::ExecuteTask("listdir_dir", (Util::ResourcePath() + "models/" + filename));
	std::vector<std::string> files = Util::Split(filelist, '@');

	for (auto x : files)
	{
		m_anims.push_back(new MeshSequence(filename + "/" + x, stepSize));
	}

	for (size_t i = 0; i < files.size(); i++)
	{
		m_anims.push_back(new MeshSequence(filename + "/" + files[i], stepSize));
	}
}
