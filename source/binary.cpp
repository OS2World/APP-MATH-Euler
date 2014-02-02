#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

#include "header.h"
#include "stack.h"

#define wrong_arg() { error=26; output("Wrong argument\n"); return; }
#define wrong_arg_in(x) { error=26; output1("Wrong arguments for %s\n",x); return; }

#define no_file() { error=800; output("No file open\n"); return; }

FILE *fa=0;

void mopen (header *hd)
{   header *st=hd,*hd1,*result;
	if (fa) fclose(fa);
	hd1=nextof(hd);
	hd=getvalue(hd); if (error) return;
	hd1=getvalue(hd1); if (error) return;
	if (hd->type!=s_string || hd1->type!=s_string) wrong_arg_in("open");
	fa=fopen(stringof(hd),stringof(hd1));
	if (!fa)
	{	error=1;
		output("Could not open the file!\n");
		return;
	}
	result=new_real((double)ferror(fa),""); if (error) return;
	moveresult(st,result);
}

void mclose (header *hd)
{	new_real(fa!=0?(double)ferror(fa):-1,""); if (error) return;
	if (fa) fclose(fa); fa=0;
}

void mwrite (header *hd)
{   header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("write");
	if (!fa) no_file();
	fprintf(fa,"%s",stringof(hd));
	result=new_real(ferror(fa),""); if (error) return;
	moveresult(st,result);
}

void mputuchar (header *hd)
{   header *st=hd,*result;
	int n,i;
	unsigned char *p,*start=(unsigned char *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real && hd->type!=s_matrix)
		wrong_arg_in("putchar(v)");
	getmatrix(hd,&i,&n,&m);
	if (i!=1 || n<1) wrong_arg_in("putchar(v)");
	if (fa)
	{	if (!freeramfrom(start,n))
		{	output("Stack overflow in getchar(n).");
			error=1; return;
		}
		for (p=start,i=0; i<n; i++) *p++=(unsigned char)*m++;
		fwrite(start,1,n,fa);
	}
	else no_file();
	result=new_real(ferror(fa),""); if (error) return;
	moveresult(st,result);
}

void mputuword (header *hd)
{	header *st=hd,*result;
	int n,i;
	unsigned short *p,*start=(unsigned short *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real && hd->type!=s_matrix)
		wrong_arg_in("putchar(v)");
	getmatrix(hd,&i,&n,&m);
	if (i!=1 || n<1) wrong_arg_in("putchar(v)");
	if (fa)
	{	if (!freeramfrom(start,n*sizeof(unsigned short)))
		{	output("Stack overflow in getchar(n).");
			error=1; return;
		}
		for (p=start,i=0; i<n; i++) *p++=(unsigned short)*m++;
		fwrite(start,sizeof(unsigned short),n,fa);
	}
	else no_file();
	result=new_real(ferror(fa),""); if (error) return;
	moveresult(st,result);
}

void mputulongword (header *hd)
{	header *st=hd,*result;
	int n,i;
	unsigned long *p,*start=(unsigned long *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real && hd->type!=s_matrix)
		wrong_arg_in("putchar(v)");
	getmatrix(hd,&i,&n,&m);
	if (i!=1 || n<1) wrong_arg_in("putchar(v)");
	if (fa)
	{	if (!freeramfrom(start,n*sizeof(unsigned long)))
		{	output("Stack overflow in getchar(n).");
			error=1; return;
		}
		for (p=start,i=0; i<n; i++) *p++=(unsigned long)*m++;
		fwrite(start,sizeof(unsigned long),n,fa);
	}
	else no_file();
	result=new_real(ferror(fa),""); if (error) return;
	moveresult(st,result);
}

void mgetuchar (header *hd)
{	new_real(fa?getc(fa):-1,"");
}

void mgetuchar1 (header *hd)
{   header *st=hd,*result;
	long n,count=0;
	unsigned char *start=(unsigned char *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (!hd->type==s_real) wrong_arg_in("getchar");
	n=(long)*realof(hd);
	if (n<=0) wrong_arg_in("getchar");
	if (fa)
	{	if (!freeramfrom(start,n))
		{	output("Stack overflow in getchar(n).");
			error=1; return;
		}
		newram+=n;
		count=fread(start,1,n,fa);
	}
	else no_file();
	result=new_matrix(1,count,""); if (error) return;
	m=matrixof(result);
	for (n=0; n<count; n++) *m++=*start++;
	moveresult(st,result);
}

void mgetuword (header *hd)
{	unsigned short n;
	double x=-1;
	if (fa)
	{	if (fread(&n,sizeof(unsigned short),1,fa)==1)
		{	x=n;
		}
	}
	else no_file();
	new_real(x,"");
}

void mgetuword1 (header *hd)
{   header *st=hd,*result;
	long n,count=0;
	unsigned short *start=(unsigned short *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (!hd->type==s_real) wrong_arg_in("getword");
	n=(long)*realof(hd);
	if (n<=0) wrong_arg_in("getword");
	if (fa)
	{	if (!freeramfrom(start,n*sizeof(unsigned short)))
		{	output("Stack overflow in getword(n).");
			error=1; return;
		}
		newram=(char *)(start+n);
		count=fread(start,sizeof(unsigned short),n,fa);
	}
	else no_file();
	result=new_matrix(1,count,""); if (error) return;
	m=matrixof(result);
	for (n=0; n<count; n++) *m++=*start++;
	moveresult(st,result);
}

void mgetulongword (header *hd)
{	unsigned long n;
	double x=-1;
	if (fa)
	{	if (fread(&n,sizeof(unsigned long),1,fa)==1)
		{	x=n;
		}
	}
	else no_file();
	new_real(x,"");
}

void mgetulongword1 (header *hd)
{   header *st=hd,*result;
	long n,count=0;
	unsigned long *start=(unsigned long *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (!hd->type==s_real) wrong_arg_in("getlongword");
	n=(long)*realof(hd);
	if (n<=0) wrong_arg_in("getlongword");
	if (fa)
	{	if (!freeramfrom(start,n*sizeof(unsigned long)))
		{	output("Stack overflow in getlongword(n).");
			error=1; return;
		}
		newram=(char *)(start+n);
		count=fread(start,sizeof(unsigned long),n,fa);
	}
	else no_file();
	result=new_matrix(1,count,""); if (error) return;
	m=matrixof(result);
	for (n=0; n<count; n++) *m++=*start++;
	moveresult(st,result);
}

void mgetstring (header *hd)
{   header *st=hd,*result;
	long n,count=0;
	unsigned char *start=(unsigned char *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (!hd->type==s_real) wrong_arg_in("getchar");
	n=(long)*realof(hd);
	if (n<=0) wrong_arg_in("getchar");
	if (fa)
	{	if (!freeramfrom(start,n+1))
		{	output("Stack overflow in getchar(n).");
			error=1; return;
		}
		newram+=n+1;
		count=fread(start,1,n,fa);
		start[n]=0;
	}
	else no_file();
	result=new_string((char *)start,strlen((char *)start),""); if (error) return;
	m=matrixof(result);
	for (n=0; n<count; n++) *m++=*start++;
	moveresult(st,result);
}

void mputchar (header *hd)
{   header *st=hd,*result;
	int n,i;
	char *p,*start=(char *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real && hd->type!=s_matrix)
		wrong_arg_in("putchar(v)");
	getmatrix(hd,&i,&n,&m);
	if (i!=1 || n<1) wrong_arg_in("putchar(v)");
	if (!fa) no_file();
	if (!freeramfrom(start,n))
	{	output("Stack overflow in getchar(n).");
		error=1; return;
	}
	for (p=start,i=0; i<n; i++) *p++=(char)*m++;
	fwrite(start,1,n,fa);
	result=new_real(ferror(fa),""); if (error) return;
	moveresult(st,result);
}

void mputword (header *hd)
{	header *st=hd,*result;
	int n,i;
	short *p,*start=(short *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real && hd->type!=s_matrix)
		wrong_arg_in("putchar(v)");
	getmatrix(hd,&i,&n,&m);
	if (i!=1 || n<1) wrong_arg_in("putchar(v)");
	if (!fa) no_file();
	if (!freeramfrom(start,n*sizeof(short)))
	{	output("Stack overflow in getchar(n).");
		error=1; return;
	}
	for (p=start,i=0; i<n; i++) *p++=(short)*m++;
	fwrite(start,sizeof(short),n,fa);
	result=new_real(ferror(fa),""); if (error) return;
	moveresult(st,result);
}

void mputlongword (header *hd)
{	header *st=hd,*result;
	int n,i;
	long *p,*start=(long *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real && hd->type!=s_matrix)
		wrong_arg_in("putchar(v)");
	getmatrix(hd,&i,&n,&m);
	if (i!=1 || n<1) wrong_arg_in("putchar(v)");
	if (!fa) no_file();
	if (!freeramfrom(start,n*sizeof(long)))
	{	output("Stack overflow in getchar(n).");
		error=1; return;
	}
	for (p=start,i=0; i<n; i++) *p++=(long)*m++;
	fwrite(start,sizeof(long),n,fa);
	result=new_real(ferror(fa),""); if (error) return;
	moveresult(st,result);
}

void mgetchar (header *hd)
{	new_real(fa?getc(fa):-1,"");
}

void mgetchar1 (header *hd)
{   header *st=hd,*result;
	long n,count=0;
	char *start=(char *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (!hd->type==s_real) wrong_arg_in("getchar");
	n=(long)*realof(hd);
	if (n<=0) wrong_arg_in("getchar");
	if (fa)
	{	if (!freeramfrom(start,n))
		{	output("Stack overflow in getchar(n).");
			error=1; return;
		}
		newram+=n;
		count=fread(start,1,n,fa);
	}
	else no_file();
	result=new_matrix(1,count,""); if (error) return;
	m=matrixof(result);
	for (n=0; n<count; n++) *m++=*start++;
	moveresult(st,result);
}

void mgetword1 (header *hd)
{   header *st=hd,*result;
	long n,count=0;
	short *start=(short *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (!hd->type==s_real) wrong_arg_in("getword");
	n=(long)*realof(hd);
	if (n<=0) wrong_arg_in("getword");
	if (fa)
	{	if (!freeramfrom(start,n*sizeof(short)))
		{	output("Stack overflow in getword(n).");
			error=1; return;
		}
		newram=(char *)(start+n);
		count=fread(start,sizeof(short),n,fa);
	}
	else no_file();
	result=new_matrix(1,count,""); if (error) return;
	m=matrixof(result);
	for (n=0; n<count; n++) *m++=*start++;
	moveresult(st,result);
}

void mgetlongword1 (header *hd)
{   header *st=hd,*result;
	long n,count=0;
	long *start=(long *)newram;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (!hd->type==s_real) wrong_arg_in("getlongword");
	n=(long)*realof(hd);
	if (n<=0) wrong_arg_in("getlongword");
	if (fa)
	{	if (!freeramfrom(start,n*sizeof(long)))
		{	output("Stack overflow in getlongword(n).");
			error=1; return;
		}
		newram=(char *)(start+n);
		count=fread(start,sizeof(long),n,fa);
	}
	else no_file();
	result=new_matrix(1,count,""); if (error) return;
	m=matrixof(result);
	for (n=0; n<count; n++) *m++=*start++;
	moveresult(st,result);
}

void mgetword (header *hd)
{	short n;
	double x=-1;
	if (fa)
	{	if (fread(&n,sizeof(short),1,fa)==1)
		{	x=n;
		}
	}
	else no_file();
	new_real(x,"");
}

void mgetlongword (header *hd)
{	long n;
	double x=-1;
	if (fa)
	{	if (fread(&n,sizeof(long),1,fa)==1)
		{	x=n;
		}
	}
	else no_file();
	new_real(x,"");
}

void mgetvector (header *hd)
{   header *st=hd,*result;
	int i,c,n,negative;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real) wrong_arg_in("getvector");
	if (!fa) no_file();
	n=(unsigned int) *realof(hd);
	result=new_matrix(1,n,""); if (error) return;
	m=matrixof(result);
	for (i=0; i<n; i++)
	{	another: c=getc(fa);
		if (c==EOF || feof(fa)) break;
		if (c=='-')
		{	negative=1;
			c=getc(fa);
			if (c==EOF) break;
		}
		else negative=0;
		if (!isdigit(c)) goto another;
		ungetc(c,fa);
		fscanf(fa,"%lg",m);
		if (negative) *m=-*m;
		m++;
	}
	c=i;
	for (; i<n; i++) *m++=0.0;
	moveresult(st,result);
	new_real(c,"");
}

void meof (header *hd)
{	new_real(fa?feof(fa):1,""); if (error) return;
}


