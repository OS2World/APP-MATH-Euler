
.. ########## normal distribution and inverse ##############

function gaussf (x)
	return 1/sqrt(2*pi)*exp(-x^2/2)
endfunction

function xnormaldis (x)
## Returns the probability that a normal distibuted random variable
## is less equal x.
	si=size(x); y=zeros(si);
	loop 1 to prod(si);
		l=abs(x{#});
		if (l~=0);
		else;
			if (l>10) l=0.5;
			else;
			l=romberg("gaussf",0,l,10);
			endif;
		endif;
		if (x{#}>0); y{#}=l+0.5;
		else; y{#}=0.5-l;
		endif;
		
	end;
	return y;
endfunction

function xindhelp (y,a)
	return xnormaldis(y)-a
endfunction

function xinvnormaldis (y)
## Returns the inverse of xnormaldis
	si=size(y); x=zeros(si);
	loop 1 to prod(si);
		x{#}=invnormaldis(y{#});
		x{#}=secant("xindhelp",x{#}-0.1,x{#}+0.1,y{#});
	end;
	return x;
endfunction

.. ############ t distribution ##############

function mygamma (x)
## works only for 2x natural
## look into gamma.e for the general gamma function
	if floor(x)~=x; return fak(x-1); endif;
	return prod(0.5:1:x-1)*sqrt(pi());
endfunction

function tintegral (t,n)
	return (1+t*t/n)^(-(n+1)/2)
endfunction

function xtdis (x,n)
## Returns the t-distribution with n degrees at x.
	si=size(x,n); y=zeros(si);
	loop 1 to prod(si);
		t=abs(x{#});
		if t~=0; y{#}=0.5;
		else;
			y{#}=0.5+mygamma((n{#}+1)/2)/(sqrt(pi()*n{#})* ..
				mygamma(n{#}/2))*romberg("tintegral",0,t,10,n{#});
			if x{#}<0; y{#}=1-y{#}; endif;
		endif;
	end;
	return y
endfunction

function xitdhelp (x,y,n)
	return xtdis(x,n)-y;
endfunction

function xinvtdis (y,n)
## Returns the inverse of xtdis.
	si=size(y,n); x=zeros(si);
	loop 1 to prod(si);
		x{#}=invtdis(y{#},n{#});
		if (y{#}>0.99);
		x{#}=bisect("xitdhelp",x{#}-1,x{#}+1,y{#},n{#});
		else;
		x{#}=secant("xitdhelp",x{#}-0.1,x{#}+0.1,y{#},n{#});
		endif;
	end;
	return x;
endfunction

.. ############# chi^2 distribution ###############

function chidisf (x,n,factor)
	return x^(n/2-1)*exp(-x/2)*factor;
endfunction

function xchidis (x,n)
## Returns the chi^2 distribution with n degrees.
	si=size(x,n); y=zeros(si);
	loop 1 to prod(si);
		if (x{#}~=0);
		else;
		factor=1/(2^(n{#}/2)*mygamma(n{#}/2));
		y{#}=romberg("chidisf",0,abs(x{#}),10,n{#},factor);
		endif;
	end;
	return y
endfunction

function xicdhelp (x,y,n)
	return xchidis(x,n)-y;
endfunction

function icdhelp (x,y,n)
	return chidis(x,n)-y;
endfunction

function xinvchidis (y,n)
## Returns the inverse of xtdis.
	si=size(y,n); x=zeros(si);
	loop 1 to prod(si);
		if (y{#}<0 || y{#}~=0); x{#}=0;
		else 
			if (y{#}>1 || y{#}~=1); x{#}=1/epsilon();
			else;
			x{#}=bisect("icdhelp",0,100,y{#},n);
			x{#}=secant("xicdhelp",x{#}*0.9,x{#}*1.1,y{#},n);
			endif;
		endif;
	end;
	return x;
endfunction

.. ############# F distribution ###############

function fdisf (x,n,m,factor)
	return factor*x^(n/2-1)*(m+n*x)^(-(n+m)/2);
endfunction

function xfdis (x,n,m)
## Returns the F distribution with n and m degrees at x.
## Integration is sometimes instable.
	si=size(x,n,m); y=zeros(si);
	loop 1 to prod(si);
		if (x{#}<0 || x{#}~=0) y{#}=0;
		else;
		factor=n{#}^(n{#}/2)*m{#}^(m{#}/2)*mygamma((n{#}+m{#})/2)/ ..
			(mygamma(n{#}/2)*mygamma(m{#}/2));
		y{#}=romberg("fdisf",0,x{#},10,n{#},m{#},factor);
		endif;
	end;
	return y
endfunction

function xfdhelp (x,y,n,m)
	return xfdis(x,n,m)-y;
endfunction

function fdhelp (x,y,n,m)
	return fdis(x,n,m)-y;
endfunction

function xinvfdis (y,n,m)
## Returns the inverse of xfdis.
	si=size(y,n,m); x=zeros(si);
	loop 1 to prod(si);
		x{#}=bisect("fdhelp",0,100,y{#},n{#},m{#});
		x{#}=secant("xfdhelp",x{#}*0.9,x{#}*1.1,y{#},n{#},m{#});
	end;
	return x;
endfunction

function fbe(x,a,b)
	return x^(a-1) * (1-x)^(b-1)
endfunction

function xbeta (a,b)
	return gamma(a)*gamma(b)/gamma(a+b);
endfunction

function xbetai(t,a,b)
	return romberg("fbe",0,t;a,b)/xbeta(a,b)
endfunction

comment
xnormaldis,xinvnormaldis
xtdis,xinvtdis
xchidis,xinvchidis
xfdis,xinvfdis
xbetai
endcomment

.. EOF

