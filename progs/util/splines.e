comment
natural cubic and linear splines
endcomment

.. ### Natural spline ###

function spline (x,y)
## spline(x,y) defines the natural Spline at points x(i) with
## values y(i). It returns the second derivatives at these points.
	n=cols(x);
	h=x[2:n]-x[1:n-1];
	s=h[2:n-1]+h[1:n-2];
	l=h[2:n-1]/s;
	A=diag([n,n],0,2);
	A=setdiag(A,1,0|l);
	A=setdiag(A,-1,1-l|0);
	b=6/s*((y[3:n]-y[2:n-1])/h[2:n-1]-(y[2:n-1]-y[1:n-2])/h[1:n-2]);
	return (A\(0|b|0)')';
endfunction

function splineval (x,y,h,t)
## splineval(x,y,h,t) evaluates the cubic interpolation spline for
## (x(i),y(i)) with second derivatives h(i) at t.
	p1=find(x,t); p2=p1+1;
	y1=y[p1]; y2=y[p2];
	x1=x[p1]; x2=x[p2]; f=(x2-x1)^2;
	h1=h[p1]*f; h2=h[p2]*f;
	b=h1/2; c=(h2-h1)/6;
	a=y2-y1-b-c;
	d=(t-x1)/(x2-x1);
	return y1+d*(a+d*(b+c*d));
endfunction

function linsplineval (x,y,t)
## sval(x,y,t) evaluates the linear interpolating spline for
## (x(i),y(i)) at t.
	p1=find(x,t); p2=p1+1;
	y1=y[p1]; y2=y[p2];
	x1=x[p1]; x2=x[p2];
	return y1+(t-x1)*(y2-y1)/(x2-x1);
endfunction

