comment
Compute Bezier curves.
For a test try:
beziertest; bezier3dtest; nurbstest; beziersurftest; c1test;
Uses the following functions:
bezier, bezier3d, nurbs, beziersurface
endcomment

reset();

function bezier (p,t)
## Evaluate sum p_i B_{i,n}(t) the easy and direct way.
## p must be a k x n+1 matrix (n+1) points, dimension k.
	n=cols(p)-1; i=nonzeros(t~=1);
	if cols(i)>0; t[i]=0.9999*ones(size(i)); endif;
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

function gammatest (N=[10,20,50,100])
## Bezier curve approximating a circle
	x=linspace(0,1,100);
	linewidth(4); xplot(cos(2*pi*x),sin(2*pi*x)); linewidth(1);
	loop 1 to cols(N);
	n=N[#];
	t=sqrt(linspace(0,1,n));
	p=cos(2*pi*t)_sin(2*pi*t);
	y=bezier(p,x);
	hold on; plot(y[1],y[2]); hold off;
	end;
	return "";
endfunction

function gammatest1 (N=[10,20,50,100])
## Bezier curve approximating a circle
	x=linspace(0,1,100);
	linewidth(4); xplot(cos(2*pi*x),sin(2*pi*x)); linewidth(1);
	loop 1 to cols(N);
	n=N[#];
	t=linspace(0,1,n);
	p=cos(2*pi*t)_sin(2*pi*t);
	y=bezier(p,x);
	hold on; plot(y[1],y[2]); hold off;
	end;
	return "";
endfunction

function beziertest
## The user clicks some points, which form the Bezier polygon.
## The program draws the Bezier curve
	setplot(-1,1,-1,1);
	xplot(-1,1); hold on;
	p=zeros(2,0);
	title("Mark points (click here for end)");
	repeat
		m=mouse();
		if (cols(p)>0) && (m[2]>1); break; endif;
		p=p|m';
		plot(p[1],p[2]);
	end;
	t=linspace(0,1,300);
	s=bezier(p,t);
	color(2); plot(s[1],s[2]);
	hold off;
	return "";
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
	return title("Quadratic NURBS");
endfunction

function beziersurface (x,y,z,n=20)
## Compute a Bezier surface. Return {bx,by,bz}.
	t=linspace(0,0.99999,n);
	n=rows(x)-1; i=nonzeros(t~=1);
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
	return title("A Bezier surface");
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
	wait(180);
	wi=wirecolor(1);
	linewidth(1); wire(x,y,z-1); linewidth(3);
	hold on; wire(x1,y1,z1-1); wire(x2,y2,z2-1); hold off;
	wirecolor(wi); linewidth(1);
	return title("C1 continuity of the Bezier grid");
endfunction

.. EOF
