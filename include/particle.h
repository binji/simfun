#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <list>
#include "SDL_opengl.h"
#include "SDL.h"
#include "color.h"
#include "object.h"

class Particle : public Object
{
/* types */
public:
	enum ParticleType
	{
		DUST,
		DROP_1,
		DROP_2
	};

/* fields */
private:
	ParticleType type;
	Color color;

	int lifetime;

public:
	static const Color Dust1, Dust2;
	static const Color Water1, Water2;

/* constructors */
public:
	Particle(ParticleType _type);
	Particle(const Particle &p);

/* methods */
public:
	static Particle make(ParticleType _type, const Point &pos, const Vector &vel, 
		const Color &_color, float _scale, int _lifetime);
	void draw();
	void update();
	bool processWall(const Wall &w);
	bool alive() { return lifetime > 0; }

/* setters */
public:
	void setColor(const Color &_color) { color = _color; }
	void setLifetime(int _lifetime) { lifetime = _lifetime; }
};

struct Point;
struct Vector;

class Particles
{
/* fields */
private:
	std::list<Particle> particles;

/* constructors */
public:
	Particles() {}

/* methods */
public:
	void draw();
	void update();

	void skidDust(const Point &p, const Vector &v);
	void waterSplash(const Point &p, const Vector &v);
	void add(const Particle &p) { particles.push_back(p); }
};

#endif