/* Primitive user interface for EULER.
Missing are:
	- graphics
	- single keystrokes (e.g. for tracing)
	- extended command line editing and command history
	- wait does not work, but waits for a keystroke
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>

char outputs[256];

#include "header.h"
#include "sysdep.h"
#include "euler.h"

long size=512*1024l; /* default stack size in Byte */

int linelength=80; /* width of text screen */
int wchar,hchar; /* font metrics of graphic text */
int editing=0;

int memory_init (void)
/***** memory_init
	get memory for stack.
*****/
{	ramstart=(char *)malloc(size);
	if (!ramstart) return 0;
	ramend=ramstart+size;
	return 1;
}

int shrink (size_t size)
/**** allows shrinking of memory for single task systems.
simply return 1 if you do not support this or set NOSHRINK in funcs.c
*****/
{	return 1;
}

/******************** writing the meta file ************/

void longwrite (double n)
/***** write a double to the metafile as long
*****/
{	long k[1];
	k[0]=(long)(n*1000.0);
	if (metafile) fwrite(k,sizeof(long),1,metafile);
}

void intwrite (int n)
/***** write an int to the metafile
*****/
{	int k[1];
	k[0]=n;
	if (metafile) fwrite(k,sizeof(int),1,metafile);
}

void stringwrite (char *s)
/***** write a string to the metafile
*****/
{	int c;
	if (metafile)
	{	c=0;
		while (*s) { putc(*s++,metafile); c++; }
		putc(0,metafile); c++;
		if (c%2) putc(0,metafile);
	}
}

/******************* Graphics ***************************/

/****
The graphic screen has coordinates from 0.0 to 1024.0 (double).
There should be a function, which computes the correct screen
coordinates from these internal ones.
****/

void clear_graphics (void)
/***** clear the graphics screen 
*****/
{	if (metafile) intwrite(6);
}

void graphic_mode (void)
/***** graphics
	Switch to graphics. Text must not be deleted.
	On a window system make graphics visible.
*****/
{
}

void text_mode (void)
/***** text_mode
	Switch to text. Graphics should not be deleted.
	On a window system make text visible.
*****/
{
}

void gline (double c, double r, double c1, double r1, int col,
	int st, int width)
/***** gline
	draw a line.
	col is the color, where 0 should be white and 1 black.
	st is a style from linetyp.
	width is the linewidth, where 0 or 1 are equal defaults.
*****/
{	if (fabs(r)>10000.0) return;
	if (fabs(c)>10000.0) return;
	if (fabs(r1)>10000.0) return;
	if (fabs(c1)>10000.0) return;
	if (metafile)
	{	intwrite(1);
		longwrite(c); longwrite(r); longwrite(c1); longwrite(r1);
		intwrite(col); intwrite(st); intwrite(width);
	}
}

void gmarker (double c, double r, int col, int type)
/***** gmarker
	plot a single marker on screen.
	col is the color.
	type is a type from markertyp.
*****/
{	if (metafile)
	{	intwrite(2);
		longwrite(c); longwrite(r);
		intwrite(col); intwrite(type);
	}
}

void gfill (double c[], int st, int n, int connect[])
/***** gfill
	fill an area given by n pairs of points (in c: x,y,x,y,...)
	with the style st from filltyp.
	connect pairs of points indicated in connect by a black line.
*****/
{	int i;
	for (i=0; i<2*n; i++) if (fabs(c[i])>10000.0) return;
	if (metafile)
	{	intwrite(3);
		for (i=0; i<2*n; i+=2)
		{	longwrite(c[i]); longwrite(c[i+1]);
			intwrite(connect[i]);
		}
		intwrite(st);
	}
}

void gtext (double c, double r, char *text, int color, int alignment)
/***** gtext
	output a graphic text on screen.
	alignment is left=0, centered=1, right=2.
*****/
{	if (metafile)
	{	intwrite(4); longwrite(c);
		longwrite(r); intwrite(color); intwrite(alignment);
		stringwrite(text);
	}
}

void scale (double s)
/***** scale
	scale the screen according s = true width / true height.
	This is not necessary on a window based system.
*****/
{	if (metafile)
	{	intwrite(5);
		longwrite((int)(s*1000));
	}
}

void gbar (double c, double r, double c1, double r1, double hue,
	int color, int connect)
/***** Draw a rectangle.
	hue is a hue intensity from 0 to 1.
	connect determines, if a black boundary should be drawn.
******/
{	if (metafile)
	{	intwrite(7);
		longwrite(c); longwrite(r);
		longwrite(c1); longwrite(r1);
		longwrite((hue-floor(hue))*1000.0); intwrite(color);
	}
}

void gfillh (double c[8], double hue, int color, int connect)
/***** Draw a filled polygon.
	Works like gfill, but uses hue.
*****/
{	int i;
	for (i=0; i<8; i++) if (fabs(c[i])>10000.0) return;
	if (metafile)
	{	intwrite(8);
		for (i=0; i<8; i+=2)
		{	longwrite(c[i]); longwrite(c[i+1]);
		}
		longwrite((hue-floor(hue))*1000.0);
		intwrite(color); intwrite(connect);
	}
}

/*********************** input routines **********************/

void mouse (double *x, double *y)
/****** mouse
	wait, until the user marked a screen point with the mouse.
	Return screen coordinates.
******/
{	*x=0; *y=0;
}

int wait_key (int *scan)
/***** 
	wait for a keystroke. return the scancode and the ascii code.
	scancode should be a code from scantyp. Do at least generate
	'enter'.
*****/
{	int c;
	c=getchar();
	switch (c)
	{	case '\n' : *scan=enter; break;
		default : *scan=0;
	}
	return c;
}

int test_key (void)
/***** test_key
	see, if user pressed the keyboard.
	return the scancode, if he did.
*****/
{	return 0;
}

/**************** directory *******************/

char dirpath[256];

char *cd (char *dir)
/***** sets the path if dir!=0 and returns the path
*****/
{	return "";
}

char *dir (char *pattern)
/***** Search a pattern if not 0, else research 
******/
{	return "";
}

/***************** clock and wait ********************/

double myclock (void)
/***** define a timer in seconds. 
******/
{	return (double)clock()/CLK_TCK;
}

void sys_wait (double time, int *scan)
/***** Wait for time seconds or until a key press.
Return the scan code or 0 (time exceeded).
******/
{	int c=getchar();
	printf(" *** Press Return\n");
	switch(c)
	{	case 27 : *scan=escape; break;
		default : *scan=enter;
	}
}

/***************** div. ***********************************/

void getpixelsize (double *x, double *y)
/***** Compute the size of pixel in screen coordinates.
******/
{	*x=1;
	*y=1;
}

void gflush (void)
/***** Flush out remaining graphic commands (for multitasking).
This serves to synchronize the graphics on multitasking systems.
******/
{
}

/**************** Text screen ***************************/

void clear_screen (void)
/***** Clear the text screen 
******/
{
}

/********************** Text output ***************************/

/****
The following text screen commands should be emulated on a graphic
work station. This can be done by a standard emulator (e.g. VT100)
or within a window displaying the text. Additional features may be
added, such as viewing old text. But the input line should be
visible as soon as a key is pressed by the user.
****/

void move_cl (void)
/* move the text cursor left */
{
}

void move_cr (void)
/* move the text cursor right */
{
}

void cursor_on (void)
/* switch cursor on */
{
}

void cursor_off (void)
/* switch cursor off */
{
}

void clear_eol (void)
/* clear the text line from cursor position */
{
}

void gprint (char *s)
/*****
Print a line onto the text screen, parse tabs and '\n'.
Printing should be done at the cursor position. There is no need
to clear the line at a '\n'.
The cursor should move forward with the print.
Think of the function as a simple emulator.
If you have a line buffered input with echo then do not print,
when the command line is on.
*****/
{	if (!editing) printf("%s",s);
}

void edit_off (void)
/* the command line is no longer in use (graphics or computing) */
{	editing=0;
}

void edit_on (void)
/* the command line is active */
{	editing=1;
}

/********** execute programs *************************/

int execute (char *name, char *args)
/**** execute
	Call an external program, return 0, if there was no error.
	No need to support this on multitasking systems.
****/
{	return 0;
}

/*************** main ********************************/

/* Necessary for not printing an error message on math errors */
int _RTLENTRY _matherr (struct _exception  *__e)
{	return 1;
}

int main (int argc, char *argv[])
/******
Initialize memory and call main_loop
******/
{	if (!memory_init()) return 1;
	_control87(MCW_EM,MCW_EM);
	main_loop(argc,argv);
	return 0;
}

