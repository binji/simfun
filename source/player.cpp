/***************************************************************************
* SimFun
*  player.cpp -- player object class; handles movement, drawing, input
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
#include <math.h>
#include <set>
#include "SDL.h"
#include "SDL_opengl.h"
#include "player.h"
#include "global.h"
#include "misc.h"
#include "segment.h"
#include "simulation.h"
#include "tilemap.h"
#include "wallset.h"

const float Player::DRAG = 0.99f;
const float Player::GRAVITY = 0.15f;
const float Player::AIRBORNE_GRAVITY = 0.35f;
const float Player::WATER_GRAVITY = -0.14f;
const float Player::JUMP_GRAVITY = 0.12f;
const float Player::DRIP_GRAVITY = 2.0f;
const float Player::ACCEL = 0.20f;
const float Player::AIR_ACCEL = 0.10f;
const float Player::SKID_ACCEL = 0.08f;
const float Player::WATER_ACCEL = 0.10f;
const float Player::WATER_THRUST = 3.0f;
const float Player::SKID_MULTIPLIER = 6.0f;
const float Player::SKID_MAX_ANGLE = 20.0f;
const float Player::SKID_FRICTION = 0.90f;
const float Player::CLIMB_FRICTION_X = 0.3f;
const float Player::CLIMB_FRICTION_Y = 0.5f;
const float Player::JUMP_VEL_SCALE = 0.3f;
const float Player::JUMP_THRUST = 4.5f;
const float Player::JUMP_Y_BIAS = 1.2f;
const float Player::AIRBORNE_JUMP_THRUST = 5.4f;
const float Player::VEL_MAX_X = 3.0f;
const float Player::VEL_MAX_Y = 6.0f;
const float Player::VEL_MIN = 1e-3f;
const float Player::MAX_ROT = 5.0f;
const int Player::MAX_JUMP_TIME = 20;
const int Player::TICKS_PER_DRIP = 30;
const int Player::JUMP_MAX_AIRBORNE_TIME = 6;
const int Player::WET_TIME = 60*20;

Player::Player():
	angle(0), skidAngle(0), 
	flags(0),
	jumpTime(0), airborneTime(0), wetTime(0),
	input(0), oldInput(0), newInput(0)
{
	size = Vector(12,16);
	radius = 16;
}

void Player::draw()
{
	glPushMatrix();
		glTranslatef(pos.x,pos.y,0);
		glRotatef(angle,0,0,1);
		glScalef(scale.u,scale.v,1);
		glTranslatef(-size.u,-size.v,0);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);			glVertex2f(0,0);
			glTexCoord2f(0, 1.0f);		glVertex2f(0,size.v*2);
			glTexCoord2f(0.75f, 1.0f);	glVertex2f(size.u*2,size.v*2);
			glTexCoord2f(0.75f, 0);		glVertex2f(size.u*2,0);
		glEnd();

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Player::update()
{
	/* process input */
	Vector acc;
	Vector vel(oldPos, pos);
	Vector gravity(0, GRAVITY);

	if ((input & LEFT) == LEFT) { acc.u -= 1; }
	if ((input & RIGHT) == RIGHT) { acc.u += 1; }

	/* move */
	if (onLadder())
	{
		if (input & UP) acc.v -= 1;
		if (input & DOWN) acc.v += 1;
	}

	if (isClimbing())
	{
		/* you're not climbing if you fall off the ladder,
			or if you touch the ground */
		if (!(onLadder() && isAirborne())) resetFlag(IS_CLIMBING);
	}
	else
	{
		/* you are climbing if you move while on a ladder */
		if (acc.v != 0) setFlag(IS_CLIMBING);
	}

	if (inWater())
	{
		if (newInput & DOWN)
		{
			acc.v += WATER_THRUST;
		}
		if (underWater()) gravity = Vector(0, WATER_GRAVITY);

		acc.u *= WATER_ACCEL;
	}
	else if (isClimbing())
	{
		gravity = Vector(0,0);
		vel.u *= CLIMB_FRICTION_X;
		vel.v *= CLIMB_FRICTION_Y;
	}
	else if (isAirborne())
	{
		acc.u *= AIR_ACCEL;
		gravity = Vector(0, AIRBORNE_GRAVITY);
		airborneTime++;
	}
	else
	{
		/* you're skidding if you're trying to move against your motion... */
		if (acc.u * vel.u <= 0 && vel.u != 0)
		{
			setFlag(IS_SKIDDING);
			skidAngle = clamp(SKID_MULTIPLIER * -vel.u, -SKID_MAX_ANGLE, SKID_MAX_ANGLE);
			acc.u *= SKID_ACCEL;
			vel.u *= SKID_FRICTION;

			float rnd = frand()*2-1;
			Vector r = Vector::perp(normal) * rnd;
			Point p = pos - normal * radius - r * size.u;

			Simulation::get().getParticles().skidDust(p, vel);
		}
		else
		{
			/* this "else" is for plain old ground moving */
			skidAngle = 0;
			acc.u *= ACCEL;
		}
	}

	/* drip if you're wet! */
	if (isWet() && !(inWater() || underWater()))
	{
		if ((wetTime % TICKS_PER_DRIP) == 0)
		{
			float rx = frand()*2-1, ry = frand()*2-1;
			Vector up = normal, right = Vector::perp(normal);
			Point p = pos + up * ry * size.v + right * rx * size.u;
			Simulation::get().getParticles().waterSplash(p, Vector(0,DRIP_GRAVITY));
		}
		wetTime--;
	}

	/* if you just pressed the jump button... */
	if (newInput & JUMP)
	{
		/* ... and you're not already jumping... */
		if (!isJumping())
		{
			/* and you're climbing a ladder, in water, on the ground,
				or just recently airborne... */
			if (isClimbing() || inWater() || inJumpBias())
			{
				/* you're jumping! */
				vel.v *= JUMP_VEL_SCALE;

				if (isAirborne())
				{
					acc.v = -AIRBORNE_JUMP_THRUST;
				}
				else
				{
					acc.u += JUMP_THRUST * normal.u;
					acc.v = JUMP_THRUST * JUMP_Y_BIAS * normal.v;
				}

				setFlag(IS_JUMPING);
				resetFlag(IS_CLIMBING);
				jumpTime = 0;
				angle = 0;
			}
		}
	}
	else if (isJumping())
	{
		gravity = Vector(0, JUMP_GRAVITY);
		jumpTime++;

		/* stop jumping if you release the jump button, you've already been jumping
			the maximum jump time, or if you're not airborne anymore (i.e. you hit the ground) */
		if (!((input & JUMP) && jumpTime <= MAX_JUMP_TIME && isAirborne()))
			resetFlag(IS_JUMPING);
	}

	vel.u = clamp(vel.u, -VEL_MAX_X, VEL_MAX_X);
	/* this VEL_MIN business is to keep from having ridiculously 
		small horizontal velocities from sliding */
	if (fabs(vel.u) < VEL_MIN) vel.u = 0;
	vel.v = clamp(vel.v, -VEL_MAX_Y, VEL_MAX_Y);

	oldPos = pos;
	vel += acc;
	pos += vel * DRAG + gravity;

	pos.x = clamp(pos.x, size.u, 640-size.u);
	pos.y = clamp(pos.y, size.v, 480-size.v);
	
	/* do collision */
	doCollision(Simulation::get().getBackground());

	/* rotate player */
	angle *= 0.90f;

	if (normalCount > 0)
	{
		float toAngle = (float)(atan2(normal.u, -normal.v) * 180.0f / 3.1415926f) + skidAngle;
		angle += clamp(toAngle - angle, -MAX_ROT, MAX_ROT);

		normalCount = 0;

		resetFlag(IS_AIRBORNE);
		airborneTime = 0;
	}
	else
	{
		setFlag(IS_AIRBORNE);
	}
}

void Player::doCollision(Background &bg)
{
	/* clear flags each frame */
	clearFlags(ON_LADDER | IN_WATER | UNDER_WATER);

	Object::doCollision(bg);

	/* do region collision */
	std::set<const Region *> set;
	const TileMap &tilemap = bg.getTileMap();

	/* find the regions we have to check */
	int l = (int)floor((pos.x-radius)/8), r = (int)ceil((pos.x+radius)/8);
	int t = (int)floor((pos.y-radius)/8), b = (int)ceil((pos.y+radius)/8);

	for (int j= t; j < b; j++)
		for (int i= l; i < r; i++)
		{
			const TileMapEntry *tme = tilemap.index(i,j);
			if (tme)
			{
				const Region *region = tme->getRegion();

				if (region)
					set.insert(set.begin(), region);
			}
		}

	/* check the regions... pretty simple stuff */
	std::set<const Region *>::iterator i;
	for (i = set.begin(); i != set.end(); ++i)
	{
		const Region &r = **i;
		switch (r.getType())
		{
		case Tile::LADDER:
			if (r.contains(pos))
				setFlag(ON_LADDER);
			break;
		case Tile::WATER:
			if (r.intersect(Circle(pos,radius)) != Region::DISJOINT)
				setFlag(IN_WATER);
			if (r.contains(pos))
			{
				setFlag(UNDER_WATER);
				wetTime = WET_TIME;
			}
			break;
		}
	}
}

void Player::preProcessWall(const Wall &w)
{
	const Edge &e = w.wall;
	const Segment &s = e.segment;

	if (e.type == Edge::LADDER_TOP)
	{
		if (s.intersect(Circle(pos, radius)))
			setFlag(ON_LADDER);
	}
}

bool Player::processWall(const Wall &w)
{
	const Edge &e = w.wall;
	const Segment &s = e.segment;

	/* Player handles some special cases:
		* you can walk on top of ladders, but if you climb down you can 
			move through the segment
		* you can move through one way blocks if you're climbing too
		* if the player circle bound intersects with a water segment, make 
			a splash
	*/

	if (e.type == Edge::LADDER_TOP)
	{
		Vector vel(oldPos, pos);
		if (Vector::dot(vel, s.normal) > 0 || isClimbing())
		{
//			printf("[WEAK]");
			ignore.push_back(&w);
			return false;
		}
		return true;
	}
	else if (e.type == Edge::ONE_WAY)
	{
		if (isClimbing())
		{
//			printf("[CLIMB]");
			ignore.push_back(&w);
			return false;
		}
	}
	else if (e.type == Edge::WATER)
	{
		/* kinda convoluted code to spray water nicely */
		Vector vel(oldPos, pos);
		float proj = (float)fabs(Vector::dot(vel, s.normal));

		/* we don't want to splash water unless were moving fast enough, with
			respect to the direction of the segment normal */
		if (proj > 0.15f)
		{
			Point p = s.closestPoint(pos);
			Vector v= s.normal * proj;
			float rnd = frand()*2-1;
			Vector r = Vector::perp(s.normal) * rnd;
			/* r is a random vector to add so the water particles don't all 
				originate from p */

			Simulation::get().getParticles().waterSplash(p + v + r * radius, v + r * 2);
		}
		/* water isn't a solid collision */
		return false;
	}

	return Object::processWall(w);
}

void Player::getInput(Uint8 *keys)
{
	oldInput = input;
	input = 0;

	if (keys[SDLK_UP]) input |= UP;
	if (keys[SDLK_DOWN]) input |= DOWN;
	if (keys[SDLK_LEFT]) input |= LEFT;
	if (keys[SDLK_RIGHT]) input |= RIGHT;
	if (keys[SDLK_SPACE]) input |= JUMP;

	/* newInput is the buttons that have been pressed this frame, but not
		last frame */
	newInput = (oldInput ^ input) & input;
}

void Player::loadImage(const char *file)
{
	if ((image = loadBMP(file)) == NULL) exit(1);
	if ((image = makeTexture(image)) == NULL) exit(1);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
}
