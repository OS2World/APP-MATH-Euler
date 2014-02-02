#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "header.h"
#include "sysdep.h"

/* Output to the text window via gprint in sysdep*.c */

int preventoutput=0;

void output (char *s)
{	if (preventoutput) return;
	if (outputbuffer)
	{   if (outputbuffererror) return;
		if (outputbuffer+strlen(s)>=outputbufferend)
		{	outputbuffererror=1; return;
		}
		strcpy(outputbuffer,s);
		outputbuffer+=strlen(s);
		return;
	}
	text_mode();
	if (outputing || error) gprint(s);
	if (outfile)
	{	fprintf(outfile,"%s",s);
		if (ferror(outfile))
		{	output("Error on dump file (disk full?).\n");
			error=200;
			fclose(outfile); outfile=0;
		}
	}
}

void output1 (char *s, ...)
{	char text [1024];
	va_list v;
	if (preventoutput) return;
	text_mode();
	va_start(v,s);
	vsprintf(text,s,v);
	if (outputbuffer)
	{	output(text); return;
	}
	if (outputing || error) gprint(text);
	if (outfile)
	{   fprintf(outfile,text);
		if (ferror(outfile))
		{	output("Error on dump file (disk full?).\n");
			error=200;
			fclose(outfile); outfile=0;
		}
	}
}

void output1hold (int f, char *s, ...)
{	static char text [1024];
	ULONG si;
	va_list v;
	if (f==0) text[0]=0;
	text_mode();
	va_start(v,s);
	vsprintf(text+strlen(text),s,v);
	if (f<=0) return;
	si=strlen(text);
	if (si<(ULONG)f)
	{	memmove(text+(f-si),text,si+1);
		memset(text,' ',f-si);
	}
	if (outputbuffer)
	{	output(text); return;
	}
	if (outputing || error) gprint(text);
	if (outfile)
	{   fprintf(outfile,text);
		if (ferror(outfile))
		{	output("Error on dump file (disk full?).\n");
			error=200;
			fclose(outfile); outfile=0;
		}
	}
}

/* Print an error message */

void print_error (char *p)
{	int i;
	char *q,outline[1024];
	double x;
	int comn;
	if (errorout) return;
	if (input_line<=p && input_line+1024>p)
	{	output1("error in:\n%s\n",input_line);
		if ((int)(p-input_line)<linelength-2)
			for (i=0; i<(int)(p-input_line); i++) output(" ");
		output("^\n");
	}
	else if (udfon)
	{	q=outline; p=udfline;
		while (*p)
		{	if (*p==2)
			{	p++; memmove((char *)(&x),p,sizeof(double));
				p+=sizeof(double);
				sprintf(q,"%g",x);
				q+=strlen(q);
			}
			else if (*p==3)
			{	p++;
				memmove((char *)(&comn),p,sizeof(int));
				p+=sizeof(int);
				sprintf(q,"%s",command_list[comn].name);
				q+=strlen(q);
			}
			else *q++=*p++;
			if (q>outline+1022)
			{	q=outline+1023;
				break;
			}
		}
		*q=0;
		output1("Error in :\n%s\n",outline); output("\n");
	}
	errorout=1;
}


