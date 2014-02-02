comment
Two problems:
- Child and toy problem.
- Man and dog problem.
Load the child notebook for a demonstration.
endcomment

function childsin (t)
## Child moves along a circle
	return {[t,sin(t)],[1,cos(t)]}
endfunction

function childcircle (t)
## Child moves along a circle
	return {[cos(t),sin(t)],[-sin(t),cos(t)]}
endfunction

function fchild (t,x,fff)
## ODE to be solved
	{xc,vc}=fff(t);
	w=x-xc;
	w=w/sqrt(sum(w*w));
	return (vc.w')*w;
endfunction

function child (fff,start=[2,0],dur=10,n=200)
## A child walks along a curve described by fff from time 0 to dur.
## It holds a toy on a stick, which is initially at start.
## The function displays the paths of the child and the toy.
	t=linspace(0,dur,n);
	x=zeros(n+1,2);
	loop 1 to n+1;
		x[#]=fff(t[#];fff,args());
	end;
	x=x';
	keepsquare(1);
	y=heun("fchild",t,start;fff,args());
	setplot(plotarea(x[1]_y[1],x[2]_y[2]));
	color(2); plot(x[1],x[2]);
	color(1); hold on; plot(y[1],y[2]); hold off;
	xplot();
	keepsquare(0);
	return ""
endfunction

function dogcircle (t)
	return [cos(t),sin(t)];
endfunction

function dogline (t)
	return [t,0];
endfunction

function fdog (t,x,fff,speed)
## ODE to be solved
	xc=fff(t);
	w=xc-x;
	w=w/sqrt(sum(w*w));
	return speed*w;
endfunction

function dog (fff,start=[2,0],dur=10,speed=1,n=200)
## A child walks along a curve described by fff from time 0 to dur.
## It holds a toy on a stick, which is initially at start.
## The function displays the paths of the child and the toy.
	t=linspace(0,dur,n);
	t=linspace(0,dur,n);
	x=zeros(n+1,2);
	loop 1 to n+1;
		x[#]=fff(t[#];fff,args());
	end;
	x=x';
	keepsquare(1);
	y=heun("fdog",t,start;fff,speed,args());
	setplot(plotarea(x[1]_y[1],x[2]_y[2]));
	color(2); plot(x[1],x[2]);
	color(1); hold on; plot(y[1],y[2]); hold off;
	xplot();
	keepsquare(0);
	return ""
endfunction


