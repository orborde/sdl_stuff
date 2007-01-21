#include <SDL/SDL.h>
#include <stdio.h>


#define WIDTH 640
#define HEIGHT 480

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

	// lock teh surface
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0)
			{ failures(); return 1; };
	int i,j;
	for (i = 0; i < WIDTH; i++)
	{
		for (j = 0; j < HEIGHT; j++)
		{
			putPixel(i,j,
				makeColor(i % 256, j % 256,
						(i+j) % 256),
					screen);
		}
	}

	// unlock teh surface
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	// update!
	SDL_UpdateRect(screen, 0, 0, WIDTH, HEIGHT);


	// Do some event pumping
	while(1)
	{
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
				default:
					printf("Something happened?\n");
			}
		}	// event poller
	}	// main loop

	return 0;
}
