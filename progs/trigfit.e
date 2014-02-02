.. These functions handle triginometric interpolation and best fit.

function trigeval (a,x)
## Evaluates the trigonometric polynomial at x.
## a are the cos and then the sin coefficients.
	m=(cols(a)-1)/2;
	n=cols(a);
	A=dup(0:m,cols(x)); B=dup(x,m+1)'; F1=cos(A*B);
	A=dup(1:m,cols(x)); B=dup(x,m)'; F2=sin(A*B);
	return ((F1|F2).a')';
endfunction

function triginterpol (x,y)
## Interpolates a trigonometric polynomial to x and y.
## Returns the coefficients. First the cos and then the sin
## coeffients. Returns a row vector.
	m=(cols(x)-1)/2;
	n=cols(x);
	A=dup(0:m,n); B=dup(x,m+1)'; F1=cos(A*B);
	A=dup(1:m,n); B=dup(x,m)'; F2=sin(A*B);
	return ((F1|F2)\y')';
endfunction

function trigfit (x,y,deg)
## Fits a triginometric polynomial of degree deg to (x,y).
## x must be ordered!
	T=x/sqrt(sum(x^2));
	loop 1 to deg;
		t=cos(#*x);
		t=t-(T.t')'.T;
		t=t/sqrt(sum(t^2));
		T=T_t;
		t=sin(#*x);
		t=t-t.T'.T;
		t=t/sqrt(sum(t^2));
		T=T_t;
	end
	d=y.T'.T;
	i=1:cols(x); j=i[linspace(1,cols(x),2*deg)];
	return triginterpol(x[j],d[j]);
endfunction


