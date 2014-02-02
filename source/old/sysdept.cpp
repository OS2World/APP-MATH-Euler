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
#include "meta.h"

long size=512*1024l; /* default stack size in Byte */

int linelength=80; /* width of text screen */
int wchar,hchar; /* font metrics of graphic text */
int editing=0;
int fillcolor1=3,fillcolor2=11,gscreenlines=40;
int markerfactor;

int cred[16]=  {255,0,100,0  ,0  ,0  ,100,150,100,50,220 ,80  ,80  ,80  ,140 ,190};
int cgreen[16]={255,0,0  ,100,0  ,100,100,150,100,50,80  ,220 ,80  ,140 ,140 ,190};
int cblue[16]= {255,0,0  ,0  ,100,100,  0,150,100,50,80  ,80  ,220 ,140 , 80,190};

double getcolor (int i, int j)
{	switch (j)
	{	case 0 : return cred[i]/256.0;
    	case 1 : return cgreen[i]/256.0;
        case 2 : return cblue[i]/256.0;
    }
    return 0;
}

int usecolors=1;

Meta *meta;

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

int test_code (void)
/***** test_code
	see, if user pressed the keyboard.
	return the scancode, if he did.
*****/
{	return 0;
}

/**************** directory *******************/

#include <unistd.h>

char dirpath[256];

char *cd (char *dir)
/***** sets the path if dir!=0 and returns the path
*****/
{	
#ifdef CHDIR
	static char path[256];
	chdir(dir);
	if (getcwd(path,256)) return path;
	return dir;
#else
	return "";
#endif
}

#include "dirent.h"

int match (char *pat, char *s)
{	if (*pat==0) return *s==0;
	if (*pat=='*')
	{	pat++;
		if (!*pat) return 1;
		while (*s)
		{	if (match(pat,s)) return 1;
			s++;
		}
		return 0;
	}
	if (*s==0) return 0;
	if (*pat=='?') return match(pat+1,s+1);
	if (*pat!=*s) return 0;
	return match(pat+1,s+1);
}

char *dir (char *pattern)
/* if pattern==0, find next file, else find pattern.
   return 0, if there is no file.
*/
{	static DIR *dir=0;
	static struct dirent *entry;
	static char pat[256];
	if (pattern)
	{	strcpy(pat,pattern);
		if (!pat[0]) strcpy(pat,"*");
		if (dir) { closedir(dir); dir=0; }
		dir=opendir(".");
	}
	if (dir)
	{	again: entry=readdir(dir);
		if (!entry)
		{	closedir(dir);
			dir=0;
		}
		else
		{	if (match(pat,entry->d_name)) return entry->d_name;
			else goto again;
		}
	}
	return 0;
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

void graphic_mode ()
{
}

void text_mode ()
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

int main (int argc, char *argv[])
/******
Initialize memory and call main_loop
******/
{   while (argc>1 && argv[1][0]=='-')
	{	switch (argv[1][1])
		{	case 's' : /* set stacksize */
				if (argc<3) goto error;
				size=atoi(argv[2])*1024l;
				if (size<512*1024l) size=512*1024l;
				argc-=2; argv+=2;
				break;
			default :
			error :
				fprintf(stderr,
					"Use: euler [-s KB] files\n");
				return 1;
		}
	}
	meta=new Meta(4);
	if (!memory_init()) return 1;
	main_loop(argc,argv);
	return 0;
}

