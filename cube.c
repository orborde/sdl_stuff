#include <SDL/SDL.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "framecount.h"

#define WIDTH 320
#define HEIGHT 240

#define PER1 3000
#define PER2 10000

#define NUM_POINTS 9
point3d points[NUM_POINTS] =
{
	{{-50, -50, -50}},
	{{-50, -50, 50}},
	{{-50, 50, 50}},
	{{50, 50, 50}},
	{{50, -50, 50}},
	{{50, -50, -50}},
	{{50, 50, -50}},
	{{-50, 50, -50}},
	{{-50, -50, -50}}
};

SDL_Surface * screen;

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

void drawLineSlow(int ax, int ay, int bx, int by, Uint32 color)
{
//	printf("Line(%d,%d)-(%d,%d)\n",ax,ay,bx,by);
	int dx = bx - ax;
	int dy = by - ay;

	int e = abs(dx);
	if (abs(dy) > abs(dx))
		e = abs(dy);

	if (0 == e)
	{
		putPixel(ax,ay,color);
		return;
	}

	int i;

	for (i=0; i <= e; i++)
		putPixel( (dx * i)/e + ax, (dy * i)/e  + ay, color);
}

void drawLine(int ax, int ay, int bx, int by, Uint32 color)
{
	// X-larger variant
	if (abs(bx-ax) > abs(by-ay))
	{
	if (bx < ax)
	{
//		printf("Inverting (%d,%d)-(%d,%d)...",ax,ay,bx,by);
		int swap;
		swap = bx;
		bx = ax;
		ax = swap;
		
		swap = by;
		by = ay;
		ay = swap;
	}

	int dx = bx - ax;
	int dy = by - ay;

//	printf("LINE (%d,%d)-(%d,%d),(%d,%d)\n",ax,ay,bx,by,dx,dy);

	int m = abs(dy);
	int e = 0;

	int x, y = ay;
	if ( dy > 0 )
	{
		for ( x=ax; x <= bx; x++)
		{
			e += m;
			if (e > dx)
			{
				e -= dx;
				y++;
			}

			putPixel(x,y,color);
		}
	}
	else
	{
		for ( x=ax; x <= bx; x++)
		{
			e += m;
			if (e > dx)
			{
				e -= dx;
				y--;
			}

			putPixel(x,y,color);
		}
	}

	}	// END X-larger variant
	else
	{	// BEGIN X-larger variant
	if (by < ay)
	{
//		printf("Inverting (%d,%d)-(%d,%d)...",ax,ay,bx,by);
		int swap;
		swap = bx;
		bx = ax;
		ax = swap;
		
		swap = by;
		by = ay;
		ay = swap;
	}

	int dx = bx - ax;
	int dy = by - ay;

//	printf("LINE (%d,%d)-(%d,%d),(%d,%d)\n",ax,ay,bx,by,dx,dy);

	int m = abs(dx);
	int e = 0;

	int x = ax, y;
	if (dx > 0)
	{
		for ( y=ay; y <= by; y++)
		{
			e += m;
			if (e > dy)
			{
				e -= dy;
				x++;
			}

			putPixel(x,y,color);
		}
	}
	else
	{
		for ( y=ay; y <= by; y++)
		{
			e += m;
			if (e > dy)
			{
				e -= dy;
				x--;
			}

			putPixel(x,y,color);
		}
	}
	} // END Y-LARGER Variant

}

void superLine(int ax, int ay, int bx, int by, Uint32 color)
{
	int rowLen = (screen->pitch >> 2);

	// X-larger variant
	if (abs(bx-ax) > abs(by-ay))
	{
	if (bx < ax)
	{
//		printf("Inverting (%d,%d)-(%d,%d)...",ax,ay,bx,by);
		int swap;
		swap = bx;
		bx = ax;
		ax = swap;
		
		swap = by;
		by = ay;
		ay = swap;
	}

	int dx = bx - ax;
	int dy = by - ay;

//	printf("LINE (%d,%d)-(%d,%d),(%d,%d)\n",ax,ay,bx,by,dx,dy);

	int m = abs(dy);
	int e = 0;

	int curOffset = ay * rowLen + ax;
	int endOffset = by * rowLen + bx;

	if ( dy > 0 )
	{
		for ( ; curOffset < endOffset; curOffset++)
		{
			e += m;
			if (e > dx)
			{
				e -= dx;
				curOffset += rowLen;
			}

			((Uint32 *) screen->pixels)[curOffset] = color;
		}
	}
	else
	{

		for ( ; curOffset < endOffset; curOffset++)
		{
			e += m;
			if (e > dx)
			{
				e -= dx;
				curOffset -= rowLen;
			}

			((Uint32 *) screen->pixels)[curOffset] = color;
		}
	}

	}	// END X-larger variant

	else

	{	// BEGIN Y-larger variant
	if (by < ay)
	{
//		printf("Inverting (%d,%d)-(%d,%d)...",ax,ay,bx,by);
		int swap;
		swap = bx;
		bx = ax;
		ax = swap;
		
		swap = by;
		by = ay;
		ay = swap;
	}

	int dx = bx - ax;
	int dy = by - ay;

//	printf("LINE (%d,%d)-(%d,%d),(%d,%d)\n",ax,ay,bx,by,dx,dy);

	int m = abs(dx);
	int e = 0;

	int curOffset = ay * rowLen + ax;
	int endOffset = by * rowLen + bx;

	if (dx > 0)
	{
		for (; curOffset < endOffset; curOffset+=rowLen)
		{
			e += m;
			if (e > dy)
			{
				e -= dy;
				curOffset++;
			}

			((Uint32 *) screen->pixels)[curOffset] = color;
		}
	}
	else
	{
		for (; curOffset < endOffset; curOffset+=rowLen)
		{
			e += m;
			if (e > dy)
			{
				e -= dy;
				curOffset--;
			}

			((Uint32 *) screen->pixels)[curOffset] = color;
		}
	}
	} // END Y-LARGER Variant

}


void linetest()
{
	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;

	int i;
	Uint32 red = SDL_MapRGB(screen->format,255, 0, 0);
	Uint32 green = SDL_MapRGB(screen->format,0, 255, 0);
	Uint32 blue = SDL_MapRGB(screen->format,0, 0, 255);
	Uint32 white = SDL_MapRGB(screen->format,255, 255, 255);

	Uint32 yellow = SDL_MapRGB(screen->format, 255, 255, 0);
	Uint32 purple = SDL_MapRGB(screen->format, 0, 255, 255);
	Uint32 grey = SDL_MapRGB(screen->format, 128,128,128);
	Uint32 cyan = SDL_MapRGB(screen->format, 128, 128, 255);

	for (i = 0; i < 10; i++)
	{
/*		drawLine(WIDTH/2 + i * 10, HEIGHT/2 + 100, WIDTH/2, HEIGHT/2, red);
		drawLine(WIDTH/2 - i * 10, HEIGHT/2 + 100, WIDTH/2, HEIGHT/2, green);
		drawLine(WIDTH/2 - i * 10, HEIGHT/2 - 100, WIDTH/2, HEIGHT/2, blue);
		drawLine(WIDTH/2 + i * 10, HEIGHT/2 - 100, WIDTH/2, HEIGHT/2, white);

		drawLine(WIDTH/2 + 100, HEIGHT/2 + i*10, WIDTH/2, HEIGHT/2, yellow);
		drawLine(WIDTH/2 - 100, HEIGHT/2 + i*10, WIDTH/2, HEIGHT/2, purple);
		drawLine(WIDTH/2 - 100, HEIGHT/2 - i*10, WIDTH/2, HEIGHT/2, grey);
		drawLine(WIDTH/2 + 100, HEIGHT/2 - i*10, WIDTH/2, HEIGHT/2, cyan);*/

		drawLine(WIDTH/2, HEIGHT/2, WIDTH/2 + 100, HEIGHT/2 + i * 10, red);
	}

	for (i =0; i < 10; i++)
		drawLine(WIDTH/2, HEIGHT/2, WIDTH/2 - 100, HEIGHT/2 - i * 10, green);

/*	for (i =0; i < 10; i++)
		drawLine(WIDTH/2 - 100, HEIGHT/2 - i * 10, WIDTH/2, HEIGHT/2, blue);
*/

	// unlock teh surface
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	// update!
	SDL_UpdateRect(screen, 0, 0, WIDTH, HEIGHT);
}


void drawCircle(int ax, int ay, int r, Uint32 color)
{
	// r squared
	int rsq = r * r + 1;

	// current x and y
	int x = 0, y = r;

	// x/y squared
	int xsq = x*x, ysq = y*y;

	int tot = xsq + ysq;
	while(x < y)
	{
		// draw stuff
		putPixel(ax - x, ay - y, color);
		putPixel(ax - y, ay - x, color);
		putPixel(ax + x, ay + y, color);
		putPixel(ax + y, ay + x, color);

		putPixel(ax + x, ay - y, color);
		putPixel(ax + y, ay - x, color);
		putPixel(ax - x, ay + y, color);
		putPixel(ax - y, ay + x, color);


		// are we outside the circle? oh teh noes!
		if (tot > rsq)
		{
			tot -= (y << 1) - 1;
			y--;
		}

		// recalc x and xsq FAST
		tot += (x << 1) + 1;
		x++;
	}

	// draw final pixels
	putPixel(ax - x, ay - y, color);
	putPixel(ax - y, ay - x, color);
	putPixel(ax + x, ay + y, color);
	putPixel(ax + y, ay + x, color);

	putPixel(ax + x, ay - y, color);
	putPixel(ax + y, ay - x, color);
	putPixel(ax - x, ay + y, color);
	putPixel(ax - y, ay + x, color);

//	printf("CIRCLE:x=y?%d\n",x == y);
}

void render(Uint32 curtime)
{
	static Uint32 lasttime = 0;

/*	int disp = 0;
	if (curtime >= lasttime + 1000)
	{
		disp = 1;
		lasttime = curtime;
	}

	if(disp)
		printf("time=%u\n");*/

	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;

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

	point3d temp[NUM_POINTS];
	for(i=0; i < NUM_POINTS; i++)
	{
		temp[i].mat = mat_mult_1x3_3x3(&(points[i].mat), &rot);
//		if(disp) printf("%d=(%f,%f,%f)\n", i, temp.x, temp.y, temp.z);
	}

	// clear screen
	SDL_FillRect(screen, NULL, 0);

	// draw linage and circlage
/*	for (i=1; i<5; i++)
	{
		Uint32 color = SDL_MapRGB(screen->format, (255 * i)/NUM_POINTS, 255 - (255 * i)/NUM_POINTS, 0);
		superLine(temp[i-1].x + WIDTH/2,temp[i-1].y + HEIGHT/2,
			temp[i].x + WIDTH/2,temp[i].y + HEIGHT/2,
//			SDL_MapRGB(screen->format, (curtime/100)%128+128, 0, 0));
//			SDL_MapRGB(screen->format, 255, 0, 0));
			color);
		drawCircle( temp[i].x + WIDTH/2, temp[i].y + HEIGHT/2, 10, color);
	}

	for (i=5; i<NUM_POINTS; i++)
	{
		Uint32 color = SDL_MapRGB(screen->format, (255 * i)/NUM_POINTS, 255 - (255 * i)/NUM_POINTS, 0);
		drawLine(temp[i-1].x + WIDTH/2,temp[i-1].y + HEIGHT/2,
			temp[i].x + WIDTH/2,temp[i].y + HEIGHT/2,
//			SDL_MapRGB(screen->format, (curtime/100)%128+128, 0, 0));
//			SDL_MapRGB(screen->format, 255, 0, 0));
			color);
		drawCircle( temp[i].x + WIDTH/2, temp[i].y + HEIGHT/2, 10, color);
	} */

	for (i=1; i<NUM_POINTS; i++)
	{
		Uint32 color = SDL_MapRGB(screen->format, (255 * i)/NUM_POINTS, 255 - (255 * i)/NUM_POINTS, 0);
		drawLine(temp[i-1].x + WIDTH/2,temp[i-1].y + HEIGHT/2,
			temp[i].x + WIDTH/2,temp[i].y + HEIGHT/2,
//			SDL_MapRGB(screen->format, (curtime/100)%128+128, 0, 0));
//			SDL_MapRGB(screen->format, 255, 0, 0));
			color);
		drawCircle( temp[i].x + WIDTH/2, temp[i].y + HEIGHT/2, 10, color);
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

	printf("Entering main loop\n");

	initFramecount(SDL_GetTicks());

//	int q = 1500;
	// main loop!

//		linetest();

	while(1)
	{
		Uint32 curtime = SDL_GetTicks();
		doFPS(curtime, 1000);

//		SDL_Delay(1000);

		// main render
		render(curtime);

/*		printf("Next up:%d\n",q);
		render(q);
		q++;*/

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
			} // switch

		} // poll

	} // main loop
	return 0;
}
