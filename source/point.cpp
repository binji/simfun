/***************************************************************************
* SimFun
*  point.cpp -- simple 2D point class (float based)
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
#include "point.h"

Point::Point(const Vector &v) : x(v.u), y(v.v) {}

Point & Point::operator +=(const Vector &v)
{
	x += v.u; 
	y += v.v; 
	return *this;
}

Point & Point::operator -=(const Vector &v)
{
	x -= v.u;
	y -= v.v;
	return *this;
}

Vector operator -(const Point &a, const Point &b)
{
	/* this is backward because Vector(b,a) maps a vector from b to a,
		and point a - point b maps a vector from b to a */
	return Vector(b,a);
}

float Point::dist(const Point &a, const Point &b)
{
	/* simple pythagorean distance */
	float dx = a.x - b.x, dy = a.y - b.y;
	return (float)sqrt(dx*dx + dy*dy);
}