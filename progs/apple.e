comment
Show the apple set and investigate it, starts automatically or with
test();
endcomment

function apple1 (z,z0)
## one iteration
	w=z0+z*z;
	return w
endfunction

function apple (z)
## compute 10 iterations
	w=z;
	loop 1 to 7; w=apple1(w,z); end:
	return w
endfunction

function julia (z,z0)
## compute 10 iterations
	w=z;
	loop 1 to 7; w=z0+w*w; end:
	return w
endfunction

function showapple (a)
## show the apple set in 3D
	{x,y}=field(linspace(a[1],a[2],50),linspace(a[3],a[4],50));
	z=x+1i*y; w=apple(z);
	view(5,3,-0.75,0.7); twosides(0);
	wa=abs(w); wa=max(wa,1);
	l=log(wa); l=2*l/max(max(l)');
	framedsolid(x,y,-l,1);
	return wa;
endfunction

function showjulia (z0,a)
## show the apple set in 3D
	{x,y}=field(linspace(a[1],a[2],50),linspace(a[3],a[4],50));
	z=x+1i*y; w=julia(z,z0);
	view(5,3,-0.75,0.7); twosides(0);
	wa=abs(w); wa=max(wa,1);
	l=log(wa); l=2*l/max(max(l)');
	framedsolid(x,y,-l,1);
	return wa;
endfunction

function showcontour(w,x)
## show the iterations
	clg;
	shrinkwindow();
	wl=log(w); wl=wl/totalmax(wl);
	repeat
		clg;
		setplot(x);
		density(wl*0.8+0.1); hold on; xplot();
		contour(wl,linspace(epsilon,1,10));
		t=linspace(0,2*pi,500);
		color(2);
		plot(0.5*cos(t)*(1-cos(t))+0.25,0.5*sin(t)*(1-cos(t)));
		color(1);
		hold off;
    	title("Click any point for the local Julia set! (Finish -> <here>)");
		r=0;
		m=mouse(); z=m[1]+1i*m[2];
		if m[2]>x[4]; break; endif;
		xh=[-3,3,-3,3];
		setplot(xh);
		wa=showjulia(z,xh);
		title(printf("Julia set at (%g",re(z))|printf("%g)",im(z)));
		wait(180);
		clg;
		wa=log(wa); wa=wa/totalmax(wa);
		density(wa*0.8+0.1); hold on; xplot();
		contour(wa,linspace(epsilon,1,10));
		t=sqrt(1-4*z);
		color(3); mark([(1-t)/2,(1+t)/2]); color(1);
		hold off;
		title("The fixpoints");
		wait(180);		
	end;
	hold off;
	return r
endfunction

function test()
	"Just a moment!",
	x=[-2,0.5,-1.25,1.25];
    w=showapple(x); title("Press any key please!"); wait(180);
	showcontour(w,x);
	return w;
endfunction

test();
