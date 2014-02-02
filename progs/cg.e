comment
CG Methode
endcomment

function cg (A,b,x0=-1,f=2)
## CG Methode zum Lösen von Ax=b
	n=length(A);
	if x0==-1; x=zeros(size(b));
	else; x=x0;
	endif;
	h=b-A.x; r=h;
	alt= 1e+300;
	count=0;
	repeat
		fehler=sqrt(r'.r);
		if fehler~=0;	return x;	endif;
		if count>f*n;
			if fehler>alt; return x;endif;
		endif;
		count=count+ 1, "",
		alt=fehler;
		a=(r'.r)/(h'.A.h);
		x=x+a*h;
		"", x,
		rn=r-a*A.h;
		h=rn+(rn'.rn)/(r'.r)*h;
		r=rn;
	end;
endfunction

..A=[7,8,9,6,4;1,2,3,1,1;5,6,5,4,2;2,1,3,1,4;9,5,4,3,1];
..A=A'.A;
..x=cg(A,[1;2;3;4;5]);
..A.x
