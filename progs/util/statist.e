comment
statistical functions
endcomment

function meandev (x,v=0)
## Return the mean value and the standard deviation of x1,...,xn.
## E.g. {m,d}=mean([1,2,3,3,4,2,5]).
## An additional parameter v may contain the multiplicities of x.
## E.g. {m,d}=mean([1,2,3,4,5],[1,2,2,1,1]) (same example).
## m=mean(x) will assign the mean value only!
	if (argn()==2)
		n=sum(v);
		m=sum(x*v)/n;
		d=sqrt((sum(x*x*v)-n*m*m)/(n-1));
	else
		n=cols(x);
		m=sum(x)/n;
		d=sqrt((sum(x*x)-n*m*m)/(n-1));
	endif;
	return {m,d};
endfunction

function mean (x)
## Returns the mean value of x.
## An additional parameter may contain multiplicities.
## See: meandev.
	{m,d}=meandev(x,args()); return m;
endfunction

function dev (x)
## Returns the standard deviation of x.
## An additional parameter may contain multiplicities.
## See: meandev.
	{m,d}=meandev(x,args()); return d;
endfunction

function qnormal (x,m,d)
## Returns the normal density with mean m and deviation d.
	return 1/(d*sqrt(2*pi))*exp(-(x-m)^2/2/d^2);
endfunction

function xplotrange (r,v)
## Plots a barplot of the multiplicities v[i] in the ranges
## r[i],r[i+1]. r must be one longer than v.
## You can set the plot area with setplot beforehand.
	s=scaling(1); 
	if s;	
		setplot(min(r),max(r),0,max(v));
	endif;
	scaling(s);
	n=cols(v);
	xplotbar(r[1:n],0,r[2:n+1]-r[1:n],v);
	return plot();
endfunction

function chitest (x,y)
## Perform a xhi^2 test, if x obeys the distribution y.
## Return the error, if you reject this.
	return 1-chidis(sum((x-y)^2/y),cols(x)-1);
endfunction

function testnormal (r,n,v,m,d)
## Test the data v[i] in the ranges r[i],r[i+1] against
## the normal deviation with mean m and deviation d,
## using the xhi^2 method.
## n is the total number of data (including those
## less than r[1] and larger than r[n]).
## Return the error you have, if you reject normal distribution.
	t=normaldis((r-m)/d);
	k=cols(v); p=(t[2:k+1]-t[1:k])*n;
	return chitest(v,p);
endfunction

function ttest (m,d,n,mu)
## Test, if the measured mean m with deviation d of n data
## can come from a distribution with mean value mu.
## Returns the error you have, if you reject this.
	return 1-tdis(abs(m-mu)/d*sqrt(n),n-1);
endfunction

function tcompare (m1,d1,n1,m2,d2,n2)
## Test, if two measured data with means mi, deviation di
## of ni data (i=1,2), aggree in mean.
## The data must be normally distributed.
## Returns the error you have, if you reject this.
	h1=d1^2/n1; h2=d2^2/n2;
	return 1-tdis(abs(m1-m2)/sqrt(h1+h2), ..
		(h1+h2)^2/(h1^2/(n1+1)+h2^2/(n2+1))-2);
endfunction

function tcomparedata (x,y)
## Compare x and y on same mean, where x and y are
## normally distributed with different deviation.
## Return the error you have, if you reject this.
	return tcompare(mean(x),dev(x),cols(x),mean(y),dev(y),cols(y));
endfunction

function tabletest (A)
## Test the results a[i,j] for independence of the rows
## from the columns. (chi^2 test)
## This should only be used for large table entries.
## Return the error you have, if you reject independence.
	c=sum(A); r=sum(A')';
	E=c*r/sum(r);
	return 1-chidis(totalsum((A-E)^2/E),(cols(A)-1)*(rows(A)-1));
endfunction

function varanalysis
## Test the normally distributed data x1,x2,x3,... for same
## mean value.
## varanalysis(x1,x2,x3,...), where each xi is a row vector.
## Returns the error you have, if you reject same mean.
	md=[mean(arg1),cols(arg1)];
	s=sum((arg1-md[1])^2);
	loop 2 to argn();
		x=args(#); mx=mean(x);
		md=md_[mx,cols(x)];
		s=s+sum((x-mx)^2);
	end;
	md=md'; n=sum(md[2]); mt=sum(md[1]*md[2])/n;
	s1=sum(md[2]*(md[1]-mt)^2);
	return 1-fdis((s1/(argn()-1))/(s/(n-argn())),argn()-1,n-argn());
endfunction

function binsum1 (i,n,p)
	if i>=n; return 1; endif;
	d=sqrt(n*p*(1-p));
	i1=n*p-10*d;
	if i1<0; i1=0; endif;
	if i<i1; return 0; endif;
	if i>n*p+10*d; return 1; endif;
	s=n*log(1-p);
	if i>=1;
		j=1:i+1;
		s=cumsum(s|(log((n-j+1)/j*p/(1-p))));
	endif;
	return sum(exp(s[floor(i1):i+1]));
endfunction

function binsum (i,n,p)
## Compute the probablitiy of getting i or less hits
## in n runs, where the probability for each hit is p.
## Works even for large i and n, but is ineffective then.
## One may use the normalsum function for large n and
## medium p.
	return map("binsum1",i,n,p);
endfunction

function normalsum (i,n,p)
## Works like binsum, but is much faster for large n
## and medium p.
	return normaldis((i+0.5-n*p)/sqrt(n*p*(1-p)));
endfunction

function invbinsum1 (x,n,p)
	if x<=0; return 0; endif;
	if x>=1; return n; endif;
	s=n*log(1-p);
	d=sqrt(n*p*(1-p));
	i1=floor(n*p-10*d); i2=floor(n*p+10*d);
	if i1<0; i1=0; endif;
	if i2>n; i2=n; endif;
	j=1:i2;
	s=cumsum(s|(log((n-j+1)/j*p/(1-p))));
	return find(cumsum(exp(s[i1:length(s)])),x)+i1;
endfunction

function invbinsum (x,n,p)
## Computes i, such that binomialsum(i,n,p) is just larger than x.
	return map("invbinsum1",x,n,p)
endfunction

function hypergeomsum1 (i,n,i1,n1)
	if i<i1-(n1-n); return 0; endif;
	if i>=i1; return 1; endif;
	if i1<=n1-n;
		s=logbin(n1-n,i1)-logbin(n1,i1);
		if i>=1;
			j=1:i;
			s=cumsum(s|(log(n-j+1)-log(j)+log((i1-j+1))-log((n1-n-i1+j))));
		endif;
	else
		s=log(bin(n,i1-(n1-n)))-log(bin(n1,i1));
		if i>i1-(n1-n);
			j=(i1-(n1-n)+1):i;
			s=cumsum(s|(log(n-j+1)-log(j)+log(i1-j+1)-log(n1-n-i1+j)));
		endif;
	endif
	return sum(exp(s));
endfunction

function hypergeomsum (i,n,itot,ntot)
## Return the probability of hitting i or less black balls, if
## n are chosen out of ntot, and there are a total of itot black
## balls (and ntot-itot white balls).
	return map("hypergeomsum1",i,n,itot,ntot);
endfunction

function mediantest (a,b)
## Test the two distributions a and b on equal mean value.
## To do this both distributions are checked on exceeding
## the median of both.
## Returns the error you have, if you assume a does not
## exceed the median to often (i.e. a may be equal to b)
	c=sort(a|b); n=cols(c);
	if mod(n,2)==0; m=(c[n/2]+c[n/2+1])/2;
	else m=c[n/2+1];
	endif;
	return 1-hypergeomsum(sum(a<m)-1,cols(a),floor(n/2),n);
endfunction

function ranktest (a,b,eps=sqrt(epsilon()))
## The Mann-Whitney test tests a and b on same distribution.
## Return the error you have, if you reject same distribution.
	n1=cols(a); n2=cols(b); n=n1+n2;
	{c,i}=sort(a|b); R=1:n;
	{c1,i1}=sort(c+eps*(n:-1:1));
	R=(R+R[i1])/2;
	R1=sum(R*(i<=n1)); R2=sum(R*(i>n1));
	return 1-normaldis(abs((min(R1-n1*(n1+1)/2,R2-n2*(n2+1)/2)-n1*n2/2)/ ..
		sqrt(n1*n2*(n1+n2+1)/12)));
endfunction

function signtest (a,b)
## Assume a(i) and b(i) are results of treatment a and b at i.
## a and b must be row vectors of equal length.
## Test, if a is not better than b.
## Return the error you have, if you decide that a is better
## than b.
	n=cols(a); i=sum(a>b);
	return 1-binsum(i-1,n,0.5);
endfunction

function wilcoxon (a,b,eps=sqrt(epsilon()))
## This is a sharper test for the same problem as in signtest.
## See: signtest
## Returns the error you have, if you decide that a is better
## than b.
	d=a-b; n=cols(d);
	{c,i}=sort(abs(d)); R=1:n;
	{c1,i1}=sort(c+eps*(n:-1:1));
	R=(R+R[i1])/2;
	R1=sum(R*(d[i]<0)); R2=sum(R*(d[i]>=0));
	W=R2;
	return 1-normaldis((W-n*(n+1)/4)/ ..
		sqrt(n*(n+1)*(2*n+1)/24)));
endfunction

