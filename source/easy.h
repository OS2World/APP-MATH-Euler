#ifndef _EASY_H
#define _EASY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>

#define ULONG unsigned long

class Dumpfile
{	FILE *F;
	public :
	Dumpfile (char *name)
	{	F=fopen(name,"w");
	}
	~Dumpfile () { fclose(F); }
	void printf (char *format, ...)
	{   va_list ap;
		va_start(ap,format);
		vfprintf(F,format,ap);
		va_end(ap);
	}
};

class Answers
{	public :
	enum {yes=IDYES,no=IDNO,abort=IDCANCEL};
};

class Window;
class Dialog;

void dumplong (long n);
void dumpdouble (double x);
void dump (char *s);
void Warning (char *s, char *title);
void Message (char *s, char *title);
int Question (char *s, char *title);
int QuestionAbort (char *s, char *title);
void Warning (char *s, char *title, Window &window);
void Message (char *s, char *title, Window &window);
int Question (char *s, char *title, Window &window);
int QuestionAbort (char *s, char *title, Window &window);
void Warning (char *s, char *title, Dialog &d);
void Message (char *s, char *title, Dialog &d);
int Question (char *s, char *title, Dialog &d);
int QuestionAbort (char *s, char *title, Dialog &d);
void Beep (int frequency=440, double seconds=0.5);
void Dump (char *text);

int Shift ();
int Control ();

extern int winmain (int argc, char *argv[]);

class Flag
{	int F;
	public :
	Flag (int f=0) { F=f; }
	void set () { F=1; }
	void clear () { F=0; }
	void toggle () { F=!F; }
	operator int () { return F; }
	int operator = (int flag) { return F=flag; }
};

class FixFlag
{	int F,Fix;
	public :
	FixFlag (int f=0) { F=f; Fix=0; }
	void set () { if (Fix) return; F=1; }
	void clear () { if (Fix) return; F=0; }
	void toggle () { if (Fix) return; F=!F; }
	operator int () { return F; }
	void fix () { Fix=1; }
	void unfix () { Fix=0; }
	int operator = (int flag) { if (Fix) return flag; return F=flag; }
};

typedef struct { int i; char *s; } stringtable;

class String
{	char *P;
	int Size;
	public :
	static int defaultsize;
	String ();
	String (char *text);
	String (char *text, int size);
	String (int i);
	String (String &s);
	~String ();
	char *text () { return P; }
	long size () { return Size; }
	void copy (char *text, int size);
	void copy (char *text);
	void cat (char *text); // concat with the text.
	void stripapostroph (); // remove outer "..."
	char *filename ();
	void stripfilename ();
	char *extension (); // return file extension
	void extension (char *ext); // set file extension
	int testextension (char *ext); // test, if ext is extension
	operator char * () { return P; }
	int todouble (double &x);
	int tolong (long &n);
	int empty () { return *P==0; }
	char * operator = (char *s) { copy(s); return s; }
    void getprinter ();
};

class ConvertString : public String
{	public :
	ConvertString (long n) : String("",32) { ltoa(n,*this,10); }
	ConvertString (double x) : String("",32)
		{ sprintf(*this,"%-0.10g",x); }
};

class Parameter
{	long P;
	public :
	Parameter (long s) : P(s) {}
	Parameter (void *s) : P((long)s) {}
    Parameter (int low, int hi) : P(MAKELONG(low,hi)) {}
	operator long () { return P; }
};

class WindowObject
{	WindowObject *Next;
	public :
	WindowObject ();
	virtual void create () {};
	WindowObject *next () { return Next; }
    void next (WindowObject *wo) { Next=wo; }
};

class WindowObjects
{	WindowObject *First,*Last;
	public :
	WindowObjects () : First(0),Last(0) {}
	void create ();
    void add (WindowObject *wo);
};

extern stringtable *progstrings;

class WindowClass : public WindowObject
{   String Name;
	int Icon,Style,Extra;
	public :
	enum { vredraw=CS_VREDRAW,hredraw=CS_HREDRAW,
		noclose=CS_NOCLOSE };
	WindowClass (char *name, int icon=0,
		int style=vredraw|hredraw, int extra=0);
	~WindowClass ();
	char * name () { return Name; }
	virtual void create ();
};

class Program
{   MSG Msg;
	HANDLE Instance,PrevInstance;
	int Show;
	int Param;
	WindowObjects *Wos;
	String Name;
	HWND Main;
	HANDLE Accel;
	String Filename;
	public :
	Flag started;
	void loop ();
	int getmessage ();
	void dispatch ();
	Program (stringtable *s=0) : Accel(0)
	{	Wos=new WindowObjects;
		if (s) progstrings=s;
	}
	void set (HANDLE instance, HANDLE previnstance, int show);
	int show () { return Show; }
	HANDLE instance () { return Instance; }
	HANDLE previnstance () { return PrevInstance; }
	int param () { return Param; }
	WindowObjects *wos () { return Wos; }
	char *name () { return Name; }
	void accelerate (HWND hw, int id)
	{	Main=hw;
		Accel=LoadAccelerators((HINSTANCE)instance(),MAKEINTRESOURCE(id));
	}
	char *filename ();
};

extern Program program;

class Alignment
{	public :
	enum { left,center,right,top,bottom,middle };
};

class Color
{	COLORREF C;
	public :
	Color () : C(0) {}
	Color (int red, int green, int blue)
	{	C=RGB(red,green,blue);
	}
	Color (int red, int green, int blue, int p)
	{	if (p) C=PALETTERGB(red,green,blue);
		else C=RGB(red,green,blue);
	}
	Color (COLORREF c, int p=0) : C(c)
	{	if (p) C|=0x02000000;
	}
	void set (int red, int green, int blue)
	{	C=RGB(red,green,blue);
	}
	operator COLORREF () { return C; }
	static COLORREF background () { return GetSysColor(COLOR_WINDOW); }
	static COLORREF foreground () { return GetSysColor(COLOR_WINDOWTEXT); }
	static COLORREF text () { return GetSysColor(COLOR_WINDOWTEXT); }
};

class Markers
{	public :
	enum
	{	def,cross,plus,
		diamond,star,
		square,solidsquare,
		soliddiamond,
		sixpointstar,
		eightpointstart,
		dot,circle,
		blank
	};
};

class Brush
{   protected :
	HANDLE Handle;
	public :
	HANDLE handle () { return Handle; }
};

class SolidBrush : public Brush
{	public :
	SolidBrush (Color col) { Handle=CreateSolidBrush(col); }
	~SolidBrush () { DeleteObject(handle()); }
};

class HatchBrush : public Brush
{	public :
	enum { vertical=HS_VERTICAL, bdiagonal=HS_BDIAGONAL,
		horizontal=HS_HORIZONTAL, fdiagonal=HS_FDIAGONAL,
        cross=HS_CROSS, diagcross=HS_DIAGCROSS };
	HatchBrush (int style, Color col)
	{	Handle=CreateHatchBrush(style,col);
	}
	~HatchBrush () { DeleteObject(handle()); }
};

class Pen
{   HPEN Handle;
	public :
	enum { solid=PS_SOLID, dash=PS_DASH, dot=PS_DOT,
		dashdot=PS_DASHDOT, dashdotdot=PS_DASHDOTDOT,
		alternate=PS_ALTERNATE,
		round=PS_ENDCAP_ROUND, flat=PS_ENDCAP_FLAT,
		square=PS_ENDCAP_SQUARE };
	Pen (int style, int width, Color col)
	{	LOGBRUSH b;
		b.lbStyle=BS_SOLID; b.lbColor=col; b.lbHatch=0;
		Handle=ExtCreatePen(style|PS_GEOMETRIC,width,&b,0,0);
	}
	Pen (int style, Color col)
	{	Handle=CreatePen(style|PS_COSMETIC,1,col);
	}
	~Pen () { if (Handle) DeleteObject(Handle); }
	HANDLE handle () { return Handle; }
};

class Points
{	int N,Size;
	POINT *P;
	public :
	Points (int size) : Size(size),N(0)
	{	P=new POINT[Size];
	}
	void empty () { N=0; }
	POINT * points () { return P; }
	int size () { return N; }
	void add (int x, int y)
	{	if (N<Size) { P[N].x=x; P[N].y=y; N++; }
	}
	int x (int i) { return P[i].x; }
	int y (int i) { return P[i].y; }
};

#define MAXFONTS 32

class Font;
class PS
{   protected :
	PAINTSTRUCT Paint;
	HDC Handle;
	int X,Y;
	int Width,Height;
	HANDLE HandlePen;
	int Ms; // marker size
	Color Col;
	HRGN Region;
    Font *fonts[MAXFONTS];
	public :
	enum { transparent=TRANSPARENT, opaque=OPAQUE };
	enum { copy=R2_COPYPEN, xor=R2_XORPEN, not=R2_NOT };
	PS () : Ms(4),Region(0) {}
	void init ();
	void release ();
	void setdefaults ()
	{	defaultcolor();
		backmode(transparent);
	}
	int backmode (int m) { return SetBkMode(Handle,m); }
	int backmode () { return GetBkMode(Handle); }
	int mode (int m) { return SetROP2(Handle,m); }
	int mode () { return GetROP2(Handle); }
	int savecontext () { return SaveDC(Handle); }
	void loadcontext (int i=-1) { RestoreDC(Handle,i); }
	Color color (Color col);
	Color defaultcolor ();
	void font (Font &f);
	void rememberfont (int i, Font &f);
	void font (int i);
	int textwidth (char *s, int n);
	int textheight (char *s, int n);
	Color textcolor (Color col){ return SetTextColor(Handle,col); }
	Color textcolor () { return GetTextColor(Handle); }
	Color backcolor (int c) { return SetBkColor(Handle,c); }
	Color backcolor () { return GetBkColor(Handle); }
	HDC handle () { return Handle; }
	void ctext (RECT *rect, char *text);
	HANDLE select (HANDLE object) { return SelectObject(Handle,object); }
	HANDLE proportionalfont () { return select(GetStockObject(SYSTEM_FONT)); }
	HANDLE fixedfont () { return select(GetStockObject(SYSTEM_FIXED_FONT)); }
	virtual void textextent (char *s, int &w, int &h);
	virtual void textextent (char *s, int n, int &w, int &h);
	int textheight ();
    int textavewidth ();
	double textdiameter (char *s, double x, double y);
	char * oneline (char *s, int x, int y, int w, int h);
	char * multiline (char *s, int x, int y, int w, int h);
	void alignment (int halign, int valign, int update);
	void text (char *s) { TextOut(Handle,0,0,s,strlen(s)); }
	void text (char *s, int n) { TextOut(Handle,0,0,s,n); }
	void text (int x, int y, char *s,
		int halign=Alignment::left, int valign=Alignment::bottom);
	void move (int x, int y)
	{   POINT p;
		X=x; Y=y; MoveToEx(Handle,x,y,&p);
	}
    double x ()
    {	POINT p;
    	GetCurrentPositionEx(Handle,&p);
        return p.x;
    }
    double y ()
    {	POINT p;
    	GetCurrentPositionEx(Handle,&p);
        return p.y;
    }
	void lineto (int x, int y);
	void lineto (int x, int y, Pen &p);
	void line (int x, int y, int x1, int y1)
	{	move(x,y); lineto(x1,y1);
	}
	void line (int x, int y, int x1, int y1, Pen &p)
	{	move(x,y); lineto(x1,y1,p);
    }
	int width () { return Width; }
	int height () { return Height; }
	void whiten () { PatBlt(Handle,0,0,Width,Height,WHITENESS); }
	void point (int x, int y, Color col);
	void mark (int x, int y, int type);
    void markersize (int n) { Ms=n; }
	int markersize () { return Ms; }
	void rectangle (int x, int y, int w, int h);
	void rectangle (int x, int y, int w, int h, Pen &p);
	void filledrectangle (int x, int y, int w, int h, Color col);
	void filledrectangle (int x, int y, int w, int h, Brush &b);
	void bar (int x, int y, int w, int h, Color col);
	void bar (int x, int y, int w, int h, Brush &b);
	virtual void erase ()
	{   bar(0,0,width(),height(),Color::background());
	}
	void erase (Brush &b)
	{   filledrectangle(0,0,width(),height(),b);
	}
	void circle (int x, int y, int r, double f=1);
	void circle (int x, int y, int r, double f, Pen &pen);
	void arc (int x, int y, int r, double f,
		double phi1, double phi2);
	void arc (int x, int y, int r, double f,
		double phi1, double phi2, Pen &pen);
	void clip (int x, int y, int w, int h);
	void unclip ();
	void set (int x, int y, int w, int h);
	void fillpolygon (Points &p, Color col);
	void polygon (Points &p, Color fillcol, Color col);
};

class Window
{   protected :
	HWND Handle;
	int Width,Height;
	int XOffset,YOffset;
	public :
	Window () : XOffset(0),YOffset(0) {}
	HWND handle () { return Handle; }
	int width () { return Width; }
	int height () { return Height; }
	void capture (int f)
	{	if (f) SetCapture(Handle);
		else ReleaseCapture();
	}
	void size (int w, int h)
	{	SetWindowPos(Handle,0,0,0,w,h,
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	}
	void move (int x, int y)
	{	SetWindowPos(Handle,0,x,y,0,0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	}
	void setsize (int x, int y, int w, int h)
	{	SetWindowPos(Handle,0,x,y,w,h,
			SWP_NOACTIVATE|SWP_NOZORDER);
	}
	void getsize (int &x, int &y, int &w, int &h)
	{   RECT r;
		GetWindowRect(Handle,&r);
		x=r.left; y=r.top;
		w=r.right-r.left; h=r.bottom-r.top;
	}
	int hasoffset () { return XOffset>0 || YOffset>0; }
	int xoffset () { return XOffset; }
	int yoffset () { return YOffset; }
	void usermessage (int m, Parameter p=Parameter((long)0))
	{	SendMessage(Handle,WM_USER+m,0,p);
	}
};

class Keycode
{	public :
	enum { up=1,down=2,virtualkey=4,charkey=8 };
};

class ClickType
{	public :
	enum {
		button1up,button1down,button1double,
		button2up,button2down,button2double,
		button3up,button3down,button3double,
        mousemove };
};

class Cursor
{   HCURSOR Handle;
	public :
	enum { arrow,beam,cross,wait,icon,old };
	Cursor (int id)
	{	Handle=LoadCursor((HINSTANCE)program.instance(),MAKEINTRESOURCE(id));
	}
	~Cursor ()
	{	DestroyCursor(Handle);
	}
	HCURSOR handle () { return Handle; }
};

class Scroll
{   public :
	enum { left,right,up,down,pageleft,pageright,pageup,pagedown,
		pos };
};

class StatusBar;
class ToolBar;
class Menu;
class StandardWindow : public Window, public WindowObject
{   RECT Rect;
	Menu *Windowmenu;
	StatusBar *Sb;
	ToolBar *Tb;
	String Title;
	DWORD Style;
	HWND Parent;
	HCURSOR OldCursor,NewCursor;
	Flag B1down,B2down,B3down; // Button states
	int Minwidth,Maxwidth,Minheight,Maxheight;
	Flag Caret,Havecaret,Showcaret;
	int CaretX,CaretY,CaretW,CaretH;
	String Classname;
	int Defaultw,Defaulth;
	int State;
	public :
	enum {
		defaultsize=CW_USEDEFAULT,
		defaultstyle=WS_OVERLAPPEDWINDOW|WS_BORDER|WS_CAPTION|WS_SYSMENU,
		vscroll=WS_VSCROLL,
		hscroll=WS_HSCROLL
	};
	enum {
		lineup=SB_LINEUP,linedown=SB_LINEDOWN,
		pageup=SB_PAGEUP,pagedown=SB_PAGEDOWN,
		endscroll=SB_ENDSCROLL,
		scrollposition=SB_THUMBPOSITION,
		scrolltrack=SB_THUMBTRACK
	};
	enum {
		fullscreen=SIZEFULLSCREEN,
		iconic=SIZEICONIC,
		normal=SIZENORMAL
	};
	virtual void create ();
	StandardWindow (char *title,
		WindowClass &wc,
		int w=defaultsize, int h=defaultsize,
		DWORD style=defaultstyle,
		HWND parent=NULL) :
		Title(title),Style(style),Parent(parent),NewCursor(0),
		B1down(0),B2down(0),B3down(0),
		Minwidth(0),Minheight(0),Maxwidth(10000),Maxheight(10000),
		Windowmenu(0),
		Caret(0),Havecaret(0),Showcaret(0),
		CaretX(0),CaretY(0),CaretW(1),CaretH(8),Classname(wc.name()),
		Defaultw(w),Defaulth(h),
		State(normal)
		,Sb(0),Tb(0)
	{   if (program.started) create();
		Width=w; Height=h;
	}
	void init ();
	virtual void redraw (PS &ps) {}
	virtual void sized () {}
    virtual void statechanged (int state) {}
	virtual void key (int flags, int code, int scan) {};
	virtual void clicked (int x, int y, int type) {};
	void update () { InvalidateRect(Handle,NULL,1); }
	RECT *client ();
	void quit () { PostQuitMessage(0); }
	void setmenu (Menu *m);
	void destroy ();
	friend long FAR PASCAL /*_export*/ StandardProc
		(HWND hwnd, unsigned int message,
		unsigned int wParam, long lParam);
	void title (char *s)
	{	SetWindowText(handle(),s);
	}
	void setcursor (int type);
	void setcursor (Cursor &c);
	void clientmin (int w, int h);
	void clientmax (int w, int h);
	void clientsize (int w, int h);
	void adjust (int &w, int &h);
	void top () { SetWindowPos(Handle,0,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE); }
	void hide () { ShowWindow(Handle,SW_HIDE); }
	void show () { ShowWindow(Handle,SW_SHOW); }
	void setfocus () { SetFocus(Handle); }
	void foreground () { SetForegroundWindow(Handle); }
	int focus () { return GetFocus()==Handle; }
	void accelerate (int id)
	{	program.accelerate(Handle,id);
	}
	virtual int close () { return 1; }
	virtual void user (int message, Parameter p) {}
	virtual void timer (int n) {}
	void usecaret (int flag=1) { Caret=flag; }
	void showcaret ();
	void hidecaret ();
	void setcaret (int x, int y);
	void setcaretsize (int w, int h);
	virtual void scroll (int flags, int type, int pos) {}
    void setscroll (int flag, int pos);
	void setstatus (StatusBar *s) { Sb=s; }
	void settoolbar (ToolBar *s) { Tb=s; }
	void adjustsize (unsigned int wParam, long lParam);
};

class Timer
{	int Handle;
	HWND WHandle;
	int N;
	public :
	Timer (StandardWindow &w, long msec, int n=1) : N(n)
	{   WHandle=w.handle();
		Handle=SetTimer(WHandle,n,msec,0);
	}
	~Timer () { if (Handle) KillTimer(WHandle,N); }
	void kill () { if (Handle) KillTimer(WHandle,N); Handle=0; }
};

class Menuentry
{	Menuentry *Next;
	int Id;
	void (*F) ();
	public :
	Menuentry (Menuentry *next, int id, void (*f) ()) :
		Next(next),Id(id),F(f) {}
	int id () { return Id; }
	void call () { F(); }
	Menuentry *next () { return Next; }
	Menuentry *remove (int id);
};

class Menu : public WindowObject
{   Menuentry *First;
	StandardWindow *W;
	HANDLE Handle;
	int Command;
	String Name;
	public :
	virtual void create ();
	Menu (StandardWindow &window)
		: W(&window),First(0)
	{   Name.copy("Menu");
		if (program.started) create();
	}
	Menu (StandardWindow &window, char *name)
		: W(&window),First(0),Name(name)
	{   if (program.started) create();
	}
	void add (int id, void (*f) ())
	{	First=new Menuentry(First,id,f);
	}
	int call (int id);
	int command () { return Command; }
	void init ();
	HANDLE handle () { return Handle; }
	HANDLE submenu (int pos) { return GetSubMenu((HMENU)Handle,pos); }
	void append (int sub, int id, char *text, void (*f) ());
	void remove (int sub, int id);
	void check (int id, int flag);
	void enable (int id, int flag);
};

class RedrawPS : public PS
{   Window *W;
	public :
	RedrawPS (Window &window) : W(&window)
	{	Handle=BeginPaint(window.handle(),&Paint);
		init();
    	setdefaults();
		Width=window.width(); Height=window.height();
		if (window.hasoffset())
			set(window.xoffset(),window.yoffset(),
				window.width(),window.height());
	}
	~RedrawPS ()
	{   release();
		EndPaint(W->handle(),&Paint);
	}
};

class WindowPS : public PS
{	Window *W;
	public :
	WindowPS (Window &window) : W(&window)
	{	Handle=GetDC(window.handle());
		init();
		setdefaults();
		Width=window.width(); Height=window.height();
		if (window.hasoffset())
			set(window.xoffset(),window.yoffset(),
				window.width(),window.height());
	}
	~WindowPS ()
	{   release();
		ReleaseDC(W->handle(),Handle);
	}
};

class DevicePS : public PS
{	public :
	DevicePS (HDC dc, HANDLE handle)
	{	Handle=dc;
		init();
		setdefaults();
		RECT r;
		GetClientRect((HWND)handle,&r);
		Width=r.right-r.left+1;
        Height=r.bottom-r.top+1;
	}
	~DevicePS ()
	{   release();
	}
};

class BitmapPS : public PS
{	Window *W;
	HANDLE BitmapHandle;
	public :
	BitmapPS (Window &window);
	BitmapPS (Window &window, int w, int h);
	BitmapPS (Window &window, int w, int h, int bits);
	~BitmapPS ();
	void copy ();
	void copy (PS &ps, int x=0, int y=0);
	HANDLE bitmaphandle () { return BitmapHandle; }
	void save (char *filename, int bits=24);
};

class MetafilePS : public PS
{	HANDLE MetafileHandle;
	PS *TextPs;
	public :
	MetafilePS (int w, int h);
    MetafilePS (char *filename, int w, int h);
	~MetafilePS ();
	void close ();
	void copy (PS &ps);
	void save (char *filename);
	HANDLE metafilehandle () { return MetafileHandle; }
	virtual void erase () {}
    void setTextPs (PS &ps) { TextPs=&ps; }
	virtual void textextent (char *s, int &w, int &h);
	virtual void textextent (char *s, int n, int &w, int &h);
};

class EnhancedMetafilePS : public PS
{	HANDLE MetafileHandle;
	public :
	EnhancedMetafilePS (PS &ps);
	EnhancedMetafilePS (char *filename, PS &ps);
	~EnhancedMetafilePS ();
	void close ();
	void copy (PS &ps);
	void save (char *filename);
	HANDLE metafilehandle () { return MetafileHandle; }
	virtual void erase () {}
};

class Metafile
{   HANDLE Handle;
	Flag Valid;
	int Width,Height;
	public :
	Metafile (char *filename);
	Metafile (MetafilePS &ps);
	~Metafile ();
	void copy (PS &ps);
	void save (char *filename);
	HANDLE handle () { return Handle; }
	void invalid () { Valid.clear(); }
	 virtual void erase () {}
};

class PlaceableMetafile
{   HANDLE Handle;
	Flag Valid;
	int Width,Height;
	public :
	PlaceableMetafile (MetafilePS &ps);
	~PlaceableMetafile ();
	void save (char *filename);
};

class EnhancedMetafile
{   HANDLE Handle;
	Flag Valid;
	public :
	EnhancedMetafile (EnhancedMetafilePS &ps);
	~EnhancedMetafile ();
	void copy (PS &ps);
	void save (char *filename);
	HANDLE handle () { return Handle; }
	void invalid () { Valid.clear(); }
	 virtual void erase () {}
};

class PrinterPS : public PS
{   String Device,Driver,Output,Jobname;
	int Started;
	DEVMODE *Context;
	public :
    void start (char *printer);
	PrinterPS (char *jobname);
	PrinterPS (char *jobname, char *printer);
	~PrinterPS ();
	void formfeed () { EndPage(Handle); }
	virtual void erase () {}
	int devmode (Window &s);
	int extdevmode (Window &s);
	void open ();
	void start ();
	void stop ();
};

class Windownode
{	Windownode *Next;
	HWND Handle;
	StandardWindow *W;
	public :
	Windownode (StandardWindow *w, HWND handle, Windownode *next) :
		Next(next),Handle(handle),W(w) {}
	StandardWindow *window () { return W; }
	HWND handle () { return Handle; }
	Windownode *next () { return Next; }
	void next (Windownode *next) { Next=next; }
};

class Windows
{	StandardWindow *Act;
	Windownode *First;
	public :
	Windows () : Act(0),First(0) {}
	void add (StandardWindow &w, HWND handle)
	{	First=new Windownode(&w,handle,First);
	}
	void remove (HWND handle);
	StandardWindow *find (HWND handle);
	void act (StandardWindow *window) { Act=window; }
};

class Dialogitem;
class Help;
class Dialog : public WindowObject
{   HWND WHandle;
	Window *W;
	int Id;
	FARPROC Proc;
	HWND Handle;
	Dialog *Next;
	UINT Result;
	Dialogitem *Items;
	Help *Helper;
	int Page;
	int Focus;
	public :
	enum { ok=IDOK,cancel=IDCANCEL };
	Dialog (Window &w, int id) :
		W(&w),Id(id),Proc(0),Next(0),Result(0),Items(0),Helper(0),
		Focus(0)
	{};
	Dialog (HWND h, int id) :
		W(0),WHandle(h),Id(id),Proc(0),Next(0),Result(0),Items(0),Helper(0),
		Focus(0)
	{};
	Dialog (Window &w, int id, Help &h, int page) :
		W(&w),Id(id),Proc(0),Next(0),Result(0),Items(0),Helper(&h),
		Page(page)
	{};
	Dialog (HWND ha, int id, Help &h, int page) :
		W(0),WHandle(ha),Id(id),Proc(0),Next(0),Result(0),Items(0),Helper(&h),
		Page(page)
	{};
	~Dialog ();
	void init ();
	void carryout ();
	void create ();
	friend BOOL FAR PASCAL /*_export*/ DialogProc (HWND handle, UINT msg,
	UINT wParam, LONG lParam);
	int result () { return Result; }
    Dialogitem * entry (Dialogitem *item);
    HWND handle () { return Handle; }
	void move (int x, int y)
	{	SetWindowPos(Handle,0,x,y,0,0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
	}
	void getsize (int &x, int &y, int &w, int &h)
	{   RECT r;
		GetWindowRect(Handle,&r);
		x=r.left; y=r.top;
		w=r.right-r.left+1; h=r.bottom-r.top+1;
	}
	void setfocus (int id) { Focus=id; }
	void title (char *s)
	{	SetWindowText(handle(),s);
	}
	void close ();
};

class Font;
class Dialogitem
{	Dialogitem *Next;
	Dialog *D;
	int Id;
	protected :
	Font *font;
	Flag Readonly,Disabled;
	public :
	Dialogitem (int id, Dialog &d);
	void setreadonly ();
	virtual void init ();
	virtual void exit () {}
	virtual int command (ULONG lParam)
	{   changed();
		return 1;
	}
	virtual void changed () {}
	Dialogitem *next () { return Next; }
	int id () { return Id; }
	HWND handle () { return GetDlgItem(D->handle(),Id); }
	void setfocus () { SetFocus(handle()); }
	void readonly () { Readonly.set(); }
	void disable () { Disabled.set(); }
	Dialog *dialog () { return D; }
	void setfont (Font &f);
};

class UserItem : public Dialogitem
{   public :
	UserItem (int id, Dialog &d) :
		Dialogitem(id,d) {}
	virtual void redraw (PS &ps) {}
	virtual int command (ULONG lParam)
	{	DevicePS ps((HDC)lParam,handle());
		redraw(ps);
		return 0;
	}
	void update ()
	{	InvalidateRect(handle(),0,0);
	}
};

class TextItem : public Dialogitem
{   String S;
	public :
	TextItem (int id, Dialog &d, char *s) :
		Dialogitem(id,d),S(s)
	{}
	virtual void init ();
	void set (char *s);
};

class StringItem : public Dialogitem
{   String S;
	public :
	StringItem (int id, Dialog &d, char *text) :
		Dialogitem(id,d),S(text)
	{}
	StringItem (int id, Dialog &d) :
		Dialogitem(id,d),S("")
	{}
	virtual void init ();
    virtual void exit ();
	operator char * () { return S; }
	void set (char *s);
    char * text () { exit(); return S; }
};

class MultilineItem : public Dialogitem
{   String S;
	int Size;
	public :
	MultilineItem (int id, Dialog &d, char *text, int size) :
		Dialogitem(id,d),S(text),Size(size)
	{}
	MultilineItem (int id, Dialog &d, char *text) :
		Dialogitem(id,d),S(text),Size(0)
	{}
	MultilineItem (int id, Dialog &d) :
		Dialogitem(id,d),S(""),Size(0)
	{}
	virtual void init ();
    virtual void exit ();
	operator char * () { return S; }
    void set (char *s);
};

class DoubleItem : public Dialogitem
{   double X;
	String S;
	public :
	DoubleItem (int id, Dialog &dialog, double x)
		: Dialogitem(id,dialog),S("",64),X(x)
	{}
	virtual void init ();
	virtual void exit ();
	operator double () { return X; }
};

class LongItem : public Dialogitem
{   long N;
	String S;
	public :
	LongItem (int id, Dialog &dialog, long n)
		: Dialogitem(id,dialog),S("",64),N(n)
	{}
	virtual void init ();
	virtual void exit ();
	operator long () { return N; }
    void set (long n);
};

class ButtonItem : public Dialogitem
{   int N;
	public :
	ButtonItem (int id, Dialog &d) :
		N(0),Dialogitem(id,d) {}
	virtual int command (ULONG lParam)
	{	changed(); N++; return 1; }
    operator int () { return N; }
};

class CommandItem : public ButtonItem
{   int (*F) (void *p);
	void *P;
	public :
	CommandItem (int id, Dialog &d, int (*f) (void *),
			void *p)
		: F(f),ButtonItem(id,d),P(p) {}
	virtual int command (ULONG lParam)
	{	if (F(P)) return 0;
		else return 1;
	}
};

class CheckItem : public Dialogitem
{   protected :
	int F;
	public :
	CheckItem (int id, Dialog &dialog, int f=0)
		: Dialogitem(id,dialog),F(f)
	{}
    virtual void init ();
	virtual void exit ();
	virtual int command (ULONG lParam);
	operator int () { return F; }
    void set (int f);
};

class RadioItem;
class RadioCheckButton : public CheckItem
{	RadioItem *R;
	int I;
	public :
	RadioCheckButton
		(int id, Dialog &d, int f, RadioItem *r,
			int i) :
		CheckItem(id,d,f),R(r),I(i) {}
	virtual void changed ();
};

class RadioItem
{	RadioCheckButton ** R;
	int Sel,N;
	public :
	RadioItem (int ids[], int n, Dialog &d,
		int sel=0);
    ~RadioItem ();
	void changed (int i)
	{	R[Sel]->set(0); Sel=i; changed();
	}
	virtual void changed () {}
	operator int () { return Sel; }
	void set (int i)
	{	if (Sel!=i)
		{	R[i]->set(1); R[Sel]->set(0); Sel=i;
        }
    }
	void select (int i)
	{	if (Sel!=i)
		{	set(i); changed();
		}
    }
};

class ListItem : public Dialogitem
{   int N;
	String S,T;
	public :
	ListItem (int id, Dialog &dialog)
		: Dialogitem(id,dialog),N(0)
	{}
	virtual void exit ();
	int insert (char *string);
	void remove (int n);
	void select (int n);
	operator int () { return N; }
	operator char * () { return S; }
	virtual int doubleclicked () { return 0; }
	virtual int command (ULONG lParam)
	{	if (lParam==LBN_DBLCLK && doubleclicked()) return 0;
		return 1;
	}
	char *entry (int n);
	void reset () { N=0; SendMessage(handle(),LB_RESETCONTENT,0,0); }
	void setupdate (int flag) { SendMessage(handle(),WM_SETREDRAW,flag,0); }
	void setwidth (int pixel)
	{	SendMessage(handle(),LB_SETHORIZONTALEXTENT,pixel,0);
	}
};

class MultilistItem : public Dialogitem
{   String T;
	public :
	MultilistItem (int id, Dialog &d)
		: Dialogitem(id,d) {}
	int insert (char *string);
	void remove (int n);
	void select (int n);
	void deselect (int n);
	int selected (int n);
	char *entry (int n);
	void reset () { SendMessage(handle(),LB_RESETCONTENT,0,0); }
	void setupdate (int flag) { SendMessage(handle(),WM_SETREDRAW,flag,0); }
	void setwidth (int pixel)
	{	SendMessage(handle(),LB_SETHORIZONTALEXTENT,pixel,0);
	}
};

class ComboItem : public Dialogitem
{   int N;
	String S,T;
	public :
	ComboItem (int id, Dialog &dialog)
		: Dialogitem(id,dialog),N(0)
	{}
	virtual void exit ();
	int insert (char *string);
	void remove (int n);
	void select (int n);
	operator int () { return N; }
	operator char * () { return S; }
	char *entry (int n);
	char *text () { exit(); return entry(N); }
	void reset () { SendMessage(handle(),CB_RESETCONTENT,0,0); }
};

class PrinterItem : public ListItem
{   String Printer;
	public :
	PrinterItem (int id, Dialog &d, char *init)
		: ListItem(id,d),Printer(init)
	{}
	virtual void init ();
	void properties (Window &w)
	{	HANDLE h;
		OpenPrinter(Printer,&h,0);
		PrinterProperties(w.handle(),h);
	}
};

class Filters
{	char F[1024];
	int l;
	public :
	Filters () : l(0) { F[0]=0; }
	void add (char *string, char *filter);
    operator char * () { return F; }
};

class FileSelector
// a file selector
// should be static, so that path information is not lost
{	int Freturn;
	Window *W;
	String Filter,Title,File,Ext;
	char *Filters;
	int Saving;
	OPENFILENAME o;
	public :
	enum { save=1, load=0 };
	FileSelector(Window &window,
		char *filter, int saving, char *title, char *ext,
			char *filters);
	FileSelector(Window &window,
		char *filter, int saving, char *title, char *ext);
	char *select (HANDLE handle=0);
};

class ColorSelector
{   DWORD Cust[16];
	Window *W;
	ULONG C;
    int R;
	public :
	ColorSelector(Window &window,
		Color c=Color(128,128,128)) : W(&window),C(c) {}
	Color select ();
	int result () { return R; }
};


// ************** Profiling **************

class Profile
{	String A,S,Buffer;
	char buffer[64],def[64];
	public :
	Profile (char *a, char *s) : A(s),Buffer("",256)
	{	A.cat(a);
	}
	Profile (char *a) : A("Software\\"),Buffer("",256)
	{	A.cat(a);
	}
	void write (char *key, char *s)
	{	S.copy(A); S.cat("\\"); S.cat(key);
		RegSetValue(HKEY_CURRENT_USER,S,REG_SZ,s,strlen(s));
	}
	char * read (char *key, char *def)
	{	S.copy(A); S.cat("\\"); S.cat(key);
		long a=256;
		int r=RegQueryValue(HKEY_CURRENT_USER,S,(char *)Buffer,&a);
		if (r==ERROR_MORE_DATA)
		{	Buffer.copy("",a);
			RegQueryValue(HKEY_CURRENT_USER,S,(char *)Buffer,&a);
		}
		else if (r!=ERROR_SUCCESS)
		{	Buffer.copy(def);
		}
		return Buffer;
	}
	void writeint (char *key, int d)
	{	sprintf(buffer,"%d",d); write(key,buffer);
	}
	void writelong (char *key, long d)
	{	sprintf(buffer,"%ld",d); write(key,buffer);
	}
	void writedouble (char *key, double x)
	{	sprintf(buffer,"%0.15e",x); write(key,buffer);
	}
	int readint (char *key, int x)
	{   int y;
		sprintf(def,"%d",x);
		sscanf(read(key,def),"%d",&y);
		return y;
	}
	long readlong (char *key, long x)
	{   long y;
		sprintf(def,"%ld",x);
		sscanf(read(key,def),"%ld",&y);
		return y;
	}
	double readdouble (char *key, double x)
	{   double y;
		sprintf(def,"%0.15e",x);
		sscanf(read(key,def),"%lg",&y);
		return y;
	}
	void remove (char *key)
	{	S.copy(A); S.cat("\\"); S.cat(key);
		RegDeleteKey(HKEY_CURRENT_USER,S);
	}
	void remove ()
	{	RegDeleteKey(HKEY_CURRENT_USER,A);
	}
};

// ************* Help ***************

class Help : public WindowObject
{   String Filename;
	Window *W;
	public :
	Help (char *filename, Window &w)
		: Filename(filename),W(&w) {}
	virtual void create ();
	void close ();
	void index ();
	void helponhelp ();
	void open (char *page);
	void open (int id);
};

//************** Clipboard ****************

class Clipboard
{   Window *W;
	String Content;
	public :
	Clipboard (Window &w) : W(&w) {}
	void copy (Metafile &m, int w, int h);
	void copy (EnhancedMetafile &m);
	void copy (char *s);
	char * gettext ();
};

//************** Rubberbox ****************

class Rubberbox
{
	int X,Y,W,H;
	Flag Active,Done;
	int Wmin,Hmin;
	double Ratio;
	Window *Win;
	void update ();
public :
	Rubberbox (Window &win,
			int wmin=1, int hmin=1, double ratio=0) :
		Active(0),Wmin(wmin),Hmin(hmin),Ratio(ratio),Win(&win),
		Done(0)
	{}
	void perform (int x, int y, int type);
	int x () { return X; }
	int y () { return Y; }
	int w () { return W; }
	int h () { return H; }
	void draw (PS &ps);
	int active () { return Active; }
	int done () { return Done; }
	void reset () { Done.clear(); }
	void ratio (double r) { Ratio=r; }
//	void min (int wmin, int hmin) { Wmin=wmin; Hmin=hmin; }
};

//****************** Font *********************

class Font
{	LOGFONT F;
	HFONT Handle;
	public :
	enum { normal=400, bold=700 };
	enum { italic=1,underline=2,strikeout=4 };
	Font (char *name, int height, int weight=normal,
		int style=0, int orientation=0)
	{	F.lfHeight=height;
		F.lfWidth=0;
		F.lfEscapement=orientation;
		F.lfOrientation=0;
		F.lfWeight=weight;
		F.lfItalic=(style&italic)?1:0;
		F.lfUnderline=(style&underline)?1:0;
		F.lfStrikeOut=(style&strikeout)?1:0;
		F.lfCharSet=ANSI_CHARSET;
		F.lfOutPrecision=OUT_DEFAULT_PRECIS;
		F.lfClipPrecision=CLIP_DEFAULT_PRECIS;
		F.lfQuality=DEFAULT_QUALITY;
		F.lfPitchAndFamily=0;
		strcpy(F.lfFaceName,name);
		Handle=CreateFontIndirect(&F);
	}
	~Font () { DeleteObject(Handle); }
	HFONT handle () { return Handle; }
	LOGFONT *logfont () { return &F; }
	void reinit (int style, int weight=normal)
	{	DeleteObject(Handle);
		F.lfItalic=(style&italic)?1:0;
		F.lfUnderline=(style&underline)?1:0;
		F.lfStrikeOut=(style&strikeout)?1:0;
		F.lfWeight=weight;
		Handle=CreateFontIndirect(&F);
	}
	void reinit ()
	{	DeleteObject(Handle);
		Handle=CreateFontIndirect(&F);
	}
	int style ()
	{   int k=0;
		if (F.lfItalic) k|=italic;
		if (F.lfUnderline) k|=underline;
		if (F.lfStrikeOut) k|=strikeout;
		return k;
	}
	int weight () { return F.lfWeight; }
	int height () { return F.lfHeight; }
	char *name () { return F.lfFaceName; }
};

class FontDialog
{   CHOOSEFONT C;
	Font *F;
	public :
	FontDialog (Font &f) : F(&f) {}
	enum { screen=CF_SCREENFONTS,printer=CF_PRINTERFONTS,
		both=CF_BOTH,wysiwyg=CF_WYSIWYG,
		novector=CF_NOVECTORFONTS,init=CF_INITTOLOGFONTSTRUCT,
		fixed=CF_FIXEDPITCHONLY,truetype=CF_TTONLY,
		effects=CF_EFFECTS,scalable=CF_SCALABLEONLY };
	int select (Window &w, int flags=screen|init)
	{	C.lStructSize=sizeof(CHOOSEFONT);
		C.hwndOwner=w.handle();
		C.Flags=flags;
		C.lpLogFont=F->logfont();
		int k=(ChooseFont(&C)==TRUE);
		if (flags&effects) F->reinit();
		else F->reinit(0);
		return k;
	}
	int select (Window &w, PrinterPS &ps, int flags=wysiwyg|init)
	{	C.hDC=ps.handle();
		return select(w,flags);
	}
};

#include "easy32.h"

#endif

