/***************************************************************************
* SimFun
*  object.cpp -- abstract base class for mobile entities
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

#include <algorithm>
#include "math.h"
#include "background.h"
#include "object.h"
#include "circle.h"
#include "tilemap.h"
#include "tilemapentry.h"
#include "wallset.h"

Object::Object():
	scale(1,1), radius(0), normalCount(0)
{
}

Object::Object(const Object &o):
	pos(o.pos), oldPos(o.oldPos), scale(o.scale), size(o.size), radius(o.radius),
	normal(o.normal), normalCount(o.normalCount), ignore(o.ignore)
{
}

void Object::addNormal(const Vector &n)
{
	if (normalCount > 2) return;
	normal.u = (normal.u * normalCount + n.u) / (normalCount + 1);
	normal.v = (normal.v * normalCount + n.v) / (normalCount + 1);
	normal.normalize();
	normalCount++;
}

void Object::doCollision(Background &bg)
{
	WallSet set;
	const TileMap &tilemap = bg.getTileMap();

	/* get tile bounds of circle */
	int l = (int)floor((pos.x-radius)/8), r = (int)ceil((pos.x+radius)/8);
	int t = (int)floor((pos.y-radius)/8), b = (int)ceil((pos.y+radius)/8);

	/* add all walls declared for the tiles */
	for (int j= t; j < b; j++)
		for (int i= l; i < r; i++)
		{
			const TileMapEntry *tme = tilemap.index(i,j);
			set.addFromTile(tme);
		}

	/* we want to ignore certain walls (tops of ladders when climbing through 
		them, and one way walls). This loop removes walls from the ignore list
		if the object doesn't intersect with it */
	{
		Wall::CPListIterator i;
		for (i = ignore.begin(); i != ignore.end();)
		{
			const Segment &s = (**i).wall.segment;

			if (!s.intersect(Circle(pos, radius)))
				ignore.erase(i++);
			else
				++i;
		}
	}

	/* all the collision gets messed up when the sometimes here/sometimes
		not walls enter the equation */
	bool irregularWalls = false;
	{
		WallSet::Iterator i;
		for (i = set.begin(); i != set.end(); ++i)
		{
			const Edge::EdgeType &type = (**i).wall.type;
			if (type == Edge::LADDER_TOP || type == Edge::ONE_WAY)
			{
				irregularWalls = true;
				break;
			}
		}
	}

	/* check all walls found above for collision */

	WallSet::Iterator i;
	Segment::List collide;

	for (i = set.begin(); i != set.end(); ++i)
	{
		const Wall &w = (**i);
		const Segment &s = w.wall.segment;

		preProcessWall(**i);

		/* don't process it if it's in the ignore list... */
		if (std::find(ignore.begin(), ignore.end(), &w) != ignore.end())
			continue;
		
		/* we wait to do segment-circle collision if we're doing complicated
			collision */
		if (!irregularWalls)
		{
			/* ...or if it doesn't even intersect the circle */
			if (!s.intersect(Circle(pos, radius)))
				continue;
		}

		if (processWall(**i))
		{
			if (irregularWalls)
				collide.push_back((**i).wall.segment);
			else
				collideWall((**i).wall.segment);
		}
	}

	/* the problem with using normal collision:
		if we have some walls that are here today/gone tomorrow the collision
		will be irregular because of the way segment-circle collision works.
		
		when the circle collides with two walls with the same normal near their
		intersection point, one of the segments will (probably) provide a shunt 
		vector that is not the normal, whereas the other (probably) will. This
		will push the object in the direction of the vector, even though it
		shouldn't.

		how to fix it (basic idea):
		* combine collinear segment that intersect at a point like in 
			Walls::addWall
		* sometimes walls will exist on the other side of this newly created
			wall. If the object is on the normal side, remove this "T-Bone" wall.
	*/
	if (irregularWalls)
	{
		/* combine collinear segments that intersect at a point,
			like in Walls::addWall */
		Segment::ListIterator k, l;

		for (k = collide.begin(); k != collide.end();)
		{
			bool removed = false;

			const Segment &s = *k;
			l = k;
			for (++l; l != collide.end(); ++l)
			{
				const Segment &t = *l;
				Point p0, p1;
				Segment::IntersectType it = s.intersect(t,p0,p1);

				if (it != Segment::COLLINEAR_POINT) continue;
				if (s.normal != t.normal) continue;

				if (p0 == t.p0)
					collide.push_back(Segment(s.p0, t.p1));
				else
					collide.push_back(Segment(t.p0, s.p1));

				collide.erase(k++);
				if (k == l)
					collide.erase(k++);
				else
					collide.erase(l);

				removed = true;
				break;
			}

			if (!removed) ++k;
		}

		/* look for edges that T-bone another segment */
		for (k = collide.begin(); k != collide.end();)
		{
			bool removed = false;
			const Segment &s = *k;
			l = k;
			for (++l; l != collide.end(); ++l)
			{
				const Segment &t = *l;
				Point p0, p1;
				Segment::IntersectType it = s.intersect(t,p0,p1);

				if (it != Segment::POINT) continue;
				/* we know that the combined segments are at the end of the
					collide list, therefore if two segments are T-boned, the
					top of the "T" must be t and the base of the "T" must be
					s.

					Therefore, if the intersection point is t, we know it can't
					be a T-bone.
				*/
				if (p0 == t.p0 || p0 == t.p1) continue;
				
				/* find which point connects the top of the "T" to the base. */
				if (p0 == s.p0)
				{
					/* we'll erase the base if the top normal points away from
						the base and if the object is on the top normal side */
					if (!t.faces(s.p1) && t.faces(pos))
					{
						collide.erase(k++);
						removed = true;
						break;
					}
				}
				else if (p0 == s.p1)
				{
					/* we'll erase the base if the top normal points away from
						the base and if the object is on the top normal side */
					if (!t.faces(s.p0) && t.faces(pos))
					{
						collide.erase(k++);
						removed = true;
						break;
					}
				}
			}
			if (!removed) ++k;
		}

		/* shunt object around to avoid collision */
		for (k = collide.begin(); k != collide.end(); ++k)
		{
			const Segment &s = *k;
			/* this check is necessary, if we check walls without moving the
				object, we get false positives */
			if (s.intersect(Circle(pos, radius)))
				collideWall(*k);
		}
	}
}

bool Object::processWall(const Wall &w)
{
	const Edge &e = w.wall;
	const Segment &s = e.segment;

	/* one way walls... */
	if (e.type == Edge::ONE_WAY)
	{
		/* are ignored if the object is moving in the "one way" */
		Vector vel(oldPos, pos);
		if (Vector::dot(vel, s.normal) > 0)
		{
			ignore.push_back(&w);
			return false;
		}
		return true;
	}
	else if (e.type == Edge::SOLID || e.type == Edge::LADDER_TOP)
		return true; /* nothing -- these are normal barriers */

	return false; /* move through these walls (WATER, LADDER, WEAK) */
}

void Object::collideWall(const Segment &s)
{
	/* collide if the wall normal is pointing toward the object center */
	if (s.faces(pos))
	{
		Point p = s.closestPoint(pos);

		Vector d(p, pos);
		float dlen = d.length();

		d.normalize();
		/* shunt the object out from the wall */
		pos += d * (radius - dlen);

		/* if the object collides with a "floor-like" normal, add it */
		if (s.normal.v < 0)
			addNormal(d);

	}
}