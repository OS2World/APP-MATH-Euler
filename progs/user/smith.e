function smith(r = [0, 0.2, 0.5, 1, 2, 5], x = [0.2, 0.5, 1, 2, 5])
## SMITH    Draws a Smith chart and sets hold to on so that you can plot the
##          reflection coefficient on top of it.
## This is a quick little hack that draws a simple Smith chart, a diagram
## that is very frequently used in high frequency engineering.  It maps
## the normalized impedance z to the reflection coefficient gamma:
## gamma = (z-1)/(z+1)  with z = r+i*x .
## You can customize the function by specifying the r and x parameters
## 3-13-95   Frank Wiedmann   for the matlab verion wiedmann@com.enst.fr
## 8-31-99   Eric Boucharé    for the update to euler Eric.Bouchare@wanadoo.fr
	
	_setplot([-1,1,-1,1]);
	hold on;
	h = textheight();
	w = textwidth();
	tr = 2*pi*(0:.01:1);

	plot([-1,1],[0,0]);
..	r = [0 .2 .5 1 2 5];   .. specify the r=const lines you want to draw
	for i=1 to length(r);
		rr = 1/(r{i}+1);
		cr = 1-rr;
		plot(cr+rr*cos(tr),rr*sin(tr));
		p = toscreen([cr-rr,0]);
		if r{i}==0;
			rtext(printf("%g",r{i}),[p{1}-(w/2),p{2}-(h/3)]);
		else
			rtext(printf("%g",r{i}),[p{1}-(w/2),p{2}]);
		endif;
	end;

..	x = [.2 .5 1 2 5];     .. specify the x=const lines you want to draw
	for i=1 to length(x);
		rx = 1/x{i};
		cx = rx;
		tx = 2*atan(x{i})*(0:.01:1);
		plot(1-rx*sin(tx),cx-rx*cos(tx));
		u = 1-rx*sin(2*atan(x{i}));
		v = cx-rx*cos(2*atan(x{i}));
		p=toscreen([u,v]);
		if u<0;
			rtext(printf("%g",x{i}),[p{1}-(w/2),p{2}-(h/3+h*v)]);
		elseif u>0
			text(printf("%g",x{i}),[p{1}+(w/2),p{2}-(h/3+h*v)]);
		else
			ctext(printf("%g",x{i}),[p{1},p{2}-(h+h/3)]);
		endif;
		
		plot(1-rx*sin(tx),-cx+rx*cos(tx));
		u = 1-rx*sin(2*atan(x{i}));
		v = -cx+rx*cos(2*atan(x{i}));
		p=toscreen([u,v]);
		if u<0;
			rtext(printf("%g",x{i}),[p{1}-(w/2),p{2}]);
		elseif u>0
			text(printf("%g",x{i}),[p{1}+(w/2),p{2}]);
		else
			ctext(printf("%g",x{i}),[p{1},p{2}+(h/3)]);
		endif;
	end;
	hold off;
	return 0;
endfunction
