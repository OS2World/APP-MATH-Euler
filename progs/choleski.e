comment
Choleski decomposition and LGS solver.
Eigenvalue iteration using Choleski decomposition
choleski(A)
lsolve(L,b)
choleigen(A)
endcomment

function choleski (A)
## Decompose a real matrix A, such that A=L.L'. Returns L.
## A must be a positive definite symmetric and at least 2x2 matrix.
	n=cols(A);
	L=zeros(size(A));
	L[1,1]=sqrt(A[1,1]); L[2,1]=A[1,2]/L[1,1];
	L[2,2]=sqrt(A[2,2]-L[2,1]^2);
	loop 3 to n;
		c=flipy(lusolve(flipx(flipy(L[1:#-1,1:#-1])),flipy(A[1:#-1,#])));
		L[#,1:#-1]=c';
		L[#,#]=sqrt(A[#,#]-c'.c);
	end;
	return L;
endfunction

function lsolve (L,b)
## Solve L.L'=b
	return lusolve(L',flipy(lusolve(flipx(flipy(L)),flipy(b))));
endfunction

function choleigen (A)
## Iterates the Choleski-iteration, until the diagonal converges.
## A must be positive definite symmetric and at least 2x2.
	if isvar("eps"); localepsilon(eps); endif;
	L=choleski(A);
	B=L'.L;
	d=diag(B,0);
	repeat
		L=choleski(B);
		B=L'.L;
		dnew=diag(B,0);
		if dnew~=d; break; endif;
		d=dnew;
	end;
	return dnew;
endfunction

