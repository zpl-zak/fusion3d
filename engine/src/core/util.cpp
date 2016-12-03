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

#include "util.h"
#include "../reflection.h"
#include <SDL2/SDL.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sstream>

#ifdef WIN32
#include <Windows.h>
#else
#endif

std::string s_ResourcePath = "./data/";

void Util::Init(const char* app, const char* org)
{
	if (SDL_strcmp(app, "") != 0)
	{
		char* base_path = SDL_GetPrefPath(org, app);

		if (base_path)
		{
			s_ResourcePath = SDL_strdup(base_path);

			SDL_free(base_path);
		}
	}
}

void Util::Sleep(int milliseconds)
{
	SDL_Delay(milliseconds);
}

std::string Util::ResourcePath(void)
{
	return s_ResourcePath;
}

std::string Util::ssystem(const char* command)
{
	char tmpname[L_tmpnam];
	std::tmpnam(tmpname);
	std::string scommand = command;
	std::string cmd = scommand + " >> " + tmpname;
	std::system(cmd.c_str());
	std::ifstream file(tmpname, std::ios::in);
	std::string result;
	if (file)
	{
		while (!file.eof()) result.push_back(file.get());
		file.close();
	}
	remove(tmpname);
	return result;
}


std::string Util::ExecuteTask(const std::string& task, const std::string& data)
{
#ifdef OS_WINDOWS
	std::string cmd = "python\\python.exe data/tasks/" + task + ".py " + data;
	std::string res = ssystem(cmd.c_str());
	return res.substr(0, res.size() - 1);
#else
	std::cout << "TaskHoster isn't supported on this platform." << std::endl;
	return "0";
#endif
}

void Util::CreateDir(const std::string& path, void* flags)
{
#ifdef OS_WINDOWS
	CreateDirectoryA(path.c_str(), *(LPSECURITY_ATTRIBUTES*)&flags);
#endif
}

Vector3f Util::ParseVector3(std::string n)
{
	std::vector<std::string> vec = Util::Split(n, ';');
	return Vector3f(atof(vec[0].c_str()), atof(vec[1].c_str()), atof(vec[2].c_str()));
}

Vector2f Util::ParseVector2(std::string n)
{
	std::vector<std::string> vec = Util::Split(n, ';');
	return Vector2f(atof(vec[0].c_str()), atof(vec[1].c_str()));
}

std::vector<std::string> Util::Split(const std::string& s, char delim)
{
	std::vector<std::string> elems;

	const char* cstr = s.c_str();
	unsigned int strLength = (unsigned int)s.length();
	unsigned int start = 0;
	unsigned int end = 0;

	while (end <= strLength)
	{
		while (end <= strLength)
		{
			if (cstr[end] == delim)
				break;
			end++;
		}

		elems.push_back(s.substr(start, end - start));
		start = end + 1;
		end = start;
	}

	return elems;
}
