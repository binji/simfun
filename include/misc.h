#ifndef __MISC_H__
#define __MISC_H__

#include "SDL.h"

SDL_Surface *loadBMP(const char *filename);
SDL_Surface *fixImage(SDL_Surface *src);
SDL_Surface *makeTexture(SDL_Surface *src);
void ErrorBox(const char *format,...);
float frand();

#endif