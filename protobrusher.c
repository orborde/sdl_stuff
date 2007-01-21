#include <SDL/SDL.h>
#include "SDL_drawfunc.h"
#include "protobrusher.h"

int visibleBrush(const int pointID[3], const point3d points[])
{
	mat1x3 ab = mat_sub_1x3( &(points[pointID[1]].mat), &(points[pointID[0]].mat) );
	mat1x3 ac = mat_sub_1x3( &(points[pointID[2]].mat), &(points[pointID[0]].mat) );
	mat1x3 normal = 
		mat_cross_mult_1x3(&ab, &ac);
	return normal.val[0][2] > 0;
}

void drawShadedTriangle(SDL_Surface * screen, Shaded_Triangle * targ, point3d points[])
{
	drawGoodShadedTriangle(
		screen,
		points[targ->pointID[0]].x, points[targ->pointID[0]].y, targ->colors[0].R, targ->colors[0].G, targ->colors[0].B,
		points[targ->pointID[1]].x, points[targ->pointID[1]].y, targ->colors[1].R, targ->colors[1].G, targ->colors[1].B,
		points[targ->pointID[2]].x, points[targ->pointID[2]].y, targ->colors[2].R, targ->colors[2].G, targ->colors[2].B
	);
/*
	drawOutlineTriangle(
		screen,
		points[targ->pointID[0]].x, points[targ->pointID[0]].y,
		points[targ->pointID[1]].x, points[targ->pointID[1]].y,
		points[targ->pointID[2]].x, points[targ->pointID[2]].y,
		SDL_MapRGB(screen->format,
			(targ->colors[0].R + targ->colors[1].R + targ->colors[2].R)/3,
			(targ->colors[0].G + targ->colors[1].G + targ->colors[2].G)/3,
			(targ->colors[0].B + targ->colors[1].B + targ->colors[2].B)/3)
		);
*/
}
