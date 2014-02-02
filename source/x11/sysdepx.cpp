#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/times.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h> /* for chdir and getcwd */

#ifdef RS6000
#include <sys/time.h>
void gettimer (int, struct timestruc_t *);
#endif

#define MAXPLOT 100000.0

#define WINDOW

char outputs[256];

#include "../header.h"
#include "../sysdep.h"
#include "../help.h"
#include "../meta.h"

#include "rotated.h"

char *graph_screen=0,*text_screen=0;
int graphscr=1,colors,wscreen,hscreen;
int linelength,wchar,hchar,wchart,hchart,in_text=1;
int userbreak=0;
int planes=0x01;
unsigned long mask;

#define TEXTSIZE (128*1024l)

char textstart[TEXTSIZE]={0};

int maxlines,cx,cy;
int textheight,textwidth,textoffset;
int cursoron=1,thchar,twchar,editor=0,scrolled=0;
char *textend=textstart,*textwindow=textstart,*oldtextwindow;
int markerfactor=100;

long memsize=1024*1024l;

double nextcheck;

/**************************************************************/

class XMeta : public Meta
{	public :
	XMeta (int size=32) : Meta(size) {}
	virtual int gclear (void *p);
	virtual int gline (void *p, double c, double r, double c1, double r1,
    	int color, int st, int width);
	virtual int gmarker (void *p, double c, double r, int color, int st);
	virtual int gfill (void *p, double c[], int st, int n, int connect[]);
	virtual int gfillh (void *p, double c[], double hue,
    	int color, int connect);
	virtual int gbar (void *p, double c, double r, double c1, double r1,
    	double hue,	int color, int connect);
	virtual int gbar1 (void *p, double c, double r, double c1, double r1,
		int color, int connect);
	virtual int gtext (void *p, double c, double r, char *text,
    	int color, int centered);
	virtual int gvtext (void *p, double c, double r, char *text,
    	int color, int centered);
	virtual int gvutext (void *p, double c, double r, char *text,
    	int color, int centered);
	virtual int gscale (void *p, double s);
};

XMeta *meta;

/*************** XWindow things ******************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#define maxcolors 16

Display *display;
char sdisp[64]="";
int screen, depth;
int winit=0;
Window window;
GC gc,cleargc,stipplegc,textgc,invtextgc,greygc;
Pixmap stipple[64],greystipple;

// Color management

unsigned long blackpixel, whitepixel;
Colormap colormap;
int usedcolors=maxcolors;
unsigned long color[maxcolors];

int cred[16]=  {255,0,100,0  ,0  ,0  ,100,150,100,50,220 ,80  ,80  ,80  ,140 ,190};
int cgreen[16]={255,0,0  ,100,0  ,100,100,150,100,50,80  ,220 ,80  ,140 ,140 ,190};
int cblue[16]= {255,0,0  ,0  ,100,100,  0,150,100,50,80  ,80  ,220 ,140 , 80,190};
	
static int red_bits;
static int green_bits;
static int blue_bits;
static int red_shift;
static int green_shift;
static int blue_shift;
static unsigned long red_mask;
static unsigned long green_mask;
static unsigned long blue_mask;

char *colorname[maxcolors]=
{
	"White","Black","DarkGreen","DarkBlue","Blue","SteelBlue","Khaki","Tan",
	"Grey","Yellow","Green","Red","LightBlue","LightSteelBlue",
	"LimeGreen","Navy"
};
XFontStruct *font,*textfont;
Pixmap pixmap;

char fontname[256]=GFONT;
char textfontname[256]=FONT;
int userwidth=700,userheight=700,userx=0,usery=0,usersize=0,userpos=0;
int gscreenlines=40;

int bw=0;

#include "../icon.h"

void process_event (XEvent *event);

unsigned long rgb2index(short red, short green, short blue)
{
	XColor rgb;
	
	if (depth > 8)
	{
		red >>= (8 - red_bits);
		green >>= (8 - green_bits);
		blue >>= (8 - blue_bits);

		return ((red << red_shift) & red_mask) | ((green << green_shift) & green_mask) | ((blue << blue_shift) & blue_mask);
	}
	
	rgb.red = red << 8;
	rgb.green = green << 8;
	rgb.blue = blue << 8;
	rgb.flags = DoRed|DoGreen|DoBlue;
	if(XAllocColor(display,colormap,&rgb))
		return rgb.pixel;
	return blackpixel;
}

void computechar (void)
{
	hchar=(font->ascent+font->descent+2)*1024l/hscreen;
	wchar=(XTextWidth(font,"m",1))*1024l/wscreen;
	gscreenlines=hscreen/(font->ascent+font->descent+2);
}

double getcolor (int i, int j)
{	if (i>=usedcolors) i=1;
	switch (j)
	{
/*		case 0 : return ((color[i]>>16)&0x00FF)/255.0;
		case 1 : return ((color[i]>>8)&0x00FF)/255.0;
		case 2 : return (color[i]&0x00FF)/255.0;*/
		case 0 : return cred[i]/255.0;
		case 1 : return cgreen[i]/255.0;
		case 2 : return cblue[i]/255.0;
	}
	return 0;
}

int usecolors=1;

void initX (void)
/* Sets up the connection to the X server */
{	
	display=XOpenDisplay(sdisp);
	if (!display)
	{       
		fprintf(stderr,"ERROR: Connection to X server %s failed.\n",sdisp);
		exit(1);
	}
	screen=DefaultScreen(display); /* screen # */
	depth=DefaultDepth(display,screen); /* color depth */
	switch(depth)
	{
		case 1 : usedcolors=2; break;
		case 2 : usedcolors=4; break;
		case 3 : usedcolors=8; break;
		default : usedcolors=maxcolors; break;
	}
	blackpixel=BlackPixel(display,screen);
	whitepixel=WhitePixel(display,screen);

	/* load a font */
	textfont=XLoadQueryFont(display,textfontname);
	if (!textfont)
	{
		fprintf(stderr,"Cannot find %s font\n",textfontname);
		exit(1);
	}
	textheight=textfont->ascent+textfont->descent+2;
	textwidth=XTextWidth(textfont,"m",1);
	if (textwidth!=XTextWidth(textfont,"i",1))
	{
		fprintf(stderr,"You cannot use the proportional font %s!\n",textfontname);
		exit(1);
	}
	textoffset=textwidth/2;

	font=XLoadQueryFont(display,fontname);
	if (!font)
	{
		fprintf(stderr,"Cannot find %s font\n",fontname);
		exit(1);
	}

	XFlush(display);

}

void quitX (void)
/* Disconnect to server */
{   
	static int quitted=0;
	if (quitted) return;
	XDestroyWindow(display,window);
	XFreeFont(display,font);
	XFreePixmap(display,pixmap);
	XCloseDisplay(display);
	unloadhelp();
}

int createGC (Window window, GC *gc)
/* Create a new graphic context */
{	
	XGCValues gcvalues;
	*gc=XCreateGC(display,window,(unsigned long)0,&gcvalues);
	if (*gc==0) exit(1);
	return 1;
}

void sethue (double hue, int col);

void create_contexts (Window window)
{
	int i,j,k;
	unsigned char byte,bits[512];

	Visual *the_visual;

	unsigned long r, g, b;

	colormap = DefaultColormap (display, screen);
	the_visual = DefaultVisual (display, screen);
	
	if (depth > 8)
	{
		// Calcul des decalages
		r = red_mask = the_visual->red_mask;
		g = green_mask = the_visual->green_mask;
		b = blue_mask = the_visual->blue_mask;
	
		red_bits = 0;
		green_bits = 0;
		blue_bits = 0;
		red_shift = 0;
		green_shift = 0;
		blue_shift = 0;

		while (!(r & 1))
		{
			r >>= 1;
			red_shift++;
		}
		while (r & 1)
		{
			r >>= 1;
			red_bits++;
		}
		while (!(g & 1))
		{
			g >>= 1;
			green_shift++;
		}
		while (g & 1)
		{
			g >>= 1;
			green_bits++;
		}
		while (!(b & 1))
		{
			b >>= 1;
			blue_shift++;
		}
		while (b & 1)
		{
			b >>= 1;
			blue_bits++;
		}
	}
	
	for (i=0 ; i<maxcolors ; i++)
		color[i] = rgb2index(cred[i],cgreen[i],cblue[i]);

	
	createGC(window,&gc);
	XSetForeground(display,gc,blackpixel);
	XSetBackground(display,gc,whitepixel);
	XSetFont(display,gc,font->fid);
				
	createGC(window,&cleargc);
	XSetForeground(display,cleargc,whitepixel);
	XSetBackground(display,cleargc,blackpixel);

	createGC(window,&stipplegc);
	XSetForeground(display,stipplegc,blackpixel);
	XSetBackground(display,stipplegc,whitepixel);
	XSetFillStyle(display,stipplegc,FillOpaqueStippled);
	
	for (i=0; i<64; i++)
	{	for (j=0; j<512; j++)
		{	byte=0;
			if (i==0) byte=255;
			else if (i==63) byte=0;
			else for (k=0; k<8; k++) byte=(byte<<1)|((rand()&63)>i);
			bits[j]=byte;
		}
		stipple[i]=XCreateBitmapFromData(display,window,(char *)bits,64,64);
	}

	createGC(window,&textgc);
	XSetForeground(display,textgc,blackpixel);
	XSetBackground(display,textgc,whitepixel);
	XSetFont(display,textgc,textfont->fid);

	createGC(window,&invtextgc);
	XSetForeground(display,invtextgc,whitepixel);
	XSetBackground(display,invtextgc,blackpixel);
	XSetFont(display,invtextgc,textfont->fid);
	
	k=0;
	for (i=0; i<32; i++)
	{	for (j=0; j<8; j++) bits[k++]=0xAA;
		for (j=0; j<8; j++)bits[k++]=0x55;
	}
	greystipple=XCreateBitmapFromData(display,window,(char *)bits,64,64);
		
	createGC(window,&greygc);
	XSetForeground(display,greygc,blackpixel);
	XSetBackground(display,greygc,whitepixel);
	XSetFont(display,greygc,font->fid);
	XSetFillStyle(display,greygc,FillOpaqueStippled);
	XSetStipple(display,greygc,greystipple);
}	

Window open_window (int x, int y, int width, int height, int flag)
/* create and open a window plus a graphic context and a pximap */
{	
	XSetWindowAttributes attributes;
	XSizeHints sizehints;
	Window window;
	Pixmap iconpixmap;
	XWMHints wmhints;
	/* XClassHint classhints; */
	unsigned int cursorshape;
	Cursor cursor;

	/* window attributes */
	attributes.border_pixel=BlackPixel(display,screen);
	attributes.background_pixel=WhitePixel(display,screen);

	mask= CWBackPixel | CWBorderPixel;

	/* create the window */
	window= XCreateWindow(display,RootWindow(display,screen),
	    x,y,width,height,
	    2, /* Border width */
	    depth,
	    InputOutput,
	    CopyFromParent,
	    mask,
	    &attributes);

	/* Icon setting */
	iconpixmap= XCreateBitmapFromData(display,window,
	    (char *)icon_bits,icon_width,icon_height);
	wmhints.icon_pixmap=iconpixmap;
	wmhints.initial_state=NormalState;
	/* Setting the input hint for some window managers */
	wmhints.input = True;
	wmhints.flags = IconPixmapHint|StateHint|InputHint;
	XSetWMHints(display, window, &wmhints);
	
	/* Cursor */
	cursorshape=XC_dotbox;
	cursor=XCreateFontCursor(display,cursorshape);
	XDefineCursor(display,window,cursor);

	/* class hints */
	/* classhints.res_name="Euler";
	classhints.res_class="Euler";
	XSetClassHint(display,window,classhints); */

	/* Window and Icon name */
	XStoreName(display,window,"Euler");
	XSetIconName(display,window,"Euler");

	/* size hints */
	sizehints.flags=PMinSize;
	if (userpos) sizehints.flags|= USPosition | USSize;
	else sizehints.flags|= PPosition | USSize;
	sizehints.x=x;
	sizehints.y=y;
	sizehints.width=width;
	sizehints.height=height;
	sizehints.min_width=200;
	sizehints.min_height=200;
	XSetNormalHints(display,window,&sizehints);

	create_contexts(window);

	/* events, we like to receive */
	mask=KeyPressMask|ExposureMask|ButtonPressMask|StructureNotifyMask;
	XSelectInput(display,window,mask);

	/* show the window */
	XMapWindow(display,window);

	/* create a pixmap of same size */
	pixmap=XCreatePixmap(display,window,width,height,depth);
	XFillRectangle(display,pixmap,cleargc,0,0,width,height);
	XFlush(display);

	return window;
}

void computetext (void)
{
	maxlines=(hscreen-2*textoffset)/textheight;
	linelength=(wscreen-2*textoffset)/textwidth;
	linew=linelength/fieldw;
}

void grafik (void)
/* switch to a graphic screen */
{
	if (!winit)
	{	
		initX();
		atexit(quitX);
		if (usersize)
		{
			userwidth=abs(userwidth);
			if (userwidth<256) userwidth=256;
			if (userwidth>DisplayWidth(display,screen))
				userwidth=DisplayWidth(display,screen);
			userheight=abs(userheight);
			if (userheight<256) userheight=256;
			if (userheight>DisplayHeight(display,screen))
				userheight=DisplayHeight(display,screen);
		}
		else
		{
			userwidth=DisplayWidth(display,screen)/3*2;
			userheight=DisplayHeight(display,screen)/3*2;
		}
		if (userpos)
		{
			if (userx<0) userx=DisplayWidth(display,screen)
				+userx-userwidth;
			if (usery<0) usery=DisplayHeight(display,screen)
				+usery-userheight;
			if (userx<0) userx=0;
			if (usery<0) usery=0;
		}
		else
		{
			userx=usery=0;
		}
		window=open_window(userx,usery,userwidth,userheight,0);
		wscreen=userwidth;
		hscreen=userheight;
		computetext();
		computechar();
		winit=1;
	}
}

void setcolor (int c)
{
	static int oldcolor=-1;
	if (bw && c!=0) c=1;
	if (c>=usedcolors) c=usedcolors-1;
	if (c<0) c=0;
	if (c!=oldcolor)
	{
		XSetForeground(display,gc,color[c]);
		oldcolor=c;
	}
}

int oldstipple=-1,oldscolor=-1;

void sethue (double hue, int col)
{
	if (depth<8)
	{
		int k,nostipple=0;
		if (col>=usedcolors) col=usedcolors-1;
		else if (col==0)
		{
			hue-=(int)floor(hue); col=(int)floor(hue*(usedcolors-2)*0.9999)+2;
			nostipple=1;
		}
		else if (col<0)
		{
			hue-=floor(hue);
			hue*=(usedcolors-2)*0.9999;
			col=(int)floor(hue)+2;
		}
		if (oldscolor!=col) XSetForeground(display,stipplegc,color[oldscolor=col]);
		if (!nostipple)
		{
			hue-=floor(hue); hue*=0.9999; k=(int)(hue*64);
		}
		else k=0;
		if (k!=oldstipple) XSetStipple(display,stipplegc,stipple[oldstipple=k]);
	}
	else
		XSetForeground(display,stipplegc,rgb2index((short)(2*hue*(128+cred[col]/2)),
			(short)(2*hue*(128+cgreen[col]/2)),
			(short)(2*hue*(128+cblue[col]/2))));
}

#ifdef WINDOW
#define draw(x1,x2,y1,y2) XDrawLine(display,pixmap,gc,x1,x2,y1,y2),\
	XDrawLine(display,window,gc,x1,x2,y1,y2)
#define plot(x1,x2) XDrawPoint(display,pixmap,gc,x1,x2),\
	XDrawPoint(display,window,gc,x1,x2)
#else
#define draw(x1,x2,y1,y2) XDrawLine(display,pixmap,gc,x1,x2,y1,y2)
#define plot(x1,x2) XDrawPoint(display,pixmap,gc,x1,x2)
#endif

/*************** graphics and GEM initialization *************/

int memory_init (void)
/***** memory_init
	get colors, wscreen, hscreen.
	get the graphics screen.
	get all memory for stack.
*****/
{	
	long size;
	size=memsize;
	if (size<64*1024l) size=64*1024l;
	ramstart=(char *)malloc(size);
	if (!ramstart) return 0;
	ramend=ramstart+size;
	linelength=70;
	hchar=8;
	wchar=8;
	return 1;
}

int shrink (size_t size)
{	
	if (size==0) return 0;
	return 1;
}

/******************** some graphic support functions ************/

int column (double c)
{	
	long res;
	res=(long)((c*wscreen)/1024);
	return (int)res;
}

int row (double c)
{	
	long res;
	res=(long)((c*hscreen)/1024);
	return (int)res;
}

void refresh_window (void);

void graphic_mode (void)
/***** graphics
	switch to graphics. text should not be deleted.
*****/
{   
	if (in_text)
	{
		in_text=0;
		refresh_window();
	}
}

void text_mode (void)
/***** text_mode
	switch to text. graphics should not be deleted.
*****/
{	
	if (!in_text)
	{
		in_text=1;
		refresh_window();
	}
}

int oldst=-1,oldcol=-1,oldwidth=-1;

void setline (int w, int st)
{	if (w==1) w=0;
	if (oldwidth==w && oldst==st) return;
	oldst=st;
	switch (st)
	{	case line_dotted :
		case line_dashed :
			st=LineOnOffDash;
			break;
		default :
			st=LineSolid;
			break;
	}
	XSetLineAttributes(display,gc,w,st,CapRound,JoinRound);
	oldwidth=w;
}

int XMeta::gline (void *p,
	double c, double r, double c1, double r1, int col, int st, int width)
/***** gline
	draw a line.
*****/
{   
	if (!Meta::gline(0,c,r,c1,r1,col,st,width)) return 0;
	if (st==line_none) setcolor(0);
	else setcolor(col);
	setline(width,st);
	draw(column(c),row(r),column(c1),row(r1));
	setline(1,line_solid);
	if (st==line_arrow)
	{
		double dx = c1 - c;
		double dy = r1 - r;
		double norme = sqrt(dx*dx+dy*dy);
		double cs = dx/norme;
		double sn = dy/norme;
		double a =	0.3*norme;
		double b = 0.6*a;
		XPoint p[3];
		p[0].x = column(-a*cs - b*sn + c1);
		p[0].y = row(-a*sn + b*cs + r1);
		p[1].x = column(c1);
		p[1].y = row(r1);
		p[2].x = column(-a*cs + b*sn + c1);
		p[2].y = row(-a*sn - b*cs + r1);
#ifdef WINDOW
		XFillPolygon(display,window,gc,p,3,Complex,CoordModeOrigin);
#endif
		XFillPolygon(display,pixmap,gc,p,3,Complex,CoordModeOrigin);
	}
	return 1;
}

int XMeta::gmarker (void *p,
	double c, double r, int col, int type)
/***** gmarker
	plot a single marker on screen.
*****/
{
	int ms=10;
	if (!Meta::gmarker(0,c,r,col,type)) return 0;
	setcolor(col);
	ms=wscreen/markerfactor;
	setline(1,line_solid);
	switch(type)
	{
	case marker_dot : plot(column(c),row(r)); break;
	case marker_plus :	
		draw(column(c+10),row(r),column(c-10),row(r));
		draw(column(c),row(r-10),column(c),row(r+10));
		break;
	case marker_square :
	case marker_circle :
		draw(column(c+10),row(r-10),column(c+10),row(r+10));
		draw(column(c+10),row(r+10),column(c-10),row(r+10));
		draw(column(c-10),row(r+10),column(c-10),row(r-10));
		draw(column(c-10),row(r-10),column(c+10),row(r-10));
		break;
	case marker_diamond :
		draw(column(c),row(r-10),column(c+10),row(r));
		draw(column(c+10),row(r),column(c),row(r+10));
		draw(column(c),row(r+10),column(c-10),row(r));
		draw(column(c-10),row(r),column(c),row(r-10));
		break;
	case marker_star :
		draw(column(c+10),row(r),column(c-10),row(r));
		draw(column(c),row(r-10),column(c),row(r+10));		
	default :
		draw(column(c+10),row(r-10),column(c-10),row(r+10));
		draw(column(c-10),row(r-10),column(c+10),row(r+10));
	}
	return 1;
}

int fillcolor1=11,fillcolor2=3;

int XMeta::gfill (void *p,
	double c[], int st, int n, int connect[])
/***** gfill
	fill an area given by n pairs of points (in c: x,y,x,y,...)
	with the style.
*****/
{
	int i,cc[64],ci[64],j,k,count;
	XPoint points[64];
   	if (!Meta::gfill(0,c,st,n,connect)) return 0;
	for (i=0; i<2*n; i+=2) ci[i]=(int)column(c[i]);
	for (i=1; i<2*n; i+=2) ci[i]=(int)row(c[i]);
	for (i=0; i<n; i++) {
		points[i].x=ci[2*i];
		points[i].y=ci[2*i+1];
	}
	if (usedcolors>2 || st==fill_filled || bw)
	{	if (bw) setcolor(0);
		else setcolor((st==fill_filled)?fillcolor1:fillcolor2);
#ifdef WINDOW
		XFillPolygon(display,window,gc,points,n,Complex,CoordModeOrigin);
#endif
		XFillPolygon(display,pixmap,gc,points,n,Complex,CoordModeOrigin);
	}
	else
	{
#ifdef WINDOW
		XFillPolygon(display,window,greygc,points,n,Complex,CoordModeOrigin);
#endif
		XFillPolygon(display,pixmap,greygc,points,n,Complex,CoordModeOrigin);
	}
	i=0;
	setline(0,line_solid);
	setcolor(1);
	while (i<n)
	{
		j=0;
		count=0;
		while (i<n && connect[i])
		{
			cc[j++]=ci[2*i];
			cc[j++]=ci[2*i+1];
			i++;
			count++;
		}
		if (i==n)
		{	
			cc[j++]=ci[0]; 
			cc[j++]=ci[1]; 
			count++;
		}
		else
		{	
			cc[j++]=ci[2*i]; 
			cc[j++]=ci[2*i+1]; 
			count++;
		}
		for (k=0; k<count-1; k++)
			draw(cc[2*k],cc[2*k+1],cc[2*k+2],cc[2*k+3]);
		while (i<n && !connect[i]) i++;
	}
	return 1;
}


int XMeta::gfillh (void *p,
	double c[], double hue, int color, int connect)
/***** gfillh
	fill an area given by 4 pairs of points (in c: x,y,x,y,...)
	with the hue and color. connect determines, if an outline is
	to be drawn.
*****/
{	
	int i,ci[8];
	XPoint points[5];
	if (!Meta::gfillh(0,c,hue,color,connect)) return 0;
	for (i=0; i<8; i+=2) ci[i]=(int)column(c[i]);
	for (i=1; i<8; i+=2) ci[i]=(int)row(c[i]);
	for (i=0; i<4; i++) { 
		points[i].x=ci[2*i]; 
		points[i].y=ci[2*i+1]; 
	}
	points[4].x=points[0].x;
	points[4].y=points[0].y;
	sethue(hue,color);
#ifdef WINDOW
	XFillPolygon(display,window,stipplegc,points,4,Complex,CoordModeOrigin);
#endif
	XFillPolygon(display,pixmap,stipplegc,points,4,Complex,CoordModeOrigin);
	if (!connect) return 1;
	setline(0,line_solid);
	setcolor(1);
	XDrawLines(display,pixmap,gc,points,5,CoordModeOrigin);
#ifdef WINDOW
	XDrawLines(display,window,gc,points,5,CoordModeOrigin);
#endif
	return 1;
}

int XMeta::gbar (void *p,
	double c, double r, double c1, double r1, double hue,
	int color, int connect)
{	
	int x,y,w,h;
	if (!Meta::gbar(0,c,r,c1,r1,hue,color,connect)) return 0;
	x=(int)column(c); y=(int)row(r);
	w=(int)column(c1)-x; h=(int)row(r1)-y;
	if (w<=0) w=1;
	if (h<=0) h=1;
	sethue(hue,color);
#ifdef WINDOW
	XFillRectangle(display,window,stipplegc,x,y,w,h);
#endif
	XFillRectangle(display,pixmap,stipplegc,x,y,w,h);
	if (!connect) return 1;
	setline(0,line_solid);
	setcolor(1);
	XDrawRectangle(display,pixmap,gc,x,y,w,h);
#ifdef WINDOW
	XDrawRectangle(display,window,gc,x,y,w,h);
#endif
	return 1;
}

int XMeta::gbar1 (void *p,
	double c, double r, double c1, double r1,
	int color, int connect)
{	
	int x,y,w,h;
	GC *g;
	if (!Meta::gbar1(0,c,r,c1,r1,color,connect)) return 0;
	x=(int)column(c); y=(int)row(r);
	w=(int)column(c1)-x; h=(int)row(r1)-y;
	if (w<=0) w=1;
	if (h<=0) h=1;
	switch (connect)
	{	case bar_solid : setcolor(color); g=&gc; break;
		case bar_frame : g=0; break;
		default : sethue(0.5,color); g=&stipplegc; break;
	}
	if (g)
	{
#ifdef WINDOW
		XFillRectangle(display,window,*g,x,y,w,h);
#endif
		XFillRectangle(display,pixmap,*g,x,y,w,h);
	}
	if (connect==bar_solid) return 1;
	setline(0,line_solid);
	setcolor(1);
	XDrawRectangle(display,pixmap,gc,x,y,w,h);
#ifdef WINDOW
	XDrawRectangle(display,window,gc,x,y,w,h);
#endif
	return 1;
}

int XMeta::gtext (void *p,
	double c, double r, char *text, int color, int alignment)
/***** gtext
	output a graphic text on screen.
*****/
{	
	int width;
	if (!Meta::gtext(0,c,r,text,color,alignment)) return 0;
	setcolor(color);
	switch (alignment)
	{	
		case 0:
#ifdef WINDOW
			XDrawString(display,window,gc,column(c),row(r)+font->ascent,text,strlen(text));
#endif
			XDrawString(display,pixmap,gc,column(c),row(r)+font->ascent,text,strlen(text));
			break;

		case 1:
			width=XTextWidth(font,text,strlen(text));
#ifdef WINDOW
			XDrawString(display,window,gc,column(c)-width/2,row(r)+font->ascent,text,strlen(text));
#endif
			XDrawString(display,pixmap,gc,column(c)-width/2,row(r)+font->ascent,text,strlen(text));
			break;
			
		case 2:
			width=XTextWidth(font,text,strlen(text));
#ifdef WINDOW
			XDrawString(display,window,gc,column(c)-width,row(r)+font->ascent,text,strlen(text));
#endif
			XDrawString(display,pixmap,gc,column(c)-width,row(r)+font->ascent,text,strlen(text));
			break;	
	}
	return 1;
}

int XMeta::gvtext (void *p,
	double c, double r, char *text, int color, int alignment)
/***** gtext
	output a graphic text on screen vertically.
*****/
{	
	if (!Meta::gvtext(0,c,r,text,color,alignment)) return 0;
	setcolor(color);
	switch (alignment)
	{
		case 0:
#ifdef WINDOW
			XRotDrawAlignedString(display,font,-90,window,gc,column(c),row(r),text,MLEFT);
#endif
			XRotDrawAlignedString(display,font,-90,pixmap,gc,column(c),row(r),text,MLEFT);
			break;
			
		case 1:
#ifdef WINDOW
			XRotDrawAlignedString(display,font,-90,window,gc,column(c),row(r),text,MCENTRE);
#endif
			XRotDrawAlignedString(display,font,-90,pixmap,gc,column(c),row(r),text,MCENTRE);
			break;
		
		case 2:
#ifdef WINDOW
			XRotDrawAlignedString(display,font,-90,window,gc,column(c),row(r),text,MRIGHT);
#endif
			XRotDrawAlignedString(display,font,-90,pixmap,gc,column(c),row(r),text,MRIGHT);
			break;
	}
	return 1;
}

int XMeta::gvutext (void *p,
	double c, double r, char *text, int color, int alignment)
/***** gtext
	output a graphic text on screen vertically.
*****/
{	
	if (!Meta::gvutext(0,c,r,text,color,alignment)) return 0;
	setcolor(color);
	switch (alignment)
	{
		case 0:
#ifdef WINDOW
			XRotDrawAlignedString(display,font,90.0,window,gc,column(c),row(r),text,MLEFT);
#endif
			XRotDrawAlignedString(display,font,90.0,pixmap,gc,column(c),row(r),text,MLEFT);
			break;
			
		case 1:
#ifdef WINDOW
			XRotDrawAlignedString(display,font,90.0,window,gc,column(c),row(r),text,MCENTRE);
#endif
			XRotDrawAlignedString(display,font,90.0,pixmap,gc,column(c),row(r),text,MCENTRE);
			break;
		
		case 2:
#ifdef WINDOW
			XRotDrawAlignedString(display,font,90.0,window,gc,column(c),row(r),text,MRIGHT);
#endif
			XRotDrawAlignedString(display,font,90.0,pixmap,gc,column(c),row(r),text,MRIGHT);
			break;
	}
	return 1;
}

int XMeta::gscale (void *p, double s)
/***** scale
	scale the screen according s = true width / true height.
	This is not necessary on a window based system.
*****/
{   Meta::gscale(0,s);
	return 1;
}

void translate (XKeyEvent *event, int *key, int *scan);
void mouse (double *x, double *y)
/****** mouse
	wait, until the user marked a screen point with the mouse.
******/
{	
	XEvent event;
	int taste,scan;
	while (1)
	{
		XWindowEvent(display,window,mask,&event);
		if (event.type==ButtonPress)
		{	
			*x=(int)((event.xkey.x)*1024.0/wscreen);
			*y=(int)((event.xkey.y)*1024.0/hscreen);
			return;
		}
		else if (event.type==KeyPress)
		{
			translate(&(event.xkey),&taste,&scan);
			if (scan==escape)
			{
				*x=-1; *y=-1; return;
			}
		}
	}
}

void getpixelsize (double *x, double *y)
{	*x=1024.0/wscreen;
	*y=1024.0/hscreen;
}

/********************* text cursor ******************/

/* defined as macros */

/************** text screen **************************/

void show_cursor (void)
{
	char cstring[]=" ";
	if (!cursoron || cx>linelength) return;
	cstring[0]=textend[cx];
	if (cstring[0]==0) cstring[0]=' ';
	XDrawImageString(display,window,invtextgc,
		textoffset+cx*textwidth,
		cy*textheight+textfont->ascent+textoffset,
		cstring,1);
}

void hide_cursor (void)
{
	char cstring[]=" ";
	if (!cursoron || cx>linelength) return;
	cstring[0]=textend[cx];
	if (cstring[0]==0) cstring[0]=' ';
	XDrawImageString(display,window,textgc,
		textoffset+cx*textwidth,
		cy*textheight+textfont->ascent+textoffset,
		cstring,1);
}

void move_cl (void)
{
	if (cx>0)
	{
		hide_cursor(); cx--; show_cursor();
	}
}

void move_cr (void)
{
		hide_cursor(); cx++; show_cursor();
}

void cursor_on (void)
{
	if (scrolled) return;
	cursoron=1;
	show_cursor();
}

void cursor_off (void)
{
	if (scrolled) return;
	cursoron=0;
	hide_cursor();
}

void textline (char *p, int x, int y)
{
	XDrawImageString(display,window,textgc,textoffset+x*textwidth,
		y*textheight+textfont->ascent+textoffset,p+x,
		strlen(p+x));
	if (y==cy && !scrolled) show_cursor();
}

void clearline (char *p, int cx, int cy)
{
	XFillRectangle(display,window,invtextgc,
		textoffset+cx*textwidth,textoffset+cy*textheight,
		wscreen-(textoffset+cx*textwidth),textheight);
	memset(p+cx,0,textstart+TEXTSIZE-(p+cx));
	if (!scrolled) show_cursor();
}

void clear_eol (void)
{	clearline(textend,cx,cy);
}

void clear_screen (void)
{	XFillRectangle(display,window,invtextgc,0,0,wscreen,hscreen);
	cx=0; cy=0; scrolled=0;
	textwindow=textend=textstart;
	memset(textstart,0,TEXTSIZE);
}

void textupdate (void)
{	char *tp;
	int i;
	XFillRectangle(display,window,invtextgc,0,0,wscreen,hscreen);
	tp=textwindow; i=0;
	while (tp<=textend && i<maxlines)
	{	textline(tp,0,i);
		i++; tp+=strlen(tp)+1;
	}
	XFlush(display);
}

void new_line (void)
{	int length;
	char *tp;
	hide_cursor();
	cy++; cx=0;
	textend+=strlen(textend)+1;
	if (textend>textstart+TEXTSIZE-256)
	{	tp=textstart+(TEXTSIZE-256)/8;
		tp+=strlen(tp)+1;
		memmove(textstart,tp,textend-tp);
		length=tp-textstart;
		textend-=length; textwindow-=length;
	}
	if (cy>=maxlines)
	{	cy--; textwindow+=strlen(textwindow)+1;
		XCopyArea(display,window,window,gc,
			0,textoffset+textheight,
			wscreen,
			(maxlines-1)*textheight,
			0,textoffset);
		clearline(textend,0,cy);
	}
}

void gprint (char *s)
/* print a line onto the screen, parse tabs and \nl */
{
	int cx0=cx,cx1,i;
	if (scrolled)
	{	textwindow=oldtextwindow;
		show_cursor();
		scrolled=0;
		refresh_window();
	}
	while (*s)
	{
		switch(*s)
		{
			case 10 : s++; textline(textend,cx0,cy); cx0=0;
				new_line(); break;
			case 9 :
				cx1=(cx/4+1)*4;
				for (i=cx; i<cx1; i++) textend[i]=' ';
				cx=cx1; s++;
				break;
			default :
				textend[cx]=*s; cx++;
				s++;
				break;
		}
		if (textend+cx>textstart+TEXTSIZE)
		{	cx0=0; new_line(); }
	}
	textline(textend,cx0,cy);
	XFlush(display);
}

/**************** refresh routine **********************/

void refresh_window (void)
/* refresh the screen */
{
	if (in_text) textupdate();
	else
	{
		XCopyArea(display,pixmap,window,gc,0,0,wscreen,hscreen,0,0);
		XFlush(display);
	}
}

/******************** keyboard and other events **********************/

int isgerman (KeySym k)
{	
	if ((char)k=='ö' || (char)k=='ä' || (char)k=='ü' || (char)k=='ß' ||
		(char)k=='Ö' || (char)k=='Ä' || (char)k=='Ü') return 1;
	return 0;
}

void translate (XKeyEvent *event, int *key, int *scan)
/* Translate events into key codes */
{
	int length,i;
	char buffer[65];
	KeySym keysym;
/*	XComposeStatus status;*/

	*scan=0; *key=0;
	length=XLookupString(event,buffer,64,&keysym,0);
	switch (keysym)
	{
		case XK_Prior :
#ifdef SUN
		case 0xffda :
#endif
			if (!in_text || textwindow==textstart) break;
			if (!scrolled)
			{	oldtextwindow=textwindow;
				scrolled=1;
			}
			for (i=0; i<maxlines-1; i++)
			{	textwindow--;
				while (textwindow>textstart)
				{	if (*(textwindow-1)==0) break;
					textwindow--;
				}
				if (textwindow==textstart) break;
			}
			hide_cursor();
			refresh_window();
			return;
		case XK_Next :
#ifdef SUN
		case 0xffe0 :
#endif
			if (!scrolled || !in_text) break;
			for (i=0; i<maxlines-1; i++)
			{	textwindow+=strlen(textwindow)+1;
				if (textwindow>=oldtextwindow) break;
			}
			if (textwindow>=oldtextwindow)
			{	textwindow=oldtextwindow;
				scrolled=0;
				show_cursor();
			}
			refresh_window();
			return;
	}
	switch (keysym)
	{
		case XK_Return : *scan=enter; return;
		case XK_Escape : *scan=escape; return;
#ifndef BACKSPACE
		case XK_BackSpace : *scan=backspace; return;
		case XK_Delete : *scan=deletekey; return;
#else
		case XK_BackSpace :
		case XK_Delete : *scan=backspace; return;
#endif
		case XK_Up :
			if (event->state & ControlMask) *scan=clear_home;
			else *scan=cursor_up ;
			return;
		case XK_Down :
			if (event->state & ControlMask) *scan=clear_home;
			else *scan=cursor_down; 
			return;
		case XK_Right :
			if (event->state & ShiftMask) *scan=line_end;
			else if (event->state & ControlMask) *scan=word_right;
			else *scan=cursor_right; 
			return;
		case XK_Left :
			if (event->state & ShiftMask) *scan=line_start;
			else if (event->state & ControlMask) *scan=word_left;                         else *scan=cursor_left;
			return;
		case XK_Insert : *scan=help; return;
		case XK_F1 : 
#ifdef SUN
		case 0xff6a :
#endif
			*scan=fk1; return;
		case XK_F2 : *scan=fk2; return;
		case XK_F3 : *scan=fk3; return;
		case XK_F4 : *scan=fk4; return;
		case XK_F5 : *scan=fk5; return;
		case XK_F6 : *scan=fk6; return;
		case XK_F7 : *scan=fk7; return;
		case XK_F8 : *scan=fk8; return;
		case XK_F9 : *scan=fk9; return;
		case XK_F10 : *scan=fk10; return;
		case XK_KP_Enter : *scan=enter; return;
		case XK_Tab : *scan=switch_screen; return;
		case XK_End :
		case 0xffde :
			if (event->state & ShiftMask) *scan=clear_home;
			else if (event->state & ControlMask) *scan=clear_home;
			else *scan=line_end;
			return;
		case XK_Begin :
		case XK_Home :
		case 0xffd8 :
			if (event->state & ShiftMask) *scan=clear_home;
			else if (event->state & ControlMask) *scan=clear_home;
			else *scan=line_start;
			return;
		case XK_dead_circumflex : *key='^'; return;
		case XK_dead_tilde : *key='~'; return;
		case XK_dead_acute : *key='\''; return;
		case XK_dead_grave : *key='`'; return;
	}
	if (/*
		( ((keysym>=' ') && (keysym<='~')) || isgerman(keysym) )
		&&*/ length>0)
	{
		*key=buffer[0];
		return;
	}
}

void process_event (XEvent *event)
{
	XEvent dummyevent;
	switch (event->type)
	{
		case GraphicsExpose:
			if (event->xgraphicsexpose.count>0) break;
			refresh_window(); break;
		case Expose:
			if (event->xexpose.count>0) break;
			refresh_window(); break;
		case ConfigureNotify :
			while (XCheckWindowEvent(display,window,mask,&dummyevent));
			if (event->xconfigure.width==wscreen &&
			    event->xconfigure.height==hscreen) break;
			wscreen=event->xconfigure.width;
			hscreen=event->xconfigure.height;
			computechar();
			computetext();
			XFreePixmap(display,pixmap);
			pixmap=XCreatePixmap(display,window,
				wscreen,hscreen,depth);
			gclear();
			textwindow=textend-1;
			while (textwindow>=textstart && *textwindow) textwindow--;
			textwindow++; oldtextwindow=textwindow; cy=0;
			scrolled=0; cursoron=1;
			refresh_window();
			break;
		/* keyboard mapping changed */
		case MappingNotify:
			XRefreshKeyboardMapping(&event->xmapping);
			break;
		case DestroyNotify :
			exit(1); break;
	}
}

int wait_key (int *scan)
/***** 
	wait for a keystroke. return the scancode.
*****/
{   
	int taste;
	XEvent event;

	*scan=key_none; taste=0;
	while (1)
	{
		XWindowEvent(display,window,mask,&event);
		if (event.type==KeyPress)
		{	translate(&(event.xkey),&taste,scan);
			if (*scan || taste) break;
		}
		process_event(&event);
	}
	return taste;
}

int notekey=0;

int test_key (void)
/***** test_key
	see, if user pressed the keyboard.
	return the scan_code, if he did.
*****/
{   
	XEvent event;
	int scan,key;
	notekey=0;
	if (userbreak)
	{
		userbreak=0; return escape;
	}
	else if (
		XCheckWindowEvent(display,window,mask,&event))
	{
		if (event.type==KeyPress)
		{	translate(&(event.xkey),&key,&scan);
			if (scan==escape) return scan;
			else 
			{	if (scan) notekey=scan;
				else notekey=key;
				return 0;
			}
		}
		else process_event(&event);
	}
	return 0;
}

int test_code (void)
/***** test_code
	see, if user pressed the keyboard.
	return the scan_code, if he did.
*****/
{   
	XEvent event;
	int scan,key;
	if (notekey)
	{	key=notekey;
		notekey=0;
		return key;
	}
	if (XCheckWindowEvent(display,window,mask,&event))
	{
		if (event.type==KeyPress)
		{	translate(&(event.xkey),&key,&scan);
			if (scan) return scan;
			else return key;
		}
		else process_event(&event);
	}
	return 0;
}

/**************** directory *******************/

static char path[256];

char *cd (char *dir)
/* sets the path if dir!=0 and returns the path */
{	
	chdir(dir);
	if (getcwd(path,256)) return path;
	return dir;
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

/***************** clear screens ********************/

int XMeta::gclear (void *p)
{	
	if (!Meta::gclear(0)) return 0;
	XFillRectangle(display,pixmap,cleargc,0,0,wscreen,hscreen);
#ifdef WINDOW
	XFillRectangle(display,window,cleargc,0,0,wscreen,hscreen);
#endif
	XFlush(display);
	return 1;
}

int execute (char *name, char *args)
/**** execute
	call an external program, return 0, if there was no error.
****/
{	return 0;
}

void gflush (void)
{
#ifndef WINDOW
	XCopyArea(display,pixmap,window,gc,0,0,wscreen,hscreen,0,0);
#endif
	XFlush(display);
}

#ifndef SY_CLK_TCK
#define SY_CLK_TCK 50
#endif

double myclock (void)
/* Return the time in seconds */
{
#ifdef RS6000
	struct timestruc_t t;
	gettimer(TIMEOFDAY,&t);
	return (t.tv_sec+t.tv_nsec/1000000000.0);
#else
	struct tms b;
	times(&b);
	return (double)(b.tms_utime)/(SY_CLK_TCK);
#endif
}

void edit_on (void)
{
}

void edit_off (void)
{
}

void sys_wait (double time, int *scan)
{	double now;
	XSync(display,0);
	now=myclock();
	while (myclock()<now+time)
	{	*scan=test_code();
		if (*scan==switch_screen)
		{	if (in_text)
			{	graphic_mode();
				wait_key(scan);
				text_mode();
			}
			else
			{	text_mode();
				wait_key(scan);
				graphic_mode();
			}
			*scan=0;
		}
		if (*scan) return;
	}
	*scan=0;
}

/************ main *******************/

void usage (void)
{
	fprintf(stderr,"euler [-f FONT] [-g FONT] [-s KBYTES] [-x]\n"
		" [-geom GEOMETRY] [-d DISPLAY] [-0..15 COLOR] [-b] files\n");
}

void get_geometry (char *s)
{
	long d;
	char *end;
	d=strtol(s,&end,10);
	if (!end || end==s) return; else { userwidth=d; s=end; }
	if (*s++!='x') { usage(); exit(1); }
	d=strtol(s,&end,10);
	if (!end || end==s) return; else { userheight=d; s=end; }
	usersize=1;
	if (*s==0) return;
	d=strtol(s,&end,10);
	if (!end || end==s) return; 
	else 
	{ 
		userx=d;
		if (*s=='-' && d==0) userx=-1;
		s=end; 
	}
	userpos=1;
	if (*s==0) return;
	d=strtol(s,&end,10);
	if (!end || end==s) return;
	else
	{ 
		usery=d;
		if (*s=='-' && d==0) usery=-1;
		s=end; 
	}
}

void setint (int code)
{       
	userbreak=1;
	signal(SIGTERM,setint);
}

int fpe=0;

#ifdef FPE

void setfpe (int code)
{       
	if (fpe)
	{	error=1;
		output("Math error generated!\n");
	}
	signal(SIGFPE,setfpe);
}

#endif

int ioerrorhandler (Display *display)
{	exit(1)	;
	return 0;
}

int main (int argc, char *argv[])
{
	XEvent event;
	int nn;
	/* init GEM */
	signal(SIGINT,setint);
#ifdef FPE
	signal(SIGFPE,setfpe);
#endif
	XSetIOErrorHandler(ioerrorhandler);
	meta=new XMeta();
	nextcheck=myclock();
	while (argc>1)
	{
		if (argv[1][0]=='=')
		{	get_geometry(argv[1]+1);
			argc--; argv++;
		}
		else if (argv[1][0]=='-')
		{
			switch(argv[1][1])
			{
				case 'f' :
					if (argv[1][2])
					{	strcpy(textfontname,argv[1]+2); break;
					}
					else if (argv[2])
					{	strcpy(textfontname,argv[2]); argc--; argv++;
						break;
					}
					break;
				case 'g' :
					if (strncmp(argv[1],"-geom",5)==0)
					{	if (argv[2])
						{	get_geometry(argv[2]); argc--; argv++;
						}
					}
					else if (argv[1][2])
					{	strcpy(fontname,argv[1]+2); break;
					}
					else if (argv[2])
					{	strcpy(fontname,argv[2]); argc--; argv++;
						break;
					}
					break;
				case 's' :
					if (argv[1][2])
					{	memsize=atoi(argv[1]+2)*1024l; break;
					}
					else if (argv[2])
					{	memsize=atoi(argv[2])*1024l; argc--; argv++;
						break;
					}
					break;
				case 'd' :
					strcpy(sdisp,argv[2]); argc--; argv++; break;
				case 'b' :
					bw=1; break;
				case 'x' :
					fpe=1; break;
				default :
					if (sscanf(argv[1]+1,"%d",&nn)==1 && nn>=0 && nn<maxcolors)
					{	colorname[nn]=argv[2]; argc--; argv++;
						break;
					}
					usage(); exit(1);
			}
			argc--; argv++;
		}
		else break;
	}
	if (!memory_init()) exit(1);
	grafik();
	XWindowEvent(display,window,ExposureMask,&event);
	process_event(&event);
	loadhelp("help.txt");
	main_loop(argc,argv);
	return 0;
}

