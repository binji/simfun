/***************************************************************************
* SimFun
*  vector.cpp -- simple 2D vector class
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

#include "SDL_opengl.h"
#include "vector.h"

/* these constructors definitions must be moved out of vector.h */
Vector::Vector(const Point &a) : u(a.x), v(a.y) {}
Vector::Vector(const Point &a, const Point &b): u(b.x-a.x), v(b.y-a.y) {}

void Vector::draw(const Point &p)
{
	/* draw a blue vector */
	/* why blue? */
	/* ...don't ask stupid questions */

	glColor3f(0.0f, 0.0f, 1.0f);

	glBegin(GL_LINES);

	glVertex2f(p.x, p.y);
	glVertex2f(p.x + u, p.y + v);

	glEnd();
}