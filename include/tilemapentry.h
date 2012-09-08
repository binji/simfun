#ifndef __TILE_MAP_ENTRY_H__
#define __TILE_MAP_ENTRY_H__

#include <list>
#include "point.h"
#include "tiles.h"
#include "wall.h"

class Region;
struct Wall;

class TileMapEntry
{
/* types */
public:
	typedef std::list<TileMapEntry *> PList;
	typedef PList::iterator PListIterator;
	typedef PList::const_iterator PListConstIterator;

/* fields */
private:
	const Region *region;
	Tile::TileType type;
	Wall::CPList walls;
	Point ulCorner;
	int i, j;

/* constructors */
public:
	TileMapEntry::TileMapEntry(Tile::TileType _type = Tile::EMPTY) : type(_type), region(NULL) {}

/* methods */
public:
	void addWall(const Wall *w) { walls.push_back(w); }
	void setTileType(Tile::TileType _type) { type = _type; }
	void setULCorner(const Point &p) { ulCorner = p; }
	void setIndex(int _i, int _j) { i = _i; j = _j; }
	void setRegion(const Region *_region) { region = _region; }

/* getters */
public:
	const Tile & getTile() const { return Tiles::get().getTile(type); }
	Wall::CPList & getWalls() { return walls; }
	const Wall::CPList & getWalls() const { return walls; }
	const Point &getULCorner() const { return ulCorner; }
	int getI() const { return i; }
	int getJ() const { return j; }
	Tile::TileType getTileType() const { return type; }
	const Region * getRegion() const { return region; }
};

#endif