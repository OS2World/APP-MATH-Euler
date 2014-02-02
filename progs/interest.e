comment
Computes interest rates (no guarantee!).
rate(x); computes the interest rate for x[0],...,x[m], the sum of
which must be positive, x[i] is at time i=0..m.
rate2(x,n); does the same, but x[i] is at time n[i].
investment(start,rate,final,n,i0,i1); computes the interest rate
of an investment start at 0 with paybacks rate from time i0 to
time n-i1, and a final investment at n.
This may be used for loans or savings.
endcomment

function rate (x)
## Compute the interest rate in % for a vector events at times
## 0,1,...,n. Negative numbers mean payments, positive mean income.
## E.g.: rate([-100,5,5,5,5,5,5,5,105]) will be 5%.
    if sum(x)<0; error("Rendite negativ?"); endif;
    q=polydif(x);
    c=1;
    repeat
        cnew=c-polyval(x,c)/polyval(q,c);
        if c~=cnew; break; endif;
        c=cnew;
    end;
    return (1/cnew-1)*100;
endfunction

function rate2 (x,n)
## Same as rate but at times n1,n2,... (can be fractional)
    if sum(x)<0; error("Rendite negativ?"); endif;
    k2=x*n; n2=n-1;
    k2=k2[2:length(x)]; n2=n2[2:length(x)];
    c=1;
    repeat
        cnew=c-sum(x*c^n)/sum(k2*c^n2);
        if c~=cnew; break; endif;
        c=cnew;
    end;
    return (1/cnew-1)*100;
endfunction

function rateformula (f,k0,r,k1,n,i0,i1)
	return k0*f^n+r*f^i1*(f^(n-i0-i1+1)-1)/(f-1)+k1
endfunction

function investment (start,instalment,final,n,i0=1,i1=1)
## Computes the interest rate of a loan and other investments.
## Note that payments are negative!
## start: start investment at period 0
## instalment: from start of periods i0 to n-i1
## final: that is the final dept after n periods
## The result is the interest rate in %.
## Example for a loan: investment(1000,-100,-900,10,1,0)
## (first payment after 1 period, last after n periods)
## Example for a savings account: investment(0,-100,1200,10,0,1)
## (first payment immediately, last aftern n-1 periods)
	si=size(start,instalment,final,n,i0,i1);
	f=zeros(si);
	loop 1 to prod(si);
		f{#}=bisect("rateformula",1.0000001,2,start{#},instalment{#}, ..
			final{#},n{#},i0{#},i1{#});
	end;
	return (f-1)*100;
endfunction

function finaldebt (loan,periods,rate,payment,start=1)
## Compute the final dept of a loan after periods of payments.
	f=1+rate/100;
	return loan*f^periods-payment*(f^(periods-start+1)-1)/(f-1);
endfunction

