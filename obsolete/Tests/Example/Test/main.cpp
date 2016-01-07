/*
* Copyright (C) 2015 Subvision Studio
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

#include "3DEngine.h"
#include "testing.h"

#include <fstream>
#include <vector>

class TestGame : public Game
{
public:
	TestGame()
	{
	}
	virtual ~TestGame()
	{
	}

	virtual void Init(const Window& window);
protected:
private:
	TestGame(const TestGame& other) {}
	void operator=(const TestGame& other) {}
};

void TestGame::Init(const Window& window)
{
	AddToScene((new Entity(Vector3f(1, 5, 0)))
		->AddComponent(new CameraComponent((new Matrix4f())->InitPerspective(ToRadians(70), window.GetAspect(), 0.001, 1000)))
		);
}

#include <iostream>

class TestRenderer : public RenderingEngine
{
public:
	TestRenderer(Window* window) :
		RenderingEngine(window)
	{
		SetVector3f("ambient", Vector3f(0.2, 0.4, 0.6));
	}

	void Render(const Entity& object) override
	{
		RenderingEngine::Render(object);
	}
};

int main()
{
	Testing::RunAllTests();

	Window window(800, 600, "Fusion3D");
	//window.SetFullScreen(1);
	TestRenderer renderer(&window);
	PhysicsEngine physics;

	CoreEngine engine(60, &window, &renderer, &physics);
	TestGame game;
	engine.LoadGame(&game);
	engine.Start();

	return 0;
}
