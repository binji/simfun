#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include "background.h"
#include "player.h"
#include "particle.h"

class Simulation
{
/* types */
public:
	enum Flags
	{
		DRAW_TILES = 1,
		DRAW_WALLS = 2,
		DRAW_PLAYER = 4,
		DRAW_PARTICLES = 8
	};

/* consts */
private:
	static const int FRAME_RATE;

/* fields */
private:
	Background bg;
	Player player;
	Particles particles;

	int flags;

/* singleton generator */
public:
	static Simulation &get() { static Simulation s; return s; }

/* constructor */
private:
	Simulation(): flags(DRAW_TILES | DRAW_PLAYER | DRAW_PARTICLES) {}

/* methods */
private:
	void draw();
	void update();
	void loadMap(int map);

public:
	void initGraphics();
	void initData();
	void mainLoop();

/* getters */
public:
	Background & getBackground() { return bg; }
	Player & getPlayer() { return player; }
	Particles & getParticles() { return particles; }
};

#endif