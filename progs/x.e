comment
Exact solver
endcomment

.. ### Exact arithmetic ###

function xlgs (A,b,n=20)
## Compute the solution of Ax=b.
## n is the number of residual iterations.
## The iteration stops, when there is no more change in x.
## You can specify an epsilon eps with eps=... as last parameter.
	{LU,rows,c,d}=lu(A); LU=LU[rows];
	if (isvar("eps")); localepsilon(eps); endif;
	v=lusolve(LU,b[rows]);
	loop 1 to n;
		r=residuum(A,v,b);
		vn=v-lusolve(LU,r[rows]);
		if vn~=v; break; endif;
		v=vn;
	end;
	return vn;
endfunction

function xinv (A,n=20)
## Cumpute the invers of A using residual iteration.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); return xlgs(A,id(cols(A)),eps=eps);
	else (isvar("eps")); return xlgs(A,id(cols(A)));
	endif
endfunction

function xlusolve (A,b,n=20)
## Compute the solution of Ax=b for LU-matrices A.
## E.g., A may be a lower triangle matrix.
## n is the number of residual iterations.
## The iteration stops, when there is no more change in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	v=lusolve(A,b);
	loop 1 to n;
		r=residuum(A,v,b);
		vn=v-lusolve(A,r);
		if vn~=v; break; endif;
		v=vn;
	end;
	return vn;
endfunction

function xpolyval (p,t,n=20)
## Evaluate the polynomial at values t using exact residual
## operations. The iteration stops, when there is no more
## change in the value.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	s=t; si=size(t);
	A=id(cols(p));
	b=flipx(p)';
	loop 1 to prod(si);
		A=setdiag(A,-1,-t{#});
		v=lusolve(A,b);
		loop 1 to n;
			r=residuum(A,v,b);
			vn=v-lusolve(A,r);
			if vn~=v; break; endif;
			v=vn;
		end;
		s{#}=v[cols(p)];
	end;
	return s;
endfunction

function xeigenvalue (a,l,x=0)
## xeigenvalue(A,l) returns an improved the eigenvalue of A.
## l must be closed to a simple eigenvalue.
## Returns the eigenvalue and the eigenvector.
	l1=l;
	if x==0; x=kernel(a-l1*id(cols(a))); endif;
	if x==0; error("Eigenvalue generation failed!"); endif;
	x=x[:,1];
	repeat;
		x=(a-l1*id(cols(a)))\x;
		x=x/sqrt(x'.x);
		l2=residuum(x',residuum(a,x,0),0);
		if l1~=l2; return {l2,x}; endif;
		l1=l2;
	end;
endfunction

