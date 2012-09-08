#ifndef __WALL_SET_H__
#define __WALL_SET_H__

#include <set>
#include "wall.h"

class TileMapEntry;

class WallSet : public std::set<const Wall *>
{
/* types */
public:
	typedef std::set<const Wall *>::iterator Iterator;
	typedef std::set<const Wall *>::const_iterator ConstIterator;

/* methods */
public:
	void add(const Wall &w) { insert(begin(), &w); }
	void addFromTile(const TileMapEntry *tme);
};

#endif