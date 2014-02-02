/* OS/2 PM user interface for EULER. */

#define INCL_WIN
#define INCL_GPI
#define INCL_DEV
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_BASE
#define INCL_GPIERRORS

#include <os2.h>
#include "eulerpm.h" /* resource ID and so on */
#undef LONG /* otherwise you get a LONG redefined error */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <process.h>

char outputs[256];

#include "header.h"
#include "sysdep.h"
#include "help.h"

#include "version.h"

/* Define default stacksize */
#ifndef STACKSIZE
#define STACKSIZE (4*1024*1024l)
#endif

long stacksize=STACKSIZE; /* default stack size in Byte */

int linelength=60; /* default width of text screen */
extern int linew,fieldw;
int wchar,hchar; /* font metrics of graphic text */
int editing=0;
int cursoron=0; /* text cursor visible? */
int cursorvisible=0; /* cursor line on screen? */
int matherrors=1; /* generate an error on math errors */
char *topline,*oldtopline; /* top line of visible text */
int notebookcol=1; /* Use colors in notebooks */
int idletime=0; /* Compute on idle time */

HAB hab;
HMQ hmq;
HWND GrafHandle,GrafFrameHandle;
HWND TextHandle,TextFrameHandle;
ULONG GrafStyleFlags =
	FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MAXBUTTON|
	FCF_SHELLPOSITION|FCF_TASKLIST|FCF_ICON;
ULONG TextStyleFlags =
	FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MINMAX|
	FCF_SHELLPOSITION|FCF_TASKLIST|FCF_ICON|FCF_MENU|
	FCF_ACCELTABLE|FCF_VERTSCROLL|FCF_HORZSCROLL;
QMSG qmsg;
LONG xsize,ysize;

int xtextpos=0,ytextpos,htextpos,wtextpos,
	xgrafpos=0,ygrafpos,wgrafpos,hgrafpos; /* window profile pos */

#ifndef TEXTSIZE
#define TEXTSIZE (64*1024l)
#endif
long textsize=TEXTSIZE;

int keyscan,keycode,keydone;
double mousex,mousey;
char *textarea,*textend,*textpos,*textareaend;
LONG xtext,ytext,htextchar,wtextchar,htextabove,htextbelow,
	hgrafabove;
LONG ytextstart=0;
int textlines=10;
int nofocus=0;
int offset=0;
int colorbits=8;
int insertcommands=0;

typedef enum {flogcolor,fpalette,fdither} fillmodetype;
fillmodetype fillmode=flogcolor;

#define MYPALSIZE 64
ULONG mypal [MYPALSIZE]; /* own palette */
HPAL hpal;
int palvalid=0,palcolor;

HWND HelpHandle; /* Handle for Help Instances */
int helpon=0; /* Helpfile found? */

char *startsel,*endsel; /* Selected text */
int selected=0; /* Selection pointers valid? */

/* color palette shuffle: */
LONG palette[16] =
{CLR_WHITE,CLR_BLACK,CLR_DARKRED,CLR_DARKGREEN,
CLR_BLUE,CLR_PINK,CLR_CYAN,CLR_YELLOW,CLR_DARKGRAY,
CLR_DARKBLUE,CLR_RED,CLR_DARKPINK,CLR_GREEN,
CLR_DARKCYAN,CLR_BROWN,CLR_PALEGRAY};

char *colornames[16] =
{"white","black","red","green","blue","cyan","yellow",
"gray","dark gray","dark blue","dark red","dark pink",
"dark green","dark cyan","brown","pale gray"};

int fillcolor1=3,fillcolor2=12,dlinecolor=1;
int uselinestyle=0;
int bw; /* Use black and white */

/* Used to set hue colors: */
LONG alTable[16],oldcol;
int pure=0;

/* Buffer for pasting input from clipboard (or other) */
#ifndef CLIPBUFSIZE
#define CLIPBUFSIZE TEXTSIZE
#endif
long clipsize=CLIPBUFSIZE;
char *clipbuf,*pclip,*clipend;
int pasting=0;

/* Global Graphic Presentation space */

HPS globalgps;
int gpsvalid=0; /* true, if globalgps is set */
int gpsprinter=0; /* Is the global HPS a printer device */
int gpsmeta=0; /* true, if meta file output */

/* Printer things */

char stdprint[256],queuename[256]="Default";
HDC hdcPrinter;
DEVOPENSTRUC dos;
PVOID pQueue;
ULONG cReturnedQueue,cbNeededQueue,cTotalQueue;
SIZEL sizel={0,0};

double xpixel,ypixel,xoffset,yoffset;
int pcsize=90,pcalignhor=50,pcalignver=50;
int printcolor=0; /* Translate 3D colors or print them */
int printgw=1; /* Print backside gray */

HDC hdcMem;
PBITMAPINFO2 pbmi;
HBITMAP hbm;
HPS bps;
int bitmapvalid=0, /* bps is valid */
	bit=0, /* draw to bitmap */
	drawbitmap=0, /* draw to to bitmap during redraw (size change) */
	deletebitmap=0; /* delete bitmap at next redraw (size change) */

int printclip=1, /* clip printing to window */
	colorlines=0,colortext=0,colormarker=0; /* use color on prints */

/* Set size of notebook page for graphic output */
#ifndef PAGESIZE
#define PAGESIZE (512*1024l)
#endif

#ifndef NOTEPAGES
#define NOTEPAGES 16
#endif

char *endnote, /* end of meta note page */
	*note; /* write position */
char *notepages[NOTEPAGES]; /* pointers to pages */
int usedpages=0,existpages=0; /* used and allocated pages */
int noting=1, /* write to meta note book */
	usenote=1; /* use a meta note book in program */

int usemarkers=0;

PFONTMETRICS pfm,printerpfm,textpfm;
LONG fontnumber,textfontnumber,printerfontnumber;
char name[256],last[256]="";

char startpath[256]="";

int nargc;
char **nargv;
TID tid=(unsigned long)(-1),ptid=(unsigned long)(-1),gtid=(unsigned long)(-1);

HMQ hmq1;
HWND DummyHandle=0;

int waiting=0; /* Computation thread is waiting for a key */

HMTX drawsem,textsem; /* Draw semaphor */

long copy_xsize=0,copy_ysize=0; /* Used by copy_bitmap */
int gpscopy=0; /* indicates copying to bitmap */
int copy_usewindow=1; /* Use window size for copying */

/******** Prototypes *************/

void showcursor (void);
void hidecursor (void);

void keyboard (MPARAM mp1, MPARAM mp2);
void update_text (void);
void replay (void);
void select_text (POINTS *p);
void clip_copy (void);
void clip_copy_commands (void);
void paste (void);
void paste_commands (void);
void copy_bitmap (void);
void copy_meta (void);
void save_config (void);
void save_font_config (void);
void save_graphics_config (void);
void save_size_config (void);
void save_print_config (void);
void save_path_config (void);
void save_text (void);
void save_commands (void);
void save_windows (void);
void print_graphics (void);
void write_meta (void);
void write_bitmap (void);

void product (void);
void set_fonts (void);
void set_sizes (void);
void set_graphics (void);
void set_print (void);
void set_paths (void);

void window_exit (void);

void set_slider (void);
void page_up (void);
void page_up_cursor (void);
void page_down (void);
void page_down_cursor (void);
void line_up (void);
void line_down (void);
void set_page (int pos);
void slide_page (int pos);
void set_hslider (void);
void page_left (void);
void page_right (void);
void line_left (void);
void line_right (void);
void set_horpage (int pos);
void new_notebook (void);
void save_notebook_as (void);

char *select_file (char *filter, int saving,
	char *title, char *ok, char *def, char *path);

void destroy_dummy (void);
void edit_function (void);

void prev_command (void);
void next_command (void);
void delete_command (void);
void insert_command (void);
void insert_commands (void);
void remove_output (void);
void set_pos (POINTS *p);
char *det_line (POINTS *p);
void cut (void);
void save_notebook (void);
void load_notebook (void);
void edit_text (void);
void clear_text (void);
void show_command (void);
void update_ycursor (void);

void idle_time (void);

/******************* Debug tools **********************/

HWND dumphandle=HWND_DESKTOP;

void dumplong (long n)
{	char s[256];
	sprintf(s,"%ld, %lx",n,n);
	WinMessageBox(HWND_DESKTOP,dumphandle,s,"Dump",0,
		MB_OK);
}

void dump (char *s)
{	WinMessageBox(HWND_DESKTOP,dumphandle,s,"Dump",0,
		MB_OK);
}

void message (char *s)
{	WinMessageBox(HWND_DESKTOP,dumphandle,s,"Euler",0,
		MB_OK|MB_INFORMATION);
}

void warning (char *s)
{	WinMessageBox(HWND_DESKTOP,dumphandle,s,"Euler",0,
		MB_ENTER|MB_WARNING);
}

int question (char *s)
{	return (WinMessageBox(HWND_DESKTOP,dumphandle,s,"Euler",0,
		MB_YESNO|MB_QUERY)==MBID_YES);
}

/********** Menu Commands that generate EULER text ******/

char loadpath[256]="";

void menu_load (void)
{	char *fname;
	if (pasting) return;
	fname=select_file("*.e",0,"Load File","Load",".e",
		loadpath);
	if (!fname) return;
	sprintf(clipbuf,"\x1Bload \"%s\";\x0A",fname);
	pclip=clipbuf; pasting=1;
	WinPostMsg(DummyHandle,WM_CHAR,0,0);
}

char storepath[256]="";

void menu_store (void)
{	char *fname;
	if (pasting) return;
	fname=select_file("*.sto",1,"Store Euler","Store",".sto",
		storepath);
	if (!fname) return;
	sprintf(clipbuf,"\x1Bstore(\"%s\");\x0A",fname);
	pclip=clipbuf; pasting=1;
	WinPostMsg(DummyHandle,WM_CHAR,0,0);
}

void menu_restore (void)
{	char *fname;
	if (pasting) return;
	fname=select_file("*.sto",0,"Restore Euler","Restore",".sto",
		storepath);
	if (!fname) return;
	sprintf(clipbuf,"\x1Brestore(\"%s\");\x0A",fname);
	pclip=clipbuf; pasting=1;
	WinPostMsg(DummyHandle,WM_CHAR,0,0);
}

void menu_setfile (void)
{	char *fname;
	if (pasting) return;
	fname=select_file("*.e",0,"Set File","Set",".e",loadpath);
	if (!fname) return;
	sprintf(clipbuf,"\x1B""file=\"%s\";\x0A",fname);
	pclip=clipbuf; pasting=1;
	WinPostMsg(DummyHandle,WM_CHAR,0,0);
}

void menu_demo (void)
{   if (pasting) return;
	sprintf(clipbuf,"\x1Bload \"demo\";\x0A");
	pclip=clipbuf; pasting=1;
	WinPostMsg(DummyHandle,WM_CHAR,0,0);
}

void menu_escape (void)
{   if (pasting) return;
	sprintf(clipbuf,"\x1B""endfunction\x0A");
	pclip=clipbuf; pasting=1;
	WinPostMsg(DummyHandle,WM_CHAR,0,0);
}

void menu_loadfile (void)
{   if (pasting) return;
	sprintf(clipbuf,"\x1Bload file;\x0A");
	pclip=clipbuf; pasting=1;
	WinPostMsg(DummyHandle,WM_CHAR,0,0);
}

void menu_reset (void)
{   if (pasting) return;
	sprintf(clipbuf,"\x1Breset();\x0A");
	pclip=clipbuf; pasting=1;
	WinPostMsg(DummyHandle,WM_CHAR,0,0);
}

/******************** Help ******************************/

void init_help (void)
{	HELPINIT hini;
	hini.cb=sizeof(HELPINIT);
	hini.ulReturnCode=0L;
	hini.pszTutorialName=(PSZ)NULL;
	hini.phtHelpTable=
		(PHELPTABLE)MAKELONG(ID_HelpTable,0xFFFF);
	hini.hmodHelpTableModule=(HMODULE)0;
	hini.hmodAccelActionBarModule=(HMODULE)0;
	hini.idAccelTable=0;
	hini.idActionBar=0;
	hini.pszHelpWindowTitle="Euler Help";
	hini.fShowPanelId = CMIC_HIDE_PANEL_ID;
	hini.pszHelpLibraryName="euler.hlp";
	HelpHandle=WinCreateHelpInstance(hab,&hini);
	if (!HelpHandle) { helpon=0; return; }
	helpon=1;
	WinAssociateHelpInstance(HelpHandle,TextFrameHandle);
}

void general (void)
{	if (helpon) WinSendMsg(HelpHandle,HM_EXT_HELP,NULL,NULL);
}

void index (void)
{	if (helpon) WinSendMsg(HelpHandle,HM_HELP_INDEX,NULL,NULL);
}

void content (void)
{   if (helpon) WinSendMsg(HelpHandle,HM_HELP_CONTENTS,NULL,NULL);
}

void show_help (int page)
{	if (helpon) WinSendMsg(HelpHandle,HM_DISPLAY_HELP,MPFROMSHORT(page),
		NULL);
}

/************** Bitmap usage ****************************/

void show_bitmap (HPS bps)
{	HPS gps;
	POINTL a[4];
	gps=WinGetPS(GrafHandle);
	a[0].x=0; a[0].y=0; a[1].x=xsize; a[1].y=ysize;
	a[2].x=0; a[2].y=0; a[3].x=xsize; a[3].y=ysize;
	GpiBitBlt(gps,bps,4,a,ROP_SRCCOPY,BBO_IGNORE);
	WinReleasePS(gps);
}

void create_bitmap (void)
{	PSZ pszData[4]={"Display",NULL,NULL,NULL};
	SIZEL sizlPage={0,0};
	BITMAPINFOHEADER2 bmp;
	LONG alData[2],size;
	hdcMem=DevOpenDC(hab,OD_MEMORY,"*",4,
		(PDEVOPENDATA)pszData,NULLHANDLE);
	sizlPage.cx=xsize; sizlPage.cy=ysize;
	bps=GpiCreatePS(hab,hdcMem,&sizlPage,
		PU_PELS|GPIA_ASSOC|GPIT_MICRO);
	GpiQueryDeviceBitmapFormats(bps,2,(PLONG)alData);
	bmp.cbFix=(ULONG)sizeof(BITMAPINFOHEADER2);
	bmp.cx=xsize; bmp.cy=ysize;
	bmp.cPlanes=alData[0]; bmp.cBitCount=alData[1];
	bmp.ulCompression=BCA_UNCOMP;
	bmp.cbImage=(((xsize*(1<<bmp.cPlanes)*(1<<bmp.cBitCount))+31)
		/32)*ysize;
	bmp.cxResolution=70; bmp.cyResolution=70;
	bmp.cclrUsed=2; bmp.cclrImportant=0;
	bmp.usUnits=BRU_METRIC; bmp.usReserved=0;
	bmp.usRecording=BRA_BOTTOMUP; bmp.usRendering=BRH_NOTHALFTONED;
	bmp.cSize1=0; bmp.cSize2=0;
	bmp.ulColorEncoding=BCE_RGB; bmp.ulIdentifier=0;
	size=sizeof(BITMAPINFO2)+
		(sizeof(RGB2)*(1<<bmp.cPlanes)*(1<<bmp.cBitCount));
	if (DosAllocMem((void **)&pbmi,size,PAG_COMMIT|PAG_READ|PAG_WRITE))
	{	bitmapvalid=0;
		warning("Could not open a bitmap!");
		GpiDestroyPS(bps);
		DevCloseDC(hdcMem);
	}
	pbmi->cbFix=bmp.cbFix;
	pbmi->cx=bmp.cx; pbmi->cy=bmp.cy;
	pbmi->cPlanes=bmp.cPlanes; pbmi->cBitCount=bmp.cBitCount;
	pbmi->ulCompression=BCA_UNCOMP;
	pbmi->cbImage=((xsize+31)/32)*ysize;
	pbmi->cxResolution=70; pbmi->cyResolution=70;
	pbmi->cclrUsed=2; pbmi->cclrImportant=0;
	pbmi->usUnits=BRU_METRIC;
	pbmi->usReserved=0;
	pbmi->usRecording=BRA_BOTTOMUP;
	pbmi->usRendering=BRH_NOTHALFTONED;
	pbmi->cSize1=0; pbmi->cSize2=0;
	pbmi->ulColorEncoding=BCE_RGB; pbmi->ulIdentifier=0;
	hbm=GpiCreateBitmap(bps,&bmp,FALSE,NULL,pbmi);
	GpiSetBitmap(bps,hbm);
	bit=bitmapvalid=1;
	GpiErase(bps);
}

void delete_bitmap (void)
{   if (!bitmapvalid) return;
	bit=bitmapvalid=0;
	GpiDeleteBitmap(hbm);
	DosFreeMem(pbmi);
	GpiDestroyPS(bps);
	DevCloseDC(hdcMem);
}

/*************** Fonts **********************************/

#define FDEFAULT "Default"
char fontname[256]=FDEFAULT,textfontname[256]=FDEFAULT,
	printerfontname[256]=FDEFAULT;
int graphiclines=20,dtextlines=20,outline=0;
FATTRS fat,textfat,printfat;
double fontfactor=1.0;

PFONTMETRICS getfonts (HPS hps, LONG *number)
{	PFONTMETRICS pfm;
	*number=0;
	*number=GpiQueryFonts(hps,QF_PUBLIC,(PSZ)NULL,(PLONG)number,
			sizeof(FONTMETRICS),(PFONTMETRICS)NULL);
	DosAllocMem((void **)&pfm,
		sizeof(FONTMETRICS)*(*number),PAG_COMMIT|PAG_WRITE);
	GpiQueryFonts(hps,QF_PUBLIC,(PSZ)NULL,(PLONG)number,
			sizeof(FONTMETRICS),(PFONTMETRICS)pfm);
	return pfm;
}

void setfat (HPS gps, FATTRS *fat, char *name, long extend)
{	PFONTMETRICS p;
	char *fn;
	LONG i,n,ifound=0,diff,optdiff=10000000l;
	int outl=(unsigned long)(-1);
	p=getfonts(gps,&n);
	if (!strcmp(name,FDEFAULT)) fn=name=p[0].szFacename;
	else
	{   if (name[0]=='(')
		{	if (name[1]=='O') outl=1;
			else outl=0;
			fn=name+3;
		}
		else fn=name;
	}
	for (i=0; i<n; i++)
	{	if (!strcmp(fn,p[i].szFacename) &&
				(outl<0 ||
				(outl && (p[i].fsDefn&FM_DEFN_OUTLINE)) ||
				(!outl && !(p[i].fsDefn&FM_DEFN_OUTLINE))
				)
			)
			{	diff=labs(p[i].lMaxBaselineExt-extend);
				if (diff<optdiff)
				{	optdiff=diff;
					ifound=i;
				}
			}
	}
	i=ifound;
	memset(fat,0,sizeof(FATTRS));
	strcpy(fat->szFacename,p[i].szFacename);
	fat->usRecordLength=sizeof(FATTRS);
	fat->lMatch=p[i].lMatch;
	DosFreeMem(p);
}

void setfont (HPS hps, FATTRS *fat)
{	GpiCreateLogFont(hps,NULL,1,fat);
}

void comp_charsize (HPS gps)
{   FONTMETRICS fm;
	setfont(gps,&fat);
	GpiSetCharSet(gps,1);
	GpiQueryFontMetrics(gps,sizeof(FONTMETRICS),&fm);
	outline=(fm.fsDefn&FM_DEFN_OUTLINE)!=0;
	if (outline)
	{	hchar=1024.0/graphiclines;
		if (gpscopy)
		{	wchar=(copy_ysize/graphiclines)*
			((double)fm.lAveCharWidth/fm.lMaxBaselineExt)/
			copy_xsize*1024.0;
			fontfactor=
				((double)copy_ysize/graphiclines)/fm.lMaxBaselineExt;
		}
		else
		{	wchar=(ysize/graphiclines)*
			((double)fm.lAveCharWidth/fm.lMaxBaselineExt)/
			xsize*1024.0;
			fontfactor=
				((double)ysize/graphiclines)/fm.lMaxBaselineExt;
		}
	}
	else
	{   if (gpscopy)
		{	hchar=fm.lMaxBaselineExt*1024.0/copy_ysize;
			wchar=fm.lAveCharWidth*1024.0/copy_xsize;
		}
		else
		{	hchar=fm.lMaxBaselineExt*1024.0/ysize;
			wchar=fm.lAveCharWidth*1024.0/xsize;
		}
	}
}

void comp_textsize (HPS tps)
{	FONTMETRICS fm;
	GpiQueryFontMetrics(tps,sizeof(FONTMETRICS),&fm);
	htextchar=fm.lMaxBaselineExt;
	htextabove=fm.lMaxAscender;
	htextbelow=fm.lMaxDescender;
	wtextchar=fm.lAveCharWidth+1;
	textlines=ytext/htextchar;
	linelength=xtext/wtextchar;
	linew=linelength/fieldw;
}

void set_bothfonts (void)
{   HPS hps;
	/* set graphics font */
	hps=WinGetPS(GrafHandle);
	setfat(hps,&fat,fontname,ysize/graphiclines);
	comp_charsize(hps);
	WinReleasePS(hps);
	/* set text font */
	DosRequestMutexSem(textsem,-1);
	hps=WinGetPS(TextHandle);
	setfat(hps,&textfat,textfontname,ytextstart/dtextlines);
	setfont(hps,&textfat);
	GpiSetCharSet(hps,1);
	comp_textsize(hps);
	update_text();
	WinReleasePS(hps);
	DosReleaseMutexSem(textsem);
}

/*************** Window handling ************************/

void text_top (void)
{	WinSetWindowPos(TextFrameHandle,HWND_TOP,0,0,0,0,SWP_ZORDER);
	WinSetFocus(HWND_DESKTOP,TextHandle);
}

void graphic_top (void)
{	WinSetWindowPos(GrafFrameHandle,HWND_TOP,0,0,0,0,SWP_ZORDER);
	WinSetFocus(HWND_DESKTOP,GrafHandle);
}

void disable (HWND handle, int id)
{	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(id,TRUE),
		MPFROM2SHORT(MIA_DISABLED,MIA_DISABLED));
}

void enable (HWND handle, int id)
{	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(id,TRUE),
		MPFROM2SHORT(MIA_DISABLED,0));
}

void busymenu (void)
{	HWND handle=WinWindowFromID(TextFrameHandle,FID_MENU);
	disable(handle,IDM_PrintGraphics);
	disable(handle,IDM_SaveGraphicsMeta);
	disable(handle,IDM_SaveGraphicsBitmap);
	disable(handle,IDM_CopyGraphicsM);
	disable(handle,IDM_CopyGraphicsB);
}

void endbusymenu (void)
{	HWND handle=WinWindowFromID(TextFrameHandle,FID_MENU);
	enable(handle,IDM_PrintGraphics);
	enable(handle,IDM_SaveGraphicsMeta);
	enable(handle,IDM_SaveGraphicsBitmap);
	enable(handle,IDM_CopyGraphicsM);
	enable(handle,IDM_CopyGraphicsB);
}

HPOINTER hgrafmouse,htextmouse;

void startbusy (void)
{   hgrafmouse=htextmouse=
		WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE);
	WinSetPointer(HWND_DESKTOP,htextmouse);
}

void endbusy (void)
{   hgrafmouse=WinQuerySysPointer(HWND_DESKTOP,
		SPTR_ARROW,FALSE);
	htextmouse=WinQuerySysPointer(HWND_DESKTOP,
		SPTR_TEXT,FALSE);
	WinSetPointer(HWND_DESKTOP,hgrafmouse);
}

void gthread (void)
{   DosRequestMutexSem(drawsem,-1);
	WinCreateMsgQueue(hab,0);
	globalgps=WinGetPS(GrafHandle);
	drawbitmap=1;
	gpsvalid=1;
	replay();
	gpsvalid=0;
	drawbitmap=0;
	WinReleasePS(globalgps);
	gtid=(unsigned long)(-1);
	DosReleaseMutexSem(drawsem);
	WinInvalidateRect(GrafHandle,NULL,FALSE);
	_endthread();
}

SWP sp;

MRESULT EXPENTRY GrafWindowProc(HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{   HPS gps;
	POINTS *p;
	LONG width,height;
	switch(msg)
	{	case WM_SIZE :
			width=SHORT1FROMMP(mp2);
			height=SHORT2FROMMP(mp2);
			if (width==0 ||
				(width==xsize && height==ysize)) break;
			if (DosRequestMutexSem(drawsem,0)!=0)
			{	WinSetWindowPos(GrafFrameHandle,0,
					sp.x,sp.y,sp.cx,sp.cy,SWP_MOVE|SWP_SIZE);
				break;
			}
			xsize=width;
			ysize=height;
			gps=WinGetPS(GrafHandle);
			setfat(gps,&fat,fontname,ysize/graphiclines);
			comp_charsize(gps);
			WinReleasePS(gps);
			if (bitmapvalid) deletebitmap=1;
			else deletebitmap=0;
			DosReleaseMutexSem(drawsem);
			WinQueryWindowPos(GrafFrameHandle,&sp);
			break;
		case WM_PAINT :
			if (deletebitmap)
			{	delete_bitmap();
				deletebitmap=0;
			}
			if (!bitmapvalid)
			{	create_bitmap();
				drawbitmap=1;
			}
			gps=WinBeginPaint(GrafHandle,NULLHANDLE,NULL);
			GpiErase(gps);
			WinEndPaint(gps);
			if (bitmapvalid && !drawbitmap) show_bitmap(bps);
			else
			{   if (gtid!=(unsigned long)(-1)) break;
				gtid=_beginthread(
					(void (_USERENTRY *)(void *))gthread,
					32*1024l,0);
			}
			break;
		case WM_CHAR :
			if (editing)
			{   text_mode();
				goto def;
			}
			if (pasting || ptid!=(unsigned long)(-1)) goto def;
			keyboard(mp1,mp2);
			break;
		case WM_CLOSE :
			break;
		case WM_BUTTON1CLICK :
			if (!waiting) break;
			if (WinQueryFocus(HWND_DESKTOP)!=GrafHandle)
			{   goto def;
			}
			p=(POINTS *)&mp1;
			mousex=p->x*1024.0/xsize;
			mousey=1024.0-p->y*1024.0/ysize;
			keyscan=(unsigned long)(-1);
			keydone=1;
			WinPostMsg(DummyHandle,WM_CHAR,0,0);
			break;
		case WM_MOUSEMOVE :
			WinSetPointer(HWND_DESKTOP,hgrafmouse);
			break;
		default :
			def :
			return WinDefWindowProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

typedef void threadfunctiontype (void);
threadfunctiontype *threadfunction=0;

void thread (void)
{   if (!threadfunction) return;
	DosRequestMutexSem(drawsem,-1);
	WinCreateMsgQueue(hab,0);
	threadfunction();
	DosEnterCritSec();
	ptid=(unsigned long)(-1);
	WinPostMsg(TextHandle,WM_SEM1,0,0);
	DosReleaseMutexSem(drawsem);
	DosExitCritSec();
	_endthread();
}

void startthread (threadfunctiontype *f)
{   if (ptid!=(unsigned long)(-1)) return;
	startbusy(); busymenu(); WinEnableWindow(GrafHandle,0);
	threadfunction=f;
	ptid=_beginthread(
		(void (_USERENTRY *)(void *))thread,
			32*1024l,0);
	if (ptid==(unsigned long)(-1))
	{	warning("Unable to get print thread!");
	}
	DosSetPriority(PRTYS_THREAD,PRTYC_NOCHANGE,-31,ptid);
}

void check (void)
/* Check menu entry for B/W */
{	HWND handle;
	handle=WinWindowFromID(TextFrameHandle,FID_MENU);
	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(IDM_BW,TRUE),MPFROM2SHORT(MIA_CHECKED,
		bw?MIA_CHECKED:0));
	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(IDM_MathErrors,TRUE),MPFROM2SHORT(MIA_CHECKED,
		matherrors?MIA_CHECKED:0));
	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(IDM_NotebookColors,TRUE),MPFROM2SHORT(MIA_CHECKED,
		notebookcol?MIA_CHECKED:0));
	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(IDM_Idletime,TRUE),MPFROM2SHORT(MIA_CHECKED,
		idletime?MIA_CHECKED:0));
	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(IDM_InsertCommands,TRUE),MPFROM2SHORT(MIA_CHECKED,
		insertcommands?MIA_CHECKED:0));
}

MRESULT EXPENTRY TextWindowProc(HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{	HPS tps;
	POINTS *p;
	LONG width,height;
	switch(msg)
	{	case WM_SIZE :
			width=SHORT1FROMMP(mp2);
			height=SHORT2FROMMP(mp2);
			if (width==0 || (width==xsize && height==ysize)) break;
			DosRequestMutexSem(textsem,-1);
			xtext=width;
			ytext=height;
			if (ytextstart==0) ytextstart=ytext;
			tps=WinGetPS(hwnd);
			setfont(tps,&textfat);
			GpiSetCharSet(tps,1);
			comp_textsize(tps);
			WinReleasePS(tps);
			DosReleaseMutexSem(textsem);
			break;
		case WM_PAINT :
			tps=WinBeginPaint(TextHandle,NULLHANDLE,NULL);
			WinEndPaint(tps);
			DosRequestMutexSem(textsem,-1);
			update_text();
			DosReleaseMutexSem(textsem);
			break;
		case WM_CHAR :
			if (ptid!=(unsigned long)(-1)) goto def;
			if (!nofocus) keyboard(mp1,mp2);
			break;
		case WM_CLOSE :
			if (ptid!=(unsigned long)(-1))
			{	if (!question("Abort print?"))
					break;
			}
			WinPostMsg(hwnd,WM_QUIT,(MPARAM)0,(MPARAM)0);
			break;
		case WM_SHOW :
			if (SHORT1FROMMP(mp1))
				WinSetWindowPos(GrafFrameHandle,NULLHANDLE,0,0,0,0,SWP_SHOW);
			else
				WinSetWindowPos(GrafFrameHandle,NULLHANDLE,0,0,0,0,SWP_HIDE);
			break;
		case WM_MOUSEMOVE :
			WinSetPointer(HWND_DESKTOP,htextmouse);
			break;
		case WM_COMMAND :
			switch (SHORT1FROMMP(mp1))
			{	case IDM_Exit :
					if (ptid!=(unsigned long)(-1))
					{	if (!question("Abort print?"))
							break;
					}
					WinPostMsg(hwnd,WM_QUIT,(MPARAM)0,(MPARAM)0);
					break;
				case IDM_General :
					if (!waiting) break;
					general();
					break;
				case IDM_Index :
					if (!waiting) break;
					index();
					break;
				case IDM_Content :
					if (!waiting) break;
					content();
					break;
				case IDM_Keyboard :
					if (!waiting) break;
					show_help(816);
					break;
				case IDM_Product :
					if (!waiting) break;
					product();
					break;
				case IDM_CopyAll :
					if (!waiting) break;
					DosRequestMutexSem(textsem,-1);
					clip_copy();
					DosReleaseMutexSem(textsem);
					break;
				case IDM_BW :
					if (!waiting) break;
					bw=!bw;
					check();
					break;
				case IDM_MathErrors :
					if (!waiting) break;
					matherrors=!matherrors;
					check();
					break;
				case IDM_PasteAll :
					if (!waiting) break;
					DosRequestMutexSem(textsem,-1);
					paste();
					DosReleaseMutexSem(textsem);
					break;
				case IDM_CopyCommands :
					if (!waiting) break;
					DosRequestMutexSem(textsem,-1);
					clip_copy_commands();
					DosReleaseMutexSem(textsem);
					break;
				case IDM_PasteCommands :
					if (!waiting) break;
					DosRequestMutexSem(textsem,-1);
					paste_commands();
					DosReleaseMutexSem(textsem);
					break;
				case IDM_SaveSettings :
					if (!waiting) break;
					save_config();
					break;
				case IDM_SaveText :
					if (!waiting) break;
					DosRequestMutexSem(textsem,-1);
					save_text();
					DosReleaseMutexSem(textsem);
					break;
				case IDM_SaveCommands :
					if (!waiting) break;
					DosRequestMutexSem(textsem,-1);
					save_commands();
					DosReleaseMutexSem(textsem);
					break;
				case IDM_SaveWindows :
					if (!waiting) break;
					save_windows();
					break;
				case IDM_Load :
					if (!waiting) break;
					menu_load();
					break;
				case IDM_Store :
					if (!waiting) break;
					menu_store();
					break;
				case IDM_Restore :
					if (!waiting) break;
					menu_restore();
					break;
				case IDM_LoadDemo :
					if (!waiting) break;
					menu_demo();
					break;
				case IDM_SetFile :
					if (!waiting) break;
					menu_setfile();
					break;
				case IDM_Reset :
					if (!waiting) break;
					menu_reset();
					break;
				case IDM_LoadFile :
					if (!waiting) break;
					menu_loadfile();
					break;
				case IDM_CopyGraphicsB :
					if (!waiting) break;
					if (copy_usewindow)
					{	copy_xsize=xsize;
						copy_ysize=ysize;
					}
					startthread(copy_bitmap);
					break;
				case IDM_CopyGraphicsM :
					if (!waiting) break;
					startthread(copy_meta);
					break;
				case IDM_PrintGraphics :
					if (!waiting) break;
					startthread(print_graphics);
					break;
				case IDM_SaveGraphicsMeta :
					if (!waiting) break;
					startthread(write_meta);
					break;
				case IDM_SaveGraphicsBitmap :
					if (!waiting) break;
					if (copy_usewindow)
					{	copy_xsize=xsize;
						copy_ysize=ysize;
					}
					startthread(write_bitmap);
					break;
				case IDM_SetFonts :
					if (!waiting) break;
					set_fonts();
					break;
				case IDM_SetSizes :
					if (!waiting) break;
					set_sizes();
					break;
				case IDM_SetGraphics :
					if (!waiting) break;
					set_graphics();
					break;
				case IDM_SetPrint :
					if (!waiting) break;
					set_print();
					break;
				case IDM_SetPaths :
					if (!waiting) break;
					set_paths();
					break;
				case IDM_Function :
					if (!waiting) break;
					edit_function();
					break;
				case IDM_PreviousCommand :
					if (!editing || nojump)
					{	if (waiting) { keyscan=cursor_up; keydone=1; }
						break;
					}
					prev_command();
					break;
				case IDM_NextCommand :
					if (!editing || nojump)
					{	if (waiting) { keyscan=cursor_down; keydone=1; }
						break;
					}
					next_command();
					break;
				case IDM_DeleteCommand :
					if (!editing || nojump) break;
					delete_command();
					break;
				case IDM_InsertCommand :
					if (!editing || nojump) break;
					insert_command();
					break;
				case IDM_InsertCommands :
					if (!editing) break;
					insert_commands();
					break;
				case IDM_NotebookColors :
					if (!waiting) break;
					notebookcol=!notebookcol;
					check();
					break;
				case IDM_RemoveOutput :
					if (!editing) break;
					remove_output();
					break;
				case IDM_Cut :
					if (!editing) break;
					cut();
					break;
				case IDM_SaveNotebook :
					if (!editing) break;
					save_notebook();
					break;
				case IDM_SaveNotebookAs :
					if (!editing) break;
					save_notebook_as();
					break;
				case IDM_LoadNotebook :
					if (!editing) break;
					load_notebook();
					break;
				case IDM_NewNotebook :
					if (!editing) break;
					new_notebook();
					break;
				case IDM_EditText :
					if (!editing) break;
					edit_text();
					break;
				case IDM_DeleteText :
					if (!editing) break;
					clear_text();
					break;
				case IDM_Idletime :
					idle_time();
					break;
			}
			break;
		case WM_SEM1 :
			endbusy(); endbusymenu();
			WinEnableWindow(GrafHandle,1);
			break;
		case WM_SEM2 :
			warning((char *)mp2);
			break;
		case WM_SEM3 :
			message("Job done");
			break;
		case WM_BUTTON1CLICK :
			if (!waiting) break;
			if (WinQueryFocus(HWND_DESKTOP)!=TextHandle)
			{   goto def;
			}
			p=(POINTS *)&mp1;
			DosRequestMutexSem(textsem,-1);
			set_pos(p);
			DosReleaseMutexSem(textsem);
			break;
		case WM_BUTTON2CLICK :
			if (!waiting) break;
			if (WinQueryFocus(HWND_DESKTOP)!=TextHandle)
			{   goto def;
			}
			p=(POINTS *)&mp1;
			DosRequestMutexSem(textsem,-1);
			select_text(p);
			DosReleaseMutexSem(textsem);
			break;
		case WM_SETFOCUS :
			if (SHORT1FROMMP(mp2))
			{   nofocus=0;
				showcursor();
			}
			else
			{   hidecursor();
				nofocus=1;
			}
			break;
		case WM_VSCROLL :
			if (!waiting) break;
			DosRequestMutexSem(textsem,-1);
			switch (SHORT2FROMMP(mp2))
			{	case SB_PAGEUP :
					page_up(); break;
				case SB_PAGEDOWN :
					page_down(); break;
				case SB_LINEUP :
					line_up(); break;
				case SB_LINEDOWN :
					line_down(); break;
				case SB_SLIDERPOSITION :
					set_page(SHORT1FROMMP(mp2));
				case SB_SLIDERTRACK :
					slide_page(SHORT1FROMMP(mp2));
			}
			DosReleaseMutexSem(textsem);
			break;
		case WM_HSCROLL :
			if (!waiting) break;
			DosRequestMutexSem(textsem,-1);
			switch (SHORT2FROMMP(mp2))
			{	case SB_PAGELEFT :
					page_left(); break;
				case SB_PAGERIGHT :
					page_right(); break;
				case SB_LINELEFT :
					line_left(); break;
				case SB_LINERIGHT :
					line_right(); break;
				case SB_SLIDERPOSITION :
					set_horpage(SHORT1FROMMP(mp2));
			}
			DosReleaseMutexSem(textsem);
			break;
		default :
			def : return WinDefWindowProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

int window_init (void)
/* Open the windows */
{	hab=WinInitialize(0);
	hmq=WinCreateMsgQueue(hab,0);
	WinRegisterClass(hab,"EulerG",GrafWindowProc,
		CS_SIZEREDRAW,0);
	WinRegisterClass(hab,"EulerT",TextWindowProc,
		CS_SIZEREDRAW,0);
	TextFrameHandle=WinCreateStdWindow(HWND_DESKTOP,
		WS_VISIBLE,&TextStyleFlags,"EulerT",
		"Euler Text",WS_VISIBLE,
		(HMODULE)0,ID_TextWindow,&TextHandle);
	if (xtextpos>0 && ytextpos>0 && wtextpos>0 && htextpos>0)
	{	WinSetWindowPos(TextFrameHandle,NULLHANDLE,
			xtextpos,ytextpos,wtextpos,htextpos,SWP_MOVE|SWP_SIZE);
	}
	GrafFrameHandle=WinCreateStdWindow(HWND_DESKTOP,
		WS_VISIBLE,&GrafStyleFlags,"EulerG",
		"Euler Graphics",WS_VISIBLE,
		(HMODULE)0,ID_GrafWindow,&GrafHandle);
	if (xgrafpos>0 && ygrafpos>0 && wgrafpos>0 && hgrafpos>0)
	{	WinSetWindowPos(GrafFrameHandle,NULLHANDLE,
			xgrafpos,ygrafpos,wgrafpos,hgrafpos,SWP_MOVE|SWP_SIZE);	}
	WinSetWindowPos(TextFrameHandle,HWND_TOP,0,0,0,0,SWP_ZORDER);
	WinSetFocus(HWND_DESKTOP,TextHandle);
	create_bitmap();
	dumphandle=TextHandle;
	init_help();
	set_bothfonts();
	set_slider();
	set_hslider();
	return 1;
}

void window_exit (void)
/* Close the windows, free stack */
{	WinDestroyWindow(GrafHandle);
	WinDestroyWindow(TextHandle);
	WinDestroyMsgQueue(hmq);
	WinTerminate(hab);
	delete_bitmap();
	free(ramstart);
}

int memory_init (void)
/***** memory_init
	get memory for stack.
*****/
{	ramstart=(char *)malloc(stacksize);
	if (!ramstart) return 0;
	ramend=ramstart+stacksize;
	textpos=textend=topline=textarea=(char *)malloc(textsize);
	if (!textarea) return 0;
	*textarea=0;
	textareaend=textarea+textsize;
	clipbuf=(char *)malloc(clipsize);
	if (!clipbuf) return 0;
	clipend=clipbuf+clipsize;
	return 1;
}

#ifndef NOSHRINK

int shrink (ULONG size)
/**** allows shrinking of memory for single task systems.
simply return 1 if you do not support this or set NOSHRINK in funcs.c
*****/
{	return 1;
}

#endif

/******************** writing the meta file ************/

int create_page (void)
/* create a new meta note page */
{	if (usedpages>=NOTEPAGES) return 0;
	if (usedpages<existpages)
	{	usedpages++;
		*((int *)note)=-1;
		note=notepages[usedpages];
		endnote=notepages[usedpages]+PAGESIZE;
		return 1;
	}
	notepages[usedpages]=(char *)malloc(PAGESIZE);
	if (!notepages[usedpages]) return 0;
	if (usedpages>0) *((int *)note)=-1;
	note=notepages[usedpages];
	endnote=notepages[usedpages]+PAGESIZE;
	existpages++; usedpages++;
	return 1;
}

void destroy_pages (void)
/* free all but one page */
{	int i;
	for (i=1; i<existpages; i++)
		free(notepages[i]);
	existpages=1; usedpages=1;
	note=notepages[0]; endnote=note+PAGESIZE-sizeof(int);
}

void testnote (int n)
/* test if space for n chars on meta note page */
{   if (!usenote) return;
	if (note+n<=endnote) return;
	if (create_page()) return;
	usenote=0;
}

void longwrite (double n)
/***** write a double to the metafile as long
*****/
{	long k[1];
	k[0]=(long)(n*1000.0);
	if (metafile) fwrite(k,sizeof(long),1,metafile);
	if (usenote)
	{	*((long *)note)++=k[0];
	}
}

void intwrite (int n)
/***** write an int to the metafile
*****/
{	int k[1];
	k[0]=n;
	if (metafile) fwrite(k,sizeof(int),1,metafile);
	if (usenote)
	{	*((int *)note)++=k[0];
	}
}

void stringwrite (char *s)
/***** write a string to the metafile
*****/
{	int c;
	size_t size;
	if (metafile)
	{	c=0;
		while (*s) { putc(*s++,metafile); c++; }
		putc(0,metafile); c++;
		if (c%2) putc(0,metafile);
	}
	size=((strlen(s)+2)/2)*2;
	if (usenote)
	{	strcpy(note,s);
		note+=size;
	}
}

/******************* Graphics ***************************/

HPS geth (void)
{   if (gpsvalid) return globalgps;
	DosRequestMutexSem(drawsem,-1);
	return WinGetPS(GrafHandle);
}

void relh (HPS h)
{   if (gpsvalid) return;
	DosReleaseMutexSem(drawsem);
	WinReleasePS(h);
}

/****
The graphic screen has coordinates from 0.0 to 1024.0 (double).
There should be a function, which computes the correct screen
coordinates from these internal ones.
****/

void clear_graphics (void)
/***** clear the graphics screen
*****/
{   POINTL p;
	HPS gps=geth();
	if (noting)
	{   destroy_pages();
		intwrite(6);
	}
	if (!bw && !gpsmeta) GpiErase(gps);
	else
	{	p.x=0; p.y=0;
		GpiSetBackMix(gps,BM_OVERPAINT);
		GpiSetPattern(gps,PATSYM_SOLID);
		GpiMove(gps,&p);
		if (gpscopy)
		{	p.x=copy_xsize-1; p.y=copy_ysize-1;
		}
		else
		{	p.x=xsize-1; p.y=ysize-1;
		}
		GpiSetColor(gps,bw?CLR_WHITE:CLR_BACKGROUND);
		GpiBox(gps,DRO_FILL,&p,0,0);
		GpiSetBackMix(gps,BM_LEAVEALONE);
		GpiSetColor(gps,CLR_BLACK);
	}
	if (bit)
	{	if (!bw) GpiErase(bps);
		else
		{	p.x=0; p.y=0;
			GpiSetBackMix(bps,BM_OVERPAINT);
			GpiSetPattern(bps,PATSYM_SOLID);
			GpiMove(bps,&p);
			p.x=xsize-1; p.y=ysize-1;
			GpiSetColor(bps,bw?CLR_WHITE:CLR_BACKGROUND);
			GpiBox(bps,DRO_FILL,&p,0,0);
			GpiSetBackMix(bps,BM_LEAVEALONE);
			GpiSetColor(bps,CLR_BLACK);
		}
	}
	relh(gps);
	WinValidateRect(GrafHandle,NULL,0);
}

int textmode=0;

void graphic_mode (void)
/***** graphics
	Switch to graphics. Text must not be deleted.
	On a window system make graphics visible.
*****/
{   if (!textmode) return;
	graphic_top();
	textmode=0;
}

void text_mode (void)
/***** text_mode
	Switch to text. Graphics should not be deleted.
	On a window system make text visible.
*****/
{   if (textmode) return;
	text_top();
	textmode=1;
}

/****************** palette creation and destruction ********/

void create_palette (HPS hps, int color)
{	int i,red,green,blue,red0,green0,blue0;
	LONG rgb;
	double hue;
	if (palvalid && palcolor==color) return;
	if (palvalid) GpiDeletePalette(hpal);
	if (color==0)
	{	for (i=0; i<MYPALSIZE/2; i++)
		{	hue=(double)i/MYPALSIZE*2;
			red=(1-hue)*255; green=hue*256; blue=0;
			if (green>255) green=255;
			mypal[i]=red*65536l+green*256+blue;
			red=0; green=(1-hue)*255; blue=hue*256;
			if (blue>255) blue=255;
			mypal[MYPALSIZE/2+i]=red*65536l+green*256+blue;
		}
	}
	else if (color==1)
	{	for (i=0; i<MYPALSIZE; i++)
		{	hue=(double)i/MYPALSIZE;
			red=green=blue=hue*256;
			mypal[i]=red*65536l+green*256+blue;
		}
	}
	else
	{	GpiQueryLogColorTable(hps,0,0,16,(PLONG)alTable);
		rgb=alTable[palette[!bw?color:(color>0)]];
		red0=(rgb>>16)&255; green0=(rgb>>8)&255; blue0=rgb&255;
		for (i=0; i<MYPALSIZE; i++)
		{	hue=(double)i/MYPALSIZE;
			red=red0+hue*(256-red0);
			green=green0+hue*(256-green0);
			blue=blue0+hue*(256-blue0);
			mypal[i]=red*65536l+green*256+blue;
		}
	}
	hpal=GpiCreatePalette(hab,0,LCOLF_CONSECRGB,MYPALSIZE,mypal);
	palvalid=1; palcolor=color;
}

void setcolor (HPS hps, double hue, int color)
/* set draw mode depending on hue and color and hue style.
important for gbar and gfillh. hue is in [0,1).
*/
{   LONG rgb;
	int red,green,blue,sat;
	double hue1;
	switch (bw?fdither:fillmode)
	{   case fpalette :
			create_palette(hps,color);
			GpiSelectPalette(hps,hpal);
			GpiSetColor(hps,hue*MYPALSIZE);
			GpiSetPattern(hps,PATSYM_SOLID);
			if (bit)
			{	create_palette(hps,color);
				GpiSelectPalette(bps,hpal);
				GpiSetColor(bps,hue*MYPALSIZE);
				GpiSetPattern(bps,PATSYM_SOLID);
			}
			break;
		case flogcolor :
			GpiQueryLogColorTable(hps,0,2,1,
				(PLONG)alTable);
			oldcol=alTable[0];
			if (color==1)
			{	sat=floor(255*hue);
				rgb=sat*65536l+sat*256+sat;
			}
			else if (color==0)
			{	if (hue<0.5)
				{	blue=0;
					red=(1-2*hue)*256;
					if (red>255) red=255;
					green=floor(hue*512);
					if (green>255) green=255;
				}
				else
				{	red=0;
					green=(1-hue)*512;
					if (green>255) green=255;
					blue=floor((hue-0.5)*512);
					if (blue>255) blue=255;
				}
				rgb=red*65536l+green*256+blue;
			}
			else
			{	sat=floor(255*hue); hue1=1-hue;
				red=sat+((oldcol>>16)&255)*hue1;
				if (red>255) red=255;
				green=sat+((oldcol>>8)&255)*hue1;
				if (green>255) green=255;
				blue=sat+(oldcol&255)*hue1;
				if (blue>255) blue=255;
				rgb=red*65536l+green*256+blue;
			}
			alTable[0]=rgb;
			GpiCreateLogColorTable(hps,pure?LCOL_PURECOLOR:0,
				LCOLF_CONSECRGB,2,1,(PLONG)alTable);
			GpiSetPattern(hps,PATSYM_SOLID);
			GpiSetColor(hps,2);
			if (bit)
			{	GpiCreateLogColorTable(bps,pure?LCOL_PURECOLOR:0,
					LCOLF_CONSECRGB,2,1,(PLONG)alTable);
				GpiSetPattern(bps,PATSYM_SOLID);
				GpiSetColor(bps,2);
			}
			break;
		case fdither :
			if (color==0)
			{	if (hue<11.0/19.0)
				{   hue*=19.0/11.0;
					GpiSetColor(hps,CLR_RED);
					GpiSetBackColor(hps,CLR_GREEN);
					if (bit)
					{	GpiSetColor(bps,CLR_RED);
						GpiSetBackColor(bps,CLR_GREEN);
					}
				}
				else
				{	hue=(1-hue)*19.0/10.0;
					GpiSetColor(hps,CLR_BLUE);
					GpiSetBackColor(hps,CLR_GREEN);
					if (bit)
					{	GpiSetColor(bps,CLR_BLUE);
						GpiSetBackColor(bps,CLR_GREEN);
					}
				}
			}
			else
			{	GpiSetColor(hps,palette[!bw?color:(color>0)]);
				if (bit) GpiSetColor(bps,palette[bw?color:(color>0)]);
			}
			switch ((int)floor(hue*10))
			{	case 0 : GpiSetPattern(hps,PATSYM_SOLID); break;
				case 1 : GpiSetPattern(hps,PATSYM_DENSE1); break;
				case 2 : GpiSetPattern(hps,PATSYM_DENSE2); break;
				case 3 : GpiSetPattern(hps,PATSYM_DENSE3); break;
				case 4 : GpiSetPattern(hps,PATSYM_DENSE4); break;
				case 5 : GpiSetPattern(hps,PATSYM_DENSE5); break;
				case 6 : GpiSetPattern(hps,PATSYM_DENSE6); break;
				case 7 : GpiSetPattern(hps,PATSYM_DENSE7); break;
				case 8 : GpiSetPattern(hps,PATSYM_DENSE8); break;
				default : GpiSetPattern(hps,PATSYM_BLANK); break;
			}
			GpiSetBackMix(hps,BM_OVERPAINT);
			if (bit)
			{	switch ((int)floor(hue*10))
				{	case 0 : GpiSetPattern(bps,PATSYM_SOLID); break;
					case 1 : GpiSetPattern(bps,PATSYM_DENSE1); break;
					case 2 : GpiSetPattern(bps,PATSYM_DENSE2); break;
					case 3 : GpiSetPattern(bps,PATSYM_DENSE3); break;
					case 4 : GpiSetPattern(bps,PATSYM_DENSE4); break;
					case 5 : GpiSetPattern(bps,PATSYM_DENSE5); break;
					case 6 : GpiSetPattern(bps,PATSYM_DENSE6); break;
					case 7 : GpiSetPattern(bps,PATSYM_DENSE7); break;
					case 8 : GpiSetPattern(bps,PATSYM_DENSE8); break;
					default : GpiSetPattern(bps,PATSYM_BLANK); break;
				}
				GpiSetBackMix(bps,BM_OVERPAINT);
			}
			break;
	}
}

void resetcolor (HPS hps)
/* reset color to ordinary */
{   switch(fillmode)
	{	case fpalette :
			GpiSelectPalette(hps,NULLHANDLE);
			if (bit) GpiSelectPalette(bps,NULLHANDLE);
			break;
		case flogcolor :
			alTable[0]=oldcol;
			GpiCreateLogColorTable(hps,0,LCOLF_CONSECRGB,
				2,1,(PLONG)alTable);
			if (bit) GpiCreateLogColorTable(bps,0,LCOLF_CONSECRGB,
				2,1,(PLONG)alTable);
			break;
		case fdither :
			GpiSetBackColor(hps,CLR_DEFAULT);
			if (bit) GpiSetBackColor(bps,CLR_DEFAULT);
			GpiSetBackMix(hps,BM_DEFAULT);
			break;
	}
}

/* Conversion from 0..1024 to screen coordinates */

long xcoord (double c)
{	if (gpsprinter) return (long)(xoffset+xpixel*c/1024.0);
	else if (gpscopy) return (long)(copy_xsize*c/1024.0);
	else return (long)(xsize*c/1024.0);
}

long ycoord (double r)
{	r=1024-r;
	if (gpsprinter) return (long)(yoffset+ypixel*r/1024.0);
	else if (gpscopy) return (long)(copy_ysize*r/1024.0);
	else return (long)(ysize*r/1024.0);
}

int  coladj (int c)
{	if (c<0) return 0;
	if (c>15) return 15;
	else return c;
}

void gline (double c, double r, double c1, double r1, int col,
	int st, int width)
/***** gline
	draw a line.
	col is the color, where 0 should be white and 1 black.
	st is a style from linetyp.
	width is the linewidth, where 0 or 1 are equal defaults.
*****/
{	POINTL p;
	HPS gps;
	if (fabs(r)>10000.0) return;
	if (fabs(c)>10000.0) return;
	if (fabs(r1)>10000.0) return;
	if (fabs(c1)>10000.0) return;
	if (noting)
	{   testnote(4*sizeof(int)+4*sizeof(long));
		intwrite(1);
		longwrite(c); longwrite(r); longwrite(c1); longwrite(r1);
		intwrite(col); intwrite(st); intwrite(width);
	}
	if ((r<0 || r>1024 || c<0 || c>1024) &&
		(r1<0 || r1>1024 || c1<0 || c1>1024)) return;
	col=coladj(col);
	gps=geth();
	if (col<0) col=0;
	if (col>15) col=15;
	if (!gpsprinter || colorlines)
		GpiSetColor(gps,palette[!bw?col:(col>0)]);
	else GpiSetColor(gps,CLR_BLACK);
	switch (st)
	{   case line_none :
			GpiSetColor(gps,CLR_WHITE); break;
		case line_dashed :
			if (uselinestyle) GpiSetLineType(gps,LINETYPE_SHORTDASH);
			else GpiSetLineType(gps,LINETYPE_ALTERNATE);
			break;
		case line_dotted :
			if (uselinestyle) GpiSetLineType(gps,LINETYPE_DOT);
			else GpiSetLineType(gps,LINETYPE_ALTERNATE);
			break;
		default :
			GpiSetLineType(gps,LINETYPE_SOLID);
	}
	if (bit)
	{	GpiSetColor(bps,palette[!bw?col:(col>0)]);
		switch (st)
		{   case line_none :
				GpiSetColor(bps,CLR_WHITE);
				break;
			case line_dashed :
				if (uselinestyle) GpiSetLineType(bps,LINETYPE_SHORTDASH);
				else GpiSetLineType(bps,LINETYPE_ALTERNATE);
				break;
			case line_dotted :
				if (uselinestyle) GpiSetLineType(bps,LINETYPE_DOT);
				else GpiSetLineType(bps,LINETYPE_ALTERNATE);
				break;
			default :
				GpiSetLineType(bps,LINETYPE_SOLID);
		}
	}
	if (width<2)
	{	p.x=xcoord(c); p.y=ycoord(r); GpiMove(gps,&p);
		p.x=xcoord(c1); p.y=ycoord(r1); GpiLine(gps,&p);
	}
	else
	{	GpiBeginPath(gps,1);
		p.x=xcoord(c); p.y=ycoord(r); GpiMove(gps,&p);
		p.x=xcoord(c1); p.y=ycoord(r1); GpiLine(gps,&p);
		GpiEndPath(gps);
		GpiSetLineWidthGeom(gps,width);
		GpiStrokePath(gps,1,0);
	}
	if (bit)
	{	if (width<2)
		{	p.x=xcoord(c); p.y=ycoord(r); GpiMove(bps,&p);
			p.x=xcoord(c1); p.y=ycoord(r1); GpiLine(bps,&p);
		}
		else
		{	GpiBeginPath(bps,1);
			p.x=xcoord(c); p.y=ycoord(r); GpiMove(bps,&p);
			p.x=xcoord(c1); p.y=ycoord(r1); GpiLine(bps,&p);
			GpiEndPath(bps);
			GpiSetLineWidthGeom(bps,width);
			GpiStrokePath(bps,1,0);
		}
	}
	GpiSetLineType(gps,LINETYPE_SOLID);
	if (bit) GpiSetLineType(bps,LINETYPE_SOLID);
	relh(gps);
}

int markerfactor=100;

void gmarker (double c, double r, int col, int type)
/***** gmarker
	plot a single marker on screen.
	col is the color.
	type is a type from markertyp.
*****/
{   POINTL p;
	HPS gps;
	if (noting)
	{   testnote(3*sizeof(int)+2*sizeof(long));
		intwrite(2);
		longwrite(c); longwrite(r);
		intwrite(col); intwrite(type);
	}
	col=coladj(col);
	gps=geth();
	if (usemarkers)
	{	switch (type)
		{	case marker_cross : GpiSetMarker(gps,MARKSYM_CROSS); break;
			case marker_circle : GpiSetMarker(gps,MARKSYM_SMALLCIRCLE); break;
			case marker_diamond : GpiSetMarker(gps,MARKSYM_DIAMOND); break;
			case marker_dot : GpiSetMarker(gps,MARKSYM_DOT); break;
			case marker_square : GpiSetMarker(gps,MARKSYM_SQUARE); break;
			case marker_plus : GpiSetMarker(gps,MARKSYM_PLUS); break;
			case marker_star : GpiSetMarker(gps,MARKSYM_EIGHTPOINTSTAR); break;
		}
		if (bit)
		{	switch (type)
			{	case marker_cross : GpiSetMarker(bps,MARKSYM_CROSS); break;
				case marker_circle : GpiSetMarker(bps,MARKSYM_SMALLCIRCLE); break;
				case marker_diamond : GpiSetMarker(bps,MARKSYM_DIAMOND); break;
				case marker_dot : GpiSetMarker(bps,MARKSYM_DOT); break;
				case marker_square : GpiSetMarker(bps,MARKSYM_SQUARE); break;
				case marker_plus : GpiSetMarker(bps,MARKSYM_PLUS); break;
				case marker_star : GpiSetMarker(bps,MARKSYM_EIGHTPOINTSTAR); break;
			}
		}
		if (col<0) col=0;
		if (col>15) col=15;
		if (!gpsprinter || colormarker)
			GpiSetColor(gps,palette[!bw?col:(col>0)]);
		else GpiSetColor(gps,CLR_BLACK);
		GpiSetTextAlignment(gps,TA_NORMAL_HORIZ,TA_NORMAL_VERT);
		if (bit) GpiSetTextAlignment(bps,TA_NORMAL_HORIZ,TA_NORMAL_VERT);
		GpiMarker(gps,&p);
		if (bit)
		{	GpiSetColor(bps,palette[!bw?col:(col>0)]);
			GpiMarker(bps,&p);
		}
	}
	else
	{	p.x=xcoord(c); p.y=ycoord(r);
		if (col<0) col=0;
		if (col>15) col=15;
		if (!gpsprinter || colormarker)
			GpiSetColor(gps,palette[!bw?col:(col>0)]);
		else GpiSetColor(gps,CLR_BLACK);
		if (bit)
		{	GpiSetColor(bps,palette[!bw?col:(col>0)]);
		}
		switch (type)
		{   case marker_cross :
			case marker_star :
				p.x=xcoord(c-10); p.y=ycoord(r+10);
				GpiMove(gps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r-10);
				GpiLine(gps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r+10);
				GpiMove(gps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r-10);
				GpiLine(gps,&p);
				if (type!=marker_star) break;
			case marker_plus :
				p.x=xcoord(c-10); p.y=ycoord(r);
				GpiMove(gps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r);
				GpiLine(gps,&p);
				p.x=xcoord(c); p.y=ycoord(r-10);
				GpiMove(gps,&p);
				p.x=xcoord(c); p.y=ycoord(r+10);
				GpiLine(gps,&p);
				break;
			case marker_diamond :
				p.x=xcoord(c-10); p.y=ycoord(r);
				GpiMove(gps,&p);
				p.x=xcoord(c); p.y=ycoord(r+10);
				GpiLine(gps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r);
				GpiLine(gps,&p);
				p.x=xcoord(c); p.y=ycoord(r-10);
				GpiLine(gps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r);
				GpiLine(gps,&p);
				break;
			case marker_square :
				p.x=xcoord(c-10); p.y=ycoord(r-10);
				GpiMove(gps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r+10);
				GpiLine(gps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r+10);
				GpiLine(gps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r-10);
				GpiLine(gps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r-10);
				GpiLine(gps,&p);
				break;
			case marker_dot :
				p.x=xcoord(c); p.y=ycoord(r);
				GpiMove(gps,&p);
				GpiLine(gps,&p);
				break;
			case marker_circle :
				p.x=xcoord(c-10); p.y=ycoord(r+5);
				GpiMove(gps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r-5);
				GpiLine(gps,&p);
				p.x=xcoord(c-5); p.y=ycoord(r-10);
				GpiLine(gps,&p);
				p.x=xcoord(c+5); p.y=ycoord(r-10);
				GpiLine(gps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r-5);
				GpiLine(gps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r+5);
				GpiLine(gps,&p);
				p.x=xcoord(c+5); p.y=ycoord(r+10);
				GpiLine(gps,&p);
				p.x=xcoord(c-5); p.y=ycoord(r+10);
				GpiLine(gps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r+5);
				GpiLine(gps,&p);
				break;
		}
		if (bit) switch(type)
		{   case marker_cross :
			case marker_star :
				p.x=xcoord(c-10); p.y=ycoord(r+10);
				GpiMove(bps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r-10);
				GpiLine(bps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r+10);
				GpiMove(bps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r-10);
				GpiLine(bps,&p);
				if (type!=marker_star) break;
			case marker_plus :
				p.x=xcoord(c-10); p.y=ycoord(r);
				GpiMove(bps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r);
				GpiLine(bps,&p);
				p.x=xcoord(c); p.y=ycoord(r-10);
				GpiMove(bps,&p);
				p.x=xcoord(c); p.y=ycoord(r+10);
				GpiLine(bps,&p);
				break;
			case marker_diamond :
				p.x=xcoord(c-10); p.y=ycoord(r);
				GpiMove(bps,&p);
				p.x=xcoord(c); p.y=ycoord(r+10);
				GpiLine(bps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r);
				GpiLine(bps,&p);
				p.x=xcoord(c); p.y=ycoord(r-10);
				GpiLine(bps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r);
				GpiLine(bps,&p);
				break;
			case marker_square :
				p.x=xcoord(c-10); p.y=ycoord(r-10);
				GpiMove(bps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r+10);
				GpiLine(bps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r+10);
				GpiLine(bps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r-10);
				GpiLine(bps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r-10);
				GpiLine(bps,&p);
				break;
			case marker_dot :
				p.x=xcoord(c); p.y=ycoord(r);
				GpiMove(bps,&p);
				GpiLine(bps,&p);
				break;
			case marker_circle :
				p.x=xcoord(c-10); p.y=ycoord(r+5);
				GpiMove(bps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r-5);
				GpiLine(bps,&p);
				p.x=xcoord(c-5); p.y=ycoord(r-10);
				GpiLine(bps,&p);
				p.x=xcoord(c+5); p.y=ycoord(r-10);
				GpiLine(bps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r-5);
				GpiLine(bps,&p);
				p.x=xcoord(c+10); p.y=ycoord(r+5);
				GpiLine(bps,&p);
				p.x=xcoord(c+5); p.y=ycoord(r+10);
				GpiLine(bps,&p);
				p.x=xcoord(c-5); p.y=ycoord(r+10);
				GpiLine(bps,&p);
				p.x=xcoord(c-10); p.y=ycoord(r+5);
				GpiLine(bps,&p);
				break;
		}
	}
	relh(gps);
}

void gfill (double c[], int st, int n, int connect[])
/***** gfill
	fill an area given by n pairs of points (in c: x,y,x,y,...)
	with the style st from filltyp.
	connect pairs of points indicated in connect by a black line.
*****/
{	int i;
	HPS gps;
	POINTL p[32];
	for (i=0; i<2*n; i++) if (fabs(c[i])>10000.0) return;
	if (noting)
	{	testnote((3+n)*sizeof(int)+2*n*sizeof(long));
		intwrite(3);
		intwrite(n);
		for (i=0; i<n; i++)
		{	longwrite(c[2*i]); longwrite(c[2*i+1]);
			intwrite(connect[i]);
		}
		intwrite(st);
	}
	for (i=0; i<n; i++)
	{	p[i].x=xcoord(c[2*i]);
		p[i].y=ycoord(c[2*i+1]);
	}
	gps=geth();
	GpiBeginPath(gps,1);
	GpiMove(gps,&p[0]);
	GpiPolyLine(gps,n-1,p+1);
	GpiCloseFigure(gps);
	GpiEndPath(gps);
	if (gpsprinter && !printcolor)
	{   if (printgw && st==fill_filled)
		{	GpiSetColor(gps,CLR_BLACK);
			GpiSetPattern(gps,PATSYM_HALFTONE);
			GpiSetBackMix(gps,BM_OVERPAINT);
		}
		else
		{	GpiSetColor(gps,CLR_WHITE);
			GpiSetPattern(gps,PATSYM_SOLID);
		}
	}
	else
	{	if (st!=fill_filled)
			GpiSetColor(gps,palette[!bw?fillcolor1:0]);
		else
			GpiSetColor(gps,palette[!bw?fillcolor2:0]);
		GpiSetPattern(gps,PATSYM_SOLID);
	}
	GpiFillPath(gps,1,FPATH_ALTERNATE);
	if (bit)
	{	GpiBeginPath(bps,1);
		GpiMove(bps,&p[0]);
		GpiPolyLine(bps,n-1,p+1);
		GpiCloseFigure(bps);
		GpiEndPath(bps);
		if (st!=fill_filled)
			GpiSetColor(bps,palette[!bw?fillcolor1:0]);
		else
			GpiSetColor(bps,palette[!bw?fillcolor2:0]);
		GpiSetPattern(bps,PATSYM_SOLID);
		GpiFillPath(bps,1,FPATH_ALTERNATE);
	}
	GpiSetColor(gps,palette[!bw?dlinecolor:1]);
	GpiSetLineType(gps,LINETYPE_SOLID);
	GpiMove(gps,&p[0]);
	for (i=0; i<n-1; i++)
	{	if (connect[i]) GpiLine(gps,&p[i+1]);
		else GpiMove(gps,&p[i+1]);
	}
	if (connect[n-1]) GpiLine(gps,&p[0]);
	if (bit)
	{	GpiSetColor(bps,palette[!bw?dlinecolor:1]);
		GpiSetLineType(bps,LINETYPE_SOLID);
		GpiMove(bps,&p[0]);
		for (i=0; i<n-1; i++)
		{	if (connect[i]) GpiLine(bps,&p[i+1]);
			else GpiMove(bps,&p[i+1]);
		}
		if (connect[n-1]) GpiLine(bps,&p[0]);
	}
	GpiSetBackMix(gps,BM_DEFAULT);
	if (bit) GpiSetBackMix(bps,BM_DEFAULT);
	relh(gps);
}

void gtext (double c, double r, char *text, int color, int alignment)
/***** gtext
	output a graphic text on screen.
	alignment is left=0, centered=1, right=2.
*****/
{   HPS gps;
	POINTL p,pts[TXTBOX_COUNT];
	RECTL rect;
	SIZEF size,size1;
	if (noting)
	{   testnote(3*sizeof(int)+2*sizeof(long)+
			(strlen(text)/2+1)*2);
		intwrite(4); longwrite(c);
		longwrite(r); intwrite(color); intwrite(alignment);
		stringwrite(text);
	}
	color=coladj(color);
	gps=geth();
	if (gpsprinter) setfont(gps,&printfat);
	else if (gpsmeta) setfont(gps,&fat);
	else
	{	setfont(gps,&fat);
		if (bit) setfont(bps,&fat);
	}
	GpiSetCharSet(gps,1);
	if (bit) GpiSetCharSet(bps,1);
	if (outline)
	{   GpiQueryCharBox(gps,&size);
		size1.cx=size.cx*fontfactor;
		size1.cy=size.cy*fontfactor;
		GpiSetCharBox(gps,&size1);
		if (bit) GpiSetCharBox(bps,&size1);
	}
	GpiSetTextAlignment(gps,TA_LEFT,TA_BOTTOM);
	if (bit) GpiSetTextAlignment(bps,TA_LEFT,TA_BOTTOM);
	GpiQueryTextBox(gps,strlen(text),text,TXTBOX_COUNT,pts);
	p.x=xcoord(c); p.y=ycoord(r)-(pts[0].y-pts[1].y);
	if (alignment!=0)
	{	if (alignment==1) p.x-=(pts[3].x-pts[1].x)/2;
		else p.x-=pts[3].x-pts[1].x;
	}
	if (color<0) color=0;
	if (color>15) color=15;
	if (!gpsprinter || colortext)
		GpiSetColor(gps,palette[!bw?color:(color>0)]);
	else GpiSetColor(gps,CLR_BLACK);
	rect.xLeft=xcoord(0); rect.yBottom=ycoord(1024);
	rect.xRight=xcoord(1024); rect.yTop=ycoord(0);
	GpiCharStringPosAt(gps,&p,&rect,CHS_CLIP,strlen(text),text,0);
	if (bit)
	{	GpiSetColor(bps,palette[!bw?color:(color>0)]);
		GpiCharStringPosAt(bps,&p,&rect,CHS_CLIP,strlen(text),text,0);
	}
	if (outline)
	{	GpiSetCharBox(gps,&size);
		if (bit) GpiSetCharBox(bps,&size);
	}
	relh(gps);
}

void scale (double s)
/***** scale
	scale the screen according s = true width / true height.
	This is not necessary on a window based system.
*****/
{   if (noting)
	{   testnote(sizeof(int)+sizeof(long));
		intwrite(5);
		longwrite(s);
	}
}

void gvtext (double c, double r, char *text, int color, int alignment)
/***** gvtext
	output a graphic text on screen vertically downwards.
	alignment is left=0, centered=1, right=2.
*****/
{   HPS gps;
	POINTL p,pts[TXTBOX_COUNT];
	GRADIENTL pg;
	RECTL rect;
	SIZEF size,size1;
	if (noting)
	{   testnote(3*sizeof(int)+2*sizeof(long)+
			(strlen(text)/2+1)*2);
		intwrite(9); longwrite(c);
		longwrite(r); intwrite(color); intwrite(alignment);
		stringwrite(text);
	}
	color=coladj(color);
	gps=geth();
	if (gpsprinter) setfont(gps,&printfat);
	else if (gpsmeta) setfont(gps,&fat);
	else
	{	setfont(gps,&fat);
		if (bit) setfont(bps,&fat);
	}
	GpiSetCharSet(gps,1);
	if (bit) GpiSetCharSet(bps,1);
	if (outline)
	{   GpiQueryCharBox(gps,&size);
		size1.cx=size.cx*fontfactor;
		size1.cy=size.cy*fontfactor;
		GpiSetCharBox(gps,&size1);
		if (bit) GpiSetCharBox(bps,&size1);
	}
	GpiSetCharMode(gps,CM_MODE2);
	if (bit) GpiSetCharMode(bps,CM_MODE2);
	pg.x=0; pg.y=-100;
	GpiSetCharAngle(gps,&pg);
	if (bit) GpiSetCharAngle(bps,&pg);
	GpiSetTextAlignment(gps,TA_NORMAL_HORIZ,TA_NORMAL_VERT);
	if (bit) GpiSetTextAlignment(bps,TA_NORMAL_HORIZ,TA_NORMAL_VERT);
	p.x=xcoord(c); p.y=ycoord(r);
	GpiQueryTextBox(gps,strlen(text),text,TXTBOX_COUNT,pts);
	if (alignment==1) p.y-=(pts[3].y-pts[1].y)/2;
	else if (alignment==2) p.y-=pts[3].y-pts[1].y;
	if (color<0) color=0;
	if (color>15) color=15;
	if (!gpsprinter || colortext)
		GpiSetColor(gps,palette[!bw?color:(color>0)]);
	else GpiSetColor(gps,CLR_BLACK);
	rect.xLeft=xcoord(0); rect.yBottom=ycoord(1024);
	rect.xRight=xcoord(1024); rect.yTop=ycoord(0);
	GpiCharStringPosAt(gps,&p,&rect,CHS_CLIP,strlen(text),text,0);
	if (bit)
	{	GpiSetColor(bps,palette[!bw?color:(color>0)]);
		GpiCharStringPosAt(bps,&p,&rect,CHS_CLIP,strlen(text),text,0);
	}
	if (outline)
	{	GpiSetCharBox(gps,&size);
		if (bit) GpiSetCharBox(bps,&size);
	}
	pg.x=0; pg.y=0;
	GpiSetCharAngle(gps,&pg);
	if (bit) GpiSetCharAngle(bps,&pg);
	GpiSetCharMode(gps,CM_DEFAULT);
	if (bit) GpiSetCharMode(bps,CM_DEFAULT);
	relh(gps);
}

void gvutext (double c, double r, char *text, int color, int alignment)
/***** gvtext
	output a graphic text on screen vertically upwards.
	alignment is left=0, centered=1, right=2.
*****/
{   HPS gps;
	POINTL p,pts[TXTBOX_COUNT];
	GRADIENTL pg;
	RECTL rect;
	SIZEF size,size1;
	if (noting)
	{   testnote(3*sizeof(int)+2*sizeof(long)+
			(strlen(text)/2+1)*2);
		intwrite(10); longwrite(c);
		longwrite(r); intwrite(color); intwrite(alignment);
		stringwrite(text);
	}
	color=coladj(color);
	gps=geth();
	if (gpsprinter) setfont(gps,&printfat);
	else if (gpsmeta) setfont(gps,&fat);
	else
	{	setfont(gps,&fat);
		if (bit) setfont(bps,&fat);
	}
	GpiSetCharSet(gps,1);
	if (bit) GpiSetCharSet(bps,1);
	if (outline)
	{   GpiQueryCharBox(gps,&size);
		size1.cx=size.cx*fontfactor;
		size1.cy=size.cy*fontfactor;
		GpiSetCharBox(gps,&size1);
		if (bit) GpiSetCharBox(bps,&size1);
	}
	GpiSetCharMode(gps,CM_MODE2);
	if (bit) GpiSetCharMode(bps,CM_MODE2);
	pg.x=0; pg.y=100;
	GpiSetCharAngle(gps,&pg);
	if (bit) GpiSetCharAngle(bps,&pg);
	GpiSetTextAlignment(gps,TA_NORMAL_HORIZ,TA_NORMAL_VERT);
	if (bit) GpiSetTextAlignment(bps,TA_NORMAL_HORIZ,TA_NORMAL_VERT);
	p.x=xcoord(c); p.y=ycoord(r);
	GpiQueryTextBox(gps,strlen(text),text,TXTBOX_COUNT,pts);
	if (alignment==1) p.y+=(pts[1].y-pts[3].y)/2;
	else if (alignment==2) p.y+=pts[1].y-pts[3].y;
	if (color<0) color=0;
	if (color>15) color=15;
	if (!gpsprinter || colortext)
		GpiSetColor(gps,palette[!bw?color:(color>0)]);
	else GpiSetColor(gps,CLR_BLACK);
	rect.xLeft=xcoord(0); rect.yBottom=ycoord(1024);
	rect.xRight=xcoord(1024); rect.yTop=ycoord(0);
	GpiCharStringPosAt(gps,&p,&rect,CHS_CLIP,strlen(text),text,0);
	if (bit)
	{	GpiSetColor(bps,palette[!bw?color:(color>0)]);
		GpiCharStringPosAt(bps,&p,&rect,CHS_CLIP,strlen(text),text,0);
	}
	if (outline)
	{	GpiSetCharBox(gps,&size);
		if (bit) GpiSetCharBox(bps,&size);
	}
	pg.x=0; pg.y=0;
	GpiSetCharAngle(gps,&pg);
	if (bit) GpiSetCharAngle(bps,&pg);
	GpiSetCharMode(gps,CM_DEFAULT);
	if (bit) GpiSetCharMode(bps,CM_DEFAULT);
	relh(gps);
}

void gbar (double c, double r, double c1, double r1, double hue,
	int color, int connect)
/***** Draw a rectangle.
	hue is a hue intensity from 0 to 1.
	connect determines, if a black boundary should be drawn.
******/
{   HPS gps;
	POINTL p;
	if (noting)
	{	testnote(3*sizeof(int)+4*sizeof(long));
		intwrite(7);
		longwrite(c); longwrite(r);
		longwrite(c1); longwrite(r1);
		longwrite(hue); intwrite(color);
		intwrite(connect);
	}
	color=coladj(color);
	gps=geth();
	p.x=xcoord(c); p.y=ycoord(r);
	GpiMove(gps,&p);
	if (bit) GpiMove(bps,&p);
	p.x=xcoord(c1); p.y=ycoord(r1);
	hue=hue-floor(hue);
	setcolor(gps,hue,color);
	GpiBox(gps,DRO_FILL,&p,0,0);
	if (bit) GpiBox(bps,DRO_FILL,&p,0,0);
	resetcolor(gps);
	p.x=xcoord(c); p.y=ycoord(r);
	GpiMove(gps,&p);
	p.x=xcoord(c1); p.y=ycoord(r1);
	if (connect)
	{	GpiSetColor(gps,CLR_BLACK);
		GpiBox(gps,DRO_OUTLINE,&p,0,0);
	}
	if (bit)
	{	p.x=xcoord(c); p.y=ycoord(r);
		GpiMove(bps,&p);
		if (bit) GpiMove(bps,&p);
		p.x=xcoord(c1); p.y=ycoord(r1);
		if (connect)
		{	GpiSetColor(bps,CLR_BLACK);
			GpiBox(bps,DRO_OUTLINE,&p,0,0);
		}
	}
	relh(gps);
}

void gbar1 (double c, double r, double c1, double r1,
	int color, int connect)
/***** Draw a rectangle.
	hue is a hue intensity from 0 to 1.
	connect determines, if a black boundary should be drawn.
******/
{   HPS gps;
	POINTL p;
	if (noting)
	{	testnote(3*sizeof(int)+4*sizeof(long));
		intwrite(11);
		longwrite(c); longwrite(r);
		longwrite(c1); longwrite(r1);
		intwrite(color);
		intwrite(connect);
	}
	color=coladj(color);
	gps=geth();
	p.x=xcoord(c); p.y=ycoord(r);
	GpiMove(gps,&p);
	if (bit) GpiMove(bps,&p);
	p.x=xcoord(c1); p.y=ycoord(r1);
	setcolor(gps,1.0,color);
	GpiBox(gps,DRO_FILL,&p,0,0);
	if (bit) GpiBox(bps,DRO_FILL,&p,0,0);
	resetcolor(gps);
	p.x=xcoord(c); p.y=ycoord(r);
	GpiMove(gps,&p);
	p.x=xcoord(c1); p.y=ycoord(r1);
	if (connect)
	{	GpiSetColor(gps,CLR_BLACK);
		GpiBox(gps,DRO_OUTLINE,&p,0,0);
	}
	if (bit)
	{	p.x=xcoord(c); p.y=ycoord(r);
		GpiMove(bps,&p);
		if (bit) GpiMove(bps,&p);
		p.x=xcoord(c1); p.y=ycoord(r1);
		if (connect)
		{	GpiSetColor(bps,CLR_BLACK);
			GpiBox(bps,DRO_OUTLINE,&p,0,0);
		}
	}
	relh(gps);
}

void gfillh (double c[8], double hue, int color, int connect)
/***** Draw a filled polygon.
	Works like gfill, but uses hue.
*****/
{	int i;
	HPS gps;
	POINTL p[32];
	for (i=0; i<8; i++) if (fabs(c[i])>10000.0) return;
	if (noting)
	{   testnote(3*sizeof(int)+9*sizeof(long));
		intwrite(8);
		for (i=0; i<8; i++) longwrite(c[i]);
		longwrite(hue);
		intwrite(color); intwrite(connect);
	}
	for (i=0; i<4; i++)
	{	p[i].x=xcoord(c[2*i]);
		p[i].y=ycoord(c[2*i+1]);
	}
	color=coladj(color);
	gps=geth();
	GpiBeginPath(gps,1);
	GpiMove(gps,&p[0]);
	GpiPolyLine(gps,3,p+1);
	GpiCloseFigure(gps);
	GpiEndPath(gps);
	GpiSetPattern(gps,PATSYM_SOLID);
	hue=hue-floor(hue);
	if (bit)
	{	GpiBeginPath(bps,1);
		GpiMove(bps,&p[0]);
		GpiPolyLine(bps,3,p+1);
		GpiCloseFigure(bps);
		GpiEndPath(bps);
		GpiSetPattern(gps,PATSYM_SOLID);
	}
	setcolor(gps,hue,color);
	GpiFillPath(gps,1,FPATH_ALTERNATE);
	if (bit) GpiFillPath(bps,1,FPATH_ALTERNATE);
	resetcolor(gps);
	GpiSetLineType(gps,LINETYPE_SOLID);
	if (connect)
	{	GpiSetColor(gps,palette[!bw?dlinecolor:1]);
		GpiMove(gps,&p[0]);
		for (i=0; i<3; i++) GpiLine(gps,&p[i+1]);
		GpiLine(gps,&p[0]);
	}
	if (bit)
	{	GpiSetLineType(bps,LINETYPE_SOLID);
		if (connect)
		{	GpiSetColor(bps,palette[!bw?dlinecolor:1]);
			GpiMove(bps,&p[0]);
			for (i=0; i<3; i++) GpiLine(bps,&p[i+1]);
			GpiLine(bps,&p[0]);
		}
	}
	relh(gps);
}

/*************** Replay Graphics ********************/

#define nextint(p) (*((int *)p)++)
#define nextlong(p) (*((long *)p)++/1000.0)

void replay (void)
/* Redraw the graphics as noted in notespace */
{   double r,c,r1,c1,cc[16],hue;
	int col,st,width,co[16],i,n,pn;
	size_t size;
	char *p=notepages[0],msg[256];
	int actpage=0;
	noting=0;
	bit=drawbitmap;
	if (p>=note)
	{	clear_graphics();
		goto end;
	}
	while (actpage<usedpages-1 || (actpage==usedpages-1 && p<note))
	{	switch(pn=nextint(p))
		{	case 1 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				width=nextint(p);
				gline(c,r,c1,r1,col,st,width);
				break;
			case 2 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gmarker(c,r,col,st);
				break;
			case 3 :
				n=nextint(p);
				for (i=0; i<n; i++)
				{	cc[2*i]=nextlong(p);
					cc[2*i+1]=nextlong(p);
					co[i]=nextint(p);
				}
				st=nextint(p);
				gfill(cc,st,n,co);
				break;
			case 4 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gtext(c,r,p,col,st);
				size=strlen(p);
				size=((size+2)/2)*2;
				p+=size;
				break;
			case 5 :
				c=nextlong(p);
				break;
			case 6 :
				clear_graphics();
				break;
			case 7 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				hue=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gbar(c,r,c1,r1,hue,col,st);
				break;
			case 8 :
				for (i=0; i<8; i++) cc[i]=nextlong(p);
				hue=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gfillh(cc,hue,col,st);
				break;
			case -1 :
				actpage++;
				p=notepages[actpage];
				break;
			case 9 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gvtext(c,r,p,col,st);
				size=strlen(p);
				size=((size+2)/2)*2;
				p+=size;
				break;
			case 10 :
				c=nextlong(p);
				r=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gvutext(c,r,p,col,st);
				size=strlen(p);
				size=((size+2)/2)*2;
				p+=size;
				break;
			case 11 :
				c=nextlong(p);
				r=nextlong(p);
				c1=nextlong(p);
				r1=nextlong(p);
				col=nextint(p);
				st=nextint(p);
				gbar1(c,r,c1,r1,col,st);
				break;
			default :
				sprintf(msg,"Internal Error in Metafile\n(%d)",pn);
				message(msg);
				return;
		}
	}
	end: noting=1;
	bit=bitmapvalid;
}

/*********************** input routines **********************/

extern int udf;

void keyboard (MPARAM mp1, MPARAM mp2)
/* proceed a keypress */
{   int flags=SHORT1FROMMP(mp1);
	keyscan=0;
	keycode=0;
	if (flags & KC_KEYUP) return;
	if (flags & KC_CHAR)
	{	keycode=SHORT1FROMMP(mp2);
		if (keycode<32 && keycode>=0) goto vk;
		keydone=1;
		WinPostMsg(DummyHandle,WM_CHAR,0,0);
		return;
	}
	vk:
	if (flags & KC_VIRTUALKEY)
	{   switch(SHORT2FROMMP(mp2))
		{	case VK_ENTER :
			case VK_NEWLINE :
				keyscan=enter; break;
			case VK_BACKSPACE : keyscan=backspace; break;
			case VK_BREAK :
			case VK_ESC :
				if (udf) menu_escape();
				else if (!editing) keyscan=escape;
				break;
			case VK_F1 : keyscan=fk1; break;
			case VK_F2 : keyscan=fk2; break;
			case VK_F3 : keyscan=fk3; break;
			case VK_F4 : keyscan=fk4; break;
			case VK_F5 : keyscan=fk5; break;
			case VK_F6 : keyscan=fk6; break;
			case VK_F7 : keyscan=fk7; break;
			case VK_F8 : keyscan=fk8; break;
			case VK_F9 : keyscan=fk9; break;
			case VK_F10 : keyscan=fk10; break;
			case VK_TAB : keyscan=switch_screen; break;
			case VK_LEFT :
				if (flags&KC_SHIFT) keyscan=word_left;
				else keyscan=cursor_left;
				break;
			case VK_RIGHT :
				if (flags&KC_SHIFT) keyscan=word_right;
				else keyscan=cursor_right;
				break;
			case VK_UP :
				if (flags&KC_CTRL) keyscan=clear_home;
				else if (flags&KC_SHIFT) keyscan=cursor_up;
				else if (editing && !nojump) prev_command();
				else keyscan=cursor_up;
				break;
			case VK_DOWN :
				if (flags&KC_CTRL) keyscan=clear_home;
				else if (flags&KC_SHIFT) keyscan=cursor_down;
				else if (editing && !nojump) next_command();
				else keyscan=cursor_down;
				break;
			case VK_END : keyscan=line_end; break;
			case VK_HOME : keyscan=line_start; break;
			case VK_SPACE :
				if (flags & KC_CHAR) keycode=SHORT1FROMMP(mp2);
				else keycode=' ';
				break;
			case VK_INSERT : keyscan=help; break;
			case VK_DELETE :
				if (flags & KC_CTRL) keyscan=clear_home;
				else keyscan=deletekey;
				break;
			case VK_PAGEUP : page_up_cursor(); return;
			case VK_PAGEDOWN : page_down_cursor(); return;
			default : return;
		}
		keydone=1;
		WinPostMsg(DummyHandle,WM_CHAR,0,0);
		return;
	}
}

void mouse (double *x, double *y)
/****** mouse
	wait, until the user marked a screen point with the mouse.
	Return screen coordinates.
******/
{   int scan=0;
	while (scan!=-1)
	{	wait_key(&scan);
	}
	*x=mousex; *y=mousey;
}

void update_cursor (void);

int wait_key (int *scan)
/*****
	wait for a keystroke. return the scancode and the ascii code.
	scancode should be a code from scantyp. Do at least generate
	'enter'.
*****/
{   QMSG qmsg;
	keydone=0;
	if (editing) update_cursor();
	while (!keydone)
	{	if (pasting)
		{	if (!*pclip) pasting=0;
			else
			{	if (*pclip==10)
				{	*scan=enter; pclip++; return '\n';
				}
				else if (*pclip==27)
				{	*scan=escape; pclip++; return 27;
				}
				else
				{	pclip++; return *(pclip-1);
				}
			}
		}
		waiting=1;
		WinGetMsg(hab,&qmsg,DummyHandle,0,0);
		WinDispatchMsg(hab,&qmsg);
		waiting=0;
	}
	*scan=keyscan;
	return keycode;
}

int test_key (void)
/***** test_key
	see, if user pressed the keyboard.
	return the scancode, if he did.
*****/
{   QMSG qmsg;
	static int k=0;
	if (k>=10)
	{	if (WinPeekMsg(hab,&qmsg,DummyHandle,0,0,PM_REMOVE))
		{   WinDispatchMsg(hab,&qmsg);
		}
		k=0;
	}
	else k++;
	if (keydone)
	{   keydone=0;
		return keyscan;
	}
	else return 0;
}

int test_code (void)
/***** test_cod
	see, if user pressed the keyboard.
	return the scancode, if he did.
*****/
{   QMSG qmsg;
	static int k=0;
	if (k>=10)
	{	if (WinPeekMsg(hab,&qmsg,DummyHandle,0,0,PM_REMOVE))
		{   WinDispatchMsg(hab,&qmsg);
		}
		k=0;
	}
	else k++;
	if (keydone)
	{   keydone=0;
		return keyscan;
	}
	else return 0;
}

/**************** directory *******************/

char dirpath[256];

char *cd (char *dir)
/***** sets the path if dir!=0 and returns the path
*****/
{   ULONG d,l;
	if (dir[0])
	{	if (dir[1]==':')
		{	DosSetDefaultDisk(toupper(dir[0])-'A'+1);
			dir+=2;
		}
		l=strlen(dir);
		if (l>1 && dir[l-1]=='\\') dir[l-1]=0;
		DosSetCurrentDir(dir);
	}
	DosQueryCurrentDisk(&d,&l);
	dirpath[0]=d+'A'-1; dirpath[1]=':'; dirpath[2]='\\';
	l=250; DosQueryCurrentDir(0,dirpath+3,&l);
	dirpath[3+l]=0;
	return dirpath;
}

char *dir (char *pattern)
/***** Search a pattern if not 0, else research
******/
{   static int snext=0;
	char *pat;
	static HDIR hdir=HDIR_SYSTEM;
	static FILEFINDBUF3 fbuf;
	ULONG count=1;
	cont:
	if (!snext)
	{	if (!pattern) return "";
		if (!*pattern) pat="*.*";
		else pat=pattern;
		DosFindFirst(pat,&hdir,
			FILE_DIRECTORY|FILE_ARCHIVED,
			&fbuf,sizeof(FILEFINDBUF3),
			&count,FIL_STANDARD);
	}
	else
	{	DosFindNext(hdir,&fbuf,sizeof(FILEFINDBUF3),&count);
	}
	if (count==1 && fbuf.achName[0]=='.')
	{	snext=1;
		goto cont;
	}
	if (count==1)
	{   if (fbuf.attrFile & FILE_DIRECTORY)
		{	strcat(fbuf.achName," (DIR)");
		}
		snext=1;
		return fbuf.achName;
	}
	else
	{   snext=0;
		return "";
	}
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
{	WinStartTimer(hab,DummyHandle,1,time*1000);
	wait_key(scan);
}

/***************** div. ***********************************/

void getpixelsize (double *x, double *y)
/***** Compute the size of pixel in screen coordinates.
******/
{	*x=xsize/1024.0;
	*y=ysize/1024.0;
}

void gflush (void)
/***** Flush out remaining graphic commands (for multitasking).
This serves to synchronize the graphics on multitasking systems.
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


char * startofline (char *p)
{	while (p>textarea && *(p-1)) p--;
	return p;
}

char * nextline (char *p)
{	return p+strlen(p)+1;
}

char * prevline (char *p)
{   if (p==textarea) return p;
	p--; return startofline(p);
}

int iscommand (char *p)
{   return *p=='>' || *p=='$';
}

int istext (char *p)
{   return *p=='%';
}

char * nextcommand (char *p)
{	while (p<textend)
	{	p=nextline(p);
		if (iscommand(p)) return p;
	}
	return 0;
}

char * nexttruecommand (char *p)
{	while (p<textend)
	{	p=nextline(p);
		if (*p=='>') return p;
	}
	return 0;
}

char * nexttext (char *p)
{	while (p<textend)
	{	p=nextline(p);
		if (iscommand(p) || istext(p)) return p;
	}
	return 0;
}

char * prevtext (char *p)
{   char *q;
	p=startofline(p);
	q=prevline(p);
	if (!istext(q)) return 0;
	while (1)
	{	if (q==p || !istext(q)) break;
		p=q; q=prevline(p);
	}
	return p;
}

char * prevcommand (char *p)
{	p=startofline(p);
	while (p>textarea)
	{	p=prevline(p);
		if (iscommand(p)) return p;
	}
	return 0;
}

void clear_screen (void)
/***** Clear the text screen
Set textpos to the start of the textarea textarea.
Remove all text.
Redraw the screen.
******/
{	oldtopline=topline=textpos=textend=textarea;
	memset(textarea,0,TEXTSIZE);
	update_text();
}

long xcursor=0,ycursor,yout;

void update_ycursor (void)
/* Determine the y position of the cursor
using textpos, topline and textlines.
Set yout=1, if the cursor line is out of screen.
*/
{	char *p=topline;
	int i=0;
	if (p!=textarea && *(p-1)) p=startofline(p);
	if (p>textpos)
	{	yout=1; return;
	}
	while (1)
	{   p+=strlen(p)+1;
		if (p>textpos)
		{	ycursor=ytext-(i+1)*htextchar+htextbelow;
			yout=0; return;
		}
		i++;
		if (i>=textlines)
		{	yout=1; return;
		}
	}
}

void update_cursor (void)
/* See if the cursor is to the right or to the left of the screen.
Shift the screen, if necessary (changing offset).
*/
{   long x=xcursor;
	if (x<0)
	{	offset-=(xtext/2-x)/wtextchar;
		if (offset<0) offset=0;
		set_hslider();
		update_text();
	}
	else if (x>xtext-wchar)
	{	offset+=(x-xtext/2)/wtextchar;
		set_hslider();
		update_text();
	}
}

void showcursor (void)
/* create a cursor and show it according to
textpos.
texpos is the position of the cursor in the text area.
screen.
*/
{   HPS hps;
	POINTL pts[TXTBOX_COUNT];
	char *l;
	if (cursorvisible) return;
	if (!cursoron || nofocus || yout)
	{   cursorvisible=0;
		return;
	}
	l=textpos;
	while (l>textarea && *(l-1)) l--; /* get start of line */
	hps=WinGetPS(TextHandle);
	setfont(hps,&textfat);
	GpiSetCharSet(hps,1);
	GpiQueryTextBox(hps,textpos-l,l,TXTBOX_COUNT,pts);
	WinReleasePS(hps);
	WinCreateCursor(TextHandle,
		xcursor=pts[3].x-offset*wtextchar,ycursor,0,htextabove,
		CURSOR_SOLID,(PRECTL)NULL);
	WinShowCursor(TextHandle,TRUE);
	cursorvisible=1;
}

void hidecursor (void)
/* hide the text cursor */
{   if (!cursorvisible) return;
	WinShowCursor(TextHandle,FALSE);
	WinDestroyCursor(TextHandle);
	cursorvisible=0;
}

void cursor_on (void)
/* switch cursor on */
{   cursoron=1;
	showcursor();
}

void cursor_off (void)
/* switch cursor off */
{	hidecursor();
	cursoron=0;
}

void update_topline (void)
/* completely redraw first line, selected text in inverse */
{	HPS tps;
	char *p;
	size_t size;
	POINTL ptl;
	tps=WinGetPS(TextHandle);
	setfont(tps,&textfat);
	GpiSetCharSet(tps,1);
	GpiSetBackMix(tps,BM_OVERPAINT);
	p=topline;
	if (p!=textarea && *(p-1))
	{	p=topline=startofline(p);
	}
	ptl.x=0; ptl.y=ytext-htextchar;
	GpiMove(tps,&ptl);
	ptl.x=xtext-1; ptl.y+=htextchar-1;
	GpiSetPattern(tps,PATSYM_BLANK);
	GpiBox(tps,DRO_FILL,&ptl,0,0);
	ptl.x=-offset*wtextchar;
	ptl.y=ytext-htextchar+htextbelow;
	size=strlen(p);
	if (!selected || p+size+1<=startsel || p>endsel)
	{   if (*p=='%' && !selected)
		{   GpiSetColor(tps,CLR_DARKGREEN);
			GpiCharStringAt(tps,&ptl,size,p);
			GpiSetColor(tps,CLR_DEFAULT);
		}
		else if ((*p=='>' || *p=='$') && !selected)
		{   GpiSetColor(tps,CLR_DARKRED);
			GpiCharStringAt(tps,&ptl,size,p);
			GpiSetColor(tps,CLR_DEFAULT);
		}
		else
			GpiCharStringAt(tps,&ptl,size,p);
	}
	else
	{   GpiMove(tps,&ptl);
		if (p<=startsel)
		{	if (p<startsel)
			{	GpiCharString(tps,startsel-p,p);
				size-=(startsel-p);
				p+=startsel-p;
			}
		}
		GpiSetColor(tps,CLR_BACKGROUND);
		GpiSetBackColor(tps,CLR_BLACK);
		if (p+size>endsel)
		{	GpiCharString(tps,endsel-p,p);
			p=endsel;
		}
		else
		{	GpiCharString(tps,size,p);
			p+=size;
		}
		if (p>=endsel)
		{	GpiSetColor(tps,CLR_DEFAULT);
			GpiSetBackColor(tps,CLR_BACKGROUND);
		}
		size=strlen(p);
		if (*p) GpiCharString(tps,size,p);
	}
	WinReleasePS(tps);
}

void update_buttomline (void)
/* completely last line, selected text in inverse */
{	HPS tps;
	char *p;
	int i;
	size_t size;
	POINTL ptl;
	tps=WinGetPS(TextHandle);
	setfont(tps,&textfat);
	GpiSetCharSet(tps,1);
	GpiSetBackMix(tps,BM_OVERPAINT);
	p=topline;
	if (p!=textarea && *(p-1))
	{	p=topline=startofline(p);
	}
	for (i=0; i<textlines-1; i++)
	{   if (p>textend) break;
		p+=strlen(p)+1;
	}
	ptl.x=0; ptl.y=0;
	GpiMove(tps,&ptl);
	ptl.x=xtext-1; ptl.y=ytext-i*htextchar;
	GpiSetPattern(tps,PATSYM_BLANK);
	GpiBox(tps,DRO_FILL,&ptl,0,0);
	ptl.x=-offset*wtextchar;
	ptl.y=ytext-(i+1)*htextchar+htextbelow;
	size=strlen(p);
	if (!selected || p+size+1<=startsel || p>endsel)
	{   if (*p=='%' && !selected)
		{   GpiSetColor(tps,CLR_DARKGREEN);
			GpiCharStringAt(tps,&ptl,size,p);
			GpiSetColor(tps,CLR_DEFAULT);
		}
		else if ((*p=='>' || *p=='$') && !selected)
		{   GpiSetColor(tps,CLR_DARKRED);
			GpiCharStringAt(tps,&ptl,size,p);
			GpiSetColor(tps,CLR_DEFAULT);
		}
		else
			GpiCharStringAt(tps,&ptl,size,p);
	}
	else
	{   GpiMove(tps,&ptl);
		if (p<=startsel)
		{	if (p<startsel)
			{	GpiCharString(tps,startsel-p,p);
				size-=(startsel-p);
				p+=startsel-p;
			}
		}
		GpiSetColor(tps,CLR_BACKGROUND);
		GpiSetBackColor(tps,CLR_BLACK);
		if (p+size>endsel)
		{	GpiCharString(tps,endsel-p,p);
			p=endsel;
		}
		else
		{	GpiCharString(tps,size,p);
			p+=size;
		}
		if (p>=endsel)
		{	GpiSetColor(tps,CLR_DEFAULT);
			GpiSetBackColor(tps,CLR_BACKGROUND);
		}
		size=strlen(p);
		if (*p) GpiCharString(tps,size,p);
	}
	WinReleasePS(tps);
}

void update_text (void)
/* completely redraw the text screen, selected text in inverse */
{	HPS tps;
	char *p;
	int i,k;
	size_t size;
	POINTL ptl;
	hidecursor();
	tps=WinGetPS(TextHandle);
	setfont(tps,&textfat);
	GpiSetCharSet(tps,1);
	GpiErase(tps);
	GpiSetBackMix(tps,BM_OVERPAINT);
	p=topline;
	if (p!=textarea && *(p-1))
	{	p=topline=startofline(p);
	}
	k=textlines;
	GpiSetBackColor(tps,CLR_BACKGROUND);
	GpiSetColor(tps,CLR_DEFAULT);
	ptl.x=-offset*wtextchar; ptl.y=ytext-htextchar+htextbelow;
	for (i=0; i<k; i++)
	{   if (p>textend) { break; }
		size=strlen(p);
		if (!selected || p+size+1<=startsel || p>=endsel)
		{   if (*p=='%' && !selected)
			{   GpiSetColor(tps,CLR_DARKGREEN);
				GpiCharStringAt(tps,&ptl,size,p);
				GpiSetColor(tps,CLR_DEFAULT);
			}
			else if ((*p=='>' || *p=='$') && !selected)
			{   GpiSetColor(tps,CLR_DARKRED);
				GpiCharStringAt(tps,&ptl,size,p);
				GpiSetColor(tps,CLR_DEFAULT);
			}
			else
				GpiCharStringAt(tps,&ptl,size,p);
		}
		else
		{   GpiMove(tps,&ptl);
			if (p<=startsel || i==0)
			{	if (p<startsel)
				{	GpiCharString(tps,startsel-p,p);
					size-=(startsel-p);
					p+=startsel-p;
				}
				GpiSetColor(tps,CLR_BACKGROUND);
				GpiSetBackColor(tps,CLR_BLACK);
			}
			if (p+size>endsel)
			{	GpiCharString(tps,endsel-p,p);
				p=endsel;
			}
			else
			{	GpiCharString(tps,size,p);
				p+=size;
			}
			if (p>=endsel)
			{	GpiSetColor(tps,CLR_DEFAULT);
				GpiSetBackColor(tps,CLR_BACKGROUND);
			}
			size=strlen(p);
			if (*p) GpiCharString(tps,size,p);
		}
		p+=size+1;
		if (p>=endsel)
		{	GpiSetColor(tps,CLR_DEFAULT);
			GpiSetBackColor(tps,CLR_BACKGROUND);
		}
		ptl.y-=htextchar;
	}
	WinReleasePS(tps);
	update_ycursor();
	showcursor();
	WinValidateRect(TextHandle,NULL,0);
	set_slider();
}

void move_cl (void)
/* move the text cursor left */
{	hidecursor(); textpos--; showcursor();
	if (editing) update_cursor();
}

void move_cr (void)
/* move the text cursor right */
{	hidecursor(); textpos++; showcursor();
	if (editing) update_cursor();
}

void truncate_text (void);

void insert_char (int n)
/* Insert n character at the text position textpos
only in memory.
*/
{   if (textpos<textend+1)
	{	memmove(textpos+n,textpos,textend+1-textpos);
	}
	if (topline>textpos) topline+=n;
	if (oldtopline>textpos) topline+=n;
	textend+=n;
	truncate_text();
}

void delete_char (int n)
/* Delete n character at the text position textpos
only in memory.
*/
{	if (textend+1-textpos>n)
	{	memmove(textpos,textpos+n,textend+3-textpos-n);
		textend-=n;
		if (topline>textpos) topline-=n;
		if (oldtopline>textpos) oldtopline-=n;
	}
	else
	{	textend=textpos;
		if (topline>textpos) topline=startofline(textpos);
		if (oldtopline>textpos) oldtopline=topline;
	}
}

void clear_line (void)
/* clear the text line from cursor position
only on the screen.
*/
{   HPS hps;
	POINTL p,ptl[TXTBOX_COUNT];
	char *l;
	hps=WinGetPS(TextHandle);
	setfont(hps,&textfat);
	GpiSetCharSet(hps,1);
	l=textpos;
	while (l>textarea && *(l-1)) l--;
	GpiQueryTextBox(hps,textpos-l,l,TXTBOX_COUNT,ptl);
	p.x=ptl[3].x-offset*wtextchar; p.y=ycursor-htextbelow;
	GpiMove(hps,&p);
	p.x=xtext; p.y=ycursor+htextabove-1;
	GpiSetPattern(hps,PATSYM_SOLID);
	GpiSetBackMix(hps,BM_OVERPAINT);
	GpiSetColor(hps,CLR_BACKGROUND);
	GpiBox(hps,DRO_OUTLINEFILL,&p,0,0);
	WinReleasePS(hps);
}

void clear_eol (void)
/* clear the text line from cursor position.
on screen and in the text area.
*/
{   clear_line();
	delete_char(strlen(textpos));
}

void truncate_text (void)
/* See, if there is enough space in the text area */
{   char *newstart;
	size_t size;
	if (textend<textareaend-128) return;
	newstart=textarea+TEXTSIZE/3;
	while (*newstart) newstart++;
	newstart++;
	while (*newstart!='>' && newstart<textend)
	{	while (*newstart) newstart++;
		newstart++;
	}
	if (newstart>textend)
	{	clear_screen();
	}
	else
	{	size=textend-newstart;
		memmove(textarea,newstart,size);
		textpos-=(newstart-textarea);
		textend-=(newstart-textarea);
		topline-=(newstart-textarea);
		if (topline<textarea) topline=textarea;
		oldtopline-=(newstart-textarea);
		if (oldtopline<textarea) oldtopline=textarea;
		if (textpos<textarea)
		{	textpos=textarea+1;
		}
		update_text();
	}
}

void new_line (void)
/* Add a new line to the text area */
{   RECTL p;
	textpos+=strlen(textpos); /* goto 0 at line end */
	insert_char(1); /* insert one character */
	*textpos++=0; /* set it to 0 and advance to next line */
	ycursor-=htextchar;
	if ((int)ycursor<(int)htextbelow)
	{   ycursor+=htextchar;
		topline+=strlen(topline)+1;
		if (WinScrollWindow(TextHandle,0,htextchar,
			NULL,NULL,NULLHANDLE,NULL,0)==RGN_COMPLEX)
		{	update_text();
		}
		else clear_line();
	}
	else
	{	p.xLeft=0; p.yBottom=0;
		p.xRight=xtext; p.yTop=ycursor+htextchar-htextbelow;
		if (WinScrollWindow(TextHandle,0,-htextchar,
			&p,NULL,NULLHANDLE,NULL,0)==RGN_COMPLEX)
		{	update_text();
		}
		else clear_line();
	}
}

void append_char (char c)
{   int i=0;
	char *p;
	if (*textpos)
	{   clear_line();
		delete_char(strlen(textpos)-1);
	}
	else insert_char(1);
	if (c==9)
	{   *textpos++=' ';
		p=textpos; i=1;
		while (p>textarea && *(p-1))
		{	i++; p--;
		}
		while (i%4!=1)
		{	insert_char(1); *textpos++=' '; i++;
		}
	}
	else
	{	*textpos++=c;
	}
	*textpos=0;
}

void update_note (char *note)
/* Print the line from note on */
{   HPS hps;
	POINTL p,ptl[TXTBOX_COUNT];
	char *l;
	if (!*note) return;
	hps=WinGetPS(TextHandle);
	setfont(hps,&textfat);
	GpiSetCharSet(hps,1);
	l=startofline(note);
	if (iscommand(l)) GpiSetColor(hps,CLR_DARKRED);
	GpiQueryTextBox(hps,note-l,l,TXTBOX_COUNT,ptl);
	p.x=ptl[3].x-offset*wtextchar; p.y=ycursor;
	GpiMove(hps,&p);
	GpiCharStringAt(hps,&p,strlen(note),note);
	WinReleasePS(hps);
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
{	char *p=s,*note=textpos;
	DosRequestMutexSem(textsem,-1);
	if (error) pasting=0;
	if (selected)
	{	selected=0;
		update_text();
		update_ycursor();
	}
	if (yout) /* cursor not on screen */
	{	if (oldtopline<textend) topline=oldtopline+1;
		update_ycursor();
		if (yout)
		{	topline=textpos-1;
			while (topline>textarea && *topline) topline--;
			if (topline<textarea) topline=textarea;
			update_ycursor();
		}
		update_text();
	}
	hidecursor();
	while (*p)
	{   truncate_text();
		if (*p=='\n')
		{   update_note(note);
			new_line();
			note=textpos;
		}
		else append_char(*p);
		p++;
	}
	update_note(note);
	if (editing)
	{	clear_line();
	}
	set_slider();
	showcursor();
	oldtopline=topline;
	DosReleaseMutexSem(textsem);
}

void edit_off (void)
/* the command line is no longer in use (graphics or computing) */
{   char *p;
	editing=0; cursor_off();
	while (*textpos) textpos++;
	p=nexttext(textpos);
	if (!p)
	{	p=nextline(textpos);
		textend=p;
		return;
	}
	delete_char(p-1-textpos);
	update_text();
}

void edit_on (void)
/* the command line is active */
{   char *p,*q,*h,c;
	editing=1;
	if (!nojump && !insertcommands)
	{	p=nextline(textpos);
		if (p>=textend) goto cont;
		q=startofline(textpos);
		if (!iscommand(q)) goto cont;
		h=nextcommand(textpos);
		if (!h) goto cont;
		c=*h;
		if (*q=='$' && c=='>') goto cont;
		textpos=q; delete_char(p-q);
		if (!iscommand(q)) q=nextcommand(q);
		*q=c;
		textpos=q+1;
		set_editline(textpos,0);
		update_ycursor();
		update_text();
	}
	cont : cursor_on();
	show_command();
}

/********** Handle command switching ****************/

void show_command (void)
{   char *p;
	hidecursor();
	update_ycursor();
	topline=startofline(topline);
	if (yout)
	{	topline=startofline(textpos);
		p=prevcommand(p);
		if (p) topline=p;
		update_ycursor();
		if (yout)
		{   p=prevtext(p);
			if (p) topline=p;
			update_ycursor();
			if (yout) topline=startofline(textpos);
		}
		update_text();
	}
	showcursor();
}

void prev_command (void)
{	char *p=textpos,c;
	p=startofline(p);
	if (!iscommand(p)) return;
	c=*p;
	p=prevcommand(p);
	if (!p) return;
	if (c=='$' && *p=='>') return;
	while (*p!=c)
	{	p=prevcommand(p);
		if (!p) return;
	}
	textpos=p+1;
	set_editline(textpos,0);
	show_command();
}

void next_command (void)
{	char *p=textpos,c;
	p=startofline(p);
	if (!iscommand(p)) return;
	c=*p;
	p=nextcommand(p);
	if (!p) return;
	if (c=='$' && *p=='>') return;
	while (*p!=c)
	{	p=nextcommand(p);
		if (!p) return;
	}
	textpos=p+1;
	set_editline(textpos,0);
	show_command();
}

void delete_command (void)
/* Delete the command at textpos */
{	char *p,*ph,*q;
	p=startofline(textpos);
	if (!iscommand(p)) return;
	ph=prevtext(p);
	q=nexttext(p);
	if (ph) p=ph;
	if (!q) return;
	textpos=p;
	delete_char(q-textpos);
	if (!iscommand(textpos)) textpos=nextcommand(textpos);
	textpos++;
	update_ycursor();
	update_text();
	set_editline(textpos,0);
}

void insert_command (void)
/* Insert a new, empty command */
{	char *p=textpos,*q,c;
	p=startofline(p);
	if (!iscommand(p)) return;
	c=*p;
	q=prevtext(p);
	if (q) textpos=q;
	else textpos=p;
	insert_char(2);
	*textpos=c;
	*(textpos+1)=0;
	textpos++;
	update_ycursor();
	update_text();
	set_editline(textpos,0);
}

void insert_commands (void)
/* Toggle command insertion */
{	HWND handle=WinWindowFromID(TextFrameHandle,FID_MENU);
	insertcommands=!insertcommands;
	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(IDM_InsertCommands,TRUE),MPFROM2SHORT(MIA_CHECKED,
		insertcommands?MIA_CHECKED:0));
}

void remove_output (void)
/* Remove all output in the selected area */
{   char *p,*q,*qh,*noted;
	if (!selected)
	{	warning("Need a selected block!");
		return;
	}
	p=startsel;
	p=startofline(p);
	if (!iscommand(p)) p=nextcommand(p);
	if (!p) return;
	noted=q=p;
	while (q<endsel)
	{	qh=nextcommand(q);
		if (!qh) break;
		if (qh>endsel || qh>textend) break;
		q=qh;
	}
	while (p<q)
	{	p=nextline(p);
		if (!iscommand(p) && !istext(p))
		{	qh=nexttext(p);
			if (!qh) return;
			textpos=p; delete_char(qh-p);
			q-=qh-p;
			if (istext(p)) p=nextcommand(p);
		}
	}
	selected=0;
	textpos=noted+1;
	update_ycursor();
	update_text();
	show_command();
	set_editline(textpos,0);
}

void set_pos (POINTS *points)
/* p is the screen position. Select text */
{	char *p=det_line(points),*q;
	if (!p) return;
	q=startofline(p);
	if (*q!='>') return;
	q++;
	if (p<q) p++;
	textpos=p;
	update_ycursor();
	show_command();
	set_editline(q,(int)(textpos-q));
}

extern char notebookname[256];

void new_notebook (void)
{   char c;
	HWND h;
	if (question("Delete current Notebook?")==0) return;
	textpos=startofline(textpos);
	c=*textpos;
	clear_screen();
	*textpos++=c;
	set_editline(textpos,0);
	update_ycursor();
	update_text();
	strcpy(notebookname,"");
	h=WinWindowFromID(TextFrameHandle,FID_TITLEBAR);
	WinSetWindowText(h,"Euler Text");
}

void clear_text (void)
{   char *p=textpos,*q;
	p=startofline(p);
	q=prevtext(p);
	if (!q || q==p) return;
	textpos=q; delete_char(p-q);
	textpos++;
	set_editline(textpos,0);
	update_ycursor();
	update_text();
}

#define maxtext 1024
char textbuf[maxtext]="";

MRESULT EXPENTRY textproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{	HWND h;
	switch (msg)
	{   case WM_INITDLG :
			h=WinWindowFromID(hwnd,ID_Text);
			WinSendMsg(h,MLM_SETTEXTLIMIT,(void *)maxtext,NULL);
			WinSetWindowText(h,textbuf);
			break;
		case WM_COMMAND :
			switch(SHORT1FROMMP(mp1))
			{   case DID_OK :
					h=WinWindowFromID(hwnd,ID_Text);
					WinQueryWindowText(h,maxtext,textbuf);
				case DID_CANCEL :
					finish :
					WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			}
			break;
		case WM_HELP :
			show_help(837);
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

void edit_text (void)
{   int ret,start=0,line=linelength*3/4,pos=0;
	char *p,*q,*qh;
	size_t size;
	if (!iscommand(q=startofline(textpos))) return;
	p=prevtext(q);
	if (!p) textbuf[0]=0;
	else
	{   qh=textbuf;
		p++; if (*p==' ') p++;
		while (1)
		{	size=strlen(p);
			if (qh+size-textbuf>maxtext-8) break;
			strcpy(qh,p);
			qh+=size;
			p=nextline(p);
			newline :
			if (istext(p))
			{   p++; if (*p==' ') p++;
				if (*p)
				{	if (!start) *qh++=' ';
					start=0;
				}
				else
				{	*qh++=13; *qh++=10;
					p++; start=1;
					goto newline;
				}
			}
			else break;
			if (qh-textbuf>maxtext-8) break;
		}
		*qh=0;
	}
	ret=WinDlgBox(HWND_DESKTOP,TextHandle,
		textproc,0,DLG_Text,0);
	if (ret==DID_CANCEL) return;
	clear_text();
	textpos=startofline(textpos);
	q=textbuf;
	while (*q)
	{	qh=q;
		pos=0;
		while (*qh && *qh!=13 && *qh!=10)
		{   qh++; pos++;
			if (pos>line && *qh==' ')
			{	insert_char(qh-q+3);
				*textpos++='%';
				*textpos++=' ';
				memmove(textpos,q,qh-q);
				textpos+=(qh-q);
				*textpos++=0;
				q=++qh;
				pos=0;
			}
		}
		insert_char(qh-q+3);
		*textpos++='%';
		*textpos++=' ';
		memmove(textpos,q,qh-q);
		textpos+=(qh-q);
		*textpos++=0;
		pos=0;
		q=qh;
		if (*q==13 || *q==10) q+=2;
		if (*q)
		{	insert_char(3);
			*textpos++='%';
			*textpos++=' ';
			*textpos++=0;
		}
	}
	textpos++;
	set_editline(textpos,0);
	update_ycursor();
	update_text();
	show_command();
}

/********** Handle page up or down ******************/

void set_hslider (void)
{	HWND h;
	h=WinWindowFromID(TextFrameHandle,FID_HORZSCROLL);
	WinSendMsg(h,SBM_SETSCROLLBAR,MPFROMSHORT(offset),
		MPFROM2SHORT(0,256));
}

void set_horpage (int pos)
{	offset=pos;
	set_hslider();
	update_text();
}

void page_left (void)
{	offset-=8; if (offset<0) offset=0;
	set_hslider();
	update_text();
}

void page_right (void)
{	offset+=8; if (offset>256) offset=256;
	set_hslider();
	update_text();
}

void line_left (void)
{	offset--; if (offset<0) offset=0;
	set_hslider();
	update_text();
}

void line_right (void)
{	offset++; if (offset>256) offset=256;
	set_hslider();
	update_text();
}

void set_page (int pos)
/* set the topline according to the slider position */
{	long o;
	char *p;
	o=(long)(pos*(double)(textend-textarea)/1000);
	p=textarea+o;
	while (p>textarea && *(p-1)) p--;
	if (p>=textend || p==topline) return;
	topline=p;
	update_ycursor();
	update_text();
	set_slider();
	showcursor();
}

void slide_page (int pos)
/* set the topline according to the slider position smoothly */
{	long o;
	char *p,*q;
	int i,j;
	o=(long)(pos*(double)(textend-textarea)/1000);
	p=textarea+o;
	while (p>textarea && *(p-1)) p--;
	if (p>=textend) return;
	if (p>topline)
	{	i=0; q=topline;
		while (q<p)
		{	q=q+strlen(q)+1;
			i++;
		}
		if (i<textlines/3)
		{   for (j=0; j<i; j++) line_down();
			return;
		}
	}
	else if (p<topline)
	{	i=0; q=p;
		while (q<topline)
		{	q=q+strlen(q)+1;
			i++;
		}
		if (i<textlines/3)
		{   for (j=0; j<i; j++) line_up();
			return;
		}
	}
	else if (p==topline) return;
	topline=p;
	update_ycursor();
	update_text();
	set_slider();
	showcursor();
}

void set_slider (void)
{	HWND h;
	int pos;
	h=WinWindowFromID(TextFrameHandle,FID_VERTSCROLL);
	if (textpos==textarea) pos=1000;
	else pos=
		(int)((double)(topline-textarea)/(textpos-textarea)*1000);
	WinSendMsg(h,SBM_SETSCROLLBAR,MPFROMSHORT(pos),
		MPFROM2SHORT(0,1000));
}

void page_up (void)
/* scroll back a screen, so that old output is visible */
{   int i;
	if (topline==textarea) return;
	hidecursor();
	for (i=0; i<textlines-1; i++)
	{   topline--;
		while (topline>textarea && *(topline-1)) topline--;
		if (topline==textarea) break;
	}
	update_ycursor();
	update_text();
	set_slider();
	showcursor();
}

void page_up_cursor (void)
/* scroll back a screen, so that old output is visible,
try to put the cursor somewhere on that screen */
{   int i;
	char *p,*q;
	if (topline==textarea) return;
	hidecursor();
	for (i=0; i<textlines-1; i++)
	{   topline--;
		while (topline>textarea && *(topline-1)) topline--;
		if (topline==textarea) break;
	}
	if (editing)
	{   p=startofline(textpos);
		if (*p=='>')
		{   if (*topline!='>')
				p=nexttruecommand(topline);
			else p=topline;
			q=textpos;
			if (p)
			{	textpos=p+1;
				update_ycursor();
				if (yout) textpos=q;
				set_editline(textpos,0);
			}
		}
	}
	update_ycursor();
	update_text();
	set_slider();
	showcursor();
}

void page_down (void)
/* scroll forward a screen */
{	int i;
	char *p;
	hidecursor();
	for (i=0; i<textlines-1; i++)
	{   p=topline+strlen(topline)+1;
		if (p>=textend) break;
		topline=p;
	}
	update_ycursor();
	update_text();
	set_slider();
	showcursor();
}

void page_down_cursor (void)
/* scroll forward a screen */
{	int i;
	char *p,*q;
	hidecursor();
	for (i=0; i<textlines-1; i++)
	{   p=topline+strlen(topline)+1;
		if (p>=textend) break;
		topline=p;
	}
	if (editing)
	{   p=startofline(textpos);
		if (*p=='>')
		{   if (*topline!='>')
				p=nexttruecommand(topline);
			else p=topline;
			q=textpos;
			if (p)
			{	textpos=p+1;
				update_ycursor();
				if (yout) textpos=q;
				set_editline(textpos,0);
			}
		}
	}
	update_ycursor();
	update_text();
	set_slider();
	showcursor();
}

void line_up (void)
/* scroll up one line */
{   if (topline==textarea) return;
	hidecursor();
	topline--;
	while (*(topline-1) && topline>textarea) topline--;
	hidecursor();
	if (WinScrollWindow(TextHandle,0,-htextchar,
			NULL,NULL,NULLHANDLE,NULL,0)==RGN_COMPLEX) update_text();
	update_ycursor();
	update_topline();
	set_slider();
	showcursor();
}

void line_down (void)
/* scroll one line down */
{   char *p;
	p=topline+strlen(topline)+1;
	if (p>=textend) return;
	topline=p;
	hidecursor();
	if (WinScrollWindow(TextHandle,0,htextchar,
			NULL,NULL,NULLHANDLE,NULL,0)==RGN_COMPLEX) update_text();
	update_ycursor();
	update_buttomline();
	set_slider();
	showcursor();
}

/*********** Text and Commands save *****************/

char *select_file (char *filter, int saving,
	char *title, char *ok, char *def, char *savepath)
{	static FILEDLG fd;
	static char path[512];
	char *q;
	HWND fh;
	memset(&fd,0,sizeof(FILEDLG));
	fd.cbSize=sizeof(FILEDLG);
	fd.fl=FDS_CENTER|FDS_ENABLEFILELB
		|(saving?FDS_SAVEAS_DIALOG:FDS_OPEN_DIALOG);
	fd.pszTitle=title;
	fd.pszOKButton=ok;
	if (savepath[strlen(savepath)-1]!='\\' && *savepath)
		strcat(savepath,"\\");
	strcpy(fd.szFullFile,savepath);
	strcat(fd.szFullFile,filter);
	fh=WinFileDlg(HWND_DESKTOP,TextHandle,&fd);
	if (!fh || fd.lReturn!=DID_OK) return 0;
	if (!def || !*def) return path;
	strcpy(path,fd.szFullFile);
	q=path+strlen(path);
	while (q>path)
	{	if (*q=='.' || *q=='\\' || *q==':') break;
		q--;
	}
	if (*q!='.') strcat(path,def);
	strcpy(savepath,fd.szFullFile);
	q=savepath+strlen(savepath);
	while (q>savepath)
	{	if (*q=='\\' || *q==':') break;
		q--;
	}
	if (q>savepath) q++;
	*q=0;
	return path;
}

MRESULT EXPENTRY appendproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{	switch (msg)
	{	case WM_COMMAND :
			WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

char textpath[256]="";
char notebookpath[256]="";

void save_text (void)
{	char *p,s[512];
	FILE *f;
	p=select_file("*.txt",1,"Save Text","Save",".txt",textpath);
	if (!p) return;
	f=fopen(p,"r");
	if (f) /* File exists */
	{   fclose(f);
		switch (WinDlgBox(HWND_DESKTOP,TextHandle,
			appendproc,0,DLG_Append,0))
		{	case BUT_Append : fopen(p,"a"); break;
			case BUT_Overwrite : fopen(p,"w"); break;
			default : return;
		}
	}
	else f=fopen(p,"w");
	if (!f)
	{   sprintf(s,"Could not open\n%s",p);
		warning(s); return;
	}
	p=textarea;
	while (p<textend)
	{	fprintf(f,"%s\n",p);
		p+=strlen(p)+1;
	}
	fclose(f);
}

char notebookname[256]="";

void load_notebook (void)
{	char *p,s[512];
	static char wt[256];
	FILE *f;
	HWND h;
	p=select_file("*.en",0,"Load Notebook","Load",".en",notebookpath);
	if (!p) return;
	f=fopen(p,"r");
	if (!f)
	{   sprintf(s,"Could not open\n%s",p);
		warning(s); return;
	}
	strcpy(notebookname,p);
	h=WinWindowFromID(TextFrameHandle,FID_TITLEBAR);
	strcpy(wt,p);
	WinSetWindowText(h,wt);
	p=textarea;
	clear_screen();
	while (!feof(f))
	{	fgets(p,1024,f);
		p+=strlen(p);
		if (*(p-1)=='\n') { p--; *p=0; }
		textend=p;
		p++;
		if (p>textareaend-1024)
		{	warning("Notebook to large!");
			break;
		}
	}
	fclose(f);
	textpos=topline=oldtopline=textarea;
	if (!iscommand(textpos)) textpos=nextcommand(textpos);
	if (!textpos)
	{	warning("Notebook has not a single command!");
		clear_screen();
		*textpos='>';
	}
	textpos++;
	update_ycursor();
	update_text();
	show_command();
	set_editline(textpos,0);
}

void save_notebook_as (void)
{	char *p,s[512];
	FILE *f;
	HWND h;
	p=select_file("*.en",1,"Save Notebook","Save",".en",
		notebookpath);
	if (!p) return;
	f=fopen(p,"r");
	if (f) /* File exists */
	{   fclose(f);
		switch (WinDlgBox(HWND_DESKTOP,TextHandle,
			appendproc,0,DLG_Append,0))
		{	case BUT_Append : fopen(p,"a"); break;
			case BUT_Overwrite : fopen(p,"w"); break;
			default : return;
		}
	}
	else f=fopen(p,"w");
	if (!f)
	{   sprintf(s,"Could not open\n%s",p);
		warning(s); return;
	}
	strcpy(notebookname,p);
	h=WinWindowFromID(TextFrameHandle,FID_TITLEBAR);
	WinSetWindowText(h,p);
	p=textarea;
	while (p<textend)
	{	fprintf(f,"%s\n",p);
		p+=strlen(p)+1;
	}
	fclose(f);
}

void save_notebook (void)
{	char *p,s[512];
	FILE *f;
	p=notebookname;
	if (!*p)
	{	save_notebook_as();
		return;
	}
	f=fopen(p,"r");
	if (f) /* File exists */
	{   fclose(f);
		switch (WinDlgBox(HWND_DESKTOP,TextHandle,
			appendproc,0,DLG_Append,0))
		{	case BUT_Append : fopen(p,"a"); break;
			case BUT_Overwrite : fopen(p,"w"); break;
			default : return;
		}
	}
	else f=fopen(p,"w");
	if (!f)
	{   sprintf(s,"Could not open\n%s",p);
		warning(s); return;
	}
	p=textarea;
	while (p<textend)
	{	fprintf(f,"%s\n",p);
		p+=strlen(p)+1;
	}
	fclose(f);
}

void save_commands (void)
{	char *p,s[512];
	FILE *f;
	p=select_file("*.txt",1,"Save Commands","Save",".txt",textpath);
	if (!p) return;
	f=fopen(p,"r");
	if (f) /* File exists */
	{   fclose(f);
		switch (WinDlgBox(HWND_DESKTOP,TextHandle,
			appendproc,0,DLG_Append,0))
		{	case BUT_Append : fopen(p,"a"); break;
			case BUT_Overwrite : fopen(p,"w"); break;
			default : return;
		}
	}
	else f=fopen(p,"w");
	if (!f)
	{   sprintf(s,"Could not open\n%s",p);
		warning(s); return;
	}
	p=textarea;
	while (p+strlen(p)+1<textpos)
	{   if (*p=='>') fprintf(f,"%s\n",p+1);
		else if (*p=='$') fprintf(f,"    %s\n",p+4);
		p+=strlen(p)+1;
	}
	fclose(f);
}

/******** Text selection routines *******************/

char *det_line (POINTS *point)
/* Determine the text line into which the user clicked */
{	int i,l;
	char *p;
	size_t size;
	HPS tps;
	POINTL pc[1024];
	l=(ytext-1-point->y)/htextchar;
	p=topline;
	for (i=0; i<l; i++)
	{	size=strlen(p);
		p+=size+1;
		if (p>textend) return 0;
	}
	tps=WinGetPS(TextHandle);
	setfont(tps,&textfat);
	GpiSetCharSet(tps,1);
	pc[0].x=-offset*wtextchar; pc[0].y=0; GpiMove(tps,pc);
	GpiQueryCharStringPos(tps,0L,strlen(p),p,NULL,pc);
	i=1;
	while (*p && point->x>pc[i].x)
	{	i++; p++;
	}
	WinReleasePS(tps);
	return p;
}

void select_text (POINTS *p)
/* p is the screen position. Select text */
{	if (!selected)
	{	startsel=det_line(p);
		if (startsel) selected=1;
		else return;
		endsel=textend;
		if (endsel<=startsel)
		{	selected=0;
			return;
		}
	}
	else if (selected==1)
	{	endsel=det_line(p);
		if (!endsel)
		{	endsel=textend;
		}
		else if (endsel<textend && *(endsel+1)==0) endsel++;
		if (endsel<=startsel) selected=0;
		else selected=2;
	}
	else
	{	selected=0;
	}
	update_text();
}

void clip_copy (void)
{   PSZ psz;
	size_t size;
	char *p,*q;
	if (!selected) return;
	p=startsel; size=0;
	while (p<endsel)
	{	if (!*p++) size+=2;
		else size++;
	}
	if (!WinOpenClipbrd(hab))
	{	warning("Could not open Clipboard");
		return;
	}
	DosAllocSharedMem((void **)&psz,NULL,size+1,
		PAG_WRITE|PAG_COMMIT|OBJ_GIVEABLE);
	p=startsel; q=psz;
	while (p<endsel)
	{	if (*p) *q++=*p++;
		else { *q++=13; *q++=10; p++; }
	}
	*q++=0;
	WinEmptyClipbrd(hab);
	WinSetClipbrdData(hab,(ULONG)psz,CF_TEXT,CFI_POINTER);
	WinCloseClipbrd(hab);
}

void cut (void)
/* Cut selected text from output */
{   char *p;
	size_t size;
	if (!selected) return;
	size=endsel-startsel+1;
	p=textpos;
	if (startofline(p)>=endsel) p-=size;
	else if (nextline(p)>=startsel)
	{	warning("Please, position the cursor line\noutside the block!");
		return;
	}
	textpos=startsel;
	delete_char(size);
	textpos=p;
	selected=0;
	update_ycursor();
	update_text();
	show_command();
}

void copy_bitmap (void)
{	PSZ pszData[4]={"Display",NULL,NULL,NULL};
	HDC hdcMem;
	SIZEL sizlPage={0,0};
	BITMAPINFOHEADER2 bmp;
	PBITMAPINFO2 pbmi;
	HBITMAP hbm;
	LONG alData[2],size;
	hdcMem=DevOpenDC(hab,OD_MEMORY,"*",4,
		(PDEVOPENDATA)pszData,NULLHANDLE);
	sizlPage.cx=copy_xsize; sizlPage.cy=copy_ysize;
	globalgps=GpiCreatePS(hab,hdcMem,&sizlPage,
		PU_PELS|GPIA_ASSOC|GPIT_MICRO);
	GpiQueryDeviceBitmapFormats(globalgps,2,(PLONG)alData);
	bmp.cbFix=(ULONG)sizeof(BITMAPINFOHEADER2);
	bmp.cx=copy_xsize; bmp.cy=copy_ysize;
	bmp.cPlanes=alData[0];
	bmp.cBitCount=alData[1];
	if (bmp.cPlanes+bmp.cBitCount!=colorbits)
	{	bmp.cPlanes=1; bmp.cBitCount=colorbits;
	}
	bmp.ulCompression=BCA_UNCOMP;
	bmp.cbImage=(((copy_xsize*(1<<bmp.cPlanes)*(1<<bmp.cBitCount))+31)
		/32)*copy_ysize;
	bmp.cxResolution=70; bmp.cyResolution=70;
	bmp.cclrUsed=2; bmp.cclrImportant=0;
	bmp.usUnits=BRU_METRIC; bmp.usReserved=0;
	bmp.usRecording=BRA_BOTTOMUP; bmp.usRendering=BRH_NOTHALFTONED;
	bmp.cSize1=0; bmp.cSize2=0;
	bmp.ulColorEncoding=BCE_RGB; bmp.ulIdentifier=0;
	size=sizeof(BITMAPINFO2)+
		(sizeof(RGB2)*(1<<bmp.cPlanes)*(1<<bmp.cBitCount));
	if (DosAllocSharedMem((void **)&pbmi,NULL,size,
		PAG_COMMIT|PAG_READ|PAG_WRITE|OBJ_GIVEABLE))
	{	warning("Could not get memory\nfor bitmap.");
		return;
	}
	pbmi->cbFix=bmp.cbFix;
	pbmi->cx=bmp.cx; pbmi->cy=bmp.cy;
	pbmi->cPlanes=bmp.cPlanes; pbmi->cBitCount=bmp.cBitCount;
	pbmi->ulCompression=BCA_UNCOMP;
	pbmi->cbImage=((copy_xsize+31)/32)*copy_ysize;
	pbmi->cxResolution=70; pbmi->cyResolution=70;
	pbmi->cclrUsed=2; pbmi->cclrImportant=0;
	pbmi->usUnits=BRU_METRIC;
	pbmi->usReserved=0;
	pbmi->usRecording=BRA_BOTTOMUP;
	pbmi->usRendering=BRH_NOTHALFTONED;
	pbmi->cSize1=0; pbmi->cSize2=0;
	pbmi->ulColorEncoding=BCE_RGB;
	pbmi->ulIdentifier=0;
	hbm=GpiCreateBitmap(globalgps,&bmp,FALSE,NULL,pbmi);
	GpiSetBitmap(globalgps,hbm);
	gpsvalid=1;
	gpscopy=1;
	comp_charsize(globalgps);
	replay();
	gpscopy=0;
	comp_charsize(globalgps);
	gpsvalid=0;
	GpiDestroyPS(globalgps);
	DevCloseDC(hdcMem);
	if (!WinOpenClipbrd(hab))
	{	warning("Could not open Clipboard");
		return;
	}
	WinEmptyClipbrd(hab);
	WinSetClipbrdOwner(hab,NULLHANDLE);
	WinSetClipbrdData(hab,hbm,CF_BITMAP,CFI_HANDLE);
	WinCloseClipbrd(hab);
}

void copy_meta (void)
{   HDC hdc;
	DEVOPENSTRUC dop;
	HMF hmf;
	if (!usenote)
	{	warning("Meta file not available!");
		return;
	}
	dop.pszLogAddress=(PSZ)NULL;
	dop.pszDriverName="DISPLAY";
	hdc=DevOpenDC(hab,OD_METAFILE,"*",2L,(PDEVOPENDATA)&dop,0);
	if (hdc==DEV_ERROR)
	{	warning("Could not open meta file");
		return;
	}
	globalgps=GpiCreatePS(hab,hdc,&sizel,
		PU_PELS|GPIA_ASSOC);
	gpsvalid=1;
	gpsmeta=1;
	replay();
	gpsvalid=0;
	gpsmeta=0;
	GpiAssociate(globalgps,(HDC)NULLHANDLE);
	GpiDestroyPS(globalgps);
	hmf=DevCloseDC(hdc);
	if (!WinOpenClipbrd(hab))
	{	warning("Could not open Clipboard");
		return;
	}
	WinEmptyClipbrd(hab);
	WinSetClipbrdData(hab,hmf,CF_METAFILE,CFI_HANDLE);
	WinCloseClipbrd(hab);
}

void clip_copy_commands (void)
{   PSZ psz;
	size_t size;
	char *p,*q;
	if (!selected) return;
	p=startsel; size=0;
	while (p<endsel)
	{	if (p==textarea || !*(p-1)) /* start of line */
		{	if (*p=='>') p++;
			else if (*p=='$') p++;
			else goto cont;
			while (*p && p<endsel) { p++; size++; }
			if (p<endsel) size+=2;
			else break;
		}
		cont: p++;
	}
	if (!WinOpenClipbrd(hab))
	{	warning("Could not open Clipboard");
		return;
	}
	DosAllocSharedMem((void **)&psz,NULL,size+1,
		PAG_WRITE|PAG_COMMIT|OBJ_GIVEABLE);
	p=startsel; q=psz;
	while (p<endsel)
	{	if (p==textarea || !*(p-1)) /* start of line */
		{	if (*p=='>' || *p=='$') p++;
			else goto cont1;
			while (*p && p<endsel) { *q++=*p++; }
			if (p<endsel) { *q++=13; *q++=10; }
			else break;
		}
		cont1: p++;
	}
	*q++=0;
	WinEmptyClipbrd(hab);
	WinSetClipbrdData(hab,(ULONG)psz,CF_TEXT,CFI_POINTER);
	WinCloseClipbrd(hab);
}

void paste (void)
{   PSZ psz;
	char *p;
	if (!WinOpenClipbrd(hab)) return;
	psz=(PSZ)WinQueryClipbrdData(hab,CF_TEXT);
	if (!psz)
	{	WinCloseClipbrd(hab);
		return;
	}
	p=clipbuf;
	while (*psz)
	{	if (*psz==13 && *(psz+1)==10)
		{	psz+=2; *p++=10;
		}
		else *p++=*psz++;
		if (p>clipend-2)
		{   warning("Clip buffer overflow");
			pasting=0; p=clipbuf; break;
		}
	}
	*p=0;
	WinCloseClipbrd(hab);
	pclip=clipbuf;
	if (p>clipbuf)
	{	pasting=1;
		WinPostMsg(DummyHandle,WM_CHAR,0,0);
	}
}

void paste_commands (void)
{   PSZ psz;
	char *p;
	if (!WinOpenClipbrd(hab)) return;
	psz=(PSZ)WinQueryClipbrdData(hab,CF_TEXT);
	if (!psz)
	{	WinCloseClipbrd(hab);
		return;
	}
	p=clipbuf;
	while (*psz)
	{   if (p==clipbuf || *(p-1)==10)
		{	if (*psz=='>' || *p=='$') { psz++; }
			else goto cont;
			while (*psz && *psz!=13 && *psz!=10)
			{	*p++=*psz++;
				if (p>clipend-2)
				{   warning("Clip buffer overflow");
					pasting=0; p=clipbuf; goto end;
				}
			}
			*p++=10;
		}
		cont: psz++;
	}
	end: *p=0;
	WinCloseClipbrd(hab);
	pclip=clipbuf;
	if (p>clipbuf)
	{	pasting=1;
		WinPostMsg(DummyHandle,WM_CHAR,0,0);
	}
}

/***** Printing *********************/

int initstd (void)
/* Search all queues and devices */
{   SplEnumQueue(NULL,3,pQueue,0,
		&cReturnedQueue,&cTotalQueue,&cbNeededQueue,NULL);
	if (cTotalQueue==0)
	{   warning("No printer queues found!");
		return 0;
	}
	if (DosAllocMem(&pQueue,cbNeededQueue,
			PAG_READ|PAG_WRITE|PAG_COMMIT))
	{   warning("Allocation error");
		return 0;
	}
	SplEnumQueue(NULL,3,pQueue,cbNeededQueue,
		&cReturnedQueue,&cTotalQueue,&cbNeededQueue,NULL);
	return 1;
}

void exitstd (void)
/* release queues and devices */
{	DosFreeMem(pQueue);
}

int openstd (void)
/* set device context */
{	LONG alArray [CAPS_WIDTH_IN_CHARS];
	double w,h;
	RECTL r;
	int z;
	PPRQINFO3 prqQueue=(PPRQINFO3)pQueue;
	if (!strcmp(queuename,"Default"))
		strcpy(stdprint,prqQueue->pszName);
	else strcpy(stdprint,queuename);
	for (z=0; z<cReturnedQueue; z++)
	{	if (!strcmp(stdprint,prqQueue->pszName)) break;
		prqQueue++;
	}
	if (z==cReturnedQueue)
	{   warning("Could not find printer queue!");
		return 0;
	}
	/* fill device open structure */
	dos.pszLogAddress=prqQueue->pszName;
	dos.pszDriverName=prqQueue->pszDriverName;
	*strchr(dos.pszDriverName,'.')=0;
	dos.pszComment="Euler";
	dos.pszDataType="PM_Q_STD";
	dos.pszQueueProcName=prqQueue->pszPrProc;
	dos.pszQueueProcParams=NULL;
	dos.pszSpoolerParams=NULL;
	dos.pszNetworkParams=NULL;
	dos.pdriv=prqQueue->pDriverData;
	/* open printer device */
	hdcPrinter=DevOpenDC(hab,OD_QUEUED,"*",9,
		(PDEVOPENDATA)&dos,(HDC)0);
	if (hdcPrinter==DEV_ERROR)
	{   warning("Could not open printer device!");
		return 0;
	}
	globalgps=GpiCreatePS(hab,hdcPrinter,&sizel,
		PU_PELS|GPIA_ASSOC);
	gpsvalid=1;
	gpsprinter=1;
	DevQueryCaps(hdcPrinter,CAPS_FAMILY,CAPS_WIDTH_IN_CHARS,
		(PLONG)alArray);
	w=alArray[CAPS_WIDTH];
	h=alArray[CAPS_HEIGHT];
	if ((double)w/h < (double)xsize/ysize)
	{	xpixel=w*(pcsize/100.0);
		ypixel=xpixel*(double)ysize/xsize;
		xoffset=(double)(w-xpixel)*pcalignhor/100.0;
		yoffset=h-ypixel-(double)(h-ypixel)*pcalignver/100.0;
		ypixel--;
	}
	else
	{	ypixel=h*(pcsize/100.0);
		xpixel=ypixel*(double)xsize/ysize;
		yoffset=h-ypixel-(double)(h-ypixel)*pcalignver/100.0;
		xoffset=(double)(w-xpixel)*pcalignhor/100.0;
		ypixel--;
	}
	setfat(globalgps,&printfat,printerfontname,
		ypixel/graphiclines);
	if (DevEscape(hdcPrinter,DEVESC_STARTDOC,
			strlen("Euler"),"Euler",0,0)
		!=DEV_OK)
	{   warning("Printer error");
		return 0;
	}
	if (printclip)
	{	r.xLeft=xoffset; r.xRight=xoffset+xpixel;
		r.yBottom=yoffset; r.yTop=yoffset+ypixel;
		GpiIntersectClipRectangle(globalgps,&r);
	}
	return 1;
}

void closestd (void)
{	DevEscape(hdcPrinter,DEVESC_ENDDOC,0,0,0,0);
	GpiDestroyPS(globalgps);
	DevCloseDC(hdcPrinter);
	gpsvalid=0;
	gpsprinter=0;
}

void print_graphics (void)
{   int o,printeroutline;
	FONTMETRICS fm;
	if (gpsvalid) goto stop;
	if (!initstd()) goto stop;
	if (!openstd()) goto stop;
	GpiQueryFontMetrics(globalgps,sizeof(FONTMETRICS),&fm);
	if (fm.fsDefn&FM_DEFN_OUTLINE) printeroutline=1;
	else printeroutline=0;
	o=outline; outline=printeroutline;
	replay();
	outline=o;
	closestd();
	exitstd();
	stop: ;
}

HPS get_printhandle (void)
/* set device context */
{	PPRQINFO3 prqQueue;
	int z;
	SplEnumQueue(NULL,3,pQueue,0,
		&cReturnedQueue,&cTotalQueue,&cbNeededQueue,NULL);
	if (cTotalQueue==0) return NULLHANDLE;
	if (DosAllocMem(&pQueue,cbNeededQueue,
			PAG_READ|PAG_WRITE|PAG_COMMIT)) return NULLHANDLE;
	SplEnumQueue(NULL,3,pQueue,cbNeededQueue,
		&cReturnedQueue,&cTotalQueue,&cbNeededQueue,NULL);
	prqQueue=(PPRQINFO3)pQueue;
	if (!strcmp(queuename,"Default"))
		strcpy(stdprint,prqQueue->pszName);
	else strcpy(stdprint,queuename);
	for (z=0; z<cReturnedQueue; z++)
	{	if (!strcmp(stdprint,prqQueue->pszName)) break;
		prqQueue++;
	}
	if (z==cReturnedQueue) return NULLHANDLE;
	/* fill device open structure */
	dos.pszLogAddress=prqQueue->pszName;
	dos.pszDriverName=prqQueue->pszDriverName;
	*strchr(dos.pszDriverName,'.')=0;
	dos.pdriv=prqQueue->pDriverData;
	/* open printer device */
	hdcPrinter=DevOpenDC(hab,OD_INFO,"*",3,
		(PDEVOPENDATA)&dos,(HDC)0);
	if (hdcPrinter==DEV_ERROR) return NULLHANDLE;
	return GpiCreatePS(hab,hdcPrinter,&sizel,PU_PELS|GPIA_ASSOC);
}

void release_printhandle (HPS hps)
{   exitstd();
	DevEscape(hdcPrinter,DEVESC_ENDDOC,0,0,0,0);
	GpiDestroyPS(hps);
	DevCloseDC(hdcPrinter);
}

/*********** Meta file output ************************/

char graphicspath[256]="";

void write_meta (void)
{   HDC hdc;
	DEVOPENSTRUC dop;
	HMF hmf;
	char *p;
	if (!usenote)
	{   warning("Meta file not available!");
		return;
	}
	dop.pszLogAddress=(PSZ)NULL;
	dop.pszDriverName="DISPLAY";
	hdc=DevOpenDC(hab,OD_METAFILE,"*",2L,(PDEVOPENDATA)&dop,0);
	if (hdc==DEV_ERROR)
	{   warning("Could not open meta file");
		return;
	}
	globalgps=GpiCreatePS(hab,hdc,&sizel,
		PU_PELS|GPIA_ASSOC);
	gpsvalid=1;
	gpsmeta=1;
	replay();
	gpsvalid=0;
	gpsmeta=0;
	GpiAssociate(globalgps,(HDC)NULLHANDLE);
	GpiDestroyPS(globalgps);
	hmf=DevCloseDC(hdc);
	p=select_file("*.met",1,"Save as Metafile","Save",".met",
		graphicspath);
	if (!p) return;
	DosDelete(p);
	GpiSaveMetaFile(hmf,p);
	GpiDeleteMetaFile(hmf);
}

/********* Bitmap save *******************************/

void save_bitmap (HPS bps)
{   char *p;
	FILE *f;
	BITMAPFILEHEADER2 bfh;
	char *buffer;
	PBITMAPINFO2 pbmi;
	long size,colorsize,hdsize=20;
	ULONG compression=BCA_UNCOMP;
	p=select_file("*.bmp",1,"Save as Bitmap","Save",".bmp",
		graphicspath);
	if (!p) return;
	f=fopen(p,"wb");
	if (!f)
	{   warning("Could not open\nthat file.");
		return;
	}
	bfh.usType=BFT_BMAP;
	colorsize=((colorbits<=8)?(1<<colorbits):256)*sizeof(RGB2);
	size=((copy_xsize*colorbits+31)/32)*4*copy_ysize;
	bfh.offBits=14+hdsize+colorsize;
	bfh.cbSize=14+hdsize+colorsize+size;
	buffer=(char *)malloc(size);
	pbmi=(PBITMAPINFO2)malloc(16+colorsize);
	memset(pbmi,0,sizeof(BITMAPINFOHEADER2));
	pbmi->cbFix=hdsize;
	pbmi->cPlanes=1;
	pbmi->cBitCount=colorbits;
	pbmi->ulCompression=compression;
	if (GpiQueryBitmapBits(bps,0,copy_ysize,buffer,pbmi)!=copy_ysize)
	{   warning("Bitmap Save Error!");
		goto end;
	}
	fwrite((char *)&bfh,14,1,f);
	fwrite((char *)pbmi,hdsize+colorsize,1,f);
	fwrite(buffer,1,size,f);
	end: free(pbmi);
	free(buffer);
	fclose(f);
}

void write_bitmap (void)
{	PSZ pszData[4]={"Display",NULL,NULL,NULL};
	HDC hdcMem;
	SIZEL sizlPage={0,0};
	BITMAPINFOHEADER2 bmp;
	PBITMAPINFO2 pbmi;
	HBITMAP hbm;
	LONG size;
	hdcMem=DevOpenDC(hab,OD_MEMORY,"*",4,
		(PDEVOPENDATA)pszData,NULLHANDLE);
	sizlPage.cx=copy_xsize; sizlPage.cy=copy_ysize;
	globalgps=GpiCreatePS(hab,hdcMem,&sizlPage,
		PU_PELS|GPIA_ASSOC|GPIT_MICRO);
	bmp.cbFix=(ULONG)sizeof(BITMAPINFOHEADER2);
	bmp.cx=copy_xsize; bmp.cy=copy_ysize;
	bmp.cPlanes=1; bmp.cBitCount=colorbits;
	bmp.ulCompression=BCA_UNCOMP;
	bmp.cbImage=(((copy_xsize*(1<<bmp.cPlanes)*(1<<bmp.cBitCount))+31)
		/32)*copy_ysize;
	bmp.cxResolution=70; bmp.cyResolution=70;
	bmp.cclrUsed=2; bmp.cclrImportant=0;
	bmp.usUnits=BRU_METRIC; bmp.usReserved=0;
	bmp.usRecording=BRA_BOTTOMUP; bmp.usRendering=BRH_NOTHALFTONED;
	bmp.cSize1=0; bmp.cSize2=0;
	bmp.ulColorEncoding=BCE_RGB; bmp.ulIdentifier=0;
	size=sizeof(BITMAPINFO2)+
		(sizeof(RGB2)*(1<<bmp.cPlanes)*(1<<bmp.cBitCount));
	if (DosAllocMem((void **)&pbmi,size,PAG_COMMIT|PAG_READ|PAG_WRITE))
	{   warning("Could not get memory\nfor bitmap.");
		return;
	}
	pbmi->cbFix=bmp.cbFix;
	pbmi->cx=bmp.cx; pbmi->cy=bmp.cy;
	pbmi->cPlanes=bmp.cPlanes; pbmi->cBitCount=bmp.cBitCount;
	pbmi->ulCompression=BCA_UNCOMP;
	pbmi->cbImage=((copy_xsize+31)/32)*copy_ysize;
	pbmi->cxResolution=70; pbmi->cyResolution=70;
	pbmi->cclrUsed=2; pbmi->cclrImportant=0;
	pbmi->usUnits=BRU_METRIC;
	pbmi->usReserved=0;
	pbmi->usRecording=BRA_BOTTOMUP;
	pbmi->usRendering=BRH_NOTHALFTONED;
	pbmi->cSize1=0; pbmi->cSize2=0;
	pbmi->ulColorEncoding=BCE_RGB; pbmi->ulIdentifier=0;
	hbm=GpiCreateBitmap(globalgps,&bmp,FALSE,NULL,pbmi);
	GpiSetBitmap(globalgps,hbm);
	gpsvalid=1;
	gpscopy=1;
	comp_charsize(globalgps);
	replay();
	gpscopy=0;
	comp_charsize(globalgps);
	gpsvalid=0;
	save_bitmap(globalgps);
	GpiDeleteBitmap(hbm);
	DosFreeMem(pbmi);
	GpiDestroyPS(globalgps);
	DevCloseDC(hdcMem);
}

/********** Settings *********************************/

int ask_save (void)
{	return question("Do you want to save settings?");
}

int clicked (HWND hwnd, int id)
{	HWND h;
	h=WinWindowFromID(hwnd,id);
	return (int)WinSendMsg(h,BM_QUERYCHECK,NULL,NULL);
}

long getnumber (HWND hwnd, int id)
{	HWND h;
	char text[256];
	h=WinWindowFromID(hwnd,id);
	WinQueryWindowText(h,254,text);
	return atol(text);
}

int getchoice (HWND hwnd, int id, char *c[], int n)
{	HWND h;
	int i;
	char text[256];
	h=WinWindowFromID(hwnd,id);
	WinQueryWindowText(h,254,text);
	for (i=0; i<n; i++)
	{	if (!strcmp(text,c[i])) return i;
	}
	return 0;
}

char *makename (PFONTMETRICS p)
{	if (p->fsDefn&FM_DEFN_OUTLINE) strcpy(name,"(O)");
	else strcpy(name,"(I)");
	strcat(name,p->szFacename);
	return name;
}

MRESULT EXPENTRY setfontproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{   HWND h;
	PFONTMETRICS p;
	int i,k=1,sel;
	switch (msg)
	{   case WM_INITDLG :
			/* show graphics fonts */
			h=WinWindowFromID(hwnd,ID_GraphicsFont);
			p=pfm;
			strcpy(last,"");
			WinSendMsg(h,LM_INSERTITEM,MPFROMSHORT(LIT_END),
				FDEFAULT);
			for (sel=0, k=1, i=0; i<fontnumber; i++)
			{   if (strcmp(makename(p),last))
				{	WinSendMsg(h,LM_INSERTITEM,
						MPFROMSHORT(LIT_END),name);
					strcpy(last,name);
					if (!strcmp(last,fontname)) sel=k;
					k++;
				}
				p++;
			}
			WinSendMsg(h,LM_SELECTITEM,MPFROMSHORT(sel),
				MPFROMSHORT(TRUE));
			/* show text fonts */
			h=WinWindowFromID(hwnd,ID_TextFont);
			p=textpfm;
			strcpy(last,"");
			WinSendMsg(h,LM_INSERTITEM,MPFROMSHORT(LIT_END),
				FDEFAULT);
			for (sel=0, k=1, i=0; i<textfontnumber; i++)
			{   if (strcmp(makename(p),last))
				{	WinSendMsg(h,LM_INSERTITEM,
						MPFROMSHORT(LIT_END),name);
					strcpy(last,name);
					if (!strcmp(last,textfontname)) sel=k;
					k++;
				}
				p++;
			}
			WinSendMsg(h,LM_SELECTITEM,MPFROMSHORT(sel),
				MPFROMSHORT(TRUE));
			/* show printer fonts */
			h=WinWindowFromID(hwnd,ID_PrinterFont);
			p=printerpfm;
			strcpy(last,"");
			WinSendMsg(h,LM_INSERTITEM,MPFROMSHORT(LIT_END),
				FDEFAULT);
			for (sel=0, i=0, k=1; i<printerfontnumber; i++)
			{   if (strcmp(makename(p),last))
				{	WinSendMsg(h,LM_INSERTITEM,
						MPFROMSHORT(LIT_END),name);
					strcpy(last,name);
					if (!strcmp(last,printerfontname)) sel=k;
					k++;
				}
				p++;
			}
			WinSendMsg(h,LM_SELECTITEM,MPFROMSHORT(sel),
				MPFROMSHORT(TRUE));
			/* show lines per screen for graphics */
			h=WinWindowFromID(hwnd,ID_Lines);
			WinSendMsg(h,SPBM_SETLIMITS,(MPARAM)200,(MPARAM)10);
			WinSendMsg(h,SPBM_SETCURRENTVALUE,
				(MPARAM)graphiclines,0);
			h=WinWindowFromID(hwnd,ID_TextLines);
			WinSendMsg(h,SPBM_SETLIMITS,(MPARAM)200,(MPARAM)10);
			WinSendMsg(h,SPBM_SETCURRENTVALUE,
				(MPARAM)dtextlines,0);
			break;
		case WM_COMMAND :
			switch (SHORT1FROMMP(mp1))
			{   case DID_OK :
				case DID_SAVE :
					h=WinWindowFromID(hwnd,ID_GraphicsFont);
					WinQueryWindowText(h,254,fontname);
					h=WinWindowFromID(hwnd,ID_TextFont);
					WinQueryWindowText(h,254,textfontname);
					h=WinWindowFromID(hwnd,ID_PrinterFont);
					WinQueryWindowText(h,254,printerfontname);
					h=WinWindowFromID(hwnd,ID_Lines);
					WinSendMsg(h,SPBM_QUERYVALUE,&graphiclines,0);
					h=WinWindowFromID(hwnd,ID_TextLines);
					WinSendMsg(h,SPBM_QUERYVALUE,&dtextlines,0);
				case DID_CANCEL :
					WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			}
			break;
		case WM_HELP :
			show_help(831);
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

void set_fonts (void)
{   HPS hps;
	int ret;
	hps=WinGetPS(GrafHandle);
	pfm=getfonts(hps,&fontnumber);
	WinReleasePS(hps);
	hps=WinGetPS(TextHandle);
	textpfm=getfonts(hps,&textfontnumber);
	WinReleasePS(hps);
	hps=get_printhandle();
	if (hps!=NULLHANDLE)
	{	printerpfm=getfonts(hps,&printerfontnumber);
		release_printhandle(hps);
	}
	else printerfontnumber=0;
	ret=WinDlgBox(HWND_DESKTOP,TextHandle,
		setfontproc,0,DLG_Fonts,0);
	if (ret==DID_CANCEL) goto cont;
	set_bothfonts();
	/* free memory */
	cont : DosFreeMem(pfm);
	DosFreeMem(textpfm);
	if (printerfontnumber>0) DosFreeMem(printerpfm);
	if (ret==DID_SAVE) save_font_config();
}

MRESULT EXPENTRY setpathsproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{   HWND h;
	switch (msg)
	{   case WM_INITDLG :
			h=WinWindowFromID(hwnd,ID_LoadPath);
			WinSetWindowText(h,loadpath);
			h=WinWindowFromID(hwnd,ID_TextPath);
			WinSetWindowText(h,textpath);
			h=WinWindowFromID(hwnd,ID_GraphicsPath);
			WinSetWindowText(h,graphicspath);
			h=WinWindowFromID(hwnd,ID_StartPath);
			WinSetWindowText(h,startpath);
			h=WinWindowFromID(hwnd,ID_StorePath);
			WinSetWindowText(h,storepath);
			h=WinWindowFromID(hwnd,ID_NotebookPath);
			WinSetWindowText(h,notebookpath);
			break;
		case WM_COMMAND :
			switch (SHORT1FROMMP(mp1))
			{   case DID_OK :
				case DID_SAVE :
					h=WinWindowFromID(hwnd,ID_TextPath);
					WinQueryWindowText(h,254,textpath);
					h=WinWindowFromID(hwnd,ID_GraphicsPath);
					WinQueryWindowText(h,254,graphicspath);
					h=WinWindowFromID(hwnd,ID_LoadPath);
					WinQueryWindowText(h,254,loadpath);
					h=WinWindowFromID(hwnd,ID_StartPath);
					WinQueryWindowText(h,254,startpath);
					h=WinWindowFromID(hwnd,ID_StorePath);
					WinQueryWindowText(h,254,storepath);
					h=WinWindowFromID(hwnd,ID_NotebookPath);
					WinQueryWindowText(h,254,notebookpath);
				case DID_CANCEL :
					WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			}
			break;
		case WM_HELP :
			show_help(835);
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

void set_paths (void)
{   int ret;
	ret=WinDlgBox(HWND_DESKTOP,TextHandle,
		setpathsproc,0,DLG_Paths,0);
	if (ret==DID_CANCEL) return;
	if (ret==DID_SAVE) save_path_config();
}

MRESULT EXPENTRY sizesproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{   HWND h;
	int i;
	static char sw[16],sh[16];
	switch (msg)
	{   case WM_INITDLG :
			h=WinWindowFromID(hwnd,ID_Stacksize);
			for (i=0; i<=15; i++)
				WinSendMsg(h,SLM_SETTICKSIZE,
					MPFROM2SHORT(i,3),NULL);
			WinSendMsg(h,SLM_SETSCALETEXT,MPFROMSHORT(0),"1");
			WinSendMsg(h,SLM_SETSCALETEXT,MPFROMSHORT(15),"16");
			WinSendMsg(h,SLM_SETSLIDERINFO,
				MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),
				MPFROMSHORT((int)(stacksize/(1024*1024l)-1)));
			h=WinWindowFromID(hwnd,ID_Textbuffer);
			for (i=0; i<=15; i++)
				WinSendMsg(h,SLM_SETTICKSIZE,
					MPFROM2SHORT(i,3),NULL);
			WinSendMsg(h,SLM_SETSCALETEXT,MPFROMSHORT(0),"64");
			WinSendMsg(h,SLM_SETSCALETEXT,MPFROMSHORT(15),"1024");
			WinSendMsg(h,SLM_SETSLIDERINFO,
				MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),
				(MPARAM)(textsize/(64*1024l)-1));
			h=WinWindowFromID(hwnd,ID_Clipbuffer);
			for (i=0; i<=15; i++)
				WinSendMsg(h,SLM_SETTICKSIZE,
					MPFROM2SHORT(i,3),NULL);
			WinSendMsg(h,SLM_SETSCALETEXT,MPFROMSHORT(0),"64");
			WinSendMsg(h,SLM_SETSCALETEXT,MPFROMSHORT(15),"1024");
			WinSendMsg(h,SLM_SETSLIDERINFO,
				MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),
				(MPARAM)(clipsize/(64*1024l)-1));
			switch (colorbits)
			{	case 4 :
					h=WinWindowFromID(hwnd,ID_4Bits); break;
				case 24 :
					h=WinWindowFromID(hwnd,ID_24Bits); break;
				default :
					h=WinWindowFromID(hwnd,ID_8Bits); break;
			}
			WinSendMsg(h,BM_CLICK,(MPARAM)1,NULL);
			if (copy_usewindow)
			{	copy_xsize=0; copy_ysize=0;
			}
			h=WinWindowFromID(hwnd,ID_SizesWidth);
			sprintf(sw,"%d",copy_xsize);
			WinSetWindowText(h,sw);
			h=WinWindowFromID(hwnd,ID_SizesHeight);
			sprintf(sh,"%d",copy_ysize);
			WinSetWindowText(h,sh);
			break;
		case WM_COMMAND :
			switch(SHORT1FROMMP(mp1))
			{	case DID_OK :
				case DID_SAVE :
					h=WinWindowFromID(hwnd,ID_Stacksize);
					stacksize=((int)WinSendMsg(h,SLM_QUERYSLIDERINFO,
						MPFROM2SHORT(SMA_SLIDERARMPOSITION,
							SMA_INCREMENTVALUE),NULL)+1)*1024l*1024;
					h=WinWindowFromID(hwnd,ID_ClipBuffer);
					clipsize=((int)WinSendMsg(h,SLM_QUERYSLIDERINFO,
						MPFROM2SHORT(SMA_SLIDERARMPOSITION,
							SMA_INCREMENTVALUE),NULL)+1)*1024l*64;
					h=WinWindowFromID(hwnd,ID_TextBuffer);
					textsize=((int)WinSendMsg(h,SLM_QUERYSLIDERINFO,
						MPFROM2SHORT(SMA_SLIDERARMPOSITION,
							SMA_INCREMENTVALUE),NULL)+1)*1024l*64;
									case DID_CANCEL :
					if (clicked(hwnd,ID_4Bits)) colorbits=4;
					else if (clicked(hwnd,ID_8Bits)) colorbits=8;
					else if (clicked(hwnd,ID_24Bits)) colorbits=24;
					h=WinWindowFromID(hwnd,ID_SizesWidth);
					WinQueryWindowText(h,16,sw);
					copy_xsize=atol(sw);
					h=WinWindowFromID(hwnd,ID_SizesHeight);
					WinQueryWindowText(h,16,sh);
					copy_ysize=atol(sh);
					if (copy_xsize<10 || copy_ysize<10)
						copy_usewindow=1;
					else
						copy_usewindow=0;
					WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			}
			break;
		case WM_HELP :
			show_help(832);
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

void set_sizes (void)
{   int ret;
	char *p;
	long oldstacksize=stacksize,
		oldtextsize=textsize,oldclipsize=clipsize;
	ret=WinDlgBox(HWND_DESKTOP,TextHandle,
		sizesproc,0,DLG_Sizes,0);
	if (ret==DID_CANCEL) return;
	if (stacksize!=oldstacksize)
		message("Stack size change will be effective\n"
			"at your next EULER session.\n"
			"And only, if settings are saved!");
	if (ret==DID_SAVE) save_size_config();
	if (oldclipsize<clipsize)
	{	p=(char *)malloc(clipsize);
		if (p)
		{   memmove(p,clipbuf,oldclipsize);
			if (pasting) pclip=p+(pclip-clipbuf);
			free(clipbuf);
			clipbuf=p;
			clipend=clipbuf+clipsize;
		}
	}
	if (oldtextsize<textsize)
	{	p=(char *)malloc(textsize);
		if (p)
		{   memmove(p,textarea,oldtextsize);
			textpos=p+(textpos-textarea);
			textarea=p;
			textareaend=p+textsize;
			cursorvisible=0;
			update_text();
			free(textarea);
		}
	}
}

MRESULT EXPENTRY graphicsproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{   HWND h;
	int i;
	switch (msg)
	{   case WM_INITDLG :
			switch (fillmode)
			{	case fpalette :
					h=WinWindowFromID(hwnd,ID_Palette);
					break;
				case flogcolor :
					h=WinWindowFromID(hwnd,ID_Logcolor);
					break;
				default :
					h=WinWindowFromID(hwnd,ID_Patterns);
			}
			WinSendMsg(h,BM_CLICK,(MPARAM)1,NULL);
			h=WinWindowFromID(hwnd,ID_Pure);
			WinSendMsg(h,BM_SETCHECK,
				(MPARAM)(pure?1:0),NULL);
			h=WinWindowFromID(hwnd,ID_FG);
			for (i=0; i<16; i++)
			{   WinSendMsg(h,LM_INSERTITEM,
					MPFROMSHORT(LIT_END),colornames[i]);
			}
			WinSendMsg(h,LM_SELECTITEM,MPFROMSHORT(fillcolor1),
				MPFROMSHORT(TRUE));
			h=WinWindowFromID(hwnd,ID_BG);
			for (i=0; i<16; i++)
			{   WinSendMsg(h,LM_INSERTITEM,
					MPFROMSHORT(LIT_END),colornames[i]);
			}
			WinSendMsg(h,LM_SELECTITEM,MPFROMSHORT(fillcolor2),
				MPFROMSHORT(TRUE));
			h=WinWindowFromID(hwnd,ID_Grid);
			for (i=0; i<16; i++)
			{   WinSendMsg(h,LM_INSERTITEM,
					MPFROMSHORT(LIT_END),colornames[i]);
			}
			WinSendMsg(h,LM_SELECTITEM,MPFROMSHORT(dlinecolor),
				MPFROMSHORT(TRUE));
			h=WinWindowFromID(hwnd,ID_Linestyles);
			WinSendMsg(h,BM_SETCHECK,
				(MPARAM)(uselinestyle?1:0),NULL);
			h=WinWindowFromID(hwnd,ID_UseMarkers);
			WinSendMsg(h,BM_SETCHECK,
				(MPARAM)(usemarkers?1:0),NULL);
			break;
		case WM_COMMAND :
			switch(SHORT1FROMMP(mp1))
			{	case DID_OK :
				case DID_SAVE :
					if (clicked(hwnd,ID_Palette)) fillmode=fpalette;
					if (clicked(hwnd,ID_Logcolor)) fillmode=flogcolor;
					if (clicked(hwnd,ID_Patterns)) fillmode=fdither;
					pure=clicked(hwnd,ID_Pure);
					uselinestyle=clicked(hwnd,ID_Linestyles);
					fillcolor1=getchoice(hwnd,ID_FG,colornames,16);
					fillcolor2=getchoice(hwnd,ID_BG,colornames,16);
					dlinecolor=getchoice(hwnd,ID_Grid,colornames,16);
				case DID_CANCEL :
					WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			}
			break;
		case WM_HELP :
			show_help(833);
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

void set_graphics (void)
{	int ret;
	ret=WinDlgBox(HWND_DESKTOP,TextHandle,
		graphicsproc,0,DLG_Graphics,0);
	if (ret==DID_CANCEL) return;
	if (bitmapvalid)
	{	delete_bitmap();
		bit=bitmapvalid=0;
	}
	WinInvalidateRect(GrafHandle,NULL,FALSE);
	if (ret==DID_SAVE) save_graphics_config();
}

MRESULT EXPENTRY productproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{   HWND h;
	static char version[32];
	switch (msg)
	{   case WM_INITDLG :
			h=WinWindowFromID(hwnd,ID_Version);
			sprintf(version,"Version %s",VERSION);
			WinSetWindowText(h,version);
			break;
		case WM_COMMAND :
			switch(SHORT1FROMMP(mp1))
			{	case DID_OK :
					WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			}
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

void product (void)
{	WinDlgBox(HWND_DESKTOP,TextHandle,
		productproc,0,DLG_Information,0);
}

MRESULT EXPENTRY printproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{	PPRQINFO3 prqQueue=(PPRQINFO3)pQueue;
	HWND h;
	int z,sel,k,i;
	switch (msg)
	{   case WM_INITDLG :
			h=WinWindowFromID(hwnd,ID_Queue);
			WinSendMsg(h,
				LM_INSERTITEM,MPFROMSHORT(LIT_END),"Default");
			for (sel=0, k=1, z=0; z<cReturnedQueue; z++, k++)
			{   WinSendMsg(h,LM_INSERTITEM,
					MPFROMSHORT(LIT_END),prqQueue->pszName);
				if (!strcmp(prqQueue->pszName,queuename))
					sel=k;
				prqQueue++;
			}
			WinSendMsg(h,LM_SELECTITEM,MPFROMSHORT(sel),
				MPFROMSHORT(TRUE));
			/* set slider */
			h=WinWindowFromID(hwnd,ID_Printsize);
			for (i=0; i<=80; i+=10)
				WinSendMsg(h,SLM_SETTICKSIZE,
					MPFROM2SHORT(i,(i==30||i==80)?5:3),NULL);
			WinSendMsg(h,SLM_SETSLIDERINFO,
				MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),
				MPFROMSHORT(pcsize-20));
			h=WinWindowFromID(hwnd,ID_AlignHor);
			for (i=0; i<=100; i+=10)
				WinSendMsg(h,SLM_SETTICKSIZE,
					MPFROM2SHORT(i,(i==0||i==50||i==100)?5:3),NULL);
			WinSendMsg(h,SLM_SETSLIDERINFO,
				MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),
				MPFROMSHORT(pcalignhor));
			h=WinWindowFromID(hwnd,ID_AlignVer);
			for (i=0; i<=100; i+=10)
				WinSendMsg(h,SLM_SETTICKSIZE,
					MPFROM2SHORT(i,(i==0||i==50||i==100)?5:3),NULL);
			WinSendMsg(h,SLM_SETSLIDERINFO,
				MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_INCREMENTVALUE),
				MPFROMSHORT(100-pcalignver));
			/* set readio bottons */
			if (printcolor) h=WinWindowFromID(hwnd,ID_Color);
			else
			{	if (printgw) h=WinWindowFromID(hwnd,ID_GW);
				else h=WinWindowFromID(hwnd,ID_WW);
			}
			WinSendMsg(h,BM_CLICK,(MPARAM)1,NULL);
			h=WinWindowFromID(hwnd,ID_Clip);
			WinSendMsg(h,BM_SETCHECK,(MPARAM)(printclip?1:0),NULL);
			h=WinWindowFromID(hwnd,ID_ColorText);
			WinSendMsg(h,BM_SETCHECK,(MPARAM)(colortext?1:0),NULL);
			h=WinWindowFromID(hwnd,ID_ColorLines);
			WinSendMsg(h,BM_SETCHECK,(MPARAM)(colorlines?1:0),NULL);
			h=WinWindowFromID(hwnd,ID_ColorMarker);
			WinSendMsg(h,BM_SETCHECK,(MPARAM)(colormarker?1:0),NULL);
			break;
		case WM_COMMAND :
			switch(SHORT1FROMMP(mp1))
			{	case DID_OK :
				case DID_SAVE :
					h=WinWindowFromID(hwnd,ID_Queue);
					WinQueryWindowText(h,254,queuename);
					if (clicked(hwnd,ID_WW))
					{	printcolor=0; printgw=0;
					}
					else if (clicked(hwnd,ID_GW))
					{	printcolor=0; printgw=1;
					}
					else
					{	printcolor=1;
					}
					h=WinWindowFromID(hwnd,ID_Printsize);
					pcsize=(int)WinSendMsg(h,SLM_QUERYSLIDERINFO,
						MPFROM2SHORT(SMA_SLIDERARMPOSITION,
							SMA_INCREMENTVALUE),NULL)+20;
					h=WinWindowFromID(hwnd,ID_AlignHor);
					pcalignhor=(int)WinSendMsg(h,SLM_QUERYSLIDERINFO,
						MPFROM2SHORT(SMA_SLIDERARMPOSITION,
							SMA_INCREMENTVALUE),NULL);
					h=WinWindowFromID(hwnd,ID_AlignVer);
					pcalignver=100-
						(int)WinSendMsg(h,SLM_QUERYSLIDERINFO,
						MPFROM2SHORT(SMA_SLIDERARMPOSITION,
							SMA_INCREMENTVALUE),NULL);
					colortext=clicked(hwnd,ID_ColorText);
					colorlines=clicked(hwnd,ID_ColorLines);
					colormarker=clicked(hwnd,ID_ColorMarker);
					printclip=clicked(hwnd,ID_Clip);
				case DID_CANCEL :
					WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			}
			break;
		case WM_HELP :
			show_help(834);
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

void set_print (void)
{   int ret;
	initstd();
	ret=WinDlgBox(HWND_DESKTOP,TextHandle,
		printproc,0,DLG_Print,0);
	exitstd();
	if (ret==DID_SAVE) save_print_config();
}

#define maxedit 8192
char editbuf[maxedit],editsave[maxedit];
char editsavepath[512]="";
int isel;
int printudf (char *name, char *buffer, long size);

MRESULT EXPENTRY editproc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{	HWND h;
	header *hd;
	long i;
	char name[256],*savename;
	FILE *f;
	switch (msg)
	{   case WM_INITDLG :
			if (editsave) memmove(editbuf,editsave,maxedit);
			h=WinWindowFromID(hwnd,ID_Function);
			hd=(header *)ramstart;
			WinSendMsg(h,LM_INSERTITEM,MPFROMSHORT(LIT_END),
				"*** Your text ***");
			while (hd->type==s_udf && hd<(header *)udfend)
			{   WinSendMsg(h,LM_INSERTITEM,
					MPFROMSHORT(LIT_END),hd->name);
				hd=nextof(hd);
			}
			WinSendMsg(h,LM_SELECTITEM,MPFROMSHORT(0),MPFROMSHORT(1));
			h=WinWindowFromID(hwnd,ID_Edit);
			WinSendMsg(h,MLM_SETTEXTLIMIT,(void *)maxedit,NULL);
			WinSetWindowText(h,editbuf);
			isel=0;
			break;
		case WM_CONTROL :
			switch(SHORT1FROMMP(mp1))
			{   case ID_Function :
					if (SHORT2FROMMP(mp1)==CBN_LBSELECT)
					{   h=WinWindowFromID(hwnd,ID_Function);
						i=(long)WinSendMsg(h,LM_QUERYSELECTION,
												NULL,NULL);
						if (i>0 && isel==0)
							memmove(editsave,editbuf,maxedit);
						if (i==0 && isel>0)
						{	memmove(editbuf,editsave,maxedit);
							h=WinWindowFromID(hwnd,ID_Edit);
							WinSetWindowText(h,editbuf);
						}
						isel=i;
						if (i==0) break;
						WinSendMsg(h,LM_QUERYITEMTEXT,
							MPFROM2SHORT(i,maxedit),name);
						h=WinWindowFromID(hwnd,ID_Edit);
						if (!printudf(name,editbuf,maxedit))
						{	warning("Cannot display this function!");
							break;
						}
						WinSetWindowText(h,editbuf);
					}
					break;
			}
			break;
		case WM_COMMAND :
			switch(SHORT1FROMMP(mp1))
			{   case ID_Save :
					h=WinWindowFromID(hwnd,ID_Edit);
					WinQueryWindowText(h,maxedit,editbuf);
					savename=select_file("*.e",1,"Save Text",
						"Save",".e",editsavepath);
					if (!savename || !*savename) break;
					f=fopen(savename,"wb");
					if (!f) goto saveerror;
					fwrite(editbuf,1,strlen(editbuf),f);
					fclose(f);
					if (ferror(f))
					{	saveerror :
						warning("Unable to write file!");
					}
					break;
				case ID_FLoad :
					savename=select_file("*.e",0,"Load Text",
						"Load",".e",editsavepath);
					if (!savename || !*savename) break;
					f=fopen(savename,"rb");
					if (!f) goto readerror;
					i=fread(editbuf,1,maxedit,f);
					if (i>=maxedit)
					{   fclose(f);
						goto readerror;
					}
					editbuf[i+1]=0;
					fclose(f);
					h=WinWindowFromID(hwnd,ID_Edit);
					WinSetWindowText(h,editbuf);
					if (ferror(f))
					{	readerror :
						warning("Unable to read file\nor file to big!");
					}
					break;
				case DID_OK :
				case ID_Interpret :
					h=WinWindowFromID(hwnd,ID_Edit);
					WinQueryWindowText(h,maxedit,editbuf);
					if (isel==0) memmove(editsave,editbuf,maxedit);
				case DID_CANCEL :
					finish :
					WinDismissDlg(hwnd,SHORT1FROMMP(mp1));
			}
			break;
		case WM_HELP :
			show_help(836);
			break;
		default :
			return WinDefDlgProc(hwnd,msg,mp1,mp2);
	}
	return (MRESULT)FALSE;
}

void edit_function (void)
{   int ret;
	FILE *f;
	ret=WinDlgBox(HWND_DESKTOP,TextHandle,
		editproc,0,DLG_Function,0);
	if (ret==ID_Interpret)
	{	f=fopen("aaaa$$$$.e","wb");
		if (!f)
		{	warning("Unable to write AAAA$$$$.E");
			return;
		}
		fwrite(editbuf,1,strlen(editbuf),f);
		fclose(f);
		if (pasting) return;
		strcpy(clipbuf,"\x1Bload \"aaaa$$$$.e\";\x0A");
		pclip=clipbuf; pasting=1;
		WinPostMsg(DummyHandle,WM_CHAR,0,0);
	}
}

/********* configuration save and read ***************/

void read_config (void)
{   char s[256];
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"fillmode","0",s,254);
	fillmode=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"purecolors","0",s,254);
	pure=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"graphicsfontname",FDEFAULT,fontname,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"graphiclines","20",s,254);
	graphiclines=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"printerfontname",FDEFAULT,printerfontname,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"textfontname",FDEFAULT,textfontname,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"textlines","20",s,254);
	dtextlines=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"stacksize","4096",s,254);
	stacksize=atol(s);
	if (stacksize<512l) stacksize=512l;
	stacksize*=1024l;
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"textsize","128",s,254);
	textsize=atol(s);
	if (textsize<64l) textsize=64l;
	textsize*=1024l;
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"clipsize","128",s,254);
	clipsize=atol(s);
	if (clipsize<64l) clipsize=64l;
	clipsize*=1024l;
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"uselinestyle","0",s,254);
	uselinestyle=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"usemarkers","0",s,254);
	usemarkers=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"pcsize","90",s,254);
	pcsize=atoi(s);
	if (pcsize<20) pcsize=20;
	if (pcsize>100) pcsize=100;
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"pcalignhor","50",s,254);
	pcalignhor=atoi(s);
	if (pcalignhor<0) pcalignhor=0;
	if (pcalignhor>100) pcalignhor=100;
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"pcalignver","50",s,254);
	pcalignver=atoi(s);
	if (pcalignver<0) pcalignver=0;
	if (pcalignver>100) pcalignver=100;
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"queuename","Default",queuename,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"printcolor","0",s,254);
	printcolor=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"printgw","1",s,254);
	printgw=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"savebits","8",s,254);
	colorbits=atoi(s);
	if (colorbits!=4 && colorbits!=8 && colorbits!=24)
		colorbits=8;
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"printclip","1",s,254);
	printclip=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"colortext","0",s,254);
	colortext=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"colorlines","0",s,254);
	colorlines=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"colormarker","0",s,254);
	colormarker=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"fillcolor1","3",s,254);
	fillcolor1=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"fillcolor2","12",s,254);
	fillcolor2=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"dlinecolor","1",s,254);
	dlinecolor=atoi(s);
	if (!startpath[0])
		PrfQueryProfileString(HINI_USERPROFILE,"euler",
			"startpath","",startpath,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"textpath","",textpath,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"graphicspath","",graphicspath,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"loadpath","",loadpath,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"storepath","",storepath,254);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"notebookpath","",notebookpath,254);
	/* Read Window sizes */
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"xtext","-1",s,254);
	xtextpos=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"ytext","-1",s,254);
	ytextpos=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"htext","-1",s,254);
	htextpos=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"wtext","-1",s,254);
	wtextpos=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"xgraf","-1",s,254);
	xgrafpos=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"ygraf","-1",s,254);
	ygrafpos=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"hgraf","-1",s,254);
	hgrafpos=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"wgraf","-1",s,254);
	wgrafpos=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"matherrors","1",s,254);
	matherrors=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"notebookcolors","1",s,254);
	notebookcol=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"idletime","0",s,254);
	idletime=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"insertcommands","0",s,254);
	insertcommands=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"copyusewindow","1",s,254);
	copy_usewindow=atoi(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"copyxsize","0",s,254);
	copy_xsize=atol(s);
	PrfQueryProfileString(HINI_USERPROFILE,"euler",
		"copyysize","0",s,254);
	copy_ysize=atol(s);
}

void save_windows (void)
{	char t[256];
	SWP swp;
	WinQueryWindowPos(TextFrameHandle,&swp);
	if (swp.fl & SWP_MINIMIZE)
	{	swp.x=swp.y=0;
		swp.cx=xtext; swp.cy=ytext;
	}
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"wtext",itoa(swp.cx,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"htext",itoa(swp.cy,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"xtext",itoa(swp.x,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"ytext",itoa(swp.y,t,10));
	WinQueryWindowPos(GrafFrameHandle,&swp);
	if (swp.fl & SWP_MINIMIZE)
	{	swp.x=swp.y=0;
		swp.cx=xsize; swp.cy=ysize;
	}
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"wgraf",itoa(swp.cx,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"hgraf",itoa(swp.cy,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"xgraf",itoa(swp.x,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"ygraf",itoa(swp.y,t,10));
}

void save_font_config (void)
{	char t[256];
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"graphicsfontname",fontname);
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"graphiclines",itoa(graphiclines,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"textfontname",textfontname);
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"textlines",itoa(dtextlines,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"printerfontname",printerfontname);
}

void save_print_config (void)
{	char t[256];
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"queuename",queuename);
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"pcsize",itoa(pcsize,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"pcalignhor",itoa(pcalignhor,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"pcalignver",itoa(pcalignver,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"printcolor",itoa(printcolor,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"printgw",itoa(printgw,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"printclip",itoa(printclip,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"colortext",itoa(colortext,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"colorlines",itoa(colorlines,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"colormarker",itoa(colormarker,t,10));
}

void save_graphics_config (void)
{	char t[256];
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"fillmode",itoa(fillmode,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"purecolors",itoa(pure,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"uselinestyle",itoa(uselinestyle,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"usemarkers",itoa(usemarkers,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"fillcolor1",itoa(fillcolor1,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"fillcolor2",itoa(fillcolor2,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"dlinecolor",itoa(dlinecolor,t,10));
}

void save_size_config (void)
{	char t[256];
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"stacksize",ltoa(stacksize/1024l,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"textsize",ltoa(textsize/1024l,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"clipsize",ltoa(clipsize/1024l,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"savebits",itoa(colorbits,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"copyusewindow",itoa(copy_usewindow,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"copyxsize",itoa(copy_xsize,t,10));
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"copyysize",itoa(copy_ysize,t,10));
}

void save_path_config (void)
{	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"startpath",startpath);
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"textpath",textpath);
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"loadpath",loadpath);
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"graphicspath",graphicspath);
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"storepath",storepath);
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"notebookpath",notebookpath);
}

void save_config (void)
{	save_font_config();
	save_graphics_config();
	save_print_config();
	save_size_config();
	save_path_config();
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"matherrors",matherrors?"1":"0");
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"notebookcolors",notebookcol?"1":"0");
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"insertcommands",insertcommands?"1":"0");
	PrfWriteProfileString(HINI_USERPROFILE,"euler",
		"idletime",idletime?"1":"0");
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
{   if (matherrors)
	{	output("Math error generated!\n");
		error=1000;
	}
	return 1;
}

MRESULT EXPENTRY DummyProc (HWND hwnd, ULONG msg,
	MPARAM mp1, MPARAM mp2)
{   switch (msg)
	{   case WM_CHAR :
			if (ptid!=(unsigned long)(-1)) DosWaitThread(&ptid,DCWW_WAIT);
			if (keyscan==switch_screen && textmode && waiting)
			{   graphic_mode();
				keydone=0;
			}
			break;
		case WM_TIMER :
			WinStopTimer(hab,DummyHandle,1);
			if (ptid!=(unsigned long)(-1)) DosWaitThread(&ptid,DCWW_WAIT);
			keydone=1;
			keyscan=0;
			keycode=0;
			break;
		default :
			return WinDefWindowProc(hwnd,msg,mp1,mp2);
	}
	return 0;
}

void destroy_dummy (void)
{	WinDestroyWindow(DummyHandle);
	WinDestroyMsgQueue(hmq1);
}

VOID mainproc (void)
{   ULONG flags=0;
	hmq1=WinCreateMsgQueue(hab,0);
	WinRegisterClass(hab,"Dummy",DummyProc,0,0);
	WinCreateStdWindow(HWND_OBJECT,0,&flags,"Dummy","Dummy",
		0,(HMODULE)0,1000,&DummyHandle);
	main_loop(nargc,nargv);
	destroy_dummy();
	WinPostMsg(TextHandle,WM_QUIT,0,0);
	_endthread();
}

void idle_time (void)
/* Toggle command insertion */
{	HWND handle=WinWindowFromID(TextFrameHandle,FID_MENU);
	idletime=!idletime;
	WinSendMsg(handle,MM_SETITEMATTR,
		MPFROM2SHORT(IDM_Idletime,TRUE),MPFROM2SHORT(MIA_CHECKED,
		idletime?MIA_CHECKED:0));
	if (idletime)
	{	DosSetPriority(PRTYS_THREAD,PRTYC_IDLETIME,0,tid);
	}
	else
	{	DosSetPriority(PRTYS_THREAD,PRTYC_REGULAR,0,tid);
	}
}

void main (int argc, char *argv[])
/******
Initialize memory and call main_loop
******/
{   int i,noprofile=0;
	DosCreateMutexSem(0,&drawsem,0,0);
	DosCreateMutexSem(0,&textsem,0,0);
	for (i=1; i<argc; i++)
	{	if (argv[i][0]=='-')
			switch (argv[i][1])
			{	case 'n' : /* do not read profile */
					noprofile=1;
					break;
				case 'p' :
					if (i==argc-1) break;
					strcpy(startpath,argv[i+1]);
					i++;
					break;
			}
		else break;
	}
	argc-=i-1; argv+=i-1;
	if (!noprofile) read_config();
	cd(startpath);
	if (!create_page()) goto error;
	if (!memory_init()) goto error;
	if (!window_init()) goto error;
	clear_graphics(); clear_screen();
	text_mode();
	WinSetFocus(HWND_DESKTOP,TextHandle);
	_control87(MCW_EM,MCW_EM);
		/* catch coprocessor interrupts */
	nargc=argc; nargv=argv;
	check();
	tid=_beginthread((void (_USERENTRY *)(void *))mainproc,
		256*1024l,0);
	if (tid==(unsigned long)(-1)) goto error;
	if (idletime)
	{	DosSetPriority(PRTYS_THREAD,PRTYC_IDLETIME,0,tid);
	}
	endbusy();
	/* check menu items */
	loadhelp("help.txt");
	while (WinGetMsg(hab,&qmsg,0,0,0))
		WinDispatchMsg(hab,&qmsg);
	destroy_dummy();
	window_exit();
	exit(0);
	error:
	warning("Severe error occured.\nInsufficient memory\n"
	"or Window error.\nCannot start EULER!");
	exit(1);
}

