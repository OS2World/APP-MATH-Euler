#include "easy.h"
#include <windowsx.h>
#ifndef IDHELP
#define IDHELP 9
#endif
#ifndef IDHELP32
#define IDHELP32 998
#endif

//************* Dumps etc. ********************

void dumplong (long n)
{	char text[256];
	sprintf(text,"Hex %lX\nDecimal %ld",n,n);
	MessageBox(NULL,text,"Dump",MB_OK);	
}

void dumpdouble (double x)
{	char text[256];
	sprintf(text,"Hex %g\nDecimal %20.15e",x,x);
	MessageBox(NULL,text,"Dump",MB_OK);
}

void dump (char *s)
{	MessageBox(NULL,s,"Dump",MB_OK);
}

void Warning (char *text, char *title)
{	MessageBox(NULL,text,title,MB_OK|MB_ICONEXCLAMATION);
}

void Message (char *text, char *title)
{	MessageBox(NULL,text,title,MB_OK);
}

int Question (char *text, char *title)
{	return MessageBox(NULL,text,title,MB_YESNO);
}

int QuestionAbort (char *text, char *title)
{	return MessageBox(NULL,text,title,MB_YESNOCANCEL);
}

void Warning (char *text, char *title, Window &window)
{	MessageBox(window.handle(),text,title,MB_OK|MB_ICONEXCLAMATION);
}

void Message (char *text, char *title, Window &window)
{	MessageBox(window.handle(),text,title,MB_OK);
}

int Question (char *text, char *title, Window &window)
{	return MessageBox(window.handle(),text,title,MB_YESNO);
}

int QuestionAbort (char *text, char *title, Window &window)
{	return MessageBox(window.handle(),text,title,MB_YESNOCANCEL);
}

void Warning (char *text, char *title, Dialog &window)
{	MessageBox(window.handle(),text,title,MB_OK|MB_ICONEXCLAMATION);
}

void Message (char *text, char *title, Dialog &window)
{	MessageBox(window.handle(),text,title,MB_OK);
}

int Question (char *text, char *title, Dialog &window)
{	return MessageBox(window.handle(),text,title,MB_YESNO);
}

int QuestionAbort (char *text, char *title, Dialog &window)
{	return MessageBox(window.handle(),text,title,MB_YESNOCANCEL);
}

void Beep (int frequency, double seconds)
{	MessageBeep(0);
}

void Dump (char *text)
{
#ifdef DUMP
	Message(text,"Dump");
#endif
}

int Shift ()
{	return (GetKeyState(VK_SHIFT)&0x0080)!=0;
}

int Control ()
{	return (GetKeyState(VK_CONTROL)&0x0080)!=0;
}

// ********************* Strings ************************

stringtable *progstrings=0;

int String::defaultsize=64;

String::String (char *text, int size)
{   Size=strlen(text);
	if (Size<size) Size=size;
	P=new char[Size+1];
	strcpy(P,text);
}

String::String (char *text)
{	Size=strlen(text);
	P=new char[Size+1];
	strcpy(P,text);
}

String::String ()
{   Size=defaultsize;
	P=new char[Size+1];
	*P=0;
}

String::String (int i)
{	stringtable *p=progstrings;
	char *s="???";
	while (p)
	{	if (p->i==i) { s=p->s; break; }
		if (p->i==0) break;
		p++;
	}
	Size=strlen(s);
	P=new char[Size+1];
	strcpy(P,s);
}

String::String (String &s)
{	Size=s.Size;
	P=new char[Size+1];
	strcpy(P,s.P);
}

String::~String ()
{	delete P;
}

void String::copy (char *text, int size)
{	Size=strlen(text);
	if (Size<size) Size=size;
	char *p=new char[Size+1];
	strcpy(p,text);
	delete P;
	P=p;
}

void String::copy (char *text)
{   int size=strlen(text);
	if (Size<size)
	{	delete P;
		Size=size;
		P=new char[Size+1];
	}
	strcpy(P,text);
}

void String::cat (char *text)
{   long total;
	if ((total=strlen(P)+strlen(text))>Size)
	{   Size=total;
		char *p=new char[Size+1];
		strcpy(p,P);
		delete P;
        P=p;
	}
	strcat(P,text);
}

void String::stripapostroph ()
{	while (strlen(P)>1)
	{	if (P[0]!='"' || P[strlen(P)-1]!='"') break;
		for (int i=0; i<(int)strlen(P)-1; i++)
			P[i]=P[i+1];
		P[strlen(P)-2]=0;
	}
}

char *String::filename ()
{	char *p=P+strlen(P);
	while (p>P)
	{	if (*p=='\\' || *p==':') return p+1;
		p--;
	}
	return p;
}

void String::stripfilename ()
{	*filename()=0;
}

char *String::extension ()
{	char *p=P+strlen(P);
	while (p>P)
	{	if (*p=='\\' || *p=='.') break;
		p--;
	}
	if (p>=P && *p=='.') return p;
	return P+strlen(P);
}

void String::extension (char *ext)
{   *extension()=0;
	cat(ext);
}

int String::testextension (char *ext)
{	char *p=extension();
	while (*p && *ext)
	{   if (::toupper(*p)!=::toupper(*ext)) return 0;
		p++; ext++;
	}
	if (!*p && !*ext) return 1;
	return 0;
}

int String::todouble (double &x)
{	int n;
	if (sscanf(P,"%lg%n",&x,&n)==0) return 0;
	char *p=P+n;
	if (*p) return 0;
	else return 1;
}

int String::tolong (long &x)
{	int n;
	if (sscanf(P,"%ld%n",&x,&n)==0) return 0;
	char *p=P+n;
	if (*p) return 0;
	else return 1;
}

// ********* Window Objects *****************************

WindowObject::WindowObject ()
{   if (!program.started) program.wos()->add(this);
}

void WindowObjects::add (WindowObject *wo)
{   if (!First)
	{	First=Last=wo;
	}
    else
	{   Last->next(wo);
		wo->next(0);
        Last=wo;
    }
}

void WindowObjects::create ()
{	WindowObject *p=First;
	while (p)
	{	p->create();
		p=p->next();
	}
}

//*************** external Variables ********************

extern Program program;
Windows windows;

//*************** Window things *************************

long FAR PASCAL /*_export*/ StandardProc (HWND hwnd,
	unsigned int message,
	unsigned int wParam, long lParam)
{	int flags,scan,c,id,oldw,oldh,newstate;
	StandardWindow *window=windows.find(hwnd);
	RedrawPS *ps;
	POINT *p,pt;
	HWND hp;
	if (!window)
	{   goto notmine;
	}
	switch (message)
	{	case WM_PAINT :
			ps=new RedrawPS(*window);
			window->redraw(*ps);
			delete ps;
			return 0;
		case WM_KEYDOWN :
			flags=Keycode::down|Keycode::virtualkey;
			scan=GET_WM_VKEYTOITEM_CODE(wParam,lParam);
			window->key(flags,0,scan);
			return 0;
		case WM_KEYUP :
			flags=Keycode::up|Keycode::virtualkey;
			scan=GET_WM_VKEYTOITEM_CODE(wParam,lParam);
			window->key(flags,0,scan);
			return 0;
		case WM_CHAR :
			c=GET_WM_VKEYTOITEM_CODE(wParam,lParam);
			flags=Keycode::down|Keycode::charkey;
			window->key(flags,c,0);
			return 0;
		case WM_SIZE :
			newstate=wParam;
			if (newstate!=window->State)
			{	window->State=newstate;
				window->statechanged(window->State);
				if (window->State==StandardWindow::iconic) return 0;
			}
			oldw=window->Width; oldh=window->Height;
			if (LOWORD(lParam)==0 && HIWORD(lParam)==0) return 0;
			window->Width=LOWORD(lParam);
			window->Height=HIWORD(lParam);
			window->adjustsize(wParam,lParam);
			if (oldw!=window->Width || oldh!=window->Height)
			{	window->update();
				window->sized();
			}
			return 0;
		case WM_CLOSE :
			if (window->close()) PostQuitMessage(0);
			return 0;
		case WM_LBUTTONUP :
			if (!window->B1down) return 0;
			window->B1down.clear();
			c=ClickType::button1up; goto mouse;
		case WM_LBUTTONDBLCLK : c=ClickType::button1double; goto mouse;
		case WM_LBUTTONDOWN :
			window->B1down.set();
			c=ClickType::button1down; goto mouse;
		case WM_RBUTTONUP :
			if (!window->B2down) return 0;
            window->B2down.clear();
			c=ClickType::button2up; goto mouse;
		case WM_RBUTTONDBLCLK : c=ClickType::button2double; goto mouse;
		case WM_RBUTTONDOWN :
			window->B2down.set();
			c=ClickType::button2down; goto mouse;
		case WM_MBUTTONUP :
			if (!window->B3down) return 0;
            window->B3down.clear();
			c=ClickType::button3up; goto mouse;
		case WM_MBUTTONDBLCLK : c=ClickType::button3double; goto mouse;
		case WM_MBUTTONDOWN :
			window->B3down.set();
			c=ClickType::button3down; goto mouse;
		case WM_MOUSEMOVE : c=ClickType::mousemove;
			mouse :
			window->clicked(
				(short)LOWORD(lParam)-window->xoffset(),
				(short)HIWORD(lParam)-window->yoffset(),c);
			return 0;
		case WM_COMMAND :
			id=GET_WM_COMMAND_ID(wParam,lParam);
			if (window->Windowmenu)
			{	if (window->Windowmenu->call(id)) return 0;
				else break;
			}
			else break;
		case WM_SETCURSOR :
			if (!window->NewCursor) break;
			hp=GetParent(hwnd);
			if (hp && SendMessage(hp,WM_SETCURSOR,wParam,lParam))
				return 1;
			GetCursorPos(&pt); ScreenToClient(hwnd,&pt);
			if (pt.x>=0 && pt.y>=0 &&
				pt.x<=window->Width+window->XOffset &&
						pt.y<=window->Height+window->YOffset
						&& pt.x>=window->XOffset &&
						pt.y>=window->YOffset)
					SetCursor(window->NewCursor);
			else break;
			return 0;
		case WM_GETMINMAXINFO :
			p=(POINT *)lParam;
			if (p[3].x<window->Minwidth) p[3].x=window->Minwidth;
			if (p[4].x>window->Maxwidth) p[4].x=window->Maxwidth;
			if (p[3].y<window->Minheight) p[3].y=window->Minheight;
			if (p[4].y>window->Maxheight) p[4].y=window->Maxheight;
			return 0;
		case WM_DESTROY :
			PostQuitMessage(0);
			return 0;
		case WM_TIMER :
			window->timer(wParam);
			return 0;
		case WM_SETFOCUS :
			if (window->Caret)
			{	CreateCaret(window->handle(),0,
					window->CaretW,window->CaretH);
				SetCaretPos(window->CaretX,window->CaretY);
				if (window->Showcaret) ShowCaret(window->handle());
				window->Havecaret.set();
			}
			return 0;
		case WM_KILLFOCUS :
			if (window->Caret)
			{   HideCaret(window->handle());
				DestroyCaret();
				window->Havecaret.clear();
			}
			return 0;
		case WM_VSCROLL :
			flags=StandardWindow::vscroll;
			goto scroll;
		case WM_HSCROLL :
			flags=StandardWindow::hscroll;
			scroll :
			window->scroll(flags,(short)LOWORD(wParam),
				(short)HIWORD(wParam));
			return 0;
		case WM_NOTIFY :
			if (window->Tb)
				return window->Tb->notify((NMHDR *)lParam);
			return 0;
		default :
			if (message>=WM_USER)
			{	window->user(message-WM_USER,lParam);
			}
	}
	notmine :
		return DefWindowProc(hwnd,message,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{   char *argv[16]={"Easy"},*p,c[256],*cp;
	int n=1;
	program.set(hInstance,hPrevInstance,nCmdShow);
	program.wos()->create();
	p=(char *)lpszCmdParam;
	while (*p)
	{   while (isspace(*p)) p++;
		cp=c;
		while (*p && !isspace(*p)) *cp++=*p++;
		*cp=0;
		argv[n]=*(new String(c));
		n++;
		if (n>15) break;
	}
	InitCommonControls(); // Nicht unter VisualC++
	winmain(n,argv);
	return program.param();
}

//*********** Window Class **************

WindowClass::WindowClass (char *name, int icon, int style,
	int extra)
		: Name(name),Icon(icon),Style(style),Extra(extra)
{}

void WindowClass::create ()
{   if (program.previnstance()) return;
	WNDCLASS wndclass;
	wndclass.style=Style;
	wndclass.lpfnWndProc=StandardProc;
	wndclass.cbClsExtra=Extra;
	wndclass.cbWndExtra=sizeof(DWORD);
	wndclass.hInstance=(HINSTANCE)program.instance();
	if (Icon)
		wndclass.hIcon=LoadIcon((HINSTANCE)program.instance(),MAKEINTRESOURCE(Icon));
	else
		wndclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)GetStockObject(COLOR_WINDOW);
	wndclass.lpszMenuName=NULL;
	wndclass.lpszClassName=Name;
	RegisterClass(&wndclass);
}

WindowClass::~WindowClass ()
{	if (program.previnstance()) return;
	UnregisterClass(Name,(HINSTANCE)program.instance());
}

//**************** Program ****************************

void Program::set (HANDLE hInstance, HANDLE hPrevInstance,
	int nCmdShow)
{   Instance=hInstance;
	PrevInstance=hPrevInstance;
	Show=nCmdShow;
	started.set();
}

void Program::loop ()
{	while (getmessage())
	{	dispatch();
	}
	Param=Msg.wParam;
}

int Program::getmessage ()
{	return GetMessage(&Msg,NULL,0,0);
}

void Program::dispatch ()
{   if (!Accel || Msg.hwnd!=Main ||
			!TranslateAccelerator((HWND)Main,(HACCEL)Accel,&Msg))
		{	TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
}

char *Program::filename ()
{	Filename.copy("",256);
	GetModuleFileName((HINSTANCE)program.instance(),Filename,255);
    return Filename;
}

//***************** StandardWindow ********************

void StandardWindow::create ()
{   windows.act(this);
	Handle=CreateWindow(Classname.text(),Title.text(),
		Style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		Defaultw,
		Defaulth,
		(HWND)Parent,
		(HMENU)NULL,
		(HINSTANCE)program.instance(),
		(LPVOID)NULL);
	windows.act(0);
	windows.add(*this,Handle);
    SetWindowLong(Handle,0,(DWORD)this);
}

void StandardWindow::init ()
{	ShowWindow(Handle,1);
	OldCursor=SetCursor(LoadCursor(0,IDC_WAIT));
	UpdateWindow(Handle);
	SetCursor(OldCursor);
}

RECT *StandardWindow::client ()
{	GetClientRect(Handle,&Rect);
	return &Rect;
}

StandardWindow *Windows::find (HWND handle)
{   if (Act) return Act;
//	return (StandardWindow *)GetWindowLong(handle,0);
	Windownode *n=First;
	while (n)
	{	if (n->handle()==handle) return n->window();
    	n=n->next();
	}
	return 0;
}

void Windows::remove (HWND handle)
{   Windownode *p=First,*pnote=0;
	while (p)
	{	if (p->handle()==handle)
		{	if (pnote) pnote->next(p->next());
			else First=p->next();
			delete p;
			return;
		}
		pnote=p; p=p->next();
	}
}

void StandardWindow::setmenu (Menu *m)
{	Windowmenu=m;
}

void StandardWindow::destroy ()
{	DestroyWindow(Handle);
	windows.remove(Handle);
}

void StandardWindow::setcursor (int type)
{   HCURSOR h;
	switch (type)
	{	case Cursor::arrow : h=LoadCursor(0,IDC_ARROW); break;
		case Cursor::beam : h=LoadCursor(0,IDC_IBEAM); break;
		case Cursor::wait : h=LoadCursor(0,IDC_WAIT); break;
		case Cursor::cross : h=LoadCursor(0,IDC_CROSS); break;
		case Cursor::icon : h=LoadCursor(0,IDC_ICON); break;
		case Cursor::old :
			SetCursor(OldCursor); NewCursor=0; return;
		default : return;
	}
	SetCursor(h);
	NewCursor=h;
}

void StandardWindow::setcursor (Cursor &c)
{	SetCursor(c.handle());
	NewCursor=c.handle();
}

void StandardWindow::clientmin (int w, int h)
{   adjust(w,h);
	Minwidth=w; Minheight=h;
}

void StandardWindow::clientmax (int w, int h)
{   adjust(w,h);
	Maxwidth=w; Maxheight=h;
}

void StandardWindow::adjust (int &w, int &h)
{	RECT r;
	r.left=0; r.right=w-1;
	r.top=0; r.bottom=h-1;
	AdjustWindowRect(&r,Style,Windowmenu!=0);
	w=r.right-r.left+1;
	h=r.bottom-r.top+1;
}

void StandardWindow::clientsize (int w, int h)
{	adjust(w,h);
	size(w,h);
}

void StandardWindow::showcaret ()
{   if (Showcaret) return;
	if (Havecaret) ShowCaret(handle());
	Showcaret.set();
}

void StandardWindow::setcaret (int x, int y)
{	CaretX=x; CaretY=y;
	if (Havecaret)
	{	SetCaretPos(x,y);
	}
}

void StandardWindow::setcaretsize (int w, int h)
{   if (w==CaretW && h==CaretH) return;
	CaretW=w; CaretH=h;
	if (Havecaret)
	{	if (Showcaret) HideCaret(Handle);
		DestroyCaret();
		CreateCaret(Handle,0,w,h);
		SetCaretPos(CaretX,CaretY);
		if (Showcaret) ShowCaret(Handle);
	}
}

void StandardWindow::hidecaret ()
{   if (!Showcaret) return;
	if (Havecaret) HideCaret(handle());
	Showcaret.clear();
}

void StandardWindow::setscroll (int flag, int pos)
{	SetScrollPos(Handle,flag==vscroll?SB_VERT:SB_HORZ,
		pos,TRUE);
}

void StandardWindow::adjustsize (unsigned int wParam, long lParam)
{   int h;
	if (Sb)
	{	SendMessage(Sb->handle(),
			WM_SIZE,wParam,lParam);
		Height-=Sb->setpanes();
	}
	if (Tb)
	{	SendMessage(Tb->handle(),
			WM_SIZE,wParam,lParam);
		h=Tb->height();
		Height-=h;
		YOffset=h;
	}
}

//*************** Menu ******************************

void Menu::create ()
{	W->setmenu(this);
	Handle=LoadMenu((HINSTANCE)program.instance(),Name);
}

void Menu::init ()
{	SetMenu((HWND)W->handle(),(HMENU)Handle);
}

int Menu::call (int id)
{	Menuentry *p=First;
	Command=id;
	while (p)
	{	if (p->id()==id)
		{	p->call(); return 1;
		}
		p=p->next();
	}
	return 0;
}

void Menu::append (int sub, int id, char *s, void (*f) ())
{	AppendMenu((HMENU)submenu(sub),MF_STRING,id,s);
	add(id,f);
}

void Menu::remove (int sub, int id)
{	DeleteMenu((HMENU)submenu(sub),id,MF_BYCOMMAND);
	if (First) First=First->remove(id);
}

void Menu::check (int id, int f)
{	CheckMenuItem((HMENU)Handle,id,MF_BYCOMMAND|(f?MF_CHECKED:MF_UNCHECKED));
}

void Menu::enable (int id, int f)
{	EnableMenuItem((HMENU)Handle,id,MF_BYCOMMAND|(f?MF_ENABLED:MF_GRAYED));
}

Menuentry * Menuentry::remove (int id)
{   Menuentry *h;
	if (Id==id)
	{	h=Next;
		delete this;
		return h;	
	}
	else if (Next)
	{	Next=Next->remove(id);
	}
    return this;
}

//******************* PS ****************************

void PS::init ()
{   HandlePen=CreatePen(PS_SOLID,1,Color::foreground());
	SelectObject(Handle,HandlePen);
	Col=Color::foreground();
}

void PS::release ()
{   textcolor(Color::text());
	backcolor(Color::background());
	HPEN HandlePenNew=(HPEN)GetStockObject(BLACK_PEN);
	SelectObject(Handle,HandlePenNew);
	DeleteObject(HandlePen);
	HandlePen=HandlePenNew;
    fixedfont();
	unclip();
}

Color PS::color (Color col)
{   if (Col==col)
	{	SelectObject(Handle,HandlePen);
		return Col;
	}
	HPEN HandlePenNew=CreatePen(PS_SOLID,1,col);
	SelectObject(Handle,HandlePenNew);
	DeleteObject(HandlePen);
	HandlePen=HandlePenNew;
	int old=Col;
	Col=col;
	return old;
}

Color PS::defaultcolor ()
{   textcolor(Color::text());
	backcolor(Color::background());
	return color(Color::foreground());
}

void PS::font (Font &f)
{	SelectObject(Handle,f.handle());
	rememberfont(0,f);
}

void PS::rememberfont (int i, Font &f)
{	fonts[i]=&f;
}

void PS::font (int i)
{	SelectObject(Handle,fonts[i]->handle());
}

static int dummy;
static char Work[1024];

int PS::textheight ()
{	TEXTMETRIC tm;
	GetTextMetrics(Handle,&tm);
    return tm.tmHeight;
}

int PS::textavewidth ()
{	TEXTMETRIC tm;
	GetTextMetrics(Handle,&tm);
	return tm.tmAveCharWidth;
}

int PS::textwidth (char *p, int s)
{	strncpy(Work,p,s); Work[s]=0;
	int w;
	textextent(Work,w,dummy);
	return w;
}

int PS::textheight (char *p, int s)
{	strncpy(Work,p,s); Work[s]=0;
	int h;
	textextent(Work,dummy,h);
	return h;
}

void PS::alignment (int halign, int valign, int update)
{   WORD al;
	switch (halign)
	{	case Alignment::right : al=TA_RIGHT; break;
		case Alignment::center : al=TA_CENTER; break;
		default : al=TA_LEFT;
	}
	switch (valign)
	{	case Alignment::bottom : al|=TA_BOTTOM; break;
		default : al|=TA_TOP;
	}
	if (!update) al|=TA_UPDATECP;
	SetTextAlign(Handle,al);
}

void PS::text (int x, int y, char *text,
	int halign, int valign)
{   WORD al;
	int w,h;
	switch (halign)
	{	case Alignment::right : al=TA_RIGHT; break;
		case Alignment::center : al=TA_CENTER; break;
		default : al=TA_LEFT;
	}
	switch (valign)
	{	case Alignment::bottom : al|=TA_BOTTOM; break;
		case Alignment::middle : al|=TA_BOTTOM;
			textextent(text,w,h);
			y+=h/2;
			break;
		default : al|=TA_TOP;
	}
	SetTextAlign(Handle,al);
	TextOut(Handle,x,y,text,strlen(text));
}

char *PS::oneline (char *p, int x, int y, int w, int h)
{	SetTextAlign(Handle,TA_TOP|TA_LEFT);
	char *s=p,*b=p;
	while (1)
	{	if (*s==' ')
			if (textwidth(p,s-p)>w && b>p) { s=b; break; }
			else { b=s; }
		else if (*s==13 || *s==0)
			if (textwidth(p,s-p)>w)
			{	if (b>p) s=b;
				break;
			}
			else break;
		s++;
	}
	strncpy(Work,p,s-p); Work[s-p]=0;
	TextOut(Handle,x,y,Work,strlen(Work));
	if (*s==13) return s+2;
	else if (*s==' ') return s+1;
	else return s;
}

char * PS::multiline (char *p, int x, int y, int w, int h)
{   int ht=textheight("Ty",2),ymax=y+h;
	y++;
	while (*p)
	{   while (*p==' ') *p++;
		if (y+ht>ymax) return p;
		if (*p) p=oneline(p,x,y,w,h);
		y+=ht*9/8;
	}
	return p;
}

void PS::ctext (RECT *rect, char *text)
{	DrawText(Handle,text,-1,rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
}

void PS::textextent (char *s, int &w, int &h)
{	SIZE size;
	GetTextExtentPoint32(Handle,s,strlen(s),&size);
	w=size.cx; h=size.cy;
}

void PS::textextent (char *s, int n, int &w, int &h)
{	SIZE size;
	GetTextExtentPoint32(Handle,s,n,&size);
	w=size.cx; h=size.cy;
}

double PS::textdiameter (char *s, double x, double y)
{	int w,h;
	double r=x*x+y*y;
	textextent(s,w,h);
	double r1=fabs(x*w+y*h),r2=fabs(x*w-y*h);
	if (r1>r2) return r1/r;
	else return r2/r;
}

void PS::lineto (int x, int y)
{   LineTo(Handle,x,y);
}

void PS::lineto (int x, int y, Pen &p)
{   HANDLE h=SelectObject(Handle,p.handle());
	LineTo(Handle,x,y);
	SelectObject(Handle,h);
}

void PS::point (int x, int y, Color col)
{	SetPixel(Handle,x,y,col);
}

void PS::mark (int x, int y, int type)
{   switch (type)
	{	case Markers::dot : point(x,y,Col); break;
		case Markers::cross :
			move(x-Ms,y-Ms); LineTo(Handle,x+Ms+1,y+Ms+1);
			move(x-Ms,y+Ms); LineTo(Handle,x+Ms+1,y-Ms-1);
			break;
		case Markers::plus :
			move(x-Ms,y); LineTo(Handle,x+Ms+1,y);
			move(x,y+Ms); LineTo(Handle,x,y-Ms-1);
			break;
		case Markers::star :
			move(x-Ms,y-Ms); LineTo(Handle,x+Ms+1,y+Ms+1);
			move(x-Ms,y+Ms); LineTo(Handle,x+Ms+1,y-Ms-1);
			move(x-Ms,y); LineTo(Handle,x+Ms+1,y);
			move(x,y+Ms); LineTo(Handle,x,y-Ms-1);
			break;
		case Markers::diamond :
			move(x,y-Ms);
			LineTo(Handle,x+Ms,y); LineTo(Handle,x,y+Ms);
			LineTo(Handle,x-Ms,y); LineTo(Handle,x,y-Ms);
			break;
		case Markers::circle :
			SelectObject(Handle,GetStockObject(HOLLOW_BRUSH));
			Ellipse(Handle,x-Ms,y-Ms,x+Ms,y+Ms);
			break;
		default :
			move(x+Ms,y+Ms);
			LineTo(Handle,x-Ms,y+Ms); LineTo(Handle,x-Ms,y-Ms);
			LineTo(Handle,x+Ms,y-Ms); LineTo(Handle,x+Ms,y+Ms);
			break;
	}
}

void PS::rectangle (int x, int y, int w, int h)
{	SelectObject(Handle,GetStockObject(HOLLOW_BRUSH));
	Rectangle(Handle,x,y,x+w,y+h);
}

void PS::rectangle (int x, int y, int w, int h, Pen &p)
{	HANDLE ha=SelectObject(Handle,GetStockObject(HOLLOW_BRUSH));
	HANDLE hp=SelectObject(Handle,p.handle());
	Rectangle(Handle,x,y,x+w,y+h);
	SelectObject(Handle,hp);
	SelectObject(Handle,ha);
}

void PS::filledrectangle (int x, int y, int w, int h, Color col)
{   HANDLE br=CreateSolidBrush(col);
	HANDLE hd=SelectObject(Handle,br);
	Rectangle(Handle,x,y,x+w,y+h);
	SelectObject(Handle,hd);
	DeleteObject(br);
}

void PS::filledrectangle (int x, int y, int w, int h, Brush &b)
{   HANDLE hd=SelectObject(Handle,b.handle());
	Rectangle(Handle,x,y,x+w,y+h);
	SelectObject(Handle,hd);
}

void PS::bar (int x, int y, int w, int h, Color col)
{   HANDLE br=CreateSolidBrush(col);
	RECT r;
	r.left=x; r.top=y; r.right=x+w; r.bottom=y+h;
	FillRect((HDC)Handle,&r,(HBRUSH)br);
	DeleteObject(br);
}

void PS::bar (int x, int y, int w, int h, Brush &b)
{   RECT r;
	r.left=x; r.top=y; r.right=x+w; r.bottom=y+h;
	FillRect((HDC)Handle,&r,(HBRUSH)b.handle());
}

void PS::polygon (Points &p, Color fillcol, Color col)
{   HANDLE br=CreateSolidBrush(fillcol);
	HANDLE br1=SelectObject(Handle,br);
	color(col);
	SetPolyFillMode(Handle,WINDING);
	Polygon(Handle,p.points(),p.size());
	SelectObject(Handle,br1);
	DeleteObject(br);
}

void PS::fillpolygon (Points &p, Color fillcol)
{   HANDLE br=CreateSolidBrush(fillcol);
	HANDLE br1=SelectObject(Handle,br);
	HPEN hp=CreatePen(PS_NULL,0,0);
	HPEN hp1=(HPEN)SelectObject(Handle,hp);
	SetPolyFillMode(Handle,WINDING);
	Polygon(Handle,p.points(),p.size());
	SelectObject(Handle,hp1);
	DeleteObject(hp);
	SelectObject(Handle,br1);
	DeleteObject(br);
}

void PS::circle (int x, int y, int r, double factor)
{   int r1=r;
	SelectObject(Handle,GetStockObject(HOLLOW_BRUSH));
	if (factor!=1.0) r1*=(int)factor;
	Ellipse(Handle,x-r,y-r1,x+r,y+r1);
}

void PS::circle (int x, int y, int r, double factor,
	Pen &pen)
{   int r1=r;
	SelectObject(Handle,GetStockObject(HOLLOW_BRUSH));
	HANDLE h=SelectObject(Handle,pen.handle());
	if (factor!=1.0) r1*=(int)factor;
	Ellipse(Handle,x-r,y-r1,x+r,y+r1);
    SelectObject(Handle,h);
}

void PS::arc (int x, int y, int r, double factor,
	double phi1, double phi2)
{   int r1=r;
	SelectObject(Handle,GetStockObject(HOLLOW_BRUSH));
	if (factor!=1.0) r1*=(int)factor;
	Arc(Handle,x-r,y-r1,x+r,y+r1,
		(int)(x+cos(phi1)*100),(int)(y+sin(phi1)*100),
		(int)(x+cos(phi2)*100),(int)(y+sin(phi2)*100));
}

void PS::arc (int x, int y, int r, double factor,
	double phi1, double phi2, Pen &pen)
{   int r1=r;
	SelectObject(Handle,GetStockObject(HOLLOW_BRUSH));
	HANDLE h=SelectObject(Handle,pen.handle());
	if (factor!=1.0) r1*=(int)factor;
	Arc(Handle,x-r,y-r1,x+r,y+r1,
		(int)(x+cos(phi1)*100),(int)(y+sin(phi1)*100),
		(int)(x+cos(phi2)*100),(int)(y+sin(phi2)*100));
	SelectObject(Handle,h);
}

void PS::clip (int x, int y, int w, int h)
{   if (Region) unclip();
	Region=CreateRectRgn(x,y,x+w,y+h);
	SelectClipRgn(Handle,Region);
}

void PS::unclip ()
{   SelectClipRgn(Handle,0);
	if (Region) DeleteObject(Region);
	Region=0;
}

void PS::set (int x, int y, int w, int h)
{   POINT p;
	unclip();
	SetViewportOrgEx(Handle,x,y,&p);
	Width=w; Height=h;
	clip(x,y,w,h);
}

//****************** Bitmap PS *************************

BitmapPS::BitmapPS (Window &window) : W(&window)
{   WindowPS ps(window);
	Width=ps.width(); Height=ps.height();
	BitmapHandle=CreateCompatibleBitmap(ps.handle(),Width,Height);
	Handle=CreateCompatibleDC(ps.handle());
	SelectObject(Handle,BitmapHandle);
	init();
	setdefaults();
}

BitmapPS::BitmapPS (Window &window, int w, int h, int bits) :
	W(&window)
{   WindowPS ps(window);
	Width=w; Height=h;
	BitmapHandle=CreateBitmap(Width,Height,1,bits,0);
	Handle=CreateCompatibleDC(ps.handle());
	SelectObject(Handle,BitmapHandle);
	init();
	setdefaults();
}

BitmapPS::BitmapPS (Window &window, int w, int h) : W(&window)
{   WindowPS ps(window);
	Width=w; Height=h;
	BitmapHandle=CreateCompatibleBitmap(ps.handle(),Width,Height);
	Handle=CreateCompatibleDC(ps.handle());
	SelectObject(Handle,BitmapHandle);
	init();
	setdefaults();
}

BitmapPS::~BitmapPS ()
{   release();
	DeleteDC(Handle);
	DeleteObject(BitmapHandle);
}

void BitmapPS::copy ()
{   WindowPS ps(*W);
	copy(ps);
}

void BitmapPS::copy (PS &ps, int x, int y)
{	BitBlt(ps.handle(),x,y,Width,Height,Handle,0,0,SRCCOPY);
}

void BitmapPS::save (char *filename, int nbits)
{	BITMAP bm;
	GetObject(bitmaphandle(),sizeof(BITMAP),&bm);
	size_t size=((width()*nbits-1)/32+1)*4*height();
	char *bits=new char[size];
	int c=0;
	switch (nbits)
	{   case 1 : c=4; break;
		case 8 : c=1024; break;
		case 4 : c=64; break;
	}
	BITMAPINFOHEADER *ih=
		(BITMAPINFOHEADER *)new char[sizeof(BITMAPINFOHEADER)+c];
	ih->biSize=sizeof(BITMAPINFOHEADER);
	ih->biWidth=bm.bmWidth;
	ih->biHeight=bm.bmHeight;
	ih->biPlanes=1;
	ih->biBitCount=nbits;
	ih->biCompression=BI_RGB;
	ih->biSizeImage=0;
	ih->biXPelsPerMeter=0;
	ih->biYPelsPerMeter=0;
	ih->biClrUsed=0;
	ih->biClrImportant=0;
	GetDIBits((HDC)handle(),(HBITMAP)bitmaphandle(),0,height(),
		bits,(BITMAPINFO *)ih,DIB_RGB_COLORS);
	BITMAPFILEHEADER fh;
	fh.bfType=0x4D42;
	fh.bfSize=sizeof(BITMAPFILEHEADER)
		+sizeof(BITMAPINFOHEADER)+size+c;
	fh.bfReserved1=0; fh.bfReserved2=0;
	fh.bfOffBits=sizeof(BITMAPFILEHEADER)+
		sizeof(BITMAPINFOHEADER)+c;
	FILE *f=fopen(filename,"wb");
	 if (!f) return;
	fwrite(&fh,sizeof(BITMAPFILEHEADER),1,f);
	fwrite(ih,sizeof(BITMAPINFOHEADER),1,f);
	if (c>0) fwrite((char *)ih+sizeof(BITMAPINFOHEADER),c,1,f);
	fwrite(bits,1,size,f);
	fclose(f);
	delete bits;
}

//********** Meta Files ************

MetafilePS::MetafilePS (int w, int h)
	: MetafileHandle(0),TextPs(0)
{   Width=w; Height=h;
	Handle=CreateMetaFile(0);
	init();
	setdefaults();
	SetMapMode(Handle,MM_ANISOTROPIC);
    SetWindowOrgEx(Handle,0,0,0);
    SetWindowExtEx(Handle,w,h,0);
}

MetafilePS::~MetafilePS ()
{   release();
	if (!MetafileHandle) close();
	DeleteMetaFile((HMETAFILE)MetafileHandle);
}

void MetafilePS::close ()
{	MetafileHandle=CloseMetaFile(Handle);
}

void MetafilePS::copy (PS &ps)
{   PlayMetaFile((HDC)ps.handle(),(HMETAFILE)MetafileHandle);
}

void MetafilePS::save (char *filename)
{	CopyMetaFile((HMETAFILE)MetafileHandle,filename);
}

void MetafilePS::textextent (char *s, int &w, int &h)
{	SIZE size;
	GetTextExtentPoint32(TextPs?TextPs->handle():Handle,s,strlen(s),&size);
	w=size.cx; h=size.cy;
}

void MetafilePS::textextent (char *s, int n, int &w, int &h)
{	SIZE size;
	GetTextExtentPoint32(TextPs?TextPs->handle():Handle,s,n,&size);
	w=size.cx; h=size.cy;
}

EnhancedMetafilePS::EnhancedMetafilePS (PS &ps)
	: MetafileHandle(0)
{   Width=ps.width(); Height=ps.height();
	RECT r;
	r.left=r.top=0; r.right=ps.width(); r.bottom=ps.height();
	Handle=CreateEnhMetaFile(ps.handle(),0,&r,"");
	init();
	setdefaults();
}

EnhancedMetafilePS::~EnhancedMetafilePS ()
{   release();
	if (!MetafileHandle) close();
	DeleteEnhMetaFile((HENHMETAFILE)MetafileHandle);
}

void EnhancedMetafilePS::close ()
{	MetafileHandle=CloseEnhMetaFile(Handle);
}

void EnhancedMetafilePS::copy (PS &ps)
{   PlayEnhMetaFile((HDC)ps.handle(),(HENHMETAFILE)MetafileHandle,0);
}

void EnhancedMetafilePS::save (char *filename)
{	CopyEnhMetaFile((HENHMETAFILE)MetafileHandle,filename);
}

Metafile::Metafile (char *filename) : Valid(1)
{	Handle=GetMetaFile(filename);
}

Metafile::Metafile (MetafilePS &ps) : Valid(1)
{	Handle=CopyMetaFile((HMETAFILE)ps.metafilehandle(),0);
	Width=ps.width(); Height=ps.height();
}

Metafile::~Metafile ()
{	if (Valid) DeleteMetaFile((HMETAFILE)Handle);
}

void Metafile::copy (PS &ps)
{   PlayMetaFile((HDC)ps.handle(),(HMETAFILE)Handle);
}

void Metafile::save (char *filename)
{   CopyMetaFile((HMETAFILE)Handle,filename);
	DeleteMetaFile(CopyMetaFile((HMETAFILE)Handle,filename));
}

PlaceableMetafile::PlaceableMetafile (MetafilePS &ps) : Valid(1)
{	Handle=CopyMetaFile((HMETAFILE)ps.metafilehandle(),0);
	Width=ps.width(); Height=ps.height();
}

typedef struct
{
  DWORD Key;           /* Magic number (always 9AC6CDD7h) */
  WORD  Handle;        /* Metafile HANDLE number (always 0) */
  SHORT Left;          /* Left coordinate in metafile units */
  SHORT Top;           /* Top coordinate in metafile units */
  SHORT Right;         /* Right coordinate in metafile units */
  SHORT Bottom;        /* Bottom coordinate in metafile units */
  WORD  Inch;          /* Number of metafile units per inch */
  DWORD Reserved;      /* Reserved (always 0) */
  WORD  Checksum;      /* Checksum value for previous 10 WORDs */
} metafileheader;

void metafilewrite (FILE *f, short w, short h)
{	metafileheader hd;
	hd.Key=0x9ac6cdd7l;
	hd.Handle=0;
	hd.Left=hd.Top=0;
	hd.Right=w; //(short)(1440*w/2540);
	hd.Bottom=h; //(short)(1440*h/2540);
	hd.Inch=1440;
	hd.Reserved=0;
	WORD *ptr;
	hd.Checksum = 0;
	for (ptr = (WORD *) &hd; ptr < (WORD *) hd.Checksum; ptr++)
		hd.Checksum ^= *ptr;
	fwrite(&hd,sizeof(metafileheader),1,f);
}

void PlaceableMetafile::save (char *filename)
{   FILE *f=fopen(filename,"wb");
	metafilewrite(f,Width,Height);
	int size=GetMetaFileBitsEx((HMETAFILE)Handle,0,0);
	char *p=new char[size];
	GetMetaFileBitsEx((HMETAFILE)Handle,size,p);
	fwrite(p,size,1,f);
	fclose(f);
}

PlaceableMetafile::~PlaceableMetafile ()
{	if (Valid) DeleteMetaFile((HMETAFILE)Handle);
}


EnhancedMetafile::EnhancedMetafile (EnhancedMetafilePS &ps) : Valid(1)
{	Handle=CopyEnhMetaFile((HENHMETAFILE)ps.metafilehandle(),0);
}

EnhancedMetafile::~EnhancedMetafile ()
{	if (Valid) DeleteEnhMetaFile((HENHMETAFILE)Handle);
}

void EnhancedMetafile::copy (PS &ps)
{   RECT r;
	r.left=r.top=0; r.right=ps.width(),r.bottom=ps.height();
	PlayEnhMetaFile((HDC)ps.handle(),(HENHMETAFILE)Handle,&r);
}

void EnhancedMetafile::save (char *filename)
{	CopyEnhMetaFile((HENHMETAFILE)Handle,filename);
}

//********** Printer ***************

void PrinterPS::start (char *printer)
{   char p[80],*device,*driver,*output;
	if (*printer)
	{	if (GetProfileString("devices",printer,",,,",p,80)<=0)
    		Warning("GetProfileString failed!","Warning");
		device=printer;
		driver=strtok(p,", "); if (!driver) goto noprinter;
		output=strtok(0,", "); if (!output) goto noprinter;
	}
	else
	{	GetProfileString("windows","device",",,,",p,80);
		device=strtok(p,","); if (!device) goto noprinter;
		driver=strtok(0,", "); if (!driver) goto noprinter;
		output=strtok(0,", "); if (!output) goto noprinter;
	}
	Device.copy(device);
	Driver.copy(driver);
	Output.copy(output);
	Handle=(HDC)1;
	return;
	noprinter :
	Handle=0;
}

PrinterPS::PrinterPS (char *jobname, char *printer) :
	Jobname(jobname),Started(0),Context(0)
{	start(printer);
}

PrinterPS::PrinterPS (char *jobname) :
	Jobname(jobname),Started(0),Context(0)
{	start("");
}

void String::getprinter ()
{	copy("",256);
	GetProfileString("windows","device",",,,",P,256);
	strtok(P,",");
}

void PrinterPS::start ()
{	if (Started)
	{	if (StartPage(Handle)<=0) Warning ("StartPage failed!","Warning");
		setdefaults();
		return;
	}
	open();
	DOCINFO docinfo;
	docinfo.cbSize=sizeof(int)+2*sizeof(LPCWSTR);
	docinfo.lpszDocName=(char *)Jobname;
	docinfo.lpszOutput=0;
	if (StartDoc(Handle,&docinfo)<0) Warning("StartDoc failed!","Warning");
	if (StartPage(Handle)<=0) Warning ("StartPage failed!","Warning");
	setdefaults();
	Started=1;
}

void PrinterPS::open ()
{	Handle=CreateDC(Driver,Device,Output,Context);
	if (Handle==NULL) Warning("CreateDC failed!","Warning");
	setdefaults();
	init();
	Width=GetDeviceCaps(Handle,HORZRES);
	Height=GetDeviceCaps(Handle,VERTRES);
    if (Width<200) Width=200;
    if (Height<200) Height=200;
}

void PrinterPS::stop ()
{   if (Started)
		if (EndDoc(Handle)<0) Warning("EndDoc failed!","Warning");
	Started=0;
}

PrinterPS::~PrinterPS ()
{	if (!Handle) return;
	stop();
	release();
	DeleteDC(Handle);
}

typedef int (pascal *DEVMODEPROC)
	(HWND,HANDLE,LPSTR,LPSTR);

int PrinterPS::devmode (Window &window)
{   String D(Driver); D.cat(".drv");
	HINSTANCE h=LoadLibrary(D);
	if (!h) return 0;
	DEVMODEPROC f;
	f=(DEVMODEPROC)GetProcAddress(h,"DEVICEMODE");
	if (!f)
	{   FreeLibrary(h);
		return 0;
	}
	f(window.handle(),h,Device,Output);
	FreeLibrary(h);
	return 1;
}

typedef int (pascal *EXTDEVMODEPROC)
	(HWND,HANDLE,DEVMODE *,char *,LPSTR,DEVMODE *,LPSTR,WORD);

int PrinterPS::extdevmode (Window &w)
{   String D(Driver); D.cat(".drv");
	HINSTANCE h=LoadLibrary(D);
	if (!h) return 0;
	EXTDEVMODEPROC f;
	f=(EXTDEVMODEPROC)GetProcAddress(h,"EXTDEVICEMODE");
	if (!f)
	{   FreeLibrary(h);
		return devmode(w);
	}
	int size=f(w.handle(),h,0,Device,Output,0,0,0);
	Context=(DEVMODE *)malloc(size);
	f(w.handle(),h,Context,Device,Output,0,0,DM_OUT_BUFFER);
	f(w.handle(),h,Context,Device,Output,Context,0,DM_IN_PROMPT|DM_OUT_BUFFER);
	FreeLibrary(h);
	return 1;
}

//********************** Dialogs *************************

Dialog *activedlg=0,*dlgs=0;

BOOL FAR PASCAL /*_export*/ DialogProc (HWND handle, UINT msg,
	UINT wParam, LONG lParam)
{   Dialog *dlg,*dlg1;
	Dialogitem *item;
	int id;
	if (!activedlg) // search dialog using window handle
	{	dlg=dlgs;
		while (dlg)
		{	if (dlg->Handle==handle) break;
			dlg=dlg->Next;
		}
		if (!dlg) return FALSE;
	}
	else dlg=activedlg;
	switch (msg)
	{	case WM_INITDIALOG :
			item=activedlg->Items;
			activedlg->Handle=handle;
			while (item)
			{	item->init();
				item=item->next();
			}
			if (activedlg->Focus)
			{	SetFocus(GetDlgItem(handle,activedlg->Focus));
			}
			activedlg->Next=dlgs;
			dlgs=activedlg;
			activedlg=0;
			return TRUE;
		case WM_CTLCOLORSTATIC :
			item=dlg->Items;
			while (item)
			{   if (item->handle()==(void *)lParam)
				{   item->command(wParam);
					return TRUE;
				}
				item=item->next();
			}
			return FALSE;
		case WM_COMMAND :
			item=dlg->Items;
			id=GET_WM_COMMAND_ID(wParam,lParam);
			while (item)
			{   dlg->Result=id;
				if (item->id()==id)
				{   if (item->command(GET_WM_COMMAND_CMD(wParam,lParam)))
						return TRUE;
					else goto end;
				}
				item=item->next();
			}
			if (id==IDOK || id==IDCANCEL)
			{   end :
				dlg->Result=id;
				// remove dialog from dialog chain
				if (dlgs==dlg) dlgs=dlg->Next;
				else
				{	dlg1=dlgs;
					while (dlg1)
					{	if (dlg1->Next==dlg) break;
						dlg1=dlg1->Next;
					}
					if (dlg1) dlg1->Next=dlg->Next;
				}
				item=dlg->Items;
				while (item)
				{	item->exit();
					item=item->next();
				}
				EndDialog(handle,0);
			}
			else if ((id==IDHELP || id==IDHELP32) && dlg->Helper)
			{   dlg->Helper->open(dlg->Page);
			}
			return TRUE;
	}
	return FALSE;
}

void Dialog::init ()
{	Proc=MakeProcInstance((FARPROC)DialogProc,program.instance());
}

void Dialog::close ()
{	EndDialog(Handle,0);
}

void Dialog::carryout ()
{   if (!Proc) init();
	activedlg=this;
	if (DialogBox((HINSTANCE)program.instance(),MAKEINTRESOURCE(Id),
		W?W->handle():WHandle,(DLGPROC)Proc)==-1)
	{	Warning("Could not create the Dialog!","Error");
	}
}

void Dialog::create ()
{	if (!Proc) init();
	activedlg=this;
	Handle=CreateDialog((HINSTANCE)program.instance(),MAKEINTRESOURCE(Id),
		W?W->handle():WHandle,(DLGPROC)Proc);
    if (Handle==0)
	{	Warning("Could not create the Dialog!","Error");
	}
}

Dialog::~Dialog ()
{	if (Proc) FreeProcInstance(Proc);
}

Dialogitem * Dialog::entry (Dialogitem *item)
{	Dialogitem *note=Items;
	Items=item;
	return note;
}

//********************** Dialogitems ***********************

Dialogitem::Dialogitem (int id, Dialog &d) :
	D(&d),Id(id),Readonly(0),Disabled(0),font(0)
{	Next=d.entry(this);
}

void Dialogitem::init ()
{	if (font) SendMessage(handle(),WM_SETFONT,(WPARAM)font->handle(),
		MAKELPARAM(TRUE,0));
}

void Dialogitem::setreadonly ()
{	SendMessage(handle(),EM_SETREADONLY,TRUE,0);
}

void Dialogitem::setfont (Font &f)
{	font=&f;
}

void TextItem::init ()
{   Dialogitem::init();
	SendMessage(handle(),WM_SETTEXT,0,Parameter(S));

}

void TextItem::set (char *s)
{   S.copy(s);
	SendMessage(handle(),WM_SETTEXT,0,Parameter(S));
}

void StringItem::init ()
{   SendMessage(handle(),WM_SETTEXT,0,Parameter(S));
	if (Readonly) setreadonly();
	if (Disabled) EnableWindow(handle(),0);
}

void StringItem::set (char *s)
{	S.copy(s);
    SendMessage(handle(),WM_SETTEXT,0,Parameter(S));
}

void StringItem::exit ()
{   size_t size=SendMessage(handle(),WM_GETTEXTLENGTH,0,0);
	S.copy("",size);
	SendMessage(handle(),WM_GETTEXT,size+1,Parameter(S));
}

void MultilineItem::init ()
{   Dialogitem::init();
	SendMessage(handle(),WM_SETTEXT,0,Parameter(S));
	if (Size>0) SendMessage(handle(),EM_LIMITTEXT,Parameter(Size),0);
	if (Readonly) setreadonly();
    if (Disabled) EnableWindow(handle(),0);
}

void MultilineItem::set (char *s)
{	S.copy(s);
	SendMessage(handle(),WM_SETTEXT,0,Parameter(S));
}

void MultilineItem::exit ()
{   size_t size=SendMessage(handle(),WM_GETTEXTLENGTH,0,0);
	S.copy("",size);
	SendMessage(handle(),WM_GETTEXT,size+1,Parameter(S));
}

void DoubleItem::init ()
{	Dialogitem::init();
	sprintf(S,"%-0.10g",X);
    SendMessage(handle(),WM_SETTEXT,0,Parameter(S));
	if (Readonly) setreadonly();
    if (Disabled) EnableWindow(handle(),0);
}

void DoubleItem::exit ()
{   size_t size=SendMessage(handle(),WM_GETTEXTLENGTH,0,0);
	S.copy("",size);
	SendMessage(handle(),WM_GETTEXT,size+1,Parameter(S));
	sscanf(S,"%lg",&X);
}

void LongItem::init ()
{	Dialogitem::init();
	sprintf(S,"%-ld",N);
	SendMessage(handle(),WM_SETTEXT,0,Parameter(S));
	if (Readonly) setreadonly();
    if (Disabled) EnableWindow(handle(),0);
}

void LongItem::exit ()
{   size_t size=SendMessage(handle(),WM_GETTEXTLENGTH,0,0);
	S.copy("",size);
	SendMessage(handle(),WM_GETTEXT,size+1,Parameter(S));
	sscanf(S,"%ld",&N);
}

void LongItem::set (long l)
{   N=l;
	sprintf(S,"%-ld",N);
	SendMessage(handle(),WM_SETTEXT,0,Parameter(S));
}

int CheckItem::command (ULONG lParam)
{   int Fn=(SendMessage(handle(),BM_GETCHECK,0,0)!=0);
	if (F!=Fn) { F=Fn; changed(); }
	return 1;
}

void CheckItem::init ()
{	Dialogitem::init();
	SendMessage(handle(),BM_SETCHECK,Parameter(F),0);
	if (Disabled) EnableWindow(handle(),0);
}

void CheckItem::exit ()
{	F=(SendMessage(handle(),BM_GETCHECK,0,0)!=0);
}

void CheckItem::set (int f)
{   F=f;
	SendMessage(handle(),BM_SETCHECK,Parameter(F),0);
}

void RadioCheckButton::changed ()
{   if (F) R->changed(I);
}

RadioItem::RadioItem (int ids[], int n, Dialog &d, int sel) :
	N(n),Sel(sel)
{	R=new RadioCheckButton * [N];
	int i;
	for (i=0; i<N; i++)
		R[i]=new RadioCheckButton(ids[i],d,sel==i,this,i);
}

RadioItem::~RadioItem ()
{ 	int i;
	for (i=0; i<N; i++) delete R[i];
	delete R;
}

void ListItem::exit ()
{	N=SendMessage(handle(),LB_GETCURSEL,0,0);
	int size=SendMessage(handle(),LB_GETTEXTLEN,N,0);
	S.copy("",size);
	SendMessage(handle(),LB_GETTEXT,N,Parameter((char *)S));
}

int ListItem::insert (char *s)
{	return (SendMessage(handle(),LB_ADDSTRING,0,Parameter(s))>=0);
}

void ListItem::remove (int n)
{	SendMessage(handle(),LB_DELETESTRING,n,0);
}

void ListItem::select (int n)
{   N=n;
	SendMessage(handle(),LB_SETCURSEL,N,0);
}

char * ListItem::entry (int n)
{	int l=(int)SendMessage(handle(),LB_GETTEXTLEN,n,0);
	T.copy("",l+1);
	SendMessage(handle(),LB_GETTEXT,n,Parameter((char *)T));
	return T;
}

int MultilistItem::insert (char *s)
{	return (SendMessage(handle(),LB_ADDSTRING,0,Parameter(s))>=0);
}

void MultilistItem::remove (int n)
{	SendMessage(handle(),LB_DELETESTRING,n,0);
}

void MultilistItem::select (int n)
{   SendMessage(handle(),LB_SETSEL,1,n);
}

void MultilistItem::deselect (int n)
{   SendMessage(handle(),LB_SETSEL,0,n);
}

int MultilistItem::selected (int n)
{	return SendMessage(handle(),LB_GETSEL,n,0)!=0;
}

char * MultilistItem::entry (int n)
{	int l=(long)SendMessage(handle(),LB_GETTEXTLEN,n,0);
	T.copy("",l);
	SendMessage(handle(),LB_GETTEXT,n,Parameter((char *)T));
	return T;
}

void ComboItem::exit ()
{	N=SendMessage(handle(),CB_GETCURSEL,0,0);
	int size=SendMessage(handle(),CB_GETLBTEXTLEN,N,0);
	S.copy("",size);
	SendMessage(handle(),CB_GETLBTEXT,N,Parameter((char *)S));
}

int ComboItem::insert (char *s)
{	return (SendMessage(handle(),CB_ADDSTRING,0,Parameter(s))>=0);
}

void ComboItem::remove (int n)
{	SendMessage(handle(),CB_DELETESTRING,n,0);
}

void ComboItem::select (int n)
{   N=n;
	SendMessage(handle(),LB_SETCURSEL,N,0);
}

char * ComboItem::entry (int n)
{	int l=SendMessage(handle(),CB_GETLBTEXTLEN,n,0);
	T.copy("",l);
	SendMessage(handle(),CB_GETLBTEXT,n,(long)(char *)T);
	return T;
}

void PrinterItem::init ()
{   Dialogitem::init();
	String S("",4096);
	GetProfileString("devices",0,"",(char *)S,4096);
	char *p=(char *)S;
	int i=0,sel=0;
	if (*Printer==0)
	{	Printer.copy("",256);
		GetProfileString("windows","device","",Printer,256);
		strtok(Printer,",");
	}
	while (*p)
	{	insert(p);
		if (strcmp(p,Printer)==0) sel=i;
		p+=strlen(p)+1;
        i++;
	}
	select(sel);
}

//******************* File Selector ********************

void Filters::add (char *string, char *filter)
{	strcpy(F+l,string); l+=strlen(string)+1;
	strcpy(F+l,filter); l+=strlen(filter)+1;
	F[l]=0;
}

FileSelector::FileSelector (Window &window,
		char *filter, int saving, char *title, char *ext,
		char *filters)
			: Filter(filter),Title(title),W(&window),
			  File(filter,256),Ext(ext),Filters(filters),
			  Saving(saving)
{}


FileSelector::FileSelector (Window &window,
		char *filter, int saving, char *title, char *ext)
			: Filter(filter),Title(title),W(&window),
			  File(filter,256),Ext(ext),Filters(""),
			  Saving(saving)
{}


char *FileSelector::select (HANDLE handle)
{   String Note(File);
	String Dir(File);
	Dir.stripfilename();
	o.lStructSize=sizeof(OPENFILENAME);
	o.hwndOwner=handle?(HWND)handle:(HWND)W->handle();
	o.lpstrFilter=(*Filters==0)?0:Filters;
	o.lpstrCustomFilter=0;
	o.nMaxFile=256;
	o.nFilterIndex=0;
	File.copy(Filter);
	o.lpstrFile=File;
	o.lpstrInitialDir=Dir;
	o.lpstrFileTitle=0;
	o.lpstrTitle=Title;
	o.Flags=(Saving?OFN_OVERWRITEPROMPT:OFN_FILEMUSTEXIST)
		|OFN_NOCHANGEDIR;
	o.nFileOffset=0;
	o.nFileExtension=0;
	o.lpstrDefExt=Ext;
	o.lCustData=0;
	o.lpfnHook=0;
	o.lpTemplateName=0;
	if (Saving)
	{	if (GetSaveFileName(&o)) return File;
	}
	else
	{	if (GetOpenFileName(&o)) return File;
	}
	File.copy(Note);
	return "";
}

//********** Color Selector ***********

Color ColorSelector::select ()
{	CHOOSECOLOR c;
	c.lStructSize=sizeof(CHOOSECOLOR);
	c.hwndOwner=W->handle();
	c.hInstance=0;
	c.rgbResult=C;
	c.lpCustColors=Cust;
	c.Flags=CC_RGBINIT|CC_FULLOPEN;
	c.lCustData=0;
	c.lpfnHook=0;
	c.lpTemplateName=0;
	R=ChooseColor(&c);
	if (R==TRUE) R=Dialog::ok;
    else R=Dialog::cancel;
	return c.rgbResult;
}

//************ Clipboard ******

void Clipboard::copy (Metafile &meta, int xw, int xh)
{   HGLOBAL h;
	METAFILEPICT *m;
	h=GlobalAlloc(GHND,sizeof(METAFILEPICT));
	m=(METAFILEPICT *)GlobalLock(h);
	m->mm=MM_ANISOTROPIC;
	m->xExt=xw;
	m->yExt=xh;
	m->hMF=(HMETAFILE)meta.handle();
	GlobalUnlock(h);
	OpenClipboard(W->handle());
	EmptyClipboard();
	SetClipboardData(CF_METAFILEPICT,h);
	CloseClipboard();
	meta.invalid();
}

void Clipboard::copy (EnhancedMetafile &meta)
{	OpenClipboard(W->handle());
	EmptyClipboard();
	SetClipboardData(CF_ENHMETAFILE,meta.handle());
	CloseClipboard();
	meta.invalid();
}

void Clipboard::copy (char *s)
{	HGLOBAL h=GlobalAlloc(GHND,strlen(s)+1);
	char *p=(char *)GlobalLock(h);
	strcpy(p,s);
	GlobalUnlock(h);
	OpenClipboard(W->handle());
	EmptyClipboard();
	SetClipboardData(CF_TEXT,h);
	CloseClipboard();
}

char * Clipboard::gettext ()
{	OpenClipboard(W->handle());
	HGLOBAL h=GetClipboardData(CF_TEXT);
	if (!h)
	{	Content.copy("");
    	return Content;
	}
	char *p=(char *)GlobalLock(h);
	Content.copy(p);
	GlobalUnlock(h);
	CloseClipboard();
    return Content;
}

//*********** Help ************

void Help::create ()
{	String a("",256);
	GetModuleFileName((HINSTANCE)program.instance(),a,255);
	a.stripfilename();
	a.cat(Filename);
    Filename.copy(a);
}

void Help::close ()
{	WinHelp(W->handle(),NULL,HELP_QUIT,0);
}

void Help::index ()
{	WinHelp(W->handle(),Filename,HELP_INDEX,0);
}

void Help::helponhelp ()
{	WinHelp(W->handle(),Filename,HELP_HELPONHELP,0);
}

void Help::open (char *s)
{	WinHelp(W->handle(),Filename,HELP_KEY,long(s));
}

void Help::open (int id)
{	WinHelp(W->handle(),Filename,HELP_CONTEXT,id);
}

//************* Rubberbox ***************

void Rubberbox::update ()
{	if (Ratio!=0)
	{	H=((int)(W*Ratio))/2*2+1;
	}
	if (W<Wmin) W=Wmin;
	if (H<Hmin) H=Hmin;
}

void Rubberbox::draw (PS &ps)
{   if (!Active) return;
	ps.mode(PS::xor);
	ps.color(Color(255,255,255));
	ps.rectangle(X-W/2,Y-H/2,W,H);
	ps.mode(PS::copy);
}

void Rubberbox::perform (int x, int y, int type)
{	switch (type)
	{	case ClickType::button1down :
		{	if (Active) break;
			X=x; Y=y; W=1; H=1;
			Active.set();
			update();
            WindowPS wps(*Win);
			draw(wps);
			Win->capture(1);
			Done.clear();
         }
			break;
		case ClickType::mousemove :
		{	if (!Active) return;
            WindowPS wps(*Win);
			draw(wps);
			W=2*abs(x-X)+1; H=2*abs(y-Y)+1;
			update();
			draw(wps);
		}	
            break;
		case ClickType::button1up :
		{	if (!Active) return;
            WindowPS wps(*Win);
			draw(wps);
			Win->capture(0);
			Active.clear();
			Done.set();
		}	
            break; 
	}
}


