#include "PLACEHOLDERGame.h"
#include "components/DevMode.h"

void PLACEHOLDERGame::Init (const Window& window)
{
	AddToScene((new Entity())
        ->AddComponent(new DevMode(window, true))
    );
}
