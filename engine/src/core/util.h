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

#ifndef UTIL_H
#define UTIL_H

//#include <stdio.h>
//#include <GL/glew.h>
//
//#define GL_CHECK(x) do{
//    x;
//    GLenum glerr = glGetError();
//    if (glerr != GL_NO_ERROR) 
//	{
//        printf("OpenGL error: %d, file: %s, line: %d", glerr, __FILE__, __LINE__);
//    }
//} while (0)

#ifndef WIN32

#endif

#ifdef WIN32
#define SNPRINTF _snprintf_s
#else
#define SNPRINTF snprintf
#endif

//#define ZERO_MEM(a) memset(a,0,sizeof(a))
//#define SAFE_DELETE(p) if(p) {delete p; p = NULL;}
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define INVALID_VALUE 0xFFFFFFFF

#include <vector>
#include <string>
#include <sstream>
#include <string>
#include "math3d.h"

/**
 * Various functions that didn't make it to actual classes but are still useful in general uses.
 */ 
namespace Util
{
	void Init(const char *app, const char *org = "Fusion3D");
	void Sleep(int milliseconds);
	std::vector<std::string> Split(const std::string &s, char delim);
	std::string ResourcePath(void);
	std::string ssystem(const char *command);
	std::string ExecuteTask(const std::string& task, const std::string& data="");
	void CreateDir(const std::string& path, void* flags);


    template < typename T > std::string to_string(const T& n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }

	inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	inline void ScreenToWorld (int X, int Y, int W, int H, Matrix4f matrix, Vector3f & out_origin, Vector3f & out_dir)
	{
		Vector4f rayStart (
			((float)X / (float)W - 0.5f) * 2.0f,
			((float)Y / (float)H - 0.5f) * 2.0f,
			-1.0f,
			1.0f
			);

		Vector4f rayEnd (
			((float)X / (float)W - 0.5f) * 2.0f,
			((float)Y / (float)H - 0.5f) * 2.0f,
			0.0f,
			1.0f
			);

		Matrix4f M = matrix.Inverse ();

		Vector4f rayS = (Vector4f)M.Transform(rayStart); rayS = rayS.Normalized();
		Vector4f rayE = (Vector4f)M.Transform(rayEnd);  rayE = rayE.Normalized();

		Vector3f rayDir (Vector4f (rayE - rayS).GetXYZ ());
		rayDir = rayDir.Normalized ();

		out_origin = (Vector3f) rayS.GetXYZ();
		out_dir = rayDir;
	}

	inline std::string chomp(const std::string& s)
	{
		std::string r = "";
		int c = 0;
		for (size_t i = s.size(); i > -1; i--)
		{
			if (s[i] == ' ' || s[i] == '\n' || s[i] == '\t' || s[i] == '\0')
			{
				c++;
			}
			else
			{
				break;
			}
		}

		for (size_t i = 0; i < s.size()-c+1; i++)
		{
			r += s.at(i);
		}

		return r;
	}


	inline std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	Vector3f ParseVector3(std::string n);
	Vector2f ParseVector2(std::string n);
	
	
};

#endif
