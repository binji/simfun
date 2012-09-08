#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "point.h"
#include "vector.h"
#include "wall.h"

class Background;

class Object
{
/* fields */
protected:
	Point pos;
	Point oldPos;
	Vector scale;
	Vector size;
	float radius;
	Vector normal;
	int normalCount;

	Wall::CPList ignore;

/* constructors */
public:
	Object();
	Object(const Object &o);

/* methods */
public:
	virtual void draw() = 0;
	virtual void update() = 0;
	virtual void doCollision(Background &bg);
	virtual void preProcessWall(const Wall &w) {}
	virtual bool processWall(const Wall &w);
	void collideWall(const Segment &s);
	void addNormal(const Vector &n);

/* setters */
public:
	void setPos(Point _pos) { pos = _pos; }
	void setOldPos(Point _oldPos) { oldPos = _oldPos; }
	void setScale(Vector _scale) { scale = _scale; }
	void setSize(Vector _size) { size = _size; }
	void setRadius(float _radius) { radius = _radius; }

/* getters */
public:
	Point & getPos() { return pos; }
	Point & getOldPos() { return oldPos; }
	Vector & getScale() { return scale; }
	Vector & getSize() { return size; }
	float getRadius() { return radius; }
	Vector & getNormal() { return normal; }
};

#endif