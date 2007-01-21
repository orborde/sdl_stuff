#ifndef FRAMECOUNT_H
#define FRAMECOUNT_H

extern int framecount;
extern int framelastcount;
extern unsigned long int framelasttime;
extern float framerate;

// set up the framecounter
// pass in current time in ms
void initFramecount(unsigned long int curtime);

// increment framecounter
void countFrame();

// compute FPS based on time difference
// pass in current time, in ms
void makeFPS(unsigned long int curtime);

// display FPS statistics to stdout
// uses results from makeFPS
void printFPS();

// increment framecount, and
// compute and display FPS every (interval) ms
// pass in current time in ms
void doFPS(unsigned long int curtime, unsigned int interval);

#endif
