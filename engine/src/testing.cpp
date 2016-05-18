// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

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
