comment
Shows vectorfields and solutions of DGL.
E.g. dgltest("x*sin(y)",-pi,pi,-pi,pi);
endcomment

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

function dgltest (expr,x1,x2,y1,y2,nx=20,ny=20,n=10)
## Draw a vector field and let the user click to a starting point.
## expr must be an expression in x and y.
## x1,x2,y1,y2 are the rectangle, which will be drawn.
## nx,ny is the number of discretization points for the drawing.
## n is the number of discretization (times nx) for the DGL.
	vectorfield(expr,x1,x2,y1,y2,nx,ny);
	title("Click for a solution (>END<)");
	repeat
		p=mouse();
		if (p[2]>y2); break; endif;
		hold on;
		linewidth(2); 
		t=p[1]:(x2-x1)/(nx*n):x2;
		plot(t,heun(expr,t,p[2]));
		t=p[1]:(x1-x2)/(nx*n):x1;
		plot(t,heun(expr,t,p[2]));
		linewidth(1);
		hold off;
	end;
	return plot();
endfunction

function vectorfield2 (expr1,expr2,x1,x2,y1,y2,nx=20,ny=20)
## Draw the vector field of a differential equation in x and y.
## expr must be the expression "f(x,y)", which computes the
## derivative of y(x) at x.
	setplot(x1,x2,y1,y2);
	x=linspace(x1,x2,nx-1);
	y=linspace(y1,y2,ny-1);
	{X,Y}=field(x,y); V1=zeros(size(X)); V2=zeros(size(X));
	loop 1 to prod(size(X));
		V1{#}=expreval(expr1,X{#},y=Y{#});
		V2{#}=expreval(expr2,X{#},y=Y{#});
	end;
	n=prod(size(X));
	h=(x2-x1)/nx/4;
	v1=redim(V1,[n,1]);
	v2=redim(V2,[n,1]);
	x=redim(X,[n,1]); x=(x-h*v1)|(x+h*v1);
	y=redim(Y,[n,1]); y=(y-h*v2)|(y+h*v2);
	st=linestyle("->"); xplot(x,y); linestyle(st);
	return plot();
endfunction

dgltest("x*sin(y)",-pi,pi,-pi,pi);

function fff2test (X,Y,expr1,expr2)
	return [expreval(expr1,Y[1],y=Y[2]),expreval(expr2,Y[1],y=Y[2])]
endfunction

function dgl2test (expr1,expr2,x1,x2,y1,y2,nx=20,ny=20,t=0:0.01:5)
## Draw a vector field and let the user click to a starting point.
## x1,x2,y1,y2 are the rectangle, which will be drawn.
## expr1 is the x-value of the derivative and expr2 its y-value.
## Both expressions must be expressions in x and y.
## E.g., v'=f(v), with f([x,y])=[expr1,expr2].
## nx,ny is the number of discretization points for the drawing.
## t determines, where the ODE will be evaluated.
	vectorfield2(expr1,expr2,x1,x2,y1,y2,nx,ny);
	title("Click for a solution (>END<)");
	repeat
		p=mouse();
		if (p[2]>y2); break; endif;
		hold on;
		linewidth(2);
		y=heun("fff2test",t,p;expr1,expr2);
		plot(y[1],y[2]);
		linewidth(1);
		hold off;
	end;
	return plot();
endfunction

dgl2test("y","-x",-1,1,-1,1);

