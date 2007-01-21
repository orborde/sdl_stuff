#ifndef PROTOBRUSHER_H
#define PROTOBRUSHER_H

#include <SDL/SDL.h>
#include "matrix.h"
typedef struct
{
	unsigned char R, G, B;
}
color24;

typedef struct
{
	color24 colors[3];
	int pointID[3];
}
Shaded_Triangle;

// returns 0 if brush should not be drawn due to backface culling
// returns 1 if it hasn't been eliminated
// pass in the pointer ID list and your transformed pointer list
// probably ought to be rewritten to handle raw brushes later
int visibleBrush(const int pointID[3], const point3d points[]);

void drawShadedTriangle(SDL_Surface * screen, Shaded_Triangle * targ, point3d points[]);

#endif
