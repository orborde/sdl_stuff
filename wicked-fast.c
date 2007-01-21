#include <SDL/SDL.h>
#include <stdio.h>
#include <math.h>
#include "framecount.h"

int WIDTH = 320;
//int HEIGHT = 240;
int HEIGHT = 300;

int magnification = 1;

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

#define PRESIN_SIZE (256*314*2)
int presinvals[PRESIN_SIZE];

void makepresin()
{
	int i;
	for (i=0; i < PRESIN_SIZE; i++)
		presinvals[i] = sin(((float) i) / PRESIN_SIZE * 3.14 * 2) * (1 << 16);
}

// hand in (angle * 256 * 100), and get out something in (-127, 127)
int presin (long int input)
{
	return (presinvals[(input % PRESIN_SIZE)] * 127) >> 16;
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
			putPixel(i,j,
				makeColor(0,0 ,
						presin( (((long int) i*i + j*j) << 8)/magnification +
								(SDL_GetTicks() << 8)*314/1000) 
							+ 128),
				screen);
		}
	}

	// unlock teh surface
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	// update!
	SDL_UpdateRect(screen, 0, 0, WIDTH, HEIGHT);
}

void graphinate( SDL_Surface * screen )
{
	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;

	int i,j;
	for (i = 0; i < WIDTH; i++)
	{
		putPixel(i,HEIGHT/2,makeColor(128,128,128),screen);
		putPixel(i,HEIGHT/2 + 127 * sin(((float) i) / WIDTH * 3.14 * 2),makeColor(0,255,0),screen);
//		printf("Graphing (%d,%d)\n",i,HEIGHT/2 + presin((i * 314 * 2) / WIDTH));
		putPixel(i,HEIGHT/2 + presin((i * 314 * 2 * 256) / WIDTH),
			makeColor(255,0,0),
			screen);
	}

/*	for (i=0; i < PRESIN_SIZE; i++)
		putPixel((i * WIDTH) / PRESIN_SIZE, HEIGHT/2 + presin(i/100), makeColor(255,0,0), screen);*/

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

	makepresin();
	printf("Populated presin table\n");

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
//			graphinate(screen);
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
