comment
D'Hondt and Hare-Niemeyer (best fit).
hondt(v,n); best(v,n);
cumpute seat distributions for n seats based on votes v (1xn vector).
For a test run
test(0:5:100,[100,200],127);
test(0:0.5:5,100,37);
endcomment

function hondt (v,n)
## Compute the distribution of seats s[1],...s[k], which corresonds
## to v[1],...,v[k], such that sum s[i] = n, using d'Hondt.
	k=length(v);
	t=1/(1:n); S=[]; I=[];
	loop 1 to k;
		S=S|(v[#]*t); I=I|(dup(#,n)');
	end;
	{S,i}=sort(-S);
	I=I[i]; I=I[1:n];
	return count(I-0.5,k);
endfunction

function best (v,n)
## Compute the distribution of seats s[1],...s[k], which corresonds
## to v[1],...,v[k], such that sum s[i] = n, using closest fit.
## The method minimizes the sum of absolute errors and is
## often called Hare-Niemeyer method.
	s=v/sum(v)*n; S=floor(s);
	d=n-sum(S);
	if d>0;
		{f,i}=sort(S-s); i=i[1:d];
		S[i]=S[i]+1;
	endif;
	return S;
endfunction

function test (v1,v,n)
## Test hondt(v1|v,n), for all elements in p.
	p=v1; s=v1; sb=v1; su=sum(v);
	loop 1 to length(p);
		r=hondt(v1[#]|v,n)/n; s[#]=r[1];
		r=best(v1[#]|v,n)/n; sb[#]=r[1];
		p[#]=v1[#]/(su+v1[#]);
	end;
	xplot(p,p); hold on;
	style("mx]"); color(2); mark(p,s); color(1);
	style("m[]"); color(3); mark(p,sb); color(1);
	hold off;
	title("% of votes, % of seats (x Hondt, [] best)"); wait(180);
	m=max(max(abs(s-p)),max(abs(sb-p)));
	setplot(min(p),max(p),-m,m);
	xplot(p,zeros(size(p))); hold on;
	style("mx]"); color(2); mark(p,s-p); color(1);
	style("m[]"); color(3); mark(p,sb-p); color(1);
	hold off;
	title("% error (x Hondt, [] best)"); wait(180);
	"sum of errors (hondt)", sum(abs(s-p)),
	"sum of errors (best)", sum(abs(sb-p)),
	return 0;
endfunction

.. EOF
