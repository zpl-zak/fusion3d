#ifndef LEVEL_H
#define LEVEL_H

#include <string>

class Entity;
class Vector3f;
class Game;

class Level
{
    public:
        Level(Game* game, Entity* entity, std::string mapName, Vector3f spot, std::string matName);
        virtual ~Level();
    protected:
    private:
};

#endif // LEVEL_H
