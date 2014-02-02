comment
 gcd(a,b) returns the greatest common divisor of a and b
 lcm(a,b) returns least common multiple of a and b
 ggt(x,y) returns {g,a,b} such that ax+by=g.
 invmod(x,m) returns y such that x*y is 1 modulo m.
 primes(n) returns all primes up to n using the Sieve of Erastothenes.
 isprime(n) returns true if n is a prime number, false otherwise.
 factor(n) returns prime factorization of n.  If n==1, returns 1.
endcomment


function gcd1(a,b)
## gcd(a,b) returns the greatest common divisor of a and b
## See also: primes, isprime, factor, lcm, mod, invmod, ggt.
	if a>b;
		u=a;v=b;
	else
		u=b;v=a;
	endif;
	repeat;
		if v==0; break; endif;
		r=mod(u,v);
		u=v;
		v=r;
	end;
	return u;
endfunction

function gcd(a,b)
	return map("gcd1",a,b)
endfunction

function lcm(a,b)
## lcm(a,b) returns least common multiple of a and b
## See also: primes, isprime, factor, gcd, mod, invmod, ggt.
	return a*b/gcd(a,b);
endfunction;

function ggt (x,y)
## return {g,a,b} such that ax+by=g.
## See also: primes, isprime, factor, gcd, lcm, mod, invmod.
	if x<y; {g,a,b}=ggt(y,x); return {g,b,a}; endif;
	if y~=1; return {1,0,1}; endif;
	m=mod(x,y);
	if m~=0; return {y,0,1}; endif;
	{g,aa,bb}=ggt(y,m);
	return {g,bb,aa-bb*floor(x/y)};
endfunction

function invmod1 (x,m)
## Return y such that x*y is 1 modulo m.
## See also: primes, isprime, factor, gcd, lcm, mod, ggt.
	{g,a,b}=ggt(x,m);
	if !(g~=1); error("no inverse!"); endif;
	if a<0; return m+a; endif;
	return a;
endfunction

function invmod (x,m)
	return map("invmod1",x,m);
endfunction

function primes(n=100)
## Return all primes up to n using the Sieve of Erastothenes.
## See also: isprime, factor, gcd, lcm, mod, invmod, ggt.
	if n>=3
		len = floor((n-1)/2);		.. length of the sieve
		sieve = ones ([1,len]);		.. assume every odd number is prime
		for i=1 to (sqrt(n)-1)/2	.. check up to sqrt(n)
			if (sieve[i])			.. if i is prime, eliminate multiples of i
				sieve[3*i+1:2*i+1:len] = 0; .. do it
			endif
		end
		return [2, 1+2*nonzeros(sieve)];     # primes remaining after sieve
	elseif n>=2
		return 2;
	else
		return [];
	endif
endfunction


function _isprime(n)
	if (n != ceil(n) || n < 2)
		return 0;
	elseif n < 4
		return 1;
	else
		q = n/[2:sqrt(n)];
		return !any(q == ceil(q));
	endif
endfunction

function isprime(n)
## isprime(n) returns true if n is a prime number, false otherwise.
## See also: primes, factor, gcd, lcm, mod, invmod, ggt.
	return map("_isprime",n);
endfunction


function factor(n)
## factor(n) returns prime factorization of n.  If n==1, returns 1.
## See also: primes, isprime, gcd, lcm, mod, invmod, ggt.
	if n < 4
		return n;
	else
		p = primes(sqrt(n));
		q = n/p;
		idx = nonzeros (q == ceil(q));
		if cols(idx)==0
			return n;
		else
			r = p(idx);
			q = factor(n/prod(p(idx)));
			if q != 1
				return sort([r, q]);
			endif
		endif
	endif
endfunction
