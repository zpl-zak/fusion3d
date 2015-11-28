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

#ifndef FIBONACCI_PROG_H
#define FIBONACCI_PROG_H

#include "../components/programComponent.h"

PROGRAM(FibonacciProgram)
public:
	FibonacciProgram(int in)
	{
		m_in = in;
		m_timer.SetDelay(60);
	}

	virtual int Update(float delta)
	{
		if (m_in == 0)
		{
			PushError("Number cannot be nil.");
			return 0;
		}

		m_out = 42; //fib(m_in + (int)Time::GetTime() % 20);

		return 1;
	}

	virtual int Render(const Shader& shader, const RenderingEngine& renderingEngine, const Camera& camera)
	{
		if (m_timer.HasElapsed())
		{
			std::cout << "Fibonacci: " << m_out << std::endl;
		}

		return 1;
	}

private:
	int m_in;
	int m_out;
	DelayTimer m_timer;

	int fib(int n)
	{
		if (1 == n || 2 == n)
		{
			return 1;
		}
		else
		{
			return fib(n - 1) + fib(n - 2);
		}

	}
};

#endif