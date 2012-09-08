#ifndef __WALLS_H__
#define __WALLS_H__

#include <list>
#include <set>
#include "wall.h"

class Background;
class TileMap;
class TileMapEntry;

class Walls
{
/* fields */
private:
	Wall::List walls;

/* constructors */
public:
	Walls(Background &bg);

/* methods */
private:
	void addWall(const Edge &e, TileMap &tilemap, TileMapEntry &tme);
	void addTile(TileMap &tilemap, TileMapEntry &tme);
	void remapWall(const Wall &w, Wall *new1, Wall *new2);
	void removeWall(const Wall *w);

public:
	void draw();
};

#endif