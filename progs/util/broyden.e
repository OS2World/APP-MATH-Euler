comment
Broyden algorithm
endcomment

.. ### Broyden ###

function broyden (ffunction,xstart,A=0)
## broyden("f",x) or broyden("f",x,A;...) finds a zero of f.
## x is the starting value and A is a approximation of the jacobian.
## ... is passed to f(x,...)
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	x=xstart; n=cols(x);
	delta=sqrt(epsilon());
	if A==0;
		A=zeros([n n]);
		y=ffunction(x,args());
		loop 1 to n;
			x0=x; x0[#]=x0[#]+delta;
			x1=x; x1[#]=x1[#]-delta;
			A[:,#]=(ffunction(x0,args())-ffunction(x1,args()))'/(2*delta);
		end;
	endif;
	y=ffunction(x,args());
	repeat
		d=-A\y'; x=x+d';
		y1=ffunction(x,args()); q=y1-y;
		A=A+((q'-A.d).d')/(d'.d);
		if d~=0; break; endif;
		y=y1;
	end;
	return x;
endfunction

function nbroyden (ffunction,xstart,nmax,A=0)
## broyden("f",x,n) or broyden("f",x,n,A;...) does n steps of the
## Broyden algorithm.
## x is the starting value and a is a approximation of the jacobian.
## if A is 0, it is neglected.
## ... is passed to f(x,...)
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	x=xstart; n=cols(x); r=x;
	delta=epsilon();
	if A==0;
		A=zeros([n n]);
		y=ffunction(x,args());
		loop 1 to n;
			x0=x; x0[#]=x0[#]+delta;
			A[:,#]=(ffunction(x0,args())-y)'/delta;
		end;
	endif;
	y=ffunction(x,args());
	count=0;
	repeat
		count=count+1;
		d=-A\y'; xn=x+d';
		if xn~=x || count>nmax; r=r_xn; break; endif;
		x=xn;
		y1=ffunction(x,args()); q=y1-y;
		A=A+((q'-A.d).d')/(d'.d);
		y=y1;
		r=r_xn;
	end;
	return r;
endfunction


