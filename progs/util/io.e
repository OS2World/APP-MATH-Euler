comment
matrix input and output
endcomment

function getmatrix (n,m,filename="")
## Read a real nxm Matrix from the file.
## If the filename is "", the file must be opened before.
## Else the file will open and close for read.
## Matrix values must be decimal fix point numbers with
## a dot (not a comma). They must be stored row after row.
	if filename<>""; open(filename,"r"); endif;
	{v,N}=getvector(n*m);
	if filename<>""; close(); endif;
	if (N<n*m); error("Incorrect values in file"); endif;
	v=v[1:n*m];
	return redim(v,n,m);
endfunction

function writematrix (v,filename="",format="%0.16f")
## Write a real nxm Matrix to the file row after row.
## If the filename is not "", the file will be created (beware!).
	if typeof(v)<>2; error("Can write only real matrices"); endif;
	if filename<>""; open(filename,"w"); endif;
	f=format|" ";
	for i=1 to rows(v);
		for j=1 to cols(v);
			write(printf(f,v[i,j]));
		end;
		putchar(10);
	end;
	if filename<>""; close(); endif;
	return "";
endfunction

function printformat (x,f="%g",sep1=", ",sep2="; ")
## Print the vector x formatted with format f.
## See: printf
## The format is applied to real and imaginary part,
## or lower and upper part of the elements of x.
	s=size(x);
	loop 1 to s[1];
		i=#;
		loop 1 to s[2]
		if iscomplex(x);
			write(printf(f|sep1,re(x[i,#])));
			write(printf(f|sep2,im(x[i,#])));
		elseif isinterval(x);
			write(printf(f|sep1,left(x[i,#])));
			write(printf(f|sep2,right(x[i,#])));
		else write(printf(f|sep2,x[i,#]));
		endif;
		end;
		putchar(10);
	end;
	return "";
endfunction

function writeform (x)
	if isreal(x); return printf("%25.16e",x); endif;
	if iscomplex(x);
		return printf("%25.16e",re(x))|printf("+%25.16ei",im(x));
	endif;
	if isstring(x); return x; endif;
	error("Cannot print this!");
endfunction

function varwrite (x,s="")
## Write a variable to ouput in EULER syntax. Use s as the name.
## Output looks like s=...
	if s==""; s=name(x); endif;
	si=size(x);
	if max(si)==1; s|" = "|writeform(x)|";", return 0; endif;
	s|" = [ .."
	for i=1 to si[1];
		for j=1 to si[2]-1;
			writeform(x[i,j])|",",
		end;
		if i==si[1]; writeform(x[i,si[2]])|"];",
		else; writeform(x[i,si[2]])|";",
		endif;
	end;
	return 0
endfunction

