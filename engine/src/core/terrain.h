#pragma once

#include <string>


class Entity;
class Game;
class Mesh;

class Terrain
{
public:
	Terrain (std::string mapName, int height, bool flat = true);
	~Terrain ();

	void Start(Entity* entity, std::string matName);

private:
	Mesh* m_mesh;
};

