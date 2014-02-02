#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "header.h"
#include "stack.h"
#include "builtin.h"
#include "sysdep.h"

double fevalreal (char *fname, double x, int argn, header *args, long size)
{	char *ram=newram;
	header *arg,*hdudf;
	arg=new_real(x,""); if (error) return 0;
	if (size>0)
	{	memmove(nextof(arg),args,size);
		newram+=size;
	}
	if (!exec_builtin(fname,1,arg))
	{	hdudf=searchudf(fname);
		if (error || !hdudf) { error=1; newram=ram; return 0; }
		interpret_udf(hdudf,arg,1+argn,0);
	}
	newram=ram;
	if (arg->type!=s_real) { error=1; return 0; }
	return *realof(arg);
}

#define CGOLD 0.3819660112501051
#undef SHFT
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#undef SIGN
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

double brent
	(double ax, double bx, double cx,
	char *fname, int argn, header *args, long size,
	double tol)
{
	int iter;
	double a,b,d=1,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
	double e=0.0;

	a=(ax < cx ? ax : cx);
	b=(ax > cx ? ax : cx);
	x=w=v=bx;
	fw=fv=fx=fevalreal(fname,x,argn,args,size);
	for (iter=1;iter<=1000;iter++) {
		if (test_key()==escape) { error=1; return 0; }
		xm=0.5*(a+b);
		tol2=2.0*(tol1=tol*fabs(x)+epsilon);
		if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
			return x;
		}
		if (fabs(e) > tol1) {
			r=(x-w)*(fx-fv);
			q=(x-v)*(fx-fw);
			p=(x-v)*q-(x-w)*r;
			q=2.0*(q-r);
			if (q > 0.0) p = -p;
			q=fabs(q);
			etemp=e;
			e=d;
			if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
				d=CGOLD*(e=(x >= xm ? a-x : b-x));
			else {
				d=p/q;
				u=x+d;
				if (u-a < tol2 || b-u < tol2)
					d=SIGN(tol1,xm-x);
			}
		} else {
			d=CGOLD*(e=(x >= xm ? a-x : b-x));
		}
		u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
		fu=fevalreal(fname,u,argn,args,size);
		if (fu <= fx) {
			if (u >= x) a=x; else b=x;
			SHFT(v,w,x,u)
			SHFT(fv,fw,fx,fu)
		} else {
			if (u < x) a=u; else b=u;
			if (fu <= fw || w == x) {
				v=w;
				w=u;
				fv=fw;
				fw=fu;
			} else if (fu <= fv || v == x || v == w) {
				v=u;
				fv=fu;
			}
		}
	}
	output("Too many iterations in brent\n");
	error=1;
	return x;
}

void mbrent (header *hd)
{	header *st=hd,*hd1,*hd2,*hd3,*args,*h,*result;
	double x,d,x1,x2,y,y1,y2;
	long size;
	int argn;
	char *fname;
	if ((header *)newram<=hd) wrong_arg_in("brent");
	hd1=nextof(hd);
	if ((header *)newram<=hd1) wrong_arg_in("brent");
	hd2=nextof(hd1);
	if ((header *)newram<=hd2) wrong_arg_in("brent");
	hd3=nextof(hd2);
	if ((header *)newram<=hd3) wrong_arg_in("brent");
	args=nextof(hd3); size=newram-(char *)args;
	argn=0;
	if (size>0)
	{   h=args;
		while (h<(header *)newram)
		{	argn++; h=nextof(h);
		}
	}
	hd=getvalue(hd); if (error) return;
	hd1=getvalue(hd1); if (error) return;
	hd2=getvalue(hd2); if (error) return;
	hd3=getvalue(hd3); if (error) return;
	if (hd->type!=s_string || hd1->type!=s_real || hd2->type!=s_real
		|| hd3->type!=s_real)
		wrong_arg_in("brent");
	x=*realof(hd1); d=*realof(hd2);
	if (d<=epsilon) d=epsilon;
	fname=stringof(hd);
	y=fevalreal(fname,x,argn,args,size); if (error) return;
	x1=x+d;
	y1=fevalreal(fname,x1,argn,args,size); if (error) return;
	if (y1<y)
	{   x2=x1+d; y2=fevalreal(fname,x2,argn,args,size); if (error) return;
		while (y2<y1)
		{   if (test_key()==escape) { error=1; return; }
			x=x1; x1=x2; y1=y2;
			x2=x1+d; y2=fevalreal(fname,x2,argn,args,size);
			if (error) return;
		}
	}
	else
	{	x2=x1; x1=x; y1=y;
		x=x-d; y=fevalreal(fname,x,argn,args,size); if (error) return;
		while (y<y1)
		{	if (test_key()==escape) { error=1; return; }
			x2=x1; x1=x; y1=y;
			x=x1-d; y=fevalreal(fname,x,argn,args,size);
			if (error) return;
		}
	}
	x=brent(x,x1,x2,stringof(hd),argn,args,size,*realof(hd3));
	if (error) return;
	result=new_real(x,"");
	moveresult(st,result);
}

double fevalvector (char *fname, double x[], int n,
	int argn, header *args, long size)
{	char *ram=newram;
	header *arg,*hdudf;
	double *m;
	int i;
	arg=new_matrix(1,n,""); if (error) return 0;
	m=matrixof(arg);
	for (i=0; i<n; i++) m[i]=x[i];
	memmove(nextof(arg),args,size); newram+=size;
	if (!exec_builtin(fname,1,arg))
	{	hdudf=searchudf(fname);
		if (error || !hdudf) { error=1; newram=ram; return 0; }
		interpret_udf(hdudf,arg,1+argn,0);
	}
	newram=ram;
	if (arg->type!=s_real) { error=1; return 0; }
	return *realof(arg);
}

#define NMAX 5000
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

double *ptry=0;

double amotry(double **p, double y[], double psum[], int ndim,
	char *fname, int argn, header *args, long size,
	int ihi, double fac)
{
	int j;
	double fac1,fac2,ytry;

	fac1=(1.0-fac)/ndim;
	fac2=fac1-fac;
	for (j=0;j<ndim;j++) ptry[j]=psum[j]*fac1-p[ihi][j]*fac2;
	ytry=fevalvector(fname,ptry,ndim,argn,args,size); if (error) return 0;
	if (ytry < y[ihi])
	{	y[ihi]=ytry;
		for (j=0;j<ndim;j++)
		{	psum[j] += ptry[j]-p[ihi][j];
			p[ihi][j]=ptry[j];
		}
	}
	return ytry;
}

void nelder (double **p, double y[], int ndim, double ftol,
	char *fname, int argn, header *args, long size)
{
	int i,ihi,ilo,inhi,j,mpts=ndim+1,nfunk=0;
	double rtol,sum,swap,ysave,ytry,*psum;

	if (ftol<=0) ftol=epsilon;

	psum=(double *)malloc(ndim*sizeof(double));
	for (j=0;j<ndim;j++)
	{	for (sum=0.0,i=0;i<mpts;i++) sum += p[i][j];
		psum[j]=sum;
	}
	while (1)
	{   if (test_key()==escape) { error=1; goto stop; }
		ilo=0;
		ihi = y[0]>y[1] ? (inhi=1,0) : (inhi=0,1);
		for (i=0;i<mpts;i++)
		{	if (y[i] <= y[ilo]) ilo=i;
			if (y[i] > y[ihi])
			{	inhi=ihi;
				ihi=i;
			}
			else if (y[i] > y[inhi] && i != ihi) inhi=i;
		}
		rtol=2.0*fabs(y[ihi]-y[ilo]);
		if (rtol < ftol)
		{	SWAP(y[0],y[ilo])
			for (i=0;i<ndim;i++) SWAP(p[0][i],p[ilo][i])
			break;
		}
		if (nfunk >= NMAX)
		{	output("Two many iterations in nelder\n");
			error=1; goto stop;
		}
		nfunk +=2;
		ytry=amotry(p,y,psum,ndim,fname,argn,args,size,ihi,-1.0);
		if (error) return;
		if (ytry <= y[ilo])
		{	amotry(p,y,psum,ndim,fname,argn,args,size,ihi,2.0);
			if (error) goto stop;
		}
		else if (ytry >= y[inhi])
		{	ysave=y[ihi];
			ytry=amotry(p,y,psum,ndim,fname,argn,args,size,ihi,0.5);
			if (error) goto stop;
			if (ytry >= ysave)
			{	for (i=0;i<mpts;i++) {
					if (i != ilo)
					{	for (j=0;j<ndim;j++)
							p[i][j]=psum[j]=0.5*(p[i][j]+p[ilo][j]);
						y[i]=fevalvector(fname,psum,ndim,argn,args,size);
						if (error) goto stop;
					}
				}
				nfunk += ndim;
				for (j=0;j<ndim;j++)
				{	for (sum=0.0,i=0;i<mpts;i++) sum += p[i][j];
					psum[j]=sum;
				}
			}
		} else --(nfunk);
	}
	stop: free(psum);
}

void mnelder (header *hd)
{	header *st=hd,*hd1,*hd2,*hd3,*args,*h,*result;
	double **p,*y,*m,d;
	long size;
	int argn,n,i,j;
	char *fname;
	if ((header *)newram<=hd) wrong_arg_in("brent");
	hd1=nextof(hd);
	if ((header *)newram<=hd1) wrong_arg_in("brent");
	hd2=nextof(hd1);
	if ((header *)newram<=hd2) wrong_arg_in("brent");
	hd3=nextof(hd2);
	if ((header *)newram<=hd3) wrong_arg_in("brent");
	args=nextof(hd3); size=newram-(char *)args;
	argn=0;
	if (size>0)
	{   h=args;
		while (h<(header *)newram)
		{	argn++; h=nextof(h);
		}
	}
	hd=getvalue(hd); if (error) return;
	hd1=getvalue(hd1); if (error) return;
	hd2=getvalue(hd2); if (error) return;
	hd3=getvalue(hd3); if (error) return;
	if (hd->type!=s_string || hd1->type!=s_matrix || hd2->type!=s_real
		|| hd3->type!=s_real || dimsof(hd1)->r!=1 || dimsof(hd1)->c<2)
		wrong_arg_in("nelder");
	n=dimsof(hd1)->c;
	m=matrixof(hd1);
	p=(double **)malloc((n+1)*sizeof(double *));
	for (i=0; i<=n; i++)
		p[i]=(double *)malloc(n*sizeof(double));
	y=(double *)malloc((n+1)*sizeof(double));
	ptry=(double *)malloc(n*sizeof(double));
	fname=stringof(hd);
	for (j=0; j<n; j++) p[n][j]=m[j];
	d=*realof(hd2);
	if (d<=0) d=epsilon;
	for (i=0; i<n; i++)
	{	for (j=0; j<n; j++) p[i][j]=m[j];
		p[i][i]+=d;
	}
	for (i=0; i<=n; i++)
	{	y[i]=fevalvector(fname,p[i],n,argn,args,size);
		if (error)
		{	output("Function call failed in nelder.\n");
			goto stop;
		}
	}
	nelder(p,y,n,*realof(hd3),fname,argn,args,size);
	if (error)
	{	output("Error in nelder"); goto stop;
	}
	result=new_matrix(1,n,""); if (error) goto stop;
	m=matrixof(result);
	for (i=0; i<n; i++) m[i]=p[0][i];
	moveresult(st,result);
	stop : free(p); free(y); free(ptry);
}

void fevaldgl (double x, double y[], double yd[],
	int n,
	char *fname, int argn, header *args, long size)
{	char *ram=newram;
	header *arg,*argx,*hdudf;
	double *m;
	int i;
	argx=new_real(x,""); if (error) return;
	arg=new_matrix(1,n,""); if (error) return;
	m=matrixof(arg);
	for (i=0; i<n; i++) m[i]=y[i];
	memmove(nextof(arg),args,size); newram+=size;
	hdudf=searchudf(fname);
	if (error || !hdudf) { error=1; newram=ram; return; }
	interpret_udf(hdudf,argx,2+argn,0);
	newram=ram;
	if (n>1)
	{	if (argx->type!=s_matrix || dimsof(argx)->r!=1 ||
			dimsof(argx)->c!=n) { error=1; return; }
		m=matrixof(argx);
	}
	else
	{	if (argx->type!=s_real) { error=1; return; }
    	m=realof(argx);
	}
	for (i=0; i<n; i++) yd[i]=m[i];
}

double *dym,*dyt,*yt;

void rk4(double y[], double dydx[], int n, double x, double h,
	double yout[],
	char *fname, int argn, header *args, long size)
{
	int i;
	double xh,hh,h6;

	hh=h*0.5;
	h6=h/6.0;
	xh=x+hh;
	for (i=0; i<n; i++) yt[i]=y[i]+hh*dydx[i];
	fevaldgl(xh,yt,dyt,n,fname,argn,args,size);
	if (error) return;
	for (i=0; i<n; i++) yt[i]=y[i]+hh*dyt[i];
	fevaldgl(xh,yt,dym,n,fname,argn,args,size);
	if (error) return;
	for (i=0; i<n; i++)
	{	yt[i]=y[i]+h*dym[i];
		dym[i] += dyt[i];
	}
	fevaldgl(x+h,yt,dyt,n,fname,argn,args,size);
	if (error) return;
	for (i=0; i<n; i++)
		yout[i]=y[i]+h6*(dydx[i]+dyt[i]+2.0*dym[i]);
}

void runge1 (double a, double b, int steps, double y[], int n,
	char *fname, int argn, header *args, long size)
{	int i,k;
	double x,h;
	double *vout,*dv;
	vout=(double *)malloc(n*sizeof(double));
	dv=(double *)malloc(n*sizeof(double));
	dym=(double *)malloc(n*sizeof(double));
	dyt=(double *)malloc(n*sizeof(double));
	yt=(double *)malloc(n*sizeof(double));
	x=a;
	h=(b-a)/steps;
	for (k=0; k<steps; k++)
	{   if (test_key()==escape) { error=1; goto stop; }
		fevaldgl(x,y,dv,n,fname,argn,args,size);
		if (error) goto stop;
		rk4(y,dv,n,x,h,vout,fname,argn,args,size);
		if (error) goto stop;
		x += h;
		for (i=0; i<n; i++) y[i]=vout[i];
	}
	stop :
	free(vout);
	free(dv);
	free(dym);
	free(dyt);
	free(yt);
}

void mrunge1 (header *hd)
{	header *st=hd,*hdx1,*hdx2,*hdy,*hdn,*args,*h,*result;
	double *m,*my,*y;
	long size;
	int argn,n,i,steps;
	char *fname;
	if ((header *)newram<=hd) wrong_arg_in("runge1");
	hdx1=nextof(hd);
	if ((header *)newram<=hdx1) wrong_arg_in("runge1");
	hdx2=nextof(hdx1);
	if ((header *)newram<=hdx2) wrong_arg_in("runge1");
	hdn=nextof(hdx2);
	if ((header *)newram<=hdn) wrong_arg_in("runge1");
	hdy=nextof(hdn);
	if ((header *)newram<=hdy) wrong_arg_in("runge1");
	args=nextof(hdy); size=newram-(char *)args;
	argn=0;
	if (size>0)
	{   h=args;
		while (h<(header *)newram)
		{	argn++; h=nextof(h);
		}
	}
	hd=getvalue(hd); if (error) return;
	hdx1=getvalue(hdx1); if (error) return;
	hdx2=getvalue(hdx2); if (error) return;
	hdn=getvalue(hdn); if (error) return;
	hdy=getvalue(hdy); if (error) return;
	if (hd->type!=s_string ||
		hdx1->type!=s_real || hdx2->type!=s_real
		|| ((hdy->type!=s_matrix || dimsof(hdy)->r!=1)
				&& hdy->type!=s_real)
		|| hdn->type!=s_real)
		wrong_arg_in("runge1");
	if (hdy->type!=s_real)
	{   my=matrixof(hdy);
		n=dimsof(hdy)->c;
	}
	else
	{   my=realof(hdy);
		n=1;
	}
	if (n<1) wrong_arg_in("runge1");
	steps=(int)*realof(hdn);
	if (steps<1) steps=1;
	fname=stringof(hd);
	y=(double *)malloc(n*sizeof(double));
	for (i=0; i<n; i++) y[i]=my[i];
	runge1(*realof(hdx1),*realof(hdx2),steps,y,n,
		fname,argn,args,size);
	if (error)
	{	output("Error in runge1\n"); goto stop;
	}
	if (hdy->type==s_matrix)
	{	result=new_matrix(1,n,""); if (error) goto stop;
		m=matrixof(result);
		for (i=0; i<n; i++) m[i]=y[i];
	}
	else
	{	result=new_real(*y,""); if (error) goto stop;
	}
	moveresult(st,result);
	stop: free(y);
}

double *yerr,*ytemp,*ak2,*ak3,*ak4,*ak5,*ak6,*ytemp1;

#define SAFETY 0.9
#define PGROW -0.2
#define PSHRNK -0.25
#define ERRCON 1.89e-4

double FMAX (double a, double b)
{	if (a>b) return a;
	else return b;
}

void rkck(double y[], double dydx[], int n, double x, double h,
	double yout[], double yerr[],
	char *fname, int argn, header *args, long size)
{	double *ytemp=ytemp1;
	int i;
	static double a2=0.2,a3=0.3,a4=0.6,a5=1.0,a6=0.875,b21=0.2,
		b31=3.0/40.0,b32=9.0/40.0,b41=0.3,b42 = -0.9,b43=1.2,
		b51 = -11.0/54.0, b52=2.5,b53 = -70.0/27.0,b54=35.0/27.0,
		b61=1631.0/55296.0,b62=175.0/512.0,b63=575.0/13824.0,
		b64=44275.0/110592.0,b65=253.0/4096.0,c1=37.0/378.0,
		c3=250.0/621.0,c4=125.0/594.0,c6=512.0/1771.0,
		dc5 = -277.0/14336.0;
	double
		dc1=c1-2825.0/27648.0,dc3=c3-18575.0/48384.0,
		dc4=c4-13525.0/55296.0,dc6=c6-0.25;

	for (i=0; i<n; i++)
		ytemp[i]=y[i]+b21*h*dydx[i];
	fevaldgl(x+a2*h,ytemp,ak2,n,fname,argn,args,size);
	if (error) return;
	for (i=0; i<n; i++)
		ytemp[i]=y[i]+h*(b31*dydx[i]+b32*ak2[i]);
	fevaldgl(x+a3*h,ytemp,ak3,n,fname,argn,args,size);
	if (error) return;
	for (i=0; i<n; i++)
		ytemp[i]=y[i]+h*(b41*dydx[i]+b42*ak2[i]+b43*ak3[i]);
	fevaldgl(x+a4*h,ytemp,ak4,n,fname,argn,args,size);
	if (error) return;
	for (i=0; i<n; i++)
		ytemp[i]=y[i]+h*(b51*dydx[i]+b52*ak2[i]+b53*ak3[i]+b54*ak4[i]);
	fevaldgl(x+a5*h,ytemp,ak5,n,fname,argn,args,size);
	if (error) return;
	for (i=0; i<n; i++)
		ytemp[i]=y[i]+h*(b61*dydx[i]+b62*ak2[i]+b63*ak3[i]+b64*ak4[i]+b65*ak5[i]);
	fevaldgl(x+a6*h,ytemp,ak6,n,fname,argn,args,size);
	if (error) return;
	for (i=0; i<n; i++)
		yout[i]=y[i]+h*(c1*dydx[i]+c3*ak3[i]+c4*ak4[i]+c6*ak6[i]);
	for (i=0; i<n; i++)
		yerr[i]=h*(dc1*dydx[i]+dc3*ak3[i]+dc4*ak4[i]+dc5*ak5[i]+dc6*ak6[i]);
}

void rkqs (double y[], double dydx[], int n, double *x,
	double htry, double eps, double yscal[],
	double *hdid, double *hnext,
	char *fname, int argn, header *args, long size)
{	int i;
	double errmax,h,xnew;

	h=htry;
	while (1)
	{	rkck(y,dydx,n,*x,h,ytemp,yerr,
			fname,argn,args,size);
		errmax=0.0;
		for (i=0; i<n; i++)
			errmax=FMAX(errmax,fabs(yerr[i]/yscal[i]));
		errmax /= eps;
		if (errmax > 1.0)
		{	h=SAFETY*h*pow(errmax,PSHRNK);
			if (h < 0.1*h) h *= 0.1;
			xnew=(*x)+h;
			if (xnew == *x)
			{	output("Stepsize underflow in runge2\n");
				error=1;
				return;
			}
		}
		else
		{	if (errmax > ERRCON) *hnext=SAFETY*h*pow(errmax,PGROW);
			else *hnext=5.0*h;
			*x += (*hdid=h);
			for (i=0; i<n; i++) y[i]=ytemp[i];
			break;
		}
	}
}

#define MAXSTP 10000
#define TINY 1e-30

double runge2 (double a, double b, double ystart[], int n,
	double eps, double step,
	char *fname, int argn, header *args, long size)
{   int nstp,i;
	double x,hnext,hdid,h;
	double *yscal,*y,*dydx;

	yscal=(double *)malloc(n*sizeof(double));
	y=(double *)malloc(n*sizeof(double));
	dydx=(double *)malloc(n*sizeof(double));
	yerr=(double *)malloc(n*sizeof(double));
	ytemp=(double *)malloc(n*sizeof(double));
	ak2=(double *)malloc(n*sizeof(double));
	ak3=(double *)malloc(n*sizeof(double));
	ak4=(double *)malloc(n*sizeof(double));
	ak5=(double *)malloc(n*sizeof(double));
	ak6=(double *)malloc(n*sizeof(double));
	ytemp1=(double *)malloc(n*sizeof(double));

	x=a;
	h=SIGN(step,b-a);
	for (i=0; i<n; i++) y[i]=ystart[i];
	for (nstp=0; nstp<MAXSTP; nstp++)
	{	fevaldgl(x,y,dydx,n,fname,argn,args,size);
		if (error) goto stop;
		for (i=0; i<n; i++)
			yscal[i]=fabs(y[i])+fabs(dydx[i]*h)+TINY;
		if ((x+h-b)*(x+h-a) > 0.0) h=b-x;
		rkqs(y,dydx,n,&x,h,eps,yscal,&hdid,&hnext,
			fname,argn,args,size);

		if ((x-b)*(b-a) >= -fabs(hnext*0.0001))
		{   for (i=0; i<n; i++) ystart[i]=y[i];
			goto stop;
		}
		h=hnext;
	}
	output ("Two many steps in runge2\n");
	error=1;
	stop :
	free(yscal); free(y); free(dydx);
	free(yerr); free(ytemp); free(ytemp1);
	free(ak2); free(ak3); free(ak4); free(ak5); free(ak6);
	return hnext;
}

void mrunge2 (header *hd)
{	header *st=hd,*hdx1,*hdx2,*hdy,*hdeps,*hdstep,*args,*h,*result;
	double *m,*my,*y,hnext;
	long size;
	int argn,n,i;
	char *fname;
	if ((header *)newram<=hd) wrong_arg_in("runge2");
	hdx1=nextof(hd);
	if ((header *)newram<=hdx1) wrong_arg_in("runge2");
	hdx2=nextof(hdx1);
	if ((header *)newram<=hdx1) wrong_arg_in("runge2");
	hdy=nextof(hdx2);
	if ((header *)newram<=hdy) wrong_arg_in("runge2");
	hdeps=nextof(hdy);
	if ((header *)newram<=hdeps) wrong_arg_in("runge2");
	hdstep=nextof(hdeps);
	if ((header *)newram<=hdstep) wrong_arg_in("runge2");
	args=nextof(hdstep); size=newram-(char *)args;
	argn=0;
	if (size>0)
	{   h=args;
		while (h<(header *)newram)
		{	argn++; h=nextof(h);
		}
	}
	hd=getvalue(hd); if (error) return;
	hdx1=getvalue(hdx1); if (error) return;
	hdx2=getvalue(hdx2); if (error) return;
	hdeps=getvalue(hdeps); if (error) return;
	hdstep=getvalue(hdstep); if (error) return;
	hdy=getvalue(hdy); if (error) return;
	if (hd->type!=s_string ||
		hdx1->type!=s_real || hdx2->type!=s_real
		|| ((hdy->type!=s_matrix || dimsof(hdy)->r!=1)
				&& hdy->type!=s_real)
		|| hdeps->type!=s_real || hdstep->type!=s_real)
		wrong_arg_in("runge2");
	if (hdy->type!=s_real)
	{   my=matrixof(hdy);
		n=dimsof(hdy)->c;
	}
	else
	{   my=realof(hdy);
		n=1;
	}
	if (n<1) wrong_arg_in("runge2");
	fname=stringof(hd);
	y=(double *)malloc(n*sizeof(double));
	for (i=0; i<n; i++) y[i]=my[i];
	hnext=runge2(*realof(hdx1),*realof(hdx2),y,n,
		*realof(hdeps),*realof(hdstep),
		fname,argn,args,size);
	if (error)
	{	output("Error in runge2\n"); goto stop;
	}
	if (hdy->type==s_matrix)
	{	result=new_matrix(1,n,""); if (error) goto stop;
		m=matrixof(result);
		for (i=0; i<n; i++) m[i]=y[i];
	}
	else
	{	result=new_real(*y,""); if (error) goto stop;
	}
	new_real(hnext,"");
	moveresult1(st,result);
	stop : free(y);
}

