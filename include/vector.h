#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>
#include "point.h"
#include "global.h"

/* forward declaration */
struct Point;

struct Vector
{
/* fields */
	float u, v;

/* constructors */
	Vector(): u(0), v(0) {}
	Vector(float _u, float _v) : u(_u), v(_v) {}
	Vector(const Point &a);
	Vector(const Point &a, const Point &b);
	Vector(const Vector &a) : u(a.u), v(a.v) {}

/* methods */
	Vector & operator =(const Vector &a)  { u  = a.u; v  = a.v; return *this; }
	Vector & operator +=(const Vector &a) { u += a.u; v += a.v; return *this; }
	Vector & operator -=(const Vector &a) { u -= a.u; v -= a.v; return *this; }
	Vector & operator *=(float b) { u *= b; v *= b; return *this; }

	static float dot(const Vector &a, const Vector &b) { return a.u * b.u + a.v * b.v; }
	static Vector perp(const Vector &a) { return Vector(a.v,-a.u); }
	static float perp(const Vector &a, const Vector &b) { return a.u * b.v - a.v * b.u; }

	float length() const { return (float)sqrt(u*u+v*v); }
	void normalize() { float l = length(); if (l != 0) { u /= l; v /= l; } }

	void draw(const Point &p);
};

inline Vector operator +(const Vector &a, const Vector &b) { Vector r = a; r += b; return r; }
inline Vector operator -(const Vector &a, const Vector &b) { Vector r = a; r -= b; return r; }
inline Vector operator *(const Vector &a, float b) { Vector r = a; r *= b; return r; }
inline Vector operator *(float b, const Vector &a) { Vector r = a; r *= b; return r; }

inline Vector operator +(const Vector &a) { return a; }
inline Vector operator -(const Vector &a) { return Vector(-a.u, -a.v); }
inline bool operator ==(const Vector &a, const Vector &b)
{
	return ( fabs(a.u - b.u) < FLOAT_EPSILON &&
			fabs(a.v - b.v) < FLOAT_EPSILON );
}
inline bool operator !=(const Vector &a, const Vector &b) { return !(a==b); }

#endif