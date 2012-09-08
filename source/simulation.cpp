/***************************************************************************
* SimFun
*  simulation.cpp -- singleton class that contains all data for simulation
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
#include "SDL_opengl.h"
#include "SDL.h"
#include "simulation.h"
#include "misc.h"

#define MEDIA_DIR "..\\data\\"

/* frame rate is given in ms */
const int Simulation::FRAME_RATE = 1000/60;

static const struct
{
	const char *mapName;
	float x, y;
} mapData[] =
{
	{ MEDIA_DIR "map1.txt", 270, 250 },
	{ MEDIA_DIR "map2.txt", 40, 20 },
	{ MEDIA_DIR "map3.txt", 80, 430 }
};

void Simulation::initGraphics()
{
	/* intialize sdl */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ErrorBox("Couldn't initialize SDL video.\n");
		exit(1);
	}
	SDL_WM_SetCaption("SimFun -- Simulation of Fun", NULL);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if ( SDL_SetVideoMode(640, 480, 0, SDL_OPENGL) == NULL )
	{
		ErrorBox("Couldn't initialize GL.\n");
		exit(1);
	}

	/* initialize GL */
    glViewport(0, 0, 640, 480);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,640,480,0);

	glMatrixMode(GL_MODELVIEW);
}

void Simulation::loadMap(int map)
{
	bg.loadMap(mapData[map].mapName);

	Point p(mapData[map].x, mapData[map].y);
	player.setPos(p);
	player.setOldPos(p);
}

void Simulation::initData()
{
	/* load background */
	bg.loadTiles(MEDIA_DIR "tiles.bmp");

	/* load player */
	player.loadImage(MEDIA_DIR "SimFunPlayer.bmp");

	/* load map */
	loadMap(0);
}

void Simulation::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (flags & DRAW_TILES) bg.drawTiles();
	if (flags & DRAW_WALLS) bg.drawWalls();
	if (flags & DRAW_PLAYER) player.draw();
	if (flags & DRAW_PARTICLES) particles.draw();

	glFlush();
    SDL_GL_SwapBuffers();
}

void Simulation::update()
{
	Uint8 *keys = SDL_GetKeyState(NULL);
	player.getInput(keys);
	player.update();
	particles.update();
}

void Simulation::mainLoop()
{
	bool running = true, active = true;

	while ( running )
	{
		SDL_Event event;

		if (active)
		{
			int tickStart = SDL_GetTicks(), tickEnd;

			update();
			draw();

			/* keep constant frame rate (if we're drawing too fast) */
			tickEnd = SDL_GetTicks();
			if (tickEnd - tickStart < FRAME_RATE)
				SDL_Delay(FRAME_RATE - (tickEnd - tickStart));
		}
		else
		{
			SDL_WaitEvent(&event);
			SDL_PushEvent(&event);
		}

		while ( SDL_PollEvent(&event) ) {
			switch (event.type)
			{
			case SDL_ACTIVEEVENT:
				if ( (event.active.state & (SDL_APPACTIVE | SDL_APPINPUTFOCUS)) != 0)
					active = event.active.gain == 1;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_w:
					flags ^= DRAW_WALLS;
					break;
				case SDLK_t:
					flags ^= DRAW_TILES;
					break;
				case SDLK_p:
					flags ^= DRAW_PLAYER;
					break;
				case SDLK_a:
					flags ^= DRAW_PARTICLES;
					break;
				case SDLK_1:
					loadMap(0);
					break;
				case SDLK_2:
					loadMap(1);
					break;
				case SDLK_3:
					loadMap(2);
					break;
				}
				break;
			case SDL_QUIT:
				running = false;
				break;
			}
		}
	}
}