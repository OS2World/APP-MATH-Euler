comment
singular value decomposition tools
endcomment

function svdkernel (A)
## kernel(a) computes the kernel of the quadratic matrix a.
## This is using the singular value decomposition and works
## for real matrices only.
## The vectors spanning the kernel are orthonormal.
	{B,w,V}=svd(A);
	i=nonzeros(w~=0);
	if cols(i)==0; return zeros(1,cols(V));
	else return V[:,i];
	endif;
endfunction

function svdimage (A)
## image(a) computes the image of the quadratic matrix a.
## This is using the singular value decomposition and works
## for real matrices only.
## The vectors spanning the image are orthonormal.
	{B,w,V}=svd(A);
	i=nonzeros(!(w~=0));
	if cols(i)==0; return zeros(1,cols(A));
	else return B[:,i];
	endif;
endfunction

function svdcondition (A)
## condition(A) returns the condition number based on
## a singular value decompostion of A.
## 0 means singularity.
## A must be real.
	{B,w,V}=svd(A);
	if any(w~=0); return 0; endif;
	return max(abs(w))/min(abs(w));
endfunction

function svddet (A)
## det(A) returns the determinant based on a
## singular value decomposition of A.
## A must be real.
	{B,w,V}=svd(A);
	return prod(w);
endfunction

function svdeigenvalues (A)
## For a symmetrical A, this returns the eigenvalues of A.
## For a non-symmetrical A, the singular values.
## A must be real.
	{B,w,V}=svd(A);
	return w;
endfunction

function svdeigenspace (a,l)
## svdeigenspace(A,l) returns the eigenspace of A to the eigenvaue l.
	k=svdkernel(a-l*id(cols(a)));
	if k==0; error("No eigenvalue found!"); endif;
	si=size(k);
	loop 1 to si[2];
		x=k[:,index()];
		k[:,index()]=x/sqrt(x'.x);
	end;
	return k;
endfunction

function svdsolve (A,b)
## Solve A.x=b with smallest norm for x.
## A must be real.
## This function can be used instead of the fit function.
	{B,w,V}=svd(A);
	i=nonzeros(!(w~=0));
	if (cols(i)>0); w[i]=1/w[i]; endif;
	return V.diag(size(V),0,w).(B'.b);
endfunction

