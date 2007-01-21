CFLAGS = -O2 -Werror -pg -g
LDFLAGS=-lm -lSDL

all : wicked wicked-fc wicked-fast cube particles cube2 mousedl cube3

clean :
	rm framecount.o matrix.o protobrusher.o SDL_drawfunc.o cube3_brusher.o

framecount.o : framecount.h framecount.c
	gcc -c framecount.c $(CFLAGS) 

matrix.o : matrix.h matrix.c
	gcc -c matrix.c $(CFLAGS) 

protobrusher.o : SDL_drawfunc.h matrix.h protobrusher.h protobrusher.c
	gcc -c protobrusher.c $(CFLAGS)

cube3_brusher.o : SDL_drawfunc.h matrix.h cube3_brusher.c cube3_brusher.h
	gcc -c cube3_brusher.c $(CFLAGS)

SDL_drawfunc.o : SDL_drawfunc.h SDL_drawfunc.c
	gcc -c SDL_drawfunc.c $(CFLAGS) 

wicked : wicked.c
	gcc wicked.c $(CFLAGS) $(LDFLAGS) -o wicked

wicked-fc : wicked-fc.c framecount.o
	gcc wicked-fc.c framecount.o -lSDL $(CFLAGS) -o wicked-fc

wicked-fast : wicked-fast.c framecount.o
	gcc wicked-fast.c framecount.o -lSDL $(CFLAGS) -o wicked-fast

cube : matrix.o framecount.o cube.c
	gcc cube.c matrix.o framecount.o -lSDL $(CFLAGS) -o cube

particles : particles.c
	gcc particles.c -lSDL $(CFLAGS) -o particles

cube2 : matrix.o framecount.o SDL_drawfunc.o protobrusher.o cube2.c
	gcc cube2.c matrix.o framecount.o SDL_drawfunc.o protobrusher.o -lSDL $(CFLAGS) -o cube2

mousedl : SDL_drawfunc.o framecount.o mousedl.c
	gcc mousedl.c SDL_drawfunc.o framecount.o -lSDL $(CFLAGS) -o mousedl

cube3 : matrix.o framecount.o SDL_drawfunc.o cube3_brusher.o cube3.c
	gcc cube3.c matrix.o framecount.o SDL_drawfunc.o cube3_brusher.o -lSDL $(CFLAGS) -o cube3
