comment
Shows how to print numbers, so that they can appear in a TeX table.
texprint(x,d); prints the matrix x with d decimal places.
itexprint(x,d); the same for interval matrices.
endcomment

function texprint (x,d=5)
	form="\hfill %0."|printf("%g",d)|"g & ";
	form1="\quad %0."|printf("%g",d)|"g & ";
	filler=printf(form1,1/300000000000);
	n=cols(x); m=rows(x)
	s="\settabs\+ \qquad \quad & ";
	loop 1 to n
		s=s|filler;
	end
	s|"\cr",
	loop 1 to m;
		i=#; s="\+ & ";
		loop 1 to n
			s=s|printf(form,x[i,#]);
		end;
		s|"\cr",
	end;
	return ""
endfunction

function itexprint (x,d=5)
	lform="\hfill [%0."|printf("%g",d)|"g";
	rform=",%0."|printf("%g",d)|"g] & ";
	form1="%0."|printf("%g",d)|"g";
	f=printf(form1,1/30000000000);
	filler="\quad "|f|f|" & ";
	n=cols(x); m=rows(x)
	s="\settabs\+ \qquad \quad & ";
	loop 1 to n
		s=s|filler;
	end
	s|"\cr",
	loop 1 to m;
		i=#; s="\+ & ";
		loop 1 to n
			s=s|printf(lform,left(x[i,#]))|printf(rform,right(x[i,#]));
		end;
		s|"\cr",
	end;
	return ""
endfunction

 
