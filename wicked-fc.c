#include <SDL/SDL.h>
#include <stdio.h>
#include <math.h>
#include "framecount.h"

int WIDTH = 320;
int HEIGHT = 240;

float magnification = 1;

void failures ()
{
	printf("SDL ARE FAILURED\nALL YOUR SDL ARE BELONG TO US\n");
}

unsigned int makeColor(
	unsigned char r, unsigned char g, unsigned char b)
{
	return (((unsigned int) r) << 16) |
		(((unsigned int) g) << 8) |
		((unsigned int) b);
}

void putPixel(int x, int y, unsigned int color,
	SDL_Surface * screen)
{
	unsigned int offset =
		x + (screen->pitch >> 2) * y;
	((unsigned int *) screen->pixels)[offset] = color;
}

void drawinate( SDL_Surface * screen )
{
	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;

	int i,j;
	for (i = 0; i < WIDTH; i++)
	{
		for (j = 0; j < HEIGHT; j++)
		{
			float fi = i;
			float fj = j;
			putPixel(i,j,
				makeColor(0,0 ,
						sin((fi*fi + fj*fj)/magnification +
								(float) SDL_GetTicks()/1000*3.14) 
							* 127 + 128),
				screen);
		}
	}

	// unlock teh surface
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	// update!
	SDL_UpdateRect(screen, 0, 0, WIDTH, HEIGHT);
}

int main ()
{
	// init video, check for success
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		failures();
		return 1;
	}

	// reg SDL_Quit to be called at exit to clean up
	atexit(SDL_Quit);

	printf("SDL is up and running!\n");


	SDL_Surface * screen;

	// make me a screen worthy of Mordor
	screen = SDL_SetVideoMode(WIDTH,HEIGHT,32,SDL_SWSURFACE | SDL_RESIZABLE);

	if (screen == NULL) { failures(); return 1; };

	printf("Video is set!\n");

	int next = 0;
	int cur = 0;
	int fnext = 1000;

	initFramecount(SDL_GetTicks());

	// Do some event pumping
	while(1)
	{
		cur = SDL_GetTicks();
		if (cur >= next)
		{
			next = cur + 0;
			drawinate(screen);
			countFrame();
			if (cur >= fnext)
			{
				fnext = cur + 1000;
				makeFPS(cur);
				printFPS();
			}
		}

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							return 0;
							break;
						case SDLK_UP:
						case SDLK_PLUS:
						case SDLK_EQUALS:
							magnification *= 2;
							break;
						case SDLK_DOWN:
						case SDLK_MINUS:
							magnification /= 2;
							break;
					}
					break;
				case SDL_VIDEORESIZE:
//					printf("Resized from %dx%d ", WIDTH, HEIGHT);
					WIDTH = event.resize.w;
					HEIGHT = event.resize.h;
//					printf("to %dx%d\n", WIDTH, HEIGHT);
					screen = SDL_SetVideoMode(WIDTH,HEIGHT,32,SDL_SWSURFACE | SDL_RESIZABLE);
					break;
				case SDL_QUIT:
					return 0;
					break;
//				default:
//					printf(".");
			}
		}	// event poller
	}	// main loop

	return 0;
}
