#include <SDL/SDL.h>
#include <stdio.h>

#define WIDTH 320
#define HEIGHT 240

#define NUM_TRAILS 100
#define TRAIL_SIZE 50

// all coordinates are specified in x.8 fixed-point from here on out,
// so REMEMBER TO CONVERT THEM!
#define ORIG_X ((WIDTH / 2) << 8)
#define ORIG_Y ((HEIGHT / 2 - HEIGHT / 6) << 8)

#define ORIG_VX (400 << 8)
#define ORIG_VY (400 << 8)

// this is in pixels/sec^2
#define GRAVITY (400 << 8)

#define FPS 1000
// delay time between frames in msec
#define DELAY (1000 / FPS)

SDL_Surface * screen;

typedef struct
{
	int x,y;	// x.8 fixed
} trailpart;

typedef struct
{
	int x,y;	// x.8 fixed
	int vx,vy;	// x.8 fixed, pixels/second
	trailpart trail[TRAIL_SIZE];
	int trailpos;
} shooter;

// main array of shooters
shooter shooters[NUM_TRAILS];

// array of shooter colors
Uint32 colormap[TRAIL_SIZE];

// some basic colors, defined once we have a screen set up
Uint32 white, black;

int inbounds(int x, int y)
{
	return (x < WIDTH && y < HEIGHT && x >= 0 && y >= 0);
}

void putPixel(int x, int y, Uint32 color)
{
//	printf(" %x@%d,%d", color, x, y);
	unsigned int offset =
		x + (screen->pitch >> 2) * y;
	((Uint32 *) screen->pixels)[offset] = color;
}

void init_shooter(shooter * targ)
{
//	printf("(re-init)");
	targ->x = ORIG_X;
	targ->y = ORIG_Y;
	targ->vx = ((int) rand() % ORIG_VX - (ORIG_VX >> 1));
	targ->vy = -((int) rand() % ORIG_VY/* - (ORIG_VY >> 1)*/);
}

// time is passed in milliseconds
void do_shooter(shooter * targ, int time)
{
	// generic counter
	int i;

/*	printf(" dt=%u o:s=(%d,%d),v=(%d,%d)", time,
		targ->x, targ->y,
		targ->vx, targ->vy);*/

	// is current position valid?
	// if not, re-init
	if ( !inbounds(targ->x >> 8, targ->y >> 8) ) init_shooter(targ);

/*	// okay, shuffle everything down the stack
	for (i = TRAIL_SIZE - 1; i > 0; i--)
		targ->trail[i] = targ->trail[i-1];
	// put current on the stack
	targ->trail[0].x = targ->x;
	targ->trail[0].y = targ->y;*/

	// rotate the stack pointer
	targ->trailpos--;
	if (targ->trailpos < 0) targ->trailpos += TRAIL_SIZE;
	// put current on the stack
	targ->trail[targ->trailpos].x = targ->x;
	targ->trail[targ->trailpos].y = targ->y;

	int j;
	// redraw everything
	for (i = targ->trailpos, j = 0; i < TRAIL_SIZE; i++, j++)
		putPixel(targ->trail[i].x >> 8, targ->trail[i].y >> 8, colormap[j]);
	for (i = 0; i < targ->trailpos; i++, j++)
		putPixel(targ->trail[i].x >> 8, targ->trail[i].y >> 8, colormap[j]);

	// recalculate current position and velocity
	int cx = targ->x;
	int cy = targ->y;
	int vx = targ->vx;
	int vy = targ->vy;

	targ->x = cx + (vx * time) / 1000;
	targ->y = cy + (vy * time) / 1000 + (GRAVITY * (time * time)) / (2000000);
//                                                                       ^ 1/2 and msec->sec in one

	targ->vy = vy + (GRAVITY * time) / 1000;

	// done!	colormap[TRAIL_SIZE - 2] = SDL_MapRGB(screen->format, 255,0,0);

/*	printf(" n:s=(%d,%d),v=(%d,%d)",
		targ->x, targ->y,
		targ->vx, targ->vy);*/
}

void render(Uint32 time)
{
	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;

	// run and render the shooters
	int i;
	for (i = 0; i < NUM_TRAILS; i++)
	{
//		printf("\nID:%d",i);
		do_shooter(shooters + i, time);
	}

	// unlock teh surface
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	// update!
	SDL_UpdateRect(screen, 0, 0, WIDTH, HEIGHT);
}

void framecount()
{
	static Uint32 nextPrintTime = 1000, lastPrintTime = 0, frameCount = 0;
	Uint32 curTime = SDL_GetTicks();

	frameCount++;

	if (curTime >= nextPrintTime)
	{
		printf("%u frames, FPS=%0.2f/sec\n",
			frameCount,
			(float) frameCount / (float) (nextPrintTime - lastPrintTime) * 1000);
		lastPrintTime = curTime;
		nextPrintTime = curTime + 5000;
		frameCount = 0;
	}
}

void framethrottle()
{
	static Uint32 lastRenderTime = 0;
	Uint32 cur = SDL_GetTicks();
	Uint32 toWait = lastRenderTime + DELAY - cur;
	if (lastRenderTime + DELAY < cur ) toWait = 0;
	SDL_Delay(toWait);
	lastRenderTime = SDL_GetTicks();
}
int main ()
{
	// handy-dandy generic variables
	int i,j;

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


	// initialize the basic colors
	Uint32 white = SDL_MapRGB(screen->format, 255, 255, 255);
	Uint32 black = SDL_MapRGB(screen->format, 0, 0, 0);

	// populate the color array
	for(i = 0; i < TRAIL_SIZE; i++)
		colormap[i] = SDL_MapRGB(screen->format,
			(255 * i) / TRAIL_SIZE,
			(255 * i) / TRAIL_SIZE,
			255);
	// this is to erase the tail end
	colormap[TRAIL_SIZE - 1] = black;

	// init all the shooters and whatnot
	for (i = 0; i < NUM_TRAILS; i++)
	{
		init_shooter(shooters + i);
		shooters[i].trailpos = 0;
/*		Let's have some fun and not do this for now
		for (j = 0; j < TRAIL_SIZE; j++)
		{
			shooters[i].trail[i].x = shooters[i].x;
			shooters[i].trail[i].y = shooters[i].y;
		}*/
	}


	printf("Entering main loop\n");
	Uint32 last_time = SDL_GetTicks();
	// main loop!
	while(1)
	{
		// main render
		Uint32 cur_time = SDL_GetTicks();
		render(cur_time - last_time);
		last_time = cur_time;

		framecount();

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

		framethrottle();
	} // main loop
	return 0;
}
