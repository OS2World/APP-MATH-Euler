comment
A try to improve the Eigenvalues with Newton iteration
endcomment

function eigenspace1
## eigenspace1(A,l) returns the eigenspace of A to the eigenvalue l.
## returns {x,l1}, where l1 should be an improvement over l, and
## x contains the eigenvectors as columns.
	eps=epsilon();
	repeat;
		k=kernel(arg1-arg2*id(cols(arg1)));
		if k==0; else; break; endif;
		if epsilon()>1 break; endif;
		setepsilon(100*epsilon());
	end;
	if k==0; error("No eigenvalue found!"); endif;
	setepsilon(eps);
	{dummy,l}=eigennewton(arg1,k[:,1],arg2);
	eps=epsilon();
	repeat;
		k=kernel(arg1-l*id(cols(arg1)));
		if k==0; else; break; endif;
		if epsilon()>1 break; endif;
		setepsilon(100*epsilon());
	end;
	if k==0; error("No eigenvalue found!"); endif;
	setepsilon(eps);
	si=size(k);
	loop 1 to si[2];
		x=k[:,index()];
		k[:,index()]=x/sqrt(x'.x);
	end;
	return {k,l};
endfunction

function eigen1 (A)
## eigen1(A) returns the eigenvectors and a basis of eigenvectors of 
## A. {l,x,ll}=eigen(A), where l is a vector of eigenvalues,
## x is a basis of eigenvectors,
## and ll is a vector of distinct eigenvalues.
## Improved version of eigen.
	l=eigenvalues(A);
	{s,li}=eigenspace1(A,l[1]);
	si=size(s); v=dup(li,si[2])'; vv=li;
	l=eigenremove(l,si[2]);
	repeat;
		if min(size(l))==0; break; endif;
		{sp,li}=eigenspace1(A,l[1]);
		si=size(sp); l=eigenremove(l,si[2]);
		s=s|sp; v=v|dup(li,si[2])'; vv=vv|li;
	end;
	return {v,s,vv}
endfunction

function eigennewton
## eigennewton(a,x,l) does a newton step to improve the eigenvalue l
## of a and the eigenvector x.
## returns {x1,l1}.
	a=arg1; x=arg2; x=x/sqrt(x'.x); n=cols(a);
	d=((a-arg3*id(n))|-x)_(2*x'|0);
	b=d\((a.x-arg3*x)_0);
	return {x-b[1:n],arg3-b[n+1]};
endfunction

