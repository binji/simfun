#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <list>
#include "point.h"
#include "vector.h"
#include "circle.h"

struct Segment
{
/* types */
public:
	enum IntersectType
	{
		DISJOINT,
		COLLINEAR_POINT,
		POINT,
		SEGMENT
	};

	typedef std::list<Segment> List;
	typedef List::iterator ListIterator;
	typedef List::const_iterator ListConstIterator;

/* fields */
public:
	Point p0, p1;
	Vector normal;

/* constructors */
public:
	Segment(const Point &_p0, const Point &_p1) : 
		p0(_p0), p1(_p1), normal( Vector::perp(Vector(_p0,_p1)) )
	{
		normal.normalize();
	}

/* methods */
private:
	/* used by intersect(const Segment...) ... not stand-alone */
	bool contains(const Point &p) const;
public:
	float dist(const Point &p) const;
	Point closestPoint(const Point &p) const;
	bool intersect(const Circle &c) const;
	IntersectType intersect(const Segment &s, Point &p0, Point &p1) const;
	bool contains(const Segment &s) const;
	bool faces(const Point &p) const;
	
	Segment &operator +=(const Vector &v) { p0 += v; p1 += v; return *this; }
};

inline Segment operator +(const Segment &s, const Vector &v) { Segment r = s; r += v; return r; }

#endif
