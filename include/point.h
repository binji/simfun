#ifndef __POINT_H__
#define __POINT_H__

#include "vector.h"
#include "global.h"

/* forward declaration */
struct Vector;

struct Point
{
/* fields */
	float x, y;

/* constructors */
	Point() : x(0), y(0) {}
	Point(float _x, float _y) : x(_x), y(_y) {}
	Point(const Vector &v);

/* methods */
	Point &operator +=(const Vector &a);
	Point &operator -=(const Vector &a);

	static float dist(const Point &a, const Point &b);
};

inline Point operator +(const Point &a, const Vector &b) { Point r = a; r += b; return r; }
inline Point operator -(const Point &a, const Vector &b) { Point r = a; r -= b; return r; }
inline bool operator ==(const Point &a, const Point &b)
{
	return fabs(a.x - b.x) < FLOAT_EPSILON && fabs(a.y - b.y) < FLOAT_EPSILON;
}
inline bool operator !=(const Point &a, const Point &b) { return !(a == b); }
Vector operator -(const Point &a, const Point &b);

#endif