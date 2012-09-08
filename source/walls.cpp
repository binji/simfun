/***************************************************************************
* SimFun
*  walls.cpp -- container for all walls on the map
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

#include <set>
#include <assert.h>
#include "SDL_opengl.h"
#include "walls.h"
#include "background.h"
#include "wallset.h"

Walls::Walls(Background &bg)
{
	/* basic idea:
		process all the tiles from the Background map
		(not to be confused with TileMap, which is much cooler)
	*/
	TileMap &tilemap = bg.getTileMap();
	int i, j;

	for (j = 0; j < 60; j++)
		for (i= 0; i < 80; i++)
		{
			Point p(i*8, j*8);

			TileMapEntry *tme = tilemap.index(i,j);
			assert(tme);

			tme->setTileType(bg.mapIndex(i, j));
			tme->setULCorner(p);
			tme->setIndex(i,j);
			addTile(tilemap, *tme);
		}
}

void Walls::addWall(const Edge &e, TileMap &tilemap, TileMapEntry &tme)
{
	/* this is a pretty serious function...*/
	/* basic idea:
		* The Wall constructor adds tile sized edges from class Tiles to the 
			right place
		* addWall checks this edge against it's neighbors to determine if it
			can be combined or removed
			* two collinear adjacent (connected by a point) segments, with 
				the same normal, and of the same type can be combined
			* two collinear segments that intersect such that one segment
				contains the other, with opposite normals of the same type
				(or of certain other types, see below) can be removed
		* otherwise, add it to the list
	*/

	const Segment &s = e.segment + tme.getULCorner();
	Segment *add = NULL;
	const Wall *remove = NULL;

	int tileI = tme.getI(), tileJ = tme.getJ();
	WallSet set;

	/* add walls to set from <i-1,j-1>,<i,j-1>,<i+1,j-1>,<i-1,j> */
	set.addFromTile( tilemap.index(tileI-1, tileJ-1) );
	set.addFromTile( tilemap.index(tileI+0, tileJ-1) );
	set.addFromTile( tilemap.index(tileI+1, tileJ-1) );
	set.addFromTile( tilemap.index(tileI-1, tileJ+0) );

	WallSet::Iterator i;

	for (i = set.begin(); i != set.end(); ++i)
	{
		const Edge &e2 = (*i)->wall;
		const Segment &t = e2.segment;

		/* p0 and p1 are return values */
		Point p0, p1;
		Segment::IntersectType it = s.intersect(t,p0,p1);
		switch (it)
		{
		case Segment::DISJOINT:
		case Segment::POINT:
			continue;
		case Segment::COLLINEAR_POINT:
			/* normals must be the same to combine */
			if (s.normal != t.normal)
				break;

			/* can only combine like types */
			if (e.type != e2.type)
				break;

			if (p0 == t.p0)
			{
				/* segment goes s -> t
				 extend t back to s
				*/

				add = new Segment(s.p0, t.p1);
			}
			else
			{
				/* segment goes t -> s
				extend t to s
				*/

				add = new Segment(t.p0, s.p1);
			}

			remove = *i;
			break;
		case Segment::SEGMENT:
			/* normals must be opposite to cancel out */
			/* (shouldn't be otherwise, anyway -- it would imply that two 
				tiles were added to the same place) */
			assert(s.normal == -t.normal);

			/* can only cancel like types, or a one way with a weak, or a ladder top with a ladder */
			if (e.type != e2.type && 
				!( (e.type == Edge::ONE_WAY && e2.type == Edge::WEAK) ||
					(e.type == Edge::WEAK && e2.type == Edge::ONE_WAY) ||
					(e.type == Edge::LADDER_TOP && e2.type == Edge::LADDER) ||
					(e.type == Edge::LADDER && e2.type == Edge::LADDER_TOP) )
				)
				break;

			/* delete add if it has been created */
			if (add)
				delete add;

			Wall *new1 = NULL, *new2 = NULL;
			Segment *s1 = NULL, *s2 = NULL;

			/* here we make some assumptions about the way edges are defined in
				Tile::makeTiles and how Segment::intersect returns p0 and p1:
				* edges are defined in clockwise order
				* p0 and p1 are returned as distances along t (the second segment),
					where p0 is closest to t.p0 and p1 is closest to t.p1
			*/

			/* we know that if s and t intersect that either
				* s contains t
				* t contains s
				* neither of above
					but this can't happen because but s must always be at 
					least as small as t (because t could have been combined).

				* if s and t are the same size, p0 = t.p0 and p1 = t.p1
				* if s is smaller than t, p0 will be the point closest to
					t.p0 (which is s.p1, because s and t's points are declared
					in opposite directions, because they're normals are opposite)
					and p1 will be the point closest to t.p1 (s.p0 for the same
					reason).
			*/

			if ( p0 == t.p0 && p1 == t.p1 )
			{
				/* t is the short segment,
				   remove t portion of s, keep ends (if any) */

				if (s.p0 != p1) s1 = new Segment(s.p0, p1);
				if (p0 != s.p1) s2 = new Segment(p0, s.p1);
			}
			else if ( p0 == s.p1 && p1 == s.p0 )
			{
				/* s is the short segment,
					remove s portion of t, keep ends (if any) */

				if (t.p0 != p0) s1 = new Segment(t.p0, p0);
				if (p1 != t.p1) s2 = new Segment(p1, t.p1);
			}
			else
			{
				/* shouldn't happen */
				assert(0);
			}

			/* add the new walls */
			if (s1)
			{
				walls.push_back(Edge(*s1, e2.type));
				new1 = &walls.back();
				delete s1;
			}

			if (s2)
			{
				walls.push_back(Edge(*s2, e2.type));
				new2 = &walls.back();
				delete s2;
			}

			/* remap the old wall */
			remapWall( **i, new1, new2 );
			/* remove the old wall */
			removeWall(*i);
			return;
			break;
		}
	}

	if (add != NULL)
	{
		/* combine */
		walls.push_back( Wall(Edge(*add, e.type)) );
		delete(add);

		Wall *new1 = &walls.back();

		tme.addWall( new1 );
		new1->addTile(&tme);

		remapWall(*remove, new1, NULL);
		removeWall(remove);
	}
	else
	{
		Wall w( Edge(s,e.type) );

		/* add the TileMapEntry to the wall */
		w.addTile(&tme);
		/* add the Wall to the main list */
		walls.push_back( w );
		/* add the Wall to the TileMapEntry list */
		tme.addWall( &walls.back() );
	}
}

void Walls::remapWall(const Wall &w, Wall *new1, Wall *new2)
{
	TileMapEntry::PListConstIterator i;

	for (i = w.tiles.begin(); i != w.tiles.end(); ++i)
	{
//		bool test;
		TileMapEntry &tme = **i;

		/* find the wall to remove from this TileMapEntry */
		Wall::CPList &walls = tme.getWalls();
		walls.erase(std::find(walls.begin(), walls.end(), &w));

		/* find to which new wall segment each tile belongs */
		Edge::ListConstIterator j;
		const Tile &t = tme.getTile();

		if (new1 != NULL && new2 != NULL)
		{
			/* not the most efficient method, but eh... */
			for (j = t.edges.begin(); j != t.edges.end(); ++j)
			{
				const Edge &e = *j;

				if (new1 && new1->wall.segment.contains(e.segment + tme.getULCorner()))
				{
					tme.addWall(new1);
					new1->addTile(&tme);
					break;
				}

				if (new2 && new2->wall.segment.contains(e.segment + tme.getULCorner()))
				{
					tme.addWall(new2);
					new2->addTile(&tme);
					break;
				}
			}
		}
		else if (new1 != NULL)
		{
			tme.addWall(new1);
			new1->addTile(&tme);
		}
		else if (new2 != NULL)
		{
			tme.addWall(new2);
			new2->addTile(&tme);
		}
	}
}

void Walls::removeWall(const Wall *w)
{
	/* look for the wall to erase */
	Wall::ListIterator i;
	for (i = walls.begin(); i != walls.end(); ++i)
		if (w == &*i)
		{
			walls.erase(i);
			break;
		}
}

void Walls::addTile(TileMap &tilemap, TileMapEntry &tme)
{
	Edge::ListConstIterator i;
	const Tile &t = tme.getTile();

	for (i = t.edges.begin(); i != t.edges.end(); ++i)
		addWall(*i, tilemap, tme);
}

void Walls::draw()
{
	Wall::ListConstIterator i;
	glBegin(GL_LINES);

	for (i = walls.begin(); i != walls.end(); ++i)
	{
		const Wall &w = *i;
		const Segment &s = w.wall.segment;
		const Point &p0 = s.p0, &p1 = s.p1;

		glColor3f(0.0f, 0.0f, 0.0f);

		/* draw wall */
		glVertex2f(p0.x, p0.y);
		glVertex2f(p1.x, p1.y);

		/* draw normal, 4 pixels long, starting from the midpoint of the 
			segment */
		Point pm( (p0.x + p1.x)/2, (p0.y + p1.y)/2 );

		glColor3f(1.0f, 0.0f, 0.0f);

		glVertex2f(pm.x, pm.y);
		glVertex2f(pm.x + 4 * s.normal.u, pm.y + 4 * s.normal.v);
	}

	glEnd();
}
