function stereo
	x=-1:0.2:1; y=x';
	hold on;
	clg;
	s=300; m=510;
	window(m-s,m-s/2,m,m+s/2);
	view(3,1.5,0.5,0.5);
	wire(x,y,x^2*y^3);
	window(m,m-s/2,m+s,m+s/2);
	view(3,1.5,0.55,0.5);
	wire(x,y,x^2*y^3);
	hold off;
	shrinkwindow();
	return ""
endfunction

"Try to see three pictures and focus the center one."
"Works better, when you print the graphics."
"Press return"
wait(20);

stereo; wait(180);

