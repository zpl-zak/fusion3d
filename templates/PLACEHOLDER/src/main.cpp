// Copyright [YEAR] [NAME] [STATEMENT]
#include "main.h"
#include "3DEngine.h"
#include "testing.h"

#include "Games/PLACEHOLDERGame.h"
#include "Renderers/PLACEHOLDERRenderer.h"
#include "Physics/PLACEHOLDERPhysics.h"
#include <iostream>
int main()
{
#ifdef DEBUG
	Testing::RunAllTests ();
#endif


	Util::Init("PLACEHOLDER", "Fusion3D");
	std::cout << "Data Path: " << Util::ResourcePath() << std::endl;

	Window gameWindow (1280, 800, "PLACEHOLDER");
	
	PLACEHOLDERRenderer gameRenderer (&gameWindow);
	
	PLACEHOLDERPhysics gamePhysics;

	PLACEHOLDERGame mainGame;

	CoreEngine engine (60, &gameWindow, &gameRenderer, &gamePhysics);

	RegisterUserspace (&engine);
	
	engine.LoadGame (&mainGame);

	engine.Start ();

	return 0;
}