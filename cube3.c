#include <SDL/SDL.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "framecount.h"
#include "SDL_drawfunc.h"
#include "cube3_brusher.h"

const float pi = 3.14159265358979323846264338327950288419716939937510;

#define WIDTH 320
#define HEIGHT 240

// set this to nonzero to limit the output to 50FPS
#define FRAMETHROTTLE 1
// (Because, as cool as 400FPS is, this program is not designed to fry eggs.)

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

// main rotation matrix
mat3x3 rot =
{{
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 0, 1 }
}};

void render(Uint32 curtime)
{
	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;

	// rendering stuff here
	int i;

	// mouse coordinate change
	int mousedx = 0 , mousedy = 0;
	// last left mouse button state
	static Uint8 last_button_state = 0;

	// read in the current mouse state
	SDL_PumpEvents();
	Uint8 cur_button_state = SDL_GetRelativeMouseState(&mousedx, &mousedy) & SDL_BUTTON(1);
//	if (cur_button_state)
//		printf("LOL MOUSE IS PRESSED\n");

	// if the button is pressed, AND the mouse has moved, let's recalculate the
	// rotation matrix
	if (last_button_state && cur_button_state && (mousedx || mousedy))
	{
		// change in angles about the x and y axes
		// 1 degree per pixel
		// though these are in RADIANS

		// this is the rotation around the X AXIS
		// this corresponds to VERTICAL (y) motion
		// inverted as a kludgey way of correcting apparently incorrect drawing
		float angx = -mousedy * pi / 180;

		// this is the rotation around the Y AXIS
		// this corresponds to HORIZONTAL (x) motion
		float angy = mousedx * pi / 180;

		// generate rotation matrix around x axis
		mat3x3 rotx =
		{{
			{1, 0, 0},
			{0, cos(angx), sin(angx) },
			{0, sin(angx), -cos(angx) }
		}};

		// generate rotation matrix around y axis
		mat3x3 roty =
		{{
			{cos(angy), 0, sin(angy)},
			{0, 1, 0},
			{sin(angy), 0, -cos(angy)}
		}};

		// create combined rotation matrix
		mat3x3 deltarot = mat_mult_3x3_3x3(&rotx,&roty);

		// reset main rotation matrix
		rot = mat_mult_3x3_3x3(&rot, &deltarot);
	}

	// log new button state
	last_button_state = cur_button_state;

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

	int halted = 0;
	// main loop!
	while(1)
	{
		// main render
		Uint32 curtime = SDL_GetTicks();
		if (!halted)
		{
			// Hey, let's not even bother doing anything unless we're holding the
			// mouse button down!
			if (FRAMETHROTTLE)
				{ if (SDL_GetRelativeMouseState(NULL, NULL) & SDL_BUTTON(1)) render(curtime); }
			else
				render(curtime);

			doFPS(curtime, 1000);
		}

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

		if (FRAMETHROTTLE)
		{
			// throttle the video output to 50FPS
			Uint32 newtime = SDL_GetTicks();
			if (newtime < curtime + 20)
				SDL_Delay(curtime + 20 - newtime);
		}

	} // main loop
	return 0;
}
