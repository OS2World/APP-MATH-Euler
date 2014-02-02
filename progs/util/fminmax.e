comment
minimization
endcomment

function fmin (fff,a,b)
## Compute the Minimum of the convex function fff in [a,b],
## using the golden cut method.
## Additional parameters are passed to fff.
## fff may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	x0=a; x3=b;
	if isfunction(fff)
		y0=fff(x0,args()); y3=fff(x3,args());
	else
		y0=expreval(fff,x0); y3=expreval(fff,x3);
	endif
	l=(3-sqrt(5))/2;
	x1=x0+l*(x3-x0); x2=x3-l*(x3-x0);
	if isfunction(fff)
		y1=fff(x1,args()); y2=fff(x2,args());
	else
		y1=expreval(fff,x1); y2=expreval(fff,x2);
	endif
	repeat
		if y1>y2;
			x0=x1; x1=x2; x2=x3-l*(x3-x0);
			y0=y1; y1=y2;
			if isfunction(fff); y2=fff(x2,args());
			else y2=expreval(fff,x2);
			endif;
		else
			x3=x2; x2=x1; x1=x0+l*(x3-x0);
			y3=y2; y2=y1;
			if isfunction(fff); y1=fff(x1,args());
			else y1=expreval(fff,x1);
			endif;
		endif;
		if x0~=x3; return x0; endif;
	end;
endfunction

function fmax (fff,a,b)
## Compute the Maximum of the concave function fff in [a,b],
## using the golden cut method.
## Additional parameters are passed to fff.
## fff may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	x0=a; x3=b;
	if isfunction(fff)
		y0=fff(x0,args()); y3=fff(x3,args());
	else
		y0=expreval(fff,x0); y3=expreval(fff,x3);
	endif
	l=(3-sqrt(5))/2;
	x1=x0+l*(x3-x0); x2=x3-l*(x3-x0);
	if isfunction(fff)
		y1=fff(x1,args()); y2=fff(x2,args());
	else
		y1=expreval(fff,x1); y2=expreval(fff,x2);
	endif
	repeat
		if y1<y2;
			x0=x1; x1=x2; x2=x3-l*(x3-x0);
			y0=y1; y1=y2;
			if isfunction(fff); y2=fff(x2,args());
			else y2=expreval(fff,x2);
			endif;
		else
			x3=x2; x2=x1; x1=x0+l*(x3-x0);
			y3=y2; y2=y1;
			if isfunction(fff); y1=fff(x1,args());
			else y1=expreval(fff,x1);
			endif;
		endif;
		if x0~=x3; return x0; endif;
	end;
endfunction

function fextrema (fff,a,b,n=100)
## Find all internal extrema of fff in [a,b].
## fff may be an expression in x or a function.
## You can specify an epsilon eps with eps=... as last parameter.
## Returns {minima,maxima} (maybe of lengths 0)
	if (isvar("eps")); localepsilon(eps); endif;
	x=linspace(a,b,n);
	mi=[]; ma=[];
	if isfunction(fff);
		y=fff(x;args());
	else
		y=expreval(fff,x);
	endif;
	loop 2 to n;
		if y[#]>=y[#-1] && y[#]>y[#+1];
			ma=ma|fmax(fff,x[#-1],x[#+1];args());
		elseif y[#]<=y[#-1] && y[#]<y[#+1]
			mi=mi|fmin(fff,x[#-1],x[#+1];args());
		endif;
	end;
	return {mi,ma}
endfunction

function fffexprv (x,expr)
	return evaluate(expr);
endfunction

function brentmin (fff,a,d=0.1,eps=epsilon())
## Compute the minimum of fff around d with Brent's method.
## d is an initial step size to look for a starting interval.
## eps is the final accuracy.
## Returns the point of minimal value.
## fff may be an expression in x.
## Return the point of minimum.
	if isfunction(fff);
		return brent(fff,a,d,eps;args());
	else;
		return brent("fffexprv",a,d,eps,fff);
	endif;
endfunction

function neldermin (fff,v,d=0.1,eps=epsilon())
## Compute the minimum of fff around d with Nelder-Means's method.
## d is an initial step size for a starting simplex.
## eps is the final accuracy.
## Returns the point of minimal value.
## x is a 1xn vector.
## fff may be an expression in x (n>2).
## Return the point of minimum (1xn vector).
	if isfunction(fff);
		return nelder(fff,v,d,eps,args());
	else;
		return nelder("fffexprv",v,d,eps,fff);
	endif;
endfunction


