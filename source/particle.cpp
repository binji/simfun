/***************************************************************************
* SimFun
*  particle.cpp -- draw, move, create particles for dust and water
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

#include <algorithm>
#include "math.h"
#include "particle.h"
#include "misc.h"
#include "simulation.h"

const Color Particle::Dust1(128,128,128,48);
const Color Particle::Dust2(192,192,192,64);
const Color Particle::Water1(0,119,130,64);
const Color Particle::Water2(0,0,128,128);

Particle::Particle(Particle::ParticleType _type):
	type(_type)
{
	size = Vector(4,4);
	radius = 4;
	lifetime = 0;
}

Particle::Particle(const Particle &p) :
	Object(p), type(p.type), color(p.color), lifetime(p.lifetime)
{}

Particle Particle::make(ParticleType _type, const Point &pos, const Vector &vel, 
	const Color &_color, float _scale, int _lifetime)
{
	Particle p(_type);
	p.setPos(pos);
	p.setOldPos(pos - vel);
	p.setColor(_color);
	p.setLifetime(_lifetime);
	p.setScale(Vector(_scale,_scale));
	p.radius *= _scale;
	p.size *= _scale;

	return p;
}

void Particle::draw()
{
	glPushMatrix();
		glTranslatef(pos.x,pos.y,0);
		glScalef(scale.u,scale.v,1);
		glTranslatef(-size.u,-size.v,0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4ub(color.r, color.g, color.b, color.a);

		glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(0,size.v*2);
			glVertex2f(size.u*2,size.v*2);
			glVertex2f(size.u*2,0);
		glEnd();

		glDisable(GL_BLEND);
	glPopMatrix();
}

void Particle::update()
{
	static const float DUST_GRAVITY = -0.1f;
	static const float DROP_GRAVITY = 0.15f;
	static const float DRAG = 0.99f;
	static const float VEL_MAX = 4.5f;

	lifetime--;
	Vector vel(oldPos, pos);
	Vector gravity;

	switch (type)
	{
	case DUST:
		gravity = Vector(0, DUST_GRAVITY);
		break;
	case DROP_1:
	case DROP_2:
		gravity = Vector(0, DROP_GRAVITY);
		break;
	}

	if (vel.length() > VEL_MAX)
	{
		vel.normalize();
		vel *= VEL_MAX;
	}

	oldPos = pos;
	pos += vel * DRAG + gravity;

	pos.x = clamp(pos.x, size.u, 640-size.u);
	pos.y = clamp(pos.y, size.v, 480-size.v);

	if (type == DROP_1 || type == DROP_2)
	{
		/* do a simple collision check for drops first
			to see if they've hit a wall or water */
		int x = (int)floor(pos.x/8), y = (int)floor(pos.y/8);
		/* whew! */
		Tile::TileType type = Simulation::get().getBackground().getTileMap().index(x, y)->getTileType();
		if (type == Tile::SOLID || type == Tile::WATER)
			lifetime = 0;

		/* not really needed, but it makes the collision a little nicer */
		doCollision(Simulation::get().getBackground());
	}
}

bool Particle::processWall(const Wall &w)
{
	static const float NORMAL_SCALE = 2.0f;
	static const float VECTOR_SCALE = 0.8f;
	static const float LIFETIME_SCALE = 80;
	static const int NUM_PARTS = 2;

	lifetime = 0;

	if (type == DROP_1)
	{
		Particles &particles = Simulation::get().getParticles();

		for (int i= 0; i < NUM_PARTS; i++)
		{
			Color color = Color::randomRange(Particle::Water1, Particle::Water2);
			Vector rnd(frand(), frand());
			int lifetime = (int)floor(frand()*LIFETIME_SCALE);

			rnd += w.wall.segment.normal * NORMAL_SCALE;
			rnd *= VECTOR_SCALE;

			particles.add( Particle::make(Particle::DROP_2, pos, rnd, color, scale.u/2, lifetime) );
		}
	}

	return false;
}

void Particles::draw()
{
	std::list<Particle>::iterator i;
	for (i = particles.begin(); i != particles.end(); ++i)
		(*i).draw();
}

void Particles::update()
{
	std::list<Particle>::iterator i;
	for (i = particles.begin(); i != particles.end();)
	{
		(*i).update();

		/* erase the dead particles */
		if (!(*i).alive())
			particles.erase(i++);
		else
			++i;
	}
}

void Particles::skidDust(const Point &p, const Vector &v)
{
	/* I just tweaked these constants until I thought they looked good. */
	static const float NUM_PARTICLE_SCALE = 4.0f;
	static const float VECTOR_SCALE = 0.5f;
	static const float LIFETIME_SCALE = 15;

	int numParts = (int)floor(v.length()*NUM_PARTICLE_SCALE);

	for (int i= 0; i < numParts; i++)
	{
		Color color = Color::randomRange(Particle::Dust1, Particle::Dust2);
		Vector rnd(frand()*2-1, frand()*2-1);
		int lifetime = (int)floor(frand()*LIFETIME_SCALE);

		rnd += v;
		rnd *= VECTOR_SCALE;

		particles.push_back( Particle::make(Particle::DUST, p, rnd, color, frand(), lifetime) );
	}
}

void Particles::waterSplash(const Point &p, const Vector &v)
{
	/* I just tweaked these constants until I thought they looked good. */
	static const float NUM_PARTICLE_SCALE = 2.5f;
	static const float VECTOR_SCALE = 0.8f;
	static const float LIFETIME_SCALE = 80;

	int numParts = (int)floor(v.length()*NUM_PARTICLE_SCALE);

	for (int i= 0; i < numParts; i++)
	{
		Color color = Color::randomRange(Particle::Water1, Particle::Water2);
		Vector rnd(frand(), frand());
		int lifetime = (int)floor(frand()*LIFETIME_SCALE);

		rnd += v;
		rnd *= VECTOR_SCALE;

		particles.push_back( Particle::make(Particle::DROP_1, p, rnd, color, frand(), lifetime) );
	}
}
