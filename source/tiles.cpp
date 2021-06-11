/***************************************************************************
* SimFun
*  tiles.cpp -- classes Tile and Tiles; contain constant info about tiles
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

#include "tiles.h"

const unsigned int Tile::tileWidth = 8;
const unsigned int Tile::tileHeight = 8;

/* this (sorta) copy constructor flips the tile, so I don't have to do it
	by hand */
Tile::Tile(const Tile &t, unsigned int flags)
{
	std::list<Edge>::const_iterator i;
	for (i= t.edges.begin(); i != t.edges.end(); i++)
	{
		const Edge &e = *i;
		const Segment &s = e.segment;
		Point p0(s.p0), p1(s.p1);

		if (flags & H_FLIP)
		{
			p0.x = tileWidth - p0.x;
			p1.x = tileWidth - p1.x;

			std::swap(p0, p1);
		}
		if (flags & V_FLIP)
		{
			p0.y = tileHeight - p0.y;
			p1.y = tileHeight - p1.y;

			std::swap(p0, p1);
		}

		edges.push_back( Edge( Segment( p0, p1 ), e.type) );
	}
}

/* make all the tiles' edges */
/* FYI:
	LADDER_TOP is different so you can walk on top of ladders, and it behaves
		like SOLID
	WEAK is a wall whose sole purpose is to obliterate a ONE_WAY -- it provides
		no collision
	ONE_WAY is SOLID in most cases, but can be ignored, if necessary
	...
	SOLID is solid
	WATER is water
	LADDER is ladder
*/
void Tiles::makeTiles()
{
	Point ul(0,0);
	Point ur(Tile::tileWidth, 0);
	Point dr(Tile::tileWidth, Tile::tileHeight);
	Point dl(0, Tile::tileHeight);

	{
		/* Empty */
		Tile t;

		addTile(Tile::EMPTY, t);
	}

	{
		/* Ladder */
		Tile t;
		t.addEdge( Edge( Segment(ul, ur), Edge::LADDER_TOP) );
		t.addEdge( Edge( Segment(ur, dr), Edge::LADDER) );
		t.addEdge( Edge( Segment(dr, dl), Edge::LADDER) );
		t.addEdge( Edge( Segment(dl, ul), Edge::LADDER) );

		addTile(Tile::LADDER, t);
	}

	{
		/* Water */
		Tile t;
		t.addEdge( Edge( Segment(ul, ur), Edge::WATER) );
		t.addEdge( Edge( Segment(ur, dr), Edge::WATER) );
		t.addEdge( Edge( Segment(dr, dl), Edge::WATER) );
		t.addEdge( Edge( Segment(dl, ul), Edge::WATER) );

		addTile(Tile::WATER, t);
	}

	{
		/* Solid */
		Tile t;
		t.addEdge( Edge( Segment(ul, ur), Edge::SOLID) );
		t.addEdge( Edge( Segment(ur, dr), Edge::SOLID) );
		t.addEdge( Edge( Segment(dr, dl), Edge::SOLID) );
		t.addEdge( Edge( Segment(dl, ul), Edge::SOLID) );

		addTile(Tile::SOLID, t);
	}

	{
		/* One Way Up/Down */
		Tile t;
		t.addEdge( Edge( Segment(ul, ur), Edge::ONE_WAY) );
		t.addEdge( Edge( Segment(dr, dl), Edge::WEAK) );

		addTile(Tile::ONE_WAY_U, t);
		addTile(Tile::ONE_WAY_D, Tile(t, Tile::V_FLIP));
	}

	{
		/* One Way Left/Right */
		Tile t;
		t.addEdge( Edge( Segment(ur, dr), Edge::WEAK) );
		t.addEdge( Edge( Segment(dl, ul), Edge::ONE_WAY) );


		addTile(Tile::ONE_WAY_L, t);
		addTile(Tile::ONE_WAY_R, Tile(t, Tile::H_FLIP));
	}

	{
		/* 45 degree DL, DR, UL, UR */
		Tile t;
		t.addEdge( Edge( Segment( ul, dr), Edge::SOLID) );
		t.addEdge( Edge( Segment( dr, dl), Edge::SOLID) );
		t.addEdge( Edge( Segment( dl, ul), Edge::SOLID) );

		addTile(Tile::DL_45, t);
		addTile(Tile::DR_45, Tile(t, Tile::H_FLIP));
		addTile(Tile::UL_45, Tile(t, Tile::V_FLIP));
		addTile(Tile::UR_45, Tile(t, Tile::H_FLIP | Tile::V_FLIP));
	}

	{
		/* 26 degree high DL, DR, UL, UR */
		Tile t;
		Point p(ur.x, (ur.y + dr.y) / 2);
		t.addEdge( Edge( Segment( ul, p), Edge::SOLID) );
		t.addEdge( Edge( Segment( p, dr), Edge::SOLID) );
		t.addEdge( Edge( Segment( dr, dl), Edge::SOLID) );
		t.addEdge( Edge( Segment( dl, ul), Edge::SOLID) );

		addTile(Tile::DL_26_1, t);
		addTile(Tile::DR_26_1, Tile(t, Tile::H_FLIP));
		addTile(Tile::UL_26_1, Tile(t, Tile::V_FLIP));
		addTile(Tile::UR_26_1, Tile(t, Tile::H_FLIP | Tile::V_FLIP));
	}

	{
		/* 26 degree low DL, DR, UL, UR */
		Tile t;
		Point p(ul.x, (ul.y + dl.y) / 2);
		t.addEdge( Edge( Segment( p, dr), Edge::SOLID) );
		t.addEdge( Edge( Segment( dr, dl), Edge::SOLID) );
		t.addEdge( Edge( Segment( dl, p), Edge::SOLID) );

		addTile(Tile::DL_26_2, t);
		addTile(Tile::DR_26_2, Tile(t, Tile::H_FLIP));
		addTile(Tile::UL_26_2, Tile(t, Tile::V_FLIP));
		addTile(Tile::UR_26_2, Tile(t, Tile::H_FLIP | Tile::V_FLIP));
	}
	{
		/* 63 degree high DL, DR, UL, UR */
		Tile t;
		addTile(Tile::DL_63_1, t);
		addTile(Tile::DR_63_1, Tile(t, Tile::H_FLIP));
		addTile(Tile::UL_63_1, Tile(t, Tile::V_FLIP));
		addTile(Tile::UR_63_1, Tile(t, Tile::H_FLIP | Tile::V_FLIP));
	}

	{
		/* 63 degree low DL, DR, UL, UR */
		Tile t;
		Point p((dr.x + dl.x)/2, dr.y);
		t.addEdge( Edge( Segment( ul, p), Edge::SOLID) );
		t.addEdge( Edge( Segment( p, dl), Edge::SOLID) );
		t.addEdge( Edge( Segment( dl, ul), Edge::SOLID) );

		addTile(Tile::DL_63_2, t);
		addTile(Tile::DR_63_2, Tile(t, Tile::H_FLIP));
		addTile(Tile::UL_63_2, Tile(t, Tile::V_FLIP));
		addTile(Tile::UR_63_2, Tile(t, Tile::H_FLIP | Tile::V_FLIP));
	}

}
