#ifndef __REGION_H__
#define __REGION_H__

#include "circle.h"
#include "point.h"
#include "tiles.h"
#include "wallset.h"

class Region
{
/* types */
public:
	enum IntersectType
	{
		DISJOINT,
		INTERSECT,
		CONTAIN
	};

	typedef std::list<Region> List;
	typedef List::iterator ListIterator;
	typedef List::const_iterator ListConstIterator;

/* fields */
private:
	Tile::TileType type;
	WallSet walls;

/* constructors */
public:
	Region(Tile::TileType _type): type(_type) {}

/* methods */
public:
	void addWall(const Wall &w) { walls.add(w); }
	void addFromTile(const TileMapEntry *tme) { walls.addFromTile(tme); }
	bool contains(const Point &p) const;
	IntersectType intersect(const Circle &c) const;

/* getters */
public:
	Tile::TileType getType() const { return type; }
	WallSet &getWalls() { return walls; }
};

#endif