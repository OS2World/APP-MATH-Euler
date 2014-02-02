comment
animation
endcomment

function animate (d=0.1)
## Animate the pages with delay d.
	testkey();
	repeat
		loop 2 to pages();
			showpage(#); 
			if testkey(); showpage(0); return pages(); endif;
			dd=wait(d);
			if (dd<d); showpage(0); return pages(); endif;
		end;
	end;
	showpage(0);
	return pages();
endfunction;	

function rotate (fff,d=0.01,n=120)
## Shows a rotating animation of the plot fff,
## which must be a 3D plot command.
## Example: rotate("f3dplot";"x*y^2");
	deletepages();
	clg;
	title("Creating Animation");
	addpage();
	showpage(1);
	t=linspace(0,2*pi,n);
	v=view();
	loop 1 to cols(t);
		v[3]=t[#]; view(v);
		fff(args());
		addpage();
	end;
	animate(d);
	return pages();
endfunction

function fanimate (fff,t,d=0.01)
## Animate the function fff, which takes
## a parameter t. fff should display
## some graphics depending on t.
## Additional parameters are passed to
## f.
	deletepages();
	clg;
	title("Creating Animation");
	addpage();
	showpage(1);
	loop 1 to cols(t);
		fff(t[#],args());
		addpage();
	end;
	animate(d);
	return pages();
endfunction
