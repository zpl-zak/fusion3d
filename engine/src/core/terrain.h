#pragma once

#include <string>

class Entity;
class Game;

class Terrain
{
public:
	Terrain (Game* game, Entity* entity, std::string mapName, int height, std::string matName);
	~Terrain ();
};

