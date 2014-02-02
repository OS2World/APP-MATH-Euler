#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

#include "header.h"
#include "sysdep.h"
#include "funcs.h"
#include "matheh.h"
#include "polynom.h"
#include "helpf.h"
#include "interval.h"
#include "spread.h"
#include "express.h"
#include "stack.h"
#include "builtin.h"

extern int outputlength,ioutputlength,iformat,precission,iprecission,ilinew;

extern double fraceps;

char *argname[] =
	{ "arg1","arg2","arg3","arg4","arg5","arg6","arg7","arg8","arg9",
		"arg10",
	"arg11","arg12","arg13","arg14","arg15","arg16","arg17","arg18","arg19",
		"arg20"	} ;
int xors[20];

void csin (double *x, double *xi, double *z, double *zi)
{	*z=cosh(*xi)*sin(*x);
	*zi=sinh(*xi)*cos(*x);
}

void msin (header *hd)
{	spread1i(sin,csin,isin,hd);
	test_error("sin");
}

void ccos (double *x, double *xi, double *z, double *zi)
{	*z=cosh(*xi)*cos(*x);
	*zi=-sinh(*xi)*sin(*x);
}

void mcos (header *hd)
{	spread1i(cos,ccos,icos,hd);
	test_error("cos");
}

void ctan (double *x, double *xi, double *z, double *zi)
{	double s,si,c,ci;
	csin(x,xi,&s,&si); ccos(x,xi,&c,&ci);
	complex_divide(&s,&si,&c,&ci,z,zi);
}

double rtan (double x)
{	if (cos(x)==0.0) return 1e10;
	return tan(x);
}

void mtan (header *hd)
{	spread1i(
#ifdef FLOAT_TEST
	rtan,
#else
	tan,
#endif
	ctan,itan,hd);
	test_error("tan");
}

double ratan (double x)
{	if (x<=-M_PI && x>=M_PI) return 1e10;
	else return atan(x);
}

void carg (double *x, double *xi, double *z)
{
#ifdef FLOAT_TEST
	if (*x==0.0 && *xi==0.0) *z=0.0;
#endif
	*z = atan2(*xi,*x);
}

double rlog (double x)
{	
#ifdef FLOAT_TEST
	if (x<=0) { error=1; return 0; }
	else
#endif
		return log(x);
}

void cclog (double *x, double *xi, double *z, double *zi)
{
#ifdef FLOAT_TEST
	*z=rlog(sqrt(*x * *x + *xi * *xi));
#else
	*z=log(sqrt(*x * *x + *xi * *xi));
#endif
	carg(x,xi,zi);
}

double rsign (double x)
{	if (x<0) return -1;
	else if (x<=0) return 0;
	else return 1;
}

void msign (header *hd)
{	spread1(rsign,0,hd);
	test_error("sign");
}

void catan (double *x, double *xi, double *y, double *yi)
{	double h,hi,g,gi,t,ti;
	h=1-*xi; hi=*x; g=1+*xi; gi=-*x;
	complex_divide(&h,&hi,&g,&gi,&t,&ti);
	cclog(&t,&ti,&h,&hi);
	*y=hi/2; *yi=-h/2;
}

void matan (header *hd)
{	spread1i(
#ifdef FLOAT_TEST
	ratan,
#else
	atan,
#endif
	catan,iatan,hd);
	test_error("atan");
}

double rasin (double x)
{	if (x<-1 || x>1) { error=1; return 0; }
	else return asin(x);
}

void csqrt (double *x, double *xi, double *z, double *zi)
{	double a,r;
	carg(x,xi,&a); a=a/2.0;
	r=sqrt(sqrt(*x * *x + *xi * *xi));
	*z=r*cos(a);
	*zi=r*sin(a);
}

void casin (double *x, double *xi, double *y, double *yi)
{	double h,hi,g,gi;
	complex_multiply(x,xi,x,xi,&h,&hi);
	h=1-h; hi=-hi;
	csqrt(&h,&hi,&g,&gi);
	h=-*xi+g; hi=*x+gi;
	cclog(&h,&hi,yi,y);
	*yi=-*yi;
}

void masin (header *hd)
{	spread1(
#ifdef FLOAT_TEST
	rasin,
#else
	asin,
#endif
	casin,hd);
	test_error("asin");
}

double racos (double x)
{	if (x<-1 || x>1) { error=1; return 0; }
	else return acos(x);
}

void cacos (double *x, double *xi, double *y, double *yi)
{	double h,hi,g,gi;
	complex_multiply(x,xi,x,xi,&h,&hi);
	h=1-h; hi=-hi;
	csqrt(&h,&hi,&g,&gi);
	hi=*xi+g; h=*x-gi;
	cclog(&h,&hi,yi,y);
	*yi=-*yi;
}

void macos (header *hd)
{	spread1(
#ifdef FLOAT_TEST
	racos,
#else
	acos,
#endif
	cacos,hd);
	test_error("acos");
}

void cexp (double *x, double *xi, double *z, double *zi)
{	double r=exp(*x);
	*z=cos(*xi)*r;
	*zi=sin(*xi)*r;
}

void rcexp (double *x, double *xi, double *z, double *zi)
{	double r;
    if (*x>=1000) { error=1; return; }
    r=exp(*x);
	*z=cos(*xi)*r;
	*zi=sin(*xi)*r;
}

inline double rexp (double x)
{	if (x>=1000) { error=1; return 0; }
	else return exp(x);
}

void mexp (header *hd)
{
#ifdef FLOAT_TEST
	spread1i(rexp,rcexp,iexp,hd);
#else
	spread1i(exp,cexp,iexp,hd);
#endif
	test_error("exp");
}

double rarg (double x)
{	if (x>=0) return 0.0;
	else return M_PI;
}

void mlog (header *hd)
{
#ifdef FLOAT_TEST
	spread1i(rlog,cclog,ilog,hd);
#else
	spread1i(log,cclog,ilog,hd);
#endif
	test_error("log");
}

double rsqrt (double x)
{	if (x<0.0) { error=1; return 0; }
	else return sqrt(x);
}

void msqrt (header *hd)
{	spread1i(
#ifdef FLOAT_TEST
	rsqrt,
#else
	sqrt,
#endif
	csqrt,isqrt,hd);
	test_error("sqrt");
}

void mceil (header *hd)
{	spread1(ceil,0,hd);
	test_error("ceil");
}

void mfloor (header *hd)
{	spread1(floor,0,hd);
	test_error("floor");
}

void cconj (double *x, double *xi, double *z, double *zi)
{	*zi=-*xi; *z=*x;
}

double ident (double x)
{	return x;
}

void mconj (header *hd)
{	spread1(ident,cconj,hd);
	test_error("conj");
}

double rnot (double x)
{	if (x!=0.0) return 0.0;
	else return 1.0;
}

void cnot (double *x, double *xi, double *r)
{	if (*x==0.0 && *xi==0.0) *r=1.0;
	else *r=0.0;
}

void mnot (header *hd)
{	spread1r(rnot,cnot,hd);
	test_error("!");
}

void crealpart (double *x, double *xi, double *z)
{	*z=*x;
}

void mre (header *hd)
{	spread1r(ident,crealpart,hd);
	test_error("re");
}

double zero (double x)
{	return 0.0;
}

void cimagpart (double *x, double *xi, double *z)
{	*z=*xi;
}

void mim (header *hd)
{	spread1r(zero,cimagpart,hd);
	test_error("im");
}

void marg (header *hd)
{	spread1r(rarg,carg,hd);
	test_error("arg");
}

void cxabs (double *x, double *xi, double *z)
{	*z=sqrt(*x * *x + *xi * *xi);
}

void mabs (header *hd)
{   spread1ir(fabs,cxabs,iabs,hd);
	test_error("abs");
}

void mpi (header *hd)
{	new_real(M_PI,"");
}

void margn (header *hd)
{	new_real(actargn,"");
}

void mtime (header *hd)
{	new_real(myclock(),"");
}

void mfree (header *hd)
{	new_real(ramend-endlocal,"");
}

#ifndef NOSHRINK

void mshrink (header *hd)
{	header *st=hd,*result;
	ULONG size;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real) wrong_arg_in("shrink");
	if (*realof(hd)>LONG_MAX) wrong_arg_in("shrink");
	size=(ULONG)*realof(hd);
	if (ramend-size<newram)
	{	output("Cannot shrink that much!\n");
		error=171; return;
	}
	if (size)
	{	if (shrink(size)) ramend-=size;
		else
		{	output("Shrink failed!\n"); error=172; return;
		}
	}
	result=new_real(ramend-ramstart,"");
	moveresult(st,result);
}

#endif

void mepsilon (header *hd)
{	new_real(epsilon,"");
}

void msetepsilon (header *hd)
{	header *stack=hd,*hd1,*result;
	hd1=getvalue(hd); if (error) return;
	if (hd1->type!=s_real) wrong_arg_in("setepsilon");
	result=new_real(epsilon,"");
	changedepsilon=epsilon=*realof(hd1);
	moveresult(stack,result);
}

void mlocalepsilon (header *hd)
{	header *stack=hd,*hd1,*result;
	hd1=getvalue(hd); if (error) return;
	if (hd1->type!=s_real) wrong_arg_in("localepsilon");
	result=new_real(epsilon,"");
	epsilon=*realof(hd1);
	moveresult(stack,result);
}

void mindex (header *hd)
{	new_real((double)loopindex,"");
}

void rmod (double *x, double *n, double *y)
{
	if (*n==0.0)
		*y=*x;
	else {
		*y=fmod(*x,*n);
		if (*y<0) *y+=*n;
	}
}

void mmod (header *hd)
{	spreadf2(rmod,0,0,hd);
	test_error("mod");
}

void cpow (double *x, double *xi, double *y, double *yi,
	double *z, double *zi)
{	double l,li,w,wi;
	if (fabs(*x)<epsilon && fabs(*xi)<epsilon)
	{	*z=*zi=0.0; return;
	}
	cclog(x,xi,&l,&li);
	complex_multiply(y,yi,&l,&li,&w,&wi);
	cexp(&w,&wi,z,zi);
}

void rpow (double *x, double *y, double *z)
{	int n;
	if (*x>0.0) *z=pow(*x,*y);
	else if (*x==0.0) if (*y==0.0) *z=1.0; else *z=0.0;
	else
	{	n=(int)*y;
		if (n!=*y)
		{   output("Illegal argument for ^\n"); error=1;
			return;
		}
		if (n%2) *z=-pow(-*x,n);
		else *z=pow(-*x,n);
	}
}

void mpower (header *hd)
{	spreadf2(rpow,cpow,ipow,hd);
	test_error("^");
}

void rgreater (double *x, double *y, double *z)
{	if (*x>*y) *z=1.0;
	else *z=0.0;
}

void igreater (double *xa, double *xb, double *ya, double *yb,
	double *z)
{	if (*xa>*yb) *z=1.0;
	else *z=0.0;
}

void mgreater (header *hd)
{   header *st=hd,*hd1,*result,*hdv;
	hdv=getvariable(hd);
	if (hdv->type==s_string)
	{   hd=getvalue(hd);
		hd1=getvalue(nextof(st)); if (error) return;
		if (hd1->type!=s_string) wrong_arg_in("==");
		result=new_real(strcmp(stringof(hd),stringof(hd1))>0,"");
		moveresult(st,result);
	}
	else spreadf2r(rgreater,0,igreater,st);
	test_error("==");
}

void rless (double *x, double *y, double *z)
{	if (*x<*y) *z=1.0;
	else *z=0.0;
}

void ilesst (double *xa, double *xb, double *ya, double *yb,
	double *z)
{	if (*xb<*ya) *z=1.0;
	else *z=0.0;
}

void mless (header *hd)
{   header *st=hd,*hd1,*result,*hdv;
	hdv=getvariable(hd);
	if (hdv->type==s_string)
	{   hd=getvalue(hd);
		hd1=getvalue(nextof(st)); if (error) return;
		if (hd1->type!=s_string) wrong_arg_in("==");
		result=new_real(strcmp(stringof(hd),stringof(hd1))<0,"");
		moveresult(st,result);
	}
	else spreadf2r(rless,0,ilesst,st);
	test_error("<");
}

void rgreatereq (double *x, double *y, double *z)
{	if (*x>=*y) *z=1.0;
	else *z=0.0;
}

void igreatereq (double *xa, double *xb, double *ya, double *yb,
	double *z)
{	if (*xa>=*yb) *z=1.0;
	else *z=0.0;
}

void mgreatereq (header *hd)
{   header *st=hd,*hd1,*result,*hdv;
	hdv=getvariable(hd);
	if (hdv->type==s_string)
	{   hd=getvalue(hd);
		hd1=getvalue(nextof(st)); if (error) return;
		if (hd1->type!=s_string) wrong_arg_in("==");
		result=new_real(strcmp(stringof(hd),stringof(hd1))>=0,"");
		moveresult(st,result);
	}
	else spreadf2r(rgreatereq,0,igreatereq,st);
	test_error(">=");
}

void rlesseq (double *x, double *y, double *z)
{	if (*x<=*y) *z=1.0;
	else *z=0.0;
}

void ilesseq (double *xa, double *xb, double *ya, double *yb,
	double *z)
{	if (*xb<=*ya) *z=1.0;
	else *z=0.0;
}

void mlesseq (header *hd)
{   header *st=hd,*hd1,*result,*hdv;
	hdv=getvariable(hd);
	if (hdv->type==s_string)
	{   hd=getvalue(hd);
		hd1=getvalue(nextof(st)); if (error) return;
		if (hd1->type!=s_string) wrong_arg_in("==");
		result=new_real(strcmp(stringof(hd),stringof(hd1))<=0,"");
		moveresult(st,result);
	}
	else spreadf2r(rlesseq,0,ilesseq,st);
	test_error("<=");
}

void ror (double *x, double *y, double *z)
{	if (*x!=0.0 || *y!=0.0) *z=1.0;
	else *z=0.0;
}

void mor (header *hd)
{	spreadf2(ror,0,ior,hd);
	test_error("||");
}

void mintersects (header *hd)
{	spreadf2r(0,0,iintersects,hd);
	test_error("intersects");
}

void rrand (double *x, double *y, double *z)
{	if (*x!=0.0 && *y!=0.0) *z=1.0;
	else *z=0.0;
}

void mand (header *hd)
{	spreadf2(rrand,0,iand,hd);
	test_error("&&");
}

void requal (double *x, double *y, double *z)
{	if (*x==*y) *z=1.0;
	else *z=0.0;
}

void cequal (double *x, double *xi, double *y, double *yi, double *z)
{	if (*x==*y && *xi==*yi) *z=1.0;
	else *z=0.0;
}

void mtype (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	result=new_real(hd->type,"");
	moveresult(st,result);
}

void mequal (header *hd)
{   header *st=hd,*hd1,*result,*hdv;
	hdv=getvariable(hd);
	if (hdv->type==s_string)
	{   hd=getvalue(hd);
		hd1=getvalue(nextof(st)); if (error) return;
		if (hd1->type!=s_string) wrong_arg_in("==");
		result=new_real(strcmp(stringof(hd),stringof(hd1))==0,"");
		moveresult(st,result);
	}
	else spreadf2r(requal,cequal,cequal,st);
	test_error("==");
}

void runequal (double *x, double *y, double *z)
{	if (*x!=*y) *z=1.0;
	else *z=0.0;
}

void cunequal (double *x, double *xi, double *y, double *yi, double *z)
{	if (*x!=*y || *xi!=*yi) *z=1.0;
	else *z=0.0;
}

void iunequal (double *x, double *xi, double *y, double *yi, double *z)
{	if (*x!=*y || *xi!=*yi) *z=1.0;
	else *z=0.0;
}

void munequal (header *hd)
{   header *st=hd,*hd1,*result,*hdv;
	hdv=getvariable(hd);
	if (hdv->type==s_string)
	{   hd=getvalue(hd);
		hd1=getvalue(nextof(st)); if (error) return;
		if (hd1->type!=s_string) wrong_arg_in("==");
		result=new_real(strcmp(stringof(hd),stringof(hd1))!=0,"");
		moveresult(st,result);
	}
	else spreadf2r(runequal,cunequal,iunequal,st);
	test_error("<>");
}

void raboutequal (double *x, double *y, double *z)
{   double rx=fabs(*x),ry=fabs(*y);
	if (rx<epsilon)
	{	if (ry<epsilon) *z=1.0;
		else *z=0.0;
	}
	else if (ry<epsilon)
	{	if (rx<epsilon) *z=1.0;
		else *z=0.0;
	}
	else if (fabs(*x-*y)/rx<epsilon) *z=1.0;
	else *z=0.0;
}

void caboutequal
	(double *x, double *xi, double *y, double *yi, double *z)
{	raboutequal(x,y,z);
	if (*z!=0.0) raboutequal(xi,yi,z);
}

void iaboutequal (double *x, double *xi, double *y, double *yi, double *z)
{	raboutequal(x,y,z);
	if (*z!=0.0) raboutequal(xi,yi,z);
}

void maboutequal (header *hd)
{	spreadf2r(raboutequal,caboutequal,iaboutequal,hd);
	test_error("~=");
}

void mlusolve (header *hd)
{	header *st=hd,*hd1,*result;
	double *m,*m1;
	int r,c,r1,c1;
	hd=getvalue(hd);
	hd1=next_param(st);
	if (hd1) hd1=getvalue(hd1);
	if (error) return;
	if (hd->type==s_matrix || hd->type==s_real)
	{	getmatrix(hd,&r,&c,&m);
		if (hd1->type==s_cmatrix)
		{	make_complex(st);
			mlusolve(st); return;
		}
		if (hd1->type!=s_matrix && hd1->type!=s_real)
			wrong_arg_in("lu");
		getmatrix(hd1,&r1,&c1,&m1);
		if (c!=r || c<1 || r!=r1) wrong_arg_in("lu");
		result=new_matrix(r,c1,""); if (error) return;
		lu_solve(m,r,m1,c1,matrixof(result));
		if (error) return;
		moveresult(st,result);
	}
	else if (hd->type==s_cmatrix || hd->type==s_complex)
	{	getmatrix(hd,&r,&c,&m);
		if (hd1->type==s_matrix || hd1->type==s_real)
		{	make_complex(next_param(st));
			mlusolve(st); return;
		}
		if (hd1->type!=s_cmatrix && hd1->type!=s_complex)
			wrong_arg_in("lu");
		getmatrix(hd1,&r1,&c1,&m1);
		if (c!=r || c<1 || r!=r1) wrong_arg_in("lu");
		result=new_cmatrix(r,c1,""); if (error) return;
		clu_solve(m,r,m1,c1,matrixof(result));
		if (error) return;
		moveresult(st,result);
	}
	else wrong_arg_in("lu");
}

void msolve (header *hd)
{	header *st=hd,*hd1,*result;
	double *m,*m1;
	int r,c,r1,c1;
	hd1=nextof(st);
	equal_params_2(&hd,&hd1);
	if (error) return;
	if (hd->type==s_matrix || hd->type==s_real)
	{	getmatrix(hd,&r,&c,&m);
		if (hd1->type==s_cmatrix)
		{	make_complex(st);
			msolve(st); return;	
		}
		if (hd1->type!=s_matrix && hd1->type!=s_real)
			wrong_arg_in("\\");
		getmatrix(hd1,&r1,&c1,&m1);
		if (c!=r || c<1 || r!=r1) wrong_arg_in("\\");
		result=new_matrix(r,c1,""); if (error) return;
		solvesim(m,r,m1,c1,matrixof(result));
		if (error) return;
		moveresult(st,result);
	}
	else if (hd->type==s_cmatrix || hd->type==s_complex)
	{	getmatrix(hd,&r,&c,&m);
		if (hd1->type==s_matrix || hd1->type==s_real)
		{	make_complex(next_param(st));
			msolve(st); return;
		}
		if (hd1->type!=s_cmatrix && hd1->type!=s_complex)
			wrong_arg_in("\\");
		getmatrix(hd1,&r1,&c1,&m1);
		if (c!=r || c<1 || r!=r1) wrong_arg_in("\\");
		result=new_cmatrix(r,c1,""); if (error) return;
		c_solvesim(m,r,m1,c1,matrixof(result));
		if (error) return;
		moveresult(st,result);
	}
	else if (hd->type==s_imatrix || hd->type==s_interval)
	{	getmatrix(hd,&r,&c,&m);
		if (hd1->type==s_matrix || hd1->type==s_real)
		{	make_interval(next_param(st));
			msolve(st); return;
		}
		if (hd1->type!=s_imatrix && hd1->type!=s_interval)
			wrong_arg_in("\\");
		getmatrix(hd1,&r1,&c1,&m1);
		if (c!=r || c<1 || r!=r1) wrong_arg_in("\\");
		result=new_imatrix(r,c1,""); if (error) return;
		i_solvesim(m,r,m1,c1,matrixof(result));
		if (error) return;
		moveresult(st,result);
	}
	else wrong_arg_in("\\");
}

void mcomplex (header *hd)
{	header *st=hd,*result;
	double *m,*mr;
	LONG i,n;
	int c,r;
	hd=getvalue(hd);
	if (hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,c,""); if (error) return;
		n=(LONG)r*c;
        mr=matrixof(result)+(LONG)2*(n-1);
		m+=n-1;
		for (i=0; i<n; i++)
		{	*mr=*m--; *(mr+1)=0.0; mr-=2;
		}
		moveresult(st,result);
	}
	else if (hd->type==s_real)
	{	result=new_complex(*realof(hd),0.0,""); if (error) return;
		moveresult(st,result);
	}
}

void msum (header *hd)
{	header *st=hd,*result;
	int c,r,i,j;
	double *m,*mr,s,si,x,y;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++) 
		{	s=0.0;
			for (j=0; j<c; j++) s+=*m++;
			*mr++=s;
		}
	}
	else if (hd->type==s_complex || hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++) 
		{	s=0.0; si=0.0;
			for (j=0; j<c; j++) { s+=*m++; si+=*m++; }
			*mr++=s; *mr++=si;
		}
	}
	else if (hd->type==s_interval || hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	s=0.0; si=0.0;
			for (j=0; j<c; j++)
			{	interval_add(&s,&si,m,m+1,&x,&y);
				s=x; si=y; m+=2;
			}
			*mr++=s; *mr++=si;
		}
	}
	else wrong_arg_in("sum");
	moveresult(st,result);
}

void mprod (header *hd)
{	header *st=hd,*result;
	int c,r,i,j;
	double *m,*mr,s,si,h,hi,x,y;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++) 
		{	s=1.0;
			for (j=0; j<c; j++) s*=*m++;
			*mr++=s;
		}
	}
	else if (hd->type==s_complex || hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	s=1.0;
			for (j=0; j<c; j++)
			{	complex_multiply(&s,&si,m,m+1,&h,&hi);
				s=h; si=hi; m+=2;
			}
			*mr++=s; *mr++=si;
		}
	}
	else if (hd->type==s_interval || hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	s=1.0; si=1.0;
			for (j=0; j<c; j++)
			{	interval_mult(&s,&si,m,m+1,&x,&y);
				s=x; si=y; m+=2;
			}
			*mr++=s; *mr++=si;
		}
	}
	else wrong_arg_in("prod");
	moveresult(st,result);
}

void msize (header *hd)
{	header *result,*st=hd,*hd1,*end=(header *)newram;
	int r,c,r0=0,c0=0;
	if (!hd) wrong_arg_in("size");
	result=new_matrix(1,2,""); if (error) return;
	while (end>hd)
	{	hd1=getvariable(hd); if (!hd1) varnotfound("size");
		if (hd1->type==s_matrix || hd1->type==s_cmatrix
			|| hd1->type==s_imatrix)
		{	r=dimsof(hd1)->r;
			c=dimsof(hd1)->c;
		}
		else if (hd1->type==s_real || hd1->type==s_complex
			|| hd1->type==s_interval)
		{	r=c=1;
		}
		else if (hd1->type==s_submatrix || hd1->type==s_csubmatrix
			|| hd1->type==s_isubmatrix)
		{	r=submdimsof(hd1)->r;
			c=submdimsof(hd1)->c;
		}
		else wrong_arg_in("size");
		if ((r>1 && r0>1 && r!=r0) || (c>1 && c0>1 && c!=c0))
		{	if (r0!=r && c0!=c)
			{	output("Matrix dimensions must agree for size!\n");
				error=1021; return;
			}
		}
		else
		{	if (r>r0) r0=r;
			if (c>c0) c0=c;
		}
		hd=nextof(hd);
	}
	*matrixof(result)=r0;
	*(matrixof(result)+1)=c0;
	moveresult(st,result);
}

void mcols (header *hd)
{	header *st=hd,*res;
	int n;
	hd=getvalue(hd); if (error) return;
	switch (hd->type)
	{	case s_matrix :
		case s_cmatrix :
		case s_imatrix : n=dimsof(hd)->c; break;
		case s_submatrix :
		case s_csubmatrix :
		case s_isubmatrix : n=submdimsof(hd)->c; break;
		case s_real :
		case s_complex :
		case s_interval : n=1; break;
		case s_string : n=(int)strlen(stringof(hd)); break;
		default : wrong_arg_in("cols");
	}
	res=new_real(n,""); if (error) return;
	moveresult(st,res);
}

void mrows (header *hd)
{	header *st=hd,*res;
	int n;
	hd=getvalue(hd); if (error) return;
	switch (hd->type)
	{	case s_matrix :
		case s_cmatrix :
		case s_imatrix : n=dimsof(hd)->r; break;
		case s_submatrix :
		case s_csubmatrix :
		case s_isubmatrix : n=submdimsof(hd)->r; break;
		case s_real :
		case s_complex :
		case s_interval : n=1; break;
		default : wrong_arg_in("rows");
	}
	res=new_real(n,""); if (error) return;
	moveresult(st,res);
}

void mzerosmat (header *hd)
{	header *result,*st=hd;
	double rows,cols,*m;
	int r,c;
	LONG i,n;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2)
		wrong_arg_in("zeros");
	rows=*matrixof(hd); cols=*(matrixof(hd)+1);
	if (rows<0 || rows>=INT_MAX || cols<0 || cols>=INT_MAX)
		wrong_arg_in("zeros");
	r=(int)rows; c=(int)cols;
	result=new_matrix(r,c,""); if (error) return;
	m=matrixof(result);
	n=c*r;
	for (i=0; i<n; i++) *m++=0.0;
	moveresult(st,result);
}

void mones (header *hd)
{	header *result,*st=hd;
	double rows,cols,*m;
	int r,c;
	LONG i,n;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2)
		wrong_arg_in("ones");
	rows=*matrixof(hd); cols=*(matrixof(hd)+1);
	if (rows<0 || rows>=INT_MAX || cols<0 || cols>=INT_MAX)
		wrong_arg_in("ones");
	r=(int)rows; c=(int)cols;
	result=new_matrix(r,c,""); if (error) return;
	m=matrixof(result);
	n=c*r;
	for (i=0; i<n; i++) *m++=1.0;
	moveresult(st,result);
}

void mdiag (header *hd)
{	header *result,*st=hd,*hd1,*hd2=0;
	double rows,cols,*m,*md;
	int r,c,i,ik=0,k,rd,cd;
	LONG l,n;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2)
		wrong_arg_in("diag");
	rows=*matrixof(hd); cols=*(matrixof(hd)+1);
	if (rows<0 || rows>=INT_MAX || cols<0 || cols>=INT_MAX)
		wrong_arg_in("diag");
	r=(int)rows; c=(int)cols;
	hd1=next_param(st); if (hd1) hd2=next_param(hd1);
	if (hd1) hd1=getvalue(hd1);
	if (hd2) hd2=getvalue(hd2);
	if (error) return;
	if	(hd1->type!=s_real) wrong_arg_in("diag");
	k=(int)*realof(hd1);
	if (hd2->type==s_matrix || hd2->type==s_real)
	{	result=new_matrix(r,c,""); if (error) return;
		m=matrixof(result);
		n=(LONG)c*r;
		for (l=0; l<n; l++) *m++=0.0;
		getmatrix(hd2,&rd,&cd,&md);
		if (rd!=1 || cd<1) wrong_arg_in("diag");
		m=matrixof(result);
		for (i=0; i<r; i++)
		{	if (i+k>=0 && i+k<c)
			{	*mat(m,c,i,i+k)=*md;
				ik++; if (ik<cd) md++;
			}
		}
	}
	else if (hd2->type==s_cmatrix || hd2->type==s_complex)
	{	result=new_cmatrix(r,c,""); if (error) return;
		m=matrixof(result);
		n=(LONG)2*(LONG)c*r;
		for (l=0; l<n; l++) *m++=0.0;
		getmatrix(hd2,&rd,&cd,&md);
		if (rd!=1 || cd<1) wrong_arg_in("diag");
		m=matrixof(result);
		for (i=0; i<r; i++)
		{	if (i+k>=0 && i+k<c)
			{	*cmat(m,c,i,i+k)=*md;
				*(cmat(m,c,i,i+k)+1)=*(md+1);
				ik++; if (ik<cd) md+=2;
			}
		}
	}
	else if (hd2->type==s_imatrix || hd2->type==s_interval)
	{	result=new_imatrix(r,c,""); if (error) return;
		m=matrixof(result);
		n=(LONG)2*(LONG)c*r;
		for (l=0; l<n; l++) *m++=0.0;
		getmatrix(hd2,&rd,&cd,&md);
		if (rd!=1 || cd<1) wrong_arg_in("diag");
		m=matrixof(result);
		for (i=0; i<r; i++)
		{	if (i+k>=0 && i+k<c)
			{	*imat(m,c,i,i+k)=*md;
				*(imat(m,c,i,i+k)+1)=*(md+1);
				ik++; if (ik<cd) md+=2;
			}
		}
	}
	else wrong_arg_in("diag");
	moveresult(st,result);
}

void msetdiag (header *hd)
{	header *result,*st=hd,*hd1,*hd2=0;
	double *m,*md,*mhd;
	int r,c,i,ik=0,k,rd,cd;
	hd=getvalue(st); if (error) return;
	if (hd->type!=s_matrix && hd->type!=s_cmatrix)
		wrong_arg_in("setdiag");
	getmatrix(hd,&c,&r,&mhd);
	hd1=next_param(st); if (hd1) hd2=next_param(hd1);
	if (hd1) hd1=getvalue(hd1);
	if (hd2) hd2=getvalue(hd2);
	if (error) return;
	if	(hd1->type!=s_real) wrong_arg_in("setdiag");
	k=(int)*realof(hd1);
	if (hd->type==s_matrix &&
			(hd2->type==s_complex || hd2->type==s_cmatrix))
		{	make_complex(st); msetdiag(st); return;
		}
	else if (hd->type==s_cmatrix &&
			(hd2->type==s_real || hd2->type==s_matrix))
		{	make_complex(nextof(nextof(st))); msetdiag(st); return;
		}
	else if (hd->type==s_imatrix &&
			(hd2->type==s_real || hd2->type==s_matrix))
		{	make_interval(nextof(nextof(st))); msetdiag(st); return;
		}
	if (hd->type==s_matrix)
	{	result=new_matrix(r,c,""); if (error) return;
		m=matrixof(result);
		memmove((char *)m,(char *)mhd,(LONG)c*r*sizeof(double));
		getmatrix(hd2,&rd,&cd,&md);
		if (rd!=1 || cd<1) wrong_arg_in("setdiag");
		for (i=0; i<r; i++)
		{	if (i+k>=0 && i+k<c)
			{	*mat(m,c,i,i+k)=*md;
				ik++; if (ik<cd) md++;
			}
		}
	}
	else if (hd->type==s_cmatrix)
	{	result=new_cmatrix(r,c,""); if (error) return;
		m=matrixof(result);
        memmove((char *)m,(char *)mhd,(LONG)c*r*(LONG)2*sizeof(double));
		getmatrix(hd2,&rd,&cd,&md);
		if (rd!=1 || cd<1) wrong_arg_in("setdiag");
		m=matrixof(result);
		for (i=0; i<r; i++)
		{	if (i+k>=0 && i+k<c) 
			{	*cmat(m,c,i,i+k)=*md;
				*(cmat(m,c,i,i+k)+1)=*(md+1);
				ik++; if (ik<cd) md+=2;
			}
		}
	}
	else wrong_arg_in("setdiag");
	moveresult(st,result);
}

void mextrema (header *hd)
{	header *result,*st=hd;
	double x,*m,*mr,min,max;
	int r,c,i,j,imin,imax;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,4,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++) 
		{	min=max=*m; imin=imax=0; m++;
			for (j=1; j<c; j++) 
			{	x=*m++;
				if (x<min) { min=x; imin=j; }
				if (x>max) { max=x; imax=j; }
			}
			*mr++=min; *mr++=imin+1; *mr++=max; *mr++=imax+1;
		}
	}
	else wrong_arg_in("extrema");
	moveresult(st,result);
}

void mcumsum (header *hd)
{	header *result,*st=hd;
	double *m,*mr,sum=0,sumr=0,sumi=0;
	int r,c,i,j;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		if (c<1) result=new_matrix(r,1,"");
		else result=new_matrix(r,c,"");
		if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++) 
		{	if (c>=1) sum=*m++;
			*mr++=sum;
			for (j=1; j<c; j++) 
			{	sum+=*m++;
				*mr++=sum;
			}
		}
	}
	else if (hd->type==s_complex || hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		if (c<1) result=new_cmatrix(r,1,"");
		else result=new_cmatrix(r,c,"");
		if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	if (c>=1) { sumr=*m++; sumi=*m++; }
			*mr++=sumr; *mr++=sumi;
			for (j=1; j<c; j++) 
			{	sumr+=*m++; *mr++=sumr;
				sumi+=*m++; *mr++=sumi;
			}
		}
	}
	else wrong_arg_in("cumsum");
	moveresult(st,result);
}

void mcumprod (header *hd)
{	header *result,*st=hd;
	double *m,*mr,sum=1,sumi=1,sumr=0,x,y;
	int r,c,i,j;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		if (c<1) result=new_matrix(r,1,"");
		else result=new_matrix(r,c,"");
		if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++) 
		{	if (c>=1) sum=*m++; 
			*mr++=sum;
			for (j=1; j<c; j++) 
			{	sum*=*m++;
				*mr++=sum;
			}
		}
	}
	else if (hd->type==s_complex || hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		if (c<1) result=new_cmatrix(r,1,"");
		else result=new_cmatrix(r,c,"");
		if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	if (c>=1) { sumr=*m++; sumi=*m++; }
			*mr++=sumr; *mr++=sumi;
			for (j=1; j<c; j++)
			{	sum=sumr*(*m)-sumi*(*(m+1));
				sumi=sumr*(*(m+1))+sumi*(*m);
				sumr=sum;
				m+=2;
				*mr++=sumr;
				*mr++=sumi;
			}
		}
	}
	else if (hd->type==s_interval || hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		if (c<1) result=new_imatrix(r,1,"");
		else result=new_imatrix(r,c,"");
		if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	if (c>=1) { sumr=*m++; sumi=*m++; }
			*mr++=sumr; *mr++=sumi;
			for (j=1; j<c; j++)
			{   interval_mult(m,m+1,&sumr,&sumi,&x,&y);
				sumr=x; sumi=y;
				m+=2;
				*mr++=sumr;
				*mr++=sumi;
			}
		}
	}
	else if (hd->type==s_interval || hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		if (c<1) result=new_imatrix(r,1,"");
		else result=new_imatrix(r,c,"");
		if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	if (c>=1) { sumr=*m++; sumi=*m++; }
			*mr++=sumr; *mr++=sumi;
			for (j=1; j<c; j++)
			{   interval_add(m,m+1,&sumr,&sumi,&x,&y);
				sumr=x; sumi=y;
				m+=2;
				*mr++=sumr;
				*mr++=sumi;
			}
		}
	}
	else wrong_arg_in("cumprod");
	moveresult(st,result);
}

void mwait (header *hd)
{	header *st=hd,*result;
	double now;
	int h;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real) wrong_arg_in("wait");
	now=myclock();
	sys_wait(*realof(hd),&h);
	if (h==escape) { output("Interrupt\n"); error=1; return; }
	now=myclock()-now;
	if (h==0 || now>*realof(hd)) now=*realof(hd);
	if (h!=0 && now>=*realof(hd)) now=*realof(hd)*0.999;
	result=new_real(now,"");
	if (error) return;
	moveresult(st,result);
}

void mkey (header *hd)
{	int scan,key;
	key=wait_key(&scan);
	if (scan==escape) { output("Interrupt\n"); error=1; return; }
	if (key) new_real(key,"");
	else new_real(scan,"");
}

void mcode (header *hd)
{	new_real(test_code(),"");
}

void mformat (header *hd)
{	header *st=hd,*result;
	static int l=10,d=5;
	int oldl=l,oldd=d;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2)
		wrong_arg_in("format");
	l=(int)*matrixof(hd); d=(int)*(matrixof(hd)+1);
	if (l<2 || l>80 || d<0 || d>(DBL_DIG+4))
		wrong_arg_in("format");
	if (d>l-3) d=l-3;
	outputlength=l;
	if (outputlength>64) outputlength=64;
	if (outputlength<1) outputlength=1;
	sprintf(fixedformat,"%%0.%df",d);
	sprintf(expoformat,"%%0.%de",d);
	minexpo=pow(10,-d);
	maxexpo=pow(10,l-d-3);
	fieldw=l+1;
	linew=linelength/fieldw;
	if (linew<=0) linew=1;
	if (iformat==0)
	{	ioutputlength=2*outputlength;
		ilinew=linelength/ioutputlength;
		if (ilinew<=0) ilinew=1;
	}
	result=new_matrix(1,2,""); if (error) return;
	*matrixof(result)=oldl;
	*(matrixof(result)+1)=oldd;
	moveresult(st,result);
}

void mgformat (header *hd)
{	header *st=hd,*result;
	static int l=10,d=5;
	int oldl=l,oldd=d;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2)
		wrong_arg_in("goodformat");
	l=(int)*matrixof(hd); d=(int)*(matrixof(hd)+1);
	if (l<2 || l>80 || d<0 || d>(DBL_DIG+4))
		wrong_arg_in("goodformat");
	if (d>l-3) d=l-3;
	outputlength=l;
	precission=d;
	if (outputlength>64) outputlength=64;
	if (outputlength<1) outputlength=1;
	sprintf(fixedformat,"%%0.%dg",d+1);
	sprintf(expoformat,"%%0.%dg",d+1);
	minexpo=pow(10,-d);
	maxexpo=pow(10,l-d-3);
	fieldw=l+1;
	linew=linelength/fieldw;
	if (linew<=0) linew=1;
	if (iformat==0)
	{	ioutputlength=2*outputlength;
		ilinew=linelength/ioutputlength;
		if (ilinew<=0) ilinew=1;
	}
	result=new_matrix(1,2,""); if (error) return;
	*matrixof(result)=oldl;
	*(matrixof(result)+1)=oldd;
	moveresult(st,result);
	fraceps=0;
}

void meformat (header *hd)
{	header *st=hd,*result;
	static int l=10,d=5;
	int oldl=l,oldd=d;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2)
		wrong_arg_in("expformat");
	l=(int)*matrixof(hd); d=(int)*(matrixof(hd)+1);
	if (l<2 || l>80 || d<0 || d>(DBL_DIG+4))
		wrong_arg_in("expformat");
	if (d>l-3) d=l-3;
	outputlength=l;
	precission=d;
	if (outputlength>64) outputlength=64;
	if (outputlength<1) outputlength=1;
	sprintf(fixedformat,"%%0.%de",d);
	sprintf(expoformat,"%%0.%de",d);
	minexpo=pow(10,-d);
	maxexpo=pow(10,l-d-3);
	fieldw=l+1;
	linew=linelength/fieldw;
	if (linew<=0) linew=1;
	if (iformat==0)
	{	ioutputlength=2*outputlength;
		ilinew=linelength/ioutputlength;
		if (ilinew<=0) ilinew=1;
	}
	result=new_matrix(1,2,""); if (error) return;
	*matrixof(result)=oldl;
	*(matrixof(result)+1)=oldd;
	moveresult(st,result);
	fraceps=0;
}

void mfformat (header *hd)
{	header *st=hd,*result;
	static int l=10,d=5;
	int oldl=l,oldd=d;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2)
		wrong_arg_in("fixedformat");
	l=(int)*matrixof(hd); d=(int)*(matrixof(hd)+1);
	if (l<2 || l>80 || d<0 || d>(DBL_DIG+4))
		wrong_arg_in("fixedformat");
	if (d>l-3) d=l-3;
	outputlength=l;
	precission=d;
	if (outputlength>64) outputlength=64;
	if (outputlength<1) outputlength=1;
	sprintf(fixedformat,"%%0.%df",d);
	sprintf(expoformat,"%%0.%df",d);
	minexpo=pow(10,-d);
	maxexpo=pow(10,l-d-3);
	fieldw=l+2;
	linew=linelength/fieldw;
	if (linew<=0) linew=1;
	if (iformat==0)
	{	ioutputlength=2*outputlength;
		ilinew=linelength/ioutputlength;
		if (ilinew<=0) ilinew=1;
	}
	result=new_matrix(1,2,""); if (error) return;
	*matrixof(result)=oldl;
	*(matrixof(result)+1)=oldd;
	moveresult(st,result);
	fraceps=0;
}

void miformat (header *hd)
{	header *st=hd,*result;
	int oldi=iformat,k;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real) wrong_arg_in("iformat");
	k=(int)(*realof(hd));
	if (k>0)
	{   iformat=k;
		if (k>80) k=80;
		if (k<21) k=21;
		iprecission=(k-3)/2-7;
		ioutputlength=k;
		ilinew=1;
		if (ilinew<=0) ilinew=1;
	}
	else
	{   iformat=0;
		ilinew=linew/2;
		if (ilinew<=0) ilinew=1;
		ioutputlength=outputlength*2;
	}
	result=new_real(oldi,""); if (error) return;
	moveresult(st,result);
	fraceps=0;
}

void mfracformat (header *hd)
{	header *st=hd,*result;
	int oldl=outputlength,l;
	double oldeps=fraceps,eps;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2)
		wrong_arg_in("fracformat");
	l=(int)*matrixof(hd);
	eps=*(matrixof(hd)+1);
	if (l<2 || l>80 || eps<=0 || eps>1)
		wrong_arg_in("fracformat");
	outputlength=l;
	fieldw=l+1;
	linew=linelength/fieldw;
	if (linew<=0) linew=1;
	result=new_matrix(1,2,""); if (error) return;
	*matrixof(result)=oldl;
	*(matrixof(result)+1)=oldeps;
	moveresult(st,result);
    fraceps=eps;
}

void mfastrandom (header *hd)
{	header *st=hd,*result;
	double *m;
	int r,c;
	LONG k,n;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2
		|| *(m=matrixof(hd))<0 || *m>=INT_MAX
		|| *(m+1)<0 || *(m+1)>INT_MAX)
		wrong_arg_in("random");
	r=(int)*m;
	c=(int)*(m+1);
	result=new_matrix(r,c,""); if (error) return;
	m=matrixof(result);
	n=(LONG)c*r;
	for (k=0; k<n; k++) *m++=(double)rand()/(double)RAND_MAX;
	moveresult(st,result);
}

void mnormal (header *hd)
{	header *st=hd,*result;
	double *m,r1,r2;
	int r,c;
	LONG k,n;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1 || dimsof(hd)->c!=2
		|| *(m=matrixof(hd))<0 || *m>=INT_MAX 
		|| *(m+1)<0 || *(m+1)>INT_MAX)
		wrong_arg_in("normal");
	r=(int)*m;
	c=(int)*(m+1);
	result=new_matrix(r,c,""); if (error) return;
	m=matrixof(result);
	n=(LONG)c*r;
	for (k=0; k<n; k++) 
	{	r1=(double)rand()/(double)RAND_MAX;
		if (r1==0.0) *m++=0.0;
		else
		{	r2=(double)rand()/(double)RAND_MAX;
			*m++=sqrt(-2*log(r1))*cos(2*M_PI*r2);
		}
	}
	moveresult(st,result);
}

double rfak (double x)
{	int i,n;
	double res=1;
	if (x<2 || x>INT_MAX) return 1.0;
	n=(int)x;
	for (i=2; i<=n; i++) res=res*i;
	return res;
}

void mfak (header *hd)
{	spread1(rfak,0,hd);
	test_error("fak");
}

double rlogfak (double x)
{	int i,n;
	double res=0;
	if (x<2 || x>INT_MAX) return 0.0;
	n=(int)x;
	for (i=2; i<=n; i++) res=res+log(i);
	return res;
}

void mlogfak (header *hd)
{	spread1(rlogfak,0,hd);
	test_error("logfak");
}

void rbin (double *x, double *y, double *z)
{   long i,n,m,k;
	double res;
	n=(long)*x; m=(long)*y;
	if (m<=0) *z=1.0;
	else
	{	res=k=n-m+1;
		for (i=2; i<=m; i++) { k++; res=res*k/i; }
		*z=res;
	}
}

void mbin (header *hd)
{	spreadf2(rbin,0,0,hd);
	test_error("bin");
}

void rlogbin (double *x, double *y, double *z)
{   long i,n,m,k;
	double res;
	n=(long)*x; m=(long)*y;
	if (m<=0) *z=0.0;
	else
	{   k=n-m+1;
		res=log(n-m+1);
		for (i=2; i<=m; i++) { k++; res+=log(k)-log(i); }
		*z=res;
	}
}

void mlogbin (header *hd)
{	spreadf2(rlogbin,0,0,hd);
	test_error("bin");
}

void rtd (double *xa, double *yf, double *zres)
{	double t,t1,a,b,h,z,p,y,x;
	int flag=0;
	if (fabs(*xa)<epsilon) { *zres=0.5; return; }
	if (*xa<0) flag=1;
	t=*xa * *xa;
	if (t>=1) { a=1; b=*yf; t1=t; }
	else { a=*yf; b=1; t1=1/t; }
	y=2/(9*a); z=2/(9*b);
	h=pow(t1,1.0/3);
	x=fabs((1-z)*h-1+y)/sqrt(z*h*h+y);
	if (b<4) x=x*(1+0.08*x*x*x*x/(b*b*b));
	h=1+x*(0.196854+x*(0.115194+x*(0.000344+x*0.019527)));
	p=0.5/(h*h*h*h);
	if (t<0.5) *zres=p/2+0.5;
	else *zres=1-p/2;
	if (flag) *zres=1-*zres;
}

void mtd (header *hd)
{	spreadf2(rtd,0,0,hd);
	test_error("tdis");
}

double invgauss (double a)
{	double t,c,d;
	int flag=0;
	if (a<0.5) { a=1-a; flag=1; }
	t=sqrt(-2*log(fabs(1-a)));
	c=2.515517+t*(0.802853+t*0.010328);
	d=1+t*(1.432788+t*(0.189269+t*0.001308));
	if (flag) return (c/d-t);
	else return t-c/d;
}

void minvgauss (header *hd)
{	spread1(invgauss,0,hd);
	test_error("invnormaldis");
}

void invrtd (double *x, double *y, double *zres)
{	double z=*x,f=*y,g1,g2,g3,g4,z2;
	int flag=0;
	if (z<0.5) { flag=1; z=1-z; }
	z=invgauss(z);
	z2=z*z;
	g1=z*(1+z2)/4.0;
	g2=z*(3+z2*(16+5*z2))/96.0;
	g3=z*(-15+z2*(17+z2*(19+z2*3)))/384.0;
	g4=z*(-945+z2*(-1920+z2*(1482+z2*(776+z2*79))))/92160.0;
	*zres=(((g4/f+g3)/f+g2)/f+g1)/f+z;
	if (flag) *zres=-*zres;
}

void minvtd (header *hd)
{	spreadf2(invrtd,0,0,hd);
	test_error("invtdis");
}

void chi (double *xa, double *yf, double *zres)
{	double ch=*xa,x,y,s,t,g,j=1;
	long i,p,f;
	f=(long)*yf;
	if (ch<epsilon) { *zres=0.0; return; }
	p=(f+1)/2;
	for (i=f; i>=2; i-=2) j=j*i;
	x=pow(ch,p)*exp(-(ch/2))/j;
	if (f%2==0) y=1;
	else y=sqrt(2/(ch*M_PI));
	s=1; t=1; g=f;
	while (t>1e-5)
	{	g=g+2;
		t=t*ch/g;
		s=s+t;
	}
	*zres=x*y*s;
}

void mchi (header *hd)
{	spreadf2(chi,0,0,hd);
	test_error("chidis");
}

double f1,f2;

double rfd (double F)
{	double f0,a,b,h,z,p,y,x;
	if (F<epsilon) return 0.0;
	if (F<1) { a=f2; b=f1; f0=1/F; }
	else { a=f1; b=f2; f0=F; }
	y=2/(9*a); z=2/(9*b);
	h=pow(f0,1.0/3);
	x=fabs((1-z)*h-1+y)/sqrt(z*h*h+y);
	if (b<4) x=x*(1+0.08*x*x*x*x/(b*b*b));
	h=1+x*(0.196854+x*(0.115194+x*(0.000344+x*0.019527)));
	p=0.5/(h*h*h*h);
	if (F>=1) return 1-p;
	else return p;
}

void mfdis (header *hd)
{	header *st=hd,*hd1,*hd2=0;
	hd1=next_param(st);
	if (hd1)
	{	hd2=next_param(hd1);
		hd1=getvalue(hd1);
	}
	if (hd2) hd2=getvalue(hd2);
	if (error) return;
	if (hd1->type!=s_real || hd2->type!=s_real) wrong_arg_in("fdis");
	f1=*realof(hd1);
	f2=*realof(hd2);
	spread1(rfd,0,hd);
	test_error("fdis");
}

void rmax (double *x, double *y, double *z)
{	if (*x>*y) *z=*x;
	else *z=*y;
}

void mmax (header *hd)
{   spreadf2(rmax,0,imax,hd);
	test_error("max");
}

void rmin (double *x, double *y, double *z)
{	if (*x>*y) *z=*y;
	else *z=*x;
}

void mmin (header *hd)
{	spreadf2(rmin,0,imin,hd);
	test_error("min");
}

typedef struct { double val; int ind; } sorttyp;

int sorttyp_compare (const sorttyp *x, const sorttyp *y)
{	if (x->val>y->val) return 1;
	else if (x->val==y->val) return 0;
	else return -1;
}

void msort (header *hd)
{	header *st=hd,*result,*result1;
	double *m,*m1;
	sorttyp *t;
	int r,c,i;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real && hd->type!=s_matrix) wrong_arg_in("sort");
	getmatrix(hd,&r,&c,&m);
	if (c==1 || r==1) result=new_matrix(r,c,"");
	else wrong_arg_in("sort");
	if (error) return;
	result1=new_matrix(r,c,"");
	if (error) return;
	if (c==1) c=r;
	if (c==0) wrong_arg_in("sort");
	if (!freeram(c*sizeof(sorttyp)))
	{	output("Out of memory!\n"); error=600; return; 
	}
	t=(sorttyp *)newram;
	for (i=0; i<c; i++)
	{	t->val=*m++; t->ind=i; t++;
	}
	qsort(newram,c,sizeof(sorttyp),
		(int (*) (const void *, const void *))sorttyp_compare);
	m=matrixof(result); m1=matrixof(result1);
	t=(sorttyp *)newram;
	for (i=0; i<c; i++)
	{	*m++=t->val; *m1++=t->ind+1; t++;
	}
	moveresult(st,result);
	moveresult(nextof(st),result1);
}

void mnonzeros (header *hd)
{	header *st=hd,*result;
	double *m,*mr;
	int r,c,i,k;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real && hd->type!=s_matrix) wrong_arg_in("nonzeros");
	getmatrix(hd,&r,&c,&m);
	if (r!=1 && c!=1) wrong_arg_in("nonzeros");
	if (c==1) c=r;
	result=new_matrix(1,c,""); if (error) return;
	k=0; mr=matrixof(result);
	for (i=0; i<c; i++)
	{	if (*m++!=0.0)
		{	*mr++=i+1; k++;
		}
	}
	dimsof(result)->c=k;
	result->size=matrixsize(1,k);
	moveresult(st,result);
}

void mstatistics (header *hd)
{	header *st=hd,*hd1,*result;
	int i,n,r,c,k;
	double *m,*mr;
	hd=getvalue(hd);
	hd1=next_param(st);
	if (hd1) hd1=getvalue(hd1); if (error) return;
	if (hd1->type!=s_real || hd->type!=s_matrix) wrong_arg_in("count");
	if (*realof(hd1)>INT_MAX || *realof(hd1)<2) wrong_arg_in("count");
	n=(int)*realof(hd1);
	getmatrix(hd,&r,&c,&m);
	if (r!=1 && c!=1) wrong_arg_in("count");
	if (c==1) c=r;
	result=new_matrix(1,n,""); if (error) return;
	mr=matrixof(result); for (i=0; i<n; i++) *mr++=0.0;
	mr=matrixof(result);
	for (i=0; i<c; i++)
	{	if (*m>=0 && *m<n)
		{	k=(int)floor(*m);
			mr[k]+=1.0;
		}
		m++;
	}
	moveresult(st,result);
}

void minput (header *hd)
{	header *st=hd,*result;
	char input[1024],*oldnext;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("input");
	retry: output(stringof(hd)); output("? ");
	nojump=1;
	edit(input);
	stringon=1;
	oldnext=next; next=input; result=scan_value(); next=oldnext;
	stringon=0;
	if (error) 
	{	output("Error in input!\n"); error=0; goto retry;
	}
	moveresult(st,result);
}

void mlineinput (header *hd)
{	header *st=hd,*result;
	char input[1024];
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("lineinput");
	nojump=1;
	output(stringof(hd)); output("? ");
	edit(input);
	result=new_string(input,strlen(input),"");
	moveresult(st,result);
}

void minterpret (header *hd)
{	header *st=hd,*result;
	char *oldnext;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("interpret");
	stringon=1;
	oldnext=next; next=stringof(hd); result=scan(); next=oldnext;
	stringon=0;
    if (error) { result=new_string("Error",8,""); error=0; }
	moveresult(st,result);
}

void mevaluate (header *hd)
{	header *st=hd,*result;
	char *oldnext;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("evaluate");
	stringon=1;
	oldnext=next; next=stringof(hd); result=scan(); next=oldnext;
	stringon=0;
    if (error) { result=new_string("Syntax error!",16,""); }
	moveresult(st,result);
}

void mmax1 (header *hd)
{	header *result,*st=hd;
	double x,*m,*mr,max,max1;
	int r,c,i,j;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	max=*m++;
			for (j=1; j<c; j++)
			{	x=*m++;
				if (x>max) max=x;
			}
			*mr++=max;
		}
	}
	else if (hd->type==s_interval || hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	max=*m++;
			max1=*m++;
			for (j=1; j<c; j++)
			{	imax(&max,&max1,m,m+1,&max,&max1);
				m+=2;
			}
			*mr++=max;
			*mr++=max1;
		}
	}
	else wrong_arg_in("max");
	moveresult(st,result);
}

void mmin1 (header *hd)
{	header *result,*st=hd;
	double x,*m,*mr,max=0,max1=0;
	int r,c,i,j;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++) 
		{	max=*m; m++;
			for (j=1; j<c; j++) 
			{	x=*m++;
				if (x<max) max=x;
			}
			*mr++=max;
		}
	}
	else if (hd->type==s_interval || hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	max=*m++;
			max1=*m++;
			for (j=1; j<c; j++)
			{	imin(&max,&max1,m,m+1,&max,&max1);
				m+=2;
			}
			*mr++=max;
			*mr++=max1;
		}
	}
	else wrong_arg_in("min");
	moveresult(st,result);
}

void make_xors (void)
{	int i;
	for (i=0; i<20; i++) xors[i]=xor(argname[i]);
}

void mdo (header *hd)
{	header *st=hd,*hd1,*result;
	int count=0;
	ULONG size;
	if (!hd) wrong_arg_in("do");
	hd=getvalue(hd);
	result=hd1=next_param(st);
	if (hd->type!=s_string) wrong_arg_in("do");
	if (error) return;
	hd=searchudf(stringof(hd));
	if (!hd || hd->type!=s_udf) wrong_arg_in("do");
	while (hd1)
	{	strcpy(hd1->name,argname[count]);
		hd1->xor=xors[count];
		hd1=next_param(hd1); count++;
	}
	if (result)
	{	size=(char *)result-(char *)st;
		if (size>0 && newram!=(char *)result)
			memmove((char *)st,(char *)result,newram-(char *)result);
		newram-=size;
	}
	interpret_udf(hd,st,count,0);
}

void mlu (header *hd)
{	header *st=hd,*result,*res1,*res2,*res3;
	double *m,*mr,*m1,*m2,det,deti;
	int r,c,*rows,*cols,rank,i;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_matrix || hd->type==s_real)
	{	getmatrix(hd,&r,&c,&m);
		if (r<1) wrong_arg_in("lu");
		result=new_matrix(r,c,""); if (error) return;
		mr=matrixof(result);
		memmove((char *)mr,(char *)m,(LONG)r*c*sizeof(double));
		make_lu(mr,r,c,&rows,&cols,&rank,&det); if (error) return;
		res1=new_matrix(1,rank,""); if (error) return;
		res2=new_matrix(1,c,""); if (error) return;
		res3=new_real(det,""); if (error) return;
		m1=matrixof(res1);
		for (i=0; i<rank; i++)
		{	*m1++=*rows+1;
			rows++;
		}
		m2=matrixof(res2);
		for (i=0; i<c; i++)
		{	*m2++=*cols++;
		}
		moveresult(st,getvalue(result)); st=nextof(st);
		moveresult(st,getvalue(res1)); st=nextof(st);
		moveresult(st,getvalue(res2)); st=nextof(st);
		moveresult(st,getvalue(res3));
	}
	else if (hd->type==s_cmatrix || hd->type==s_complex)
	{	getmatrix(hd,&r,&c,&m);
		if (r<1) wrong_arg_in("lu");
		result=new_cmatrix(r,c,""); if (error) return;
		mr=matrixof(result);
        memmove((char *)mr,(char *)m,(LONG)r*c*(LONG)2*sizeof(double));
		cmake_lu(mr,r,c,&rows,&cols,&rank,&det,&deti); 
			if (error) return;
		res1=new_matrix(1,rank,""); if (error) return;
		res2=new_matrix(1,c,""); if (error) return;
		res3=new_complex(det,deti,""); if (error) return;
		m1=matrixof(res1);
		for (i=0; i<rank; i++)
		{	*m1++=*rows+1;
			rows++;
		}
		m2=matrixof(res2);
		for (i=0; i<c; i++)
		{	*m2++=*cols++;
		}
		moveresult(st,getvalue(result)); st=nextof(st);
		moveresult(st,getvalue(res1)); st=nextof(st);
		moveresult(st,getvalue(res2)); st=nextof(st);
		moveresult(st,getvalue(res3));
	}
	else wrong_arg_in("lu");
}

void miscomplex (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd);
	if (hd->type==s_complex || hd->type==s_cmatrix)
		result=new_real(1.0,"");
	else result=new_real(0.0,"");
	if (error) return;
	moveresult(st,result);
}

void misreal (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
		result=new_real(1.0,"");
	else result=new_real(0.0,"");
	if (error) return;
	moveresult(st,result);
}

void misfunction (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_string
		&& (searchudf(stringof(hd))!=0 || find_builtin(stringof(hd))!=0))
			result=new_real(1.0,"");
	else result=new_real(0.0,"");
	if (error) return;
	moveresult(st,result);
}

void misvar (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_string
		&& searchvar(stringof(hd))!=0)
			result=new_real(1.0,"");
	else result=new_real(0.0,"");
	if (error) return;
	moveresult(st,result);
}

void misinterval (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_interval || hd->type==s_imatrix)
		result=new_real(1.0,"");
	else result=new_real(0.0,"");
	if (error) return;
	moveresult(st,result);
}

double rounder;

double rround (double x)
{	x*=rounder;
	if (x>0) x=floor(x+0.5);
	else x=-floor(-x+0.5);
	return x/rounder;
}

void cround (double *x, double *xi, double *z, double *zi)
{	*z=rround(*x);
	*zi=rround(*xi);
}

double frounder[]={1.0,10.0,100.0,1000.0,10000.0,100000.0,1000000.0,
10000000.0,100000000.0,1000000000.0,10000000000.0};

void mround (header *hd)
{	header *hd1;
	int n;
	hd1=next_param(hd);
	if (hd1) hd1=getvalue(hd1); if (error) return;
	if (hd1->type!=s_real) wrong_arg_in("round");
	n=(int)(*realof(hd1));
	if (n>0 && n<11) rounder=frounder[n];
	else rounder=pow(10.0,n);
	spread1(rround,cround,hd);
	test_error("round");
}

void mchar (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real) wrong_arg_in("char");
	result=new_string("a",1,""); if (error) return;
	*stringof(result)=(char)*realof(hd);
	moveresult(st,result);
}

void mascii (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("ascii");
	result=new_real(*stringof(hd),""); if (error) return;
	moveresult(st,result);
}

void merror (header *hd)
{	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("error");
	output1("Error : %s\n",stringof(hd));
	error=301;
}

extern int preventoutput;

void merrlevel (header *hd)
{	header *st=hd,*res;
	char *oldnext;
	int en;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("errorlevel");
	stringon=1;
	oldnext=next; next=stringof(hd);
	res=new_real(0,"");
	preventoutput=1;
	scan();
	preventoutput=0;
	next=oldnext;
	stringon=0;
	en=error; error=0;
	if (en)
	{	*realof(res)=en;
		moveresult(st,res);
	}
	else
	{	moveresult1(st,res);
	}
}

void mprintf (header *hd)
{	header *st=hd,*hd1,*result;
	char string[1024];
	hd1=next_param(hd);
	hd=getvalue(hd);
	hd1=getvalue(hd1); if (error) return;
	if (hd->type!=s_string || hd1->type!=s_real)
		wrong_arg_in("printf");
	sprintf(string,stringof(hd),*realof(hd1));
	result=new_string(string,strlen(string),""); if (error) return;
	moveresult(st,result);
}

void msetkey (header *hd)
/*****
	set a function key
*****/
{	header *st=hd,*hd1,*result;
	char *p;
	int n;
	hd=getvalue(hd); if (error) return;
	hd1=nextof(st); hd1=getvalue(hd1); if (error) return;
	if (hd->type!=s_real || hd1->type!=s_string) wrong_arg_in("setkey");
	n=(int)(*realof(hd))-1; p=stringof(hd1);
	if (n<0 || n>=10 || strlen(p)>63) wrong_arg_in("setkey");
	result=new_string(fktext[n],strlen(fktext[n]),"");
	if (error) return;
	strcpy(fktext[n],p);
	moveresult(st,result);
}

void many (header *hd)
{	header *st=hd,*result;
	int c,r,res=0;
	LONG i,n;
	double *m;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		n=(LONG)(c)*r;
	}
	else if (hd->type==s_complex || hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
        n=(LONG)2*(LONG)(c)*r;
	}
	else wrong_arg_in("any");
	for (i=0; i<n; i++)
		if (*m++!=0.0) { res=1; break; }
	result=new_real(res,""); if (error) return;
	moveresult(st,result);
}

void mcd (header *hd)
{	header *st=hd,*result;
	char *path;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("cd");
	path=cd(stringof(hd));
	result=new_string(path,strlen(path),"");
	moveresult(st,result);
}

void mdir (header *hd)
{	header *st=hd,*result;
	char *name;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("dir");
	name=dir(stringof(hd));
	if (name) result=new_string(name,strlen(name),"");
	else result=new_string("",0,"");
	if (error) return;
	moveresult(st,result);
}

void margs (header *hd)
/* return all args from realof(hd)-st argument on */
{	header *st=hd,*hd1,*result;
	int i,n;
	ULONG size;
	hd=getvalue(hd);
	if (hd->type!=s_real) wrong_arg_in("args");
	n=(int)*realof(hd);
	if (n<1) wrong_arg_in("args");
	if (n>actargn)
	{	newram=(char *)st; return;
	}
	result=(header *)startlocal; i=1;
	while (i<n && result<(header *)endlocal)
	{	result=nextof(result); i++;
	}
	hd1=result;
	while (i<actargn+1 && hd1<(header *)endlocal)
	{	hd1=nextof(hd1); i++;
	}
	size=(char *)hd1-(char *)result;
	if (size<=0)
	{	output("Error in args!\n"); error=2021; return;
	}
	memmove((char *)st,(char *)result,size);
	newram=(char *)st+size;
}

void margs0 (header *hd)
/* return all args from the ; argument on */
{	header *st=(header *)newram,*hd1,*result;
	int i,n;
	ULONG size;
	n=actsp+1;
	if (actsp==0) return;
	if (n>actargn) n=actargn;
	result=(header *)startlocal; i=1;
	while (i<n && result<(header *)endlocal)
	{	result=nextof(result); i++;
	}
	hd1=result;
	while (i<actargn+1 && hd1<(header *)endlocal)
	{	hd1=nextof(hd1); i++;
	}
	size=(char *)hd1-(char *)result;
	if (size<=0)
	{	output("Error in args!\n"); error=2021; return;
	}
	memmove((char *)st,(char *)result,size);
	newram=(char *)st+size;
}

void mname (header *hd)
{	header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	result=new_string(hd->name,strlen(hd->name),"");
	moveresult(st,result);
}

void mdir0 (header *hd)
{	char *name;
	name=dir(0);
	if (name) new_string(name,strlen(name),"");
	else new_string("",0,"");
}

void mflipx (header *hd)
{	header *st=hd,*result;
	double *m,*mr,*mr1;
	int i,j,c,r;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_complex || hd->type==s_interval)
	{	moveresult(st,hd); return;
	}
	else if (hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=mr+(c-1);
			for (j=0; j<c; j++) *mr1--=*m++;
			mr+=c;
		}
	}
	else if (hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=mr+(2l*(c-1)+1);
			for (j=0; j<c; j++)
			{	*mr1--=*(m+1); *mr1--=*m; m+=2;
			}
			mr+=2l*c;
		}
	}
	else if (hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=mr+(2l*(c-1)+1);
			for (j=0; j<c; j++)
			{	*mr1--=*(m+1); *mr1--=*m; m+=2;
			}
			mr+=2l*c;
		}
	}
	else wrong_arg_in("flipx");
	moveresult(st,result);
}

void mflipy (header *hd)
{	header *st=hd,*result;
	double *m,*mr;
	int i,c,r;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_complex || hd->type==s_interval)
	{	moveresult(st,hd); return;
	}
	else if (hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,c,""); if (error) return;
		mr=matrixof(result);
		mr+=(long)(r-1)*c;
		for (i=0; i<r; i++)
		{	memmove((char *)mr,(char *)m,c*sizeof(double));
			m+=c; mr-=c;
		}
	}
	else if (hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		mr+=2l*(long)(r-1)*c;
		for (i=0; i<r; i++)
		{	memmove((char *)mr,(char *)m,2l*c*sizeof(double));
			m+=2l*c; mr-=2l*c;
		}
	}
	else if (hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		mr+=2l*(long)(r-1)*c;
		for (i=0; i<r; i++)
		{	memmove((char *)mr,(char *)m,2l*c*sizeof(double));
			m+=2l*c; mr-=2l*c;
		}
	}
	else wrong_arg_in("flipy");
	moveresult(st,result);
}

void mrotleft (header *hd)
{	header *st=hd,*result;
	double *m,*mr,*mr1;
	int i,j,c,r;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_complex || hd->type==s_interval)
	{	moveresult(st,hd); return;
	}
	else if (hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{   mr1=m+1;
        	for (j=0; j<c-1; j++) *mr++=*mr1++;
            *mr++=*m;
            m+=c;
		}
	}
	else if (hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=m+2l;
			for (j=0; j<c-1; j++)
			{	*mr++=*mr1++; *mr++=*mr1++;
			}
            *mr++=*m; *mr++=*(m+1);
			m+=2l*c;
		}
	}
	else if (hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=m+2l;
			for (j=0; j<c-1; j++)
			{	*mr++=*mr1++; *mr++=*mr1++;
			}
            *mr++=*m; *mr++=*(m+1);
			m+=2l*c;
		}
	}
	else wrong_arg_in("flipx");
	moveresult(st,result);
}

void mrotright (header *hd)
{	header *st=hd,*result;
	double *m,*mr,*mr1;
	int i,j,c,r;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_complex || hd->type==s_interval)
	{	moveresult(st,hd); return;
	}
	else if (hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{   mr1=m;
            *mr++=*(m+c-1);
        	for (j=1; j<c; j++) *mr++=*mr1++;
            m+=c;
		}
	}
	else if (hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=m;
            *mr++=*(m+2l*(c-1)); *mr++=*(m+2l*(c-1)+1);
			for (j=1; j<c; j++)
			{	*mr++=*mr1++; *mr++=*mr1++;
			}
			m+=2l*c;
		}
	}
	else if (hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=m;
            *mr++=*(m+2l*(c-1)); *mr++=*(m+2l*(c-1)+1);
			for (j=1; j<c; j++)
			{	*mr++=*mr1++; *mr++=*mr1++;
			}
			m+=2l*c;
		}
	}
	else wrong_arg_in("flipx");
	moveresult(st,result);
}

void mshiftleft (header *hd)
{	header *st=hd,*result;
	double *m,*mr,*mr1;
	int i,j,c,r;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_complex || hd->type==s_interval)
	{	moveresult(st,hd); return;
	}
	else if (hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{   mr1=m+1;
        	for (j=0; j<c-1; j++) *mr++=*mr1++;
            *mr++=0;
            m+=c;
		}
	}
	else if (hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=m+2l;
			for (j=0; j<c-1; j++)
			{	*mr++=*mr1++; *mr++=*mr1++;
			}
            *mr++=0; *mr++=0;
			m+=2l*c;
		}
	}
	else if (hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=m+2l;
			for (j=0; j<c-1; j++)
			{	*mr++=*mr1++; *mr++=*mr1++;
			}
            *mr++=0; *mr++=0;
			m+=2l*c;
		}
	}
	else wrong_arg_in("flipx");
	moveresult(st,result);
}

void mshiftright (header *hd)
{	header *st=hd,*result;
	double *m,*mr,*mr1;
	int i,j,c,r;
	hd=getvalue(hd); if (error) return;
	if (hd->type==s_real || hd->type==s_complex || hd->type==s_interval)
	{	moveresult(st,hd); return;
	}
	else if (hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_matrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{   mr1=m;
            *mr++=0;
        	for (j=1; j<c; j++) *mr++=*mr1++;
            m+=c;
		}
	}
	else if (hd->type==s_cmatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_cmatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=m;
            *mr++=0; *mr++=0;
			for (j=1; j<c; j++)
			{	*mr++=*mr1++; *mr++=*mr1++;
			}
			m+=2l*c;
		}
	}
	else if (hd->type==s_imatrix)
	{	getmatrix(hd,&r,&c,&m);
		result=new_imatrix(r,c,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r; i++)
		{	mr1=m;
            *mr++=0; *mr++=0;
			for (j=1; j<c; j++)
			{	*mr++=*mr1++; *mr++=*mr1++;
			}
			m+=2l*c;
		}
	}
	else wrong_arg_in("flipx");
	moveresult(st,result);
}

void mmatrix (header *hd)
{	header *st=hd,*hd1,*result;
	long i,n;
	double x,xi;
	double *m,*mr;
	int c,r,c1,r1;
	hd1=nextof(hd);
	hd=getvalue(hd);
	if (error) return;
	hd1=getvalue(hd1);
	if (error) return;
	if (hd->type==s_matrix)
	{	getmatrix(hd,&r,&c,&m);
		if (*m<0 || *m>INT_MAX || *(m+1)<0 || *(m+1)>INT_MAX)
			wrong_arg_in("matrix");
		r1=(int)*m; c1=(int)*(m+1);
		if (hd1->type==s_real)
		{	result=new_matrix(r1,c1,"");
			mr=matrixof(result);
			x=*realof(hd1);
			n=(long)c1*r1;
			for (i=0; i<n; i++) *mr++=x;
		}
		else if (hd1->type==s_complex)
		{	result=new_cmatrix(r1,c1,"");
			mr=matrixof(result);
			x=*realof(hd1); xi=*(realof(hd1)+1);
			n=(long)c1*r1;
			for (i=0; i<n; i++) 
			{	*mr++=x; *mr++=xi;
			}
		}
		else wrong_arg_in("matrix");
	}
	else wrong_arg_in("matrix");
	moveresult(st,result);
}

void mredim (header *hd)
{	header *st=hd,*hd1,*result;
	int c1,r1;
	double *m;
	ULONG i,n,size1,size;
	hd1=nextof(hd);
	hd=getvalue(hd); if (error) return;
	hd1=getvalue(hd1); if (error) return;
	if (hd1->type!=s_matrix || dimsof(hd1)->r!=1 || dimsof(hd1)->c!=2
		|| (hd->type!=s_matrix && hd->type!=s_cmatrix))
		wrong_arg_in("redim");
	m=matrixof(hd1);
	if (*m<1 || *m>INT_MAX) wrong_arg_in("redim");
	r1=(int)(*m++);
	if (*m<1 || *m>INT_MAX) wrong_arg_in("redim");
	c1=(int)(*m);
	size1=(long)c1*r1;
	size=(long)dimsof(hd)->c*dimsof(hd)->r;
	if (size<size1) n=size;
	else n=size1;
	if (hd->type==s_matrix)
	{	result=new_matrix(r1,c1,""); if (error) return;
		memmove((char *)matrixof(result),(char *)matrixof(hd),
			n*sizeof(double));
		if (n<size1)
		{	m=matrixof(result)+n;
			for (i=n; i<size1; i++) *m++=0.0;
		}
	}
	else if (hd->type==s_cmatrix)
	{	result=new_cmatrix(r1,c1,""); if (error) return;
		memmove((char *)matrixof(result),(char *)matrixof(hd),
			2*n*sizeof(double));
		if (n<size1)
		{	m=matrixof(result)+2*n;
			for (i=n; i<size1; i++) { *m++=0.0; *m++=0.0; }
		}
	}
	else wrong_arg_in("redim");
	moveresult(st,result);
}

void mresize (header *hd)
{	header *st=hd,*hd1,*result;
	int c1,r1,c0,r0,i,j;
	double *m,*m0,*mr,*mm;
	hd1=nextof(hd);
	hd=getvalue(hd); if (error) return;
	hd1=getvalue(hd1); if (error) return;
	if (hd1->type!=s_matrix || dimsof(hd1)->r!=1 || dimsof(hd1)->c!=2)
		wrong_arg_in("redim");
	m=matrixof(hd1);
	if (*m<1 || *m>INT_MAX) wrong_arg_in("redim");
	r1=(int)(*m++);
	if (*m<1 || *m>INT_MAX) wrong_arg_in("redim");
	c1=(int)(*m);
	getmatrix(hd,&r0,&c0,&m0); mm=m0;
	if ((r0!=1 && r1!=r0) || (c0!=1 && c1!=c0))
	{	output("Cannot resize these!\n"); error=1; return;
	}
	if (hd->type==s_matrix || hd->type==s_real)
	{   result=new_matrix(r1,c1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r1; i++)
		{	if (c0==1)
			{	for (j=0; j<c1; j++) *mr++=*m0;
				m0++;
			}
			else for (j=0; j<c1; j++) *mr++=*m0++;
			if (r0==1) m0=mm;
		}
	}
	else if (hd->type==s_cmatrix || hd->type==s_complex)
	{   result=new_cmatrix(r1,c1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r1; i++)
		{	if (c0==1)
			{	for (j=0; j<c1; j++) { *mr++=*m0; *mr++=*(m0+1); }
				m0+=2;
			}
			else for (j=0; j<c1; j++) { *mr++=*m0++; *mr++=*m0++; }
			if (r0==1) m0=mm;
		}
	}
	else if (hd->type==s_imatrix || hd->type==s_interval)
	{   result=new_imatrix(r1,c1,""); if (error) return;
		mr=matrixof(result);
		for (i=0; i<r1; i++)
		{	if (c0==1)
			{	for (j=0; j<c1; j++) { *mr++=*m0; *mr++=*(m0+1); }
				m0+=2;
			}
			else for (j=0; j<c1; j++) { *mr++=*m0++; *mr++=*m0++; }
			if (r0==1) m0=mm;
		}
	}
	else wrong_arg_in("redim");
	moveresult(st,result);
}

#ifdef WAVES

void mplaywav (header *hd)
{	header *st=hd;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_string) wrong_arg_in("playwave");
	sys_playwav(stringof(hd));
	moveresult(st,hd);
}

#endif
