#include "dlldef.h"
#include <string.h>

extern "C" char * _export fold (header * p[], int np,
	char *newram, char *ramend)
{   if (np!=2)
	{	strcpy(newram,"Need 2 Parameters");
		return newram;
	}
	if (p[0]->type!=s_matrix || p[1]->type!=s_matrix)
	{	strcpy(newram,"Need 2 vectors");
		return newram;
	}
	int n=dimsof(p[0])->c;
	if (dimsof(p[0])->r!=1 || dimsof(p[1])->r!=1 ||
		dimsof(p[1])->c!=n)
	{	strcpy(newram,"Need to vectors of same size");
		return newram;
	}
	header *hd=new_matrix(1,n,newram,ramend);
	if (!hd)
	{	strcpy(newram,"Out of memory");
		return newram;
	}
	int i,j,k;
	double x;
	for (i=0; i<n; i++)
	{   x=0; k=i;
		for (j=0; j<n; j++)
		{	x+=matrixof(p[0])[k]*matrixof(p[1])[j];
			k++; if (k>=n) k=0;
		}
		matrixof(hd)[i]=x;
	}
	new_real(2.3,newram,ramend);
	return newram;
}

