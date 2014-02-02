comment
Steffenson iteration
endcomment

function steffenson (ggg,x0)
## Does n steps of the Steffenson operator, starting from x.
## Additional parameters are passed to ggg.
## The loop is continued to convergence.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	x=x0;
	repeat
		if isfunction(ggg);
			a=ggg(x,args(4)); b=ggg(a,args(4));
		else
			a=expreval(ggg,x); b=expreval(ggg,a);
		endif;
		c=b-2*a+x;
		if c~=0; return x; endif;
		xn=(x*b-a*a)/c;
		if xn~=x; return xn; endif;
		x=xn;
	end
endfunction

function nsteffenson (ggg,x0,n)
## Does n steps of the Steffenson operator, starting from x0.
## Additional parameters are passed to ggg. Returns the
## vector of iterated values.
## ggg may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	y=zeros(n,cols(x0));
	x=x0;
	loop 1 to n;
		if isfunction(ggg);
			a=ggg(x,args(4)); b=ggg(a,args(4));
		else
			a=expreval(ggg,x); b=expreval(ggg,a);
		endif;
		c=b-2*a+x;
		if c~=0; i=#; return y[1:i-1]; endif;
		x=(x*b-a*a)/c;
		y[#]=x;
	end
	return y;
endfunction


