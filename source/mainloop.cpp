#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>

#include "header.h"
#include "sysdep.h"
#include "funcs.h"
#include "graphics.h"
#include "version.h"
#include "interval.h"
#include "builtin.h"
#include "stack.h"
#include "meta.h"

char 
	*ramstart, // tobal start of RAM for the Euler stack
	*ramend, // total end of RAM for the Euler stack
	*udfend, // end of user defined functions (starts at ramstart)
	*startlocal, // start of current local variables
	*endlocal, // end of current local variables
	*newram, // start of new ram for new variables on stack
	*startglobal, // remembers the start of the global variables
	*endglobal; // end of the global variables
char *next,*udfline;
char* path[32]={"."};
int npath=1;

int searchglobal=0;

FILE *metafile=0;

double epsilon,changedepsilon;

char titel[]="This is EULER, Version %s.\n\n"
	"Type help(Return) for help.\n"
	"Enter command: (%ld Bytes free.)\n\n";

int error,quit,surpressed,udf=0,errorout,outputing=1,stringon=0,
	trace=0;
char input_line[1024];

long loopindex=0;

int fieldw=16,linew=5,ilinew=1,precission=5,iprecission=15;
double maxexpo=1.0e6,minexpo=1.0e-5;
int outputlength=14,ioutputlength=42;
char expoformat[16]="%0.6g";
char fixedformat[16]="%0.6g";
int iformat=42;
double fraceps=0;

int printcomments=1;

int nosubmref=0;

FILE *infile=0,*outfile=0;

header commandheader;

int promptnotebook=1,booktype=0;

extern commandtyp command_list[];

char *outputbuffer=0,*outputbufferend;
int outputbuffererror=0;

int commandtype;

void do_end (void);
void do_loop (void);
void do_repeat (void);
void do_for (void);

void scan_space (void)
{	start: while (*next==' ' || *next==TAB) next++;
	if (!udfon && *next=='.' && *(next+1)=='.')
		{	next_line(); if (error) return; goto start; }
}

void scan_end (void)
/***** scan_end
	scan for "end".
*****/
{	int comn;
	commandtyp *com;
	char *oldline=udfline;
	while (1)
	{	switch (*next)
		{	case 1 :
				output("End missing!\n");
				error=110; udfline=oldline; return;
			case 0 : udfline=next+1; next++; break;
			case 2 : next+=1+sizeof(double); break;
			case 3 : next++;
				memmove((char *)(&comn),next,sizeof(int));
				next+=sizeof(int);
				com=command_list+comn;
				if (com->nr==c_end)
				{	if (trace>0) trace_udfline(udfline);
				    return;
				}
				else if (com->nr==c_repeat || com->nr==c_loop ||
					com->nr==c_for)
				{	scan_end(); 
					if (error) return;
				}
				break;
			default : next++;
		}
	}
}

void do_endif (void);
void do_else (void);
void do_elseif (void);
void do_if (void);

void scan_endif (void)
/***** scan_endif
	scan for "endif".
*****/
{	commandtyp *com;
	int comn;
	char *oldline=udfline;
	while (1)
	{	switch (*next)
		{	case 1 :
				output("Endif missing, searching for endif!\n");
				error=110; udfline=oldline; return;
			case 0 : udfline=next+1; next++; break;
			case 2 : next+=1+sizeof(double); break;
			case 3 : next++;
				memmove((char *)(&comn),next,sizeof(int));
				next+=sizeof(int);
				com=command_list+comn;
				if (com->nr==c_endif)
				{	if (trace>0) trace_udfline(udfline);
					return;
				}
				else if (com->nr==c_if)
				{	scan_endif(); if (error) return; }
				break;
			default : next++;
		}
	}
}

int scan_else (void)
/***** scan_else
	scan for "else".
	return 1, if elseif was found.
*****/
{	commandtyp *com;
	int comn;
	char *oldline=udfline;
	while (1)
	{	switch (*next)
		{	case 1 :
				output("Endif missing, searching for else!\n");
				error=110; udfline=oldline; return 0;
			case 0 : udfline=next+1; next++; break;
			case 2 : next+=1+sizeof(double); break;
			case 3 : next++;
				memmove((char *)(&comn),next,sizeof(int));
				next+=sizeof(int);
				com=command_list+comn;
				if (com->nr==c_endif || com->nr==c_else)
				{	if (trace>0) trace_udfline(udfline);
					return 0;
				}
				else if (com->nr==c_elseif)
				{	return 1;
				}
				else if (com->nr==c_if)
				{	scan_endif(); if (error) return 0; }
				break;
			default : next++;
		}
	}
}

void scan_namemax (char *name, int lmax)
{	int count=0;
	if (*next=='\"')
	{	next++;
		while (*next!='\"' && *next)
		{	*name++=*next++; count++;
			if (count>=lmax-1)
			{	output("Name too long!\n");
				error=11; break;
			}
		}
		if (*next=='\"') next++;
	}
	else if (!isalpha(*next) && *next!='_')
	{   output1("Name expected at:\n%s<EOL>\n",next);
		error=11; *name=0; return;
	}
	else
	{	if (*next=='_') { *name++=*next++; count++; }
		while (isalpha(*next) || isdigit(*next))
		{	*name++=*next++; count++;
			if (count>=lmax-1)
			{	output("Name too long!\n");
				error=11; break;
			}
		}
	}
	*name=0;
}

void scan_name (char *name)
{	scan_namemax (name,16);
}

void getmatrix (header *hd, int *r, int *c, double **m)
/***** getmatrix
	get rows and columns from a matrix.
*****/
{	dims *d;
	if (hd->type==s_real || hd->type==s_complex
		|| hd->type==s_interval)
	{	*r=*c=1;
		*m=realof(hd);
	}
	else if (hd->type==s_matrix || hd->type==s_cmatrix
		|| hd->type==s_imatrix)
	{	d=dimsof(hd);
		*m=matrixof(hd);
		*r=d->r; *c=d->c;
	}
	else error=1;
}

header *searchvar (char *name)
/***** searchvar
	search a local variable, named "name".
	return 0, if not found.
*****/
{	int r;
	header *hd=(header *)startlocal;
	r=xor(name);
	while ((char *)hd<endlocal)
	{	if (r==hd->xor && !strcmp(hd->name,name)) return hd;
		hd=nextof(hd);
	}
	if (udfon && searchglobal)
	{   hd=(header *)udfend;
		while ((char *)hd<startlocal)
		{	if (r==hd->xor && !strcmp(hd->name,name)) return hd;
			hd=nextof(hd);
		}
	}
	return 0;
}

void kill_local (char *name)
/***** kill_local
	kill a loal variable name, if there is one.
*****/
{	ULONG size,rest;
	header *hd=(header *)startlocal;
	while ((char *)hd<endlocal)
	{	if (!strcmp(hd->name,name)) /* found! */
		{	size=hd->size;
			rest=newram-(char *)hd-size;
			if (size) memmove((char *)hd,(char *)hd+size,rest);
			endlocal-=size; newram-=size;
			return;
		}
		hd=(header *)((char *)hd+hd->size);
	}
}

header *next_param (header *hd)
/***** next_param
	get the next value on stack, if there is one
*****/
{	hd=(header *)((char *)hd+hd->size);
	if ((char *)hd>=newram) return 0;
	else return hd;
}

/********************* interpreter **************************/



#define MAX 1e+10
#define MIN 1e-10

double frac (double v, long *n, long *d, double error)
{
	long D, N, t;
	double epsilon, r=0, m;
	int count=0;


	if (v < MIN || v > MAX || error < 0.0)
		return(-1.0);
	*d = D = 1;
	*n = (int)v;
	N = (*n) + 1;
	goto three;

one:	count++;
	if (r > 1.0)
		goto two;
	r = 1.0/r;
two:	N += (*n)*(long)r;
	D += (*d)*(long)r;
	(*n) += N;
	(*d) += D;
three:
	if (v*(*d) == (double)(*n))
		goto four;
	r = (N - v*D)/(v*(*d) - (*n));
	if (r > 1.0)
		goto four;
	t = N;
	N = (*n);
	*n = t;
	t = D;
	D = (*d);
	*d = t;
four:
	epsilon = fabs(1.0 - (*n)/(v*(*d)));
	if (epsilon <= error)
		goto six;
	m = 1.0;
	do {
		m *= 10.0;
	} while (m*epsilon < 1.0);
	epsilon = 1.0/m * ((int)(0.5 + m*epsilon));
	six : if (epsilon <= error)
		return 0;
	if (r != 0.0 && count<1000)
		goto one;
	return -1;
}

#undef MIN
#undef MAX

int frac_out (double x)
{   long n,d;
	if (x==0.0) output1hold(0,"0");
	else
	{	if (frac(fabs(x),&n,&d,fraceps)<0) return 0;
		if (x<0) output1hold(0,"-");
		else output1hold(0,"");
		if (d>1) output1hold(-1,"%ld/%ld",n,d);
		else output1hold(-1,"%ld",n);
	}
	return 1;
}

int frac_out0 (double x)
{   long n,d;
	if (x==0.0) output1hold(-1,"0");
	else
	{	if (frac(fabs(x),&n,&d,fraceps)<0) return 0;
		if (x<0) output1hold(-1,"-");
		else output1hold(-1,"");
		if (d>1) output1hold(-1,"%ld/%ld",n,d);
		else output1hold(-1,"%ld",n);
	}
	return 1;
}

void double_out (double x)
/***** double_out
	print a double number.
*****/
{   if (fraceps>0)
	{	if (!frac_out(x)) goto one;
	}
	else
	{   one :
		if ((fabs(x)>maxexpo || fabs(x)<minexpo) && x!=0.0)
			output1hold(0,expoformat,x);
		else if (x==0.0) output1hold(0,fixedformat,0.0); /* take care of -0 */
		else output1hold(0,fixedformat,x);
	}
	output1hold(outputlength," ");
}

void out_matrix (header *hd)
/***** out_matrix
   print a matrix.
*****/
{	int c,r,i,j,c0,cend;
	double *m,*x;
	getmatrix(hd,&r,&c,&m);
	for (c0=0; c0<c; c0+=linew)
	{	cend=c0+linew-1;
		if (cend>=c) cend=c-1;
		if (c>linew) output2("Column %d to %d:\n",c0+1,cend+1);
		for (i=0; i<r; i++)
		{	x=mat(m,c,i,c0);
			for (j=c0; j<=cend; j++) double_out(*x++);
			output("\n");
			if (test_key()==escape) return;
		}
	}
}

void complex_out (double x, double y)
/***** complex_out
	print a complex number.
*****/
{	if (fraceps>0)
	{   if (!frac_out(x)) goto one;
		if (y!=0.0)
		{	if (y>0) output1hold(-1,"+");
			else if (y<0) output1hold(-1,"-");
			if (!frac_out0(fabs(y))) goto two;
		}
	}
	else
	{   one :
		if ((fabs(x)>maxexpo || fabs(x)<minexpo) && x!=0.0)
		output1hold(0,expoformat,x);
		else output1hold(0,fixedformat,x);
		if (y>=0) output1hold(-1,"+");
		else output1hold(-1,"-");
		y=fabs(y);
		two :
		if ((y>maxexpo || y<minexpo) && y!=0.0)
		output1hold(-1,expoformat,y);
		else output1hold(-1,fixedformat,y);
	}
	output1hold(outputlength*2,"i ");
}

void out_cmatrix (header *hd)
/***** out_matrix
   print a complex matrix.
*****/
{	int c,r,i,j,c0,cend;
	double *m,*x;
	getmatrix(hd,&r,&c,&m);
	for (c0=0; c0<c; c0+=linew/2)
	{	cend=c0+linew/2-1;
		if (cend>=c) cend=c-1;
		if (c>linew/2) output2("Column %d to %d:\n",c0+1,cend+1);
		for (i=0; i<r; i++)
		{	x=cmat(m,c,i,c0);
			for (j=c0; j<=cend; j++) { complex_out(*x,*(x+1));
				x+=2; }
			output("\n");
			if (test_key()==escape) return;
		}
	}
}

void interval_out (double x, double y)
/***** double_out
	print a complex number.
*****/
{   int d1,d2,l;
	char form[16];
	if (iformat>0 && (x>0 || y<0) && x!=y)
	{   if (x>0)
		{	d1=(int)log10(y); d2=(int)log10(y-x);
		}
		else
		{	d1=(int)log10(-x); d2=(int)log10(-x+y);
		}
		l=d1-d2+2;
		if (l>DBL_DIG+3) l=DBL_DIG+3;
		sprintf(form,"%%0.%dg",l);
		y+=pow(10.0,floor(log10(fabs(y)))-l+1)/2.00000001;
		x-=pow(10.0,floor(log10(fabs(x)))-l+1)/2.00000001;
		output1hold(0,"~");
		output1hold(-1,form,x);
		output1hold(-1,",");
		output1hold(-1,form,y);
		output1hold(ioutputlength,"~ ");
		return;
	}
	output1hold(0,"~");
	output1hold(-1,"%0.2g",x);
	output1hold(-1,",");
	output1hold(-1,"%0.2g",y);
	output1hold(ioutputlength,"~ ");
}

void out_imatrix (header *hd)
/***** out_matrix
   print a complex matrix.
*****/
{	int c,r,i,j,c0,cend;
	double *m,*x;
	getmatrix(hd,&r,&c,&m);
	for (c0=0; c0<c; c0+=ilinew)
	{	cend=c0+ilinew-1;
		if (cend>=c) cend=c-1;
		if (c>ilinew) output2("Column %d to %d:\n",c0+1,cend+1);
		for (i=0; i<r; i++)
		{	x=imat(m,c,i,c0);
			for (j=c0; j<=cend; j++) { interval_out(*x,*(x+1));
				x+=2; }
			output("\n");
			if (test_key()==escape) return;
		}
	}
}

void give_out (header *hd)
/***** give_out
	print a value.
*****/
{	switch(hd->type)
	{	case s_real : double_out(*realof(hd)); output("\n"); break;
		case s_complex : complex_out(*realof(hd),*imagof(hd));
			output("\n"); break;
		case s_matrix : out_matrix(hd); break;
		case s_cmatrix : out_cmatrix(hd); break;
		case s_imatrix : out_imatrix(hd); break;
		case s_string : output(stringof(hd)); output("\n"); break;
		case s_interval : interval_out(*aof(hd),*bof(hd));
			output("\n"); break;
		default : output("?\n");
	}
}

/***************** some builtin commands *****************/

#define EXTENSION ".e"
#define BOOKEXTENSION ".en"

void load_file (void)
/***** load_file
	interpret a file.
*****/
{	char filename[256];
	char oldline[1024],fn[256],*oldnext;
	int oldbooktype=booktype,pn;
	header *hd;
	FILE *oldinfile;
	if (udfon)
	{	output("Cannot load a file in a function!\n");
		error=221; return;
	}
	scan_space();
	if (*next=='(')
	{   hd=scan_value();
		if (error) return;
		if (hd->type!=s_string)
		{	output("String value expected!\n");
			error=1; return;
		}
		strcpy(filename,stringof(hd));
	}
	else
	{	scan_namemax(filename,256);
	}
	if (error) return;
	oldinfile=infile;
	pn=-1;
	retry :
	if (pn>=0)
	{	strcpy(fn,path[pn]);
		strcat(fn,PATH_DELIM_STR);
		strcat(fn,filename);
	}
	else strcpy(fn,filename);
	infile=fopen(fn,"r");
	if (!infile)
	{   strcat(fn,EXTENSION);
		infile=fopen(fn,"r");
		pn++;
		if (!infile)
		{	if (pn>=npath)
			{	output1("Could not open %s!\n",filename);
				error=53; infile=oldinfile; return;
			}
			else goto retry;
		}
	}
	strcpy(oldline,input_line); oldnext=next;
	*input_line=0; next=input_line;
	booktype=0;
	while (!error && infile && !quit) command();
	booktype=oldbooktype;
	if (infile) fclose(infile);
	infile=oldinfile;
	strcpy(input_line,oldline); next=oldnext;
}

void load_book (void)
/***** load_book
	interpret a notebook file.
*****/
{	header *hd;
	char name[256];
	char oldline[1024],fn[256],*oldnext;
	int oldbooktype=booktype;
	FILE *oldinfile;
	if (udfon)
	{	output("Cannot load a notebook in a function!\n");
		error=221; return;
	}
	 scan_space();
	if (*next=='(')
	{   hd=scan_value();
		if (error) return;
		if (hd->type!=s_string)
		{	output("String value expected!\n");
			error=1; return;
		}
		strcpy(name,stringof(hd));
	}
	else
	{	scan_namemax(name,256);
	}
	if (error) return;
	oldinfile=infile;
	infile=fopen(name,"r");
	if (!infile)
	{	strcpy(fn,name);
		strcat(fn,BOOKEXTENSION);
		infile=fopen(fn,"r");
		if (!infile)
		{	output1("Could not open %s!\n",stringof(name));
			error=53; infile=oldinfile; return;
		}
	}
	strcpy(oldline,input_line); oldnext=next;
	*input_line=0; next=input_line;
	booktype=1;
	while (!error && infile && !quit)
	{	startglobal=startlocal; endglobal=endlocal;
		command();
	}
	booktype=oldbooktype;
	if (infile) fclose(infile);
	infile=oldinfile;
	strcpy(input_line,oldline); next=oldnext;
}

void do_return (void)
{	if (!udfon)
	{	output("Use return only in functions!\n");
		error=56; return;
	}
	else udfon=2;
}

void do_break (void)
{	if (!udfon)
	{	output("Break only allowed in functions!\n"); error=57;
	}
}

void do_for (void)
/***** do_for
	do a for command in a UDF.
	for i=value to value step value; .... ; end
*****/
{	int h,signum;
	char name[16],*jump;
	header *hd,*init,*end,*step;
	double vend,vstep;
	struct { header hd; double value; } rv;
	if (!udfon)
	{	output("For only allowed in functions!\n"); error=57; return;
	}
	rv.hd.type=s_real; *rv.hd.name=0;
	rv.hd.size=sizeof(header)+sizeof(double); rv.value=0.0;
	scan_space(); scan_name(name); if (error) return;
	kill_local(name);
	newram=endlocal;
	hd=new_reference(&rv.hd,name); if (error) return;
	endlocal=newram=(char *)hd+hd->size;
	scan_space(); if (*next!='=')
	{	output("Syntax error in for.\n"); error=71; goto end;
	}
	next++; init=scan(); if (error) goto end;
	init=getvalue(init); if (error) goto end;
	if (init->type!=s_real)
	{	output("Startvalue must be real!\n"); error=72; goto end;
	}
	rv.value=*realof(init);
	scan_space(); if (strncmp(next,"to",2))
	{	output("Endvalue missing in for!\n"); error=73; goto end;
	}
	next+=2;
	end=scan(); if (error) goto end;
	end=getvalue(end); if (error) goto end;
	if (end->type!=s_real)
	{	output("Endvalue must be real!\n"); error=73; goto end;
	}
	vend=*realof(end);
	scan_space();
	if (!strncmp(next,"step",4))
	{	next+=4;
		step=scan(); if (error) goto end;
		step=getvalue(step); if (error) goto end;
		if (step->type!=s_real)
		{	output("Stepvalue must be real!\n"); error=73; goto end;
		}
		vstep=*realof(step);
	}
	else vstep=1.0;
	signum=(vstep>=0)?1:-1;
	vend=vend+signum*epsilon;
	if (signum>0 && rv.value>vend) { scan_end(); goto end; }
	else if (signum<0 && rv.value<vend) { scan_end(); goto end; }
	newram=endlocal;
	scan_space(); if (*next==';' || *next==',') next++;
	jump=next;
	while (!error)
	{	if (*next==1)
		{	output("End missing!\n");
			error=401; goto end;
		}
		h=command();
		if (udfon!=1 || h==c_return) break;
		if (h==c_break) { scan_end(); break; }
		if (h==c_end)
		{	rv.value+=vstep;
			if (signum>0 && rv.value>vend) break;
			else if (signum<0 && rv.value<vend) break;
			else next=jump;
			if (test_key()==escape)
			{   output("User interrupted!\n");
				error=1; break;
			}
		}
	}
	end : kill_local(name);
}

void do_loop (void)
/***** do_loop
	do a loop command in a UDF.
	loop value to value; .... ; end
*****/
{	int h;
	char *jump;
	header *init,*end;
	long vend,oldindex;
	if (!udfon)
	{	output("Loop only allowed in functions!\n");
		error=57; return;
	}
	init=scan(); if (error) return;
	init=getvalue(init); if (error) return;
	if (init->type!=s_real)
	{	output("Startvalue must be real!\n"); error=72; return;
	}
	oldindex=loopindex;
	loopindex=(long)*realof(init);
	scan_space(); if (strncmp(next,"to",2))
	{	output("Endvalue missing in loop!\n"); error=73; goto end;
	}
	next+=2;
	end=scan(); if (error) goto end;
	end=getvalue(end); if (error) goto end;
	if (end->type!=s_real)
	{	output("Endvalue must be real!\n"); error=73; goto end;
	}
	vend=(long)*realof(end);
	if (loopindex>vend) { scan_end(); goto end; }
	newram=endlocal;
	scan_space(); if (*next==';' || *next==',') next++;
	jump=next;
	while (!error)
	{	if (*next==1)
		{	output("End missing in loop!\n");
			error=401; goto end;
		}
		h=command();
		if (udfon!=1 || h==c_return) break;
		if (h==c_break) { scan_end(); break; }
		if (h==c_end)
		{	loopindex++;
			if (loopindex>vend) break;
			else next=jump;
			if (test_key()==escape)
			{	output("User interrupted!\n");
				error=1; break;
			}
		}
	}
	end : loopindex=oldindex;
}

void do_repeat (void)
/***** do_loop
	do a loop command in a UDF.
	for value to value; .... ; endfor
*****/
{	int h;
	char *jump;
	if (!udfon)
	{	output("Repeat only allowed in functions!\n");
		error=57; return;
	}
	newram=endlocal;
	scan_space(); if (*next==';' || *next==',') next++;
	jump=next;
	while (!error)
	{	if (*next==1)
		{	output("End missing in repeat statement!\n");
			error=401; break;
		}
		h=command();
		if (udfon!=1 || h==c_return) break;
		if (h==c_break) { scan_end(); break; }
		if (h==c_end)
		{	next=jump;
			if (test_key()==escape)
			{   output1("User interrupted\n");
				error=1; break;
			}
		}
	}
}

void do_end (void)
{	if (!udfon)
	{	output("End only allowed in functions!\n"); error=57;
	}
}

void do_else (void)
{	if (!udfon)
	{	output("Else only allowed in functions!\n"); error=57; return;
	}
	scan_endif();
}

void do_elseif (void)
{	if (!udfon)
	{	output("Elseif only allowed in functions!\n"); error=57; return;
	}
	scan_endif();
}

void do_endif (void)
{	if (!udfon)
	{	output("Endif only allowed in functions!\n"); error=57;
	}
}

int ctest (header *hd)
/**** ctest
	test, if a matrix contains nonzero elements.
****/
{	double *m;
	LONG n,i;
	hd=getvalue(hd); if (error) return 0;
	if (hd->type==s_string) return (*stringof(hd)!=0);
	if (hd->type==s_real) return (*realof(hd)!=0.0);
	if (hd->type==s_complex) return (*realof(hd)!=0.0 &&
		*imagof(hd)!=0.0);
	if (hd->type==s_matrix)
	{	n=(LONG)(dimsof(hd)->r)*dimsof(hd)->c;
		m=matrixof(hd);
		for (i=0; i<n; i++) if (*m++==0.0) return 0;
		return 1;
	}
	if (hd->type==s_cmatrix)
	{	n=(LONG)(dimsof(hd)->r)*dimsof(hd)->c;
		m=matrixof(hd);
		for (i=0; i<n; i++) 
		{	if (*m==0.0 && *m==0.0) return 0; m+=2; }
		return 1;
	}
	return 0;
}

void do_if (void)
{	header *cond;
	int flag;
	if (!udfon)
	{	output("If only allowed in functions!\n"); error=111; return;
	}
	another : cond=scan(); if (error) return;
	flag=ctest(cond); if (error) return;
	if (!flag)
		if (scan_else()) goto another;
}

void do_clg (void)
{	graphic_mode(); gclear(); gflush();
}

void do_cls (void)
{	text_mode(); clear_screen();
}

void do_clear (void)
{	if (udfon)
	{	output("Cannot clear in a function!\n");
		error=120; return;
	}
	clear();
}

void do_quit (void)
{	quit=1;
}

void do_exec (void)
{	header *name;
	char *s;
	name=scan_value(); if (error) return;
	if (name->type!=s_string)
	{	output("Cannot execute a number or matrix!\n");
		error=130; return;
	}
	s=stringof(name);
	while (*s && !isspace(*s)) s++;
	if (*s) *s++=0;
	if (execute(stringof(name),s))
	{	output("Execution failed or program returned a failure!\n");
		error=131;
	}
}

void do_forget (void)
{	char name[16];
	header *hd;
	int r;
	if (udfon)
	{	output("Cannot forget functions in a function!\n");
		error=720; return;
	}
	while (1)
	{	scan_space();
		scan_name(name);
		r=xor(name);
		hd=(header *)ramstart;
		while ((char *)hd<udfend)
		{	if (r==hd->xor && !strcmp(hd->name,name)) break;
			hd=nextof(hd);
		}
		if ((char *)hd>=udfend)
		{	output1("Function %s not found!\n",name);
			error=160; return;
		}
		kill_udf(name);
		scan_space();
		if (*next!=',') break;
		else next++;
	}
}

void do_global (void)
{	char name[16];
	int r;
	header *hd;
	while (1)
	{	scan_space(); scan_name(name); r=xor(name);
		hd=(header *)udfend;
		if (hd==(header *)startlocal) break;
		while ((char *)hd<startlocal)
		{	if (r==hd->xor && !strcmp(hd->name,name)) break;
			hd=nextof(hd);
		}
		if ((char *)hd>=startlocal)
		{	output1("Variable %s not found!\n",name);
			error=160; return;
		}
		newram=endlocal;
		hd=new_reference(hd,name);
		newram=endlocal=(char *)nextof(hd);
		scan_space();
		if (*next!=',') break;
		else next++;
	}
}

void do_useglobal (void)
{   searchglobal=1;
}

void print_commands (void);

void do_list (void)
{	header *hd;
	int lcount=0;
	output("  *** Builtin functions:\n");
	print_builtin();
	output("  *** Commands:\n");
	print_commands();
	output("  *** Your functions:\n");
	hd=(header *)ramstart;
	while ((char *)hd<udfend)
	{	if (hd->type!=s_udf) break;
		if (lcount+(int)strlen(hd->name)+2>=linelength)
			{ lcount=0; output("\n"); }
		output1("%s ",hd->name);
		lcount+=(int)strlen(hd->name)+1;
		hd=nextof(hd);
	}
	output("\n");
}

void listvar1 (char *s, header *hd)
{	output1("%-20sType: %s\n",hd->name,s);
}

void listvar2 (char *s, header *hd)
{	output1("%-20sType: %s (%dx%d)\n",hd->name,s,dimsof(hd)->r,dimsof(hd)->c);
}

void listvar3 (char *s, header *hd)
{	output1("%-20sType: %s (%dx%0d)",hd->name,s,
		submdimsof(hd)->r,submdimsof(hd)->c);
}

void do_listvar (void)
{	header *hd=(header *)startlocal;
	while (hd<(header *)endlocal)
	{	switch (hd->type)
		{	case s_real : listvar1("Real",hd); break;
			case s_interval : listvar1("Interval",hd); break;
			case s_complex : listvar1("Complex",hd); break;
			case s_string : listvar1("String",hd); break;
			case s_matrix : listvar2("Real Matrix",hd); break;
			case s_cmatrix : listvar2("Complex Matrix",hd); break;
			case s_imatrix : listvar2("Interval Matrix",hd); break;
			case s_reference : listvar1("Reference",hd); break;
			case s_submatrix : listvar3("Real Submatrix",hd); break;
			case s_isubmatrix : listvar3("Interval Submatrix",hd); break;
			case s_csubmatrix : listvar3("Complex Submatrix",hd); break;
			default: listvar1("Unknown Type",hd); break;
		}
		hd=nextof(hd);
		if (test_key()==escape) break;
	}
}

void do_dump (void)
{	header *file;
	if (outfile)
	{	if (fclose(outfile))
		{	output("Error while closing dumpfile.\n");
		}
		outfile=0;
	}
	scan_space();
	if (*next==';' || *next==',' || *next==0)
	{	if (*next) next++; return; }
	file=scan_value();
	if (error || file->type!=s_string)
	{	output("Dump needs a filename!\n");
		error=201; return;
	}
	outfile=fopen(stringof(file),"a");
	if (!outfile)
	{	output1("Could not open %s.\n",stringof(file));
	}
}

void do_dir (void)
{	header *file;
	char *s;
	scan_space();
	if (*next==';' || *next==',' || *next==0)
	{	file=new_string("*.*",5,"");
	}
	else file=scan_value();
	if (error || file->type!=s_string)
	{	output("Dir needs a string!\n");
		error=201; return;
	}
	s=dir(stringof(file));
	if (!s || !*s) return;
	output1("%s\n",s);
	while (1)
	{	s=dir(0);
		if (!s || !*s) break;
		output1("%s\n",s);
	}
	if (*next==',' || *next==';') next++;
}

void do_path (void)
{	header *ppath;
	char s[256],*p,*q;
	int i;
	scan_space();
	if (*next==';' || *next==',' || *next==0)
	{   out :
		for (i=0; i<npath; i++)
		{	output1("%s;",path[i]);
		}
		output("\n");
		return;
	}
	ppath=scan_value();
	if (error || ppath->type!=s_string)
	{	output("Path needs a string!\n");
		error=201; return;
	}
	p=stringof(ppath);
	for (i=0; i<npath; i++) free(path[i]);
	npath=0;
	while (*p)
	{	q=s;
		while (*p && *p!=';') *q++=*p++;
		if (q>s && *(q-1)==PATH_DELIM_CHAR) q--;
		*q=0;
		if (*p==';') p++;
		path[npath]=(char *)malloc(strlen(s)+1);
		strcpy(path[npath],s);
		npath++;
	}
	if (npath==0)
	{	path[0]=(char *)malloc(5);
		strcpy(path[0],".");
	}
	if (*next!=';') goto out;
}

void do_cd (void)
{	header *hd;
	char name[256];
	char *s;
	scan_space();
	if (*next==';' || *next==',' || *next==0)
	{	s=cd("");
		output1("%s\n",s);
		return;
	}
	if (*next=='(')
	{   hd=scan_value();
		if (error) return;
		if (hd->type!=s_string)
		{	output("String value expected!\n");
			error=1; return;
		}
		strcpy(name,stringof(hd));
	}
	else
	{	scan_namemax(name,256);
	}
	if (error) return;
	s=cd(name);
	if (*next!=';') output1("%s\n",s);
	if (*next==',' || *next==';') next++;
}

void do_meta (void)
{	header *file;
	scan_space();
	file=scan_value();
	if (error || file->type!=s_string)
	{	output("Meta needs a filename!\n");
		error=201; return;
	}
	FILE *metafile=fopen(stringof(file),"wb");
	if (!metafile)
	{	output1("Could not open %s.\n",stringof(file));
	}
    dump_meta(metafile);
    fclose(metafile);
}

void do_postscript (void)
{	header *file;
	scan_space();
	file=scan_value();
	if (error || file->type!=s_string)
	{	output("Postscript needs a filename!\n");
		error=201; return;
	}
	FILE *metafile=fopen(stringof(file),"w");
	if (!metafile)
	{	output1("Could not open %s.\n",stringof(file));
	}
    dump_postscript(metafile);
    fclose(metafile);
}

void do_remove (void)
{	header *hd;
	char name[256];
	if (*next=='(')
	{   hd=scan_value();
		if (error) return;
		if (hd->type!=s_string)
		{	output("String value expected!\n");
			error=1; return;
		}
		strcpy(name,stringof(hd));
	}
	else
	{	scan_namemax(name,256);
	}
	if (error) return;
	remove(name);
}

void do_do (void)
{	int udfold;
	char name[16];
	char *oldnext=next,*udflineold;
	header *var;
	scan_space(); scan_name(name); if (error) return;
	var=searchudf(name);
	if (!var || var->type!=s_udf)
	{	output("Need a udf!\n"); error=220; return;
	}
	udflineold=udfline; udfline=next=udfof(var); udfold=udfon; udfon=1;
	while (!error && udfon==1)
	{	command();
		if (udfon==2) break;
		if (test_key()==escape) 
		{	output("User interrupted!\n"); error=58; break;
		}
	}
	if (error) output1("Error in function %s\n",var->name);
	if (udfon==0)
	{	output1("Return missing in %s!\n",var->name); error=55; }
	udfon=udfold; udfline=udflineold;
	if (udfon) next=oldnext;
	else { next=input_line; *next=0; }
}

void do_mdump (void)
{	header *hd;
	output1("ramstart : 0\nstartlocal : %ld\n",startlocal-ramstart);
	output1("endlocal : %ld\n",endlocal-ramstart);
	output1("newram   : %ld\n",newram-ramstart);
	output1("ramend   : %ld\n",ramend-ramstart);
	hd=(header *)ramstart;
	while ((char *)hd<newram)
	{
		output1("%6ld : %16s, ",(char *)hd-ramstart,hd->name);
		output1("size %6ld ",(long)hd->size);
		output1("type %d\n",hd->type);
		hd=nextof(hd);
	}
}

void hex_out1 (int n)
{	if (n<10) output1("%c",n+'0');
	else output1("%c",n-10+'A');
}

void hex_out (unsigned int n)
{	hex_out1(n/16);
	hex_out1(n%16);
	output(" ");
}

void string_out (unsigned char *p)
{	int i;
	unsigned char a;
	for (i=0; i<16; i++) 
	{	a=*p++;
		output1("%c",(a<' ')?'_':a);
	}
}

void do_hexdump (void)
{	char name[16];
	unsigned char *p,*end;
	int i=0,j;
	ULONG count=0;
	header *hd;
	scan_space(); scan_name(name); if (error) return;
	hd=searchvar(name);
	if (!hd) hd=searchudf(name);
	if (error || hd==0) return;
	p=(unsigned char *)hd; end=p+hd->size;
	output1("\n%5lx ",count);
	while (p<end)
	{	hex_out(*p++); i++; count++;
		if (i>=16) 
		{	i=0; string_out(p-16);
			output1("\n%5lx ",count);
			if (test_key()==escape) break;
		}
	}
	for (j=i; j<16; j++) output("   ");
	string_out(p-i);
	output("\n");
}

void do_output (void)
/**** do_output
	toggles output.
****/
{	scan_space();
	if (!strncmp(next,"off",3))
	{	outputing=0; next+=3;
	}
	else if (!strncmp(next,"on",2))
	{	outputing=1; output("\n"); next+=2;
	}
	else outputing=!outputing;
}

void do_prompt (void)
/**** do_prompt
	toggles notebook prompt.
****/
{	scan_space();
	if (!strncmp(next,"off",3))
	{   promptnotebook=0; next+=3;
	}
	else if (!strncmp(next,"on",2))
	{	promptnotebook=1; output("\n"); next+=2;
	}
	else promptnotebook=!promptnotebook;
}

void do_comments (void)
/**** do_comments
	toggles comments
****/
{	scan_space();
	if (!strncmp(next,"off",3))
	{	printcomments=0; next+=3;
	}
	else if (!strncmp(next,"on",2))
	{	printcomments=1; output("\n"); next+=2;
	}
	else printcomments=!printcomments;
}

void do_comment (void)
{	FILE *fp=infile;
	if (!fp || udfon)
	{	output("comment illegal at this place\n");
		error=1001; return;
	}
	while (1)
	{	next_line();
		if (infile!=fp)
		{	output("endcomment missing!\n"); error=1002;
			return;
		}
		if (strncmp(next,"endcomment",10)!=0)
		{	if (printcomments)
			{	output(input_line); output("\n");
			}
		}
		else break;
	}
	next_line();
}

void do_trace(void)
/**** do_trace
	toggles tracing or sets the trace bit of a udf.
****/
{	header *f;
	char name[64];
	scan_space();
	if (!strncmp(next,"off",3))
	{	trace=0; next+=3;
	}
	else if (!strncmp(next,"alloff",6))
	{	next+=6;
		f=(header *)ramstart;
		while ((char *)f<udfend && f->type==s_udf)
		{	f->flags&=~1;
			f=nextof(f);
		}
		trace=0;
	}	
	else if (!strncmp(next,"on",2))
	{	trace=1; next+=2;
	}
	else if (*next==';' || *next==',' || *next==0) trace=!trace;
	else
	{	if (*next=='"') next++;
		scan_name(name); if (error) return;
		if (*next=='"') next++;
		f=searchudf(name);
		if (!f || f->type!=s_udf)
		{	output("Function not found!\n");
			error=11021; return;
		}
		f->flags^=1;
		if (f->flags&1) output1("Tracing %s\n",name);
		else output1("No longer tracing %s\n",name);
		scan_space();
	}
	if (*next==';' || *next==',') next++;
}

int command_count;

void do_type (void);
void do_help (void);

commandtyp command_list[] =
	{{"quit",c_quit,do_quit},
	 {"hold",c_hold,ghold},
	 {"shg",c_shg,show_graphics},
	 {"load",c_load,load_file},
	 {"function",c_udf,get_udf},
	 {"return",c_return,do_return},
	 {"for",c_for,do_for},
	 {"endif",c_endif,do_endif},
	 {"end",c_end,do_end},
	 {"break",c_break,do_break},
	 {"loop",c_loop,do_loop},
	 {"else",c_else,do_else},
	 {"elseif",c_elseif,do_elseif},
	 {"if",c_if,do_if},
	 {"repeat",c_repeat,do_repeat},
	 {"clear",c_clear,do_clear},
	 {"clg",c_clg,do_clg},
	 {"cls",c_cls,do_cls},
	 {"exec",c_exec,do_exec},
	 {"forget",c_forget,do_forget},
	 {"global",c_global,do_global},
	 {"useglobal",c_global,do_useglobal},
	 {"list",c_global,do_list},
	 {"listvar",c_global,do_listvar},
	 {"type",c_global,do_type},
	 {"dump",c_global,do_dump},
	 {"remove",c_global,do_remove},
	 {"help",c_global,do_help},
	 {"do",c_global,do_do},
	 {"memorydump",c_global,do_mdump},
	 {"hexdump",c_global,do_hexdump},
	 {"output",c_global,do_output},
	 {"comments",c_global,do_comments},
	 {"meta",c_global,do_meta},
	 {"postscript",c_global,do_postscript},
	 {"comment",c_global,do_comment},
	 {"trace",c_global,do_trace},
	 {"notebook",c_global,load_book},
	 {"prompt",c_global,do_prompt},
	 {"cd",c_global,do_cd},
	 {"dir",c_global,do_dir},
	 {"path",c_global,do_path},
	 {0,c_none,0} };

void print_commands (void)
{	int linel=0,i;
	for (i=0; i<command_count; i++)
	{	if (linel+strlen(command_list[i].name)+2>
				(unsigned int)linelength)
			{ output("\n"); linel=0; }
		output1("%s ",command_list[i].name);
		linel+=(int)strlen(command_list[i].name)+1;
	}
	output("\n");
}

int command_compare (const commandtyp *p1, const commandtyp *p2)
{	return strcmp(p1->name,p2->name);
}

void sort_command (void)
{	command_count=0;
	while (command_list[command_count].name) command_count++;
	qsort(command_list,command_count,sizeof(commandtyp),
		(int (*)(const void *, const void *))command_compare);
}

commandtyp *preview_command (ULONG *l)
{	commandtyp h;
	char name[16],*a,*n;
	*l=0;
	a=next; n=name;
	while (*l<15 && isalpha(*a)) { *n++=*a++; *l+=1; }
	*n=0; if (isalpha(*a)) return 0;
	h.name=name;
	return (commandtyp *)bsearch(&h,command_list,command_count,sizeof(commandtyp),
		(int (*)(const void *, const void *))command_compare);
}

int builtin (void)
/***** builtin
	interpret a builtin command, number no.
*****/
{	ULONG l;
	commandtyp *p;
	int comn;
	if (*next==3)
	{	next++;
#ifdef SPECIAL_ALIGNMENT
		memmove((char *)(&comn),next,sizeof(int));
#else
		comn=*((int *)next);
#endif
		p=command_list+comn;
		l=sizeof(int);
	}
	else if (udfon) return 0;
	else p=preview_command(&l);
	if (p)
	{	next+=l;
		p->f();
		if (*next==';' || *next==',') next++;
		commandtype=p->nr;
		return 1;
	}
	return 0;
}

header *scan_expression (void)
/***** scan_expression
	scans a variable, a value or a builtin command.
*****/
{	if (builtin()) return &commandheader;
	return scan();
}

#define addsize(hd,size) ((header *)((char *)(hd)+size))

void do_assignment (header *var)
/***** do_assignment
	assign a value to a variable.
*****/
{	header *variable[8],*rightside[8],*rs,*v,*mark;
	int rscount,varcount,i,j;
	ULONG offset,oldoffset,dif;
	char *oldendlocal;
	scan_space();
	if (*next=='=')
	{	next++;
		nosubmref=1; rs=scan_value(); nosubmref=0;
		if (error) return;
		varcount=0;
		/* count the variables, that get assigned something */
		while (var<rs)
		{	if (var->type!=s_reference && var->type!=s_submatrix
				&& var->type!=s_csubmatrix && var->type!=s_isubmatrix)
			{	output("Cannot assign to value!\n");
				error=210;
			}
			variable[varcount]=var; var=nextof(var); varcount++;
			if (varcount>=8)
			{	output("To many commas!\n"); error=100; return;
			}
		}
		/* count and note the values, that are assigned to the
			variables */
		rscount=0;
		while (rs<(header *)newram)
		{	rightside[rscount]=rs;
			rs=nextof(rs); rscount++;
			if (rscount>=8)
			{	output("To many commas!\n"); error=101; return;
			}
		}
		/* cannot assign 2 values to 3 variables , e.g. */
		if (rscount>1 && rscount<varcount)
		{	output("Illegal multiple assignment!\n"); error=102; return;
		}
		oldendlocal=endlocal;
		offset=0;
		/* do all the assignments */
		if (varcount==1) var=assign(variable[0],rightside[0]);
		else
		for (i=0; i<varcount; i++)
		{	oldoffset=offset;
			/* assign a variable */
			var=assign(addsize(variable[i],offset),
				addsize(rightside[(rscount>1)?i:0],offset));
			if (error) return;
			offset=endlocal-oldendlocal;
			if (oldoffset!=offset) /* size of var. changed */
			{	v=addsize(variable[i],offset);
				if (v->type==s_reference) mark=referenceof(v);
				else mark=submrefof(v);
				/* now shift all references of the var.s */
				if (mark) /* not a new variable */
					for (j=i+1; j<varcount; j++)
					{	v=addsize(variable[j],offset);
						dif=offset-oldoffset;
						if (v->type==s_reference && referenceof(v)>mark)
							referenceof(v)=addsize(referenceof(v),dif);
						else if (submrefof(v)>mark)
							submrefof(v)=addsize(submrefof(v),dif);
					}
			}
		}
	}
	else /* just an expression which is a variable */
	{	var=getvalue(var);
	}
	if (error) return;
	if (*next!=';') give_out(var);
	if (*next==',' || *next==';') next++;
}

int command (void)
/***** command
	scan a command and interpret it.
	return, if the user wants to quit.
*****/
{	header *expr;
	int ret=c_none;
	quit=0; error=0; errorout=0;
	while(1)
	{	scan_space();
		if (*next) break;
		else next_line();
	}
	if (*next==1) return ret;
	expr=scan_expression();
	if (!expr) { newram=endlocal; return ret; }
	if (error)
	{	newram=endlocal;
		print_error(next);
		next=input_line; input_line[0]=0;
		return ret;
	}
	if (expr==&commandheader)
	{	newram=endlocal;
		return commandtype;
	}
	switch (expr->type)
	{	case s_real :
		case s_complex :
		case s_matrix :
		case s_cmatrix :
		case s_imatrix :
		case s_string :
		case s_interval :
			if (*next!=';') give_out(expr);
			if (*next==',' || *next==';') next++;
			break;
		case s_reference :
		case s_submatrix :
		case s_csubmatrix :
		case s_isubmatrix :
			do_assignment(expr);
			break;
		default : break;
	}
	if (error) print_error(next);
	newram=endlocal;
	if (error) { next=input_line; input_line[0]=0; }
	return ret;
}

/******************* main functions ************************/

void clear_fktext (void)
{	int i;
	for (i=0; i<10; i++) fktext[i][0]=0;
}

void main_loop (int argc, char *argv[])
{	int i;
	output2(titel,VERSION,(unsigned long)(ramend-ramstart));
	newram=startlocal=endlocal=ramstart;
	udfend=ramstart;
	changedepsilon=epsilon=10000*DBL_EPSILON;
	sort_builtin(); sort_command(); make_xors(); clear_fktext();
	accuinit();
	next=input_line; *next=0;		/* clear input line */
	strcpy(input_line,"wait(1); load \"euler.cfg\";");
	for (i=1; i<argc; i++)
	{	if (argv[i][0]=='"') strcat(input_line," load ");
		else strcat(input_line," load \"");
		strcat(input_line,argv[i]);
		if (argv[i][0]=='"') strcat(input_line,";");
		else strcat(input_line,"\";");
	}
	path[0]=(char *)malloc(5);
	strcpy(path[0],".");
	npath=1;
	while (!quit)
	{   startglobal=startlocal;
		endglobal=endlocal;
		command();	/* interpret until "quit" */
		if (trace<0) trace=0;
	}
}

