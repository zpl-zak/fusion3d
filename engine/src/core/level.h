#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include "math3d.h"
#include "game.h"

class Level
{
    public:
        Level(Game* game, std::string mapName, Vector3f spot, std::string matName);
        virtual ~Level();
    protected:
    private:
};

#endif // LEVEL_H
