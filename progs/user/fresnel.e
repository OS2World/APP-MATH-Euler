function vector(x,y)
	st=linestyle("->");
	plot([0,x],[0,y]);
	linestyle(st);
	return 0;
endfunction

function fresnel(t=0,VM=10,T=20e-3,theta=0)
## draws a sinusoïde and the corresponding fresnel vector at time t
##
##   t      : the current time (s)
##   VM     : peak voltage (V)
##   T      : period (s)
##   theta  : origin phase (rad)
##

	.. draw an expanded trigonometric circle

	.. wd=window([20,40,320,340]); .. if square window
	wd=window([20,40,320,470]); .. if maximized window

	_setplot([-VM,VM,-VM,VM]);
	hold on;

	lw=linewidth(1);
	a = 2*pi*(0:0.01:1);
	plot(VM*cos(a),VM*sin(a));

	plot([0,0],[-VM,VM]);
	plot([-VM,VM],[0,0]);

	wt = 2*pi*t/T;
	ph = wt+theta;
	x = VM*cos(ph);
	y = VM*sin(ph);

	st = linestyle(".");
	plot([0,x],[y,y]);
	plot([x,x],[0,y]);
	linestyle(st);
	
	c=color(10);
	vector(x,y);
	color(c);

	.. draw a cartesian sinus

	.. wd=window([340,40,1004,340]); .. if square window
	window([340,40,1004,470]); .. if maximized window
	_setplot([0,2*pi,-VM,VM]);

	st=linestyle(".");
	plot([0,wt],[y,y]);
	plot([wt,wt],[0,y]);
	linestyle(st);

	plot([0,2*pi],[0,0]);
	c=color(10);
	plot(a,VM*sin(a+theta));
	color(c);
	text("0",toscreen([0,0]));
	rtext("T/2",toscreen([pi,0]));
	rtext("T",toscreen([2*pi,0]));
	p=toscreen([wt,0]);
	if y<0; p{2}=p{2}-textheight(); endif;
	ctext("t",p);
	text(printf("t       = %g s",t),[340,470]);
	text(printf("VM      = %g V",VM),[340,470+textheight()]);
	text(printf("T       = %g s",T),[340,470+2*textheight()]);
	text(printf("f = 1/T = %g Hz",1/T),[340,470+3*textheight()]);
	text(printf("theta   = %g rad",theta),[340,470+4*textheight()]);
	text(printf("v(t) = %g",VM)|printf(" sin(2*pi*%g",1/T)|printf("*t + %g)",theta),[340,470+6*textheight()]);
	linewidth(lw);
	hold off;
	return 0;
endfunction


function animatefresnel(VM=10,T=20e-3,theta=0,p=20,d=0.2)
## draws the fresnel vector moving as the time goes on
##
##   VM     : peak voltage (V)
##   T      : period (s)
##   theta  : origin phase (rad)
##
##   p      : number of frames used for the animation
##   d      : frame display rate (s)
##
	.. create animation
	reset;
	deletepages();
	clg;
	title("Préparation de l'animation, un peu de patience...");
	addpage();
	showpage(1);
	
	for i=1 to p
		clg;
		title("le vecteur de fresnel tourne à la pulsation w=2*pi*f rad/s");
		fresnel(T*(i-1)/p,VM,T,theta);
		addpage();
	end;
	clg;

	.. display animation
	repeat
		loop 2 to p+1
			showpage(#);wait(d);
			if testkey; showpage(0); return pages(); endif;
		end;
	end;
	showpage(0);
	return pages();
endfunction;

function drawfresnel()
	clg;
	T     = 20e-3;
	VM    = 10;
	theta = 0;
	fresnel(0,VM,T,theta);
	repeat
		title("le vecteur de fresnel");
		.. wd=window([20,40,320,340]); .. if square window
		wd=window([20,40,320,470]); .. if maximized window
		_setplot([-VM,VM,-VM,VM]);
		
		p=mouse();
		if p{1}<-VM || p{1}>VM || p{2}<-VM || p{2}>VM;
			return 0;
		endif;
		clg;
		if p{1}>=0 && p{2}>=0;
			theta=atan(p{2}/p{1});
		elseif p{1}>=0 && p{2}<0;
			theta=2*pi+atan(p{2}/p{1});
		else
			theta=pi+atan(p{2}/p{1});
		endif;
		fresnel(0,VM,20e-3,theta);


		.. wd=window([20,40,320,340]); .. if square window
		wd=window([20,40,320,470]); .. if maximized window
		_setplot([-VM,VM,-VM,VM]);
	hold on;
	a = 2*pi*(0:0.01:1);

	ph = theta;
	x = VM/sqrt(2)*cos(ph);
	y = VM/sqrt(2)*sin(ph);

	c = color(11);
	st = linestyle(".");
	plot(VM/sqrt(2)*cos(a),VM/sqrt(2)*sin(a));
	plot([0,x],[y,y]);
	plot([x,x],[0,y]);
	linestyle(st);
	
	lw=linewidth(2);
	vector(x,y);
	linewidth(lw);
	color(c);
	hold off;

	end;
	return 0;
endfunction;

