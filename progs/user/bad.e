.. This program starts automatically.

cls;

""
"                       BirthAndDeath"
""

comment

This program simulates any birth and death process with 
birthrate, deathrate and population on start time as parameters.

Written by bjoern boening & helge nordmann   -   1996.06.05

endcomment

wait(15);
cls;

function okok(delay=90)
	""
	"                             >>   Please press [Enter]"
	return wait(delay);
endfunction

function rnd
##	generate a random number
	u=random(1,1);
##	be sure 'u' is in ]0,1[
	if (u==0 || u==1); u=rnd(); endif;
	return u;
endfunction

function deltatime(bi)
##	the time changing algorithm
	u=rnd();
	z=-log(1-u)/bi;
	return z;
endfunction

function deltapop(lambda, mue)
##	the population growing / shrinking algorithm
	s=-1;
	u=rnd();
	pPlus=lambda / (lambda + mue);
	if (u<pPlus); s=1; endif;
	return s;
endfunction

function binomi(p0, n, r)
##	binomial density function
	p=bin(n, r) * p0 ^r * (1 -p0) ^(n -r);
	return p;
endfunction

function binomidistr(p0, n, r)
##	binomial distribution
	sp=0;
	for i=0 to r;
		p=binomi(p0, n, i);
		sp=sp +p;
	end;
	return sp;
endfunction

function numberof
##	user input
	cls;
	""
	"How many times should the simulation be repeated"
	""
	"(Try   nrloop=5)"
	""
	nrloop=input("nrloop");
	if (nrloop<=0); nrloop=numberof(); endif;
	return nrloop;
endfunction

function birthequation
##	user input
	""
	"Please enter a constant or a numerical expression or a function of"
	"xi ; ti ; i  (i.e.: actual state ; actual time ; loop index)"
	"to be used to calculate the actual birth rate 'lambda'"
	""
	"(Try   lambda=0.5   or   lambda=0.5 * xi + 0.2)"
	""
	lambdatext=lineinput("lambda=");
	return lambdatext; 
endfunction

function deathequation
##	user input
	""
	"Please enter a constant or a numerical expression or a function of"
	"xi ; ti ; i  (i.e.: actual state ; actual time ; loop index)"
	"to be used to calculate the actual death rate 'mue'"
	""
	"(Try   mue=0.4   or   mue=0.5 * xi)"
	""
	muetext=lineinput("mue=");
	return muetext; 
endfunction

function startvalue
##	user input
	""
	"Please enter a non negative integer as a startvalue for the simulation"
	" - 'population' at start time"
	""
	x0=input("x0=");
	if (x0<0); x0=startvalue(); endif;
	return x0; 
endfunction

function graph(t, x, nrloop)
##	printing on screen the simulation graphics
	hold off;
	tmaxv=max(t);
	tmax=max((tmaxv)');
	xmaxv=max(x);
	xmax=max((xmaxv)');
	xminv=min(x);
	xmin=min((xminv)');
	setplot(0, tmax, 0.9 * xmin, 1.1 * xmax);
	xplot(t, x); ctext("time", [500, 950]); vctext("p o p u l a t i o n", [20, 500]);
	title("population");wait(360);
	return "";
endfunction

function graphst(t, xm, xs)
##	printing on screen the statistic graphics
	tmax=max(t);
	xmax=max(xm+xs);
	xmin=min(xm-xs);
	if (xmin<0); xmin=0; endif;
	setplot(0, tmax, 0.9 * xmin, 1.1 * xmax);
	xstat=xm-xs;
##	be sure the graph is inside the window
	if (xmin<=0); 
		for i=1 to cols(xstat)*rows(xstat);
			if (xstat{i}<0); xstat{i}=0; endif;
		end;
	endif;
	color(3);
	xplot(t, xstat);
	hold on;
	xstat=xm+xs;
	plot(t, xstat);
	color(2);
	xstat=xm;
	plot(t, xstat);  ctext("time", [500, 950]); vctext("p o p u l a t i o n", [20, 500]);
	title("mean value ± standard deviation"); wait(360);
	return "";
endfunction

function bindistrgraph(r, p)
	hold off;
	color(2);
	markerstyle("m+");
	xmark(r, p);  ctext("population", [500, 950]); vctext("p r o b a b i l i t y", [20, 500]);
	title("probability of 'less than or equal' population (binomial distr)"); wait(360);
	return 0;
endfunction

function normdistrgraph(xmm, xss)
	ug=xmm -3 *xss;
	og=xmm +3 *xss;
	ug=round( ug, 0);
	og=round( og, 0);
	if ( ug<0 ); ug=0; endif;
	step=1;

	## 'x' are integers, but
	if ( (og -ug)>500 ); step= (og -ug) / 500; endif;	
	## no higher precision needed
	x=ug:step:og;
	hold off;
	setplot( ug, og, 0, 1);
	p=normaldis((x-xmm)/xss);
	color(2);
	xplot(x, p);  ctext("population", [500, 950]); vctext("p r o b a b i l i t y", [20, 500]);
	title("probability of 'less than or equal' population"); wait(360);
	return 0;
endfunction

function simulation(lambdatext, muetext, ti, xi, i)
##	the simulation algorithm

##	calculate the new birth- and death rate
	lambda=interpret(lambdatext);
	mue=interpret(muetext);

##	legal values ?
	if (xi<0); lambda=0; endif; 
## xi=0 may be allowed in some simulations
	if (xi<=0); mue=0; endif; 
## all dead
	bi= lambda + mue;
	if (bi<=0); return { 1. * ti, xi}; endif;
		## no more changes 	in population

##	calculate the new time and population and put it out
	ti=ti+deltatime(bi);
	xi=xi+deltapop(lambda, mue);
	return {ti, xi};
endfunction

function simustat(nrloop, nrstep, t, x)
##	calculate the mean value for every simulation and all simulation 
##	steps

##	declare & initialize the help variables
	ssx=0;
	ssxq=0;

##	declare & initialize the vectors
	st=zeros(1, nrstep);
	sx=zeros(1, nrstep);
	sxq=zeros(1, nrstep);
	sxx=zeros(1, nrstep);

##	setting the helpvariables
	for i=1 to nrstep;

		for nr=1 to nrloop;
		st{i}=st{i}+t[nr, i];
		sx{i}=sx{i}+x[nr, i];
		sxq{i}=sxq{i}+x[nr, i] * x[nr, i];
		ssx=ssx+x[nr, i];
		ssxq=ssxq+x[nr, i]*x[nr, i];
		end;

		sxx{i}=sxq{i} - sx{i} * sx{i} / nrloop;
	end;
	ssxx=ssxq - ssx *ssx / (nrstep * nrloop);

##	now create 3 functions giving the range of all the processes
	tm=st / nrloop;
	xm=sx / nrloop;
	xs=sqrt( sxx / nrloop);

##	print on screen the results of the statistics
	if (nrloop>1); graphst(tm, xm, xs), endif;

##	calculate the mean value and the standard distribution for all simulations
##	and all the steps
	xmm=ssx / (nrstep * nrloop);
	xss=sqrt( ssxx / (nrstep * nrloop) );

##	the distribution is discrete, in case of small population we use the
##  binomial distrib.
	p0=1 - xss / xmm;
	n=xmm * xmm / (xmm -xss);
	n=round(n, 0);
## 'n' must be an integer

##	binomial distrib. is allowed
	if (n<=60);
		ug=xmm -1.5 * xss;
		og=xmm +1.5 * xss;
		if (ug<=0); ug=0; endif;
		ug=round(ug, 0);
		og=round(og, 0);
		rlength= og -ug;
		p=zeros(1, rlength+1);
		for i=0 to rlength+1;
			r=ug + i;
			p{i}=binomidistr(p0, n, r);
		end;

##	calling the graphic function to show the binomial distr.
		r=ug:og;
		bindistrgraph(r, p);

##	alternatively on larger population we use the normal distribution (built in)
	else;
##	calling the graphic function to show the distribution
		normdistrgraph(xmm, xss);
	endif;

##	telling the user the mean val. & stand. distr.
	""
	"Over a time "  tm[1, nrstep]
	"the population is" xmm 
	"with a standard deviation" xss
	""
	okok();
	return 0;
endfunction

function continue
##	give the user a chance to do the simulation as many times as needed
	repeat;
		cls;
		""
		"Do You want to do another simulation ?"
		""
		"Yes: Y   or   No: N"
		""
		again=lineinput("Y / N ");
		if ( ( again=="Y" ) || ( again=="y" ) ); 
			mainBaD();
		else;
			break;
		endif;
	end;
	return 0;
endfunction

function mainBaD
##	main program - BaD: BirthAndDeath

##	nrstep: steps to calculate for one simulation (precision of screen window)
	nrstep=500;

##	get the parameters from user
	nrloop=numberof();
	lambdatext=birthequation();
	muetext=deathequation();
	x0=startvalue();

##	declare the matrices
	t=zeros(nrloop,nrstep);
	x=zeros(nrloop,nrstep);

##	informing the user about the process
	""
	"now calculating ..."

##	simulate it nrloop times
	for nr=1 to nrloop;
		ti=0;
		xi=x0;

##	one simulation
		for i=1 to nrstep;
			{ti, xi}=simulation(lambdatext, muetext, ti, xi, i);
			t[nr, i]=ti;
			x[nr, i]=xi;
		end;
	end;

##	print on screen the results of the simulation
	shrinkwindow();
	graph(t, x, nrloop);

##	doing some statistics
	""
	"doing some statistics ..."

##	calling the statistics function
	simustat(nrloop, nrstep, t, x);

	return 0;
endfunction



comment

Used variables:

i                  index, step number of simulation
x0                begin process state 'population'
xi                actual process state 'population'
ti                 actual time (start time is 0)
lambda           birth rate at any time
mue              death rate at any time
lambdatext       string holding an equation to calculate lambda
muetext          string holding an equation to calculate mue

and some other variables in local processes

endcomment

okok();
cls;

comment

The algorithm:

With birth rate 'lambda', death rate 'mue' and a random number 'u'
u in the open interval ]0,1[ 
we generate a (positive) 'deltatime'
deltatime = - log ( 1 - u ) / ( lambda + mue ) ,
after this time a change in population 'deltapop' will be made
with a probability 'pIndex' of
deltapop=+1   :   pPlus = lambda / ( lambda + mue )
deltapop=-1   :   pMinus = mue / ( lambda + mue )   .

Then we repeat ...        (500 times)
endcomment

okok();
mainBaD();
continue();

