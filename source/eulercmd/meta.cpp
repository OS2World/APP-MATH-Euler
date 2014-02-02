/*** meta.cpp
Metafile Stuff. This is for all versions of Euler (since 1.50).
Euler now needs a C++ compiler for all platforms.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include "meta.h"
#include "psgraph.h"

double getcolor (int i, int j);
extern int fillcolor1,fillcolor2,gscreenlines,usecolors;

double pswidth=15,psheight=15;

void Meta::write (void *l, int n)
{   if (!Active || Full) return;
	memmove(End,l,n);
	End+=n;
}

void Meta::commandwrite (int i)
{   if (!Active) return;
	if (End-Start>(int)(Size-512)) { Full=1; return; }
	*End++=i;
}

void Meta::longwrite (double n)
/***** write a double to the metafile as long
*****/
{	long k;
	k=(long)(n*1000.0);
	write(&k,sizeof(long));
}

void Meta::shortwrite (short n)
/***** write an int to the metafile
*****/
{	write(&n,sizeof(short));
}

void Meta::stringwrite (char *s)
/***** write a string to the metafile
*****/
{	write(s,strlen(s)+1);
}

int Meta::nextcommand (char * &p)
{	int k=*p; p++; return k;
}

double Meta::nextlong (char * &p)
{	long x;
	memmove(&x,p,sizeof(long));
	p+=sizeof(long);
	return x/1000.0;
}

int Meta::nextint (char * &p)
{   short n;
	memmove(&n,p,sizeof(short));
	p+=sizeof(short);
	return n;
}

int Meta::gline (void *p, double c, double r, double c1, double r1, int color,
	int st, int width)
{   if (!Active) return 1;
	if (fabs(r)>10000.0) return 0;
	if (fabs(c)>10000.0) return 0;
	if (fabs(r1)>10000.0) return 0;
	if (fabs(c1)>10000.0) return 0;
	commandwrite(10);
	longwrite(c); longwrite(r); longwrite(c1); longwrite(r1);
	shortwrite(color); shortwrite(st); shortwrite(width);
    return 1;
}

int Meta::gmarker (void *p, double c, double r, int color, int type)
{	if (!Active) return 1;
	commandwrite(20);
	longwrite(c); longwrite(r);
	shortwrite(color); shortwrite(type);
    return 1;
}

int Meta::gfill (void *p, double c[], int st, int n, int connect[])
{	if (!Active) return 1;
	int i;
	for (i=0; i<2*n; i++) if (fabs(c[i])>10000.0) return 0;
	commandwrite(30);
	shortwrite(n);
	for (i=0; i<n; i++)
	{	longwrite(c[2*i]); longwrite(c[2*i+1]);
		shortwrite(connect[i]);
	}
	shortwrite(st);
    return 1;
}

int Meta::gfillh (void *p, double c[8], double hue, int color, int connect)
{	if (!Active) return 1;
	int i;
	for (i=0; i<8; i++) if (fabs(c[i])>10000.0) return 0;
	hue-=floor(hue);
	commandwrite(31);
	for (i=0; i<8; i+=2)
	{	longwrite(c[i]); longwrite(c[i+1]);
	}
	longwrite(hue);
	shortwrite(color); shortwrite(connect);
    return 1;
}

int Meta::gbar (void *p, double c, double r, double c1, double r1, double hue,
	int color, int style)
{	if (!Active) return 1;
	commandwrite(32);
	longwrite(c); longwrite(r);
	longwrite(c1); longwrite(r1);
	longwrite(hue);
	shortwrite(color);
    shortwrite(style);
    return 1;
}

int Meta::gbar1 (void *p, double c, double r, double c1, double r1,
	int color, int style)
{	if (!Active) return 1;
	commandwrite(33);
	longwrite(c); longwrite(r);
	longwrite(c1); longwrite(r1);
	shortwrite(color);
	shortwrite(style);
    return 1;
}

int Meta::gtext (void *p, double c, double r, char *text, int color, int alignment)
{	if (!Active) return 1;
	commandwrite(40); longwrite(c);
	longwrite(r); shortwrite(color); shortwrite(alignment);
	stringwrite(text);
    return 1;
}

int Meta::gvtext (void *p, double c, double r, char *text, int color, int alignment)
{	if (!Active) return 1;
	commandwrite(41); longwrite(c);
	longwrite(r); shortwrite(color); shortwrite(alignment);
	stringwrite(text);
    return 1;
}

int Meta::gvutext (void *p, double c, double r, char *text, int color, int alignment)
{	if (!Active) return 1;
	commandwrite(42); longwrite(c);
	longwrite(r); shortwrite(color); shortwrite(alignment);
	stringwrite(text);
    return 1;
}

int Meta::gclip (void *p, double c, double r, double c1, double r1)
{
	if (!Active) return 1;
	if (fabs(r)>10000.0) return 0;
	if (fabs(c)>10000.0) return 0;
	if (fabs(r1)>10000.0) return 0;
	if (fabs(c1)>10000.0) return 0;
	commandwrite(2);
	longwrite(c);
	longwrite(r);
	longwrite(c1);
	longwrite(r1);
	return 1;
}
 
int Meta::gclear (void *p)
{   if (!Active) return 1;
	clear();
	commandwrite(1);
    return 1;
}

int Meta::gscale (void *p, double s)
{	if (!Active) return 1;
	commandwrite(50);
	longwrite(s*1000);
    return 1;
}

void Meta::replay (void *ps, int clip)
/* Redraw the graphics as noted in notespace */
{	Active=0;
	char *p=Start;
	double c,r,c1,r1,cc[16],hue;
	int col,st,width,n,i,co[16];
	while (p<End)
	{   int command=nextcommand(p);
    	switch(command)
		{	case 1 :
				gclear(p);
				break;
			case 2 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				if (clip) gclip(ps,c,r,c1,r1);
				break;
         	case 10 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				width=nextint(p);
				gline(ps,c,r,c1,r1,col,st,width);
				break;
			case 20 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gmarker(ps,c,r,col,st);
				break;
			case 30 :
				n=nextint(p);
				for (i=0; i<n; i++)
				{	cc[2*i]=nextlong(p);
					cc[2*i+1]=nextlong(p);
					co[i]=nextint(p);
				}
				st=nextint(p);
				gfill(ps,cc,st,n,co);
				break;
			case 31 :
				for (i=0; i<8; i++) cc[i]=nextlong(p);
				hue=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gfillh(ps,cc,hue,col,st);
				break;
			case 32 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				hue=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gbar(ps,c,r,c1,r1,hue,col,st);
				break;
			case 33 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gbar1(ps,c,r,c1,r1,col,st);
				break;
			case 40 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gtext(ps,c,r,p,col,st);
				p+=strlen(p)+1;
				break;
			case 41 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gvtext(ps,c,r,p,col,st);
				p+=strlen(p)+1;
				break;
			case 42 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gvutext(ps,c,r,p,col,st);
				p+=strlen(p)+1;
				break;
			case 50 :
				nextlong(p);
				break;
			default :
            	Active=1;
				return;
		}
	}
	Active=1;
}

void Meta::dump (FILE *out)
{	fwrite(Start,1,End-Start,out);
}

void Meta::postscript(FILE *out)
{	Active=0;
	char *p=Start;
	double c,r,c1,r1,cc[16],hue;
	int col,st,width,n,i,j,co[16];
	PSGraph	*ps=new PSGraph(out,1024,1024);
	for (i=0; i<16; i++)
		for (j=0; j<3; j++)
			ps->setcolor(i,getcolor(i,0),getcolor(i,1),getcolor(i,2));
	ps->setfillcolors(fillcolor1,fillcolor2);
	ps->setscreenlines(gscreenlines);
	ps->setusecolor(usecolors);
	ps->init();
	while (p<End) {
		int command=nextcommand(p);
	    	switch(command) {
			case 1 :
				ps->clear();
				break;
			case 2 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				ps->clip(c,r,c1,r1);
				break;
         	case 10 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				width=nextint(p);
				ps->line(c,r,c1,r1,col,st,width);
				break;
			case 20 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				ps->marker(c,r,col,st);
				break;
			case 30 :
				n=nextint(p);
				for (i=0; i<n; i++)
				{	cc[2*i]=nextlong(p);
					cc[2*i+1]=nextlong(p);
					co[i]=nextint(p);
				}
				st=nextint(p);
				ps->fill(cc,st,n,co);
				break;
			case 31 :
				for (i=0; i<8; i++) cc[i]=nextlong(p);
				hue=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				ps->fillh(cc,hue,col,st);
				break;
			case 32 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				hue=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				ps->bar(c,r,c1,r1,hue,col,st);
				break;
			case 33 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				ps->bar1(c,r,c1,r1,col,st);
				break;
			case 40 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				ps->text(c,r,p,col,st);
				p+=strlen(p)+1;
				break;
			case 41 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				ps->vtext(c,r,p,col,st);
				p+=strlen(p)+1;
				break;
			case 42 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				ps->vutext(c,r,p,col,st);
				p+=strlen(p)+1;
				break;
			case 50 :
				nextlong(p);
				break;
			default :
            			Active=1;
				delete ps;
				return;
		}
	}
	delete ps;
	Active=1;
}

/**************** The externally visible things ****************/
// Called from graphics, mainloop etc.

void gclear (void)
{	meta->gclear(0);
}

void gclip(double c, double r, double c1, double r1)
{
	meta->gclip(0,c,r,c1,r1);
}

void gline (double c, double r, double c1, double r1, int color,
	int st, int width)
/***** gline
	draw a line.
	col is the color, where 0 should be white and 1 black.
	st is a style from linetyp.
	width is the linewidth, where 0 or 1 are equal defaults.
*****/
{   meta->gline(0,c,r,c1,r1,color,st,width);
}

void gmarker (double c, double r, int color, int st)
/***** gmarker
	plot a single marker on screen.
	col is the color.
	type is a type from markertyp.
*****/
{   meta->gmarker(0,c,r,color,st);
}

void gfill (double c[], int st, int n, int connect[])
{	meta->gfill(0,c,st,n,connect);
}

void gfillh (double c[], double hue, int color, int connect)
/***** Draw a filled polygon.
	Works like gfill, but uses hue.
*****/
{	meta->gfillh(0,c,hue,color,connect);
}

void gbar (double c, double r, double c1, double r1, double hue,
	int color, int connect)
/***** Draw a rectangle.
	hue is a hue intensity from 0 to 1.
	style determines, if a black boundary should be drawn.
******/
{	meta->gbar(0,c,r,c1,r1,hue,color,connect);
}

void gbar1 (double c, double r, double c1, double r1,
	int color, int connect)
/***** Draw a rectangle.
	hue is a hue intensity from 0 to 1.
	style determines, if a black boundary should be drawn.
******/
{	meta->gbar1(0,c,r,c1,r1,color,connect);
}

void gtext (double c, double r, char *text, int color, int centered)
/***** gtext
	output a graphic text on screen.
	alignment is left=0, centered=1, right=2.
*****/
{	meta->gtext(0,c,r,text,color,centered);
}

void gvtext (double c, double r, char *text, int color, int centered)
/***** gvtext
	like gtext downwards
*****/
{	meta->gvtext(0,c,r,text,color,centered);
}

void gvutext (double c, double r, char *text, int color, int centered)
/***** gvutext
	like gtext upwards.
*****/
{	meta->gvutext(0,c,r,text,color,centered);
}

void gscale (double s)
{	meta->gscale(0,s);
}

void dump_meta (FILE *out)
{	meta->dump(out);
}

void dump_postscript (FILE *out)
{	meta->postscript(out);
}

void pswindow (double w, double h)
{	pswidth=w;
	psheight=h;
}

