#ifndef __TILE_MAP_H__
#define __TILE_MAP_H__

#include <list>
#include "tilemapentry.h"

class TileMap
{
/* fields */
private:
	TileMapEntry *map;
	unsigned int width, height;

/* constructors */
public:
	TileMap(unsigned int _width, unsigned int _height);
	~TileMap();

/* methods */
public:
	TileMapEntry * index(int i, int j);
	const TileMapEntry * index(int i, int j) const;
};

#endif