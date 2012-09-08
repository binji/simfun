#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include <stack>
#include "SDL_opengl.h"
#include "SDL.h"
#include "tiles.h"
#include "tilemap.h"
#include "region.h"

class Walls;

class Background
{
/* types */
private:
	/* used for seed fill */
	struct Strip
	{
		int xl, xr, y, dy;
		Strip(int _xl, int _xr, int _y, int _dy): 
			xl(_xl), xr(_xr), y(_y), dy(_dy) {}
	};

/* fields */
private:
	Tile::TileType *map;
	SDL_Surface *image;
	int tileWidth, tileHeight;
	GLuint tiles;

	bool *mappedTile;
	TileMap *tilemap;
	Walls *walls;
	Region::List regions;

/* constructors */
public:
	Background(): 
		tileWidth(80), tileHeight(60),
		tilemap(NULL), map(NULL), image(NULL), walls(NULL) {}
	~Background();

/* methods */
private:
	void readMapFromFile(const char *file);
	void mapRegions();
	void clearMappedTile();
	bool checkIndex(int i, int j, Tile::TileType type);
	void paintIndex(Region &region, int i, int j);
	void regionFill(Region &region, int i, int j);

public:
	void loadTiles(const char *file);
	void loadMap(const char *file);
	void deleteMap();
	void drawTiles();
	void drawWalls();

/* getters */
public:
	const Tile::TileType & mapIndex(int i, int j) const { return map[j*tileWidth+i]; }
	Tile::TileType & mapIndex(int i, int j) { return map[j*tileWidth+i]; }
	const TileMap & getTileMap() const { return *tilemap; }
	TileMap & getTileMap() { return *tilemap; }
	int getTileWidth() const { return tileWidth; }
	int getTileHeight() const { return tileHeight; }
};

#endif