function trigcoeff (s)
## determine the trigonometric polynomial interpolating
## in equidistant values s.
## returns a_0,...a_(n/2+1) and b_1,...b_(n/2)
## s(t)= a_0/2 + sum (a_nu cos nu t + b_nu sin nu t) +
##       a_(n/2+1) cos( (n/2+1) t)
	c=ifft(s); n=cols(s)/2;
	return {re(c[1]),re(2*c[2:n]),im(-2*c[2:n]),re(c[n+1])};
endfunction

function trigeval (a0,a,b,an)
	v=(a+1i*b)/2;
	return re(fft(a0|conj(v)|an|flipx(v)));
endfunction

function rho(t)
## radial function of the starlike region
	return 1/sqrt(cos(t)^2/1.25^2+sin(t)^2/0.75^2)
endfunction

function theo (tau,theta)
	h=log(rho(theta));
	{a0,a,b,an}=trigcoeff(h);
	return tau+trigeval(0,b,-a,-an);
endfunction

function it (tau)
	theta=tau;
	repeat
		thetanew=theo(tau,theta);
		if (thetanew~=theta); break; endif;
		theta=thetanew;
	end
	return thetanew
endfunction

function test(N)
	t=linspace(0,2*pi,N+1); t=t[1:N];
	s=it(t,t);
	xplot(t,s);
	u=arg(exp(1i*t)+1/(4*exp(1i*t)));
	u=u+(u<0)*2*pi;
	xplot(u-s);
	return {t,s};
endfunction


