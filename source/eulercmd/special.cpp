#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

#include "header.h"
#include "stack.h"
#include "spread.h"

double polevl(double x, double coef[], int N )
{
	double ans;
	int i;
	double *p;

	p = coef;
	ans = *p++;
	i = N;

	do
		ans = ans * x  +  *p++;
	while( --i );

	return( ans );
}

double p1evl(double x, double coef[], int N )
{
	double ans;
	double *p;
	int i;

	p = coef;
	ans = x + *p++;
	i = N-1;

	do
		ans = ans * x  + *p++;
	while( --i );

	return( ans );
}

static double P[] = {
  1.60119522476751861407E-4,
  1.19135147006586384913E-3,
  1.04213797561761569935E-2,
  4.76367800457137231464E-2,
  2.07448227648435975150E-1,
  4.94214826801497100753E-1,
  9.99999999999999996796E-1
};
static double Q[] = {
-2.31581873324120129819E-5,
 5.39605580493303397842E-4,
-4.45641913851797240494E-3,
 1.18139785222060435552E-2,
 3.58236398605498653373E-2,
-2.34591795718243348568E-1,
 7.14304917030273074085E-2,
 1.00000000000000000320E0
};
#define MAXGAM 171.624376956302725
static double LOGPI = 1.14472988584940017414;

static double STIR[5] = {
 7.87311395793093628397E-4,
-2.29549961613378126380E-4,
-2.68132617805781232825E-3,
 3.47222221605458667310E-3,
 8.33333333333482257126E-2,
};
#define MAXSTIR 143.01608
static double SQTPI = 2.50662827463100050242E0;

int sgngam = 0;
extern int sgngam;
extern double MAXLOG,MAXNUM;
#define PI M_PI

/* Gamma function computed by Stirling's formula.
 * The polynomial STIR is valid for 33 <= x <= 172.
 */
double stirf (double x)
{	double y, w, v;

	w = 1.0/x;
	w = 1.0 + w * polevl( w, STIR, 4 );
	y = exp(x);
	if( x > MAXSTIR )
	{	v = pow( x, 0.5 * x - 0.25 );
		y = v * (v / y);
	}
	else
	{	y = pow( x, x - 0.5 ) / y;
	}
	y = SQTPI * y * w;
	return( y );
}


double gamm (double x)
{
	double p, q, z;
	int i;

	sgngam = 1;
	q = fabs(x);

	if( q > 33.0 )
	{	if( x < 0.0 )
		{	p = floor(q);
			if( p == q )
				goto goverf;
			i = (int)p;
			if( (i & 1) == 0 )
				sgngam = -1;
			z = q - p;
			if( z > 0.5 )
				{
				p += 1.0;
				z = q - p;
				}
			z = q * sin( PI * z );
			if( z == 0.0 )
				{
	goverf:
				output("Overflow in gamma\n");
				error=1; return 0;
				}
			z = fabs(z);
			z = PI/(z * stirf(q) );
			}
		else
			{
			z = stirf(x);
			}
		return( sgngam * z );
		}

	z = 1.0;
	while( x >= 3.0 )
		{
		x -= 1.0;
		z *= x;
		}

	while( x < 0.0 )
		{
		if( x > -1.E-9 )
			goto small;
		z /= x;
		x += 1.0;
		}

	while( x < 2.0 )
		{
		if( x < 1.e-9 )
			goto small;
		z /= x;
		x += 1.0;
		}

	if( (x == 2.0) || (x == 3.0) )
		return(z);

	x -= 2.0;
	p = polevl( x, P, 6 );
	q = polevl( x, Q, 7 );
	return( z * p / q );

	small:
	if( x == 0.0 )
		{
		output("Wrong argument for gamma.\n");
		error=1; return 0;
		}
	else
		return( z/((1.0 + 0.5772156649015329 * x) * x) );
}



static double A[] = {
 8.11614167470508450300E-4,
-5.95061904284301438324E-4,
 7.93650340457716943945E-4,
-2.77777777730099687205E-3,
 8.33333333333331927722E-2
};
static double B[] = {
-1.37825152569120859100E3,
-3.88016315134637840924E4,
-3.31612992738871184744E5,
-1.16237097492762307383E6,
-1.72173700820839662146E6,
-8.53555664245765465627E5
};
static double C[] = {
/* 1.00000000000000000000E0, */
-3.51815701436523470549E2,
-1.70642106651881159223E4,
-2.20528590553854454839E5,
-1.13933444367982507207E6,
-2.53252307177582951285E6,
-2.01889141433532773231E6
};
/* log( sqrt( 2*pi ) ) */
static double LS2PI  =  0.91893853320467274178;
#define MAXLGM 2.556348e305

/* Logarithm of gamma function */

double gammln (double x)
{
	double p, q, w, z;
	int i;

	sgngam = 1;

	if( x < -34.0 )
		{
		q = -x;
		w = gammln(q); /* note this modifies sgngam! */
		p = floor(q);
		if( p == q )
			goto loverf;
		i = (int)p;
		if( (i & 1) == 0 )
			sgngam = -1;
		else
			sgngam = 1;
		z = q - p;
		if( z > 0.5 )
			{
			p += 1.0;
			z = p - q;
			}
		z = q * sin( PI * z );
		if( z == 0.0 )
			goto loverf;
	/*	z = log(PI) - log( z ) - w;*/
		z = LOGPI - log( z ) - w;
		return( z );
		}

	if( x < 13.0 )
		{
		z = 1.0;
		while( x >= 3.0 )
			{
			x -= 1.0;
			z *= x;
			}
		while( x < 2.0 )
			{
			if( x == 0.0 )
				goto loverf;
			z /= x;
			x += 1.0;
			}
		if( z < 0.0 )
			{
			sgngam = -1;
			z = -z;
			}
		else
			sgngam = 1;
		if( x == 2.0 )
			return( log(z) );
		x -= 2.0;
		p = x * polevl( x, B, 5 ) / p1evl( x, C, 6);
		return( log(z) + p );
		}

	if( x > MAXLGM )
		{
	loverf:
		output("Overflow in loggamma\n");
		error=1; return 0;
		}

	q = ( x - 0.5 ) * log(x) - x + LS2PI;
	if( x > 1.0e8 )
		return( q );

	p = 1.0/(x*x);
	if( x >= 1000.0 )
		q += ((   7.9365079365079365079365e-4 * p
			- 2.7777777777777777777778e-3) *p
			+ 0.0833333333333333333333) / x;
	else
		q += polevl( p, A, 4 ) / x;
	return( q );
}

void mgammaln (header *hd)
{	spread1(gammln,0,hd);
	test_error("gammaln");
}

void mgamma (header *hd)
{	spread1(gamm,0,hd);
	test_error("gamma");
}

#define ITMAX 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

double gser (double a, double x)
{	int n;
	double sum,del,ap;
	if (x <= 0.0) return 0;
	else
	{	ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++) {
			++ap;
			del *= x / ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				return sum*exp(-x+a *log(x));
			}
		}
		output("Iteration fails in gamma\n");
		error=1;
		return 0;
	}
}

double gcf (double a, double x)
{
	int i;
	double an,b,c,d,del,h;

	b=x+1.0-a;
	c=1.0/FPMIN;
	d=1.0/b;
	h=d;
	for (i=1;i<=ITMAX;i++) {
		an = -i*(i-a);
		b += 2.0;
		d=an*d+b;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=b+an/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > ITMAX)
	{	output("a too large, ITMAX too small in gcf\n");
		error=1; return 0;
	}
	return exp(-x+a*log(x))*h;
}

void gammp (double *a, double *x, double *res)
{
	if (*x < 0.0 || *a <= 0.0)
	{	output("Invalid arguments in routine gammp\n");
		error=1; return;
	}
	if (*x < (*a+1.0)) {
		*res=gser(*a,*x);
		return;
	} else {
		*res=gamm(*a)-gcf(*a,*x);
		return;
	}
}

void mgammai (header *hd)
{   spreadf2(gammp,0,0,hd);
	test_error("gamma");
}

double gauss (double z)
{   double res,a=0.5;
	double x=z*z/2;
	gammp(&a,&x,&res);
	res/=gamm(0.5)*2;
	if (z<0)
		return 0.5-res;
	else
		return 0.5+res;
}

void mgauss (header *hd)
{	spread1(gauss,0,hd);
	test_error("normaldis");
}

#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define RNMX (1.0-EPS)

static long randseed=1234512345;
#define IDUM2 123456789
static long idum2=IDUM2;
static long iy=0;
static long iv[NTAB];

double ran2 (void)
{
	int j;
	long k;
	double temp;

	if (randseed <= 0) {
		if (-(randseed) < 1) randseed=1;
		else randseed = -(randseed);
		idum2=(randseed);
		for (j=NTAB+7;j>=0;j--) {
			k=(randseed)/IQ1;
			randseed=IA1*(randseed-k*IQ1)-k*IR1;
			if (randseed < 0) randseed += IM1;
			if (j < NTAB) iv[j] = randseed;
		}
		iy=iv[0];
	}
	k=(randseed)/IQ1;
	randseed=IA1*(randseed-k*IQ1)-k*IR1;
	if (randseed < 0) randseed += IM1;
	k=idum2/IQ2;
	idum2=IA2*(idum2-k*IQ2)-k*IR2;
	if (idum2 < 0) idum2 += IM2;
	j=iy/NDIV;
	iy=iv[j]-idum2;
	iv[j] = randseed;
	if (iy < 1) iy += IMM1;
	if ((temp=AM*iy) > RNMX) return RNMX;
	else return temp;
}

void mrandom (header *hd)
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
	for (k=0; k<n; k++) *m++=(double)ran2();
	moveresult(st,result);
}

void mshuffle (header *hd)
{  header *st=hd,*result;
	double *m,*mr,x;
	int i,j,n;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_matrix || dimsof(hd)->r!=1)
		wrong_arg_in("shuffle");
	n=dimsof(hd)->c;
	result=new_matrix(1,n,"");
	m=matrixof(hd); mr=matrixof(result);
	for (i=0; i<n; i++) *mr++=*m++;
	mr=matrixof(result);
	for (i=n-1; i>0; i--)
	{	j=(int)floor(ran2()*(i+1));
		if (i!=j)
		{	x=*(mr+i); *(mr+i)=*(mr+j); *(mr+j)=x;
		}
	}
	moveresult(st,result);

}

double gasdev (void)
{	static double f=0,rsq=1,v1=1,v2=1;
	static int res=0;
	if (res)
	{	res=0; return v2*f;
	}
	do
	{	v1=2.0*ran2()-1.0;
		v2=2.0*ran2()-1.0;
		rsq=v1*v1+v2*v2;
	} while (rsq >= 1.0 || rsq <= 0.00001);
	f=sqrt(-2.0*log(rsq)/rsq);
	res=1;
	return v1*f;
}

void mnormalnew (header *hd)
{	header *st=hd,*result;
	double *m;
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
	for (k=0; k<n; k++) *m++=gasdev();
	moveresult(st,result);
}

void mseed (header *hd)
{   header *st=hd,*result;
	hd=getvalue(hd); if (error) return;
	if (hd->type!=s_real) wrong_arg_in("seed");
	result=new_real(*realof(hd),"");
	randseed=-labs((long)(*realof(hd)*LONG_MAX));
	moveresult(st,result);
}

/************ BETA Functions *******************/

#undef ITMAX
#define ITMAX 100
#undef FPMIN
#define FPMIN 1e-30

double betacf (double a, double b, double x)
{
	int m,m2;
	double aa,c,d,del,h,qab,qam,qap;

	qab=a+b;
	qap=a+1.0;
	qam=a-1.0;
	c=1.0;
	d=1.0-qab*x/qap;
	if (fabs(d) < FPMIN) d=FPMIN;
	d=1.0/d;
	h=d;
	for (m=1;m<=ITMAX;m++) {
		m2=2*m;
		aa=m*(b-m)*x/((qam+m2)*(a+m2));
		d=1.0+aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		h *= d*c;
		aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
		d=1.0+aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS) break;
	}
	if (m > ITMAX)
	{	output( "a or b too big, or ITMAX too small in betai\n" );
		error=1;
	}
	return h;
}

double betai (double x, double a, double b)
{	double bt;
	if (x < 0.0 || x > 1.0)
	{	output( "x not in [0,1] in betai\n" );
		error=1; return 0;
	}
	if (x == 0.0 || x == 1.0) bt=0.0;
	else
		bt=exp(gammln(a+b)-gammln(a)-gammln(b)+a*log(x)+b*log(1.0-x));
	if (x < (a+1.0)/(a+b+2.0))
		return bt*betacf(a,b,x)/a;
	else
		return 1.0-bt*betacf(b,a,1.0-x)/b;
}

void mbetai (header *hd)
{	header *result,*st=hd,*hda,*hdb;
	double x;
	hda=nextof(hd);
	hdb=nextof(hda);
	hd=getvalue(hd); if (error) return;
	hda=getvalue(hda); if (error) return;
	hdb=getvalue(hdb); if (error) return;
	if (hd->type!=s_real || hda->type!=s_real ||
		hdb->type!=s_real)
			wrong_arg_in("betai");
	x=betai(*realof(hd),*realof(hda),*realof(hdb));
	if (error) return;
	result=new_real(x,""); if (error) return;
	moveresult(st,result);
}

double chebev (double a, double b, double c[], int m, double x)
{
	double d=0.0,dd=0.0,sv,y,y2;
	int j;

	if ((x-a)*(x-b) > 0.0)
	{	output("x not in range in routine bessel\n");
		error = 1; return 0;
	}
	y2=2.0*(y=(2.0*x-a-b)/(b-a));
	for (j=m-1;j>=1;j--) {
		sv=d;
		d=y2*d-dd+c[j];
		dd=sv;
	}
	return y*d-dd+0.5*c[0];
}

#define NUSE1 7
#define NUSE2 8

void beschb(double x, double *gam1, double *gam2, double *gampl, double *gammi)
{	double xx;
	static double c1[] = {
		-1.142022680371172e0,6.516511267076e-3,
		3.08709017308e-4,-3.470626964e-6,6.943764e-9,
		3.6780e-11,-1.36e-13};
	static double c2[] = {
		1.843740587300906e0,-0.076852840844786e0,
		1.271927136655e-3,-4.971736704e-6,-3.3126120e-8,
		2.42310e-10,-1.70e-13,-1.0e-15};

	xx=8.0*x*x-1.0;
	*gam1=chebev(-1.0,1.0,c1,NUSE1,xx); if (error) return;
	*gam2=chebev(-1.0,1.0,c2,NUSE2,xx); if (error) return;
	*gampl= *gam2-x*(*gam1);
	*gammi= *gam2+x*(*gam1);
}
#undef NUSE1
#undef NUSE2

static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
		(imaxarg1) : (imaxarg2))

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

#undef EPS
#undef FPMIN
#define EPS 1.0e-16
#define FPMIN 1.0e-30
#define MAXIT 10000
#define XMIN 2.0

void bessjy (double xnu, double x, double *rj, double *ry,
	double *rjp, double *ryp)
{
	int i,isign,l,nl;
	double a,b,br,bi,c,cr,ci,d,del,del1,den,di,dlr,dli,dr,e,f,fact,fact2,
		fact3,ff,gam,gam1,gam2,gammi,gampl,h,p,pimu,pimu2,q,r,rjl,
		rjl1,rjmu,rjp1,rjpl,rjtemp,ry1,rymu,rymup,rytemp,sum,sum1,
		temp,w,x2,xi,xi2,xmu,xmu2;

	if (x <= 0.0 || xnu < 0.0)
	{
	  output("bad arguments in bessel\n");
	  error = 1; return;
	}
	nl=(x < XMIN ? (int)(xnu+0.5) : IMAX(0,(int)(xnu-x+1.5)));
	xmu=xnu-nl;
	xmu2=xmu*xmu;
	xi=1.0/x;
	xi2=2.0*xi;
	w=xi2/M_PI;
	isign=1;
	h=xnu*xi;
	if (h < FPMIN) h=FPMIN;
	b=xi2*xnu;
	d=0.0;
	c=h;
	for (i=1;i<=MAXIT;i++) {
		b += xi2;
		d=b-d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=b-1.0/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=c*d;
		h=del*h;
		if (d < 0.0) isign = -isign;
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > MAXIT)
	{	output("x too large in bessel\n");
		error=1; return;
	}
	rjl=isign*FPMIN;
	rjpl=h*rjl;
	rjl1=rjl;
	rjp1=rjpl;
	fact=xnu*xi;
	for (l=nl;l>=1;l--) {
		rjtemp=fact*rjl+rjpl;
		fact -= xi;
		rjpl=fact*rjtemp-rjl;
		rjl=rjtemp;
	}
	if (rjl == 0.0) rjl=EPS;
	f=rjpl/rjl;
	if (x < XMIN) {
		x2=0.5*x;
		pimu=M_PI*xmu;
		fact = (fabs(pimu) < EPS ? 1.0 : pimu/sin(pimu));
		d = -log(x2);
		e=xmu*d;
		fact2 = (fabs(e) < EPS ? 1.0 : sinh(e)/e);
		beschb(xmu,&gam1,&gam2,&gampl,&gammi);
		ff=2.0/M_PI*fact*(gam1*cosh(e)+gam2*fact2*d);
		e=exp(e);
		p=e/(gampl*M_PI);
		q=1.0/(e*M_PI*gammi);
		pimu2=0.5*pimu;
		fact3 = (fabs(pimu2) < EPS ? 1.0 : sin(pimu2)/pimu2);
		r=M_PI*pimu2*fact3*fact3;
		c=1.0;
		d = -x2*x2;
		sum=ff+r*q;
		sum1=p;
		for (i=1;i<=MAXIT;i++) {
			ff=(i*ff+p+q)/(i*i-xmu2);
			c *= (d/i);
			p /= (i-xmu);
			q /= (i+xmu);
			del=c*(ff+r*q);
			sum += del;
			del1=c*p-i*del;
			sum1 += del1;
			if (fabs(del) < (1.0+fabs(sum))*EPS) break;
		}
		if (i > MAXIT)
		{	output("bessrl series failed to converge\n");
			error=1; return;
		}
		rymu = -sum;
		ry1 = -sum1*xi2;
		rymup=xmu*xi*rymu-ry1;
		rjmu=w/(rymup-f*rymu);
	} else {
		a=0.25-xmu2;
		p = -0.5*xi;
		q=1.0;
		br=2.0*x;
		bi=2.0;
		fact=a*xi/(p*p+q*q);
		cr=br+q*fact;
		ci=bi+p*fact;
		den=br*br+bi*bi;
		dr=br/den;
		di = -bi/den;
		dlr=cr*dr-ci*di;
		dli=cr*di+ci*dr;
		temp=p*dlr-q*dli;
		q=p*dli+q*dlr;
		p=temp;
		for (i=2;i<=MAXIT;i++) {
			a += 2*(i-1);
			bi += 2.0;
			dr=a*dr+br;
			di=a*di+bi;
			if (fabs(dr)+fabs(di) < FPMIN) dr=FPMIN;
			fact=a/(cr*cr+ci*ci);
			cr=br+cr*fact;
			ci=bi-ci*fact;
			if (fabs(cr)+fabs(ci) < FPMIN) cr=FPMIN;
			den=dr*dr+di*di;
			dr /= den;
			di /= -den;
			dlr=cr*dr-ci*di;
			dli=cr*di+ci*dr;
			temp=p*dlr-q*dli;
			q=p*dli+q*dlr;
			p=temp;
			if (fabs(dlr-1.0)+fabs(dli) < EPS) break;
		}
		if (i > MAXIT)
		{	output("cf2 failed in bessjy\n");
			error=1; return;
		}
		gam=(p-f)/q;
		rjmu=sqrt(w/((p-f)*gam+q));
		rjmu=SIGN(rjmu,rjl);
		rymu=rjmu*gam;
		rymup=rymu*(p+q/gam);
		ry1=xmu*xi*rymu-rymup;
	}
	fact=rjmu/rjl;
	*rj=rjl1*fact;
	*rjp=rjp1*fact;
	for (i=1;i<=nl;i++) {
		rytemp=(xmu+i)*xi2*ry1-rymu;
		rymu=ry1;
		ry1=rytemp;
	}
	*ry=rymu;
	*ryp=xnu*xi*rymu-ry1;
}
#undef EPS
#undef FPMIN
#undef MAXIT
#undef XMIN
#undef PI

void besselj (double *x, double *k, double *r)
{	double h1,h2;
	bessjy(*k,*x,r,&h1,&h2,&h2);
}

void mbesselj (header *hd)
{   spreadf2(besselj,0,0,hd);
	test_error("besselj");
}

void bessely (double *x, double *k, double *r)
{	double h1,h2;
	bessjy(*k,*x,&h1,r,&h2,&h2);
}

void mbessely (header *hd)
{   spreadf2(bessely,0,0,hd);
	test_error("bessely");
}

void mbesselall (header *hdx)
{	header *st=hdx,*hd=nextof(hdx);
	double bj,by,bdj,bdy;
	hd=getvalue(hd); if (error) return;
	hdx=getvalue(hdx); if (error) return;
	if (hd->type!=s_real || hdx->type!=s_real)
		wrong_arg_in("besselall");
	bessjy(*realof(hd),*realof(hdx),&bj,&by,&bdj,&bdy);
	if (error) return;
	newram=(char *)st;
	new_real(bj,""); if (error) return;
	new_real(by,""); if (error) return;
	new_real(bdj,""); if (error) return;
	new_real(bdy,"");
}

#define EPS 1.0e-16
#define FPMIN 1.0e-30
#define MAXIT 10000
#define XMIN 2.0
#define PI M_PI

void bessik(double xnu, double x, double *ri, double *rk, double *rip, double *rkp)
{
	void beschb(double x, double *gam1, double *gam2, double *gampl,
		double *gammi);
	void nrerror(char error_text[]);
	int i,l,nl;
	double a,a1,b,c,d,del,del1,delh,dels,e,f,fact,fact2,ff,gam1,gam2,
		gammi,gampl,h,p,pimu,q,q1,q2,qnew,ril,ril1,rimu,rip1,ripl,
		ritemp,rk1,rkmu,rkmup,rktemp,s,sum,sum1,x2,xi,xi2,xmu,xmu2;

	if (x <= 0.0 || xnu < 0.0)
	{
	  output("bad arguments in bessik\n");
	  error = 1; return;
	}
	nl=(int)(xnu+0.5);
	xmu=xnu-nl;
	xmu2=xmu*xmu;
	xi=1.0/x;
	xi2=2.0*xi;
	h=xnu*xi;
	if (h < FPMIN) h=FPMIN;
	b=xi2*xnu;
	d=0.0;
	c=h;
	for (i=1;i<=MAXIT;i++) {
		b += xi2;
		d=1.0/(b+d);
		c=b+1.0/c;
		del=c*d;
		h=del*h;
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > MAXIT)
	{	output("x too large in bessik\n");
		error=1; return;
	}
	ril=FPMIN;
	ripl=h*ril;
	ril1=ril;
	rip1=ripl;
	fact=xnu*xi;
	for (l=nl;l>=1;l--) {
		ritemp=fact*ril+ripl;
		fact -= xi;
		ripl=fact*ritemp+ril;
		ril=ritemp;
	}
	f=ripl/ril;
	if (x < XMIN) {
		x2=0.5*x;
		pimu=PI*xmu;
		fact = (fabs(pimu) < EPS ? 1.0 : pimu/sin(pimu));
		d = -log(x2);
		e=xmu*d;
		fact2 = (fabs(e) < EPS ? 1.0 : sinh(e)/e);
		beschb(xmu,&gam1,&gam2,&gampl,&gammi);
		ff=fact*(gam1*cosh(e)+gam2*fact2*d);
		sum=ff;
		e=exp(e);
		p=0.5*e/gampl;
		q=0.5/(e*gammi);
		c=1.0;
		d=x2*x2;
		sum1=p;
		for (i=1;i<=MAXIT;i++) {
			ff=(i*ff+p+q)/(i*i-xmu2);
			c *= (d/i);
			p /= (i-xmu);
			q /= (i+xmu);
			del=c*ff;
			sum += del;
			del1=c*(p-i*ff);
			sum1 += del1;
			if (fabs(del) < fabs(sum)*EPS) break;
		}
		if (i > MAXIT)
		{
		  output("bessk series failed to converge\n");
		  error = 1; return;
		}
		rkmu=sum;
		rk1=sum1*xi2;
	} else {
		b=2.0*(1.0+x);
		d=1.0/b;
		h=delh=d;
		q1=0.0;
		q2=1.0;
		a1=0.25-xmu2;
		q=c=a1;
		a = -a1;
		s=1.0+q*delh;
		for (i=2;i<=MAXIT;i++) {
			a -= 2*(i-1);
			c = -a*c/i;
			qnew=(q1-b*q2)/a;
			q1=q2;
			q2=qnew;
			q += c*qnew;
			b += 2.0;
			d=1.0/(b+a*d);
			delh=(b*d-1.0)*delh;
			h += delh;
			dels=q*delh;
			s += dels;
			if (fabs(dels/s) < EPS) break;
		}
		if (i > MAXIT)
		{
		  output("bessik: failure to converge in cf2\n");
		  error = 1; return;
		}
		h=a1*h;
		rkmu=sqrt(PI/(2.0*x))*exp(-x)/s;
		rk1=rkmu*(xmu+x+0.5-h)*xi;
	}
	rkmup=xmu*xi*rkmu-rk1;
	rimu=xi/(f*rkmu-rkmup);
	*ri=(rimu*ril1)/ril;
	*rip=(rimu*rip1)/ril;
	for (i=1;i<=nl;i++) {
		rktemp=(xmu+i)*xi2*rk1+rkmu;
		rkmu=rk1;
		rk1=rktemp;
	}
	*rk=rkmu;
	*rkp=xnu*xi*rkmu-rk1;
}
#undef EPS
#undef FPMIN
#undef MAXIT
#undef XMIN
#undef PI

void besseli (double *x, double *k, double *r)
{	double h1,h2;
	bessik(*k,*x,r,&h1,&h2,&h2);
}

void mbesseli (header *hd)
{   spreadf2(besseli,0,0,hd);
	test_error("besseli");
}

void besselk (double *x, double *k, double *r)
{	double h1,h2;
	bessik(*k,*x,&h1,r,&h2,&h2);
}

void mbesselk (header *hd)
{   spreadf2(besselk,0,0,hd);
	test_error("besselk");
}

void mbesselmodall (header *hdx)
{	header *st=hdx,*hd=nextof(hdx);
	double bj,by,bdj,bdy;
	hd=getvalue(hd); if (error) return;
	hdx=getvalue(hdx); if (error) return;
	if (hd->type!=s_real || hdx->type!=s_real)
		wrong_arg_in("besselmodall");
	bessik(*realof(hd),*realof(hdx),&bj,&by,&bdj,&bdy);
	if (error) return;
	newram=(char *)st;
	new_real(bj,""); if (error) return;
	new_real(by,""); if (error) return;
	new_real(bdj,""); if (error) return;
	new_real(bdy,"");
}


