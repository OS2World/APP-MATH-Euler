#include "easy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <mmsystem.h>

char outputs[256];

#include "header.h"
#include "sysdep.h"
#include "euler.h"
#include "eulerhlp.h"
#include "version.h"
#include "help.h"
#include "meta.h"
#include "yacas.h"

int size=8*1024*1024; /* default stack size in Byte */

int linelength=80; /* width of text screen */
int wchar=1,hchar=1; /* font metrics of graphic text */
int editing=0,textmode=1;
int usecolors=1;
int mousewaiting=0;
int fillcolor1=11,fillcolor2=3;
int purecolors=0;
int markerfactor=100;

int wantquit=0;

int stacksize=8,graphicssize=2;

CriticalSection critical;

String wantload;

Profile profile("EULER-95\\settings");

class WindowsMeta : public Meta
{	public :
	int Draw;
	WindowsMeta (int size=32) : Meta(size),Draw(1) {}
 	WindowsMeta (Meta *meta) : Meta(meta),Draw(1) {}
	virtual int gclear (void *p);
	virtual int gclip (void *p, double c, double r, double c1, double r1);
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

Meta *meta;

String Printer(""),PrintText("");

int memory_init (void)
/***** memory_init
	get memory for stack.
*****/
{   size=stacksize*1024*1024;
	ramstart=(char *)malloc(size+512*1024);

    if (!ramstart)
    {   size=(512+16*1024)*1024;
        ramstart=(char *)malloc(size+512*1024);
    }
	ramend=ramstart+size;

	return 1;
}

int shrink (size_t size)
/**** allows shrinking of memory for single task systems.
simply return 1 if you do not support this or set NOSHRINK in funcs.c
*****/
{	return 1;
}

//******************** Color ******************

static int red[16]=  {255,0,100,0  ,0  ,0  ,100,150,100,50,220 ,80  ,80  ,80  ,140 ,190};
static int green[16]={255,0,0  ,100,0  ,100,100,150,100,50,80  ,220 ,80  ,140 ,140 ,190};
static int blue[16]= {255,0,0  ,0  ,100,100,  0,150,100,50,80  ,80  ,220 ,140 , 80,190};

double getcolor (int i, int j)
{	switch (j)
	{	case 0 : return red[i]/256.0;
    	case 1 : return green[i]/256.0;
        case 2 : return blue[i]/256.0;
    }
    return 0;
}

class MyColors
{	Color *C[16];
	public :
	MyColors ();
	~MyColors ();
	Color color (int i);
	void set (int i, Color c) { *C[i]=c; }
} mycolors;

MyColors savecolors;

MyColors::MyColors ()
{	for (int i=0; i<16; i++)
		C[i]=new Color(red[i],green[i],blue[i],purecolors);
}

MyColors::~MyColors ()
{	for (int i=0; i<16; i++) delete C[i];
}

Color MyColors::color (int i)
{	if (i<0 || i>15) i=0;
	return *C[i];
}

static void hsv_to_rgb (double h,double s,double v,double *r,double
*g,double *b)
{
	int i;
	double f, w, q, t;

	if (s == 0.0) s = 0.000001;

	if (h == -1.0) {
		*r = v;
		*g = v;
		*b = v;
	} else {
		if (h == 360.0) h = 0.0;
		h = h / 60.0;
		i = (int)h;
		f = h - i;
		w = v * (1.0 - s);
		q = v * (1.0 - (s * f));
		t = v * (1.0 - (s * (1.0 - f)));

		switch (i) {
			case 0:
				*r = v;
				*g = t;
				*b = w;
				break;
			case 1:
				*r = q;
				*g = v;
				*b = w;
				break;
			case 2:
				*r = w;
				*g = v;
				*b = t;
				break;
			case 3:
				*r = w;
				*g = q;
				*b = v;
				break;
			case 4:
				*r = t;
				*g = w;
				*b = v;
				break;
			case 5:
				*r = v;
				*g = w;
				*b = q;
				break;
		}
    }
}

class MyColor : public Color
{	public :
	MyColor (int c) : Color(mycolors.color(c),purecolors) {}
	MyColor (int c, double hue)
	{
		int r, g, b;
	
		if(c) {
			r = (int)(2*hue*(128+red[c]/2));
			g = (int)(2*hue*(128+green[c]/2));
			b = (int)(2*hue*(128+blue[c]/2));
		} else {
			double cr,cg,cb;
			hsv_to_rgb((1.0-hue)*255.0,0.5,0.9,&cr,&cg,&cb);
			r = (int)(255.0*cr);
			g = (int)(255.0*cg);
			b = (int)(255.0*cb);
		}

		set(r>255 ? 255:r, g>255 ? 255:g, b>255 ? 255:b); // the color component must be <= 255.
	}
};


int setcolor (int i, double red, double green, double blue)
{	if (i<0 || i>=16) return 1;
	if (red<0 || red>1) return 1;
	if (green<0 || green>1) return 1;
	if (blue<0 || blue>1) return 1;
	mycolors.set(i,
		*new Color(
			(int)(red*255+0.5),
			(int)(green*255+0.5),
			(int)(blue*255+0.5)));
	return 0;
}

void resetcolors ()
{	for (int i=0; i<16; i++)
		mycolors.set(i,savecolors.color(i));
}

//************* Output transforms **************************

int col (PS &ps, double x)
{	return (int)(x*ps.width()/1024.0);
}
int row (PS &ps, double y)
{	return (int)(y*ps.height()/1024.0);
}

PS *ops=0;

//******************* Font ******************************

String TextFontName("Courier New");
String EditorFontName("Courier New");
String GraphicsFontName("Courier New");
String PrinterFontName("Courier New");

int gscreenlines=39;
int tfontheight=15;

Font *courier=new Font(TextFontName,15);
Font *couriersmall=new Font(GraphicsFontName,13);
Font *couriersmalldown=new Font(GraphicsFontName,13,Font::normal,0,2700);
Font *couriersmallup=new Font(GraphicsFontName,13,Font::normal,0,900);

//******************** Main Window **********************

Program program;

static int waiting=0;

void doexit ();

class ExtString : public String
{	public :
	ExtString (char *s, int size) : String(s,size) {}
	void insert (int pos, char c);
	void insert (int pos, char *c);
};

void ExtString::insert (int pos, char c)
{   int n=strlen(text());
	if (size()<n+1)
	{	copy(text(),n+1);
	}
	memmove(text()+pos+1,text()+pos,n-pos+1);
	text()[pos]=c;
}

void ExtString::insert (int pos, char *c)
{   int n=strlen(text());
	if ((unsigned int)size()<n+strlen(c))
	{	copy(text(),n+strlen(c));
	}
	memmove(text()+pos+strlen(c),text()+pos,n-pos+1);
	memmove(text()+pos,c,strlen(c));
}

class TextLine : public ExtString
{   TextLine *Prev,*Next;
	int Type,Mark;
	public :
	enum { output,prompt,udf,comment };
	TextLine (TextLine *prev=0, TextLine *next=0, char *s="")
		:	Prev(prev),Next(next),ExtString(s,16),
			Type(output),Mark(0)
	{	if (prev) prev->Next=this;
		if (next) next->Prev=this;
	}
	~TextLine ()
	{	if (Prev) Prev->Next=Next;
		if (Next) Next->Prev=Prev;
	}
	TextLine *next () { return Next; }
	TextLine *prev () { return Prev; }
	int type () { return Type; }
	void type (int t) { Type=t; }
	int mark () { return Mark; }
	void mark (int f) { Mark=f; }
};

Flag CaretOn(0);

class Text
{   TextLine *Start,*Top,*Cursor;
	int Cursorpos,Offset,TextHeight,Height;
	StandardWindow *W;
	int CX,CY,CW,CH;
	TextLine *Dragline,*Markstart,*Markend;
	int Markstartpos,Markendpos;
	int Dragpos;
	int Changed;
	public :
	Text (StandardWindow &w)
		: W(&w),Offset(0),TextHeight(0),Dragline(0),
		Markstart(0),Markend(0)
	{	Cursor=Top=Start=new TextLine;
		Cursorpos=0; Changed=0;
	}
	~Text ();
	void draw (PS &ps);
	void delline (); // from Cursor to line end
	void inserttext (char *s);
	void insertchar (char c);
	void insertline ();
	TextLine *top (TextLine *Cursor);
	int getheight ();
	void setfont (PS &ps) { ps.font(*courier); }
	int textheight ()
	{	if (TextHeight==0) getheight();
		return TextHeight;
	}
	int height () { return Height; }
	void sized () { getheight(); Top=top(Cursor); W->update(); }
	void clear ();
	void cursorleft ();
	void cursorright ();
	void setcaret (PS &ps);
	void updatecaret ();
	void redraw ();
	void drawline (PS &ps, int y, TextLine *p);
	void drawcursorline (PS &ps);
	void drawbelow (PS &ps, TextLine *l);
	void lineup ();
	void linedown ();
	void updatetop ();
	void removeoutput ();
	void nextprompt ();
	void newprompt ();
	void load (char *filename);
	void save (char *filename);
	void settype ();
	int type () { return Cursor->type(); }
	void comment ();
	int endfunction ();
	void pageup ();
	void pagedown ();
	void scrollup ();
	void scrolldown ();
	void showcursor ();
	void computecursor (int x, int y, TextLine * &l, int &pos);
	int startdrag (int x, int y);
	void drag (int x, int y);
	void enddrag (int x, int y);
	void mark (TextLine *l, int pos);
	void unmark ();
	void unmarkall ();
	void set (char *s);
	void deleteudf ();
	void deletecommand ();
	void insertcommand ();
	void copytext ();
	void copycommands ();
    void pastecommands ();
	void deleteoutput ();
	void selectall ();
	void setscroll ();
	void setscroll (int pos);
	int changed () { return Changed; }
    void changed (int i) { Changed=i; }
} *text=0;

Text::~Text ()
{	TextLine *l=Start,*n;
	while (l)
	{	n=l->next();
		delete l;
		l=n;
	}
}

void Text::setcaret (PS &ps)
{	TextLine *p=Top;
	int i,lines=height()/textheight(),w,h;
	String Work;
	setfont(ps);
	CW=1; CH=textheight();
	for (i=0; i<lines; i++)
	{   if (Cursor==p)
		{   Work.copy(p->text());
			Work.text()[Cursorpos]=0;
			ps.textextent(Work,w,h);
			CX=w-Offset; CY=i*textheight();
			return;
		}
		p=p->next();
		if (!p) break;
	}
	CX=-1000; CY=-1000;
}

void Text::updatecaret ()
{	W->setcaretsize(CW,CH);
	if (CX>W->width()*9/10)
	{	Offset+=CX-W->width()/2;
		W->update();
		return;
	}
	if (CX<W->width()/10 && Offset>0)
	{	Offset-=CX-W->width()/2;
		if (Offset<0) Offset=0;
		W->update();
		return;
	}
	W->setcaret(CX,CY);
}

void Text::drawline (PS &ps, int y, TextLine *p)
{	switch (p->type())
	{	case TextLine::prompt : ps.textcolor(MyColor(2)); break;
		case TextLine::comment : ps.textcolor(MyColor(3)); break;
		case TextLine::udf : ps.textcolor(MyColor(4)); break;
		default : ps.textcolor(MyColor(1));
	}
	if (p->mark())
	{   int c=0;
		ps.alignment(Alignment::left,Alignment::top,0);
		ps.move(-Offset,y);
		if (p==Markstart)
		{   if (Markstartpos>0)
			{	ps.backcolor(Color::background());
				ps.text(p->text(),Markstartpos);
			}
			if (p==Markend)
			{	c=Markstartpos;
				goto mend;
			}
			ps.backcolor(Color(250,100,100));
			ps.text(p->text()+Markstartpos);
		}
		else if (p==Markend)
		{   mend :
			ps.backcolor(Color(250,100,100));
			ps.text(p->text()+c,Markendpos-c);
			ps.backcolor(Color::background());
			ps.text(p->text()+Markendpos);
		}
		else
		{	ps.backcolor(Color(250,100,100));
			ps.text(p->text());
		}
		ps.backcolor(Color::background());
	}
	else ps.text(-Offset,y,p->text(),Alignment::left,Alignment::top);
}

void Text::draw (PS &ps)
{   if (CaretOn) W->hidecaret();
	setfont(ps);
	ps.erase();
	int r=0;
	int h=getheight();
	TextLine *p=Top;
	while (1)
	{	if (r+h>ps.height() || !p) break;
		drawline(ps,r,p);
		r+=h;
		p=p->next();
	}
	setcaret(ps);
	if (CaretOn) W->showcaret();
	updatecaret();
}

void Text::drawcursorline (PS &ps)
{   if (CaretOn) W->hidecaret();
	setfont(ps);
	ps.backmode(PS::opaque);
	int r=0;
	int h=getheight(),wt,ht;
	TextLine *p=Top;
	while (1)
	{	if (r+h>ps.height() || !p) break;
		if (Cursor==p)
		{   drawline(ps,r,p);
			ps.textextent(p->text(),wt,ht);
			wt-=Offset;
			if (wt<ps.width())
			{	ps.bar(wt,r,ps.width()-wt,h,Color::background());
			}
			break;
		}
		r+=h;
		p=p->next();
	}
	setcaret(ps);
	if (CaretOn) W->showcaret();
}

void Text::drawbelow (PS &ps, TextLine *T)
{	setfont(ps);
	if (CaretOn) W->hidecaret();
	ps.backmode(PS::opaque);
	int r=0;
	int h=getheight(),wt,ht,update=0;
	TextLine *p=Top;
	while (1)
	{	if (r+h>ps.height() || !p) break;
		if (update || T==p)
		{   drawline(ps,r,p);
			ps.textextent(p->text(),wt,ht);
			wt-=Offset;
			if (wt<ps.width())
			{	ps.bar(wt,r,ps.width()-wt,h,Color::background());
			}
			update=1;
		}
		r+=h;
		p=p->next();
	}
	if (r<height())
	{	ps.bar(0,r,ps.width(),ps.height()-r,Color::background());
	}
	if (CaretOn) W->showcaret();
	setcaret(ps);
}

void Text::inserttext (char *s)
{   unmark();
	if (!*s) return;
	WindowPS ps(*W);
	ps.backmode(ps.opaque);
	TextLine *h;
	int i,n;
	while (*s)
	{   switch (*s)
		{   case '\n' :
				h=Top;
				drawcursorline(ps);
				insertline();
				if (h->next()==Top)
				{   ScrollDC(ps.handle(),0,-textheight(),0,0,0,0);
				}
				else if (h!=Top)
				{	drawbelow(ps,Top);
				}
				break;
			case 9 :
				n=4-(Cursorpos%4);
				for (i=0; i<n; i++) insertchar(' ');
				break;
			default :
				insertchar(*s);
		}
		h=Top;
		Top=top(Cursor);
		if (h!=Top) drawbelow(ps,Top);
		s++;
	}
	drawbelow(ps,Cursor);
}

TextLine * Text::top (TextLine *Cursor)
{	TextLine *p=Cursor;
	int h=textheight();
	if (h<=0) return Start;
	int lines=height()/h;
	if (lines==0) lines=1;
	lines--;
	while (lines>0)
	{	p=p->prev();
		if (p==Top) return Top;
		if (p==0) return Start;
		lines--;
	}
	return p;
}

int Text::getheight ()
{	WindowPS ps(*W);
	setfont(ps);
	TextHeight=ps.textheight();
	Height=ps.height();
	return TextHeight;
}

void Text::insertchar (char c)
{	Cursor->insert(Cursorpos,c);
	Cursorpos++;
}

void Text::insertline ()
{   Cursor=new TextLine(Cursor,Cursor->next());
	Cursorpos=0;
	Top=top(Cursor);
}

void Text::delline ()
{   Cursor->text()[Cursorpos]=0;
	WindowPS ps(*W);
	ps.mode(PS::opaque);
	drawcursorline(ps);
}

void Text::cursorleft ()
{   
    WindowPS wps(*W);
    if (Cursorpos>0) Cursorpos--;
	setcaret(wps);
}

void Text::cursorright ()
{   
    WindowPS wps(*W);
    if (Cursor->text()[Cursorpos]) Cursorpos++;
	setcaret(wps);
}

void Text::updatetop ()
{	TextLine *l=top(Cursor);
	if (l!=Top)
	{	
        WindowPS wps(*W);
        Top=l;
		drawbelow(wps,Top);
	}
}

void Text::lineup ()
{	if (!editing || Cursor->type()!=TextLine::prompt) return;
	TextLine *l=Cursor;
	while (1)
	{	l=l->prev();
		if (!l) return;
		if (l->type()==TextLine::prompt) break;
	}
	Cursor=l;
	Cursorpos=1;
	set_editline(Cursor->text()+1,Cursorpos-1);
	updatetop();
	WindowPS wps(*W);
    setcaret(wps);
	updatecaret();
}

void Text::linedown ()
{	if (!editing || Cursor->type()!=TextLine::prompt) return;
	TextLine *l=Cursor;
	while (1)
	{	l=l->next();
		if (!l) return;
		if (l->type()==TextLine::prompt) break;
	}
	Cursor=l;
	Cursorpos=1;
	set_editline(Cursor->text()+1,Cursorpos-1);
	updatetop();
    WindowPS wps(*W);
	setcaret(wps);
	updatecaret();
}

void Text::removeoutput ()
{   TextLine *p=Cursor,*h;
	if (p->type()!=TextLine::prompt ||
		!strncmp(p->text(),">function",9)) return;
	p=p->next();
	while (p)
	{	if (p->type()==TextLine::prompt) break;
		if (p->type()==TextLine::comment) p=p->next();
		else
		{	h=p->next();
			delete p;
			p=h;
		}
	}
}

void Text::nextprompt ()
{   if (Cursor->type()!=TextLine::prompt &&
			Cursor->type()!=TextLine::udf)
		return;
	TextLine *p=Cursor;
	int c=Cursor->type();
	p=p->next();
	while (p)
	{	if (p->type()==c) break;
		if (p->type()==TextLine::prompt ||
			p->type()==TextLine::udf) return;
		p=p->next();
	}
	if (!p) return;
	if (Start==Cursor) Start=Cursor->next();
	delete Cursor;
	Cursor=p; Cursorpos=1;
	set_editline(Cursor->text()+1,0);
	updatetop();
	WindowPS ps(*W);
	setcaret(ps);
	drawbelow(ps,Top);
}

void Text::clear ()
{	TextLine *l=Start,*h;
	while (l)
	{	h=l->next();
		delete l;
		l=h;
	}
	Cursor=Start=Top=new TextLine; Cursorpos=0;
	unmarkall();
	W->update();
}

void Text::newprompt ()
{	Cursor->copy(">"); Cursorpos=1; Cursor->type(TextLine::prompt);
	set_editline("",0);
	W->update();
}

void Text::load (char *filename)
{   clear();
	FILE *in;
	in=fopen(filename,"r");
	if (!in)
	{	Warning("Could not open this file!","Euler",*W);
		return;
	}
	char line[1024];
	TextLine *l=0;
	while (!feof(in))
	{	if (!fgets(line,1024,in)) break;
		if (line[strlen(line)-1]=='\n') line[strlen(line)-1]=0;
		if (l) l=new TextLine(l,0);
		else l=Start=new TextLine(0,0);
		l->copy(line);
		switch (*l->text())
		{	case '>' : l->type(TextLine::prompt); break;
			case '$' : l->type(TextLine::udf); break;
			case '%' : l->type(TextLine::comment); break;
		}
	}
	fclose(in);
	if (!Start) newprompt();
	l=Start;
	while (l->type()!=TextLine::prompt)
	{	l=l->next();
		if (!l) break;
	}
	if (!l)
	{	l=Start;
		while (l->next()) l=l->next();
		l=new TextLine(l,0);
		l->copy(">");
	}
	Cursor=l; Cursorpos=1;
	set_editline(Cursor->text()+1,0);
	updatetop();
	W->update();
}

void Text::save (char *filename)
{   FILE *out=fopen(filename,"w");
	if (!out)
	{	Warning("Could not open this file!","Euler",*W);
		return;
	}
	TextLine *l=Start;
	while (l)
	{	if (l->type()==TextLine::output &&
			(*l->text()=='>' || *l->text()=='$' || *l->text()=='%'))
		{	fputc(' ',out);
		}
		fputs(l->text(),out);
		fputc('\n',out);
		l=l->next();
	}
	fclose(out);
}

void Text::settype ()
{   if (*Cursor->text()=='>') Cursor->type(TextLine::prompt);
	else if (*Cursor->text()=='$') Cursor->type(TextLine::udf);
	WindowPS wps(*W);
    drawcursorline(wps);
}

extern Help helper;

void Text::comment ()
{	Dialog d(*W,IDD_Comment,helper,editcomment);
	static char comment[16*1024l];
	static char line[256];
	strcpy(comment,"");
	TextLine *l=Cursor,*h;
	int linestart=1;
	char *p,*q;
	while (l->prev() && l->prev()->type()==TextLine::comment)
	{	l=l->prev();
	}
	while (l!=Cursor && l->type()==TextLine::comment)
	{	p=l->text()+1;
		while (*p==' ') p++;
		if (!*p)
		{	strcat(comment,"\r\n");
			linestart=1;
		}
		else
		{   if (!linestart) strcat(comment," ");
			strcat(comment,p);
			linestart=0;
		}
		q=comment+strlen(comment)-1;
		while (*q==' ') { *q=0; q--; }
		l=l->next();
	}
	MultilineItem edit(ID_Comment,d,comment);
	edit.setfont(*courier);
	d.carryout();
	strcpy(comment,edit);
	if (d.result()!=Dialog::ok) return;
	l=Cursor;
	while (l->prev() && l->prev()->type()==TextLine::comment)
	{	l=l->prev();
	}
	while (l!=Cursor)
	{	h=l->next();
		if (Start==l) Start=h;
		delete l;
		l=h;
	}
	p=comment;
	while (*p)
	{   q=line;
		*q++='%'; *q++=' ';
		while (*p==' ') p++;
		while (1)
		{	if (*p=='\r' || *p=='\n')
			{	p+=2;
				*q=0;
				l=new TextLine(Cursor->prev(),Cursor);
				l->copy(line); l->type(TextLine::comment);
				if (Start==Cursor) Start=l;
				l=new TextLine(Cursor->prev(),Cursor);
				l->copy("% "); l->type(TextLine::comment);
				break;
			}
			else if (*p==0 || (*p==' ' && q-line>60) || q-line>254)
			{	*q=0;
				l=new TextLine(Cursor->prev(),Cursor);
				l->copy(line); l->type(TextLine::comment);
				if (Start==Cursor) Start=l;
				break;
			}
			*q++=*p++;
		}
	}
	Top=Cursor;
	updatetop();
	W->update();
}

int Text::endfunction ()
{  	if (editing && Cursor->type()==TextLine::udf &&
			strlen(*Cursor)<=1)
	{	set_editline("endfunction",0);
		Cursor->copy("$endfunction"); Cursorpos=1;
		WindowPS ps(*W);
		setcaret(ps); updatecaret();
		drawcursorline(ps);
		return 1;
	}
	return 0;
}

void Text::setscroll ()
{	int na=0,nt=0;
	TextLine *h=Start;
	while (h)
	{	if (Top==h) na=nt;
		nt++;
		h=h->next();
	}
	if (nt>0) W->setscroll(StandardWindow::vscroll,na*100/nt);
	else W->setscroll(StandardWindow::vscroll,0);
}

void Text::setscroll (int pos)
{   int nt=0;
	TextLine *h=Start;
	while (h)
	{	nt++; h=h->next();
	}
	if (nt==0) return;
	nt=nt*pos/100;
	h=Start;
	while (h && nt>0)
	{	nt--;
		h=h->next();
	}
	if (h)
	{   Top=h;
		W->update();
	}
}

void Text::pageup ()
{   if (!editing) return;
	WindowPS ps(*W);
	getheight();
	int lines=ps.height()/ps.textheight()/2;
	while (lines>0 && Top->prev())
	{	Top=Top->prev();
		lines--;
	}
	drawbelow(ps,Top);
	setcaret(ps); updatecaret();
	setscroll();
}

void Text::pagedown ()
{   if (!editing) return;
	WindowPS ps(*W);
	getheight();
	int lines=ps.height()/ps.textheight()/2;
	while (lines>0 && Top->next())
	{	Top=Top->next();
		lines--;
	}
	drawbelow(ps,Top);
	setcaret(ps); updatecaret();
	setscroll();
}

void Text::scrollup ()
{   // if (!editing) return;
	WindowPS ps(*W);
	if (Top->prev()) Top=Top->prev();
	drawbelow(ps,Top);
	setcaret(ps); updatecaret();
	setscroll();
}

void Text::scrolldown ()
{   // if (!editing) return;
	WindowPS ps(*W);
	if (Top->next()) Top=Top->next();
	drawbelow(ps,Top);
	setcaret(ps); updatecaret();
	setscroll();
}

void Text::showcursor ()
{	TextLine *h=top(Cursor);
	if (h==Top) return;
    Top=h;
	WindowPS ps(*W);
	drawbelow(ps,Top);
	setcaret(ps); updatecaret();
}

void Text::computecursor (int x, int y, TextLine * &l, int &pos)
{   if (x<0) x=0;
	if (!editing || Cursor->type()!=TextLine::prompt) return;
	WindowPS ps(*W);
	setfont(ps);
	int line=y/textheight();
	TextLine *h=Top;
	while (line>0)
	{   if (!h->next())
		{	if (line==1) break;
			else
			{	l=h; pos=strlen(l->text()); return;
			}
		}
		h=h->next();
		line--;
	}
	l=h;
	pos=strlen(l->text());
	String Work(h->text());
	int w,ht;
	x+=Offset;
	while (1)
	{   Work.text()[pos]=0;
		ps.textextent(Work,w,ht);
		if (w<=x) break;
		pos--;
	}
}

void Text::selectall ()
{	Markend=Markstart=Start; Markstartpos=0;
	while (Markend->next())
	{   Markend->mark(1);
		Markend=Markend->next();
	}
	Markendpos=strlen(Markend->text());
	redraw();
}

int Text::startdrag (int x, int y)
{	if (!waiting || !editing || Cursor->type()!=TextLine::prompt)
		return 0;
	computecursor(x,y,Dragline,Dragpos);
	return 1;
}

void Text::drag (int x, int y)
{	TextLine *l;
	int pos;
	computecursor(x,y,l,pos);
	mark(l,pos);
}

void Text::enddrag (int x, int y)
{   if (Dragline->type()!=TextLine::prompt) return;
	TextLine *l;
	int pos;
	computecursor(x,y,l,pos);
	if (l==Dragline && pos==Dragpos)
	{	Cursor=Dragline; Cursorpos=Dragpos;
		if (Cursorpos<1) Cursorpos=1;
		set_editline(Cursor->text()+1,Cursorpos-1);
		W->update();
		return;
	}
	mark(l,pos);
}

void Text::unmarkall ()
{   TextLine *h=Start;
	while (h)
	{	h->mark(0); h=h->next();
	}
	Markstart=Markend=0;
}

void Text::unmark ()
{   if (!Markstart) return;
	unmarkall();
	WindowPS ps(*W);
	drawbelow(ps,Top);
	setcaret(ps); updatecaret();
}

void Text::redraw ()
{	WindowPS ps(*W);
	drawbelow(ps,Top);
	setcaret(ps); updatecaret();
}

void Text::mark (TextLine *l, int pos)
{	TextLine *h=Start;
	if (l==Dragline)
	{   unmarkall();
		if (Dragpos==pos) return;
		Markstart=Markend=Dragline;
		if (pos<Dragpos)
		{	Markstartpos=pos; Markendpos=Dragpos;
		}
		else
		{	Markstartpos=Dragpos; Markendpos=pos;
		}
		l->mark(1);
		redraw();
        return;
	}
	int m=0;
	while (h)
	{	if (m==0 && h==l)
		{	m=1; h->mark(m);
			Markstart=l; Markstartpos=pos;
		}
		else if (m==0 && h==Dragline)
		{	m=1; h->mark(m);
			Markstart=Dragline; Markstartpos=Dragpos;
		}
		else if (m==1 && h==l)
		{	h->mark(m); m=0;
			Markend=l; Markendpos=pos;
		}
		else if (m==1 && h==Dragline)
		{	h->mark(m); m=0;
			Markend=Dragline; Markendpos=Dragpos;
		}
		else h->mark(m);
		h=h->next();
	}
	redraw();
}

void Text::deleteoutput ()
{   if (!waiting) return;
	TextLine *h=Markstart,*n;
	while (h)
	{   n=h->next();
		if (h->type()==TextLine::output)
		{   if (Start==h) Start=n;
			delete h;
		}
		h=n;
	}
	unmarkall();
	Top=top(Cursor);
	W->update();
}

void Text::set (char *s)
{	Cursor->copy(">"); Cursor->cat(s); Cursorpos=1;
	set_editline(Cursor->text()+1,0);
	WindowPS ps(*W);
	setcaret(ps); updatecaret();
	drawcursorline(ps);
	W->key(Keycode::charkey|Keycode::down,13,13);
}

void Text::deleteudf ()
{   if (!waiting || !editing || Cursor->type()!=TextLine::udf)
		return;
	if (!Cursor->next() || Cursor->next()->type()!=TextLine::udf)
		return;
	TextLine *h=Cursor->next();
	if (Cursor==Start) Start=h;
	delete (Cursor);
	Cursor=h;
	set_editline(Cursor->text()+1,0); Cursorpos=1;
	Top=top(Cursor);
	W->update();
}

void Text::deletecommand ()
{   if (Cursor->type()==TextLine::udf)
	{	deleteudf(); return;
	}
	if (!waiting || !editing || Cursor->type()!=TextLine::prompt
		|| Cursor->next()==0)
		return;
	TextLine *l=Cursor,*h,*k;
	while (l->prev() && l->prev()->type()==TextLine::comment)
		l=l->prev();
	k=l->prev();
	while (l->type()==TextLine::comment)
	{   if (Start==l) Start=l->next();
		h=l->next();
		delete l;
		l=h;
	}
	if (Start==l) Start=l->next();
	h=l->next();
	delete l;
	l=h;
	while (l && l->type()==TextLine::output ||
		l->type()==TextLine::udf)
	{   if (Start==l) Start=l->next();
		h=l->next();
		delete l;
		l=h;
	}
	if (!l)
	{	if (k) Cursor=new TextLine(k,0,">");
		else Start=Cursor=new TextLine(0,0,">");
	}
	else
	{   k=l;
		while (k && k->type()!=TextLine::prompt)
		{	k=k->next();
		}
		if (k) Cursor=k;
		else
		{	if (l==Start)
			{	Start=Cursor=new TextLine(0,l,">");
			}
			else Cursor=new TextLine(l->prev(),l,">");
		}
	}
	Cursor->type(TextLine::prompt);
	set_editline(Cursor->text()+1,0); Cursorpos=1;
	Top=top(Cursor);
	W->update();
}

void Text::insertcommand ()
{	int type=Cursor->type();
	if (!waiting || !editing || (
		type!=TextLine::prompt && type!=TextLine::udf))
		return;
	TextLine *l=Cursor;
	while (l->prev() && l->prev()->type()==TextLine::comment)
		l=l->prev();
	Cursor=new TextLine(l->prev(),l);
	if (Start==l) Start=Cursor;
	if (type==TextLine::prompt)
	{	Cursor->copy(">"); Cursor->type(TextLine::prompt);
	}
	else
	{	Cursor->copy("$"); Cursor->type(TextLine::udf);
	}
	set_editline("",0); Cursorpos=1;
	Top=top(Cursor);
	W->update();
}

void Text::copytext ()
{	if (!waiting || Markstart==0) return;
	int l=1;
	TextLine *h=Start;
	while (h)
	{	if (h->mark()) l+=strlen(h->text())+2;
		h=h->next();
	}
	char *s=new char[l],*p;
	p=s;
	h=Start;
	while (h)
	{	if (h->mark())
		{	if (h==Markstart)
			{   if (h==Markend)
				{	memcpy(p,h->text()+Markstartpos,
						Markendpos-Markstartpos);
					p+=Markendpos-Markstartpos;
				}
				else
				{	strcpy(p,h->text()+Markstartpos); p+=strlen(p);
				}
			}
			else if (h==Markend)
			{	strcpy(p,h->text()); p+=Markendpos;
			}
			else
			{	strcpy(p,h->text()); p+=strlen(p);
			}
			*p++=13; *p++=10;
		}
		h=h->next();
	}
	*p=0;
	Clipboard c(*W);
	c.copy(s);
	delete s;
}

void Text::copycommands ()
{	if (!waiting || Markstart==0) return;
	int l=1;
	TextLine *h=Start;
	while (h)
	{	if (h->mark() &&
			(h->type()==TextLine::prompt || h->type()==TextLine::udf))
			l+=strlen(h->text())+2;
		h=h->next();
	}
	char *s=new char[l],*p;
	p=s;
	h=Start;
	while (h)
	{	if (h->mark() &&
			(h->type()==TextLine::prompt || h->type()==TextLine::udf)
			&& strlen(h->text())>0)
		{	strcpy(p,h->text()+1); p+=strlen(p);
			*p++=13; *p++=10;
		}
		h=h->next();
	}
	*p=0;
	Clipboard c(*W);
	c.copy(s);
	delete s;
}

void Text::pastecommands ()
{   if (!editing || !waiting || Cursor->type()!=TextLine::prompt) return;
	Clipboard c(*W);
	char *s=c.gettext();
	if (*s==0) return;
	char *p=s,ch;
	while (*p && *p!=13) p++;
	if (strlen(p)<3)
	{	*p=0;
		Cursor->insert(Cursorpos,s);
		set_editline(Cursor->text()+1,Cursorpos-1);
	}
	else
	{   TextLine *h=Cursor,*hn;
		while (h->next() && h->next()->type()==TextLine::output)
		{	h=h->next();
		}
		while (1)
		{   ch=*p;
			*p=0;
			hn=new TextLine(h,h->next(),">");
			hn->type(TextLine::prompt);
			hn->cat(s);
			h=hn;
			*p=ch; if (*p==0) break;
			p++;
			if (*p!=10) break;
			p++;
			s=p;
			while (*p!=0 && *p!=13) p++;
		}
	}
	W->update();
}

//***************** Main Window ********************

WindowClass eulert("EULERT",IDI_Notebook);

class TextWindow : public StandardWindow
{   int Code,Scan,Dragscroll;
	Timer *T;
	public :
	enum { message_quit,message_caret,message_wantload };
	TextWindow (char *title) :
		StandardWindow(title,eulert,defaultsize,defaultsize,
			defaultstyle|vscroll),
		Dragscroll(0)
	{	text=new Text(*this);
		usecaret();
	}
	virtual void redraw (PS &ps)
	{	text->redraw();
		text->setscroll();
	}
	virtual void key (int flags, int code, int scan);
	int code () { int h=Code; Code=0; return h; }
	int scan () { int h=Scan; Scan=0; return h; }
	int scanescape ()
	{   int h=Scan;
		if (h!=escape) return 0;
		Scan=0; return h;
	}
	virtual void user (int message, Parameter p);
	virtual void sized ();
	virtual void timer (int n);
	virtual void starttimer (long msec)
	{	T=new Timer(*this,msec,1);
	}
	virtual void clicked (int x, int y, int type);
	virtual void scroll (int flags, int type, int pos);
	virtual void statechanged (int s);
	virtual int close ();
} textwindow("Euler");

int TextWindow::close ()
{   if (wantquit) return 1;
	if (Question("Really quit?","Euler",textwindow)==Answers::no) return 0;
	wantquit=1;
	return 1;
}

FileSelector savenotebook(textwindow,"*.en",FileSelector::save,
	"Save Notebook","en");
FileSelector loadnotebook(textwindow,"*.en",FileSelector::load,
	"Load Notebook","en");
String NotebookName("");
String FileName("",256);
String WindowTitle("",256);
FileSelector loadfile(textwindow,"*.e",FileSelector::load,
	"Load EULER File","e");
FileSelector savebitmap(textwindow,"*.bmp",FileSelector::save,
	"Save Graphics as Bitmap","bmp");
FileSelector savepostscript(textwindow,"*.eps",FileSelector::save,
	"Save Graphics as Postscript","eps");
FileSelector savemetafile(textwindow,"*.wmf",FileSelector::save,
	"Save Graphics as Metafile","wmf");
FileSelector saveeditor(textwindow,"*.e",FileSelector::save,
	"Save Editor","e");

void setname (void);

void TextWindow::user (int message, Parameter p)
{	if (message==message_quit) doexit();
	else if (message==message_caret && waiting)
	{	text->updatecaret();
		if (!CaretOn)
		{	showcaret(); CaretOn.set();
		}
	}
	else if (message==message_wantload)
	{	if (editing && !wantload.empty())
		{	text->load(wantload);
			FileName.copy(wantload);
			::setname();
		}
		wantload.copy("");
	}
}


Help helper("euler.hlp",textwindow);

void TextWindow::sized ()
{   delete courier;
	courier=new Font(TextFontName,tfontheight);
	WindowPS ps(*this);
	ps.font(*courier);
	linelength=width()/ps.textavewidth()-5;
	if (text) text->sized();
}

void TextWindow::scroll (int flags, int type, int pos)
{   switch (type)
	{	case pageup :
			text->pageup();
			break;
		case pagedown :
			text->pagedown();
			break;
		case lineup :
			text->scrollup();
			break;
		case linedown :
			text->scrolldown();
			break;
		case scrollposition :
			text->setscroll(pos);
            break;
	}
}

//****************** Menu *******************************

Menu menu(textwindow);

//****************** Graphics Window ********************

WindowClass eulerg("EULERG",IDI_Euler,WindowClass::noclose);

class GraphicsWindow : public StandardWindow
{   BitmapPS *B;
	Timer *T,*Tsized;
	double X,Y;
	public :
	GraphicsWindow (char *title)
		: StandardWindow(title,eulerg)
			,B(0),T(0),Tsized(0)
	{
	}
	virtual void sized ();
	void timedsized ();
	virtual void redraw (PS &ps);
	virtual void timer (int n);
	void starttimer ()
	{	T=new Timer(*this,500,1);
	}
	int stoptimer ();
	virtual void key (int f, int c, int s)
	{   textwindow.key(f,c,s);
	}
	virtual void clicked (int x, int y, int type);
	void mouse (double &x, double &y) { x=X; y=Y; }
	BitmapPS *bitmap () { return B; }
	virtual void statechanged (int s);
} graphicswindow("Euler");

void TextWindow::statechanged (int s)
{	if (s==StandardWindow::iconic) graphicswindow.hide();
	else if (s==StandardWindow::normal) graphicswindow.show();
}

void GraphicsWindow::statechanged (int s)
{	if (s==StandardWindow::iconic) textwindow.hide();
	else if (s==StandardWindow::normal) textwindow.show();
}

//*************** Frames ****************

class Frame
{	Meta *M;
	Frame *Next;
	public :
	Frame ();
	Frame (int w, int h);
	~Frame ();
	void next (Frame *next) { Next=next; }
	Frame *next () { return Next; }
	Meta *getMeta () { return M; }
	void copyfrom (Meta *meta)
	{	M=new WindowsMeta(meta);
	}
};

Frame::Frame () : Next(0)
{	copyfrom(meta);
}

Frame::Frame (int w, int h) : Next(0)
{	copyfrom(meta);
}

Frame::~Frame ()
{	delete M;
}

class Frames
{	Frame *First;
	public :
	Frames () : First(0) {}
	~Frames ();
	void append ();
	void append (int w, int h);
	void remove ();
	void show (int n);
	int nframes ();
	void copyto (int n);
	void copyfrom (int n);
};

Frames::~Frames ()
{	remove();
}

void Frames::remove ()
{	Frame *p=First,*h;
	while (p)
	{	h=p->next();
		delete p;
		p=h;
	}
	First=0;
}

void Frames::append ()
{	Frame *p=First;
	if (!p)
	{	First=new Frame();
	}
	else
	{	while (p->next()) p=p->next();
		p->next(new Frame());
	}
}

void Frames::append (int w, int h)
{	Frame *p=First;
	if (!p)
	{	First=new Frame(w,h);
	}
	else
	{	while (p->next()) p=p->next();
		p->next(new Frame(w,h));
	}
}

void Frames::show (int n)
{	if (!First) return;
	Frame *p=First;
	int i=0;
	while (p && i<n)
	{	i++; p=p->next();
	}
	if (!p) return;
	if (usecolors) ops->erase();
	else ops->bar(0,0,ops->width(),ops->height(),MyColor(0));
	critical.enter();
	p->getMeta()->replay(ops);
	critical.leave();
	WindowPS ps(graphicswindow);
	graphicswindow.redraw(ps);
	((WindowsMeta *)meta)->Draw=0;
}

void Frames::copyto (int n)
{	if (!First) return;
	Frame *p=First;
	int i=0;
	while (p && i<n)
	{	i++; p=p->next();
	}
	if (!p) return;
	p->copyfrom(meta);
}

void Frames::copyfrom (int n)
{	if (!First) return;
	Frame *p=First;
	int i=0;
	while (p && i<n)
	{	i++; p=p->next();
	}
	if (!p) return;
	p->getMeta()->replay(ops);
}

int Frames::nframes ()
{	if (!First) return 0;
	int i=0;
	Frame *p=First;
	while (p) { i++; p=p->next(); }
	return i;
}

Frames frames;

int nframes ()
{	return frames.nframes();
}

void addframe (void)
{   frames.append();
}

void addframe (int w, int h)
{	frames.append(w,h);
}

void deleteframes ()
{	frames.remove();
	showframe(0);
}

void showframe (int n)
{   if (n<=0)
	{   ((WindowsMeta *)meta)->Draw=1;
		critical.enter();
		meta->replay(ops);
		critical.leave();
		graphicswindow.update();
	}
	else
    {	frames.show(n-1);
    }
}

void copytoframe (int n)
{	frames.copyto(n-1);
}

void copyfromframe (int n)
{	frames.copyfrom(n-1);
}

void GraphicsWindow::redraw (PS &ps)
{	if (!B)
	{   sized(); return;
	}
	B->copy(ps);
}

void GraphicsWindow::timer (int n)
{   if (n==1)
	{	if (B)
		{   critical.enter(); B->copy(); critical.leave();
		}
	}
	else if (n==3) timedsized();
}

int GraphicsWindow::stoptimer ()
{   int r=(T!=0);
	if (T)
	{   if (B) B->copy();
		delete T;
	}
	T=0;
	return r;
}

//**************** Computation thread *******************

#include <signal.h>

/*
void Catcher (int reglist)
{   error=1;
	signal(SIGFPE, Catcher);
}

void sethandler (void)
{   signal(SIGFPE, Catcher);
}
*/

int _matherr (struct _exception *except)
{   error=1;
	return 0;
}

int fcompute ();

class ComputationThread : public Thread
{	int Argc;
	char **Argv;
	public :
	ComputationThread () : Thread(fcompute) {}
	void set (int n, char **a) { Argc=n; Argv=a; }
	int argc () { return Argc; }
	char ** argv () { return Argv; }
} computation;

int fcompute ()
{   //sethandler();
	main_loop(computation.argc(),computation.argv());
	wantquit=1;
	textwindow.usermessage(TextWindow::message_quit);
    return 0;
}

void TextWindow::key (int flags, int code, int scan)
{   if (!(flags&Keycode::down)) return;
	if (waiting && editing && scan!=VK_PRIOR && scan!=VK_NEXT)
	{   text->showcursor();
	}
	if (flags&Keycode::charkey)
	{	Code=code; Scan=0;
		switch (Code)
		{	case 13 : Scan=enter; Code=13; break;
			case 27 :
#ifdef YACAS
				interrupt_yacas();
#endif
				if (mousewaiting) { Code=27; computation.resume(); return; }
				Scan=escape; Code=27; break;
			case 8 : Scan=backspace; Code=0; break;
			case 9 : Scan=switch_screen; Code=9; break;
		}
	}
	else if (flags&Keycode::virtualkey)
	{   Code=0;
		switch (scan)
		{	case VK_CANCEL :
				Scan=escape; break;
			case VK_UP :
				if (!editing || Shift()) Scan=cursor_up;
				else { text->lineup(); return; }
				break;
			case VK_DOWN :
				if (!editing || Shift()) Scan=cursor_down;
				else { text->linedown(); return; }
				break;
			case VK_LEFT :
				if (Shift()) Scan=word_left;
				else Scan=cursor_left;
				break;
			case VK_RIGHT :
				if (Shift()) Scan=word_right;
				else Scan=cursor_right;
				break;
			case VK_DELETE :
				Scan=deletekey; break;
			case VK_INSERT :
				Scan=help; break;
			case VK_HOME :
				Scan=line_start; break;
			case VK_END :
            	if (text->endfunction()) break;
				Scan=line_end; break;
			case VK_F1 :
				Scan=fk1; break;
			case VK_F2 :
				Scan=fk2; break;
			case VK_F4 :
				Scan=fk3; break;
			case VK_F5 :
				Scan=fk5; break;
			case VK_F6 :
				Scan=fk6; break;
			case VK_F7 :
				Scan=fk7; break;
			case VK_F8 :
				Scan=fk8; break;
			case VK_F9 :
				Scan=fk9; break;
			case VK_F10 :
				Scan=fk10; break;
			case VK_PRIOR :
				text->pageup(); return;
			case VK_NEXT :
				text->pagedown(); return;
			default : return;
		}
	}
	else return;
	if (waiting)
	{	if (CaretOn)
		{	CaretOn.clear(); hidecaret();
		}
		computation.resume();
		Sleep(0);
	}
	if (T)
	{	T->kill(); T=0;
	}
}

void TextWindow::clicked (int x, int y, int type)
{   static int dragging=0;
	static Timer *t=0;
	switch (type)
	{   case ClickType::button1down :
    		if (!dragging)
            {	text->unmarkall();
            	text->redraw();
            }
			dragging=text->startdrag(x,y);
			if (dragging) capture(1);
			break;
		case ClickType::mousemove :
			if (!dragging) break;
			if (y<0 || y>height())
			{	if (!t) t=new Timer(*this,20,2);
				Dragscroll=(y<0)?-1:1;
				break;
			}
			if (t) delete t;
			t=0; Dragscroll=0;
			text->drag(x,y);
			break;
		case ClickType::button1up :
			if (t) delete t;
			t=0; Dragscroll=0;
			if (dragging)
			{   text->enddrag(x,y); capture(0); dragging=0;
			}
			break;
	}
}

void TextWindow::timer (int n)
{   if (n==1)
	{	if (T) T->kill(); T=0;
		Scan=Code=0;
		if (waiting) computation.resume();
	}
	else if (n==2)
	{	if (Dragscroll==1)
		{	text->scrolldown();
			text->drag(0,height()+100);
		}
		else if (Dragscroll==-1)
		{	text->scrollup();
			text->drag(0,-100);
		}
	}
}

void GraphicsWindow::clicked (int x, int y, int type)
{	if (!mousewaiting || type!=ClickType::button1down) return;
	X=x*1024.0/width(); Y=y*1024.0/height();
	computation.resume();
}

int doreplay ()
{   critical.enter();
	meta->replay(ops);
	critical.leave();
	graphicswindow.update();
	return 0;
}

Thread graphicsreplay (doreplay);

void GraphicsWindow::sized ()
{	if (Tsized) delete Tsized;
	Tsized=new Timer(*this,250,3);
}

void GraphicsWindow::timedsized ()
{   if (Tsized) delete Tsized;
	Tsized=0;
	BitmapPS *Bnew,*Bold;
	computation.suspend();
	frames.remove();
	Bnew=new BitmapPS(*this);
	if (usecolors) Bnew->erase();
	else Bnew->bar(0,0,width(),height(),MyColor(0));
	ops=(PS *)Bnew;
	Font *font=new Font(GraphicsFontName,height()/gscreenlines);
	if (Bnew->height()>0)
		hchar=(int)((double)Bnew->textheight()/Bnew->height()*1024);
	if (Bnew->width()>0)
		wchar=(int)((double)Bnew->textavewidth()/Bnew->width()*1024);
	Bold=B; B=Bnew;
	if (Bold) delete Bold;
	delete couriersmall;
	couriersmall=font;
	B->font(*couriersmall);
	couriersmallup=new Font(GraphicsFontName,height()/gscreenlines,
    	Font::normal,0,900);
    B->rememberfont(1,*couriersmallup);
	couriersmalldown=new Font(GraphicsFontName,height()/gscreenlines,
    	Font::normal,0,2700);
    B->rememberfont(2,*couriersmalldown);
	computation.resume();
	graphicsreplay.start();
}

/****
The graphic screen has coordinates from 0.0 to 1024.0 (double).
There should be a function, which computes the correct screen
coordinates from these internal ones.
****/

void graphic_mode (void)
/***** graphics
	Switch to graphics. Text must not be deleted.
	On a window system make graphics visible.
*****/
{   if (!textmode) return;
	graphicswindow.top();
	graphicswindow.foreground();
	textmode=0;
	graphicswindow.starttimer();
}

void text_mode (void)
/***** text_mode
	Switch to text. Graphics should not be deleted.
	On a window system make text visible.
*****/
{   if (textmode) return;
	textwindow.top();
	textwindow.foreground();
	textmode=1;
	graphicswindow.stoptimer();
}

/******************* Graphics ***************************/

void gline (PS &ops, double c, double r, double c1, double r1, int color,
	int st, int width)
/***** gline
	draw a line.
	col is the color, where 0 should be white and 1 black.
	st is a style from linetyp.
	width is the linewidth, where 0 or 1 are equal defaults.
*****/
{   if (!usecolors) color=1;
	if (width<2 && st==line_solid)
	{	ops.color(MyColor(color));
		ops.line(col(ops,c),row(ops,r),
			col(ops,c1),row(ops,r1));
	}
	else
	{	Pen *pen=0;
		switch (st)
		{	case line_solid :
			case line_arrow :
				pen=new Pen(Pen::solid,width,MyColor(color)); break;
			case line_dotted :
				pen=new Pen(Pen::dot,width,MyColor(color)); break;
			case line_dashed :
				pen=new Pen(Pen::dash,width,MyColor(color)); break;
			case line_none : return;
		}
		if (!pen) return;
		ops.line(col(ops,c),row(ops,r),
				col(ops,c1),row(ops,r1),*pen);
		if (st==line_arrow)
		{	double hx,hy,rr;
			hx=c1-c; hy=r1-r; rr=sqrt(hx*hx+hy*hy);
			if (rr>1)
			{	hx*=1024.0/markerfactor/rr;
				hy*=1024.0/markerfactor/rr;
				ops.line(col(ops,c1),row(ops,r1),
					col(ops,c1-hx-hy),row(ops,r1-hy+hx),*pen);
				ops.line(col(ops,c1),row(ops,r1),
					col(ops,c1-hx+hy),row(ops,r1-hy-hx),*pen);
			}
		}
		delete pen;
	}
}

void gclip (PS &ops, double c, double r, double c1, double r1)
/***** gclip
	clip graphics to a specified region.
**********/
{	ops.clip(col(ops,c),row(ops,r),col(ops,c1-c)+2,row(ops,r1-r)+2);
}

void gmarker (PS &ops, double c, double r, int color, int type)
/***** gmarker
	plot a single marker on screen.
	col is the color.
	type is a type from markertyp.
*****/
{	int t;
	if (!usecolors) color=1;
	switch (type)
	{	case marker_cross : t=Markers::cross; break;
		case marker_circle : t=Markers::circle; break;
		case marker_diamond : t=Markers::diamond; break;
		case marker_dot : t=Markers::dot; break;
		case marker_plus : t=Markers::plus; break;
		case marker_square : t=Markers::square; break;
		case marker_star : t=Markers::sixpointstar; break;
	}
	ops.markersize(ops.width()/markerfactor);
	ops.color(MyColor(color));
	ops.mark(col(ops,c),row(ops,r),t);
}

void gfill (PS &ops, double c[], int st, int n, int connect[])
/***** gfill
	fill an area given by n pairs of points (in c: x,y,x,y,...)
	with the style st from filltyp.
	connect pairs of points indicated in connect by a black line.
*****/
{	int i;
	static Points p(16);
	p.empty();
	for (i=0; i<n; i++)
	{	p.add(col(ops,c[2*i]),row(ops,c[2*i+1]));
	}
	p.add(col(ops,c[0]),row(ops,c[1]));
	if (!usecolors) ops.fillpolygon(p,MyColor(0));
	else
	{	if (st==fill_filled) ops.fillpolygon(p,MyColor(fillcolor1));
		else ops.fillpolygon(p,MyColor(fillcolor2));
	}
	ops.color(MyColor(1));
	for (i=0; i<n; i++)
	{	if (connect[i])
		{	ops.line(p.x(i),p.y(i),p.x(i+1),p.y(i+1));
		}
	}
}

void gfillh (PS &ops, double c[8], double hue, int color, int connect)
/***** Draw a filled polygon.
	Works like gfill, but uses hue.
*****/
{	int i;
	static Points p(16);
	hue-=floor(hue);
	p.empty();
    if (!usecolors) { color=1; hue=1; }
	for (i=0; i<4; i++)
	{	p.add(col(ops,c[2*i]),row(ops,c[2*i+1]));
	}
	p.add(col(ops,c[0]),row(ops,c[1]));
	if (connect)
		ops.polygon(p,MyColor(color,hue-floor(hue)),MyColor(1));
	else
    	ops.fillpolygon(p,MyColor(color,hue-floor(hue)));
	ops.color(MyColor(1));
}

void gbar (PS &ops, double c, double r, double c1, double r1, double hue,
	int color, int style)
/***** Draw a rectangle.
	hue is a hue intensity from 0 to 1.
	style determines, if a black boundary should be drawn.
******/
{	int x=col(ops,c),y=row(ops,r);
	int w=col(ops,c1)-x+1,h=row(ops,r1)-y+1;
	Color col;
	col=MyColor(usecolors?color:1,hue-floor(hue));
	ops.bar(x,y,w,h,col);
    if (style!=0)
	{	ops.color(MyColor(1));
	 	ops.rectangle(x,y,w,h);
    }
}

void gbar1 (PS &ops, double c, double r, double c1, double r1,
	int color, int style)
/***** Draw a rectangle.
	hue is a hue intensity from 0 to 1.
	style determines, if a black boundary should be drawn.
******/
{	int x=col(ops,c),y=row(ops,r);
	int w=col(ops,c1)-x+1,h=row(ops,r1)-y+1;
	Color col;
	col=MyColor(usecolors?color:1);
	Brush *b;
	switch (style)
	{	case bar_solid :
			ops.bar(x,y,w,h,col); break;
		case bar_framed :
			if (!usecolors) goto hatch;
			ops.color(MyColor(1));
			ops.filledrectangle(x,y,w,h,col);
			break;
		case bar_frame :
			ops.color(MyColor(1));
			ops.rectangle(x,y,w,h); break;
		case bar_hhatch :
			b=new HatchBrush(HatchBrush::horizontal,col); goto draw;
		case bar_vhatch :
			b=new HatchBrush(HatchBrush::vertical,col); goto draw;
		case bar_diagonal1 :
			b=new HatchBrush(HatchBrush::fdiagonal,col); goto draw;
		case bar_diagonal2 :
			b=new HatchBrush(HatchBrush::bdiagonal,col); goto draw;
		case bar_cross :
			hatch :
			b=new HatchBrush(HatchBrush::diagcross,col);
			draw :
			ops.backmode(PS::opaque);
			if (!usecolors) ops.backcolor(MyColor(0));
			ops.bar(x,y,w,h,*b);
			ops.color(col);
			ops.rectangle(x,y,w,h);
			ops.backmode(PS::transparent);
			if (!usecolors) ops.backcolor(Color::background());
			delete b;
			break;
	}
}

void gtext (PS &ops, double c, double r, char *text, int color, int alignment)
/***** gtext
	output a graphic text on screen.
	alignment is left=0, centered=1, right=2.
*****/
{	int a;
	switch (alignment)
	{	case 0 : a=Alignment::left; break;
		case 1 : a=Alignment::center; break;
		case 2 : a=Alignment::right; break;
	}
	if (usecolors) ops.textcolor(MyColor(color));
	else ops.textcolor(MyColor(1));
	ops.text(col(ops,c),row(ops,r),text,a,Alignment::top);
}

void gvtext (PS &ops, double c, double r, char *text, int color, int alignment)
/***** gtext
	output a graphic text on screen.
	alignment is left=0, centered=1, right=2.
*****/
{	int a;
	switch (alignment)
	{	case 0 : a=Alignment::left; break;
		case 1 : a=Alignment::center; break;
		case 2 : a=Alignment::right; break;
	}
    ops.font(2);
	if (usecolors) ops.textcolor(MyColor(color));
	else ops.textcolor(MyColor(1));
	ops.text(col(ops,c),row(ops,r),text,a,Alignment::top);
    ops.font(0);
}

void gvutext (PS &ops, double c, double r, char *text, int color, int alignment)
/***** gtext
	output a graphic text on screen.
	alignment is left=0, centered=1, right=2.
*****/
{	int a;
	switch (alignment)
	{	case 0 : a=Alignment::left; break;
		case 1 : a=Alignment::center; break;
		case 2 : a=Alignment::right; break;
	}
    ops.font(1);
	if (usecolors) ops.textcolor(MyColor(color));
	else ops.textcolor(MyColor(1));
	ops.text(col(ops,c),row(ops,r),text,a,Alignment::top);
    ops.font(0);
}

/*********************** input routines **********************/

void mouse (double *x, double *y)
/****** mouse
	wait, until the user marked a screen point with the mouse.
	Return screen coordinates.
******/
{   mousewaiting=1;
	int t=graphicswindow.stoptimer();
	computation.suspend();
	if (t) graphicswindow.starttimer();
	mousewaiting=0;
	if (textwindow.code()==27) { *x=-1; *y=-1; }
	else graphicswindow.mouse(*x,*y);
}

int wait_key (int *scan)
/*****
	wait for a keystroke. return the scancode and the ascii code.
	scancode should be a code from scantyp. Do at least generate
	'enter'.
*****/
{   waiting=1;
	textwindow.usermessage(TextWindow::message_caret);
	if (!wantload.empty()) textwindow.usermessage(TextWindow::message_wantload);
	int t=graphicswindow.stoptimer();
	computation.suspend();
	if (t) graphicswindow.starttimer();
	waiting=0;
	*scan=textwindow.scan();
	int code=textwindow.code();
    if (code!=0) text->changed(1);
	return code;
}

int test_key (void)
/***** test_key
	see, if user pressed the keyboard.
	return the scancode, if he did.
*****/
{   return textwindow.scanescape();
}

int test_code (void)
{   return textwindow.code();
}

/**************** directory *******************/

char dirpath[256];

#include <io.h>
#include <direct.h>

char *cd (char *dir)
/***** sets the path if dir!=0 and returns the path
*****/
{	static char path[256];
	chdir(dir);
	if (getcwd(path,256)) return path;
	return dir;
}

char *dir (char *pattern)
/***** Search a pattern if not 0, else research 
******/
{	static struct _finddata_t ff;
	static int another=0;
	static long handle;
	if (another && !pattern)
	{	if (_findnext(handle,&ff)==-1)
		{	another=0; return 0;
		}
		else return ff.name;
	}
	if (pattern)
	{	handle=_findfirst(pattern,&ff);
		if (handle!=-1)
		{	another=1; return ff.name;
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
{   doreplay();
	// graphicswindow.update();
	text->setscroll();
	if (time<1)
	{	*scan=0;
		Sleep((long)(time*1000));
		return; 
	}
	textwindow.starttimer((long)(time*1000));
	waiting=1;
	int t=graphicswindow.stoptimer();
	computation.suspend();
	if (t) graphicswindow.starttimer();
	waiting=0;
	if (time>=1) *scan=textwindow.scan();
	else *scan=0;
}

/***************** div. ***********************************/

void getpixelsize (double *x, double *y)
/***** Compute the size of pixel in screen coordinates.
******/
{	*x=1024.0/graphicswindow.width();
	*y=1024.0/graphicswindow.height();
}

void gflush (void)
/***** Flush out remaining graphic commands (for multitasking).
This serves to synchronize the graphics on multitasking systems.
******/
{
}

void sys_playwav (char *filename)
{	PlaySound(filename,0,SND_FILENAME);
}

/**************** Text screen ***************************/

void clear_screen (void)
/***** Clear the text screen
******/
{	text->clear();
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
{	text->cursorleft();
}

void move_cr (void)
/* move the text cursor right */
{	text->cursorright();
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
{	text->delline();
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
{   if (!textwindow.focus())
	{	textwindow.top();
    	textwindow.foreground();
	}
	if (*s!='\n') text->delline();
	text->inserttext(s);
}

void edit_off (void)
/* the command line is no longer in use (graphics or computing) */
{	editing=0;
	computation.priority(0);
	text->removeoutput();
}

void edit_on (void)
/* the command line is active */
{	editing=1;
	computation.priority(2);
	showframe(0);
	graphicswindow.update();
	text->settype();
	text->nextprompt();
	text->setscroll();
}

/********** execute programs *************************/

int execute (char *name, char *args)
/**** execute
	Call an external program, return 0, if there was no error.
	No need to support this on multitasking systems.
****/
{	_spawnlp(_P_NOWAIT,name,name,args,
		NULL);
	return 0;
}

// **** Several menu commands ****

void dosave (void);

void doexit ()
{  if (!wantquit &&
		  text->changed() &&
			Question("Save Notebook before quitting?",
				"Euler",textwindow)==Answers::yes
		  )
			dosave();
	textwindow.quit();
}

void setname ()
{	NotebookName.copy(FileName);
	NotebookName.copy(NotebookName.filename());
	*NotebookName.extension()=0;
	sprintf(WindowTitle,"Euler (%s)",NotebookName.text());
	textwindow.title(WindowTitle);
}

void donew ()
{   if (!editing || text->type()!=TextLine::prompt) return;
	if (text->changed() &&
		Question("Delete Notebook?","Euler",
			textwindow)!=Answers::yes) return;
	text->clear();
	text->newprompt();
	NotebookName.copy("");
	sprintf(WindowTitle,"Euler (%s)",NotebookName.text());
	textwindow.title(WindowTitle);
    text->changed(0);
}

void dosave (void);

void doopen ()
{	if (text->changed() &&
			Question("Save this Notebook\nbefore deleting it?",
				"Euler",textwindow)==Answers::yes)
	{	dosave();
	}
	if (!editing || text->type()!=TextLine::prompt) return;
	String Name("",256);
	Name.copy(loadnotebook.select());
	if (Name.empty()) return;
	FileName.copy(Name);
	text->load(FileName);
	setname();
    text->changed(0);
}

void dosave ();

void dosaveas ()
{   if (!editing || text->type()!=TextLine::prompt) return;
	String Name("",256);
	Name.copy(savenotebook.select());
	if (Name.empty()) return;
	FileName.copy(Name);
	setname();
	dosave();
}

void dosave ()
{	if (*NotebookName==0) dosaveas();
	if (*NotebookName==0) return;
	text->save(FileName);
    text->changed(0);
}

void docomment ()
{   if (!editing || text->type()!=TextLine::prompt) return;
	text->comment();
}

void docontents ()
{	helper.index();
}

void dohelphelp ()
{	helper.helponhelp();
}

void doabout ()
{   char sv[256];
	sprintf(sv,"Euler for Windows 95 (V %s)\nCopyright Rene Grothmann",
		VERSION);
	Dialog dr(textwindow,IDD_About);
	TextItem t(ID_Unregistered,dr,"Freeware");
	TextItem v(ID_Version,dr,sv);
	dr.carryout();
}

void dosetprinter ()
{	Dialog d(textwindow,IDD_Printer,helper,setprinter);
	PrinterItem p(ID_Printer,d,Printer);
	d.carryout();
	if (d.result()!=Dialog::ok) return;
	Printer.copy(p);
	p.properties(textwindow);
	profile.write("printer",Printer);
}

void dousecolors ()
{	usecolors=!usecolors;
	menu.check(IDM_UseColors,usecolors);
	graphicsreplay.start();
}

void dosetsizes ()
{   Dialog d(textwindow,IDD_Sizes,helper,setsizes);
	LongItem stack(ID_StackSize,d,stacksize);
	LongItem graphics(ID_GraphicsSize,d,graphicssize);
	d.carryout();
	if (d.result()!=Dialog::ok) return;
	stacksize=stack;
	if (stacksize<1) stacksize=1;
	if (stacksize>2000) stacksize=2000;
	graphicssize=graphics;
	if (graphicssize<1) graphicssize=1;
	if (graphicssize>32) graphicssize=32;
	Message("Changes will be effective\nat the next start of EULER.",
		"Euler",textwindow);
}

void doload ()
{   if (!waiting || !editing) return;
	String name(loadfile.select());
	if (!*name) return;
	char s[256];
	sprintf(s,"load \"%s\";",name.text());
	text->set(s);
}

void dodemo ()
{   if (!waiting || !editing) return;
	text->set("load demo;");
}

void dodeletecommand ()
{	text->deletecommand();
}

void doinsertcommand ()
{	text->insertcommand();
}

void doselectall ()
{	text->selectall();
}

void dodeleteoutput ()
{	text->deleteoutput();
}

void dodeletealloutput ()
{   text->selectall();
	text->deleteoutput();
}

void docopytext ()
{	text->copytext();
}

void docopycommands ()
{	text->copycommands();
}

void dopastecommands ()
{	text->pastecommands();
}

// **** Print the Graphics ****

PrinterPS *usedprinter;

int printit ()
{   PrinterPS &printer=*usedprinter;
	printer.start();
	int w,h,xm,ym,y,pw,ph,o;
	pw=printer.width();
	ph=printer.height();
	o=(int)(pw*0.1);
	w=pw-2*o;
	h=(int)((double)w*graphicswindow.height()/graphicswindow.width());
	xm=pw/2;
	ym=h/2+o;
	if (ym+h/2>ph-o)
	{	h=ph-2*o;
		w=(int)((double)h*graphicswindow.width()/graphicswindow.height());
		ym=ph/2;
		xm=w/2+o;
	}
	printer.set(xm-w/2,ym-h/2,w,h);
	Font pfont(PrinterFontName,h/gscreenlines);
	printer.font(pfont);
	Font pufont(PrinterFontName,h/gscreenlines,Font::normal,0,900);
	printer.rememberfont(1,pufont);
	Font pdfont(PrinterFontName,h/gscreenlines,Font::normal,0,2700);
	printer.rememberfont(2,pdfont);
	critical.enter();
	meta->replay(&printer,0);
	critical.leave();
	printer.textcolor(Color::text());
	char *p=(char *)PrintText;
	if (*p)
	{	y=ym+h/2+o;
		while (1)
		{   printer.set(o,y,pw-2*o,ph-o-y);
			p=printer.multiline(p,0,0,pw-2*o,ph-o-y);
			if (*p==0) break;
			printer.formfeed();
			y=o;
		}
	}
	printer.formfeed();
	delete usedprinter;
	return 0;
}

Thread printthread(printit);

void doprint ()
{	if (*Printer==0) Printer.getprinter();
	Dialog d(textwindow,IDD_Print,helper,printhelp);
	StringItem ds(ID_Printer,d,Printer);
	MultilineItem dtext(ID_PrintText,d,PrintText);
	dtext.setfont(*courier);
	d.carryout();
	if (d.result()!=Dialog::ok) return;
	PrintText.copy(dtext);
	usedprinter=new PrinterPS("Euler",Printer);
	if (!usedprinter->handle())
	{   Warning("No Printer available!","Euler");
		delete usedprinter;
		return;
	}
	printthread.start();
}

// **** Metafile copy ****

MetafilePS *usedmeta;

int copyit ()
{	MetafilePS &m=*usedmeta;
	Font mfont(GraphicsFontName,m.height()/gscreenlines);
	m.font(mfont);
	Font mufont(GraphicsFontName,m.height()/gscreenlines,Font::normal,0,900);
	m.rememberfont(1,mufont);
	Font mdfont(GraphicsFontName,m.height()/gscreenlines,Font::normal,0,2700);
	m.rememberfont(2,mdfont);
	critical.enter();
	meta->replay(&m,0);
	critical.leave();
	m.close();
	Clipboard c(graphicswindow);
	Metafile f(m);
	c.copy(f,m.width(),m.height());
	delete usedmeta;
	return 0;
}

Thread metafilethread(copyit);

void docopy ()
{	usedmeta=new MetafilePS(graphicswindow.width()*20,
				graphicswindow.height()*20);
	metafilethread.start();
}

// **** Internal Editor Dialog ****

String currentfile("$$$$.e");

String editstring;

class InterpretButton : public ButtonItem
{	MultilineItem *E;
	public :
	InterpretButton (int id, Dialog &d, MultilineItem &e) :
		ButtonItem(id,d),E(&e) {}
	virtual int command (ULONG p);
};

int InterpretButton::command (ULONG p)
{	E->exit();
	FILE *out=fopen((char *)currentfile,"wb");
	char s[256];
	sprintf(s,"Could not open %s !",(char *)currentfile);
	if (!out)
	{	Warning(s,"Interpret",textwindow);
		return 0;
	}
	fwrite(*E,1,strlen(*E),out);
	fclose(out);
	sprintf(s,"load \"%s\"",(char *)currentfile);
	text->set(s);
	return 0;
}

class SaveButton : public ButtonItem
{	MultilineItem *E;
	public :
	SaveButton (int id, Dialog &d, MultilineItem &e) :
		ButtonItem(id,d),E(&e) {}
	virtual int command (ULONG p);
};

int SaveButton::command (ULONG p)
{	E->exit();
	String name(saveeditor.select(dialog()->handle()));
	if (name.empty()) return 1;
	FILE *out=fopen(name,"wb");
	if (!out)
	{	Warning("Could not open this file!","Save",textwindow);
		return 1;
	}
	fwrite(*E,1,strlen(*E),out);
	fclose(out);
	currentfile.copy(name);
	dialog()->title(name);
	return 1;
}

class LoadButton : public ButtonItem
{	MultilineItem *E;
	public :
	LoadButton (int id, Dialog &d, MultilineItem &e) :
		ButtonItem(id,d),E(&e) {}
	virtual int command (ULONG p);
	virtual void init ()
	{	dialog()->title((char *)currentfile);
	}
};

FileSelector loadeditor(textwindow,"*.e",FileSelector::load,
	"Load Editor File",".e");

int LoadButton::command (ULONG p)
{	E->exit();
	String name("");
	name.copy(loadeditor.select(dialog()->handle()));
	if (name.empty()) return 1;
	FILE *in=fopen(name,"r");
	if (!in)
	{	Warning("Could not open this file!","Load",textwindow);
		return 1;
	}
	char s[1024];
	String l("",32000);
	while (!feof(in))
	{	if (!fgets(s,1024,in)) break;
		if (strlen(s)>0 && s[strlen(s)-1]=='\n') s[strlen(s)-1]=0;
		strcat(l,s);
		strcat(l,"\x0D\x0A");
		if ((int)strlen(l)>l.size()-1024)
		{	l.copy(l,l.size()+32000);
		}
	}
	E->set(l);
	fclose(in);
	currentfile.copy(name);
	dialog()->title(name);
	return 1;
}

void doedit ()
{	if (!editing) return;
	Dialog d(textwindow,IDD_Edit,helper,editor);
	MultilineItem e(ID_Edit,d,editstring);
	e.setfont(*courier);
	InterpretButton i(ID_Interpret,d,e);
	SaveButton s(ID_Save,d,e);
	LoadButton l(ID_Load,d,e);
	d.carryout();
	if (d.result()!=Dialog::ok && d.result()!=ID_Interpret) return;
	editstring.copy(e);
}

String externaleditor("notepad.exe");
String externalparameters("");

void dosetupexternaleditor ()
{   Dialog d(textwindow,IDD_SetupExternalEditor);
	StringItem name(ID_Name,d,(char *)externaleditor);
	StringItem parameters(ID_Parameters,d,(char *)externalparameters);
	StringItem file(ID_File,d,(char *)currentfile);
	d.carryout();
	if (d.result()!=Dialog::ok) return;
	externaleditor.copy((char *)(name));
	externalparameters.copy((char *)(parameters));
	currentfile.copy((char *)(file));
}

Dialog *aboutdialog;

int externaleditit ()
{	String command((char *)externaleditor);
	command.cat(" ");
	command.cat(externalparameters);
	_spawnlp(_P_WAIT,(char *)externaleditor,
		(char *)command,
		(char *)currentfile,NULL );
	char s[256];
	sprintf(s,"load \"%s\"",(char *)currentfile);
	text->set(s);
	aboutdialog->close();
	return 0;
}

Thread externaleditthread(externaleditit);

void doexternaledit ()
{	if (!editing) return;
	externaleditthread.start();
	aboutdialog=new Dialog(textwindow,IDD_Editor);
	aboutdialog->move(-100000,-100000);
	aboutdialog->carryout();
}

// **** Save Graphics as a bitmap ****

int idbits[4]={ID_1Bit,ID_4Bit,ID_8Bit,ID_24Bit};
int nbits[4]={1,4,8,24};
int savew=0,saveh=0,saveb=-1;
String Bitmapname;

int saveit ()
{   BitmapPS m(graphicswindow,savew,saveh);
	Font mfont(GraphicsFontName,saveh/gscreenlines);
	m.font(mfont);
	Font mufont(GraphicsFontName,saveh/gscreenlines,Font::normal,0,900);
	m.rememberfont(1,mufont);
	Font mdfont(GraphicsFontName,saveh/gscreenlines,Font::normal,0,2700);
	m.rememberfont(2,mdfont);
	critical.enter();
	m.bar(0,0,savew,saveh,MyColor(0));
	meta->replay(&m);
	critical.leave();
	m.save(Bitmapname,nbits[saveb]);
	return 0;
}

Thread bitmapsave(saveit);

class WindowSize : public ButtonItem
{   LongItem *W,*H;
	public :
	WindowSize (int id, Dialog &d, LongItem &w, LongItem &h) :
		ButtonItem(id,d),W(&w),H(&h) {}
	virtual int command (ULONG l)
	{   W->set(graphicswindow.width());
		H->set(graphicswindow.height());
		return 1;
	}
};

void dosavebitmap ()
{	Dialog d(textwindow,IDD_SaveBitmap,helper,bitmaps);
	if (savew==0)
	{	savew=graphicswindow.width();
		saveh=graphicswindow.height();
	}
	LongItem w(ID_Width,d,savew);
	LongItem h(ID_Height,d,saveh);
	WindowSize ws(ID_WindowSize,d,w,h);
	if (saveb<0) saveb=usecolors?2:0;
	RadioItem b(idbits,4,d,saveb);
	d.carryout();
	if (d.result()!=Dialog::ok) return;
	savew=w; saveh=h; saveb=b;
	if (savew<10) savew=10;
	if (saveh<10) saveh=10;
	Bitmapname.copy(savebitmap.select());
	if (Bitmapname.empty()) return;
	if (savew==graphicswindow.width() &&
			saveh==graphicswindow.height())
		graphicswindow.bitmap()->save(Bitmapname,nbits[saveb]);
	else
		bitmapsave.start();
}

String PostscriptName;

void dopostscript ()
{	PostscriptName.copy(savepostscript.select());
	if (PostscriptName.empty()) return;
    FILE *out=fopen(PostscriptName,"w");
    if (!out) return;
    meta->postscript(out);
    fclose(out);
}

// ***** Save Graphics as metafile *****

int metasaveit ()
{	MetafilePS &m=*usedmeta;
	Font mfont(GraphicsFontName,saveh/gscreenlines);
	m.font(mfont);
	Font mufont(GraphicsFontName,saveh/gscreenlines,Font::normal,0,900);
	m.rememberfont(1,mufont);
	Font mdfont(GraphicsFontName,saveh/gscreenlines,Font::normal,0,2700);
	m.rememberfont(2,mdfont);
	critical.enter();
	meta->replay(&m);
	critical.leave();
	m.close();
	PlaceableMetafile f(m);
	f.save(Bitmapname);
	return 0;
}

Thread metafilesave(metasaveit);

void dosavemetafile ()
{	savew=graphicswindow.width()*20; saveh=graphicswindow.height()*20;
	Bitmapname.copy(savemetafile.select());
	if (Bitmapname.empty()) return;
	usedmeta=new MetafilePS(savew,saveh);
	metafilesave.start();
}

// **** Dialog to choose both fonts ****

class TextFontButton : public ButtonItem
{   int N;
	LongItem *L;
	Font *F;
	StringItem *S;
	public :
	TextFontButton (int id, Dialog &d, LongItem &l, Font &f,
			StringItem &s) :
		ButtonItem(id,d),N(0),L(&l),F(&f),S(&s)
	{}
	virtual int command (ULONG p)
	{	FontDialog fd(*F);
		if (fd.select(textwindow))
		{	N++;
			WindowPS ps(textwindow);
			ps.font(*F);
			L->set(ps.textheight());
			S->set(F->name());
		}
		return 1;
	}
	int n () { return N; }
};

class GraphicsFontButton : public ButtonItem
{   int N;
	LongItem *L;
	Font *F;
	StringItem *S;
	public :
	GraphicsFontButton (int id, Dialog &d, LongItem &l, Font &f,
			StringItem &s) :
		ButtonItem(id,d),N(0),L(&l),F(&f),S(&s)
	{}
	virtual int command (ULONG p)
	{	FontDialog fd(*F);
		if (fd.select(textwindow,
				FontDialog::init|FontDialog::scalable|FontDialog::screen))
		{	N++;
			WindowPS ps(graphicswindow);
			ps.font(*F);
			L->set(graphicswindow.height()/ps.textheight());
			S->set(F->name());
		}
		return 1;
	}
	int n () { return N; }
};

class PrinterFontButton : public ButtonItem
{   int N;
	String Name;
	StringItem *S,*PN;
	public :
	PrinterFontButton (int id, Dialog &d, StringItem &s,
			StringItem &pn) :
		ButtonItem(id,d),N(0),Name(PrinterFontName),S(&s),
		PN(&pn)
	{}
	virtual int command (ULONG p)
	{	if (*Printer==0) Printer.getprinter();
		PN->set(Printer);
		PrinterPS printer("Euler",Printer);
		Font f(Name,10);
		FontDialog fd(f);
		if (fd.select(textwindow,printer,
				FontDialog::init|FontDialog::printer))
		{	N++;
			Name.copy(f.name());
			S->set(f.name());
		}
		return 1;
	}
	int n () { return N; }
	char *name () { return Name; }
};

void dosetfonts ()
{   Font gfont(GraphicsFontName,tfontheight);
	Font tfont(TextFontName,graphicswindow.height()/gscreenlines);
	Dialog d(textwindow,IDD_Fonts,helper,fonts);
	LongItem gn(ID_GraphicsFont,d,gscreenlines);
	LongItem tn(ID_TextFont,d,tfontheight);
	StringItem gs(ID_GraphicsFontName,d,GraphicsFontName);
	StringItem ts(ID_TextFontName,d,TextFontName);
	StringItem ps(ID_PrinterFontName,d,PrinterFontName);
	StringItem pname(ID_PrinterName,d,Printer);
	GraphicsFontButton gf(ID_SelectGraphicsFont,d,gn,gfont,gs);
	TextFontButton tf(ID_SelectTextFont,d,tn,tfont,ts);
	PrinterFontButton pf(ID_SelectPrinterFont,d,ps,pname);
	d.carryout();
	if (d.result()!=Dialog::ok) return;
	int n;
	n=gn;
	GraphicsFontName.copy(gfont.name());
	if (n!=gscreenlines && n>10 && n<200) gscreenlines=n;
	graphicswindow.sized();
	n=tn;
	TextFontName.copy(tfont.name());
	tfontheight=n;
	textwindow.sized();
	PrinterFontName.copy(pf.name());
	Message("To make these settings effective\n"
		"it is best to restart EULER.\n"
		"Or at least type >reset;","Fonts",textwindow);
}

// **** Dialog to set the 16 default colors ****

class ColorItem : public UserItem
{   int I;
	Color C;
	public :
	ColorItem (int id, Dialog &d, int i) :
		UserItem(id,d),I(i),C(MyColor(i)) {}
	virtual void redraw (PS &ps)
	{	ps.bar(0,0,ps.width(),ps.height(),C);
	}
	void set (Color c) { C=c; update(); }
};

class SetColorItem : public ButtonItem
{	int I;
	Color C;
	ColorItem *Ci;
	public :
	SetColorItem (int id, Dialog &d, int i, ColorItem &ci) :
		ButtonItem(id,d),I(i),C(MyColor(i)),Ci(&ci) {}
	virtual int command (ULONG p)
	{   ColorSelector s(textwindow,C);
		Color H=s.select();
		if (s.result()==Dialog::ok)
		{	C=H;
			Ci->set(H);
		}
		return 1;
	}
	void set (Color c) { C=c; }
	Color color () { return C; }
};

class DefaultButton : public ButtonItem
{   ColorItem **C;
	SetColorItem **Cs;
	public :
	DefaultButton (int id, Dialog &d, ColorItem **c,
		SetColorItem **cs)
		: ButtonItem(id,d),C(c),Cs(cs)
	{}
	virtual int command (ULONG p);
};

int DefaultButton::command (ULONG p)
{	for (int i=0; i<16; i++)
	{	C[i]->set(Color(red[i],green[i],blue[i],purecolors));
		Cs[i]->set(Color(red[i],green[i],blue[i],purecolors));
	}
	return 1;
}

void dosetcolors ()
{	Dialog d(textwindow,IDD_Colors,helper,colorhelp);
	int i;
	ColorItem *CI[16];
	for (i=0; i<16; i++) CI[i]=new ColorItem(100+i,d,i);
	SetColorItem *SCI[16];
	for (i=0; i<16; i++) SCI[i]=new SetColorItem(200+i,d,i,*CI[i]);
	DefaultButton b(ID_DefaultColors,d,CI,SCI);
	d.carryout();
	if (d.result()==Dialog::ok)
	{	for (i=0; i<16; i++) savecolors.set(i,SCI[i]->color());
		for (i=0; i<16; i++) mycolors.set(i,SCI[i]->color());
	}
	for (i=0; i<16; i++) delete CI[i];
	for (i=0; i<16; i++) delete SCI[i];
	textwindow.update();
}

void dopurecolors ()
// Toggle usage of pure colors only
{	purecolors=!purecolors;
	menu.check(IDM_PureColors,purecolors);
}

/*************** Metafile replay **********************/

int WindowsMeta::gclear (void *p)
/***** clear the graphics screen
*****/
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gclear(0)) return 0;
	if (Draw==0) return 1;
	if (usecolors && p==ops) ps->erase();
	else ps->bar(0,0,ps->width(),ps->height(),MyColor(0));
    return 1;
}

int WindowsMeta::gclip (void *p, double c, double r, double c1, double r1)
{
	PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gclip(0,c,r,c1,r1)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gclip(*ps,c,r,c1,r1);
	critical.leave();
	return 1;
}

int WindowsMeta::gline (void *p, double c, double r, double c1, double r1, int color,
	int st, int width)
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gline(0,c,r,c1,r1,color,st,width)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gline(*ps,c,r,c1,r1,color,st,width);
	critical.leave();
    return 1;
}

int WindowsMeta::gmarker (void *p, double c, double r, int color, int st)
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gmarker(0,c,r,color,st)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gmarker(*ps,c,r,color,st);
	critical.leave();
    return 1;
}

int WindowsMeta::gfill (void *p, double c[], int st, int n, int connect[])
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
   	if (!Meta::gfill(0,c,st,n,connect)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gfill(*ps,c,st,n,connect);
	critical.leave();
    return 1;
}

int WindowsMeta::gfillh (void *p, double c[], double hue, int color, int connect)
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gfillh(0,c,hue,color,connect)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gfillh(*ps,c,hue,color,connect);
	critical.leave();
    return 1;
}

int WindowsMeta::gbar (void *p, double c, double r, double c1, double r1, double hue,
	int color, int connect)
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gbar(0,c,r,c1,r1,hue,color,connect)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gbar(*ps,c,r,c1,r1,hue,color,connect);
	critical.leave();
    return 1;
}

int WindowsMeta::gbar1 (void *p, double c, double r, double c1, double r1,
	int color, int connect)
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gbar1(0,c,r,c1,r1,color,connect)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gbar1(*ps,c,r,c1,r1,color,connect);
	critical.leave();
    return 1;
}

int WindowsMeta::gtext (void *p, double c, double r, char *text, int color, int centered)
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gtext(0,c,r,text,color,centered)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gtext(*ps,c,r,text,color,centered);
	critical.leave();
    return 1;
}

int WindowsMeta::gvtext (void *p, double c, double r, char *text, int color, int centered)
{   PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gvtext(0,c,r,text,color,centered)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gvtext(*ps,c,r,text,color,centered);
	critical.leave();
    return 1;
}

int WindowsMeta::gvutext (void *p, double c, double r, char *text, int color, int centered)
{	PS *ps=(PS *)p;
	if (ps==0) ps=ops;
	if (!Meta::gvutext(0,c,r,text,color,centered)) return 0;
	if (Draw==0) return 1;
	critical.enter();
	::gvutext(*ps,c,r,text,color,centered);
	critical.leave();
    return 1;
}

int WindowsMeta::gscale (void *p, double s)
/***** scale
	scale the screen according s = true width / true height.
	This is not necessary on a window based system.
*****/
{   Meta::gscale(0,s);
	return 1;
}

//****************** Main Program *****************

void closelibs (); // Close all opened DLLs

int winmain (int argc, char *argv[])
/******
Initialize memory and call main_loop
******/
{	int i,x,y,w,h;
	stacksize=profile.readint("stacksize",16);
	graphicssize=profile.readint("graphicssize",16);
	if (!memory_init()) return 1;
	meta=new WindowsMeta(graphicssize);
	gscreenlines=profile.readint("graphicslines",gscreenlines);
	tfontheight=profile.readint("textfont",tfontheight);
	TextFontName.copy(profile.read("textfontname",TextFontName));
	GraphicsFontName.copy(profile.read("graphicsfontname",GraphicsFontName));
	Printer.copy(profile.read("printer",""));
	textwindow.getsize(x,y,w,h);
	x=profile.readint("xtext",x);
	y=profile.readint("ytext",y);
	w=profile.readint("wtext",w);
	h=profile.readint("htext",h);
	if (w>100 && h>100) textwindow.setsize(x,y,w,h);
	graphicswindow.getsize(x,y,w,h);
	x=profile.readint("xgraphics",x);
	y=profile.readint("ygraphics",y);
	w=profile.readint("wgraphics",w);
	h=profile.readint("hgraphics",h);
	if (w>100 && h>100) graphicswindow.setsize(x,y,w,h);
	textwindow.clientmin(300,200);
	textwindow.accelerate(ID_ACC);
	char s[32];
	for (i=0; i<16; i++)
	{   sprintf(s,"color%d",i);
		mycolors.set(i,profile.readlong(s,(long)MyColor(i)));
	}
	externaleditor.copy(
		profile.read("externaleditor",(char *)externaleditor));
	externalparameters.copy(
		profile.read("externalparameters",(char *)externalparameters));
	menu.init();
	menu.add(IDM_Exit,doexit);
	menu.add(IDM_New,donew);
	menu.add(IDM_Open,doopen);
	menu.add(IDM_Save,dosave);
	menu.add(IDM_Saveas,dosaveas);
	menu.add(IDM_Comment,docomment);
	menu.add(IDM_Contents,docontents);
	menu.add(IDM_About,doabout);
	menu.add(IDM_Print,doprint);
	menu.add(IDM_CopyClipboard,docopy);
	menu.add(IDM_SaveMetafile,dosavemetafile);
	menu.add(IDM_UseColors,dousecolors);
	menu.check(IDM_UseColors,usecolors);
	menu.add(IDM_SetPrinter,dosetprinter);
	menu.add(IDM_SetSizes,dosetsizes);
	menu.add(IDM_ExternalEditor,dosetupexternaleditor);
	menu.add(IDM_Load,doload);
	menu.add(IDM_DeleteCommand,dodeletecommand);
	menu.add(IDM_InsertCommand,doinsertcommand);
	menu.add(IDM_CopyText,docopytext);
	menu.add(IDM_SelectAll,doselectall);
	menu.add(IDM_DeleteOutput,dodeleteoutput);
	menu.add(IDM_DeleteAllOutput,dodeletealloutput);
	menu.add(IDM_Edit,doedit);
	menu.add(IDM_ExternalEdit,doexternaledit);
	menu.add(IDM_SaveBitmap,dosavebitmap);
	menu.add(IDM_Postscipt,dopostscript);
	menu.add(IDM_PasteCommands,dopastecommands);
	menu.add(IDM_CopyCommands,docopycommands);
	menu.add(IDM_SetFonts,dosetfonts);
	menu.add(IDM_SetColors,dosetcolors);
	menu.add(IDM_PureColors,dopurecolors);
	purecolors=profile.readint("purecolors",purecolors);
	menu.check(IDM_PureColors,purecolors);
	menu.add(IDM_Demo,dodemo);
	cursor_on();
	textwindow.init();
	graphicswindow.init();
	textwindow.top();
	String dir(program.filename());
	dir.stripfilename();
	FILE *t=fopen("euler.cfg","r");
	if (!t)
	{	chdir(dir);
	}
	else fclose(t);
	dir.cat("\\help.txt");
	loadhelp(dir);
	if (argc==2)
	{   String name;
		name.copy(argv[1]);
		name.stripapostroph();
		if (name.testextension(".en"))
		{	wantload.copy(name);
			argv++; argc--;
		}
	}
	init_yacas();
	computation.set(argc,argv);
	computation.start();
	program.loop();
	textwindow.getsize(x,y,w,h);
	profile.writeint("xtext",x);
	profile.writeint("ytext",y);
	profile.writeint("wtext",w);
	profile.writeint("htext",h);
	graphicswindow.getsize(x,y,w,h);
	profile.writeint("xgraphics",x);
	profile.writeint("ygraphics",y);
	profile.writeint("wgraphics",w);
	profile.writeint("hgraphics",h);
	profile.writeint("stacksize",stacksize);
	profile.writeint("graphicssize",graphicssize);
	profile.writeint("textfont",tfontheight);
	profile.writeint("graphicslines",gscreenlines);
	profile.write("textfontname",TextFontName);
	profile.write("graphicsfontname",GraphicsFontName);
	for (i=0; i<16; i++)
	{   sprintf(s,"color%d",i);
		profile.writelong(s,(long)(*new Color(savecolors.color(i),purecolors)));
	}
	profile.writeint("purecolors",purecolors);
	profile.write("externaleditor",(char *)externaleditor);
	profile.write("externalparameters",(char *)externalparameters);
	closelibs();
	exit_yacas();
	while (printthread.active())
	{	if (Question("Stop print?","EULER")==Answers::yes) break;
	}
	return 0;
}

