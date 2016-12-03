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

#include "testing.h"

#include <iostream>
#include <cassert>
#include "core/math3d.h"
#include <stdio.h>
#include <Windows.h>

void Testing::RunAllTests()
{
	//Math
	{
		Vector3f x = Vector3f(6.f, 3.f, 2.f);
		size_t x_size = x.Length();
		assert(x_size == 7.f);

		Vector3f y = Vector3f(12.f, 6.f, 4.f);
		size_t y_size = y.Length();
		assert(y_size == 14.f);

		auto nrm_x = x.Normalized();
		assert(nrm_x.Length() == 1.f);

		auto nrm_y = y.Normalized();
		assert(nrm_y.Length() == 1.f);

		assert(nrm_x == nrm_y);
	}
}
