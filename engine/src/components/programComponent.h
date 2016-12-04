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
	Program();

	virtual ~Program();

	virtual int Init();

	virtual int ProcessInput(const Input& input, float delta);

	virtual int Update(float delta);

	virtual int Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera) const;

	virtual int Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);

	virtual void DrawDebugUI();

	virtual std::string GetProgramName();

	virtual std::string GetLatestError();

	virtual void DataDeploy(tinyxml2::XMLElement* data);

	void SetParent(Entity* parent);
protected:
	std::set<std::string> m_error;

	virtual void PushError(std::string err);

	Entity* GetParent();

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
	lua_State* L;
};


class ProgramHoster : public EntityComponent
{
public:
	FCLASS (ProgramHoster);

	ProgramHoster();

	ProgramHoster(Program* program, int verbose = 0, bool start = false);

	~ProgramHoster();

	virtual void DrawDebugUI();

	virtual void Init();

	void InitProgram();

	virtual void DataDeploy(tinyxml2::XMLElement* data);

	virtual void ProcessInput(const Input& input, float delta);

	virtual void Update(float delta);

	virtual void Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera);

	static void SetActive(bool state);;

	static bool GetActive();

	void SetActiveInstance(bool state);;

	bool GetActiveInstance();

private:
	int m_input, m_update, m_render, m_verbose;
	Program* m_program;
	ProfileTimer m_updateTimer, m_renderTimer, m_inputTimer;
	bool m_delayedInit, m_loaded;
	bool m_isUpdating;

	static double lastTime;
	static double timeCounter;
	static bool isUpdating;

	static void ProgramHoster::Display(ProgramHoster* ProgramHoster);
};

#endif // ProgramHoster_H_INCLUDED
