#include "terrain.h"

#include "../3DEngine.h"

Terrain::Terrain (std::string mapName, std::string matName)
{
	Bitmap level (mapName); level.FlipY ();

	IndexedModel map;
	{

	}
}

Terrain::~Terrain ()
{
}
