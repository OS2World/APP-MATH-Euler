comment
differential equations
endcomment

function heun (ffunction,t,y0)
## y=heun("f",x,y0;...) solves the differential equation y'=f(x,y).
## f(x,y;...) must be a function.
## y0 is the starting value.
## ffunction may be an expression in x and y.
	n=cols(t);
	y=dup(y0,n);
	if isfunction(ffunction)
	loop 1 to n-1;
		h=t[#+1]-t[#];
		yh=y[#]; xh=t[#];
		k1=ffunction(xh,yh,args());
		k2=ffunction(xh+h/2,yh+h/2*k1,args());
		k3=ffunction(xh+h,yh+2*h*k2-h*k1,args());
		y[#+1]=yh+h/6*(k1+4*k2+k3);
	end;
	else
	loop 1 to n-1;
		h=t[#+1]-t[#];
		yh=y[#]; xh=t[#];
		k1=expreval(ffunction,xh,y=yh);
		k2=expreval(ffunction,xh+h/2,y=yh+h/2*k1);
		k3=expreval(ffunction,xh+h,y=yh+2*h*k2-h*k1);
		y[#+1]=yh+h/6*(k1+4*k2+k3);
	end;
	endif;
	return y';
endfunction

function fdgleval (x,y,expr)
	return evaluate(expr);
endfunction

function runge (ffunction,t,y0,steps=1)
## y=runge("f",x,y0;...) solves the differential equation y'=f(x,y).
## f(x,y;...) must be a function.
## y0 is the starting value.
## ffunction may be an expression in x and y.
## steps are intermediate steps between the t[i].
## It is fastest to use a 1x2 vector t and many steps,
## but will not yield the intermediate values.
## Returns the values y(t).
	y=dup(y0,cols(t));
	if isfunction(ffunction);
		loop 2 to cols(t);
			y[#]=runge1(ffunction,t[#-1],t[#],steps,y[#-1],args());
		end;
	else
		loop 2 to cols(t);
			y[#]=runge1("fdgleval",t[#-1],t[#],steps,y[#-1],ffunction);
		end;
	endif;
	return y';
endfunction

function adaptiverunge (ffunction,t,y0,eps=epsilon(),step=0.1)
## y=adaptiverunge("f",x,y0;...) solves the differential
## equation y'=f(x,y) with adaptive step size.
## f(x,y;...) must be a function.
## y0 is the starting value.
## ffunction may be an expression in x and y.
## steps are intermediate steps between the t[i].
## It is fastest to use a 1x2 vector t and many steps,
## but will not yield the intermediate values.
## Returns the values y(t).
	y=dup(y0,cols(t));
	if isfunction(ffunction);
		loop 2 to cols(t);
			{y[#],step}=runge2(ffunction,t[#-1],t[#],y[#-1],eps,step,args());
		end;
	else
		loop 2 to cols(t);
			{y[#],step}=runge2("fdgleval",t[#-1],t[#],y[#-1],eps,step,ffunction);
		end;
	endif;
	return y';
endfunction

function adaptintf (x,y,fff)
	return fff(x;args());
endfunction

function adaptiveint (ffunction,a,b,eps=epsilon(),steps=10)
## I=adaptiveint("f",a,b;...) returns the integral from a to b.
## f may be an expression in x, which must not contain y.
	if isfunction(ffunction);
		return runge2("adaptintf",a,b,0,eps,(b-a)/steps;ffunction,args());
	else
		return runge2("fdgleval",a,b,0,eps,(b-a)/steps;ffunction);
	endif;
endfunction

