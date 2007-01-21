#ifndef SDL_DRAWFUNC_H
#define SDL_DRAWFUNC_H

#include <SDL/SDL.h>

void putPixel(SDL_Surface * screen, int x, int y, Uint32 color);

void drawLine(SDL_Surface * screen, int ax, int ay, int bx, int by, Uint32 color);

void drawCircle(SDL_Surface * screen, int ax, int ay, int r, Uint32 color);

void drawOutlineTriangle(
	SDL_Surface * screen,
	int ax, int ay,
	int bx, int by,
	int cx, int cy,
	Uint32 color
);

/* Draws a fairly crappy (pixel gaps) shaded triangle using a simple but awful
   algorithm */
void drawSlowShadedTriangle(
	SDL_Surface * screen,
	int ax, int ay, int aR, int aG, int aB,
	int bx, int by, int bR, int bG, int bB,
	int cx, int cy, int cR, int cG, int cB);

/* Draws a nice, no-pixel-gap triangle with shading and everything.
   Seems to have a minor bug in the color gradient */
void drawGoodShadedTriangle(
	SDL_Surface * screen,
	int ax, int ay, int aR, int aG, int aB,
	int bx, int by, int bR, int bG, int bB,
	int cx, int cy, int cR, int cG, int cB);

/* Combines the bug-free (but slow)
   gradient fill of Slow with the pixel-gap-less-ness of
   Good. Used to try to figure out what the deal is with Good. 
   EXCEPT that it exhibits the same "jaggy" behavior as Good, so I'm not sure
   what's going on, really. */
void drawSlowGoodShadedTriangle(
	SDL_Surface * screen,
	int ax, int ay, int aR, int aG, int aB,
	int bx, int by, int bR, int bG, int bB,
	int cx, int cy, int cR, int cG, int cB);
#endif // header protection
