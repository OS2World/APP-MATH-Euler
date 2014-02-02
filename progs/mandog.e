.. Hundekurve

function man(t)
##	return t|-ones(size(t));
	return cos(t)|sin(t);
endfunction

function dgl (t,y)
	delta=man(t)-y;
	norm=sqrt(sum(delta*delta));
	return delta/norm;
endfunction

function mandog (t)
## Compute the dog curve at time t starting from 0.
## The dog starts in 0 and moves with speed 1.
## The mans movement is returned from function man
	return heun("dgl",t,[0,0]);
endfunction

function show (endtime)
	t=linspace(0,endtime,100);
	y=heun("dgl",t,[0,0]);
	m=man(t')';
	xplot(y[1]_m[1],y[2]_m[2]);
	title("Dog and Man");
	wait(180);
	xplot(y[1]-m[1],y[2]-m[2]);
	title("Difference");
	wait(180);
	n=length(y[1]); d=sqrt((y[1,n]-m[1,n])^2+(y[2,n]-m[2,n])^2);
	"Last difference : ", d,
	return plot();
endfunction

show(3);

