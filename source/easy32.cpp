#include "easy.h"

//Dumpfile df("test.log");

StatusBar::StatusBar (StandardWindow &w, int id) :
	W(&w),Text("",512),F(0),Id(id)
{	Handle=CreateWindow(STATUSCLASSNAME,"",WS_CHILD|WS_VISIBLE,
	0,0,0,0,w.handle(),(HMENU)id,(HINSTANCE)program.instance(),0);
	SendMessage(Handle,SB_SIMPLE,1,0);
	w.setstatus(this);
}

StatusBar::StatusBar (StandardWindow &w, int panes, double f[],
		int id)
		: W(&w),Panes(panes),Id(id)
{	Handle=CreateWindow(STATUSCLASSNAME,"",WS_CHILD|WS_VISIBLE,
	0,0,0,0,w.handle(),(HMENU)id,(HINSTANCE)program.instance(),0);
	SendMessage(Handle,SB_SIMPLE,0,0);
	F=new double[Panes];
	for (int i=0; i<Panes; i++) F[i]=f[i];
	setpanes();
	w.setstatus(this);
}

int StatusBar::setpanes ()
{   RECT r;
	int i,w,*pw;
	GetWindowRect(Handle,&r);
	if (!F) return -r.top+r.bottom;
	pw=new int[Panes+1];
	w=r.right-r.left;
	pw[0]=(int)(w*F[0]);
	for (i=1; i<Panes; i++) pw[i]=pw[i-1]+(int)(w*F[i]);
	SendMessage(Handle,SB_SETPARTS,Panes,Parameter(pw));
	delete pw;
	return -r.top+r.bottom;
}

void StatusBar::settext (char *s, int align)
{   switch (align)
	{   case Alignment::center :
			Text.copy("\t"); Text.cat(s); break;
		case Alignment::right :
			Text.copy("\t\t"); Text.cat(s); break;
		default : Text.copy(s);
	}
	SendMessage(Handle,SB_SETTEXT,255,Parameter(Text));
}

void StatusBar::settext (int pane, char *s, int align, int style)
{   switch (align)
	{   case Alignment::center :
			Text.copy("\t"); Text.cat(s); break;
		case Alignment::right :
			Text.copy("\t\t"); Text.cat(s); break;
		default : Text.copy(s);
	}
	SendMessage(Handle,SB_SETTEXT,pane|style,Parameter(Text));
}

void Buttons::addbutton (int id, char *tip="")
{	B[N].iBitmap=Nb;
	B[N].idCommand=id;
	B[N].fsState=TBSTATE_ENABLED;
	B[N].fsStyle=TBSTYLE_BUTTON;
	Tip[N].copy(tip);
	N++; Nb++;
}

void Buttons::addcheck (int id, char *tip)
{	B[N].iBitmap=Nb;
	B[N].idCommand=id;
	B[N].fsState=TBSTATE_ENABLED;
	B[N].fsStyle=TBSTYLE_BUTTON|TBSTYLE_CHECK;
	Tip[N].copy(tip);
	N++; Nb++;
}

void Buttons::addbutton (int id)
{	B[N].iBitmap=Nb;
	B[N].idCommand=id;
	B[N].fsState=TBSTATE_ENABLED;
	B[N].fsStyle=TBSTYLE_BUTTON;
	Tip[N].copy("");
	N++; Nb++;
}

void Buttons::addcheck (int id)
{	B[N].iBitmap=Nb;
	B[N].idCommand=id;
	B[N].fsState=TBSTATE_ENABLED;
	B[N].fsStyle=TBSTYLE_BUTTON|TBSTYLE_CHECK;
	Tip[N].copy("");
	N++; Nb++;
}

void Buttons::addseparator ()
{	B[N].iBitmap=0;
	B[N].idCommand=0;
	B[N].fsState=TBSTATE_ENABLED;
	B[N].fsStyle=TBSTYLE_SEP;
	N++;
}

char * Buttons::tip (int id)
{	int i;
	for (i=0; i<N; i++)
	{	if (B[i].idCommand==id) return Tip[i];
	}
	return "";
}

char * Buttons::tipnumber (int i)
{	if (i<0 || i>=Nb) return "";
	int j=0;
	while(i>=0)
	{	if (B[j].idCommand!=0) i--;
		if (i>=0) j++;
	}
	return Tip[j];
}

TBBUTTON5 * Buttons::button (int i)
{	if (i<0 || i>=Nb) return 0;
	int j=0;
	while(i>=0)
	{	if (B[j].idCommand!=0) i--;
		if (i>=0) j++;
	}
	return B+j;
}

ToolBar::ToolBar (StandardWindow &w, int bitmapid,
	Buttons &b, int id) : Id(id),W(&w),B(&b)
{	Handle=CreateWindow(TOOLBARCLASSNAME,
		0,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|CCS_TOP| //CCS_ADJUSTABLE|
		TBSTYLE_TOOLTIPS|TBSTYLE_WRAPABLE,
		0,0,0,0,
		w.handle(),(HMENU)id,(HINSTANCE)program.instance(),0);
	SendMessage(Handle,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON5),0);
	TBADDBITMAP tb;
	tb.hInst=(HINSTANCE)program.instance();
	tb.nID=bitmapid;
	SendMessage(Handle,TB_ADDBITMAP,b.nbuttons(),Parameter(&tb));
	SendMessage(Handle,TB_ADDBUTTONS,b.size(),Parameter(b.buttons()));
	w.settoolbar(this);
}

void ToolBar::press (int id)
{	SendMessage(Handle,TB_PRESSBUTTON,id,1);
}

void ToolBar::release (int id)
{	SendMessage(Handle,TB_PRESSBUTTON,id,0);
}

void ToolBar::check (int id)
{	SendMessage(Handle,TB_CHECKBUTTON,id,1);
}

void ToolBar::uncheck (int id)
{	SendMessage(Handle,TB_CHECKBUTTON,id,0);
}

int ToolBar::height ()
{	RECT r;
	GetWindowRect(Handle,&r);
	return r.bottom-r.top;
}

int ToolBar::notify (NMHDR *h)
{   TOOLTIPTEXT *t;
	TBNOTIFY *n;
	if (h->code==TTN_NEEDTEXT)
	{	t=(TOOLTIPTEXT *)h;
		lstrcpy(t->szText,B->tip(t->hdr.idFrom));
	}
	else if (h->code==TBN_QUERYINSERT)
	{   return 1;
	}
	else if (h->code==TBN_QUERYDELETE)
	{   return 1;
	}
	else if (h->code==TBN_GETBUTTONINFO)
	{   n=(TBNOTIFY *)h;
		TBBUTTON5 *b=B->button(n->iItem);
		if (b)
		{   memcpy(&(n->tbButton),b,sizeof(TBBUTTON5));
			lstrcpy(n->pszText,B->tipnumber(n->iItem));
			return 1;
		}
	}
	return 0;
}

#ifdef THREADS

void threadstart (void *parameter)
{   Thread *t=(Thread *)parameter;
	t->result(t->call());
	t->Active.clear();
	_endthread();
}

#endif
