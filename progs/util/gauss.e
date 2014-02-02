comment
Gauss integration
endcomment

function legendre (n)
	p0=1; p1=[0,1];
	loop 2 to n;
		p=((0|(2*#-1)*p1)-((#-1)*(p0|0|0)))/#;
		p0=p1; p1=p;
	end;
	return p1
endfunction

function gaussparam (n)
## returns the knots and alphas of gauss integration at n points in
## [-1,1]. Returns {gaussz,gaussa}.
	p=legendre(n);
	z=sort(re(polysolve(p)));
	Z=dup(z,n)^dup((0:n-1)',n);
	a=Z\(2/(1:n)'*mod(1:n,2)');
	return {z,a'}
endfunction

gaussz = [ ..
-9.7390652851717172e-0001,
-8.6506336668898451e-0001,
-6.7940956829902441e-0001,
-4.3339539412924719e-0001,
-1.4887433898163121e-0001,
 1.4887433898163121e-0001,
 4.3339539412924719e-0001,
 6.7940956829902440e-0001,
 8.6506336668898451e-0001,
 9.7390652851717172e-0001];
gaussa = [ ..
 6.6671344308688139e-0002,
 1.4945134915058059e-0001,
 2.1908636251598205e-0001,
 2.6926671930999635e-0001,
 2.9552422471475288e-0001,
 2.9552422471475287e-0001,
 2.6926671930999635e-0001,
 2.1908636251598205e-0001,
 1.4945134915058059e-0001,
 6.6671344308688137e-0002];

function gauss10 (ffunction,a,b)
## gauss10("f",a,b,...)
## evaluates the gauss integration with 10 knots an [a,b]
## ffunction may be an expression in x.
	global gaussa,gaussz;
	z=a+(gaussz+1)*(b-a)/2;
	if isfunction(ffunction)
		return sum(gaussa*ffunction(z,args()))*(b-a)/2;
	else
		return sum(gaussa*expreval(ffunction,z))*(b-a)/2;
	endif;
endfunction

function gauss (ffunction,a,b,n=1)
## gauss("f",a,b) gauss integration with 10 knots
## gauss("f",a,b,n,...) subdivision into n subintervals.
## a and b may be vectors.
## ffunction may be an expression in x.
	si=size(a,b); R=zeros(si);
	if n==1;
		loop 1 to prod(si);
			sum=0;
			sum=sum+gauss10(ffunction,a{#},b{#};args());
			R{#}=sum;
		end;
	else;
		loop 1 to prod(si);
			h=linspace(a{#},b{#},n); sum=0;
			loop 1 to n;
				sum=sum+gauss10(ffunction,h{#},h{#+1};args());
			end;
			R{#}=sum;
		end;
	endif;
	return R
endfunction

