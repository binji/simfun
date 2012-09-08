/***************************************************************************
* SimFun
*  color.cpp -- simple RGBA color class
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

#include <stdlib.h>
#include "color.h"
#include "misc.h"

Color Color::randomRange(const Color &a, const Color &b)
{
	Color c;
	float s = frand();
	c.r = (u8)((a.r - b.r) * s + b.r);
	c.g = (u8)((a.g - b.g) * s + b.g);
	c.b = (u8)((a.b - b.b) * s + b.b);
	c.a = (u8)((a.a - b.a) * s + b.a);

	return c;
}