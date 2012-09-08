/***************************************************************************
* SimFun
*  region.cpp -- regions are arbitrary polygons (no twisting, though)
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

#include "region.h"

bool Region::contains(const Point &p) const
{
	/* this method uses a pretty standard method for determining if a point 
		is inside a polygon, convex/concave, with/without holes:
		count how many walls a ray, with endpoint at p, intersects
		if it is odd, p is inside the polygon
		if it is even, p is outside the polygon
	*/

	bool retVal = false;
	WallSet::ConstIterator i;
	for (i = walls.begin(); i != walls.end(); ++i)
	{
		const Segment &s = (**i).wall.segment;
		if ( (p.y >= s.p0.y && p.y < s.p1.y) ||
			(p.y >= s.p1.y && p.y < s.p0.y) )
		{
			float x = (p.y - s.p0.y) * (s.p1.x - s.p0.x) / (s.p1.y - s.p0.y) + s.p0.x;
			if (p.x > x) 
				retVal = !retVal;
		}
	}
	return retVal;
}

Region::IntersectType Region::intersect(const Circle &c) const
{
	WallSet::ConstIterator i;
	IntersectType retVal = (contains(c.center) ? CONTAIN : DISJOINT);

	for (i = walls.begin(); i != walls.end(); ++i)
	{
		const Segment &s = (**i).wall.segment;
		if (s.intersect(c))
			return INTERSECT;
	}
	return retVal;
}
