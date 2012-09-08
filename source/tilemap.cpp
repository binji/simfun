/***************************************************************************
* SimFun
*  tilemap.cpp -- 2D array of type TileMapEntry
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

#include <assert.h>
#include "tilemap.h"

TileMap::TileMap(unsigned int _width, unsigned int _height):
	width(_width), height(_height)
{
	map = new TileMapEntry [_width * _height];
}

TileMap::~TileMap()
{
	delete [] map;
}

TileMapEntry * TileMap::index(int i, int j)
{
	if (i >= 0 && i < width && j >= 0 && j < height)
		return &map[j * width + i];
	return NULL;
}

const TileMapEntry * TileMap::index(int i, int j) const
{
	if (i >= 0 && i < width && j >= 0 && j < height)
		return &map[j * width + i];
	return NULL;
}