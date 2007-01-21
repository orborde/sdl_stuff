#include <SDL/SDL.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "framecount.h"
#include "SDL_drawfunc.h"
#include "protobrusher.h"

#define WIDTH 320
#define HEIGHT 240

// represents center of view window
const point3d center = {{WIDTH/2, HEIGHT/2, 0}};

// ms per half rotation
#define PER1 3000
#define PER2 10000

// global screen variable
SDL_Surface * screen = NULL;

// point data
#define NUM_POINTS 8
point3d points[NUM_POINTS] =
{
	{{-50, -50, -50}},
	{{-50, -50, 50}},
	{{-50, 50, 50}},
	{{50, 50, 50}},
	{{50, -50, 50}},
	{{50, -50, -50}},
	{{50, 50, -50}},
	{{-50, 50, -50}}
};

// brush data
#define NUM_BRUSHES 12
Shaded_Triangle brushes[NUM_BRUSHES] =
{
	{
		{{255,0,0}, {0,0,0}, {0,0,255}},
		{1, 2, 3}
	},
	{
		{{255, 0, 0}, {0, 0, 255}, {255,0,255}},
		{1, 3, 4 }
	},
	{
		{{255,255,0}, {255,255,255}, {0,255,255}},
		{0, 5, 6 }
	},
	{
		{{255,255,0}, {0,255,255}, {0,255,0}},
		{ 0, 6, 7}
	},
	{
		{{255,0,255}, {0,0,255}, {0,255,255}},
		{ 4, 3, 6 }
	},
	{
		{{255,0,255}, {0,255,255}, {255,255,255}},
		{ 4, 6, 5 }
	},
	{
		{{255,0,0}, {255,255,0}, {0,0,0}},
		{ 1, 0, 2 }
	},
	{
		{{0,0,0}, {255,255,0}, {0,255,0}},
		{ 2, 0, 7 }
	},
	{
		{{255,0,0}, {255,0,255}, {255,255,255}},
		{ 1, 4, 5 }
	},
	{
		{{255,0,0}, {255,255,255}, {255,255,0}},
		{ 1, 5, 0 }
	},
	{
		{{0,255,0}, {0,255,255}, {0,0,255}},
		{ 7, 6, 3 }
	},
	{
		{{0,255,0}, {0,0,255}, {0,0,0}},
		{ 7, 3, 2 }
	}
};

void render(Uint32 curtime)
{
	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;

	// rendering stuff here
	int i;
	mat3x3 rot1 = {{
		{cos((float) curtime/PER1*3.14), sin((float) curtime/PER1*3.14), 0},
		{sin((float) curtime/PER1*3.14), -cos((float) curtime/PER1*3.14), 0},
		{0, 0, 1}
	}};

	mat3x3 rot2 = {{
		{cos((float) curtime/PER2*3.14), 0, sin((float) curtime/PER2*3.14)},
		{0, 1, 0},
		{sin((float) curtime/PER2*3.14), 0, -cos((float) curtime/PER2*3.14)}
	}};

	mat3x3 rot = mat_mult_3x3_3x3(&rot1,&rot2);

/*	printf("\nMAT:");
	int j;
	for (i=0; i<3; i++) for(j=0; j<3; j++) printf("%f ",rot.val[i][j]);*/

	point3d transformed_points[NUM_POINTS];
	for(i=0; i < NUM_POINTS; i++)
	{
		transformed_points[i].mat = mat_mult_1x3_3x3(&(points[i].mat), &rot);
		transformed_points[i].mat = mat_add_1x3(&(transformed_points[i].mat), &(center.mat));
	}

	// clear screen
	SDL_FillRect(screen, NULL, 0);

	// render the brushes
	for(i=0; i < NUM_BRUSHES; i++)
	{
//		printf("Now drawing brush %d\n",i);
		// perform backface culling
		if (visibleBrush(brushes[i].pointID, transformed_points))
			drawShadedTriangle(screen, &brushes[i], transformed_points);
	};

	// draw some nice lines and circles
/*	for (i=1; i<NUM_POINTS; i++)
	{
		Uint32 color = SDL_MapRGB(screen->format, (255 * i)/NUM_POINTS, 255 - (255 * i)/NUM_POINTS, 0);
		drawLine(transformed_points[i-1].x,transformed_points[i-1].y,
			transformed_points[i].x ,transformed_points[i].y,
			color, screen);
		drawCircle( transformed_points[i].x, transformed_points[i].y, 10, color, screen);
	}*/

	// deprecated test pattern
/*	drawSlowShadedTriangle(
		WIDTH/2 - 50, HEIGHT/2, 255, 0, 0,
		WIDTH/2 - 50, HEIGHT/2 - 50, 0, 255, 0,
		WIDTH/2, HEIGHT/2, 0, 0, 255,
		screen);*/

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

	// initialize framecounter
	initFramecount(SDL_GetTicks());

	printf("Entering main loop\n");
	Uint32 last_time = SDL_GetTicks();
	int halted = 0;
	// main loop!
	while(1)
	{
		// main render
		Uint32 curtime = SDL_GetTicks();
		if (!halted)
		{
			render(curtime);
			doFPS(curtime, 1000);
		}

		// prevents processor cooking for the moment
		// while 200FPS is cool to see numbers-wise, it is imperceptible to humans
		// so we'll reduce the CPU load this way
		//		SDL_Delay(8);

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
						case SDLK_p:
							halted = !halted;
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
