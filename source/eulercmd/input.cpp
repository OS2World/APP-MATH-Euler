#include <stdio.h>

#include "header.h"
#include "sysdep.h"

extern int trace,nojump,booktype,promptnotebook;

void read_line (char *line)
{	int count=0,input,scan;
	char *p;
	start :
	p=line;
	while(1)
	{	input=getc(infile);
		if (input==EOF)
		{	fclose(infile);
			if (p>line) break;
			else *p++=1;
			infile=0;
			break;
		}
		if (input=='\n') break;
		if (count>=1023)
		{	output("Line too long!\n"); error=50; *line=0; return;
		}
		if ((char)input>=' ' || (signed char)input<0
			|| (char)input==TAB)
		{	*p++=(char)input; count++;
		}
	}
	*p=0;
	if (booktype)
	{	switch (*line)
		{   case '$' :
			case '>' :
				output1("%s\n",line);
				if (promptnotebook && line[1]) wait_key(&scan);
				if (scan==escape)
				{	fclose(infile); infile=0; *line=0;
					break;
				}
				*line=' '; break;
			case '%' :
				output1("%s\n",line);
				goto start;
			default :
				if (!infile) break;
				goto start;
		}
	}
}

void next_line (void)
/**** next_line
	read a line from keyboard or file.
****/
{	if (udfon)
	{	while (*next) next++;
		next++;
		if (*next==1) udfon=0; else udfline=next;
		if (trace>0) trace_udfline(next);
		return;
	}
	else
	{	if (trace==-1) trace=1;
		if (stringon)
		{	error=2300; output("Input ended in string!\n");
			return;
		}
		nojump=0;
		if (!infile) edit(input_line);
		else read_line(input_line);
		next=input_line;
	}
}

