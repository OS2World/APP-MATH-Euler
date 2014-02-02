comment
linear algebra, polynomial fit
endcomment

function id (n)
## id(n) creates a nxn identity matrix.
	return diag([n n],0,1);
endfunction

function inv (a)
## inv(a) produces the inverse of a matrix.
	return a\id(cols(a));
endfunction

function fit (a,b)
## fit(a,b) computes x such that ||a.x-b||_2 is minimal.
## a is a nxm matrix, and b is a mx1 vector.
## For badly conditioned a, you should use svdsolve instead.
	A=conj(a');
	return symmult(A,a)\(A.b)
endfunction

function norm (A)
## Compute the maximal row sum of A
	return max(sum(abs(A))');
endfunction

function kernel (A)
## kernel(a) computes the kernel of the quadratic matrix a.
## You might add eps=..., if a is almost regular.
	if isvar("eps"); localepsilon(eps); endif;
	a=A/norm(A);
	{aa,r,c,d}=lu(a);
	n=size(a); nr=size(r);
	if nr[2]==n[2]; return zeros([1,n[2]])'; endif;
	if nr[2]==0; return id(n[2]); endif;
	c1=nonzeros(c); c2=nonzeros(!c);
	b=lusolve(aa[r,c1],a[r,c2]);
	m=size(b);
	x=zeros([n[2] m[2]]);
	x[c1,:]=-b;
	x[c2,:]=id(cols(c2));
	return x
endfunction

function image (A)
## image(a) computes the image of the quadratic matrix a.
	if isvar("eps"); localepsilon(eps); endif;
	a=A/norm(A);
	{aa,r,c,d}=lu(a);
	return a[:,nonzeros(c));
endfunction

function rank (A)
## rank(A) the rank of a matrix A.
	return cols(image(A));
endfunction

function solvespecial (A,b)
## solvespecial(A,b) computes a special solution using
## the LU decomposition.
## There is also svdsolve(A,b) or fit(A,b).
	{a,r,c,d}=lu(A); x=zeros(cols(A),1); cc=nonzeros(c);
	s=A(:,cc)\b; x(cc)=s;
	return x
endfunction

function det (a)
## det(A) returns the determinant of A
	if isvar("eps"); localepsilon(eps); endif;
	r=norm(a);
	{aa,rows,c,d}=lu(a/r);
	return d*r^cols(a);
endfunction

function crossproduct (v,w)
## crossproduct(v,w) computes the cross product between
## two 3x1 vectors.
	return [v[2]*w[3]-v[3]*w[2];-v[1]*w[3]+w[1]*v[3];v[1]*w[2]-v[2]*w[1]];
endfunction

function eigenremove(l)
## helping function.
	return l(nonzeros(!(l[1]~=l)))
endfunction

function eigenvalues (a)
## eigenvalues(A) returns the eigenvalues of A.
	return polysolve(charpoly(a));
endfunction

function eigenspace (a,l)
## eigenspace(A,l) returns the eigenspace of A to the eigenvaue l.
	k=kernel(a-l*id(cols(a)));
	if k==0; error("No eigenvalue found!"); endif;
	si=size(k);
	loop 1 to si[2];
		x=k[:,index()];
		k[:,index()]=x/sqrt(x'.x);
	end;
	return k;
endfunction

function eigen (A)
## eigen(A) returns the eigenvectors and a basis of eigenvectors of A.
## {l,x,ll}=eigen(A), where l is a vector of eigenvalues,
## x is a basis of eigenvectors,
## and ll is a vector of distinct eigenvalues.
	l=eigenvalues(A);
	s=eigenspace(A,l[1]);
	si=size(s); v=dup(l[1],si[2])'; vv=l[1];
	l=eigenremove(l,si[2]);
	repeat;
		if min(size(l))==0; break; endif;
		ll=l[1]; sp=eigenspace(A,ll);
		si=size(sp); l=eigenremove(l,si[2]);
		s=s|sp; v=v|dup(ll,si[2])'; vv=vv|ll;
	end;
	return {v,s,vv}
endfunction

hilbertfactor=3*3*3*5*5*7*11*13*17*19*23*29;

function hilbert (n,f=hilbertfactor)
## hilbert(n) produces the nxn-Hilbert matrix.
## It is accurate up to the 30x30 Hilbert matrix.
    {i,j}=field(1:n,1:n);
    return f/(i+j-1);
endfunction

function hilb (n,f=hilbertfactor)
	return hilbert(n,f);
endfunction

function polyfit (xx,yy,n)
## fit(x,y,degree) fits a polynomial in L_2-norm to (x,y).
	A=ones(size(xx))_dup(xx,n); A=cumprod(A');
	return fit(A,yy')';
endfunction

function rotation2 (A)
## Rotate a 2x2-Matrix complex to
## an upper triangle matrix
## returns Q such that Q.A.conj(Q') = R
	l=eigenvalues(A);
	v=[-A[1,2];A[1,1]-l[1]];
	l=sqrt(conj(v').v);
	v=v/l;
	Q=[v[1],conj(v[2]);v[2],-conj(v[1])];
	return Q;
endfunction

function cjacobi (A,eps=sqrt(epsilon))
## Complex Jacoby method.
## Return the eigenvalues of a general real
## or complex matrix A.
	A0=A;
	n=cols(A0);
	repeat;
		H=band(A0,-n,-1);
		E=extrema(abs(H));
		J=extrema(E[:,3]');
		i=J[4];
		j=E[i,4];
		if J[3]<eps; break; endif;
		A2=A0[[j,i],[j,i]];
		Q2=rotation2(A2);
		Q=id(n);
		Q[[j,i],[j,i]]=Q2;
		A0=conj(Q').A0.Q;
	end;
	return diag(A0,0);
endfunction
