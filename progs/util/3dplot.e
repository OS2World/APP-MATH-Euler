comment
3D plots
endcomment

function f3d (ffunction,n=10)
## f3d("f") or f3d("f",n;...)
## Draw a function defined on [-1,1]^2.
## f3d uses map for the evaluation of the function "f".
## n is the spacing.
	x=-1:1/n:1;
	{X,Y}=field(x,x);
	if isfunction(ffunction); Z=map(ffunction,X,Y;args());
	else; Z=expreval(ffunction,X,y=Y);
	endif;
	solid(X,Y,Z);
	return ""
endfunction

function f3dpolar (ffunction,n=10)
## f3dpolar("f") or f3dpolar("f",n;...)
## Draw a function defined on the unit circle.
## f3d uses map for the evaluation of the function "f".
## n is the spacing.
	t=linspace(0,2*pi,3*n); r=linspace(0,1,n)'; x=cos(t)*r; y=-sin(t)*r;
	if isfunction(ffunction); Z=map(ffunction,x,y;args());
	else; Z=expreval(ffunction,x,y=y);
	endif;
	solid(x,y,Z);
	return ""
endfunction

function f3dplot (ffunction,xmin=-1,xmax=1,ymin=-1,ymax=1,nx=20,ny=20)
## f3dplot("f") plots a function f(x,y,...) in a square.
## Also f3dplot("f",xmin,xmax,ymin,ymax,nx,ny;...).
## f3dplot uses map for the evaluation of the function "f".
## "f" may be an expression in x and y.
	lx=linspace(xmin,xmax,nx-1);
	ly=linspace(ymin,ymax,ny-1);
	{x,y}=field(lx,ly);
	if isfunction(ffunction); z=map(ffunction,x,y;args());
	else z=expreval(ffunction,x,y=y);
	endif;
	return framedsolid(x,y,z,2);
endfunction

function f3daxis (ffunction,n=10)
## f3daxis("f") or f3daxis("f",n;...)
## Draw a function defined on [-1,1]^2 with x-, y- and z-axis.
## f3daxis uses map for the evaluation of the function "f".
## 2n is the spacing.
## Fixed view, cannot be rotated.
	x=-1:1/n:1; y=x;
	{X,Y}=field(x,y);
	if isfunction(ffunction); Z=map(ffunction,X,Y;args());
	else; Z=expreval(ffunction,X,y=Y);
	endif;
	view(6,3,0.4,0.2);
	z1=totalmin(Z); z2=totalmax(Z); h=(z2+z1)/2; Z=Z-h;
	i=n+1:2*n+1; j=1:n+1;
	hold off; solid(X[i,j],Y[i,j],Z[i,j]); hold on;
	linewidth(3); 
	wire([0,0],[0,0],[-1.5,1.5]);
	wire([-1.5,0],[0,0],[-h,-h]);
	wire([0,0],[0,1.5],[-h,-h]);
	linewidth(1);
	solid(X[i,i],Y[i,i],Z[i,i]);
	solid(X[j,j],Y[j,j],Z[j,j]);
	linewidth(3);
	wire([0,1.5],[0,0],[-h,-h]);
	wire([0,0],[-1.5,0],[-h,-h]);
	linewidth(1);
	solid(X[j,i],Y[j,i],Z[j,i]); hold off;
	return ""
endfunction

function stereo (fff)
## Calls the function fff, which must be the name of
## a 3D plot command, and does it twice in two different
## windows. Many people are able to view a 3D picture.
## Example: stereo("f3dplot","x*y^2");
	hold on;
	clg;
	s=300; m=510;
	win=window();
	v=view();
	window(m-s,m-s/2,m,m+s/2);
	w=v; w[3]=v[3]-0.05;
	view(w);
	fff(args(2));
	window(m,m-s/2,m+s,m+s/2);
	w=v; w[3]=v[3]+0.05;
	view(w);
	fff(args(2));
	window(win);
	hold off;
	return ""
endfunction

function anaglyphwire (x,y,z)
## Plots an Anaglyph in red-cyan
	setcolor(13,1,0,0);
	setcolor(14,0,1,1);
	v=view();
	wirecolor(14); wire(x,y,z);
	view([v[1],v[2],v[3]+0.2/v[1],v[4]]);
	wirecolor(13); hold on; wire(x,y,z); hold off;
	wirecolor(1);
	view(v);
	return ""
endfunction

function solidhue (x,y,z,h,f=1)
## solidhue(x,y,z,h) makes a shaded solid 3D-plot of x,y,z.
## h is the shading and should run between 0 and 1.
## f determines, if h is scaled to fit in between 0 and f.
	if argn()==1; return _solidhue(x,y,z,h); endif;
	ma=max(max(h)'); mi=min(min(h)'); delta=ma-mi;
	if delta~=0; delta=1; endif;
	hh=(h-mi)/delta*f*0.9999;
	return _solidhue(x,y,z,hh);
endfunction

function fillcolor (c1=0,c2=0)
## Set the fillcolor for the fore- and background of 3D plots.
## fillcolor(c1,c2) or fillcolor() returns the colors [c1,c2].
	if argn()==0; return _fillcolor();
	else if argn==1; return _fillcolor(c1); endif;
	endif;
	return _fillcolor([c1,c2]);
endfunction

function mark3 (x,y,z)
## Plots points in three dimensions. 
## x,y,z must be 1xn vectors.
	{c0,r0}=project(x,y,z);
	{x0,y0}=fromscreen(c0,r0);
	return mark(x0,y0);
endfunction

function fcd (ffunction,n=10,xmin=-1,xmax=1,ymin=-1,ymax=1,nc=20)
## plots a function of two variables or expression of x and y
## with density and contour.
## fcd uses map for the evaluation of the function "f".
	x=linspace(xmin,xmax,n); y=linspace(ymin,ymax,n);
	{X,Y}=field(x,y);
	if isfunction(ffunction); Z=map(ffunction,X,Y;args());
	else; Z=expreval(ffunction,X,y=Y);
	endif;
	z1=totalmin(Z); z2=totalmax(Z);
	density(Z,1);
	h=holding(1); contour(Z,linspace(z1,z2,nc)); holding(h);
	setplot(xmin,xmax,ymin,ymax);
	return ""
endfunction

function fcontour (ffunction,n=10,xmin=-1,xmax=1,ymin=-1,ymax=1,nc=20)
## Draw contour lines of a function or expression in x and y.
## 2n is the spacing.
## fcontour uses map for the evaluation of the function "f".
	x=linspace(xmin,xmax,n); y=linspace(ymin,ymax,n);
	{X,Y}=field(x,y);
	if isfunction(ffunction); Z=map(ffunction,X,Y;args());
	else; Z=expreval(ffunction,X,y=Y);
	endif;
	z1=totalmin(Z); z2=totalmax(Z);
	contour(Z,linspace(z1,z2,nc));
	setplot(xmin,xmax,ymin,ymax);
	return ""
endfunction

function fniveau (ffunction,niveau=0,n=10,xmin=-1,xmax=1,ymin=-1,ymax=1,nc=20)
## Draw contour lines of a function or expression in x and y.
## 2n is the spacing.
## fcontour uses map for the evaluation of the function "f".
	x=linspace(xmin,xmax,n); y=linspace(ymin,ymax,n);
	{X,Y}=field(x,y);
	if isfunction(ffunction); Z=map(ffunction,X,Y;args());
	else; Z=expreval(ffunction,X,y=Y);
	endif;
	z1=totalmin(Z); z2=totalmax(Z);
	contour(Z,niveau);
	setplot(xmin,xmax,ymin,ymax);
	return ""
endfunction

function f3daxispolar (ffunction,n=10)
## f3daxispolar("f") or f3daxispolar("f",n;...)
## Draw a function defined on [-1,1]^2 with x-, y- and z-axis.
## 2n is the spacing.
## f3daxispolar uses map for the evaluation of the function "f".
## Fixed view, cannot be rotated.
	r=0:1/n:1; p=linspace(0,2*pi,8*n);
	{P,R}=field(p,r); X=R*cos(P); Y=R*sin(P);
	if isfunction(ffunction); Z=map(ffunction,X,Y;args());
	else; Z=expreval(ffunction,X,y=Y);
	endif;
	z1=totalmin(Z); z2=totalmax(Z); h=(z2+z1)/2; Z=Z-h;
	i=1:n+1;
	j=2*n:4*n+1;
	solid(X[i,j],Y[i,j],Z[i,j]); hold on;
	linewidth(3);
	wire([0,0],[0,0],[-1.5,1.5]);
	wire([-1.5,0],[0,0],[-h,-h]);
	wire([0,0],[0,1.5],[-h,-h]);
	linewidth(1);
	j=1:2*n+1;
	solid(X[i,j],Y[i,j],Z[i,j]);
	j=4*n:6*n+1;
	solid(X[i,j],Y[i,j],Z[i,j]);
	linewidth(3);
	wire([0,1.5],[0,0],[-h,-h]);
	wire([0,0],[-1.5,0],[-h,-h]);
	linewidth(1);
	j=6*n:8*n+1;
	solid(X[i,j],Y[i,j],Z[i,j]); hold off;
	return ""
endfunction
