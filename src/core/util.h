/*
 * Copyright (C) 2014 Dominik Madarasz
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
#include <unistd.h>
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

/**
 * Various functions that didn't make it to actual classes but are still useful in general uses.
 */ 
namespace Util
{
	void Init(const char *app, const char *org = "Fusion3D");
	void Sleep(int milliseconds);
	std::vector<std::string> Split(const std::string &s, char delim);
	std::string ResourcePath(void);

    template < typename T > std::string to_string(const T& n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }

	
};

#endif
