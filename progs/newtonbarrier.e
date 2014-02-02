A=[-1,0;0,-1;1,0;0,1;1,1];
b=[0;0;1;1;1.5];
c=[1,2];

function barrier (x,A,b,a=10)
	return sum(exp(a*(A.x-b))');
endfunction

function fbarrier (x,a)
	global A,b,c;
	return barrier(x',A,b,a)-c.x';
endfunction

function fbarrierlog (x,y,A,b)
	return log(barrier(x',A,b));
endfunction

function barriertest (a)
	global A,b,c;
	return nelder("fbarrier",[0,0],0.1,0.0001;a);
endfunction
