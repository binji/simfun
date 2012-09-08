/***************************************************************************
* SimFun
*  segment.cpp -- 2D line segment class; two points and a normal
* Copyright (C) 2004	Ben Smith
*	(except where shown below)
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
#include "global.h"
#include "segment.h"

/* Segment::closestPoint(const Point &p),
   Segment::intersect(const Segment &s, Point &p0, Point &p1),
   Segment::contains(Point &p)
    are based on, and contain code copyrighted with this license:
/********************************************************/
/*
	Copyright 2001, softSurfer (www.softsurfer.com)
	This code may be freely used and modified for any purpose
	providing that this copyright notice is included with it.
	SoftSurfer makes no warranty for this code, and cannot be held
	liable for any real or imagined damage resulting from its use.
	Users of this code must verify correctness for their application.
*/
/********************************************************/

float Segment::dist(const Point &p) const
{
	return Vector(closestPoint(p),p).length();
}

Point Segment::closestPoint(const Point &p) const
{
	Vector v = p1 - p0;
	Vector w = p - p0;

	float c1 = Vector::dot(w,v);
	if (c1 <= 0) return p0;

	float c2 = Vector::dot(v,v);
	if (c2 <= c1) return p1;

	float b = c1 / c2;

	return (p0 + b * v);
}

bool Segment::intersect(const Circle &c) const
{
	return dist(c.center) <= c.radius;
}

bool Segment::faces(const Point &p) const
{
	Vector v(p0,p);
	return Vector::dot(v, normal) > 0;
}

Segment::IntersectType Segment::intersect(const Segment &s, Point &p0, Point &p1) const
{
	const Segment &s1 = *this, &s2 = s;

	Vector	  u = s1.p1 - s1.p0;
	Vector	  v = s2.p1 - s2.p0;
	Vector	  w = s1.p0 - s2.p0;
	float	  D = Vector::perp(u,v);

	// test if they are parallel (includes either being a point)
	if (fabs(D) < FLOAT_EPSILON)		// s1 and s2 are parallel
	{
		if (Vector::perp(u,w) != 0 || 
			Vector::perp(v,w) != 0) {
			return DISJOINT;			// they are NOT collinear
		}

		// they are collinear or degenerate
		// check if they are degenerate points
		float du = Vector::dot(u,u);
		float dv = Vector::dot(v,v);
		if (du==0 && dv==0) {			// both segments are points
			if (s1.p0 != s2.p0) 		// they are distinct points
				return DISJOINT;
			p0 = s1.p0; 				// they are the same point
			return POINT;
		}
		if (du==0) {					// s1 is a single point
			if (s2.contains(s1.p0))	// but is not in s2
				return DISJOINT;
			p0 = s1.p0;
			return POINT;
		}
		if (dv==0) {					// s2 a single point
			if (s1.contains(s2.p0))	// but is not in s1
				return DISJOINT;
			p0 = s2.p0;
			return POINT;
		}

		// they are collinear segments - get overlap (or not)
		float t0, t1;					// endpoints of s1 in eqn for s2
		Vector w2 = s1.p1 - s2.p0;
		if (v.u != 0) {
				t0 = w.u / v.u;
				t1 = w2.u / v.u;
		}
		else {
				t0 = w.v / v.v;
				t1 = w2.v / v.v;
		}
		if (t0 > t1) {					// must have t0 smaller than t1
			std::swap(t0,t1);
		}
		if (t0 > 1 || t1 < 0) {
			return DISJOINT;			// NO overlap
		}
		t0 = std::max<float>(0, t0);	// clip to min 0
		t1 = std::min<float>(1, t1);	// clip to max 1

		if (t0 == t1) { 				// intersect is a point
			p0= s2.p0 + t0 * v;
			return COLLINEAR_POINT;
		}

		// they overlap in a valid subsegment
		p0= s2.p0 + t0 * v;
		p1= s2.p0 + t1 * v;
		return SEGMENT;
	}

	// the segments are skew and may intersect in a point
	// get the intersect parameter for S1
	float	  sI = Vector::perp(v,w) / D;
	if (sI < 0 || sI > 1)				// no intersect with S1
		return DISJOINT;

	// get the intersect parameter for S2
	float	  tI = Vector::perp(u,w) / D;
	if (tI < 0 || tI > 1)				// no intersect with S2
		return DISJOINT;

	p0 = s1.p0 + sI * u;			   // compute S1 intersect point
	return POINT;
}

bool Segment::contains(const Point &p) const
{
    if (p0.x != p1.x) {    // S is not vertical
        if (p0.x <= p.x && p.x <= p1.x)
            return true;
        if (p0.x >= p.x && p.x >= p1.x)
            return true;
    }
    else {    // S is vertical, so test y coordinate
        if (p0.y <= p.y && p.y <= p1.y)
            return true;
        if (p0.y >= p.y && p.y >= p1.y)
            return true;
    }
    return false;
}

bool Segment::contains(const Segment &s) const
{
	Point p0, p1;
	return intersect(s,p0,p1) == SEGMENT && p0 == s.p0 && p1 == s.p1;
}