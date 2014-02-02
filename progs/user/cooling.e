.. **********************************************************************
.. Application: Newton's Law of Cooling
.. **********************************************************************
.. From the book: M.L. Abell, J.P. Braselton, "Mathematica By Example",
.. Revised Edition, Academic Press, Boston, 1994.
.. Euler implementation by Robert Jurjevic (C) M&R.


.. Newton's Law of Cooling states that the rate at which the temperature
.. T(t) changes in a cooling body is proportional to the difference between
.. the temperature of the body and the constant temperature Ts of the
.. surrounding medium. This situation is represented as the first-order
.. initial value problem
..                          dT/dt = -k*(T-Ts)                         (1)
.. subject to T(0)=T0, where T0 is the initial temperature of the body and
.. k is the constant of proportionality.

.. Example: A pie is removed from a 350 degrees Centigrade oven. In 15
.. minutes, the pie has a temperature of 150 degrees Centigrade. Determine
.. the time required to cool the pie to a temperature of 80 degrees
.. Centigrade so that it may be eaten. The temperature of surrounding
.. air is 75 degrees Centigrade.


.. Solution:

function rk4 (ffunction,t,y0)
## y=rk4("f",t,y0,...) solves the differential equation y'=f(t,y).
## f(t,y,...) must be a function.
## y0 is the starting value.
	n=cols(t);
	y=dup(y0,n);
	loop 1 to n-1;
		h=t[#+1]-t[#];
		yh=y[#]; xh=t[#];
		k1=ffunction(xh,yh,args(4));
		k2=ffunction(xh+h/2,yh+h/2*k1,args(4));
        k3=ffunction(xh+h/2,yh+h/2*k2,args(4));
		k4=ffunction(xh+h,yh+h*k3,args(4));
		y[#+1]=yh+h/6*(k1+2*k2+2*k3+k4);
	end;
	return y';
endfunction
function rhs(t, T, k, Ts)
## calculates right hand side of equation dT/dt = -k*(T-Ts)
          return -k*(T-Ts)
endfunction

function fun1(k, Ts, T0, t0, t1, h)
## calculates and plots solution of equation dT/dt =- k*(T-Ts)
     t = t0:h:t1;
     T = rk4("rhs", t, T0, k, Ts);
     shrinkwindow();
     setplot(0, 50, 50, 350);
     plot(t, T);
     xgrid(0:10:50,1); ygrid(50:50:350,1);
     title("Pie Temperature");
     return T;
endfunction

function fun2(k, Ts, T0, T1, t0, t1, n)
## calculates T(t1)-T1
     h = (t1-t0)/n;
     t = t0:h:t1;
     T = rk4("rhs", t, T0, k, Ts);
     return T[length(t)]-T1;
endfunction

function fun3(t2, k, Ts, T0, T2, t0, n)
## calculates T(t2)-T2 
     h = (t2-t0)/n;
     t = t0:h:t2;
     T = rk4("rhs", t, T0, k, Ts);
     return T[length(t)]-T2;
endfunction

function fun4(k, Ts, T0, t0, t1, n)
## calculates T(t1)
     h = (t1-t0)/n;
     t = t0:h:t1;
     T = rk4("rhs", t, T0, k, Ts);
     return T[length(t)];
endfunction


"Application: Newton's Law of Cooling"

.. The parameters are:
Ts =  75;    .. temperature of surrounding air
T0 = 350;    .. initial pie temperature
t1 =  15;    .. elapsed cooling time when pie temperature becomes T1 
T1 = 150;    .. known pie temperature after t1 minutes of cooling
T2 =  80;    .. eating pie temperature     

.. Constant k can be calculated as a root of function T(t1;k)-T1, where
.. T(t;k) is a solution of initial value problem dT/dt = -k*(T-Ts) subject
.. to T(0)=T0. Note that T is function of t, i.e. T=T(t). T(t;k) means that
.. function T contains constant k. In fact in equation T(t1;k)-T1 = 0
.. function T is considered as function of k, i.e. T=T(k), because t=t1 is
.. known constant.
"calculating  k (solution from book is: k=0.0866189)"
k  = secant("fun2", 0, 0.1, Ts, T0, T1, 0, t1, 15);
printf("k = %10.7f", k)

.. Elapsed cooling time t2, when pie temperature becomes T2, can be 
.. calculated as a root of function T(t2)-T2 = 0, where T(t) is a solution
.. of initial value problem dT/dt = -k*(T-Ts) subject to T(0)=T0. 
"calculating t2 (solution from book is: t2=46.264)"
T3 = fun4(k, Ts, T0, 0, 40, 25);     ... T3 is used to speed up calculation
t2 = secant("fun3", 45, 50, k, Ts, T3, T2, 40, 25);
printf("t2= %10.3f", t2)

"calculating and plotting the graph"
hold off
fun1(k, Ts, T0, 0, 50, 0.5);         .. pie temperature versus time graph
hold on;
markerstyle("m<>"); mark(t1, T1);    .. mark (t1,T1) known point 
markerstyle("m*"); mark(t2, T2);     .. mark (t2,T2) calculated point

"done"

.. **********************************************************************
.. **********************************************************************
