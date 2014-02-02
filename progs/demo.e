.. The Demo. Starts automatically.

comment
Demo loading.
endcomment

demodelay=180;
reset();

function delay
	global demodelay;
	return demodelay
endfunction

function warten
	wait(delay());
	return 0;
endfunction

function weiter
	"" "<< Press space to continue >>" warten(); cls;
	return 120;
endfunction

function pause
	"" "<< Press space to continue >>", warten(); "",
	return 120;
endfunction

function taste
	"" "<< Please space return to see graphics >>",
	return warten();
endfunction

function normaldemo
## Einfuehrung in EULER
	global Pi; pi=Pi;
	shrinkwindow;

	cls
	""
	"                             Welcome!" 
	""
	"EULER has been designed to ease numerical computation."
	"Most of the time one has a tiny problem, to tiny to use a compiler"
	"language."
	""
	"Besides, one may want to play with intermediate results, see a"
	"graphical representation, or simply have readily available"
	"numerical functions."
	"" 
	"All this can be done with EULER."
	"It is like using a numerical laboratory."
	pause();
	
	"But EULER is not a symbolic algebra program. It uses numbers with"
	"finite accuracy as supplied by the C compiler used. It is therefore"
	"faster than a symbolic program, like MATHEMATICA."
	""
	"However, you can do symbolic computation in Euler thanks to Yacas,"
	"a public domain computer algebra system."
	""
	"All commands are entered by the keyboard. Old commands can be"
	"recalled with cursor up or down. ESC interrupts a computation,"
	"and HELP shows the graphics screen."
	weiter();
	
## Eingabe von Matrizen
	"With EULER one can enter matrices like this:" ""
	">x=[5,6,7]" x=[5,6,7], 
	">A=[1,2,3;5,6,7;8,9,0]" A=[1 2 3;5 6 7;8 9 0],
	"The comma may be omitted."
	"These matrices are stored on the variables x and A."
	pause();
	
	"All computing is done element for element."
	""
	">x*x" x*x,
	">A+A" A+A,
	weiter();
	
## Complexe Zahlen.
	"Complex numbers and complex computations."
	""
	">1+2i", 1+2i,
	">c=[1i 3 4+5i]", c=[1i 3 4+5i],
	"" ">1i*1i", 1i*1i,
	">exp(1i*pi/2)", exp(1i*pi/2),
	weiter();

## Matrixprodukt.
	"The dot multiplies two matrices."
	""
	">A.x'" A.x'
	"Note, that x had to be transposed!"
	"" ">x", x,
	"" ">x'", x',
	"" ">x'.x" x'.x,
	weiter();
	
## :
	"Generating vectors:"
	""
	">1:10", 1:10,
	">-1:0.3:1", -1:0.3:1,
	pause();
	
	">[1 2 3]|[4 5]", [1 2 3]|[4 5],
	">[1 2 3]_[4 5 8]", [1 2 3]_[4 5 8],
	">dup([1 2 3],4)", dup([1 2 3],4),
	weiter();
	
## Funktionen.
	"Also all functions are applied to all elements of a matrix."
	"" ">sqrt([1,2,3,4])" sqrt([1,2,3,4]),
	""
	"There are all the numerical functions."
	"See a list of all functions with >list."
	pause();
	
	"It is easy to generate a table for a function, and"
	"a table is one way to plot a function in Euler."
	"" ">t=0:pi/50:2*pi; s=sin(t); xplot(t,s);"
	t=0:pi/50:2*pi; s=sin(t);
	"plots the sine function in [0,2*Pi]",
	''You could have used fplot("sin(x)",0,2*pi) with the same result'',
	taste();
	shrinkwindow(); xplot(t,s); title("Please press return!");
	warten(); cls;

## Intervalle.
	"Interval arithmetic."
	""
	"You can use intervals instead of real numbers."
	">exp(~1.1,1.2~)", exp(~1.1,1.2~)
	"The result the interval of all exp(t), 1.1<=t<=1.2",
	"" "Thus, you can make verified computations:"
	">x=~0.8~; 1+x+x^2/2+x^3/6+x^4/24+x^5/120+x^6/720*exp(~0~ || x),",
	x=~0.8~; 1+x+x^2/2+x^3/6+x^4/24+x^5/120+x^6/720*exp(~0~ || x),
	"This is a guaranteed inclusion of the exp with its series",
	"including the error term."
	"|| computes the union of two intervals."
	"" "Compare:"
	">exp(x)", exp(x),
	pause();
	"To get a better approximation, we can use"
	">n=0:20; sum(x^n/fak(n))+x^21/fak(21)*exp(~0~ || x)"
	n=0:20; sum(x^n/fak(n))+x^21/fak(21)*exp(~0~ || x)
	weiter();
	
## Untermatrizen.
	"Submatrices:" ""
	">x=[11 12 13;21 22 23; 31 32 33]"
	x=[11 12 13;21 22 23; 31 32 33],
	">x(1)" x(1),
	"One can write x[...] or x(...).",
	pause();
	
	">x[1:2,2]", x[1:2,2],
	">x[2:6,2:6]" x[2:6,2:6],
	">x[:,3]", x[:,3],
	">x[1,[2 2 1 3]]", x[1,[2 2 1 3]],
	weiter();
	
## Numerische Integration.
	"An example:" ""
	">sum(sin((1:100)/100))" sum(sin((1:100)/100)),
	"" 
	"Let us integrate the sine in [0,pi] using the simpson method (but"
	"not the builtin functions). The exact value is 2."
	"You will get the correct result with the builtin Romberg method"
	''>romberg("sin(x)",0,pi)''
	romberg("sin(x)",0,pi)
	"However, we wish to demonstrate the vector arithmetic and do"
	"the Simpson method the hard way."
	">t=0:pi/50:pi; n=length(t);", 
	t=0:pi/50:pi; n=length(t);
	">f=2*mod((1:n)+1,2)+2; f[1]=1; f[n]=1;",
	f=2*mod((1:n)+1,2)+2; f[1]=1; f[n]=1;
	">longformat(); result=sum(sin(t)*f)/3*(t[2]-t[1])", 
	longformat(); result=sum(sin(t)*f)/3*(t[2]-t[1]),
	">shortformat();"
	shortformat();
	"" "; surpresses output of intermediate results."
	pause();
	
	"By the way:"
	">f[1:5]", f[1:5],
	">f[n-4:n]", f[n-4:n],
	weiter();
	
## Gleichungssysteme.
	"Solve linear systems:"
	"" ">a=hilbert(10); b=a.dup(1,10); a\b" ""
	longformat(); a=hilbert(10); b=a.dup(1,10); a\b,
	""
	"hilbert(n) generates the badly conditioned Hilbert matrix."
	"The inaccuracy is unavoidable for the 10x10 Hilbert matrix."
	pause();
	
	"We can get rid of the error using residuum iteration."
	">xlgs(a,b)",
	xlgs(a,b),
	weiter();

## Inklusion der L”sung.
	"Let us make an inclusion of the solution"
	"The algorithm for this may be found in papers of"
	"Rump, Alefeld, and Herzberg."
	""
	">ilgs(a,b),"
	ilgs(a,b),
	pause();
	
	"By the way, we can also output fractions"
	""
	">fracformat(5,epsilon); hilbert(10)/hilberftactor"
	global hilbertfactor;
	fracformat(5,epsilon); hilbert(10)/hilbertfactor,
	">shortformat;"
	shortformat;
	weiter();

## Kurvendiskussion.
	"A discussion" ""
	"The function:"
	">x=1:0.02:10; y=log(x)/x;",
	x=1:0.02:10; y=log(x)/x;
	"xplot(x,y)",
	taste();
	shrinkwindow();
	xplot(x,y); ygrid(0); title("(return)"); warten();
	"" "The maximal value is in " ''>fmax("log(x)/x",1,10)'',
	fmax("log(x)/x",1,10),
	"" "The first derivative (numerically)"
	''>a=dif("log(x)/x",x);''
	''xplot(x,a); ygrid(0); title ("(return)");''
	taste();
	a=dif("log(x)/x",x);
	xplot(x,a); ygrid(0); title ("(return)");
	warten();
	
	weiter();

	return 0
endfunction

function fisch
## plots a fish
	global Pi;
	t=0:3;
	ph=interp(t,[0.05,0.3,0.2,1.2]);
	pb=interp(t,[0.05,0.5,0.2,0.2]);
	phi=Pi-linspace(0,2*Pi,20);
	l=linspace(0,3,40);
	psin=dup(sin(phi),length(l));
	pcos=dup(cos(phi),length(l));
	lh=dup(interpval(t,ph,l),length(phi))';
	lb=dup(interpval(t,pb,l),length(phi))';
	x=pcos*lb; z=psin*lh; y=dup(l-1.5,length(phi))';
	view(2.5,1.5,0.3,0.1);
	solid(x,y,z);
	title("A solid fish (return)"); warten();
	huecolor(1); solidhue(x,y,z,-y,1);
	title("A shaded fish (return)"); warten();
	return 0;
endfunction

function mindemo ()
	"Next we find the local minima of sin(x*x) in [0,5]."
	""
	"The first method is Brent's."
	''fplot("sin(x*x)",0,5); title("sin(x*x) (return)"); weiter();''
	''brentmin("sin(x*x)",2), brentmin("sin(x*x)",3),''
	brentmin("sin(x*x)",2), brentmin("sin(x*x)",3),
	''brentmin("sin(x*x)",4), brentmin("sin(x*x)",5),''
	brentmin("sin(x*x)",4), brentmin("sin(x*x)",5),
	taste();
	fplot("sin(x*x)",0,5); title("sin(x*x) (return)"); warten(); cls;
	""
	"The next method is the golden ratio method."
	''fmax("sin(x*x)",0,2),''
	fmax("sin(x*x)",0,2),
	"There is also a function finding all minima and maxima."
	''{mi,ma}=fextrema("sin(x*x)",0,5); mi, ma,''
	{mi,ma}=fextrema("sin(x*x)",0,5); mi, ma,
	weiter();

	"Next we minimize a function of two variables."
	""
	"Let us first plot the function."
	''fcd("sin(x)*x+cos(y)*y",50,0,10,0,10); xplot(); weiter();''
	taste();
	fcd("sin(x)*x+cos(y)*y",50,0,10,0,10); xplot(); warten();
	""
	"Now we compute a minimum and mark it in the plot"
	""
	''mi=neldermin("sin(x[1])*x[1]+cos(x[2])*x[2]",[5,5]);''
	''color(10); hold on; mark(mi[1],mi[2]); hold off; color(1);''
	taste();
	mi=neldermin("sin(x[1])*x[1]+cos(x[2])*x[2]",[5,5]);
	color(10); hold on; mark(mi[1],mi[2]); hold off; color(1);
	warten();
	''mi=neldermin("sin(x[1])*x[1]+cos(x[2])*x[2]",[5,8]);''
	taste();
	mi=neldermin("sin(x[1])*x[1]+cos(x[2])*x[2]",[5,8]);
	color(10); hold on; mark(mi[1],mi[2]); hold off; color(1);
	warten(); cls;
	return "";
endfunction

function grafikdemo
## grafikdemo shows the grafik capabilities of EULER
	fullwindow(); hold off;
	global Pi;
	cls
	""
	"Demo of EULER's graphics." ""
	"These graphics have been programmed with a few lines."
	"Study the demo file for more information!"
	taste();
	clg;
## Abbildungen von R nach R
	t=-Pi:Pi/20:Pi;
	shrinkwindow(); setplot(-Pi,Pi,-2,2);
	linewidth(4); plot(t,sin(t)); linewidth(1); hold on;
	color(2); style("."); plot(t,cos(t)); 
	color(3); style("--"); plot(t,(sin(t)+cos(t))/2);
	xgrid(-3:1:3,1); ygrid(-1:1,1);
	title("2D plots (return)");
	color(1); style(""); fullwindow(); hold off;
	warten();
	clg;
## Abbildungen von R nach R^2 in vier Fenstern
	t=-Pi:Pi/50:Pi;
	window(10,textheight()*1.5,490,490);
	plot(sin(3*t)/3+cos(t),sin(3*t)/3+sin(t)); hold on;
	title("Parametric curves (return)"); 
	window(510,textheight()*1.5,1010,490); frame();
	plot(sin(2*t)/3+cos(t),sin(2*t)/3+sin(t));
	window(10,510,490,1010); frame();
	plot(sin(2*t),sin(3*t));
	window(510,510,1010,1010); frame();
	t=t+2*Pi;
	plot(cos(t)*log(t),sin(t)*log(t));
	hold off;
	warten();
	shrinkwindow();
	
## Kosinus und Potenzreihen
	t=linspace(0,4*pi,500);
	s=(1-t^2/2);
	for i=4 to 30 step 2;
	s=s_(s[rows(s),:]+(-1)^(i/2)*t^i/fak(i));
	end
	setplot(min(t),max(t),-max(t)/2,max(t)/2); xplot(t,s);
	hold on; linewidth(3); plot(t,cos(t)); linewidth(1); hold off;
	title("Cosine and its truncated series");
	warten();

## Bar plots
	r=normal(1,100);
	c=count(r+5,10); c=c/sum(c);
	setplot(-5,5,0,0.5); clg;
	b=barcolor(1); xplotbar(-5:4,0,1,c,"\/"); barcolor(b);
	t=linspace(-5,5,300);
	hold on;plot(t,exp(-t^2/2)/sqrt(2*pi));hold off;
	title("A statistical experiment");
	style("");
	warten();

## x^2+y^3 als mesh und contour.
	fullwindow();
	x=pi*(-1:0.1:1); y=x';
	mesh(x*x+y*y*y); title("x^2+y^3 (return)"); warten();
	contour(x*x+y*y*y,-30:5:30);
	title("Contour plot of this function (return)"); warten();
	x=(-1:0.04:1)*pi; y=x';
	huecolor(1); density(sin(2*x)*cos(2*y),1);
	title("Density plot of a function (return)"); warten();
	huecolor(0); density(sin(2*x)*cos(2*y),1);
	title("The same in color (return)"); huecolor(1); warten();

## e^-r cos(r) als Drahtmodell.
	x=pi*(-1:0.1:1); y=x';
	r=x*x+y*y; f=exp(-r)*cos(r);
	view(2,1,0,0.5); ## [ Abstand, Zoom, Winkel rechts, Winkel hoch ]
	wire(x/Pi,y/Pi,f);
    title("Wire frame model (return)"); warten();
	meshbar(f);
	title("Bar plot of this function (return)"); warten();
	meshbar(-f);
	title("Negative bar plot (return)"); warten();

## Der Fisch als solides Modell.
	fisch();
	view(2,1,0.2,0.3);
	clg;
	shrinkwindow();
## Eine complexe Abbildung w=z^1.3
	setplot(-1, 2, -0.5, 1.5);
	t=0:0.1:1; z=dup(t,length(t)); z=z+1i*z';
	color(1); style("."); cplot(z); hold on;
	color(2); style(""); cplot(z^1.3); color(1);
	xgrid(0); ygrid(0);
	title("Complex numbers w=z^1.3"); hold off;
	warten();

	"End"
	shrinkwindow;
	return 0
endfunction

.. *** MatLab - Demo ***

function hump(x)
	return 1 / ((x-0.3)^2 + 0.01) + 1 / ((x-0.9)^2 + 0.04) - 6;
endfunction

function hump1(x)
	return -(2*(x-0.3)) / ((x-0.3)^2 + 0.01)^2 ..
		+ -(2*(x-0.9)) / ((x-0.9)^2 + 0.04)^2 ;
endfunction

function humpdemo
	cls
	""
	"Look at the following function:"
	type hump
	''>fplot("hump",0,2)''
	taste();
	fplot("hump",0,2);
	title("Please press return!"); warten();
	""
	"Let us integrate it, using the gauss method",
	''>gauss("hump",0,1,10),''
	gauss("hump",0,1,10),
	"And compare with the romberg integral."
	''>romberg("hump",0,1),''
	romberg("hump",0,1),
	weiter();
	"To find the zero near 1.2, we use three different methods."
	''secant("hump",1.2,1.4),''
	secant("hump",1.2,1.4),
	''bisect("hump",1.2,1.4),''
	bisect("hump",1.2,1.4),
	''inewton("hump","hump1",~1.2,1.4~),''
	inewton("hump","hump1",~1.2,1.4~),
	"The last method provides a fast inclusion of the result,"
	"but it needs the derivative of the function:"
	""
	"function hump1(x)"
	"	return -(2*(x-0.3)) / ((x-0.3)^2 + 0.01)^2 .."
	"		+ -(2*(x-0.9)) / ((x-0.9)^2 + 0.04)^2 ;"
	"endfunction"
	weiter();
	return ""
endfunction

function fftdemo

## Fourierreihe.
	cls; shrinkwindow();
	""
	"Discrete Fourier series:"
	"We generate a Fourier sum by chopping off the coefficients"
	"of the Fourier transform of a rectangle function."
	""
	">n=128; t=(0:n-1)*2*pi/n; s=(t<pi); w=ifft(s);"
	n=128; t=(0:n-1)*2*pi/n; s=(t<pi); w=ifft(s);
	">k=10; w[k+2:n-k]=zeros(1,n-2*k-1); s1=re(fft(w));"
	taste();
	for k=10 to 40 step 10;
	w1=w;
	w1[k+2:n-k]=zeros(1,n-2*k-1); s1=re(fft(w1));
	setplot(0, 2*pi, -0.5, 1.5);
	xplot(t,s); hold on;
	color(3); plot(t,s1);
	color(1);
	hold off;
	title("(return)");
	warten();
	end;
	cls;

## Fourieranalyse.
	"Fourier analysis"
	""
	"The signal:"
	">t=(0:127)*2*pi/128; x=sin(10*t)+2*cos(15*t);"
	t=(0:127)*2*pi/128; x=sin(10*t)+2*cos(15*t);
	"" "Add some noise:"
	">y=x+normal(size(x));"
	taste();
	y=x+normal(size(x));
	setplot(0,2*pi,-5, 5);
	plot(t,x); hold on;
	color(3); plot(t,y); color(1);
	title("Signal und signal with noise (return)");
	hold off;
	warten();
	""
	"Now the Fourier analysis:"
	""
	">c=fft(y); p=abs(c)^2;"
	taste();
	c=fft(y); p=abs(c);
	plot(0:64,p(1:65)); xgrid([10,15]);
	title("Spectral analysis (return)"); warten(); cls;

	"A two dimensional edge detector is folded", "with a signal matrix."
	""
	"First we generate the signal"
	">i=-32:31; j=i'; A=(i^2+j^2<200); density(A,0.99);"
	taste();
	i=-32:31; j=i'; A=(i^2+j^2<200); density(A,0.99);
	title("Signal"); warten();
	""
	"Then we fold it with E."
	">E=[-1,1,-1;1,0,1;-1,1,-1];"
	">B=zeros(64,64); B[[64,1,2],[64,1,2]]=E;"
	">density(re(ifft(fft(A)*fft(B))));"
	taste();
	E=[-1,1,-1;1,0,1;-1,1,-1];
	B=zeros(64,64); B[[64,1,2],[64,1,2]]=E;
	density(re(ifft(fft(A)*fft(B))));
	title("The edges"); warten(); cls;
	return 0;
endfunction

.. *** Approximations - Demo ***

function apprdemo
	cls;
	""
## L_2-Approximation.
	"Some approximation problems."
	"" "First the x-values and the function values are computed:"
	"" ">x=0:0.005:1; y=sqrt(x);", x=0:0.005:1; y=sqrt(x);
	"" "Then the matrix of polynomial values:"
	"" ">n=length(x); p=polyfit(x,y,5),",
	n=length(x); p=polyfit(x,y,5),
	""
	">plot(x,y); hold on; color(3); "| ..
	"plot(x,polyval(p,x)); color(1); hold off;"
	taste();
	plot(x,y); hold on; color(3);
	plot(x,polyval(p,x)); color(1); hold off;
	title("sqrt(x) and its best L_2-approximation (return)"); warten();
	xplot(x,y-polyval(p,x)); ygrid(0); title("The error (return)"); warten();
	""
	"The maximal error in this interval is"
	"" ">max(abs(y-polyval(p,x)))", max(abs(y-polyval(p,x))),
	weiter();

## Interpolation.
	"Interpolation:" ""
	">t=equispace(0,1,5); s=sin(t); d=interp(t,s);",
	t=equispace(0,1,5); s=sin(t); d=interp(t,s);
	"" ">x=0:0.01:1; max(x,abs(interpval(t,d,x)-sin(x)))",
	x=0:0.01:1; max(abs(interpval(t,d,x)-sin(x))),
	"" ">xplot(x,interpval(t,d,x)-sin(x)); ygrid(0); ",
	taste();
	xplot(x,interpval(t,d,x)-sin(x)); ygrid(0);
	title("Interpolation error (return)"); warten(); cls;

## Optimierung
	"Optimization"
	""
	"Let us first generate a convex set"
	t=1.1:0.1:5; t=flipx(1/t)|1|t|0.2; s=1/t;
	"t=1.1:0.1:5; t=flipx(1/t)|1|t|0.2; s=1/t;"
	"setplot(0,6,0,6); xplot(t,s);"
	taste();
	setplot(0,6,0,6); xplot(t,s); warten();
	""
	"Then we minimize 2*x+y on this set."
	z=t+1i*s; r=cols(z); d=-1i*(z[2:r]-z[1:r-1]);
	A=re(d')|im(d'); b=re(z[2:r]*conj(d))'; c=[2,1];
	"z=t+1i*s; r=cols(z); d=-1i*(z[2:r]-z[1:r-1]);"
	"A=re(d')|im(d'); b=re(z[2:r]*conj(d))'; c=[2,1];"
	"x=simplex(A,b,c);"
	x=simplex(A,b,c),
	"mark(x[1],x[2]); plot([x[1]-1,x[1]+1],[x[2]+2,x[2]-2]);"
	taste();
	hold on;
	mark(x[1],x[2]); plot([x[1]-1,x[1]+1],[x[2]+2,x[2]-2]);
	hold off;
	title("Optimum, Press Return");
	warten(); cls;

	mindemo();

	return 0;

endfunction

.. *** Statistik - Demo ***

function statdemo
## Statistikdemo.
	global Pi; pi=Pi;
	
## t-Test.
	cls
	"Let us generate 10 simulated measurements:"
	"" ">x=1000+5*normal(1, 10)",
	x=1000+5*normal(1, 10),
	"" "Mean value:" ">m=mean(x)", m=mean(x),
	"or" ">m=sum(x)/length(x)",
	"" "Standart deviation:", ">d=dev(x),"
	d=dev(x),
	"or" ">d=sqrt(sum((x-m)^2/(length(x)-1))"
	pause();
	
	"A 95-% confidence interval for the mean value:"
	"" ">t=invtdis(0.975,9)*d/3; [m-t,m+t]",
	t=invtdis(0.975,9)*d/3; [m-t,m+t],
	weiter();
	
## xhi^2
	"Statistical evaluation of dice throws."
	""
	">x=random(1, 600); t=count(x*6,6)'",
	x=random(1, 600); t=count(x*6,6),
	"Compute chi^2" ">chi=sum((t-100)^2/100),"
	chi=sum((t-100)^2/100),
	"We decide that the dice is unbiased, since"
	">1-chidis(chi,5)" 1-chidis(chi,5),
	pause();
	"Now we simulate a biased dice:"
	">x=(x<0.95)*x; t=count(x*6,6), chi=sum((t-100)^2/100);"
	x=(x<0.95)*x; t=count(x*6,6), chi=sum((t-100)^2/100);
	"Error probability:"
	">1-chidis(chi,5)"
	1-chidis(chi,5),
	weiter();
	
## Binomialverteilung
	"The binomial distribution:"
	"" "First we compute the binomial distribution values." 
	">n=20; t=0:n; p=0.4; b=p^t*(1-p)^(n-t)*bin(n,t);"
	">color(3); xmark(t,b); color(1);"
	n=20; t=0:n; p=0.4; b=p^t*(1-p)^(n-t)*bin(n,t);
	"" "Then we compute a normal distribution, which fits."
	">d=sqrt(n*p*(1-p));"
	">s=exp(-((t-20*p)/d)^2/2)/(d*sqrt(2*pi));"
	">hold on; xplot(t,s); hold off;"
	taste();	
	color(3); style("m[]"); xmark(t,b); color(1);
	d=sqrt(n*p*(1-p));
	s=exp(-((t-20*p)/d)^2/2)/(d*sqrt(2*pi));
	hold on; plot(t,s);
	title("Binomial and normal distribution (press return)"); hold off;
	warten();
	
	""
	"Of course, there are functions to handle this faster"
	"like the cumulative binomial sum and its inverse."
	">t=0:10; t'|binsum(t,10,0.5)';"
	(0:10)'|binsum(0:10,10,0.5)',
	weiter();

## Mehrfeldertest
	"chi^2 test:"
	""
	"Given the matrix:"
	">a"
	format(5,0);
	a=[23,37,43,52,67,74;45,25,53,40,60,83];
	a, shortformat();
	""
	"The matrix of expected values:"
	">{k,l}=field(sum(a')',sum(a)'); s=totalsum(a); b=(k*l)/s;"
	{k,l}=field(sum(a')',sum(a)'); s=totalsum(a); b=(k*l)/s,
	pause();
	"Compute chi^2:"
	">chi=totalsum((a-b)^2/b); 1-chidis(chi,5),"
	chi=totalsum((a-b)^2/b); 1-chidis(chi,5),
	""
	"or shorter"
	">chi=tabletest(a);"
	chi=tabletest(a)
	"" "That means we decide that the columns do not depend of the rows."
	weiter();

## Regression.
	"Regression analysis:" ""
	"A linear function with errors: "
	"" ">t=0:0.2:5; s=2*t+3+0.2*normal(size(t));",
	t=0:0.2:5; s=2*t+3+0.2*normal(size(t));
	">p=polyfit(t,s,1),",
	p=polyfit(t,s,1),
	">color(3); xmark(t,s);"
	">hold on; color(1); plot(t,polyval(p,t)); hold off;"
	taste();
	color(3); xmark(t,s); 
	hold on; color(1); plot(t,polyval(p,t)); hold off;
	title("The best fit (press return)"); warten();
	
	weiter();
	
	return 0;
endfunction

function fibo(n)
## fibo(n) liefert einen Vektor mit den ersten n Finonaccizahlen.
	if (n<3) return [1 1]; endif;
	f=ones(1, n);
	for i=3 to n; f[i]=f[i-1]+f[i-2]; end;
	return f
endfunction

function itertest(x)
## itertest(x) konvergiert gegen sqrt(2).
	repeat;
		x=(x+2/x)/2; x,
		if x*x~=2; break; endif;
	end;
	return x;
endfunction
	
function multitest
	return {arg1,arg1*arg1}
endfunction

function hut (x,y)
	r=x^2+y^2;
	if r<1-epsilon(); return exp(1-1/(1-r));
	else return 0;
	endif;
endfunction
		
function udfdemo
	cls;
	""
	"EULER is programmable.",
	"" "As an example we list a function, which computes the first n"
	"fibonacci numbers when called with fibo(n):"
	pause();
	">type fibo",
	type fibo;
	">fibo(5)", fibo(5),
	pause();
	"As you can see, a function starts with" 
	">function name" 
	"and ends with"
	">end"|"function" 
	"Every function must have a return statement, which stops it."
	pause();
	"Arguments are named arg1, arg2, arg3, ...,"
	"unless they are given names in the function header."
	"argn() gives the number of arguments. All variables are local and"
	"cease to exist after function execution is finished."
	"But the command" ">global name"
	"allows access of a global variable."
	weiter();
	
	"There are:" 
	""
	"Conditionals:"
	">if condition; ... { else; ... } endif;"
	""
	"The condition can be any expression, especially a test like < > etc."
	"Logical connections are made with || (or) and && (and)."
	"!condition means: not condition."
	pause();
	"Repetitive statements:"
	">for variable=start to end { step value }; ... end;"
	">loop start to end; ... end;"
	">repeat; ... end;"
	pause();
	"In loop's the index can be accessed with index() or #."
	"All these statements can be left with a break; or return ...;"
	pause();
	
	"Example:"
	""
	type itertest
	""
	">longformat(); itertest(1.5), shortformat();", 
	longformat(); itertest(1.5), shortformat();
	weiter();

	"A function can return multiple values and these values can be"
	"assigned to several variables."
	""
	"Syntax:"
	">return {variable,variable,...}"
	"The brackets { and } are part of the syntax here."
	"Then the multiple assignment looks like:"
	">{a,b}=test(parameter)"
	pause();
	"An example:"
	""
	type multitest
	""
	"{a,b}=multitest(2); a, b,", {a,b}=multitest(2); a, b,
	weiter();
	
	"Comments start with #"|"#. Comments are not listet with type."
	"If the first lines of a functions start with #"|"#,"
	"they are considered help text. With help this text can be read."
	""
	">help shortformat" "", help shortformat,
	weiter();

	"Since all EULER functions work for vector and matrix input,"
	"user defined functions should be written the same way."
	"Most of the time, this is automatic. But sometimes a function"
	"uses a complicate algorithm or cases."
	"Then you may use the map function."
	pause();
	">type hut", type hut
	taste();
	v=view(5,2,0,0.4);
	''>x=-1.2:0.2:1.2; y=x'; framedsolid(x,y,map("hut",x,y));'',
	x=-1.2:0.2:1.2; y=x';
	framedsolid(x,y,map("hut",x,y)); title("(return)");
	warten();
	view(v);
	""
	shrinkwindow();
	''>x=-2:0.05:2; xplot(x,map("hut",x,0));'',
	taste();
	x=-2:0.05:2; xplot(x,map("hut",x,0)); title("(return)"); warten();
	cls;

	"You can echo output to a file"
	""
	">dump filename"
	"" 
	"Again, filename must be string expression."
	pause();
	"With"
	">dump"
	"echoing is stopped and the file closed."
	""
	"Dumps are always appended to the file."
	"With"
	">remove filename"
	"the file is removed (killed)."
	""
	"By the way:"
	">forget f; clear a;"
	"removes a function and the variable a."
	weiter();
	
	return 0
endfunction

function vectorfield (expr,x1,x2,y1,y2,nx=20,ny=20)
## Draw the vector field of a differential equation in x and y.
## expr must be the expression "f(x,y)", which computes the
## derivative of y(x) at x.
	setplot(x1,x2,y1,y2);
	x=linspace(x1,x2,nx-1);
	y=linspace(y1,y2,ny-1);
	{X,Y}=field(x,y);
	V=expreval(expr,X,y=Y);
	n=prod(size(X));
	h=(x2-x1)/nx/4;
	v=redim(V,[n,1]);
	r=sqrt(v*v+1);
	x=redim(X,[n,1]); x=(x-h/r)|(x+h/r);
	y=redim(Y,[n,1]); y=(y-h*v/r)|(y+h*v/r);
	st=linestyle("->"); xplot(x,y); linestyle(st);
	return plot();
endfunction

function norm (v)
	return sqrt(sum(v*v))
endfunction

function f3body (x,y)
	y1=y[1,1:2]; y2=y[1,3:4]; y3=y[1,5:6];
	d21=(y2-y1)/norm(y2-y1)^3;
	d31=(y3-y1)/norm(y3-y1)^3;
	d32=(y3-y2)/norm(y3-y2)^3;
	return y[7:12]|(d21+d31)|(-d21+d32)|(-d31-d32);
endfunction

function test3body (y1)
	y=y1;
	x=0; h=0.01;
	setplot(-3,3,-3,3); clg; hold off; xplot(); frame();
	title("Any key stops the movement");
	testkey();
	repeat;
		loop 1 to 10;
			ynew=runge2("f3body",x,x+h,y,0.0001,h);
			H=(y_ynew)';
			hold on;
			color(4); plot(H[1],H[2]);
			color(5); plot(H[3],H[4]);
			color(6); plot(H[5],H[6]);
			y=ynew; x=x+h;
			hold off;
		end;
		color(1);
		if (any(abs(y[1:6])>4));
			"One body left definition area!",
			break;
		endif;
		if testkey(); break; endif;
	end;
	return x
endfunction

function specialdemo

## Differentialgleichung
	cls;
	"Differential equations : "
	""
	''vectorfield("-2*x*y",0,2,0,2);''
	''>t=0:0.05:2; s=heun("-2*x*y",t,1); plot(t,s);'',
	taste();
	vectorfield("-2*x*y",0,2,0,2);
	t=0:0.05:2; s=heun("-2*x*y",t,1); hold; plot(t,s); hold; title("(return)");
	warten();
	""
	"Maximal error : "
	""
	">max(abs(s-exp(-t^2)))", max(abs(s-exp(-t^2))),
	weiter();
	"We now solve the three body problem with the"
	"adaptive Runge-Kutta method."
	"Watch the bodies move."
	taste();
	test3body([-2,0,2,0,0,2,0.1,-0.2,0,0.2,0.1,0]); cls;
	
	return 0;
endfunction

function eigendemo
	cls
	""

## Eigenwerte :
	"Eigenvalues and eigenvectors : " ""
	"Given the matrix"
	">A=[0,1;1,1]", A=[0,1;1,1],
	""
	"We compute the eigenvalues and eigenvectors of A."
	">{l,V}=eigen(A); l=re(l),"
	{l,V}=eigen(A); l=re(l),
	">V=re(V),"
	V=re(V), 
	pause();
	
	"Then V'.D.V=A."
	""
	">D=diag(size(A),0,l); V'.D.V,"
	D=diag(size(A),0,l); V'.D.V,
	weiter();

	fracformat(5);
	"There are routines to determine all solutions"
	"of a linear system."
	""
	"Generate a random 3x5 matrix and a random right side."
	">A=floor(random(3,5)*10),"
	"(We repeat this until the rank is 3.)"
	repeat
	A=floor(random(3,5)*10),
	if cols(image(A[1:3,1:3]))==3; break; endif;
	end;
	">b=floor(random(3,1)*10),"
	b=floor(random(3,1)*10),
	pause();
	"See if Ax=b must have a solution"
	">rank(A),"
	rank(A),
	"Since rank(A) is 3, it must have a solution"
	"Choose a special solution"
	">solvespecial(A,b)"
	solvespecial(A,b),
	"plus the following homogenous solutions"
	">kernel(A),"
	kernel(A),
	shortformat();
	""
	"A good special solution (computed with singular value"
	"decomposition) would be"
	">svdsolve(A,b)"
	svdsolve(A,b),
	weiter();
	
	"Let us compute the intersection of two planes."
	fracformat(5);
	"" "First we setup two planes v+l1*v1+l2*v2 and"
	"w+l1*w1+l2*w2 in parmatric forms."
	">v=[1,2,3]'; v1=[-2,3,4]'; v2=[1,1,0]';"
	">w=[1,2,1]'; w1=[0,-1,1]'; w2=[1,0,0]';"
	v=[1,2,3]'; v1=[-2,3,4]'; v2=[1,1,0]';
	w=[1,2,1]'; w1=[0,-1,1]'; w2=[1,0,0]';
	pause();
	"Next we compute a normal form and show it in compact form"
	">vx=crossproduct(v1,v2); vc=vx'.v; vx'|vc"
	vx=crossproduct(v1,v2); vc=vx'.v; vx'|vc
	">wx=crossproduct(w1,w2); wc=wx'.w; wx'|wc"
	wx=crossproduct(w1,w2); wc=wx'.w; wx'|wc
	"" "Now we can compute the intersection"
	">x=solvespecial([vx';wx'],[vc;wc])"
	x=solvespecial([vx';wx'],[vc;wc])
	">k=kernel(vx'_wx')"
	k=kernel(vx'_wx')
	pause();
	"Finally we check if x and x+k are indeed on both planes"
	">l=fit(v1|v2,x-v); v+l[1]*v1+l[2]*v2,"
	l=fit(v1|v2,x-v); v+l[1]*v1+l[2]*v2,
	">l=fit(w1|w2,x-w); any(!(w+l[1]*w1+l[2]*w2~=x)),"
	l=fit(w1|w2,x-w); any(!(w+l[1]*w1+l[2]*w2~=x)),
	">l=fit(v1|v2,x+k-v); v+l[1]*v1+l[2]*v2|x+k,"
	l=fit(v1|v2,x+k-v); v+l[1]*v1+l[2]*v2|x+k,
	">l=fit(w1|w2,x+k-w); any(!(w+l[1]*w1+l[2]*w2~=x+k)),"
	l=fit(w1|w2,x+k-w); any(!(w+l[1]*w1+l[2]*w2~=x+k)),
	shortformat();
	weiter();
	return 0;	
endfunction

function grafiktutor
## Erkl„rt die Grafikfunktionen
	hold off;
	global Pi;
	shrinkwindow();
	cls;
	"Explanation of some grafics functions."
	""
	"First we compute a vector of x-values."
	">x=-1:0.02:1;" x=-1:0.02:1;
	"Then we compute the function at these points."
	">y=sin(4*Pi*x)*exp(-x*x);" y=sin(4*Pi*x)*exp(-x*x);
	"Finally we can plot the function."
	">plot(x,y);"
	taste();
	plot(x,y); warten();
	"The plot can be given a title."
	''>title("sin(4*Pi*x)*exp(-x*x),(return)");'',
	taste();
	title("sin(4*Pi*x)*exp(-x*x),(return)"); warten();
	"For x- and y-grids, we use the following commands."
	">xgrid(-0.5:0.5:0.5); ygrid(-1:0.5:1);"
	taste();
	xgrid(-0.5:0.5:0.5); ygrid(-1:0.5:1); warten();
	"To draw another plot above it, we use the hold command.",
	">hold on; plot(x,cos(4*Pi*x)*exp(-x*x)); hold off;"
	taste();
	hold on; plot(x,cos(4*Pi*x)*exp(-x*x)); hold off; warten(); cls;

	"One can plot several curves wth a single plot command."
	"",
	"This time, the grid is to have labels."
	">shrinkwindow();" shrinkwindow();
	"", ">x=linspace(-1,1,50); T=cos((1:5)'*acos(x));"
	x=linspace(-1,1,50); T=cos((1:5)'*acos(x));
	"This way we computed 5 rows of 5 functions cos(n*acos(x))."
	"", "First we set the plot region manually.",
	">setplot(-1.5,1.5,-1.5,1.5);"
	"The we plot all functions."
	">plot(x,T); xgrid(-2:2,1); ygrid(-2:2,1); weiter(); hold off;"
	taste();
	setplot(-1.5,1.5,-1.5,1.5);
	plot(x,T); xgrid(-2:2,1); ygrid(-2:2,1); hold off; warten(); cls;
	"One can also mark a point on the screen with the mouse."
	''>title("Click the left mouse button."); c=mouse(); c,''
	taste();
	if delay>30;
		title("Click the left mouse button."); c=mouse(); c,
		"These are the coordinates, you choose."
		weiter();
	endif;
	
	"Now a function with a singularity."
	""
	">t=-1:0.01:1; setplot(-1,1,-10,10);"
	">xplot(t,1/t); hold on; plot(t,1/t^2); hold off;"
	""
	taste();
	t=-1:0.01:1; setplot(-1,1,-10,10);
	xplot(t,1/t); hold on; plot(t,1/t^2); hold off;
	title("1/t and 1/t^2 (return)"); warten(); cls;

	"Functions of two variables :"
	""
	"First we compute the x- and y-coordinates."
	">x=-1:0.1:1; y=x';"
	x=-1:0.1:1; y=x';
	"", "Then we compute the function values at these grid points and"
	"draw them."
	">z=x*x+x*y-y*y; mesh(z);"
	taste();
	z=x*x+x*y-y*y; fullwindow(); mesh(z); warten(); cls;
	"The contour level lines of these function we can inspect with"
	">contour(z,-3:0.5:3);"
	taste();
	contour(z,-3:0.5:3); warten(); cls;
	"A perspective view of the same surface."
	">view(2.5,1,1,0.5); solid(x,y,z);"
	taste();
	view(2.5,1,1,0.5); solid(x,y,z); warten(); cls;
	"A wire frame model."
	">wire(x,y,z);"
	taste();
	wire(x,y,z); warten(); cls;
	
	return 0
endfunction

function torus
## some torus type bodies.
	view([4,1,0,0.6]);
	x=linspace(0,2*pi,40);
	y=linspace(0,2*pi,20)';
## faster than {phi,psi}=field(x,y); cosphi=cos(phi); ...

## a torus with a thick and a thin side.
	factor=1.5+cos(y)*(cos(x)/2+0.6);
	X=cos(x)*factor;
	Y=sin(x)*factor;
	Z=sin(y)*(cos(x)/2+0.6);
	solid(X,Y,Z); title("(Return)"); warten();
	
## a deformed torus
	factor=1.5+cos(y);
	X=cos(x)*factor;
	Y=sin(x)*factor;
	Z=sin(y)+cos(2*x);
	solid(X,Y,Z); title("(Return)"); warten();
	
## the Moebius band
	t=linspace(-1,1,20)';
	x=linspace(0,pi,40);
	factor=2+cos(x)*t;
	X=cos(2*x)*factor;
	Y=sin(2*x)*factor;
	Z=sin(x)*t;
	solid(X,Y,Z); title("Return"); warten();

	return 0;
endfunction

function tube
## some tube like bodies.
	view([3,1,0,0.1]);
	global Pi;
	x=linspace(0,2*Pi,40);

## a atomic modell or so.
	y=0.1+(sin(linspace(0,Pi,15))| ..
		1.5*sin(linspace(0,Pi,10))|sin(linspace(0,Pi,15)));
	cosphi=dup(cos(x),length(y));
	sinphi=dup(sin(x),length(y));
	f=dup(y',length(x));
	
	solid(f*cosphi,f*sinphi,dup(linspace(-2,2,length(y)-1)',length(x)));
	title("(Return)");
	warten(); 

## a black hole
	t=linspace(0,1,20);
	cosphi=dup(cos(x),length(t));
	sinphi=dup(sin(x),length(t));
	f=dup((t*t+0.2)',length(x));
	view([3,1.5,0,0.7]);
	solid(f*cosphi,f*sinphi,dup(t'*2-1,length(x)));
	title("(Return)"); warten();

	return 0;
endfunction

function minimal (r,phi)
	R=dup(r',length(phi));
	X=R*dup(cos(phi),length(r))+R*R/2*dup(cos(2*phi),length(r));
	Y=-R*dup(sin(phi),length(r))-R*R/2*dup(sin(2*phi),length(r));
	Z=4/3*dup((r^1.5)',length(phi))*dup(cos(1.5*phi),length(r))-1;
	view(9,1,-1.5,0.2);
	solid(X,Y,Z);
	title("A minmal surface (Return)");
	warten();
	return 0;
endfunction

function spiral
	{r,a}=field(0:0.1:1,0:pi()/8:6*pi());
	z=a/8;
	x=r*cos(a)*(1-a/20);
	y=r*sin(a)*(1-a/20);
	z=z-1.5;
	view(4,1.5,0.5,0.3);
	framedsolid(x,y,z); title("(Return)"); warten();
	return 0;
endfunction

function rings
	rr=0.2;
	t=linspace(0,2*pi,10);
	s=linspace(0,2*pi,41); n=length(s);
	r=dup(1+cos(t)*rr,n)'; m=length(t);
	x=dup(cos(s),m)*r; y=dup(sin(s),m)*r;
	z=dup(sin(t)*rr,n)';
	view([5,1.5,0.3,0.3]);
	X=x_(x+1.3)_(x-1.3);
	Y=y_-z_-z;
	Z=z_y_y;
	solid(X,Y,Z,[m,2*m]);
	title("Disconnnected surfaces (Return)");
	warten();
	return 0;
endfunction

function startdemo
	""
	"We now generate some nice 3D objects"
	weiter();
	{x,y}=field(-1:0.1:1,-1:0.1:1);
	z=x*x+y*y;
	mesh(z); title("(Return)"); warten();
	return 0;
endfunction

function knot()
	t=linspace(0,2*pi,200);
	x=sin(t)+2*sin(2*t);
	y=cos(t)-2*cos(2*t);
	z=sin(3*t);
	xv=cos(t)+4*cos(2*t);
	yv=-sin(t)+4*sin(2*t);
	zv=3*cos(3*t);
	n=sqrt(xv*xv+yv*yv+zv*zv); 
	xv=xv/n; yv=yv/n; zv=zv/n;
	xv1=1-xv*xv; yv1=-xv*yv; zv1=-xv*zv;
	n=sqrt(xv1*xv1+yv1*yv1+zv1*zv1); 
	xv1=xv1/n; yv1=yv1/n; zv1=zv1/n;
	xv2=yv*zv1-yv1*zv;
	yv2=-xv*zv1+xv1*zv;
	zv2=xv*yv1-xv1*yv;
	n=sqrt(xv2*xv2+yv2*yv2+zv2*zv2); 
	xv2=xv2/n; yv2=yv2/n; zv2=zv2/n;
	p=linspace(0,2*pi,20)';
	r=0.3;
	X=x+r*(cos(p)*xv1+sin(p)*xv2);
	Y=y+r*(cos(p)*yv1+sin(p)*yv2);
	Z=z+r*(cos(p)*zv1+sin(p)*zv2);
	view(6,2,0.5,0.7);
	framedsolidhue(X,Y,Z,cos(p)*xv1+sin(p)*xv2,2,1);
	title("A trefoil knot (Return)");
	warten();
	return 0;
endfunction

function demo3d
	cls;
	startdemo();
	torus();
	tube();
	minimal(0.1:0.1:2.5,0:pi()/20:2*pi());
	spiral();
	rings();
	knot();
	return 0;
endfunction

function g(x)
	return x^3+x^2/2-x
endfunction

function g1(x)
	return 3*x^2+x-1
endfunction

function inewtonhist (ffunc,fder,x)
## inewtonhist("f","df",x;...)
## Works like inewton,
## Returns the solution, the history and a flag, if the solution is verified.
	if !isinterval(x);
		x=expand(~newton(ffunc,fder,x;args())~,1000);
	endif
	valid=0;
	hist=x;
	repeat
		m=middle(x);
		if isfunction(ffunc); a=ffunc(~m,m~,args());
		else a=expreval(ffunc,~m,m~);
		endif;
		if isfunction(fder); b=fder(x,args());
		else b=expreval(fder,x);
		endif;
		xnew=(m-a/b);
		if !valid && (xnew << x); valid=1; endif;
		xnew=xnew && x;
		hist=hist|xnew;
		if ! (xnew << x); return {x,hist,valid}; endif;
		x=xnew;
	end;
endfunction

function intervaldemo
	cls; "",
	"Euler uses a powerful method to determine tight interval"
	"inclusions for zeros of functions. Moreover, the intervals"
	"are garanteed to contain the solution."
	""
	"Let us plot a function in [-1,1]"
	taste();
	fplot("x^3+x^2/2-x",-1,1); title("Press return"); warten();
	""
	"The we apply the interval Newton method to the interval [0.6,1]."
	""
	longformat;
	''>x=inewton("x^3+x^2/2-x","3*x^2+x-1",~0.6,1~);'',
	x=inewton("x^3+x^2/2-x","3*x^2+x-1",~0.6,1~);
	"Result", x,
	shortformat;
	""
	"This is a guaranteed inclusion of the zero of this polynomial."
	"Moreover, the function is quite fast. Here are the steps."
	""
	{x,h,f}=inewtonhist("x^3+x^2/2-x","3*x^2+x-1",~0.6,1~);
	h'
	weiter();
	"We now show how bad a simple polynomial evaluation can be."
	"This example is due to Rump."
	""
	">p=[-945804881,1753426039,-1083557822,223200658];"
	">t=linspace(1.61801916,1.61801917,100);"
	">s=polyval(p,t);"
	">xplot(t,s,ticks=0); ygrid(ticks(s)); xrange();"
	
	p=[-945804881,1753426039,-1083557822,223200658];
	t=linspace(1.61801916,1.61801917,100);
	s=polyval(p,t);
	taste();
	
	xplot(t,s,ticks=0); xrange(); pl=plot(); ygrid(ticks(pl[3],pl[4]),0,1);
	title("Incorrect polynomial values"); warten();
	
	""
	"But there is the xpolyval function, which evaluates the"
	"polynomial much better (uses an algorithm of Rump)"
	""
	">s=xpolyval(p,t,eps=1e-16);"
	">xplot(t,s,ticks=0); ygrid(ticks(s)); xrange();"
	taste();
	s=xpolyval(p,t,eps=1e-16);
	xplot(t,s,ticks=0); xrange(); pl=plot(); ygrid(ticks(pl[3],pl[4]),0,1);
	title("Correct polynomial values"); warten(); 
	
	""
	"There is also the function ipolyval, which produces a"
	"guaranteed inclusion of the polynomial value."
	"We test it at the point"
	t[length(t)/2]
	""
	"The result is"
	ipolyval(p,t[length(t)/2]),
	weiter();
	
	cls;
	return "";
endfunction

function waves (p=10)
	reset();
	view(8,2.5,0.2,0.2);
	twosides(0); v=fillcolor([0,0]); 
	deletepages();
	clg;
	title("Creating Animation");
	addpage();
	showpage(1);
	t=linspace(0,1,p);
	x=linspace(-2*pi,2*pi,20); y=x'; r=sqrt(x*x+y*y);
	loop 1 to cols(t);
	solid(x/pi,y/pi,sin(r-t[#]*2*pi)*exp(-t[#]*3)/5);
	addpage(150,150);
	end;
	twosides(1); fillcolor(v);
	return cols(t);
endfunction;	

function demof (a)
	t=linspace(-2*pi,2*pi,250);
	plot(t,sin(t*a)*exp(-t*t/10)); xplot();
	addpage(); return a;
endfunction

function animatedemo
	cls;
	if !isfunction("deletepages");
		"", "*** Not yet in this version! ***", ""
		weiter();
		return "";
	endif;
	"EULER will generate a few images.",
	"The space key will stop the animation!"
	taste();
	waves();
	animate();
	deletepages();
	"",
	"This generates a rotating 3D image."
	view(8,2.5,0.2,0.2);
	''>rotate("f3dplot";"x*y");'',
	"Press the space key to stop!", 
	taste();
	rotate("f3dplot";"x*y");
	reset();
	"",
	"We generate a rotating stereo picture with", 
	''>rotate("stereo";"wire",x,y,x^3+y^2);'',
	"Can you see it in stereographic view?",
	"Press the space key to stop the animation!", 
	taste();
	x=linspace(-1,1,11); y=x';
	reset();
	v=view(5,3,0.5,0);
	rotate("stereo";"wire",x,y,x^3+y^2);
	view(v);
	"",
	"Another animation",
	taste();
	deletepages(); 
	demof(0); showpage(1);
	map("demof",linspace(0,5,100));
	animate();
	return 0;
endfunction

function symbolicdemo 
	cls;
	if (!isfunction("yacas"))	
		"", "*** Not yet in this version! ***", ""
		weiter();
		return "";
	endif;
	"Thanks to the makers of Yacas, you can do symbolic things"
	"in Euler. You have a yacas process running in the"
	"background, if you use the yacas() function for the first"
	"time. Expression are passed to Yacas, handled and the"
	"result is returned as an Euler string."
	""
	"Here is a first example, computing the factorial of 20:",
	''>yacas("40!"), fak(40),''
	longestformat; yacas("40!"), fak(40),
	"Compare the exact Yacas result and the Euler result!",
	""
	pause();
	""
	"You can also use the Yacas result in Euler by evaluating"
	"the result string."
	''>log(evaluate(yeval("40! / 20!")),''
	log(evaluate(yacas("40! / 20!"))),

	"There is a utility function for this, where you can set",
	"variables.",
	"Let us compute the derivative of x^x in 2.",
	''>yeval("D(x) x^x",x=2),''
	yeval("D(x) x^x",x=2),
	weiter();
	
	"You can use expressions in strings in Euler in many places."
	"E.g., we use the fplot command to plot a function, its"
	"derivative, and its integral."
	""
	
	''>setplot(-1,1,-1,1); fplot("x^3-x",-1,1);''
	''>hold on;''
	''>color(12); fplot(yacas("D(x) (x^3-x)"),-1,1);''
	''>color(13); fplot(yacas("Integrate(t,0,x) (t^3-t)"),-1,1);''
	''>hold off; color(1);''

	taste();	

	setplot(-1,1,-1,1); fplot("x^3-x",-1,1);
	hold on;
	color(12); fplot(yacas("D(x) (x^3-x)"),-1,1);
	color(13); fplot(yacas("Integrate(t,0,x) (t^3-t)"),-1,1);
	hold off; color(1);
	title("Funktion, Ableitung und Integral");
		
	wait(180);

	""
	"The minimum of the function, computed by Yacas and"
	"by Euler."
	''>yacas("Solve((D(x) (x^3-x))==0,x)")''
	yacas("Solve((D(x) (x^3-x))==0,x)")
	''>yacas("N(%)")''
	yacas("N(%)")
	''>yeval("%[1][2]")''
	yeval("%[1][2]")
	''>fmin("x^3-x",0,1)''
	fmin("x^3-x",0,1)
	""
	"An interval inclusion for the minimum."
	''>e1=yacas("D(x) x^3-x")''
	e1=yacas("D(x) x^3-x")
	''>e2=yacas("D(x,2) x^3-2")''
	e2=yacas("D(x,2) x^3-2")
	''>inewton(e1,e2,~0,1~)''
	inewton(e1,e2,~0,1~)
	pause();
	
	"Here is another example of a Newton method."
	"We seek x,y such that cos(xy)=3y, sin(x+y)=2x"
	''>ynewton2("Cos(x*y)-3*y","Sin(x+y)-2*x",[1,1])''
	ynewton2("Cos(x*y)-3*y","Sin(x+y)-2*x",[1,1])
	weiter();
	
	"There is also an interface to convert vectors to"
	"Yacas and back."
	"You can use the variables of either Yacas or Euler."
	""

	''>A=[1,2,2;2,1,2;2,2,1],''
	A=[1,2,2;2,1,2;2,2,1],
	''>yset("A",A)''
	yset("A",A)
	''>yacas("Inverse(A)")''
	yacas("Inverse(A)")
	''>y2matrix("Inverse(A)")''
	y2matrix("Inverse(A)")
	weiter();
		
	reset();
	
	return 0
endfunction

function demo (del=180)
	global demodelay;
	demodelay=del;
	reset();
	repeat;
		cls
		""
		"Please select any of the following demos."
		""
		sel=menu("Basics", .. 1
			"Graphics Demo", .. 2
			"Discussion of a Function", .. 3
			"Approximations, Optimization, Minimization", .. 4
			"Statistics", .. 5
			"Programming", .. 6
			"Differential Equations", .. 7
			"Linear Algebra, Eigenvalues", .. 8
			"Intervals and Exact Evaluation", .. 9
			"Graphics Tutor", .. 10
			"3D Demo", .. 11
			"FFT", .. 12
			"Graphics Animation", .. 13
			"Symbolic Computation",.. 14
			"Quit");
		if sel==1; normaldemo();
		elseif sel==2; grafikdemo();
		elseif sel==3; humpdemo();
		elseif sel==4; apprdemo();
		elseif sel==5; statdemo();
		elseif sel==6; udfdemo();
		elseif sel==7; specialdemo();
		elseif sel==8; eigendemo();
		elseif sel==9; intervaldemo();
		elseif sel==10; grafiktutor();
		elseif sel==11; demo3d();
		elseif sel==12; fftdemo();
		elseif sel==13; animatedemo();
		elseif sel==14; symbolicdemo();
		elseif sel==15 || sel==-1; break;
		endif;
	end;
	abspann();
	reset();
	return ""
endfunction

function abspann
	""
	"I hope this demo has given you a good impression."
	"The author wishes you success with mathematics and EULER!"
	""
	return 0
endfunction



demo();

