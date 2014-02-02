.. Some optimization procedures

function simplex1 (A,b,z)
## Minimize z.x=b under the conditions A.x=b, x>=0.
## A must have full column rank.
	{Res,ri,ci,det}=lu(A);
	c1=nonzeros(ci); c2=nonzeros(ci==0);
	if cols(c1)!=rows(A); error("Rank of A to low"); endif;
	B=A[:,c1];
	{x,f}=simplex(B\A[:,c2],B\b,z[c2]-z[c1].A[:,c2]);
	if f==0;
		xr=zeros(size(z));
		xr[:,c2]=x';
		xr[c1]=(B\(b-A[:,c2].x))';
		endif;
	return {xr',f};
endfunction

function intsimplex (A,b,z,alpha=1e30)
## Minimize z.x=b under the conditions A.x<=b, x integer, x>=0.
	{x,r}=simplex(A,b,z);
	n=cols(A);
	a=z.x;
	if r==0;
		if a>alpha; return {x,-2,a}; endif;
		loop 1 to n;
			if !(x[#]~=round(x[#],0));
				{x1,r1,a1}=intsimplex(A_((1:n)==#),b_floor(x[#]),z,alpha);
				{x2,r2,a2}=intsimplex(A_-((1:n)==#),b_-ceil(x[#]),z,alpha);
				if (r1==0 && z.x1<z.x2) return {x1,r1,a1};
				else return {x2,r2,a2};
				endif;
			endif;
		end;
	endif;
	return {x,r,a}
endfunction

function intsimplex1 (A,b,z)
## Minimize z.x=b under the conditions A.x=b, x>=0.
	return intsimplex(A_-A,b_-b,z);	
endfunction

.. EOF
