   _____ _           ______
  / ____(_)         |  ____|
 | (___  _ _ __ ___ | |__ _   _ _ __
  \___ \| | '_ ` _ \|  __| | | | '_ \
  ____) | | | | | | | |  | |_| | | | |
 |_____/|_|_| |_| |_|_|   \__,_|_| |_|
 
            -- a simulation of fun --
            by Ben Smith [ bats@hush.com ]



===============================================================================
System Requirements:
===============================================================================

* runs at ~6% CPU time on my 1.6ghz Pentium M laptop
* uses approx. 7MB memory

* SDL version 1.2 or better (included)
* DirectX version 5 or better (for SDL)
* OpenGL version 1.0 or better



===============================================================================
Controls:
===============================================================================

Left Arrow      move player to the left
Right Arrow     move player to the right
Up Arrow        move player up a ladder
Down Arrow      move player down a ladder/ dive under water
Space Bar       jump

w               toggle drawing wall vectors
t               toggle drawing tiles
p               toggle drawing player
a               toggle drawing particles

1-3             load maps 1-3

escape          quit



===============================================================================
Compiling:
===============================================================================

SimFun was compiled using Microsoft Visual C++ 6.0 SP6 (MSVC) for Windows XP.
The code  has not been tested for any other compilers/operating systems/
hardware.

SimFun uses SGI's Standard Template Library (STL), downloadable from
http://www.sgi.com/tech/stl, not the MSVC built-in STL.



===============================================================================
File List:
===============================================================================

Filename                  Size (bytes)
--------------------------------------
.\
    gpl.txt                      18009
    Readme.txt                    5110
    simfun.dsp                    6869
    simfun.dsw                     535  
.\bin\
    simfun.exe                   73728
    SDL.dll                     229376
.\data\
    map1.txt                      4920
    map2.txt                      4920
    map3.txt                      4920
    SimFunPlayer.bmp              2358
    tiles.bmp                     6198
.\include\
    background.h                  1562
    circle.h                       243
    color.h                        282
    global.h                       293
    misc.h                         258
    object.h                      1216
    particle.h                    1334
    player.h                      2731
    point.h                        919
    region.h                       870
    segment.h                     1214
    simulation.h                   957
    tilemap.h                      414
    tilemapentry.h                1352
    tiles.h                       1794
    vector.h                      1876
    wall.h                         749
    walls.h                        549
    wallset.h                      433
.\source\
    background.cpp                5942
    color.cpp                     1398
    misc.cpp                      3905
    object.cpp                    8153
    particle.cpp                  6067
    player.cpp                   10938
    point.cpp                     1746
    region.cpp                    2181
    SDL_win32_main.c              8316
    segment.cpp                   5384
    simfun.cpp                    1277
    simulation.cpp                4273
    tilemap.cpp                   1634
    tiles.cpp                     6070
    vector.cpp                    1573
    walls.cpp                     9005
    wallset.cpp                   1453



===============================================================================
License:
===============================================================================

SimFun -- A Simulation of Fun
    copyright (C) 2004  Ben Smith

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

The license can be found at http://www.gnu.org/copyleft/, and in gpl.txt.
The author can be contacted at bats@hush.com.

===============================================================================
