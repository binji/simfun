#ifndef __WALL_H__
#define __WALL_H__

#include <list>
#include <algorithm>
#include "segment.h"
#include "tiles.h"

class TileMapEntry;

struct Wall
{
/* types */
public:
	typedef std::list<Wall> List;
	typedef List::iterator ListIterator;
	typedef List::const_iterator ListConstIterator;

	typedef std::list<const Wall *> CPList;
	typedef CPList::iterator CPListIterator;
	typedef CPList::const_iterator CPListConstIterator;

/* fields */
public:
	Edge wall;
	std::list<TileMapEntry *> tiles;

/* constructors */
public:
	Wall(const Edge &_wall) : wall(_wall) {}
	Wall(const Wall &w): wall(w.wall), tiles(w.tiles) {}

/* methods */
public:
	void addTile(TileMapEntry * tme) { tiles.push_back(tme); }
};

#endif