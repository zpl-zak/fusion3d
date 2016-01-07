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

#include "components/physicsObjectComponent.h"
#include "progs/fibonacci_prog.h"
#include "components/devMode.h"
#include "../build/Tests/Mafia/LostHeaven/fpswalker_prog.h"
#include "core/util.h"
#include "core/mapLoader.h"

#include <fstream>
#include <vector>

class TestGame : public Game
{
public:
	TestGame()
	{
		m_map = new MapLoader(Util::ResourcePath() + "maps/test.xml");
	}
	virtual ~TestGame() 
	{
		delete m_map;
	}

	virtual void Init(const Window& window);
protected:
private:
	MapLoader*		m_map;
	TestGame(const TestGame& other) {}
	void operator=(const TestGame& other) {}
};

void TestGame::Init(const Window& window)
{
	m_map->LoadEntities(&m_root);

	AddToScene((new Entity(Vector3f(1, 7, 0)))
		->AddComponent(new ProgramHoster(new FPSWalker(window.GetAspect(), window.GetCenter(), 1)))
		);
        
	RigidBody* rb = new RigidBody (
		new btBoxShape (btVector3 (1, 1, 1)), 1);

	rb->SetEnter ([] (RigidBody* r)
	{
		std::cout << "Collision Enter for " << r->GetParent ()->GetDisplayName () << " of address " << r->GetParent() << std::endl;
	});

    

	rb->SetStay ([] (RigidBody* r)
	{
		//std::cout << "Collision Stay" << std::endl;
		// works
	});

	rb->SetExit ([] (RigidBody* r)
	{
		std::cout << "Collision Exit" << std::endl;
	});

	auto cube = (new Entity (Vector3f (3, 6, 2)))
		->AddComponent (new MeshRenderer ("cube.obj"))
		->AddComponent (rb);

	AddToScene (cube);

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
        RenderingEngine(window),
		m_gaussBlur("filter-gausBlur7x1")
    {
        SetVector3f("ambient", Vector3f(0.1, 0.1, 0.12));
    }

    void Render(const Entity& object) override
    {
        RenderingEngine::Render(object);

		//SetVector3f ("blurScale", Vector3f (1.0f, 0.0f, 0.0f));
		//ApplyFilter (m_gaussBlur, GetTexture ("DisplayTexture"), 0);
    }

private:
	Shader m_gaussBlur;
};

int main()
{
	Testing::RunAllTests();

	Window window(800, 600, "Fusion3D");
	//window.SetFullScreen(1);
	TestRenderer renderer(&window);
	PhysicsEngine physics;
	
	CoreEngine engine(60, &window, &renderer, &physics);
	engine.SetUserspace([]() 
	{
		//You can register custom classes and programs here.
	});
	TestGame game;
	engine.LoadGame(&game);
	engine.Start();

	return 0;
}
