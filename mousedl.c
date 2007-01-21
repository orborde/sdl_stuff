#include <SDL/SDL.h>
#include <stdio.h>
#include "SDL_drawfunc.h"
#include "framecount.h"

#define WIDTH 320
#define HEIGHT 240

SDL_Surface * screen;

typedef struct
{
	int ax, ay;
	int bx, by;
	Uint32 color;
}
Line;

#define NUM_LINES 100
Line lines[NUM_LINES];

int inbounds(int x, int y)
{
	return (x < WIDTH && y < HEIGHT && x >= 0 && y >= 0);
}

void render(Uint32 time)
{
	int i;

	// next time at which to force a capture
	static Uint32 nexttime = 0;

	// next index in lines[] to write to
	static int curlinepos = 0;
	static int lastx = WIDTH/2, lasty = HEIGHT/2;

	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;

	// grab current mouse state
	int cx, cy;
	SDL_PumpEvents();
	SDL_GetMouseState(&cx, &cy);

	// if the mouse moved, capture it
	// or if it's time for an update, force a capture
	if(lastx != cx || lasty != cy || time >= nexttime)
	{
		// reset the timer
		nexttime = time + 100;

		// write into the array
		lines[curlinepos].ax = lastx;
		lines[curlinepos].ay = lasty;
		lines[curlinepos].bx = cx;
		lines[curlinepos].by = cy;
		lines[curlinepos].color = rand();

		// update last coords
		lastx = cx;
		lasty = cy;

		// increment and keep in bounds the counter
		curlinepos = (curlinepos + 1) % NUM_LINES;
	}

	// fill the screen with black
	SDL_FillRect(screen, NULL, 0);

	// draw the nice lines
	for (i = 0; i < NUM_LINES; i++)
		drawLine(screen, lines[i].ax, lines[i].ay, lines[i].bx, lines[i].by, lines[i].color);

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
	{ printf("Video init failed\n"); return 1; };

	// reg SDL_Quit to be called at exit to clean up
	atexit(SDL_Quit);

	printf("SDL is up and running!\n");

	// make me a screen worthy of Mordor
	screen = SDL_SetVideoMode(WIDTH,HEIGHT,32,SDL_SWSURFACE);
	if (screen == NULL) { printf("Video modeset failed\n"); return 1; };
	printf("Video is set!\n");

	// initialize line array to black, black, and more black
	Line temp = {WIDTH/2, HEIGHT/2, WIDTH/2, HEIGHT/2, 0};
	int i;
	for (i=0; i < NUM_LINES; i++)
		lines[i] = temp;

	initFramecount(SDL_GetTicks());
	printf("Entering main loop\n");
	// main loop!
	while(1)
	{
		// main render
		Uint32 curtime = SDL_GetTicks();
		render(curtime);
		doFPS(curtime, 5500);
//		SDL_Delay(5);

		// handle events
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
					}
					break;
				case SDL_QUIT:
					return 0;
					break;
			}
		} // poll

	} // main loop
	return 0;
}
