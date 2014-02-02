comment
interval solvers
endcomment

.. *** Interval EULER polygon method ***

function idgl (fff,x,y0)
## Compute the solution of y'=fff(t,y0;...) at points t with
## y(t[1])=y0.
## The result is an interval vector of values.
## Additional arguments are passed to fff.
## fff may be an expression in x and y.
	n=length(x);
	y=~zeros(1,n)~; y[1]=y0;
	loop 1 to n-1;
		if isfunction(fff); m=fff(x[#],y[#],args());
		else m=expreval(fff,x[#],y=y[#]);
		endif;
		i=~x[#],x[#+1]~;
		d=diameter(i);
		repeat
			J=y[#]+m*~0,d~;
			if isfunction(fff) m=expand(fff(i,J,args()),6/5);
			else m=expand(expreval(fff,i,y=J),6/5);
			endif;
			y2=y[#]+m*d;
			if (y2 <<= J); break; endif;
		end;
		y[#+1]=y2;
	end;
	return y;
endfunction

.. *** Interval LGS solver ***

function ilgs (A,b)
## Solve A.x=b for x.
## Computes an interval inclusion of the solution.
## If the inclusion fails, you have to stop execution with the ESCAPE key.
## A and b may be of interval type.
## You may supply a pseudo inverse to A as an additional argument.
	n=cols(A);
	count=1;
	if argn()==2;
		A1=middle(A); R=inv(A1');
		repeat;
			M=residuum(A1',R,id(n));
			rho=max(sum(abs(M))');
			if rho<1; break; endif;
			R=R-A1'\M;
		count=count+1;
		if (count>10); error("Could not find a pseudo invers."); endif;
		end;
		R=R';
	else; R=arg3;
	endif;
	M=~-residuum(R,A,id(n))~;
	rho=right(max(sum(abs(M))'));
	if (rho>=1); error("Pseudo inverse not good enough."); endif;
	f=~-residuum(R,b,0)~;
	if argn()==2; x=middle(A)\middle(b); else; x=residuum(R,b,0); endif;
	xn=residuum(M,~x,x~,f);
	x=expand(x,max((right(max(abs(xn-x)'))/(1-rho))'));
	repeat;
		xn=residuum(M,x,f)&&x;
		if !(xn<<x); break; endif;
		x=xn;
	end;
	return x;
endfunction

function iinv (A)
## Tries to compute an interval inverse.
	return ilgs(A,id(cols(A)));
endfunction

...*** Interval polynomial evaluation ***

function ipolyval (p,t)
## Evaluates the polynomial p at points t,
## yields interval inclusions.
	s=t; si=size(t);
	A=id(cols(p));
	b=flipx(p)';
	loop 1 to prod(si);
		A=setdiag(A,-1,-t{#});
		v=ilgs(A,b,lusolve(A,id(cols(p))));
		s{#}=v[cols(p)];
	end;
	return s;
endfunction

.. *** Interval NEWTON method ***

function inewton (ffunc,fder,x)
## inewton("f","df",x;...)
## Computes an interval inclusion of the zero of ffunc.
## fder is an inclusion of the derivative of ffunc.
## The initial interval x must already contain a zero.
## If x is a point, the function tries to get an initial
## interval with the usual Newton method.
## Additional arguments are passed to ffunc and to fder.
## ffunc and fder may be expressions in x.
## Returns the solution and a flag, if the solution is verified.
	if !isinterval(x);
		x=expand(~newton(ffunc,fder,x;args())~,1000);
	endif
	valid=0;
	repeat
		m=middle(x);
		if isfunction(ffunc); a=ffunc(~m,m~,args());
		else a=expreval(ffunc,~m,m~);
		endif;
		if isfunction(fder); b=fder(x,args());
		else b=expreval(fder,x);
		endif;
		xnew=(m-a/b);
		if !valid && (xnew << x); valid=1; endif;
		xnew=xnew && x;
		if ! (xnew << x); return {x,valid}; endif;
		x=xnew;
	end;
endfunction

function inewton2 (fff,fff1,x)
## inewton2("f","Df",x;...)
## Works like newton2, starting from a interval vector x
## which already contains a solution.
## If x is no interval, the function tries to find such an interval.
## Additional arguments are passed to fff and fff1.
	if !isinterval(x); 
		x=expand(~newton2(fff,fff1,x;args())~,1000);
	endif;
	valid=0;
	y=middle(x)';
	R=inv(fff1(y',args()));
	yi=x';
	repeat
		y=middle(yi);
		M=~residuum(R,fff1(yi',args()),id(cols(x)))~;
		yn=(y-R.fff(~y,y~',args())'+M.(yi-y));
		if !valid && (yn<<yi); valid=1; endif;
		yn=yn&&yi;
		if !(yn<<yi); return {yn,valid}; endif;
		yi=yn;
	end;
endfunction
