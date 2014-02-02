comment
contfrac, contfracval, contfracbest defined
endcomment

function contfrac (x,n=10)
## Compute the continued fraction of x.
## returns [a,b,c,...] with
## x = a + 1 / (b + 1 / (c + 1/(...
	s=x;
	r=floor(s);
	loop 1 to n
		s=1/(s-floor(s));
		r=r|floor(s);
	end
	return r;
endfunction

function contfracval (r)
## Evaluate the continued fraction
## x = a + 1 / (b + 1 / (c + 1/(...
## with r = [a,b,c,...]
## Return an Interval {x1,x2}
	n=cols(r);
	x1=r[n]; x2=r[n]+1;
	loop 1 to n-1
		 x1=1/x1+r[n-#];
		 x2=1/x2+r[n-#];
	end;
	return {x1,x2};
endfunction

function contfracbest (x,n=3)
## Return the best rational
## approximation to x
	{x1,x2}=contfracval(contfrac(x,n));
	x,
	if (abs(x-x1)<abs(x-x2)); return x1;
	else; return x2;
	endif
endfunction
