/***************************************************************************
* SimFun
*  background.cpp -- handles loading maps and drawing tiles
* Copyright (C) 2004	Ben Smith
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
* 
* The license can be found at http://www.gnu.org/copyleft/.
* The author can be contacted at bats@hush.com
*
*****************************************************************************/

#include <math.h>
#include "SDL_opengl.h"
#include "SDL.h"
#include "background.h"
#include "misc.h"
#include "player.h"
#include "walls.h"

void Background::drawTiles()
{
	if (!map) return;

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glPushMatrix();
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, tiles);

	glBegin(GL_QUADS);

	for (int j = 0; j < tileHeight; j++)
	{
		for (int i= 0; i < tileWidth; i++)
		{
			float t = mapIndex(i,j)*8/(float)image->h, b = t+8/(float)image->h;
			
			glTexCoord2f(0, t); 	glVertex2i(i*8+0,j*8+0);
			glTexCoord2f(0, b); 	glVertex2i(i*8+0,j*8+8);
			glTexCoord2f(1, b); 	glVertex2i(i*8+8,j*8+8);
			glTexCoord2f(1, t); 	glVertex2i(i*8+8,j*8+0);				
		}
	}

	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void Background::drawWalls()
{
	walls->draw();
}

void Background::readMapFromFile(const char *file)
{
	FILE *f;
	
	if ((f = fopen(file,"rb")) == NULL)
	{
		ErrorBox("Can't open map: \"%s\"",file);
		exit(1);
	}

	map = new Tile::TileType [tileWidth * tileHeight];

	int i = 0;
	int c;

	while (!feof(f) && i < tileWidth * tileHeight)
	{
		c = tolower(fgetc(f));

		if (isalpha(c) && c - 'a' + 10 < Tile::MAX_TILE_TYPES)
			map[i++] = (Tile::TileType)(c - 'a' + 10);
		else if (isdigit(c) && c - '0' < Tile::MAX_TILE_TYPES)
			map[i++] = (Tile::TileType)(c - '0');
		else if (c == '\n' || c == '\r')
			;
		else
		{
			ErrorBox("Error reading map: \"%s\"",file);
			fclose(f);
			exit(1);
		}
	}
	
	fclose(f);
}

void Background::loadMap(const char *file)
{
	deleteMap();
	readMapFromFile(file);
	tilemap = new TileMap(tileWidth, tileHeight);
	walls = new Walls(*this);
	mapRegions();
}

void Background::deleteMap()
{
	if (map) { delete [] map; map = NULL; }
	if (walls) { delete walls; walls = NULL; }
	if (tilemap) { delete tilemap; tilemap = NULL; }
	regions.clear();
}

void Background::loadTiles(const char *file)
{
	if ((image = loadBMP(file)) == NULL) exit(1);
	if ((image = makeTexture(image)) == NULL) exit(1);

	glGenTextures(1,&tiles);
	glBindTexture(GL_TEXTURE_2D, tiles);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
}

Background::~Background()
{
	deleteMap();
	if (image) SDL_FreeSurface(image);
}

void Background::clearMappedTile()
{
	for (int i= 0; i< tileWidth * tileHeight; i++)
		mappedTile[i] = false;
}

void Background::paintIndex(Region &region, int i, int j)
{
	TileMapEntry *tme = tilemap->index(i,j);
	if (tme == NULL) return;

	mappedTile[i + j*tileWidth] = true;
	region.addFromTile(tme);
	tme->setRegion(&region);
}

bool Background::checkIndex(int i, int j, Tile::TileType type)
{
	return mapIndex(i, j) == type && !mappedTile[i + j*tileWidth];
}

/* Paul Heckbert's Seed Fill algorithm/code */
/********************************************************/
/*
 * A Seed Fill Algorithm
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 ...
 * Paul Heckbert	13 Sept 1982, 28 Jan 1987
 */
/********************************************************/

void Background::regionFill(Region &region, int i, int j)
{
	Tile::TileType type = region.getType();
	std::stack<Strip> stack;

	stack.push(Strip(i,i,j,1));
	stack.push(Strip(i,i,j+1,-1));

	while (!stack.empty())
	{
		int x, l;
		Strip s = stack.top(); stack.pop();
		s.y += s.dy;
		if (s.y < 0 || s.y >= tileHeight) continue;

		for (x = s.xl; x >= 0 && checkIndex(x, s.y, type); --x)
			paintIndex(region, x, s.y);

		if (x >= s.xl) goto skip;
		l = x + 1;
		if (l < s.xl) stack.push(Strip(l, s.xl-1, s.y, -s.dy));
		x = s.xl + 1;
		do
		{
			for (; x < tileWidth && checkIndex(x, s.y, type); ++x)
				paintIndex(region, x, s.y);

			stack.push(Strip(l, x-1, s.y, s.dy));
			if (x > s.xr+1) stack.push(Strip(s.xr + 1, x - 1, s.y, -s.dy));

skip:		for (x++; x <= s.xr && mapIndex(x, s.y) != type; ++x);
			l = x;
		}
		while (x <= s.xr);
	}
}

void Background::mapRegions()
{
	mappedTile = new bool [tileWidth * tileHeight];
	clearMappedTile();

	/* basic idea:
		* iterate over all tiles
		* find ladder or water tile
		* use fill algorithm (above) to map tiles to region
		* add walls from tile to region
		* profit
	*/

	for (int j= 0; j< tileHeight; j++)
		for (int i= 0; i < tileWidth; i++)
		{
			Tile::TileType type = mapIndex(i,j);
			if (mappedTile[i + j*tileWidth]) continue;
			if (type != Tile::WATER && type != Tile::LADDER) continue;

			regions.push_back(Region(type));
			regionFill(regions.back(), i,j);
		}

	delete mappedTile;
}