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

#include "3DEngine.h"
#include "testing.h"

#include "components/freeLook.h"
#include "components/freeMove.h"
#include "components/physicsEngineComponent.h"
#include "components/physicsObjectComponent.h"
#include "components/worldComponent.h"
#include "physics/boundingSphere.h"
#include "physics/plane.h"
#include "progs/fibonacci_prog.h"
#include "components/devMode.h"
#include "../build/Tests/Mafia/LostHeaven/fpswalker_prog.h"
#include "progs/bird_prog.h"
#include "components/GUIWindow.h"

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
		->AddComponent(new ProgramComponent(new FPSWalker(window.GetAspect(), window.GetCenter(), 1)))
	//	->AddComponent(new DirectionalLight(Vector3f(0.8, 0.9, 0.7), 10, 2.96, 10))
		);

	AddToScene((new Entity())
		->AddComponent(new MeshRenderer("playground.obj"))
		->AddComponent(new PhysicsObjectComponent(
			new btBvhTriangleMeshShape(Mesh::ImportCollision("playground_col.obj"), true)
			))
		);

	AddToScene((new Entity(Vector3f(0, 2, 0), Quaternion(Vector3f(1,0,0), ToRadians(90))))
		//->AddComponent(new Decal(new Material("super", Texture("super.jpg"), 0.0, 0.0), Vector3f(1,1,1), 1))
		);

	AddToScene((new Entity(Vector3f(), Quaternion(Vector3f(1, 0, 0), ToRadians(-45))))
		->AddComponent(new DirectionalLight(Vector3f(1.0, 0.49, 0.31),3
			, 10, 80.0f, 1, 0)));
	
	AddToScene((new Entity())
		->AddComponent(new DevMode(window, true))
		);

	CoreEngine::GetCoreEngine()->SetSimulation(true);
}

#include <iostream>

class TestRenderer : public RenderingEngine
{
public:
    TestRenderer(Window* window) :
        RenderingEngine(window)
    {
        SetVector3f("ambient", Vector3f(0.2, 0.4,0.6));
    }

    void Render(const Entity& object) override
    {
        RenderingEngine::Render(object);
    }
};

int main()
{
	Testing::RunAllTests();

	TestGame game;
	Window window(800, 600, "Fusion3D");
	//window.SetFullScreen(1);
	TestRenderer renderer(&window);
	PhysicsEngine physics;
	
	CoreEngine engine(60, &window, &renderer, &physics, &game);
	engine.Start();

	return 0;
}
