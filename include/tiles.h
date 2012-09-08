#ifndef __TILES_H__
#define __TILES_H__

#include <list>
#include <vector>
#include "Segment.h"

struct Edge
{
/* types */
public:
	enum EdgeType
	{
		SOLID,
		WEAK,
		ONE_WAY,
		LADDER,
		LADDER_TOP,
		WATER
	};

	typedef std::list<Edge> List;
	typedef List::iterator ListIterator;
	typedef List::const_iterator ListConstIterator;

/* fields */
public:
	Segment segment;
	EdgeType type;

/* constructor */
public:
	Edge(const Segment &_segment, EdgeType _type): segment(_segment), type(_type) {}
	Edge(const Edge &e): segment(e.segment), type(e.type) {}
};

struct Tile
{
/* types */
public:
	enum Flags {
		NO_FLIP = 0,
		H_FLIP = 1,
		V_FLIP = 2
	};

	enum TileType {
		EMPTY,
		SOLID,
		
		ONE_WAY_U,
		ONE_WAY_D,
		ONE_WAY_L,
		ONE_WAY_R,

		LADDER,
		WATER,

		DL_45,
		DR_45,
		UL_45,
		UR_45,

		DL_26_1,
		DL_26_2,
		DR_26_1,
		DR_26_2,
		UL_26_1,
		UL_26_2,
		UR_26_1,
		UR_26_2,

		DL_63_1,
		DL_63_2,
		DR_63_1,
		DR_63_2,
		UL_63_1,
		UL_63_2,
		UR_63_1,
		UR_63_2,

		MAX_TILE_TYPES
	};

/* fields */
public:
	std::list<Edge> edges;

public:
	static const unsigned int tileWidth, tileHeight;

/* constructors */
	Tile() {}
	Tile(const Tile &t, unsigned int flags);

/* methods */
public:
	void addEdge(const Edge &e) { edges.push_back(e); }
};

class Tiles
{
/* fields */
	std::vector<Tile> tiles;

/* singleton generator */
public:
	static const Tiles &get() { static Tiles tiles; return tiles; }

/* constructors */
private:
	Tiles() : tiles(28) { makeTiles(); }

/* methods */
private:
	void addTile(int tileIndex, const Tile &t) { tiles[tileIndex] = t; }
	void makeTiles();
public:
	const Tile &getTile(int index) const { return tiles[index]; }
};

#endif