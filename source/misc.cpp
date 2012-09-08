/***************************************************************************
* SimFun
*  misc.cpp -- miscellaneous functions for loading bitmaps, errors, etc.
* Copyright (C) 2004	Ben Smith
*	(except where shown below)
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
#include <stdio.h>
#include <stdarg.h>
#include "SDL_opengl.h"
#include "SDL.h"
#include "misc.h"

/*  loadBMP(const char *filename)
	fixImage(SDL_Surface *src)

  taken from NeHe OpenGL tutorial ported to SDL: */

/********************************************************/
//
// This code was created by Jeff Molofee '99
// (ported to SDL by Sam Lantinga '2000)
//
// If you've found this code useful, please let me know.
//
// Visit me at www.demonews.com/hosted/nehe 
/********************************************************/

SDL_Surface *loadBMP(const char *filename)
{
    SDL_Surface *image;

    image = SDL_LoadBMP(filename);
    if ( image == NULL ) {
        ErrorBox("Unable to load %s: %s\n", filename, SDL_GetError());
        return(NULL);
    }
    return(image);
}

SDL_Surface *fixImage(SDL_Surface *src)
{
    Uint8 *rowhi, *rowlo;
    Uint8 *tmpbuf, tmpch;
	int i, j;

    /* GL surfaces are upsidedown and RGB, not BGR :-) */
    tmpbuf = (Uint8 *)malloc(src->pitch);
    if ( tmpbuf == NULL ) {
        ErrorBox("Out of memory\n");
        return(NULL);
    }

	if (SDL_MUSTLOCK(src) != 0) SDL_LockSurface(src);

    rowhi = (Uint8 *)src->pixels;
    rowlo = rowhi + (src->h * src->pitch) - src->pitch;
    for ( i=0; i<src->h/2; ++i ) {
        for ( j=0; j<src->w; ++j ) {
            tmpch = rowhi[j*3];
            rowhi[j*3] = rowhi[j*3+2];
            rowhi[j*3+2] = tmpch;
            tmpch = rowlo[j*3];
            rowlo[j*3] = rowlo[j*3+2];
            rowlo[j*3+2] = tmpch;
        }
        memcpy(tmpbuf, rowhi, src->pitch);
        memcpy(rowhi, rowlo, src->pitch);
        memcpy(rowlo, tmpbuf, src->pitch);
        rowhi += src->pitch;
        rowlo -= src->pitch;
    }

	if (SDL_MUSTLOCK(src) != 0) SDL_UnlockSurface(src);

    free(tmpbuf);

	return src;
}

SDL_Surface *makeTexture(SDL_Surface *src)
{
	SDL_Surface *dest;
	int b;
	int w, h;

	/* expand width and height to power of 2 */
	for (b= 1; b != 0; b <<= 1)
		if (src->w <= b)
		{
			w = b;
			break;
		}
	
	for (b= 1; b != 0; b <<= 1)
		if (src->h <= b)
		{
			h = b;
			break;
		}

	dest = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 24, 0xff, 0xff00, 0xff0000, 0);
	if (dest == NULL)
	{
        ErrorBox("Unable to make texture.\n");
        return(NULL);
	}

	if (SDL_BlitSurface(src, NULL, dest, NULL) != 0) return NULL;
	SDL_FreeSurface(src);
	return dest;
}

void ErrorBox(const char *format, ...)
{
	char buffer[255];
	va_list marker;
	va_start(marker, format);
	_vsnprintf(buffer, 255-1, format, marker);
	va_end(marker);
	MessageBox(NULL,buffer,"Error",MB_OK);
}

float frand()
{
	return (float)rand()/RAND_MAX;
}