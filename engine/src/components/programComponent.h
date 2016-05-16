// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef ProgramHoster_H_INCLUDED
#define ProgramHoster_H_INCLUDED

#include <iostream>
#include <set>
#include <cassert>

#include "../core/entityComponent.h"
#include "../core/profiling.h"
#include "../core/timing.h"
#include "../reflection.h"
#include "../core/util.h"

#include "../staticLibs/luna.h"
#include "../staticLibs/imgui.h"

#include "../core/util.h"

class Program
{
public:
	Program() :
		m_parent(0)
	{
	}

	virtual ~Program()
	{
	}

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

	virtual void DataDeploy(tinyxml2::XMLElement* data)
	{
	}

	inline void SetParent(Entity* parent)
	{
		m_parent = parent;
	}

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

protected:
	Entity* m_parent;
};

class LuaProgram : public Program
{
public:
	LuaProgram()
	{
		L = lua_open();
		luaopen_base(L);
		luaopen_table(L);
		luaopen_io(L);
		luaopen_string(L);
		luaopen_math(L);
		luaopen_debug(L);

	}

	LuaProgram(std::string file)
		: LuaProgram()
	{
		auto p = ("scripts/" + file);
		luaL_loadfile(L, p.c_str());

		lua_pcall(L, 0, 0, 0);
	}

	virtual ~LuaProgram() { lua_close(L); };


private:
	lua_State * L;
};


class ProgramHoster : public EntityComponent
{
public:
	FCLASS (ProgramHoster);

	ProgramHoster();

	ProgramHoster(Program* program, int verbose = 0, bool start = false)
		:
		m_delayedInit((!isUpdating)),
		m_loaded(false),
		m_isUpdating(start)
	{
		m_program = program;
		m_verbose = verbose;
		m_loaded = false;
	}

	~ProgramHoster()
	{
		delete m_program;
	}

	virtual void DrawDebugUI()
	{
		static char name[256] = { 0 };
		static char params[256] = { 0 };

		ImGui::InputText("Program", name, 256);
		ImGui::InputText("Parameters", params, 256);

		if (ImGui::Button("Load Program"))
		{
			m_program = (Program*)g_programs.construct(name);

			Init();
		}

		ImGui::Separator();
		ImGui::Checkbox("Running", &m_isUpdating);
	}

	virtual void Init()
	{
		if (!m_program)return;
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

	virtual void DataDeploy(tinyxml2::XMLElement* data)
	{
		m_program = (Program*)g_factory.construct(data->GetText());
	}

	virtual void ProcessInput(const Input& input, float delta)
	{
		if (!m_program)return;

		if (!isUpdating && (!m_isUpdating))
			return;

		m_inputTimer.StartInvocation();

		m_input = m_program->ProcessInput(input, delta);

		m_inputTimer.StopInvocation();
	}

	virtual void Update(float delta)
	{
		if (!m_program)return;

		if (!isUpdating && (!m_isUpdating))
			return;

		//		InitProgram();

		m_updateTimer.StartInvocation();

		m_update = m_program->Update(delta);

		m_updateTimer.StopInvocation();
	}

	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
	{
		if (!m_program)return;
		
		if (!isUpdating && (!m_isUpdating))
			return;

		m_renderTimer.StartInvocation();

		m_render = m_program->Render(shader, renderingEngine, camera);

		m_renderTimer.StopInvocation();

		ProgramHoster::Display(this);
	}

	static void SetActive(bool state)
	{
		isUpdating = state;
	};

	static bool GetActive()
	{
		return isUpdating;
	}

	void SetActiveInstance(bool state)
	{
		m_isUpdating = state;
	};

	bool GetActiveInstance()
	{
		return m_isUpdating;
	}

private:
	int m_input, m_update, m_render, m_verbose;
	Program* m_program;
	ProfileTimer m_updateTimer, m_renderTimer, m_inputTimer;
	bool m_delayedInit, m_loaded;
	bool m_isUpdating;

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


