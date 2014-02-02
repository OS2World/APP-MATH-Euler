#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "help.h"

typedef struct _helpline
{	struct _helpline *next;
	char *text;
} helpline;

typedef struct _helpitem
{	struct _helpitem *next;
	helpline *help;
	char *item;
} helpitem;

static helpitem *firstitem=0;

char * newline (FILE *in)
{   static char line[1024];
	int l;
	if (feof(in)) return 0;
	if (!fgets(line,1022,in)) return 0;
	if (feof(in)) return 0;
	l=(int)strlen(line);
	if (l>0 && line[l-1]=='\n') line[l-1]=0;
	return line;
}

helpitem * loaditem (FILE *in)
{	char *l;
	helpline *h,*hn,*hend=0;
	helpitem *hin;
	while (1)
	{	l=newline(in);
		if (!l) return 0;
		if (*l=='!') break;
	}
	hin=(helpitem *)malloc(sizeof(helpitem));
	hin->next=0;
	hin->item=(char *)malloc(strlen(l+1)+1);
	strcpy(hin->item,l+1);
	h=0;
	while (1)
	{   l=newline(in);
		if (!l || *l=='#') break;
		hn=(helpline *)malloc(sizeof(helpline));
		hn->next=0;
		hn->text=(char *)malloc(strlen(l)+1);
		strcpy(hn->text,l);
		if (!h) h=hend=hn;
		else { hend->next=hn; hend=hn; }
	}
    hin->help=h;
	return hin;
}

void loadhelp (char *filename)
{	FILE *in=fopen(filename,"r");
	helpitem *lastitem=0,*newitem;
	if (!in) return;
	firstitem=0;
	while (1)
	{	newitem=loaditem(in);
		if (!newitem) break;
		if (!firstitem)
		{	lastitem=firstitem=newitem;
		}
		else
		{	lastitem->next=newitem;
			lastitem=newitem;
		}
	}
	fclose(in);
}

void unloadhelp ()
{	helpitem *h=firstitem,*hn;
	helpline *l,*ln;
	while (h)
	{	free(h->item);
		l=h->help;
		while (l)
		{	free(l->text);
			ln=l->next;
			free(l);
			l=ln;
		}
		hn=h->next;
		free(h);
		h=hn;
	}
}

void externhelp (char *text)
{   helpitem *h=firstitem;
	helpline *hl;
	char s[256],*p,*q;
	while (h)
	{	p=h->item;
		while(*p)
		{   q=s;
			while (*p && *p!=',') *q++=*p++;
			*q=0;
			if (strcmp(s,text)==0)
			{   hl=h->help;
            	output1("\n");
				while (hl)
				{	output1("%s\n",hl->text);
					hl=hl->next;
				}
				output1("\n");
				return;
			}
			if (*p==0) break;
			p++;
		}
		h=h->next;
	}
}

