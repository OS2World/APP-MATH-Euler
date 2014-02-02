comment
Bessel functions bessel(m,x).
endcomment

function bessel (m,x,n=30)
## bessel computes the m-th bessel function at x (|x|<10).
## m must be fixed and x can be a vector.
	denom=1/((1:n)*((1:n)+m));
	denom(1)=1/fak(m+1);
	denom=cumprod(denom);
	y=(-x^2/4);
	return (sum(cumprod(dup(y,n)').denom')'+1/fak(m))*(x/2)^m
endfunction

