function findYvrange(Vmin,Vmax)
	.. volt range
	cv=[20,10,5,2,1,0.5,0.2,0.1,0.05,0.02,0.01,0.005,0.002,0.001];
	.. find the right range
	cvi=1;
	for i=1 to length(cv)-1
		if (4*cv[i+1] < Vmax) || (-4*cv[i+1] > Vmin);
			cvi=i;
			break;
		endif;
	end;
	return cv[cvi];
endfunction

function findXvrange(Vmin,Vmax)
	.. volt range
	cv=[20,10,5,2,1,0.5,0.2,0.1,0.05,0.02,0.01,0.005,0.002,0.001];
	.. find the right range
	cvi=1;
	for i=1 to length(cv)-1
		if (5*cv[i+1] < Vmax) || (-5*cv[i+1] > Vmin);
			cvi=i;
			break;
		endif;
	end;
	return cv[cvi];
endfunction

function findtrange(Tmax)
	.. time range
	ct=[0.2,0.1,0.05,0.02,0.01,0.005,0.002,0.001,0.0005,0.0002,0.0001,0.00005,0.00002,0.00001,0.000005,0.000002,0.000001,0.0000005];
	.. find the right range
	cti=1;
	for i=1 to length(ct)-1
		if (10*ct[i+1]+epsilon < Tmax);
			cti=i;
			break;
		endif;
	end;
	return ct[cti];
endfunction

function drawgrid(dt,dv)
	ho=holding(1);
	lw=linewidth(1);
	c=color(1);
	.. draw vertical lines
	ls=linestyle(".");
	for i=1 to 9
		if i != 5
			plot([i*dt,i*dt],[-4*dv,4*dv]);
		endif;
	end;
	linestyle(ls);
	plot([5*dt,5*dt],[-4*dv,4*dv]);
	for j=1 to 49
		plot([j/5*dt,j/5*dt],[-0.1*dv,0.1*dv]);
	end;

	.. draw horizontal lines
	ls=linestyle(".");
	for i=-3 to 3
		if i != 0;
			plot([0,10*dt],[i*dv,i*dv]);
		endif;
	end;
	linestyle(ls);
	plot([0,10*dt],[0,0]);
	for j=-19 to 19
		plot([4.9*dt,5.1*dt],[j/5*dv,j/5*dv]);
	end;
	
	color(c);
	linewidth(lw);
	holding(ho);
	return 0;
endfunction;

function drawXYgrid(dv1,dv2)
	ho=holding(1);
	lw=linewidth(1);
	c=color(1);
	.. draw vertical lines
	ls=linestyle(".");
	for i=-4 to 4
		if i != 0
			plot([i*dv1,i*dv1],[-4*dv2,4*dv2]);
		endif;
	end;
	linestyle(ls);
	plot([0,0],[-4*dv2,4*dv2]);
	for j=-24 to 24
		plot([j/5*dv1,j/5*dv1],[-0.1*dv2,0.1*dv2]);
	end;

	.. draw horizontal lines
	ls=linestyle(".");
	for i=-3 to 3
		if i != 0;
			plot([-5*dv1,5*dv1],[i*dv2,i*dv2]);
		endif;
	end;
	linestyle(ls);
	plot([-5*dv1,5*dv1],[0,0]);
	for j=-19 to 19
		plot([-0.1*dv1,0.1*dv1],[j/5*dv2,j/5*dv2]);
	end;
	
	color(c);
	linewidth(lw);
	holding(ho);
	return 0;
endfunction;

function scope (x=0,y1=0,y2=0)
## o scope(x,y1,), scope(x,,y2) and scope(x,y1,y2) shows a scope grid and
##   the curves y1(x) or y2(x) or both.
## o scope(,y1,y2) shows the curve y2(y1) in XY mode.
## o scope() shows only the grid.
	if argn()==0;
		p=plot();
	else if (argn()==1);
		error("not enough parameters (2 at least)");
	else if (argn()==2) || ((argn()==3) && (y1==0));
		.. normal mode ; one curve
		if !holding();clg; frame();endif;
		dt=findtrange(max(x));
		if (y2==0);
			dv1=findYvrange(min(y1),max(y1));
		else
			dv1=findYvrange(min(y2),max(y2));
		endif;
		p=[0,10*dt,-4*dv1,4*dv1];setplot(p);
		drawgrid(dt,dv1);
		.. display ranges
		tp=toscreen([7.5*dt,3.8*dv1]);
		text("time : "|printf("%g s/div",dt),tp);
		tp[2]=tp[2]+textheight();
		if (y2==0);
			text("CH1  : "|printf("%g V/div",dv1),tp);
			ho=holding(1); plot(x,y1); holding(ho);
		else
			text("CH2  : "|printf("%g V/div",dv1),tp);
			ho=holding(1); plot(x,y2); holding(ho);
		endif;
	else if (argn()==3) && (x==0);
		.. XY mode
		if !holding();clg; frame();endif;
		dv1=findXvrange(min(y1),max(y1));
		dv2=findYvrange(min(y2),max(y2));
		p=[-5*dv1,5*dv1,-4*dv2,4*dv2];setplot(p);
		drawXYgrid(dv1,dv2);
		.. display ranges
		tp=toscreen([2.5*dv1,3.8*dv2]);
		text("CH1 (X) : "|printf("%g V/div",dv1),tp);
		tp[2]=tp[2]+textheight();
		text("CH2 (Y) : "|printf("%g V/div",dv2),tp);
		ho=holding(1); plot(y1,y2); holding(ho);
	else
		.. normal display ; two curves
		if !holding();clg; frame();endif;
		dt=findtrange(max(x));
		dv1=findYvrange(min(y1),max(y1));
		dv2=findYvrange(min(y2),max(y2));
		p=[0,10*dt,-4*dv1,4*dv1];setplot(p);
		drawgrid(dt,dv1);
		.. display ranges
		tp=toscreen([7.5*dt,3.8*dv1]);
		text("time : "|printf("%g s/div",dt),tp);
		tp[2]=tp[2]+textheight();
		text("CH1  : "|printf("%g V/div",dv1),tp);
		tp[2]=tp[2]+textheight();
		text("CH2  : "|printf("%g V/div",dv2),tp);
		ho=holding(1); plot(x,y1); plot(x,dv1/dv2*y2); holding(ho);
	endif;
	endif;
	endif;
	endif;
	return p;
endfunction
