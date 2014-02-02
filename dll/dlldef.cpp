#include "dlldef.h"

#include <string.h>
#include <windows.h>

int FAR PASCAL _export DllEntryPoint (HINSTANCE hInstance,
	DWORD reason, LPVOID res)
{	return TRUE;
}

header *make_header (int type, int size, char * &newram,
	char *ramend)
{	header *hd=(header *)newram;
	if (newram+size>ramend) return 0;
	hd->size=size;
	hd->type=type;
	hd->flags=0;
	*(hd->name)=0;
	hd->xor=0;
	newram+=size;
	return hd;
}

header *new_string (char *s,
	char * &newram, char *ramend)
{	int size=sizeof(header)+(strlen(s)/2+1)*2;
	header *hd=make_header(s_string,size,newram,ramend);
	if (hd) strcpy(stringof(hd),s);
	return hd;
}

header *new_real (double x,
	char * &newram, char *ramend)
{	int size=sizeof(header)+sizeof(double);
	header *hd=make_header(s_real,size,newram,ramend);
	if (hd) *realof(hd)=x;
	return hd;
}

header *new_complex (double x, double y,
	char * &newram, char *ramend)
{	int size=sizeof(header)+2*sizeof(double);
	header *hd=make_header(s_complex,size,newram,ramend);
	if (hd)
	{	realof(hd)[1]=x;
		realof(hd)[2]=y;
	}
	return hd;
}

header *new_interval (double a, double b,
	char * &newram, char *ramend)
{	int size=sizeof(header)+2*sizeof(double);
	header *hd=make_header(s_interval,size,newram,ramend);
	if (hd)
	{	realof(hd)[1]=a;
		realof(hd)[2]=b;
	}
	return hd;
}

header *new_matrix (int rows, int columns,
	char * &newram, char *ramend)
{	int size=sizeof(header)+sizeof(dims)+
		rows*columns*sizeof(double);
	header *hd=make_header(s_matrix,size,newram,ramend);
	if (hd)
	{	dimsof(hd)->c=columns;
		dimsof(hd)->r=rows;
	}
	return hd;
}

header *new_cmatrix (int rows, int columns,
	char * &newram, char *ramend)
{	int size=sizeof(header)+sizeof(dims)+
		2*rows*columns*sizeof(double);
	header *hd=make_header(s_cmatrix,size,newram,ramend);
	if (hd)
	{	dimsof(hd)->c=columns;
		dimsof(hd)->r=rows;
	}
	return hd;
}

header *new_imatrix (int rows, int columns,
	char * &newram, char *ramend)
{	int size=sizeof(header)+sizeof(dims)+
		2*rows*columns*sizeof(double);
	header *hd=make_header(s_imatrix,size,newram,ramend);
	if (hd)
	{	dimsof(hd)->c=columns;
		dimsof(hd)->r=rows;
	}
	return hd;
}

extern "C" char * _export TEST (header * p[], int np,
	char *newram, char *ramend)
{
	return newram;
}

