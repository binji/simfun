#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <stdlib.h>
#include "SDL_opengl.h"
#include "SDL.h"
#include "object.h"

class Background;

class Player : public Object
{
/* types */
public:
	enum PlayerFlags
	{
		IS_AIRBORNE = 1,
		IS_JUMPING = 2,
		IS_SKIDDING = 4,
		IS_CLIMBING = 8,
		ON_LADDER = 16,
		IN_WATER = 32,
		UNDER_WATER = 64,
	};

	enum InputFlags
	{
		LEFT = 1,
		RIGHT = 2,
		UP = 4,
		DOWN = 8,
		JUMP = 16,
	};

private:

/* fields */
private:
	static const float DRAG;
	static const float GRAVITY;
	static const float AIRBORNE_GRAVITY;
	static const float WATER_GRAVITY;
	static const float JUMP_GRAVITY;
	static const float DRIP_GRAVITY;
	static const float ACCEL;
	static const float AIR_ACCEL;
	static const float SKID_ACCEL;
	static const float WATER_ACCEL;
	static const float WATER_THRUST;
	static const float SKID_MULTIPLIER;
	static const float SKID_MAX_ANGLE;
	static const float SKID_FRICTION;
	static const float CLIMB_FRICTION_X;
	static const float CLIMB_FRICTION_Y;
	static const float JUMP_VEL_SCALE;
	static const float JUMP_THRUST;
	static const float JUMP_Y_BIAS;
	static const float AIRBORNE_JUMP_THRUST;
	static const float VEL_MAX_X;
	static const float VEL_MAX_Y;
	static const float VEL_MIN;
	static const float MAX_ROT;
	static const int MAX_JUMP_TIME;
	static const int TICKS_PER_DRIP;
	static const int JUMP_MAX_AIRBORNE_TIME;
	static const int WET_TIME;

private:
	SDL_Surface *image;
	GLuint texture;

	float angle, skidAngle;
	unsigned int flags;
	int jumpTime, airborneTime, wetTime;

	int input, oldInput, newInput;

/* constructors */
public:
	Player();

/* methods */
public:
	void draw();
	void update();
	void doCollision(Background &bg);
	void preProcessWall(const Wall &w);
	bool processWall(const Wall &w);
	void getInput(Uint8 *keys);
	void loadImage(const char *file);

/* setters */
public:
	void clearFlags(int _flags) { flags &= ~_flags; }
	void setFlags(int mask, int flag) { flags = (flags & mask) | flag; }
	void setFlag(int flag) { flags |= flag; }
	void resetFlag(int flag) { flags &= ~flag; }

/* getters */
public:
	bool inJumpBias()	{ return airborneTime < JUMP_MAX_AIRBORNE_TIME; }
	bool isAirborne()	{ return ((flags & IS_AIRBORNE) != 0); }
	bool isJumping()	{ return ((flags & IS_JUMPING) != 0); }
	bool isSkidding()	{ return ((flags & IS_SKIDDING) != 0); }
	bool isClimbing()	{ return ((flags & IS_CLIMBING) != 0); }
	bool onLadder()		{ return ((flags & ON_LADDER) != 0); }
	bool inWater()		{ return ((flags & IN_WATER) != 0); }
	bool underWater()	{ return ((flags & UNDER_WATER) != 0); }
	bool isWet()		{ return wetTime > 0; }
};

#endif
