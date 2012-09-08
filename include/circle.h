#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include "point.h"

struct Circle
{
/* fields */
	Point center;
	float radius;

/* constructors */
	Circle(const Point &c, float r) : center(c), radius(r) {}

/* methods */
};

#endif