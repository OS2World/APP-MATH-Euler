comment
Tschebycheff Polynome
T(n,x); Computes T_n(x)
Trek(n,x); a recursiv version (for test purposes)
Tpoly(n); Computes the Chebyshev polynomials T_n
endcomment

function T(n,x)
## T(n,x) computes T_n(x) the trigonometric way.
	s=size(x,n);
	y=x;
	signum=-mod(n,2)*2+1;
	loop 1 to s(2);
		z=x{#};
		if z>1;
			w=(z+sqrt(z^2-1))^n{#};
			y{#}=(w+1/w)/2;
		else;
			if z<-1;
				w=(-z+sqrt(z^2-1))^n{#};
				y{#}=signum{#}*(w+1/w)/2;
			else;
				y{#}=cos(n{#}*acos(z));
			endif;
		endif;
	end;
	return y;
endfunction

function Trek(n,x)
## Computes the Chebyshev polynomial via the recursion formula.
	if (n==0); return ones(size(x)); endif;
	if (n==1); return x; endif;
	z=ones(size(x))_x;
	loop 3 to n+1;
		z=z_(2*x*z[#-1]-z[#-2]);
	end;
	return z[n+1];
endfunction

function Tpoly (n)
## Computes the coefficients of the n-th chebyshev polynomial
## recursively.
	t0=[1]; t1=[0,1];
	if (n==0); return t0; endif;
	if (n==1); return t1; endif;
	loop 2 to n
		t=(0|2*t1[1:#])-(t0|[0,0]);
		t0=t1; t1=t;
	end;
	return t;
endfunction
