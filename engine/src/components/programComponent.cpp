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

#include "programComponent.h"

double ProgramHoster::lastTime = 0;
double ProgramHoster::timeCounter = 0;
bool ProgramHoster::isUpdating = true;

ProgramHoster::ProgramHoster(): m_input(0), m_update(0), m_render(0), m_verbose(0), m_program(nullptr), m_delayedInit(false), m_loaded(false), m_isUpdating(false)
{
}

ProgramHoster::ProgramHoster(Program* program, int verbose, bool start)
	:
	m_delayedInit((!isUpdating)),
	m_loaded(false),
	m_isUpdating(start)
{
	m_program = program;
	m_verbose = verbose;
	m_loaded = false;
}

ProgramHoster::~ProgramHoster()
{
	delete m_program;
}

void ProgramHoster::DrawDebugUI()
{
	static char name[256] = {0};
	static char params[256] = {0};

	ImGui::InputText("Program", name, 256);
	ImGui::InputText("Parameters", params, 256);

	if (ImGui::Button("Load Program"))
	{
		m_program = (Program*)g_programs.construct(name);

		Init();
	}

	ImGui::Separator();
	ImGui::Checkbox("Running", &m_isUpdating);

	ImGui::Separator();

	if(m_program)
		m_program->DrawDebugUI();
}

void ProgramHoster::Init()
{
	if (!m_program)return;
	m_program->SetParent(GetParent());

	if (!m_program->Init())
	{
		std::cerr << "Init() failed for Program Name: " << m_program->GetProgramName() << std::endl;
	}
}

void ProgramHoster::InitProgram()
{
	if (m_loaded)
		return;

	m_loaded = true;

	if (!m_program->Init())
	{
		std::cerr << "Init() failed for Program Name: " << m_program->GetProgramName() << std::endl;
	}
}

void ProgramHoster::DataDeploy(tinyxml2::XMLElement* data)
{
	m_program = (Program*)g_factory.construct(data->GetText());
}

void ProgramHoster::ProcessInput(const Input& input, float delta)
{
	if (!m_program)return;

	if (!isUpdating && (!m_isUpdating))
		return;

	m_inputTimer.StartInvocation();

	m_input = m_program->ProcessInput(input, delta);

	m_inputTimer.StopInvocation();
}

void ProgramHoster::Update(float delta)
{
	if (!m_program)return;

	if (!isUpdating && (!m_isUpdating))
		return;

	//		InitProgram();

	m_updateTimer.StartInvocation();

	m_update = m_program->Update(delta);

	m_updateTimer.StopInvocation();
}

void ProgramHoster::Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
{
	if (!m_program)return;

	if (!isUpdating && (!m_isUpdating))
		return;

	m_renderTimer.StartInvocation();

	m_render = m_program->Render(shader, renderingEngine, camera);

	m_renderTimer.StopInvocation();

	ProgramHoster::Display(this);
}

void ProgramHoster::SetActive(bool state)
{
	isUpdating = state;
}

bool ProgramHoster::GetActive()
{
	return isUpdating;
}

void ProgramHoster::SetActiveInstance(bool state)
{
	m_isUpdating = state;
}

bool ProgramHoster::GetActiveInstance()
{
	return m_isUpdating;
}

void ProgramHoster::Display(ProgramHoster* ProgramHoster)
{
	timeCounter += Time::GetTime() - lastTime;

	if (timeCounter >= 1)
	{
		if (!ProgramHoster->m_update)
		{
			std::cerr << "Update() failed for Program Name: " << ProgramHoster->m_program->GetProgramName() << std::endl;
		}

		if (!ProgramHoster->m_input)
		{
			std::cerr << "ProcessInput() failed for Program Name: " << ProgramHoster->m_program->GetProgramName() << std::endl;
		}

		if (!ProgramHoster->m_render)
		{
			std::cerr << "Render() failed for Program Name: " << ProgramHoster->m_program->GetProgramName() << std::endl;
		}

		std::cerr << ProgramHoster->m_program->GetLatestError();

		if (ProgramHoster->m_verbose)
		{
			ProgramHoster->m_inputTimer.DisplayAndReset("Program Name: " + ProgramHoster->m_program->GetProgramName() + " (ProcessInput)");
			ProgramHoster->m_updateTimer.DisplayAndReset("Program Name: " + ProgramHoster->m_program->GetProgramName() + " (Update)");
			ProgramHoster->m_renderTimer.DisplayAndReset("Program Name: " + ProgramHoster->m_program->GetProgramName() + " (Render)");
		}
		timeCounter = 0;
	}

	lastTime = Time::GetTime();
}

Program::Program() :
	m_parent(0)
{
}

Program::~Program()
{
}

int Program::Init()
{
	return 1;
}

int Program::ProcessInput(const Input& input, float delta)
{
	return 1;
}

int Program::Update(float delta)
{
	return 1;
}

int Program::Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
{
	return 1;
}

int Program::Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
{
	return 1;
}

std::string Program::GetProgramName()
{
	return "Program";
}

std::string Program::GetLatestError()
{
	if (m_error.size() == 0)
	{
		return "";
	}

	std::vector<std::string> error(m_error.begin(), m_error.end());
	std::string lastError = "Unhandled exception: {\n";
	for (unsigned int i = 0; i < error.size(); i++)
	{
		lastError += " -- " + error[i] + "\n";
	}
	lastError += "};\n";

	m_error.clear();
	return lastError;
}

void Program::DataDeploy(tinyxml2::XMLElement* data)
{
}

void Program::SetParent(Entity* parent)
{
	m_parent = parent;
}

void Program::DrawDebugUI()
{
}

void Program::PushError(std::string err)
{
	m_error.insert(err);
}

Entity* Program::GetParent()
{
	if (m_parent == 0)
	{
		std::cerr << "Cannot get null parent in program: " << GetLatestError() << std::endl;
		assert(m_parent != 0);
	}

	return m_parent;
}
