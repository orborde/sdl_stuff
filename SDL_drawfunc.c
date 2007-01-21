#include <SDL/SDL.h>
#include <math.h>
#include <stdio.h>
#include "SDL_drawfunc.h"

// messy declaration
#define max(a,b) ((a) > (b) ? (a) : (b))

void putPixel(SDL_Surface * screen, int x, int y, Uint32 color)
{
//	printf("pixel:%x@%d,%d\n", color, x, y);
	unsigned int offset =
		x + (screen->pitch >> 2) * y;
	((Uint32 *) screen->pixels)[offset] = color;
}

void drawLine(SDL_Surface * screen, int ax, int ay, int bx, int by, Uint32 color)
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
		for ( x=ax; x < bx; x++)
		{
			e += m;
			if (e > dx)
			{
				e -= dx;
				y++;
			}

			putPixel(screen,x,y,color);
		}
	}
	else
	{
		for ( x=ax; x < bx; x++)
		{
			e += m;
			if (e > dx)
			{
				e -= dx;
				y--;
			}

			putPixel(screen,x,y,color);
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
		for ( y=ay; y < by; y++)
		{
			e += m;
			if (e > dy)
			{
				e -= dy;
				x++;
			}

			putPixel(screen,x,y,color);
		}
	}
	else
	{
		for ( y=ay; y < by; y++)
		{
			e += m;
			if (e > dy)
			{
				e -= dy;
				x--;
			}

			putPixel(screen,x,y,color);
		}
	}
	} // END Y-LARGER Variant

}

void drawCircle(SDL_Surface * screen, int ax, int ay, int r, Uint32 color)
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
		putPixel(screen, ax - x, ay - y, color);
		putPixel(screen, ax - y, ay - x, color);
		putPixel(screen, ax + x, ay + y, color);
		putPixel(screen, ax + y, ay + x, color);

		putPixel(screen, ax + x, ay - y, color);
		putPixel(screen, ax + y, ay - x, color);
		putPixel(screen, ax - x, ay + y, color);
		putPixel(screen, ax - y, ay + x, color);


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
	putPixel(screen, ax - x, ay - y, color);
	putPixel(screen, ax - y, ay - x, color);
	putPixel(screen, ax + x, ay + y, color);
	putPixel(screen, ax + y, ay + x, color);

	putPixel(screen, ax + x, ay - y, color);
	putPixel(screen, ax + y, ay - x, color);
	putPixel(screen, ax - x, ay + y, color);
	putPixel(screen, ax - y, ay + x, color);

//	printf("CIRCLE:x=y?%d\n",x == y);
}

void drawOutlineTriangle(
	SDL_Surface * screen, 
	int ax, int ay,
	int bx, int by,
	int cx, int cy,
	Uint32 color
)
{
	drawLine(screen, ax,ay,bx,by,color);
	drawLine(screen, bx,by,cx,cy,color);
	drawLine(screen, cx,cy,ax,ay,color);
}

void drawSlowShadedTriangle(
	SDL_Surface * screen,
	int ax, int ay, int aR, int aG, int aB,
	int bx, int by, int bR, int bG, int bB,
	int cx, int cy, int cR, int cG, int cB)
{
	// number of "vertical" lines to draw
	int abmax = max(abs(bx-ax),abs(by-ay));
	int acmax = max(abs(cx-ax),abs(cy-ay));
	int imax = max( abmax, acmax );

//	printf("Trianglize (%d,%d)-(%d,%d)-(%d,%d) imax=%d\n", ax, ay, bx, by, cx, cy, imax);

	// index toward above
	int i;
	for (i = 0; i <= imax; i++)
	{
		// ab-line current position - top
		int abx = ax + ((bx - ax) * i) / imax;
		int aby = ay + ((by - ay) * i) / imax;
		int abR = aR + ((bR - aR) * i) / imax;
		int abG = aG + ((bG - aG) * i) / imax;
		int abB = aB + ((bB - aB) * i) / imax;

		// ac-line current position - bottom
		int acx = ax + ((cx - ax) * i) / imax;
		int acy = ay + ((cy - ay) * i) / imax;
		int acR = aR + ((cR - aR) * i) / imax;
		int acG = aG + ((cG - aG) * i) / imax;
		int acB = aB + ((cB - aB) * i) / imax;

//		printf("element %d (%d,%d)-(%d,%d) \n", i, abx, aby, acx, acy);
		// number of line elements to draw
		int jmax = max(abs(acx-abx), abs(acy-aby));

//		printf("(%d/%d) ", i, jmax);

		// index toward above
		int j;

		if ( jmax > 0)
		for (j = 0; j <= jmax; j++)
			putPixel(
				screen,
				abx + ((acx - abx) * j) / jmax,
				aby + ((acy - aby) * j) / jmax,
				SDL_MapRGB(screen->format,
					abR + ((acR - abR) * j) / jmax,
					abG + ((acG - abG) * j) / jmax,
					abB + ((acB - abB) * j) / jmax
				)
			);
	}

//	printf("Trianglize done\n");
}

void drawGoodShadedTriangle(
	SDL_Surface * screen,
	int ax, int ay, int aR, int aG, int aB,
	int bx, int by, int bR, int bG, int bB,
	int cx, int cy, int cR, int cG, int cB)
{

  /*  
  //debug
  //  aR=bR=cR=0;
  aG=bG=cG=0;
  aB=bB=cB=0;
  */  
  
	// first, resort the points into order based on x values
	// This could probably be optimized. A lot.
	int sx, sy, sR, sG, sB;

	if (ax > bx)
	{
		// swap a and b
		sx = ax; sy = ay; sR = aR; sG = aG; sB = aB;
		ax = bx; ay = by; aR = bR; aG = bG; aB = bB;
		bx = sx; by = sy; bR = sR; bG = sG; bB = sB;
	}

	if (bx > cx)
	{
		// swap b and c
		sx = bx; sy = by; sR = bR; sG = bG; sB = bB;
		bx = cx; by = cy; bR = cR; bG = cG; bB = cB;
		cx = sx; cy = sy; cR = sR; cG = sG; cB = sB;
	}

	if (ax > bx)
	{
		// swap a and b
		sx = ax; sy = ay; sR = aR; sG = aG; sB = aB;
		ax = bx; ay = by; aR = bR; aG = bG; aB = bB;
		bx = sx; by = sy; bR = sR; bG = sG; bB = sB;
	}

//	printf("GT:(%d,%d)-(%d,%d)-(%d,%d)\n", ax,ay,bx,by,cx,cy);

#if 0
	// this should be safe to remove
       	if ( !(ax <= bx && bx <= cx) )
		printf("GoodTriangle reports bad sort!\nCheck your sort algo, tardpants!\n");
#endif


	// current x value
	int x;
	
	// counter for colors and y values for ac-line
	int i = 0;
	// value of i at which cutoff happens
	int imax = abs(cx - ax);

	// counter for colors and y values for ab/bc-set
	int j = 0;
	// value of j at which cutoff happens
	int jmax = abs(bx - ax);

	// check for imax=0, just for giggles
	if (imax == 0)
		printf("Uh oh. imax == 0!\n");

	// draw left half
	for ( x = ax; x < bx; x++, i++, j++ )
	{
		// protection from j == 0 should not be necessary here
		// but we'll leave this in to test it
		if (jmax == 0)
			printf("jmax == 0! Brace for segfault!\n");

		// calculate current y and color values
		int acy = ay + ((cy - ay) * i) / imax;
		int aby = ay + ((by - ay) * j) / jmax;

		int acR = aR + ((cR - aR) * i) / imax;
		int acG = aG + ((cG - aG) * i) / imax;
		int acB = aB + ((cB - aB) * i) / imax;

		int abR = aR + ((bR - aR) * j) / jmax;
		int abG = aG + ((bG - aG) * j) / jmax;
		int abB = aB + ((bB - aB) * j) / jmax;

//		printf("Column:(%d,%d)-(%d,%d)\n", x, aby, x, acy);

		// counter for shading the column
		int k;
		// max value for above
		int kmax = abs(acy - aby);
		// protect against 0
		if (kmax == 0)
			kmax = 1;

		// current y value
		int y;

		// so which way are we drawing this column?
		// down?
		if (acy > aby)
		{
			for (y = aby, k = 0; y <= acy; y++, k++)
			{
				putPixel(screen,
					x,y,
					SDL_MapRGB(screen->format,
						abR + ((acR - abR) * k) / kmax,
						abG + ((acG - abG) * k) / kmax,
						abB + ((acB - abB) * k) / kmax
					)
				);
			}
		}
		// or up?
		else
		{
			for (y = aby, k = 0; y >= acy; y--, k++)
			{
				putPixel(screen,
					x,y,
					SDL_MapRGB(screen->format,
						abR + ((acR - abR) * k) / kmax,
						abG + ((acG - abG) * k) / kmax,
						abB + ((acB - abB) * k) / kmax
					)
				);
			}
		}
	} // end x loop

//goto stuff;
//	printf("Begin right hand\n");

	// reset j for bc set
	j = 0;
	jmax = abs(cx - bx);
	// protect against j==0
	if (jmax == 0)
		jmax = 1;

	// draw right half
	for ( x = bx; x <= cx; x++, i++, j++ )
	{
		// this really should be safe to remove
		// makes sure that bx=0 does not draw a column
//		if (x == cx) printf("LOL x matches cx!!!\n");

		// calculate current y and color values
		int acy = ay + ((cy - ay) * i) / imax;
		int bcy = by + ((cy - by) * j) / jmax;

		int acR = aR + ((cR - aR) * i) / imax;
		int acG = aG + ((cG - aG) * i) / imax;
		int acB = aB + ((cB - aB) * i) / imax;

		int bcR = bR + ((cR - bR) * j) / jmax;
		int bcG = bG + ((cG - bG) * j) / jmax;
		int bcB = bB + ((cB - bB) * j) / jmax;

//		printf("Column:(%d,%d)-(%d,%d)\n", x, acy, x, bcy);

		// counter for shading the column
		int k;
		// max value for above
		int kmax = abs(bcy - acy);
		// protect against 0
		if (kmax == 0)
			kmax = 1;

		// current y value
		int y;

		// so which way are we drawing this column?
		// down?
		if (bcy > acy)
		{
			for (y = acy, k = 0; y <= bcy; y++, k++)
			{
				putPixel(screen,
					x,y,
					SDL_MapRGB(screen->format,
						acR + ((bcR - acR) * k) / kmax,
						acG + ((bcG - acG) * k) / kmax,
						acB + ((bcB - acB) * k) / kmax
					)
				);
			}
		}
		// or up?
		else
		{
			for (y = acy, k = 0; y >= bcy; y--, k++)
			{
				putPixel(screen,
					x,y,
					SDL_MapRGB(screen->format,
						acR + ((bcR - acR) * k) / kmax,
						acG + ((bcG - acG) * k) / kmax,
						acB + ((bcB - acB) * k) / kmax
					)
				);
			}
		}
	} // end x loop

//stuff:
//	printf("Trianglized!\n");
}


void drawSlowGoodShadedTriangle(
	SDL_Surface * screen,
	int ax, int ay, int aR, int aG, int aB,
	int bx, int by, int bR, int bG, int bB,
	int cx, int cy, int cR, int cG, int cB)
{

  /*  
  //debug
  //  aR=bR=cR=0;
  aG=bG=cG=0;
  aB=bB=cB=0;
  */  
  
	// first, resort the points into order based on x values
	// This could probably be optimized. A lot.
	int sx, sy, sR, sG, sB;

	if (ax > bx)
	{
		// swap a and b
		sx = ax; sy = ay; sR = aR; sG = aG; sB = aB;
		ax = bx; ay = by; aR = bR; aG = bG; aB = bB;
		bx = sx; by = sy; bR = sR; bG = sG; bB = sB;
	}

	if (bx > cx)
	{
		// swap b and c
		sx = bx; sy = by; sR = bR; sG = bG; sB = bB;
		bx = cx; by = cy; bR = cR; bG = cG; bB = cB;
		cx = sx; cy = sy; cR = sR; cG = sG; cB = sB;
	}

	if (ax > bx)
	{
		// swap a and b
		sx = ax; sy = ay; sR = aR; sG = aG; sB = aB;
		ax = bx; ay = by; aR = bR; aG = bG; aB = bB;
		bx = sx; by = sy; bR = sR; bG = sG; bB = sB;
	}

//	printf("GT:(%d,%d)-(%d,%d)-(%d,%d)\n", ax,ay,bx,by,cx,cy);

#if 0
	// this should be safe to remove
       	if ( !(ax <= bx && bx <= cx) )
		printf("GoodTriangle reports bad sort!\nCheck your sort algo, tardpants!\n");
#endif


	// current x value
	int x;
	
	// counter for colors and y values for ac-line
	int i = 0;
	// value of i at which cutoff happens
	int imax = abs(cx - ax);

	// counter for colors and y values for ab/bc-set
	int j = 0;
	// value of j at which cutoff happens
	int jmax = abs(bx - ax);

	// check for imax=0, just for giggles
	if (imax == 0)
	  {
	    printf("%d:Uh oh. imax == 0!\n", __LINE__);
	    printf("I am a coward and am not going to render this triangle\n");
	    return;
	  }

	// draw left half
	for ( x = ax; x < bx; x++, i++, j++ )
	{
		// protection from j == 0 should not be necessary here
		// but we'll leave this in to test it
		if (jmax == 0)
			printf("jmax == 0! Brace for segfault!\n");

		// calculate current y value
		int acy = ay + ((cy - ay) * i) / imax;
		int aby = ay + ((by - ay) * j) / jmax;

		/* calculate partial color values (divide by imax/jmax in the
		   vertical loop - this is to keep as much precision as
		   possible) */
		int acR = aR * imax + ((cR - aR) * i);
		int acG = aG * imax + ((cG - aG) * i);
		int acB = aB * imax + ((cB - aB) * i);

		int abR = aR * jmax + ((bR - aR) * j);
		int abG = aG * jmax + ((bG - aG) * j);
		int abB = aB * jmax + ((bB - aB) * j);

//		printf("Column:(%d,%d)-(%d,%d)\n", x, aby, x, acy);

		// counter for shading the column
		int k;
		// max value for above
		int kmax = abs(acy - aby);
		// protect against 0
		if (kmax == 0)
			kmax = 1;

		// current y value
		int y;

		// so which way are we drawing this column?
		// down?
		if (acy > aby)
		{
		  for (y = aby, k = 0; y <= acy; y++, k++)
		    {
		      // Calculate the current pixel values
		      /* Since ac needs to be divided by imax, and ab by jmax,
			 and since we want to divide them off at the end,
			 we multiply ac by jmax (so the jmax term cancels when
			 applied to the ac terms)
			 and
			 multiply ab by imax (to cancel the imax term).
			 Then we can divide the final result by (imax * jmax)
			 and everyone is happy. :) 
			 Also, multiply that leftmost term (before the +) by
			 kmax so kmax cancels.
			 Algebra FTW */
		      int R = (abR * imax * kmax +
			       ((acR * jmax - abR * imax) * k)) 
			/ (kmax * imax * jmax);
		      int G = (abG * imax * kmax +
			       ((acG * jmax - abG * imax) * k)) 
			/ (kmax * imax * jmax);
		      int B = (abB * imax * kmax +
			       ((acB * jmax - abB * imax) * k)) 
			/ (kmax * imax * jmax);
		      putPixel(screen,
			       x,y,
			       SDL_MapRGB(screen->format,
					  R, G, B
					  )
			       );
		    }
		}
		// or up?
		else
		{
		  for (y = aby, k = 0; y >= acy; y--, k++)
		    {
		      // Calculate the current pixel values
		      /* Since ac needs to be divided by imax, and ab by jmax,
			 and since we want to divide them off at the end,
			 we multiply ac by jmax (so the jmax term cancels when
			 applied to the ac terms)
			 and
			 multiply ab by imax (to cancel the imax term).
			 Then we can divide the final result by (imax * jmax)
			 and everyone is happy. :) 
			 Also, multiply that leftmost term (before the +) by
			 kmax so kmax cancels.
			 Algebra FTW */
		      int R = (abR * imax * kmax +
			       ((acR * jmax - abR * imax) * k)) 
			/ (kmax * imax * jmax);
		      int G = (abG * imax * kmax +
			       ((acG * jmax - abG * imax) * k)) 
			/ (kmax * imax * jmax);
		      int B = (abB * imax * kmax +
			       ((acB * jmax - abB * imax) * k)) 
			/ (kmax * imax * jmax);
		      putPixel(screen,
			       x,y,
			       SDL_MapRGB(screen->format,
					  R, G, B
					  )
			       );
		    }
		}
	} // end x loop

//goto stuff;
//	printf("Begin right hand\n");

	// reset j for bc set
	j = 0;
	jmax = abs(cx - bx);
	// protect against j==0
	if (jmax == 0)
		jmax = 1;

	// draw right half
	for ( x = bx; x <= cx; x++, i++, j++ )
	{
		// this really should be safe to remove
		// makes sure that bx=0 does not draw a column
//		if (x == cx) printf("LOL x matches cx!!!\n");

		// calculate current y
		int acy = ay + ((cy - ay) * i) / imax;
		int bcy = by + ((cy - by) * j) / jmax;

		/* calculate partial color values (divide by imax/jmax in the
		   vertical loop - this is to keep as much precision as
		   possible) */
		int acR = aR * imax + ((cR - aR) * i);
		int acG = aG * imax + ((cG - aG) * i);
		int acB = aB * imax + ((cB - aB) * i);

		int bcR = bR * jmax + ((cR - bR) * j);
		int bcG = bG * jmax + ((cG - bG) * j);
		int bcB = bB * jmax + ((cB - bB) * j);

//		printf("Column:(%d,%d)-(%d,%d)\n", x, acy, x, bcy);

		// counter for shading the column
		int k;
		// max value for above
		int kmax = abs(bcy - acy);
		// protect against 0
		if (kmax == 0)
			kmax = 1;

		// current y value
		int y;

		// so which way are we drawing this column?
		// down?
		if (bcy > acy)
		{
			for (y = acy, k = 0; y <= bcy; y++, k++)
			{
		      // Calculate the current pixel values
		      /* Since bc needs to be divided by imax, and ac by jmax,
			 and since we want to divide them off at the end,
			 we multiply bc by jmax (so the jmax term cancels when
			 applied to the bc terms)
			 and
			 multiply ac by imax (to cancel the imax term).
			 Then we can divide the final result by (imax * jmax)
			 and everyone is happy. :) 
			 Also, multiply that leftmost term (before the +) by
			 kmax so kmax cancels.
			 Algebra FTW */
		      int R = (acR * jmax * kmax +
			       ((bcR * imax - acR * jmax) * k)) 
			/ (kmax * imax * jmax);
		      int G = (acG * jmax * kmax +
			       ((bcG * imax - acG * jmax) * k)) 
			/ (kmax * imax * jmax);
		      int B = (acB * jmax * kmax +
			       ((bcB * imax - acB * jmax) * k)) 
			/ (kmax * imax * jmax);
		      putPixel(screen,
			       x,y,
			       SDL_MapRGB(screen->format,
					  R, G, B
					  )
			       );
			}
		}
		// or up?
		else
		{
			for (y = acy, k = 0; y >= bcy; y--, k++)
			{
		      // Calculate the current pixel values
		      /* Since bc needs to be divided by imax, and ac by jmax,
			 and since we want to divide them off at the end,
			 we multiply bc by jmax (so the jmax term cancels when
			 applied to the bc terms)
			 and
			 multiply ac by imax (to cancel the imax term).
			 Then we can divide the final result by (imax * jmax)
			 and everyone is happy. :) 
			 Also, multiply that leftmost term (before the +) by
			 kmax so kmax cancels.
			 Algebra FTW */
		      int R = (acR * jmax * kmax +
			       ((bcR * imax - acR * jmax) * k)) 
			/ (kmax * imax * jmax);
		      int G = (acG * jmax * kmax +
			       ((bcG * imax - acG * jmax) * k)) 
			/ (kmax * imax * jmax);
		      int B = (acB * jmax * kmax +
			       ((bcB * imax - acB * jmax) * k)) 
			/ (kmax * imax * jmax);
		      putPixel(screen,
			       x,y,
			       SDL_MapRGB(screen->format,
					  R, G, B
					  )
			       );
			}
		}
	} // end x loop

//stuff:
//	printf("Trianglized!\n");
}
