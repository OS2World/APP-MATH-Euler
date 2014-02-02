comment
numerical methods
endcomment

function bisect (ffunction,a,b)
## bisect("f",a,b;...) uses the bisection method to find a root of
## f(x,...) in [a,b]. ffunction may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	if isfunction(ffunction);
## function
	if ffunction(b,args())<0;
		b1=a; a1=b;
		if ffunction(a,args())<0 error("No zero in interval"); endif;
	else;
		a1=a; b1=b;
		if ffunction(a,args())>0 error("No zero in interval"); endif;
	endif;
	repeat
		m=(a1+b1)/2;
		if a1~=b1; break; endif;
		if ffunction(m,args())>0; b1=m; else a1=m; endif;
	end;
	else
## expression
	if expreval(ffunction,b)<0;
		b1=a; a1=b;
		if expreval(ffunction,a)<0 error("No zero in interval"); endif;
	else;
		a1=a; b1=b;
		if expreval(ffunction,a)>0 error("No zero in interval"); endif;
	endif;
	repeat
		m=(a1+b1)/2;
		if a1~=b1; break, endif;
		if expreval(ffunction,m)>0; b1=m; else a1=m; endif;
	end;
	endif;
	return m;
endfunction

function secant (ffunction,a,b)
## secant("f",a,b;...) uses the secant method to find
## a root of f(x,...) in [a,b]
## ffunction may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	if isfunction(ffunction)
## function
	x0=a; x1=b; 
	y0=ffunction(x0,args()); y1=ffunction(x1,args());
	repeat
		x2=x1-y1*(x1-x0)/(y1-y0);
		if x2~=x1; break; endif;
		x0=x1; y0=y1; x1=x2; y1=ffunction(x2,args());
	end;
	else
## expression
	x0=a; x1=b; 
	y0=expreval(ffunction,x0); y1=expreval(ffunction,x1);
	repeat
		x2=x1-y1*(x1-x0)/(y1-y0);
		if x2~=x1; break; endif;
		x0=x1; y0=y1; x1=x2; y1=expreval(ffunction,x2);
	end;
	endif;
	return x2
endfunction

function simpson (ffunction,a,b,n=50)
## simpson("f",a,b) or simpson("f",a,b,n;...) integrates 
## f(x,...) in [a,b] using the Simpson method.
## f must be able to evaluate a vector.
## ffunction may be an expression in x.
	t=linspace(a,b,2*n);
	if isfunction(ffunction);
		s=ffunction(t,args());
	else
		s=expreval(ffunction,t);
	endif;
	ff=4-mod(1:2*n+1,2)*2; ff[1]=1; ff[2*n+1]=1;
	return sum(ff*s)/3*(t[2]-t[1]);
endfunction

function romberg(ffunction,a,b,m=10)
## romberg(f,a,b) computes the Romberg integral of f(x,...) in [a,b].
## romberg(f,a,b,m;...) specifies h=(b-a)/m/2^k for k=1,...
## ffunction may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	if isfunction(ffunction)
		y=ffunction(linspace(a,b,m),args()); h=(b-a)/m;
	else
		y=expreval(ffunction,linspace(a,b,m)); h=(b-a)/m;
	endif
	y[1]=y[1]/2; y[m+1]=y[m+1]/2; I=sum(y);
	S=I*h; H=h^2; Intalt=S;
	repeat;
		if isfunction(ffunction)
			I=I+sum(ffunction(a+h/2:h:b,args())); h=h/2;
		else
			I=I+sum(expreval(ffunction,a+h/2:h:b)); h=h/2;
		endif;
		S=S|I*h;
		H=H|h^2;
		Int=interpval(H,interp(H,S),0);
		if Int~=Intalt; break; endif;
		Intalt=Int;
	end;
	return Intalt
endfunction

function iterate (ffunction,x0)
## iterate("f",x0;...) iterates the function f(x,...), starting from
## x0.
## The iteration stops at a fixed point.
## Returns the fixed point.
## ffunction may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	x=x0;
	if isinterval(x);
		repeat
			if isfunction(ffunction); x=ffunction(x,args());
			else; x=expreval(ffunction,x);
			endif;
			if left(x)~=right(x); return x; endif;
		end;
	else;
		repeat
			if isfunction(ffunction); xn=ffunction(x,args());
			else; xn=expreval(ffunction,x);
			endif;
			if (x~=xn); return xn; endif;
			x=xn;
		end;
	endif;
endfunction

function niterate (ffunction,x0,n)
## iterate("f"x0,n;...) Iterate the function f(x,...) n times,
## starting with the point x0.
## Returns the vector of iterants.
## ffunction may be an expression in x.
	x=x0; y=zeros(n,cols(x));
	loop 1 to n
		if isfunction(ffunction); x=ffunction(x,args());
		else x=expreval(ffunction,x);
		endif;
		y[#,:]=x;
	end;
	return y;
endfunction

function newton (ffunc,fder,x)
## newton("f","df",x;...) seeks a zero of f(x,...).
## Starts the Newton iteration from x.
## df is the derivative of f.
## Additional parameters are passed to ffunc and fder.
## ffunc and fder may be expressions in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	repeat
		if isfunction(ffunc); a=ffunc(x,args());
		else a=expreval(ffunc,x);
		endif;
		if isfunction(fder); b=fder(x,args());
		else b=expreval(fder,x);
		endif;
		xnew=x-a/b;
		if (xnew~=x) return xnew; endif;
		x=xnew;
	end;
endfunction

function newton2 (fff,fff1,x)
## newton2("f","Df",x;...)
## Newton method for several parameter.
## fff is the function, fff1 computes the Jacobian.
## Additional parameters are passed to fff and fff1.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	repeat
		d=(fff1(x,args())\fff(x,args())')';
		xn=x-d;
		if xn~=x; return xn; endif;
		x=xn;
	end
endfunction

function evalfff (fff)
	useglobal;
	return evaluate(fff);
endfunction

function root (fff, x)
## Find the root of an expression fff (of string type)
## by changing the variable x. Other variables may be
## set globally. Note that the actual name of the variable
## x may be different from "x".
	if (isvar("eps")); localepsilon(eps); endif;
	if (x==0) x1=0.00001;
	else x1=1.00001*x;
	endif;
	x0=x;
	a=evalfff(fff);
	repeat
		x=x1; b=evalfff(fff);
		xn=(x0*b-x1*a)/(b-a);
		if xn~=x; break; endif;
		x0=x1; x1=xn; a=b;
	end;
	return x;
endfunction

function setupdif (n)
	A=zeros([n,2*n+1]);
	loop 1 to n;
	b=zeros([2*#+1,1]); b[#+1]=fak(#);
	a=((-#:#)^(0:2*#)')\b;
	A[#,1:2*#+1]=a';
	end;
	return A;
endfunction

DifMatrix=setupdif(5);

function dif (fff, x, n=1, e=epsilon()^(1/2))
## Compute the n-th derivative of fff in the points x.
## fff may be an expression in x or a function.
## For n>=2, x must be a 1xn vector.
	global DifMatrix;
	eps=e^(1/(n+1));
	if n==0;
		if isfunction(fff); fff(x); else return expreval(fff,x); endif;
	elseif (n==1)
		if isfunction(fff);
			return (fff(x+eps)-fff(x-eps))/(2*eps);
		else
			return (expreval(fff,x+eps)-expreval(fff,x-eps))/(2*eps);
		endif;
	elseif n<=5;
		h=(-n:n)*eps; xh=h'+x; a=DifMatrix[n,0:2*n+1];
		if isfunction(fff); return (a.fff(xh))/eps^n;
		else return (a.expreval(fff,xh))/eps^n;
		endif;
	else
		error("Too high a derivative!");
	endif;
endfunction

