#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "header.h"
#include "store.h"

typedef struct { size_t udfend,startlocal,endlocal,newram; }
	ptyp;

void mstore (header *hd)
{	FILE *file;
	ptyp p;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string)
	{	output("Expect file name.\n");
		error=1100; return;
	}
	p.udfend=udfend-ramstart;
	p.startlocal=startlocal-ramstart;
	p.endlocal=endlocal-ramstart;
	p.newram=newram-ramstart;
	file=fopen(stringof(hd),"wb");
	if (!file)
	{	output1("Could not open %s.\n",stringof(hd));
		error=1101; return;
	}
	fwrite(&p,sizeof(ptyp),1,file);
	fwrite(ramstart,1,newram-ramstart,file);
	if (ferror(file))
	{	output("Write error.\n");
		error=1102; return;
	}
	fclose(file);
}

void mrestore (header *hd)
{	FILE *file;
	ptyp p;
	if (udfon)
	{	output("Cannot restore inside a UDF.\n");
		error=1; return;
	}
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string)
	{	output("Expect file name.\n");
		error=1100; return;
	}
	file=fopen(stringof(hd),"rb");
	if (!file)
	{	output1("Could not open %s.\n",stringof(hd));
		error=1103; return;
	}
	fread(&p,sizeof(ptyp),1,file);
	if (ferror(file))
	{	output("Read error.\n");
		error=1104; return;
	}
	fread(ramstart,1,p.newram,file);
	if (ferror(file))
	{	output("Read error (fatal for EULER).\n");
		error=1104; return;
	}
	fclose(file);
	udfend=ramstart+p.udfend;
	startlocal=ramstart+p.startlocal;
	endlocal=ramstart+p.endlocal;
	newram=ramstart+p.newram;
	next=input_line; *next=0;
}


