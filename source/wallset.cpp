/***************************************************************************
* SimFun
*  wallset.cpp -- set (as in unique elements) of const Wall pointers
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

#include "wallset.h"
#include "tilemapentry.h"

void WallSet::addFromTile(const TileMapEntry *tme)
{
	/* TileMapEntries are sometimes NULL (for out of bounds) */
	if (tme)
	{
		Wall::CPListConstIterator i;
		const Wall::CPList &walls = tme->getWalls();

		for (i = walls.begin(); i != walls.end(); ++i)
			insert(begin(), *i);
	}
}