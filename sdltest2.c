#include <SDL/SDL.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 320
#define HEIGHT 240

union
{
	unsigned int full;
	struct
	{
		unsigned char a;
		unsigned char b;
		unsigned char c;
		unsigned char d;
	};
} fart;
		
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
/*				makeColor(sin(fi/20)*127+128, cos(fj/20)*127+128,
						sin(sqrt(fi*fi + fj*fj)/100 + (float) SDL_GetTicks()/1000) 
							* 127 + 128),*/
				makeColor(0,0 ,
						sin(sqrt(fi*fi + fj*fj)/100 +
								(float) SDL_GetTicks()/1000*3.14) 
							* 127 + 128),
/*				makeColor(
						sin(sqrt(fi*fi + fj*fj)/100 + (float) SDL_GetTicks()/10000*3.14) 
							* 127 + 128,
						sin(sqrt(fi*fi + fj*fj)/100 + (float) SDL_GetTicks()/2000*3.14) 
							* 127 + 128,
						sin(sqrt(fi*fi + fj*fj)/100 + (float) SDL_GetTicks()/1000*3.14) 
							* 127 + 128
						), */
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
	fart.full = 0; fart.a = 0; fart.d = 128;
	printf("%u %u %u\n",fart.full,
		makeColor(255,0,0), makeColor(0,0,255));

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
	screen = SDL_SetVideoMode(WIDTH,HEIGHT,32,SDL_SWSURFACE);

	if (screen == NULL) { failures(); return 1; };

	printf("Video is set!\n");

	int next = 0;
	int cur = 0;
	int fnext = 0;
	int ftot = 0;
	int fnum = 0;
	// Do some event pumping
	while(1)
	{
		cur = SDL_GetTicks();
		if (cur >= next)
		{
			next = cur + 0;
			int start = SDL_GetTicks();
			drawinate(screen);
			int end = SDL_GetTicks();
			ftot += (end - start);
			fnum++;
			if (cur >= fnext)
			{
				fnext = cur + 1000;
				printf("Frames:%d aFPS:%2.1f\n",
					fnum,
					(float) fnum / (float) ftot * 1000);
				ftot = fnum = 0;
			}
		}

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) return 0;
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
