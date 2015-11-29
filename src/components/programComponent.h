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

#ifndef ProgramHoster_H_INCLUDED
#define ProgramHoster_H_INCLUDED

#include <iostream>
#include <set>
#include <cassert>

#include "../core/entityComponent.h"
#include "../core/profiling.h"
#include "../core/timing.h"
#include "../reflection.h"

class Program
{
public:
	Program() :
	m_parent(0) {}
	virtual ~Program() {}

	virtual int Init()
	{
		return 1;
	}

	virtual int ProcessInput(const Input& input, float delta)
	{
		return 1;
	}

	virtual int Update(float delta)
	{
		return 1;
	}

	virtual int Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const
	{
		return 1;
	}

	virtual int Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
	{
		return 1;
	}

	virtual std::string GetProgramName()
	{
		return "Program";
	}

	virtual std::string GetLatestError()
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

	inline void SetParent(Entity* parent) { m_parent = parent; }
protected:
	std::set<std::string> m_error;

	virtual void PushError(std::string err)
	{
		m_error.insert(err);
	}

	inline Entity* GetParent() 
	{
		if (m_parent == 0)
		{
			std::cerr << "Cannot get null parent in program: " << GetLatestError() << std::endl;
			assert(m_parent != 0);
		}
		
		return m_parent; 
	}

private:
	Entity* m_parent;
};

class ProgramHoster : public EntityComponent
{
public:
	ProgramHoster() {}
	ProgramHoster(Program* program, int verbose = 0)
		:
		m_delayedInit((!isUpdating)),
		m_loaded(false)
	{
		m_program = program;
		m_verbose = verbose;
		m_loaded = false;
	}

	~ProgramHoster()
	{
		delete m_program;
	}

	virtual void Init()
	{
		m_program->SetParent(GetParent());

		if (!m_program->Init())
		{
			std::cerr << "Init() failed for Program Name: " << m_program->GetProgramName() << std::endl;
		}
	}

	void InitProgram()
	{
		if (m_loaded)
			return;

		m_loaded = true;
		
		if (!m_program->Init())
		{
			std::cerr << "Init() failed for Program Name: " << m_program->GetProgramName() << std::endl;
		}
	}

	virtual void ProcessInput(const Input& input, float delta)
	{
		if (!isUpdating)
			return;

		m_inputTimer.StartInvocation();

		m_input = m_program->ProcessInput(input, delta);

		m_inputTimer.StopInvocation();
	}

	virtual void Update(float delta)
	{
		if (!isUpdating)
			return;
		
//		InitProgram();

		m_updateTimer.StartInvocation();

		m_update = m_program->Update(delta);

		m_updateTimer.StopInvocation();
	}

	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
	{
		if (!isUpdating)
			return;
		
		m_renderTimer.StartInvocation();

		m_render = m_program->Render(shader, renderingEngine, camera);

		m_renderTimer.StopInvocation();

		ProgramHoster::Display(this);
	}
	
	static void SetActive(bool state) { isUpdating = state; };
	static bool GetActive() {
		return isUpdating;
	}

private:
	int m_input, m_update, m_render, m_verbose;
	Program* m_program;
	ProfileTimer m_updateTimer, m_renderTimer, m_inputTimer;
	bool m_delayedInit, m_loaded;

	static double lastTime;
	static double timeCounter;
	static bool isUpdating;

	static void ProgramHoster::Display(ProgramHoster* ProgramHoster)
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
};

#endif // ProgramHoster_H_INCLUDED