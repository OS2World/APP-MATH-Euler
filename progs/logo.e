function spiral
	{r,a}=field(0:0.1:1,0:pi()/8:6*pi());
	z=a/8;
	x=r*cos(a)*(1-a/20);
	y=r*sin(a)*(1-a/20);
	z=z-1.4;
	view(3.1,2,0.5,0.2);
	framedsolid(x,y,z);
	return 0;
endfunction

spiral;

