.. The Demo. Starts automatically.

comment
Demo loading.
endcomment

demodelay=180;
reset();

function delay
	global demodelay;
	return demodelay
endfunction

function warten
	wait(delay());
	return 0;
endfunction

function weiter
	"" "<< Press return to continue >>" warten();
	return 120;
endfunction

function taste
	"" "<< Please press return to see graphics >>",
	return warten();
endfunction

function fisch
## plots a fish
	global Pi;
	t=0:3;
	ph=interp(t,[0.05,0.3,0.2,1.2]);
	pb=interp(t,[0.05,0.5,0.2,0.2]);
	phi=Pi-linspace(0,2*Pi,20);
	l=linspace(0,3,40);
	psin=dup(sin(phi),length(l));
	pcos=dup(cos(phi),length(l));
	lh=dup(interpval(t,ph,l),length(phi))';
	lb=dup(interpval(t,pb,l),length(phi))';
	x=pcos*lb; z=psin*lh; y=dup(l-1.5,length(phi))';
	view(2.5,1.5,0.3,0.1);
	solid(x,y,z);
	title("A solid fish"); warten();
	huecolor(1); solidhue(x,y,z,-y,1);
	title("A shaded fish"); warten();
	return 0;
endfunction

function mindemo ()
	fcd("sin(x)*x+cos(y)*y",50,0,10,0,10); xplot(); warten();
	mi=neldermin("sin(x[1])*x[1]+cos(x[2])*x[2]",[5,5]);
	color(10); hold on; mark(mi[1],mi[2]); hold off; color(1);
	title("Local Minima");
	warten();
	mi=neldermin("sin(x[1])*x[1]+cos(x[2])*x[2]",[5,8]);
	color(10); hold on; mark(mi[1],mi[2]); hold off; color(1);
	warten();
	return "";
endfunction

function grafikdemo
## grafikdemo shows the grafik capabilities of EULER
	fullwindow(); hold off;
	global Pi;

## Abbildungen von R nach R
	t=-Pi:Pi/20:Pi;
	shrinkwindow(); setplot(-Pi,Pi,-2,2);
	linewidth(4); plot(t,sin(t)); linewidth(1); hold on;
	color(2); style("."); plot(t,cos(t)); 
	color(3); style("--"); plot(t,(sin(t)+cos(t))/2);
	xgrid(-3:1:3,1); ygrid(-1:1,1);
	title("2D plots");
	color(1); style(""); fullwindow(); hold off;
	warten();
	
## Abbildungen von R nach R^2 in vier Fenstern
	t=-Pi:Pi/50:Pi;
	window(10,textheight()*1.5,490,490);
	plot(sin(3*t)/3+cos(t),sin(3*t)/3+sin(t)); hold on;
	title("Parametric curves"); 
	window(510,textheight()*1.5,1010,490);
	plot(sin(2*t)/3+cos(t),sin(2*t)/3+sin(t));
	window(10,510,490,1010);
	plot(sin(2*t),sin(3*t));
	window(510,510,1010,1010);
	t=t+2*Pi;
	plot(cos(t)*log(t),sin(t)*log(t));
	hold off;
	warten();

## Bar plots
	shrinkwindow();
	r=normal(1,100);
	c=count(r+5,10); c=c/sum(c);
	setplot(-5,5,0,0.5); clg;
	barstyle("\/"); b=barcolor(1); plotbar(-5:4,0,1,c); barcolor(b);
	t=linspace(-5,5,300);
	hold on; xplot(t,exp(-t^2/2)/sqrt(2*pi)); hold off;
	title("A statistical experiment");
	style("");
	warten();

## x^2+y^3 als mesh und contour.
	fullwindow();
	x=pi*(-1:0.1:1); y=x';
	mesh(x*x+y*y*y); title("x^2+y^3"); warten();
	contour(x*x+y*y*y,-30:5:30);
	title("Contour plot of this function"); warten();
	x=(-1:0.04:1)*pi; y=x';
	huecolor(1); density(sin(2*x)*cos(2*y),1);
	title("Density plot of a function"); warten();

## e^-r cos(r) als Drahtmodell.
	x=pi*(-1:0.1:1); y=x';
	r=x*x+y*y; f=exp(-r)*cos(r);
	view(2,1,0,0.5); ## [ Abstand, Zoom, Winkel rechts, Winkel hoch ]
	wire(x/Pi,y/Pi,f);
    title("Wire frame model"); warten();
	meshbar(f);
	title("Bar plot of this function"); warten();
	meshbar(-f);
	title("Negative bar plot"); warten();

## Der Fisch als solides Modell.
	fisch();
	view(2,1,0.2,0.3);

## Eine complexe Abbildung w=z^1.3
	setplot(-1, 2, -0.5, 1.5);
	t=0:0.1:1; z=dup(t,length(t)); z=z+1i*z';
	color(1); style("."); cplot(z); hold on;
	color(2); style(""); cplot(z^1.3); color(1);
	xgrid(0); ygrid(0);
	title("Complex numbers w=z^1.3"); hold off;
	warten();

	shrinkwindow;
	return 0
endfunction

function vectorfield (expr,x1,x2,y1,y2,nx=20,ny=20)
## Draw the vector field of a differential equation in x and y.
## expr must be the expression "f(x,y)", which computes the
## derivative of y(x) at x.
	setplot(x1,x2,y1,y2);
	x=linspace(x1,x2,nx-1);
	y=linspace(y1,y2,ny-1);
	{X,Y}=field(x,y);
	V=expreval(expr,X,y=Y);
	n=prod(size(X));
	h=(x2-x1)/nx/4;
	v=redim(V,[n,1]);
	r=sqrt(v*v+1);
	x=redim(X,[n,1]); x=(x-h/r)|(x+h/r);
	y=redim(Y,[n,1]); y=(y-h*v/r)|(y+h*v/r);
	st=linestyle("->"); xplot(x,y); linestyle(st);
	return plot();
endfunction

function norm (v)
	return sqrt(sum(v*v))
endfunction

function f3body (x,y)
	y1=y[1,1:2]; y2=y[1,3:4]; y3=y[1,5:6];
	d21=(y2-y1)/norm(y2-y1)^3;
	d31=(y3-y1)/norm(y3-y1)^3;
	d32=(y3-y2)/norm(y3-y2)^3;
	return y[7:12]|(d21+d31)|(-d21+d32)|(-d31-d32);
endfunction

function test3body (y1)
	y=y1;
	x=0; h=0.01;
	setplot(-3,3,-3,3); clg; hold off; xplot(); frame();
	title("3-body problem");
	testkey();
	repeat;
		loop 1 to 10;
			ynew=runge2("f3body",x,x+h,y,0.0001,h);
			H=(y_ynew)';
			hold on;
			color(3); plot(H[1],H[2]);
			color(4); plot(H[3],H[4]);
			color(5); plot(H[5],H[6]);
			y=ynew; x=x+h;
			hold off;
		end;
		color(1);
		if (any(abs(y[1:6])>4));
			break;
		endif;
	end;
	return x
endfunction

function dgldemo

## Differentialgleichung
	vectorfield("-2*x*y",0,2,0,2);
	t=0:0.05:2; s=heun("-2*x*y",t,1); hold; plot(t,s); hold;
	title("Solving a differential equation");
	warten();
	test3body([-2,0,2,0,0,2,0.1,-0.2,0,0.2,0.1,0]);
	
	return 0;
endfunction

function grafiktutor
## Erkl„rt die Grafikfunktionen
	hold off;
	global Pi;
	shrinkwindow();

	x=linspace(-1,1,50); T=cos((1:5)'*acos(x));
	setplot(-1.5,1.5,-1.5,1.5);
	plot(x,T); xgrid(-2:2,1); ygrid(-2:2,1); hold off; warten();

	t=-1:0.01:1; setplot(-1,1,-10,10); clg; xplot();
	hold on; plot(t,1/t); plot(t,1/t^2); hold off;
	title("1/t and 1/t^2"); warten();

	x=-1:0.1:1; y=x';
	z=x*x+x*y-y*y; fullwindow(); mesh(z); warten();
	contour(z,-3:0.5:3); warten();
	view(2.5,1,1,0.5); solid(x,y,z); warten();
	wire(x,y,z); warten();
	
	return 0
endfunction

function torus
## some torus type bodies.
	view([4,1,0,0.6]);
	x=linspace(0,2*pi,40);
	y=linspace(0,2*pi,20)';
## faster than {phi,psi}=field(x,y); cosphi=cos(phi); ...

## a torus with a thick and a thin side.
	factor=1.5+cos(y)*(cos(x)/2+0.6);
	X=cos(x)*factor;
	Y=sin(x)*factor;
	Z=sin(y)*(cos(x)/2+0.6);
	solid(X,Y,Z); warten();
	
## a deformed torus
	factor=1.5+cos(y);
	X=cos(x)*factor;
	Y=sin(x)*factor;
	Z=sin(y)+cos(2*x);
	solid(X,Y,Z); warten();
	
## the Moebius band
	t=linspace(-1,1,20)';
	x=linspace(0,pi,40);
	factor=2+cos(x)*t;
	X=cos(2*x)*factor;
	Y=sin(2*x)*factor;
	Z=sin(x)*t;
	solid(X,Y,Z); warten();

	return 0;
endfunction

function tube
## some tube like bodies.
	view([3,1,0,0.1]);
	global Pi;
	x=linspace(0,2*Pi,40);

## a atomic modell or so.
	y=0.1+(sin(linspace(0,Pi,15))| ..
		1.5*sin(linspace(0,Pi,10))|sin(linspace(0,Pi,15)));
	cosphi=dup(cos(x),length(y));
	sinphi=dup(sin(x),length(y));
	f=dup(y',length(x));
	
	solid(f*cosphi,f*sinphi,dup(linspace(-2,2,length(y)-1)',length(x)));
	warten(); 

## a black hole
	t=linspace(0,1,20);
	cosphi=dup(cos(x),length(t));
	sinphi=dup(sin(x),length(t));
	f=dup((t*t+0.2)',length(x));
	view([3,1.5,0,0.7]);
	solid(f*cosphi,f*sinphi,dup(t'*2-1,length(x)));
	warten();

	return 0;
endfunction

function minimal (r,phi)
	R=dup(r',length(phi));
	X=R*dup(cos(phi),length(r))+R*R/2*dup(cos(2*phi),length(r));
	Y=-R*dup(sin(phi),length(r))-R*R/2*dup(sin(2*phi),length(r));
	Z=4/3*dup((r^1.5)',length(phi))*dup(cos(1.5*phi),length(r))-1;
	view(9,1,-1.5,0.2);
	solid(X,Y,Z);
	title("A minmal surface");
	warten();
	return 0;
endfunction

function spiral
	{r,a}=field(0:0.1:1,0:pi()/8:6*pi());
	z=a/8;
	x=r*cos(a)*(1-a/20);
	y=r*sin(a)*(1-a/20);
	z=z-1.5;
	view(4,1.5,0.5,0.3);
	framedsolid(x,y,z); warten();
	return 0;
endfunction

function rings
	rr=0.2;
	t=linspace(0,2*pi,10);
	s=linspace(0,2*pi,41); n=length(s);
	r=dup(1+cos(t)*rr,n)'; m=length(t);
	x=dup(cos(s),m)*r; y=dup(sin(s),m)*r;
	z=dup(sin(t)*rr,n)';
	view([4,1.5,0.3,0.3]);
	X=x_(x+1.3)_(x-1.3);
	Y=y_-z_-z;
	Z=z_y_y;
	solid(X,Y,Z,[m,2*m]);
	title("Disconnnected surfaces");
	warten();
	return 0;
endfunction

function startdemo
	{x,y}=field(-1:0.1:1,-1:0.1:1);
	z=x*x+y*y;
	mesh(z); warten();
	return 0;
endfunction

function knot()
	t=linspace(0,2*pi,200);
	x=sin(t)+2*sin(2*t);
	y=cos(t)-2*cos(2*t);
	z=sin(3*t);
	xv=cos(t)+4*cos(2*t);
	yv=-sin(t)+4*sin(2*t);
	zv=3*cos(3*t);
	n=sqrt(xv*xv+yv*yv+zv*zv); 
	xv=xv/n; yv=yv/n; zv=zv/n;
	xv1=1-xv*xv; yv1=-xv*yv; zv1=-xv*zv;
	n=sqrt(xv1*xv1+yv1*yv1+zv1*zv1); 
	xv1=xv1/n; yv1=yv1/n; zv1=zv1/n;
	xv2=yv*zv1-yv1*zv;
	yv2=-xv*zv1+xv1*zv;
	zv2=xv*yv1-xv1*yv;
	n=sqrt(xv2*xv2+yv2*yv2+zv2*zv2); 
	xv2=xv2/n; yv2=yv2/n; zv2=zv2/n;
	p=linspace(0,2*pi,20)';
	r=0.3;
	X=x+r*(cos(p)*xv1+sin(p)*xv2);
	Y=y+r*(cos(p)*yv1+sin(p)*yv2);
	Z=z+r*(cos(p)*zv1+sin(p)*zv2);
	view(5,2,0.5,0.7);
	framedsolidhue(X,Y,Z,cos(p)*xv1+sin(p)*xv2,2,1);
	title("A trefoil knot");
	warten();
	return 0;
endfunction

function demo3d
	startdemo();
	torus();
	tube();
	minimal(0.1:0.1:2.5,0:pi()/20:2*pi());
	spiral();
	rings();
	knot();
	return 0;
endfunction

function g(x)
	return x^3+x^2/2-x
endfunction

function g1(x)
	return 3*x^2+x-1
endfunction

function apple1 (z,z0)
## one iteration
	w=z0+z*z;
	return w
endfunction

function apple (z)
## compute 10 iterations
	w=z;
	loop 1 to 7; w=apple1(w,z); end:
	return w
endfunction

function julia (z,z0)
## compute 10 iterations
	w=z;
	loop 1 to 7; w=z0+w*w; end:
	return w
endfunction

function showapple (a)
## show the apple set in 3D
	{x,y}=field(linspace(a[1],a[2],50),linspace(a[3],a[4],50));
	z=x+1i*y; w=apple(z);
	view(5,3,-0.75,0.7); twosides(0);
	wa=abs(w); wa=max(wa,1);
	l=log(wa); l=2*l/max(max(l)');
	framedsolid(x,y,-l,1);
	return wa;
endfunction

function showjulia (z0,a)
## show the apple set in 3D
	{x,y}=field(linspace(a[1],a[2],50),linspace(a[3],a[4],50));
	z=x+1i*y; w=julia(z,z0);
	view(5,3,-0.75,0.7); twosides(0);
	wa=abs(w); wa=max(wa,1);
	l=log(wa); l=2*l/max(max(l)');
	framedsolid(x,y,-l,1);
	return wa;
endfunction

function showcontour(w,x)
## show the iterations
	clg;
	shrinkwindow();
	wl=log(w); wl=wl/totalmax(wl);
		clg;
		setplot(x);
		density(wl*0.8+0.1); hold on; xplot();
		contour(wl,linspace(epsilon,1,10));
		t=linspace(0,2*pi,500);
		color(2);
		plot(0.5*cos(t)*(1-cos(t))+0.25,0.5*sin(t)*(1-cos(t)));
		color(1);
		hold off;
		title("Mandelbrot set Conchoide");
		warten();
		r=0;
		z=1+1i;
		xh=[-3,3,-3,3];
		setplot(xh);
		wa=showjulia(z,xh);
		title(printf("Julia set at (%g,",re(z))|printf("%g)",im(z)));
		warten();
		clg;
		wa=log(wa); wa=wa/totalmax(wa);
		density(wa*0.8+0.1); hold on; xplot();
		contour(wa,linspace(epsilon,1,10));
		t=sqrt(1-4*z);
		color(3); mark([(1-t)/2,(1+t)/2]); color(1);
		hold off;
		title("The Fixpoints");
		warten();		
	hold off;
	return r
endfunction

function appletest()
	x=[-2,0.5,-1.25,1.25];
    w=showapple(x);
    title("Potential of Mandelbrot set");
    warten();
	showcontour(w,x);
	return w;
endfunction

function bezier (p,t)
## Evaluate sum p_i B_{i,n}(t) the easy and direct way.
## p must be a k x n+1 matrix (n+1) points, dimension k.
	n=cols(p)-1; i=nonzeros(t~=1); if (cols(i)>0); t[i]=0.999; endif;
	T=dup(t/(1-t),n)';
	b=((1-t')^n)|(T*dup((n-(1:n)+1)/(1:n),cols(t)));
	b=cumprod(b);
	if (cols(i)>0); b[i]=zeros(1,n)|1; endif;
	return p.b';
endfunction

function bezier2 (p,t)
## Evaluate sum p_i B_{i,n}(t) the complicated way.
## p_i is a k x n+1 matrix (n+1) points, dimension k.
	n=cols(p)-1;
	{T,N}=field(t,0:n);
	b=bin(n,N)*T^N*(1-T)^(n-N);
	return p.b;
endfunction

function bezier3d (p)
## Shows a 3D Bezier curve and its polygon
	t=linspace(0,1,300);
	s=bezier(p,t);
	f=getframe(p[1],p[2],p[3]);
	h=holding(1); if !h; clg; endif;
	co=color(2); frame1(f);
	color(1); wire(p[1],p[2],p[3]);
	color(3); wire(s[1],s[2],s[3]);
	color(2); frame2(f);
	color(co);
	holding(h);
	title("3D Bezier curve");
	warten();
	return "";
endfunction	

function bezier3dtest (alpha=0.5,beta=0.5)
## Show a Beziercurve of dimension 3
	p=[-1,-1,-1;0,-1,-1;1,0,0;1,1,0;0,1,1;-1,1,0]';
	view(3,1.5,alpha,beta);
	bezier3d(p);
	return "";
endfunction

function nurbs (p,b,t)
## Cumpute a rational Bezier curve with control points p
## and weights b.
	K=rows(p);
	pp=(p*dup(b,K))_b;
	s=bezier(pp,t);
	return s[1:K]/(dup(s[K+1],K));
endfunction

function nurbstest
## Show some NURBS.
	p=[-1,0;0,1;1,0]';
	b=[1,1,1];
	bb=2^(-5:5);
	setplot(-1,1,0,2); clg; frame(); xplot();
	hold on; linewidth(2); plot(p[1],p[2]); linewidth(1); hold off;
	t=linspace(0,1,300);
	loop 1 to cols(bb);
		b[2]=bb[#];
		s=nurbs(p,b,t);
		hold on; plot(s[1],s[2]); hold off;
	end;
	title("Quadratic NURBS");
	warten();
	return "";
endfunction

function beziersurface (x,y,z,n=20)
## Compute a Bezier surface. Return {bx,by,bz}.
	t=linspace(0,1,n);
	n=rows(x)-1; i=nonzeros(t~=1);  if (cols(i)>0); t[i]=0.999; endif;
	T=dup(t/(1-t),n)';
	b1=((1-t')^n)|(T*dup((n-(1:n)+1)/(1:n),cols(t)));
	b1=cumprod(b1);
	if (cols(i)>0); b1[i]=zeros(1,n)|1; endif;
	n=cols(x)-1; i=nonzeros(t~=1);
	T=dup(t/(1-t),n)';
	b2=((1-t')^n)|(T*dup((n-(1:n)+1)/(1:n),cols(t)));
	b2=cumprod(b2);
	if (cols(i)>0); b2[i]=zeros(1,n)|1; endif;
	return {b1.x.b2',b1.y.b2',b1.z.b2'};
endfunction

function beziersurftest
## Show a Bezier surface
	{x,y}=field(-1:0.5:1,-1:0.5:1);
	z=2*exp(-(0.5*x*x+y*y))-1;
	view(2.5,1.5,0.5,0.5);
	{xb,yb,zb}=beziersurface(x,y,z);
	wi=wirecolor(1); linewidth(3);
	wire(x[3:5,1:5],y[3:5,1:5],z[3:5,1:5]); hold on;
	linewidth(1); wirecolor(wi);
	solid(xb,yb,zb);
	wi=wirecolor(1); linewidth(3);
	wire(x[1:3,1:5],y[1:3,1:5],z[1:3,1:5]); hold on;
	linewidth(1); wirecolor(wi);
	hold off;
	title("A Bezier surface");
	warten();
	return "";
endfunction

function c1test
## Show how two bezier surfaces can be joined.
	x1=dup(-0.5:0.25:0.5,5);
	y1=dup([0,0,0,0,1],5);
	z1=dup(1:0.25:2,5)';
	{xb1,yb1,zb1}=beziersurface(x1,y1,z1,10);
	x2=dup(-0.5:0.25:0.5,5);
	y2=(-ones(4,5))_[0,0,0,0,0];
	z2=dup(-1:0.25:0,5)';
	{xb2,yb2,zb2}=beziersurface(x2,y2,z2,10);
	x=zeros(5,5); y=x; z=x;
	x[1]=x1[1]; x[2]=x[1]-(x1[2]-x1[1]);
	x[5]=x2[5]; x[4]=x[5]+(x2[5]-x2[4]);
	x[3]=(x[4]+x[2])/2;
	y[1]=y1[1]; y[2]=y[1]-(y1[2]-y1[1]);
	y[5]=y2[5]; y[4]=y[5]+(y2[5]-y2[4]);
	y[3]=(y[4]+y[2])/2;
	z[1]=z1[1]; z[2]=z[1]-(z1[2]-z1[1]);
	z[5]=z2[5]; z[4]=z[5]+(z2[5]-z2[4]);
	z[3]=(z[4]+z[2])/2;
	{xb,yb,zb}=beziersurface(x,y,z,10);
	view(4,2,0.5,0.5);
	solid(xb1,yb1,zb1-1); hold on;
	solid(xb,yb,zb-1); solid(xb2,yb2,zb2-1);
	hold off;
	title("C1 continuity of Bezier surfaces");
	warten();
	wi=wirecolor(1);
	linewidth(1); wire(x,y,z-1); linewidth(3);
	hold on; wire(x1,y1,z1-1); wire(x2,y2,z2-1); hold off;
	wirecolor(wi); linewidth(1);
	title("C1 continuity of the Bezier grid");
	warten();
	return "";
endfunction

function feigen (x,l)
	return l*x*(1-x);
endfunction

function feigendemo
	l=3:0.005:4;
	x=0.5*ones(size(l));
	h=niterate("feigen",x,200;l); h=h[100:200];
	style("m.");
	xmark(l,h);
	title("Feigenbaum Diagram");
	warten();
	return ""
endfunction

function beziertest
	bezier3dtest();
	nurbstest();
	beziersurftest();
	c1test();
	return ""
endfunction

function force (x,y)
## simple force of a single body at (0,0)
	r=x*x+y*y; r=r*sqrt(r);
	return {-x/r,-y/r}
endfunction

function dgl (t,p)
	{fx,fy}=force(p{1},p{2});
	return [p{3},p{4},fx,fy];
endfunction

function showcurve
## solves the one body problem.
	t=linspace(0,4,100);
	clg; setplot(-1.5,1.5,-1.5,1.5); xplot(); hold on;
	s=heun("dgl",t,[1,0,0,1]);
	plot(s[1],s[2]);
	s=heun("dgl",t,[1,0,0,0.9]);
	plot(s[1],s[2]);
	s=heun("dgl",t,[1,0,0,0.8]);
	plot(s[1],s[2]);
	s=heun("dgl",t,[1,0,0,0.7]);
	plot(s[1],s[2]);
	hold off;
	title("Planet orbits");
	warten();
	return "";
endfunction

function showpotential
## demonstrates the Newton potential and a kepler elipse drawn on it.
	{x,y}=field(linspace(-0.9,1.1,23),linspace(-1,1,23));
	p=max((-1)/sqrt(x*x+y*y),-10)+0.5;
	view(3,4,0.5,1.2);
	solid(x,y,p); hold on;
	t=linspace(0,3.5,150);
	s=heun("dgl",t,[1,0,0,0.7]);
	ps=-1/sqrt(s[1]*s[1]+s[2]*s[2])+0.5;
	color(2); wire(s[1],s[2],ps); hold off;
	title("Orbit projected to gravity potential");
	warten();
	return 0;
endfunction

function keplerdemo
	showcurve();
	showpotential();
	return ""
endfunction

function flower (x=0,y=0,a=90,l=1,dl=0.8,n=5,d=10,t=0.98)
	r=random([1,5]);
	if n==0 || r[5]>t;
		style("m<>"); mark([x],[y]);
	else
		a1=a-(1+r[1]/2)*d;
		l1=l*(1+r[2]/2);
		x1=x+cos(a1*pi/180)*l1;
		y1=y+sin(a1*pi/180)*l1;
		plot([x,x1],[y,y1]);
		flower(x1,y1,a1,dl*l,dl,n-1,d);
		a1=a+(1+r[3]/2)*d;
		l1=l*(1+r[4]/2);
		x1=x+cos(a1*pi/180)*l1;
		y1=y+sin(a1*pi/180)*l1;
		plot([x,x1],[y,y1]);
		flower(x1,y1,a1,dl*l,dl,n-1,d);
	endif;
	return "";
endfunction

function flowerdemo()
	clg;
	setplot(-3,3,-1,5);
	hold on; flower(); hold off;
	title("A fractal Flower");
	warten();
	return "";
endfunction

function lorenz (t,x)
	return [-3*(x[1]-x[2]),-x[1]*x[3]+26.5*x[1]-x[2],x[1]*x[2]-x[3]]
endfunction

function lorenza
	t=0:0.02:40;
	s=adaptiverunge("lorenz",t,[0,0.1,0],0.001);
	v=view(4,2,0.5,0.5);
	framedwire(s[1]/20,s[2]/20,s[3]/20-1);
	title("The Lorenz attractor");
	view(v);
	warten();
	return ""
endfunction

function J(z)
	return (z+1/z)/2
endfunction

function invJ(z)
	w=sqrt(z^2-1);
	return z+(re(z)>0)*w-(re(z)<=0)*w;
endfunction

function K(z,a)
	return (z+a)/(1+conj(a)*z)
endfunction

function H(z)
	return (1i*z+1)/(1i+z)
endfunction

function invH(z)
	return (1i*z-1)/(1i-z)
endfunction

function L(z,a)
	return (a/conj(a))*(conj(a)*z-1)/(a-z)
endfunction

function complexdemo
	I=1i;
	r=exp(linspace(0.000001,1,20)); phi=linspace(0.0000001,2*pi,120)';
	z=r*exp(phi*I);
	setplot(-4,4,-4,4); cplot(z); xplot();
	title("Outside of circle mapped to ..."); warten();
	setplot(-1.5,1.5,-1.5,1.5); cplot(J(z)); xplot();
	title("Outside of interval"); warten();
	setplot(-2,2,-2,2); cplot(J(I*invJ(J(z)*sqrt(2)))); xplot();
	title("Outside of cross"); warten();
	setplot(-1,1,-1,1); cplot(K(1/z,0.5)); xplot();
	title("Inside to inside");
	warten();
	setplot(-3,3,0,6); cplot(H(z)); xplot();
	title("Outside to upper half plane");
	warten(20);
	setplot(-6,6,-6,6); cplot(L(z,5)); xplot();
	title("5 to infinity");
	warten();
	setplot(-2,2,-1.5,2.5); cplot(invH(J(L(z,-invJ(1i))))); xplot();
	title("Outside of half circle");
	warten();
	return "";
endfunction

function autodemo (d=10)
	global demodelay;
	demodelay=d;
	repeat
		grafikdemo();
		demo3d();
		mindemo();
		grafiktutor();
		dgldemo();
		appletest();
		beziertest();
		keplerdemo();
		feigendemo();
		flowerdemo();
		lorenza();
		complexdemo();
	end;
endfunction

autodemo(5);

