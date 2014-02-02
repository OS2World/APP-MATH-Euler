#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "header.h"
#include "builtin.h"

/*************** execute builtin functions ***********/

int builtin_count;

extern builtintyp builtin_list[];

void print_builtin (void)
{	int linel=0,i;
	for (i=0; i<builtin_count; i++)
	{	if (linel+strlen(builtin_list[i].name)+2>(size_t)linelength)
			{ output("\n"); linel=0; }
		output1("%s ",builtin_list[i].name);
		linel+=(int)strlen(builtin_list[i].name)+1;
	}
	output("\n");
}

int builtin_compare (const builtintyp *p1, const builtintyp *p2)
{	int h;
	h=strcmp(p1->name,p2->name);
	if (h) return h;
	else
	{	if (p1->nargs==-1 || p2->nargs==-1) return 0;
		else if (p1->nargs<p2->nargs) return -1;
		else if (p1->nargs>p2->nargs) return 1;
		else return 0;
	}
}

void sort_builtin (void)
{	builtin_count=0;
	while (builtin_list[builtin_count].name) builtin_count++;
	qsort(builtin_list,builtin_count,sizeof(builtintyp),
		(int (*) (const void *, const void *))builtin_compare);
}

int exec_builtin (char *name, int nargs, header *hd)
{	builtintyp *b,h;
	h.name=name; h.nargs=nargs;
	b=(builtintyp *)bsearch(&h,builtin_list,builtin_count,sizeof(builtintyp),
		(int (*) (const void *, const void *))builtin_compare);
	if (b)
	{	if (nargs==0) hd=0;
		b->f(hd); return 1;
	}
	else return 0;
}

builtintyp *find_builtin (char *name)
{	builtintyp h;
	h.name=name; h.nargs=-1;
	return (builtintyp *)bsearch(&h,builtin_list,builtin_count,sizeof(builtintyp),
		(int (*) (const void *, const void *))builtin_compare);
}

