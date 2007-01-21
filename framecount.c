#include <stdio.h>
#include "framecount.h"

int framecount;
int framelastcount;
unsigned long int framelasttime;
float framerate;

void initFramecount(unsigned long int curtime)
{
	framelasttime = curtime;
	framecount = 0;
	framerate = 0;
}

void countFrame()
{
	framecount++;
}

void makeFPS(unsigned long int curtime)
{
	unsigned long int timediff = curtime - framelasttime;
	if (0 == timediff)
	{
		// sampling is too fast!
		// leave statistics untouched
		return;
	}

	framerate = (float) framecount / timediff * 1000;
	framelastcount = framecount;
	framecount = 0;
	framelasttime = curtime;
}

void printFPS()
{
	printf("FPS=%.2f over %d frames\n", framerate, framelastcount);
}

void doFPS(unsigned long int curtime, unsigned int interval)
{
	countFrame();
	if (curtime >= framelasttime + interval)
	{
		makeFPS(curtime);
		printFPS();
	}
}
