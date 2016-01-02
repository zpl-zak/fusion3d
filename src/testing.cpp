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

#include "testing.h"

#include <iostream>
#include <cassert>
#include "core/math3d.h"
#include <stdio.h>
#include <Windows.h>

void Testing::RunAllTests()
{
	/*Vector3f v0 (1, 0, 0);
	Vector3f v1 (10, 0, 0);
	float z = v1.GetX() - v0.GetX();
	for (;;)
	{ 
		for (size_t i = 0; i < z; i++)
		{
			Vector3f v2 = v0.Lerp (v1,z/1000);
			v0 = v2;
			std::cout << v2.GetX () << std::endl;
		}
		Sleep (500);
		system ("cls");
		v0.SetX (0);
	}
	for (;;);*/
}


/*
FUSIONSCRIPT --

var count = 10;

def AddCount = (x) ->
	count += x,
	ret count;

def Init = () ->
	count *= AddCount(count);

*/