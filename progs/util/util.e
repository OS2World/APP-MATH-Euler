comment
utilities
endcomment

_view([5,2.5,0.2,0.2]);
Epsilon=epsilon();
ResetEpsilon=epsilon();
ResetView=_view();
Pi=pi();
I=1i;
vertical=0;
E=2.718281828459045235360287;
eulergamma=0.577215664901532860606512;

function reset
## reset() resets epsilon and some other graphics things,
## like the default view, the huecolor and the linewidth.
## Also it calls shortformat.
	global ResetEpsilon,ResetView;
	setepsilon(ResetEpsilon);
	unclip();
	style(""); hold off;
	color(1);
	shrinkwindow();
	shortformat();
	linewidth(1);
	view(ResetView);
	huecolor(1);
	return 0;
endfunction

function resetepsilon
## reset the default epsilon and return the current epsilon.
	global ResetEpsilon;
	return setepsilon(ResetEpsilon);
endfunction

function wait (d=300)
## waits for a key (at most 5 min)
	return _wait(d);
endfunction

function longformat
## longformat() sets a long format for numbers
## See: shortformat, longestformat
	return goodformat([24 12]);
endfunction

function longestformat
## longformat() sets a long format for numbers
## See: shortformat, longformat
	return goodformat([32 16]);
endfunction

function shortformat
## shortformat() sets a short format for numbers
## See: longformat, longestformat
	return goodformat([14 5]);
endfunction;

function  fracformat (n=20, eps=epsilon)
## Activates the fractional format
	return _fracformat([n,eps]);
endfunction

function linspace (a,b,n)
## linspace(a,b,n) generates n+1 linear spaced points in [a,b].
	if a~=b; return a; endif;
	r=a:(b-a)/n:b;
	r[n+1]=b;
	return r;
endfunction

function equispace (a,b,n)
## equispace(a,b,n) generates n+1 euqidistribution (acos) spaced values
## in [a,b].
	m=(1-cos(0:pi()/n:pi()))/2;
	return a+(b-a)*m;
endfunction

function length (v)
## length(v) returns the length of a vector
	return max(size(v));
endfunction

function polydif (p)
## polydif(p) returns the polynomial p'
	n=cols(p);
	if (n==1); return 0; endif;
	return p[2:n]*(1:n-1);
endfunction

function isstring (x)
## Test, if x is a string.
	return typeof(x)==8;
endfunction

function writeform (x)
	if isreal(x); return printf("%25.16e",x); endif;
	if iscomplex(x);
		return printf("%25.16e",re(x))|printf("+%25.16ei",im(x));
	endif;
	if isstring(x); return x; endif;
	error("Cannot print this!");
endfunction

function varwrite (x,s="")
## Write a variable to ouput in EULER syntax. Use s as the name.
## Output looks like s=...
	if s==""; s=name(x); endif;
	si=size(x);
	if max(si)==1; s|" = "|writeform(x)|";", return 0; endif;
	s|" = [ .."
	for i=1 to si[1];
		for j=1 to si[2]-1;
			writeform(x[i,j])|",",
		end;
		if i==si[1]; writeform(x[i,si[2]])|"];",
		else; writeform(x[i,si[2]])|";",
		endif;
	end;
	return 0
endfunction

function expreval (expr,x)
## Evaluate the expression. Use global variables.
	useglobal;
	return evaluate(expr);
endfunction

.. ### plot things ###
logredraw=1;
subwindows=zeros([20,4]);

function figure(n)
## figure([nr,nc]) divides the graph window into nr rows x nc columns
## of subplots
## figure(n) sets the current drawing window to the nth subplot counting
## by rows.
	global subwindows,logredraw;
	if rows(n)==1 && cols(n)==2;
		nr=n[1];nc=n[2];
		w=window();
		wd=textwidth();
		ht=textheight();
		hr=floor(1024/nr);wc=floor(1024/nc);
		for r=1 to nr
			for c=1 to nc
				w=[8*wd+(c-1)*wc,1.5*ht+(r-1)*hr,c*wc-2*wd,r*hr-3*ht];
				subwindows[(r-1)*nc+c]=w;
			end;
		end;
		hold off;
		logredraw=1;
		return _window(subwindows[1]);
	else
		hold on;
		logredraw=1;
		return _window(subwindows[n]);
	endif
endfunction

function toscreen2 (x,y)
## Convert the x-y-coordinates v[1],v[2] to screen coordinates,
## from 0..1024. Useful for Labels.
	p=plot(); w=window();
	return {w[1]+(x-p[1])/(p[2]-p[1])*(w[3]-w[1]), ..
		w[2]-(y-p[4])/(p[4]-p[3])*(w[4]-w[2])};
endfunction

function toscreen ()
## Convert the x-y-coordinates v[1],v[2] to screen coordinates,
## from 0..1024. Useful for Labels.
	if argn()==2; return toscreen2(arg1,arg2); endif;
	if argn()<>1; error("Wrong arguments for toscreen"); endif;
	v=arg1;
	{c,r}=toscreen2(v[1],v[2]);
	return [c,r]
endfunction

function fromscreen (c,r)
## Convert the screen coordinates v[1],v[2] to x-y-coordinates,
## Returns {x,y}
	p=plot(); w=window();
	return {p[1]+(c-w[1])/(w[3]-w[1])*(p[2]-p[1]), ..
		p[3]+(w[4]-r)/(w[4]-w[2])*(p[4]-p[3])}
endfunction

function label (t,x,y,offset=0)
## Label the x-y-value with the text t.
## offset is in screen coordinates 0..1024.
	c=toscreen([x,y]); c[1]=c[1]+textwidth()/4+offset; c[2]=c[2]+offset;
	return text(t,c)
endfunction

function plotbar1 (x,y,w,h)
## Plots a single bar with x,y,w,h in plot coordinates.
## use barstyle and barcolor to modify the bar.
## The parameters may be vectors.
	a=toscreen([x,y+h]);
	b=toscreen([x+w,y]);
	bar(a|(b-a));
	return 0;
endfunction

function plotbar (x,y,w,h)
## Plots a bars with x,y,w,h in plot coordinates.
## use barstyle and barcolor to modify the bar.
## The parameters may be vectors.
	if !holding(); clg; endif;
	map("plotbar1",x,y,w,h);
	return plot();
endfunction

function xplotbar (x,y,w,h,st="#O")
## Plots a bar with x,y,w,h in plot coordinates.
## Uses the plot coordinates from the last plot!
## Use setplot to set new plot coordinates.
## Draws coordinates below the bar.
## use barstyle and barcolor to modify the bar.
	if !holding(); clg; endif;
	frame; xplot();
	stold=barstyle(st);
	hold; plotbar(x,y,w,h); hold;
	barstyle(stold);
	return plot();
endfunction;

function scalematrix(A)
## Scales a matrix A, so that its value are in the range from 0 to 1.
	e=extrema(A)'; mi=min(e[1]); ma=max(e[3]);
	if ma~=mi; ma=mi+1; endif;
	return (A-mi)/(ma-mi);
endfunction	

function select
## Returns coordinates {x,y} of mouse clicks, until the user clicked
## above the plot window.
	p=plot();
	x=[];
	repeat
		m=mouse();
		if m[2]>p[4]; break; endif;
		h=holding(1); mark(m[1],m[2]); holding(h);
		x=x_m;
	end;
	x=x'; return {x[1],x[2]}
endfunction

function title (text)
## title(text) plots a title to the grafik window.
	ctext(text,[512 0]);
	return text;
endfunction

function textheight
## textheight() returns the height of a letter.
	h=textsize();
	return h[2];
endfunction

function textwidth
## textwidth() returns the width of a letter.
	h=textsize();
	return h[1];
endfunction

function fullwindow
## fullwindow() takes the full size (besides a title) for the
## plots.
	global logredraw;
	h=textsize();
	logredraw=1;
	return _window([12,textheight()*1.5,1011,1011]);
endfunction

function shrinkwindow
## shrinkwindow() shrinks the window to allow labels.
	global vertical,logredraw;
	h=textheight(); b=textwidth();
	logredraw=1;
	if vertical; return _window([2*h,1.5*h,1023-2*b,1023-3*h]);
	else; return _window([8*b,1.5*h,1023-2*b,1023-3*h]);
	endif;
endfunction

function setplot
## setplot([xmin xmax ymin ymax]) sets the plot coordinates and holds
## the plot. Also setplot(xmin,xmax,ymin,ymax).
## setplot() resets it. 
	if argn()==4; return _setplot([arg1,arg2,arg3,arg4]);
	else;
		if argn()==0; scaling(1); 
		else; _setplot(arg1);
		endif;
	endif;
	return plot();
endfunction

function ticks (aa=0,bb=0)
## returns the proper ticks to be used for intervals [a,b] and
## the factor f of the ticks.
	if argn==2; a=aa; b=bb;
	elseif argn==1; a=min(aa); b=max(aa);
	else error("Wrong arguments for ticks");
	endif;
	if (b>1e30); b=1e30; endif;
	if (a<-1e30); a=-1e30; endif;
	if (a>=b); b=a+1; endif;
	tick=10^floor(log(b-a)/log(10)-0.4);
	if b-a>10*tick; tick1=tick*2; else; tick1=tick; endif;
	if (tick>0.000001) && (tick<10000); tick=1; endif;
	return {(floor(a/tick1)+1:ceil(b/tick1)-1)*tick1,tick}
endfunction

function xplot (x=0,y=0,grid=1,ticks=1)
## xplot(x,y) or xplot(y) works like plot, but shows axis ticks.
## xplot() shows only axis ticks and the grid.
	if argn()>0;
		if argn()==1;
			if iscomplex(x); y=im(x); xh=re(x);
			else y=x; xh=1:cols(y);
			endif;
		else; xh=x;
		endif;
		p=plotarea(xh,y);
		if !holding(); clg; frame(); endif;
	else
		p=plot();
	endif;
	lw=linewidth(1);
	{t,f}=ticks(p[1],p[2]);
	xgrid(t,f,grid,ticks);
	{t,f}=ticks(p[3],p[4]);
	ygrid(t,f,grid,ticks);
	linewidth(lw);
	if argn()>0;
		ho=holding(1);plot(xh,y);holding(ho);
	endif;
	return p;
endfunction

function xmark (x=0,y=0,grid=1,ticks=1)
## xmark(x,y) or xmark(y) works like plot, but shows axis ticks.
## xmark() shows only axis ticks and the grid.
	if !holding(); clg; frame(); endif;
	if argn()==1;
		if iscomplex(x); y=im(x); xh=re(x);
		else; y=x; xh=1:cols(y);
		endif;
	else; xh=x;
	endif;
	p=plotarea(xh,y);
	{t,f}=ticks(p[1],p[2]);
	xgrid(t,f,grid,ticks);
	{t,f}=ticks(p[3],p[4]);
	ygrid(t,f,grid,ticks);
	ho=holding(1); p=mark(xh,y); holding(ho);
	return p;
endfunction

function setplotm
## The user may choose the plotting coordinates with the mouse.
## Returns the plot coordinates.
	h=holding(1);
	k1=mouse(); mark(k1[1],k1[2]);
	k2=mouse(); mark(k2[1],k2[2]);
	kl=min(k1,k2); ku=max(k1,k2);
	c=color(2);
	plot([kl[1],kl[1],ku[1],ku[1],kl[1]],[kl[2],ku[2],ku[2],kl[2],kl[2]]);
	color(c);
	setplot(kl[1],ku[1],kl[2],ku[2]);
	holding(h);
	return plot();
endfunction

function fplot (ffunction,a=0,b=0,c=0,d=0,n=200)
## fplot("f",a,b,c,d,n;...) plots the function f(x,...) in [a,b].
## fplot("f") or fplot("f",,,,n;...) plots f in the old interval.
## If c<d, setplot(a,b,c,d) is called.
## fplot uses map for the evaluation of the function "f".
## ffunction may be an expression in x.
	if a~=b; s=plot(); a=s[1]; b=s[2]; endif;
	t=linspace(a,b,n);
	if isfunction(ffunction); s=map(ffunction,t;args());
	else s=expreval(ffunction,t);
	endif;
	if c<d; setplot(a,b,c,d); endif;
	if holding; return plot(t,s);
	else return xplot(t,s);
	endif;
endfunction

function printscale (x)
	if (abs(x)>10000) || (abs(x)<0.00001);
		return printf("%12.5e",x);
	else
		return printf("%10.5f",x);
	endif;
endfunction

function niceform (x)
## Return a string, containing a nice print of x.
	y=round(x,10);
	return printf("%g",y);
endfunction

function xgrid(xx,f=1,grid=1,ticks=1,color=3,xt="default")
## xgrid([x0,x1,...]) draws vertical grid lines on the plot window at
## x0,x1,...
## xgrid([x0,x1,...],f) additionally writes x0/f to the axis.
	c=plot(); n=cols(xx); s=scaling(0); h=holding(1);
	w=window();
	st=linestyle("."); wi=linewidth(1); color(color);
	ht=textheight();
	if isstring(xt); xt=xx; endif;
	str="";
	loop 1 to n;
		x=xx[#];
		if (x<=c[2])&&(x>=c[1]); 
			if grid; _plot([x,x],[c[3],c[4]]); endif;
			if ticks;
				col=w[1]+(x-c[1])/(c[2]-c[1])*(w[3]-w[1]);
				stra=niceform(xt[#]/f);
				if !(stra==str && #<n);
					_ctext(stra,[col,w[4]+0.2*ht]);
					str=stra;
				endif;
			endif;
		endif;
	end;
	if ticks && !(f~=1);
..		_ctext("* "|printscale(f),[(w[1]+w[3])/2,w[4]+1.5*ht]);
		_rtext("* "|printscale(f),[w[3],w[4]+1.5*ht]);
	endif;
	linestyle(st); linewidth(wi); color(1); holding(h); scaling(s);
	return 0;
endfunction

function ygrid(yy,f=1,grid=1,ticks=1,color=3,yt="default")
## ygrid([x0,x1,...]) draws horizontal grid lines on the plot window at
## x0,x1,...
## ygrid([x0,x1,...],f) additionally writes x0/f to the axis.
	global vertical
	c=plot(); n=cols(yy); s=scaling(0); h=holding(1);
	st=linestyle("."); wi=linewidth(1); color(color);
	w=window(); wd=textwidth(); ht=textheight();
	if isstring(yt); yt=yy; endif;
	loop 1 to n;
		y=yy[#];
		if (y>=c[3])&&(y<=c[4]);
			if ticks;
				row=w[4]-(y-c[3])/(c[4]-c[3])*(w[4]-w[2]);
				if vertical;
					_vcutext(niceform(y,f),[w[1]-0.2*ht,row]);
				else
					_rtext(niceform(yt[#]/f),[w[1]-wd/2,row-ht/2]);
				endif;
			endif;
			if grid; _plot([c[1],c[2]],[y,y]); endif;
		endif;
	end;
	if ticks && !(f~=1);
		_text("* "|printscale(f),[w[1]-6*wd,0]);
	endif;
	linestyle(st); linewidth(wi); color(1); holding(h); scaling(s);
	return 0;
endfunction

function xrange()
## Writes the range of x below the x axis.
	c=plot(); s=scaling(0); h=holding(1);
	w=window();
	ht=textheight();
	text(niceform(c[1]),[w[1],w[4]+0.2*ht]);
	rtext(niceform(c[2]),[w[3],w[4]+0.2*ht]);
	holding(h); scaling(s);
	return 0;
endfunction

function xlabel(text,l=1)
## Puts the label text at the x-axis at point l in [0,1].
	w=window();
	ht=textheight();
..	ctext(text,[w[1]+l*(w[3]-w[1]),w[4]+0.2*ht]);
	_ctext(text,[(w[1]+w[3])/2,w[4]+1.5*ht]);
	return 0;
endfunction

function yrange()
## Writes the range of y besides the y axis.
	c=plot(); s=scaling(0); h=holding(1);
	w=window();
	wd=textwidth();
	ht=textheight();
	rtext(niceform(c[4]),[w[1]-wd/2,w[2]]);
	rtext(niceform(c[3]),[w[1]-wd/2,w[4]-ht]);
	holding(h); scaling(s);
	return 0;
endfunction

function ylabel(text,l=1)
## Puts the label text at the x-axis at point l in [0,1].
	w=window();
	wd=textwidth();
	ht=textheight();
..	rtext(text,[w[1]-wd/2,w[2]+l*(w[4]-w[2])-ht/2]);
	_vcutext(text,[w[1]-8*wd+5,(w[2]+w[4])/2]);
	return 0;
endfunction

function plot (x=0,y=0)
## plot(x,y) plots the values (x(i),y(i)) with the current style.
## If x is a matrix, y must be a matrix of the same size.
## The plot is then drawn for all rows of x and y.
## The plot is scaled automatically, unless hold is on.
## plot(x,y) and plot() return [x1,x2,y1,y2], where [x1,x2] is the range
## of the x-values and [y1,y2] of the y-values.
## plot(x) is the same as plot(1:cols(x),x).
	if !holding() && argn()>0; clg; endif;
	if argn()==1;
		if iscomplex(x);
			oldclip=clip(window());
			res = _plot(re(x),im(x));
			clip(oldclip);
			return res;
		else
			oldclip=clip(window());
			res = _plot(1:cols(x),x);
			clip(oldclip);
			return res;
		endif;
	else if argn()==2;
		oldclip=clip(window());
		res = _plot(x,y);
		clip(oldclip);
		return res;
	else return _plot();
	endif;
endfunction

function mark (x=0,y=0)
## mark(x,y) plots markers at (x(i),y(i)) according the the actual style.
## Works like plot.
	if !holding() && argn()>0; clg; endif;
	if argn()==1;
		if iscomplex(x);
			oldclip=clip(window());
			res = _mark(re(x),im(x));
			clip(oldclip);
			return res;
		else
			oldclip=clip(window());
			res = _mark(1:cols(x),x);
			clip(oldclip);
			return res;
		endif;
	else if argn()==2;
			oldclip=clip(window());
			res = _mark(x,y);
			clip(oldclip);
			return res;
	else return _plot();
	endif;
endfunction

function cplot (z)
## cplot(z) plots a grid of complex numbers.
	plot(re(z),im(z)); s=scaling(0); h=holding(1);
	w=z'; plot(re(w),im(w)); holding(h); scaling(s);
	return plot();
endfunction

function plotwindow
## plotwindow() sets the plot window to the screen coordinates.
	w=window();
	setplot(w[1],w[3],w[2],w[4]);
	return plot()
endfunction

function density (x,f=1)
## density(x,1) makes density plot of the values in the matrix x
## scaled to fit into [0,f].
	if f==0;
		_density(x);
		return x;
	else;
		ma=max(max(x)'); mi=min(min(x)'); h=ma-mi;
		if h~=0; h=1; endif;
		xx=(x-mi)/h*f*0.99;
		_density(xx);
	return xx;
	endif;
endfunction

function solidhue (x,y,z,h,f=1)
## solidhue(x,y,z,h) makes a shaded solid 3D-plot of x,y,z.
## h is the shading and should run between 0 and 1.
## f determines, if h is scaled to fit in between 0 and f.
	if argn()==1; return _solidhue(x,y,z,h); endif;
	ma=max(max(h)'); mi=min(min(h)'); delta=ma-mi;
	if delta~=0; delta=1; endif;
	hh=(h-mi)/delta*f*0.9999;
	return _solidhue(x,y,z,hh);
endfunction

function fillcolor (c1=0,c2=0)
## Set the fillcolor for the fore- and background of 3D plots.
## fillcolor(c1,c2) or fillcolor() returns the colors [c1,c2].
	if argn()==0; return _fillcolor();
	else if argn==1; return _fillcolor(c1); endif;
	endif;
	return _fillcolor([c1,c2]);
endfunction

function mark3 (x,y,z)
## Plots points in three dimensions. 
## x,y,z must be 1xn vectors.
	{c0,r0}=project(x,y,z);
	{x0,y0}=fromscreen(c0,r0);
	return mark(x0,y0);
endfunction

function animate (d=0.1)
## Animate the pages with delay d.
	testkey();
	repeat
		loop 2 to pages();
			showpage(#); 
			if testkey(); showpage(0); return pages(); endif;
			wait(d);
		end;
	end;
	showpage(0);
	return pages();
endfunction;	

.. ### log plots ###

function xticks (aa=0,bb=0)
	if argn==2; a=aa; b=bb;
	elseif argn==1; a=min(aa); b=max(aa);
	else error("Wrong arguments for ticks");
	endif;
	if (b>1e30); b=1e30; endif;
	if (a<-1e30); a=-1e30; endif;
	if (a>=b); b=a+1; endif;
	tick=10^floor(log(b-a)/log(10)-0.4);
	if b-a>10*tick; tick1=tick*2; else; tick1=tick; endif;
	if (tick>0.001) && (tick<1000); tick=1; endif;
	return (floor(a/tick1))*tick1:tick1:(ceil(b/tick1))*tick1;
endfunction;

function xlogticks(expi,expf,base)
	x=[ ]; 
	for k = floor(expi) to floor(expf);
		x = x | logbase(1:base,base) + k;
	end;
	return x;
endfunction;

function xloggrid()

endfunction

function xlogplot(x=1,y=0,xbase=10)
## xlogplot draws y(x) in a semilog paper with base xbase
	global logredraw;
	if argn()==0;
		p=plot();
	else
		if !holding();clg;endif;
		if iscomplex(x); xe=abs(x); else xe=x; endif;
		if iscomplex(y); ye=abs(y); else ye=y; endif;
		xl=logbase(xe,xbase);
	
		if holding() && !logredraw
			hd=holding();
			p=plot();
		else
			expi=floor(logbase(min(xe),xbase));
			expf=ceil(logbase(max(xe),xbase));

			xt=xlogticks(expi,expf,xbase);
			if rows(ye)==1
				yt=xticks(min(ye),max(ye));
			elseif rows(ye)>1
				yt=xticks(min(min(ye)'),max(max(ye)'));
			endif;
	
			p=setplot([expi,expf,min(yt),max(yt)]);
	
			h=textheight();w=textwidth();
	
			hd=holding(1);
			ls=linestyle(".");
			lw=linewidth(1);
			c=color(1);
			for i=1 to length(yt);
				plot([expi,expf],[yt[i],yt[i]]);
				d=toscreen([expi,yt[i]]);
				rtext(printf("%g",yt[i]),[d[1]-w,d[2]-h/2]);
			end;
	
			for i=1 to length(xt)-xbase;
				plot([xt[i],xt[i]],[p[3],p[4]]);
			end;

			for i=expi to expf;
				d=toscreen([i,p[3]]);
..				ctext(printf("%g",xbase)|"^"|printf("%g",i),[d[1],d[2]+h/2]);
				rtext(printf("%g",xbase),[d[1]+w,d[2]+0.75*h]);
				text(printf("%g",i),[d[1]+w,d[2]+h/4]);
			end;

			color(c);
			linewidth(lw);
			linestyle(ls);
			frame();
		endif;
		plot(xl,ye);
		holding(hd);
		logredraw=0;
	endif;
	return p;
endfunction;

function ylogplot(x=0,y=1,ybase=10)
## xlogplot draws y(x) in a semilog paper with base ybase
	global logredraw;
	if argn()==0;
		p=plot();
	else
		if !holding();clg;endif;
		if iscomplex(x); xe=abs(x); else xe=x; endif;
		if iscomplex(y); ye=abs(y); else ye=y; endif;
		yl=logbase(ye,ybase);
		
		if holding && !logredraw
			hd=holding;
			p=plot();
		else
			expi=floor(logbase(min(ye),ybase));
			expf=ceil(logbase(max(ye),ybase));

			xt=xticks(min(xe),max(xe));
			yt=xlogticks(expi,expf,ybase);

			p=setplot([min(xt),max(xt),expi,expf]);

			h=textheight();w=textwidth();

			hd=holding(1);
			ls=linestyle(".");
			lw=linewidth(1);
			c=color(1);
			for i=1 to length(yt)-ybase;
				plot([p[1],p[2]],[yt[i],yt[i]]);
			end;
			for i=expi to expf;
				d=toscreen([p[1],i]);
..				rtext(printf("%g",ybase)|"^"|printf("%g",i),[d[1]-w/2,d[2]-h/2]);
				rtext(printf("%g",ybase),[d[1]-40,d[2]-h/2]);
				text(printf("%g",i),[d[1]-40,d[2]-h]);
			end;

			for i=1 to length(xt);
				plot([xt[i],xt[i]],[p[3],p[4]]);
				d=toscreen([xt[i],p[3]]);
				ctext(printf("%g",xt[i]),[d[1],d[2]+h/2]);
			end;


			color(c);
			linewidth(lw);
			linestyle(ls);
			frame();
		endif
		plot(xe,yl);
		holding(hd);
		logredraw=0;
	endif;
	return p;
endfunction;

function xylogplot(x=1,y=1,xbase=10,ybase=10)
## xylogplot draws y(x) in a loglog paper with base xbase and ybase
	global logredraw;
	if argn()==0;
		p=plot();
	else
		if !holding();clg;endif;
		if iscomplex(x); xe=abs(x); else xe=x; endif;
		if iscomplex(y); ye=abs(y); else ye=y; endif;
		xl=logbase(xe,xbase);
		yl=logbase(ye,ybase);
		
		if holding() && !logredraw
			hd=holding();
			p=plot();
		else
			expix=floor(logbase(min(xe),xbase));
			expfx=ceil(logbase(max(xe),xbase));
			expiy=floor(logbase(min(ye),ybase));
			expfy=ceil(logbase(max(ye),ybase));

			xt=xlogticks(expix,expfx,xbase);
			yt=xlogticks(expiy,expfy,ybase);

			p=setplot([expix,expfx,expiy,expfy]);

			h=textheight();w=textwidth();

			hd=holding(1);
			ls=linestyle(".");
			lw=linewidth(1);
			c=color(1);

			for i=1 to length(xt)-xbase;
				plot([xt[i],xt[i]],[p[3],p[4]]);
			end;
			for i=expix to expfx;
				d=toscreen([i,p[3]]);
..				ctext(printf("%g",xbase)|"^"|printf("%g",i),[d[1],d[2]+h/2]);
				rtext(printf("%g",xbase),[d[1]+w,d[2]+0.75*h]);
				text(printf("%g",i),[d[1]+w,d[2]+h/4]);
			end;

			for i=1 to length(yt)-ybase;
				plot([p[1],p[2]],[yt[i],yt[i]]);
			end;
			for i=expiy to expfy;
				d=toscreen([p[1],i]);
..				rtext(printf("%g",ybase)|"^"|printf("%g",i),[d[1]-w/2,d[2]-h/2]);
				rtext(printf("%g",ybase),[d[1]-40,d[2]-h/2]);
				text(printf("%g",i),[d[1]-40,d[2]-h]);
			end;

			color(c);
			linewidth(lw);
			linestyle(ls);
			frame();
		endif;
		plot(xl,yl);
		holding(hd);
		logredraw=0;
	endif;
	return p;
endfunction;

function xlogmark(x=1,y=0,xbase=10)
## xlogmark draws marks at the specified points in a semilog paper with base xbase
	global logredraw;
	if argn()==0;
		p=plot();
	else
		if !holding();clg;endif;
		if iscomplex(x); xe=abs(x); else xe=x; endif;
		if iscomplex(y); ye=abs(y); else ye=y; endif;
		xl=logbase(xe,xbase);
	
		if holding() && !logredraw
			hd=holding();
			p=plot();
		else
			expi=floor(logbase(min(xe),xbase));
			expf=ceil(logbase(max(xe),xbase));

			xt=xlogticks(expi,expf,xbase);
			if rows(ye)==1
				yt=xticks(min(ye),max(ye));
			elseif rows(ye)>1
				yt=xticks(min(min(ye)'),max(max(ye)'));
			endif;
	
			p=setplot([expi,expf,min(yt),max(yt)]);
	
			h=textheight();w=textwidth();
	
			hd=holding(1);
			ls=linestyle(".");
			lw=linewidth(1);
			c=color(1);
			for i=1 to length(yt);
				plot([expi,expf],[yt[i],yt[i]]);
				d=toscreen([expi,yt[i]]);
				rtext(printf("%g",yt[i]),[d[1]-w,d[2]-h/2]);
			end;
	
			for i=1 to length(xt)-xbase;
				plot([xt[i],xt[i]],[p[3],p[4]]);
			end;

			for i=expi to expf;
				d=toscreen([i,p[3]]);
..				ctext(printf("%g",xbase)|"^"|printf("%g",i),[d[1],d[2]+h/2]);
				rtext(printf("%g",xbase),[d[1]+w,d[2]+0.75*h]);
				text(printf("%g",i),[d[1]+w,d[2]+h/4]);
			end;

			color(c);
			linewidth(lw);
			linestyle(ls);
			frame();
		endif;
		mark(xl,ye);
		holding(hd);
		logredraw=0;
	endif;
	return p;
endfunction;

function ylogmark(x=0,y=1,ybase=10)
## xlogmark draws y(x) in a semilog paper with base ybase
	global logredraw;
	if argn()==0;
		p=plot();
	else
		if !holding();clg;endif;
		if iscomplex(x); xe=abs(x); else xe=x; endif;
		if iscomplex(y); ye=abs(y); else ye=y; endif;
		yl=logbase(ye,ybase);
		
		if holding && !logredraw
			hd=holding;
			p=plot();
		else
			expi=floor(logbase(min(ye),ybase));
			expf=ceil(logbase(max(ye),ybase));

			xt=xticks(min(xe),max(xe));
			yt=xlogticks(expi,expf,ybase);

			p=setplot([min(xt),max(xt),expi,expf]);

			h=textheight();w=textwidth();

			hd=holding(1);
			ls=linestyle(".");
			lw=linewidth(1);
			c=color(1);
			for i=1 to length(yt)-ybase;
				plot([p[1],p[2]],[yt[i],yt[i]]);
			end;
			for i=expi to expf;
				d=toscreen([p[1],i]);
..				rtext(printf("%g",ybase)|"^"|printf("%g",i),[d[1]-w/2,d[2]-h/2]);
				rtext(printf("%g",ybase),[d[1]-40,d[2]-h/2]);
				text(printf("%g",i),[d[1]-40,d[2]-h]);
			end;

			for i=1 to length(xt);
				plot([xt[i],xt[i]],[p[3],p[4]]);
				d=toscreen([xt[i],p[3]]);
				ctext(printf("%g",xt[i]),[d[1],d[2]+h/2]);
			end;


			color(c);
			linewidth(lw);
			linestyle(ls);
			frame();
		endif
		mark(xe,yl);
		holding(hd);
		logredraw=0;
	endif;
	return p;
endfunction;

function xylogmark(x=1,y=1,xbase=10,ybase=10)
## xylogmark draws y(x) points in a loglog paper with base xbase and ybase
	global logredraw;
	if argn()==0;
		p=plot();
	else
		if !holding();clg;endif;
		if iscomplex(x); xe=abs(x); else xe=x; endif;
		if iscomplex(y); ye=abs(y); else ye=y; endif;
		xl=logbase(xe,xbase);
		yl=logbase(ye,ybase);
		
		if holding() && !logredraw
			hd=holding();
			p=plot();
		else
			expix=floor(logbase(min(xe),xbase));
			expfx=ceil(logbase(max(xe),xbase));
			expiy=floor(logbase(min(ye),ybase));
			expfy=ceil(logbase(max(ye),ybase));

			xt=xlogticks(expix,expfx,xbase);
			yt=xlogticks(expiy,expfy,ybase);

			p=setplot([expix,expfx,expiy,expfy]);

			h=textheight();w=textwidth();

			hd=holding(1);
			ls=linestyle(".");
			lw=linewidth(1);
			c=color(1);

			for i=1 to length(xt)-xbase;
				plot([xt[i],xt[i]],[p[3],p[4]]);
			end;
			for i=expix to expfx;
				d=toscreen([i,p[3]]);
..				ctext(printf("%g",xbase)|"^"|printf("%g",i),[d[1],d[2]+h/2]);
				rtext(printf("%g",xbase),[d[1]+w,d[2]+0.75*h]);
				text(printf("%g",i),[d[1]+w,d[2]+h/4]);
			end;

			for i=1 to length(yt)-ybase;
				plot([p[1],p[2]],[yt[i],yt[i]]);
			end;
			for i=expiy to expfy;
				d=toscreen([p[1],i]);
..				rtext(printf("%g",ybase)|"^"|printf("%g",i),[d[1]-w/2,d[2]-h/2]);
				rtext(printf("%g",ybase),[d[1]-40,d[2]-h/2]);
				text(printf("%g",i),[d[1]-40,d[2]-h]);
			end;

			color(c);
			linewidth(lw);
			linestyle(ls);
			frame();
		endif;
		mark(xl,yl);
		holding(hd);
		logredraw=0;
	endif;
	return p;
endfunction;

function logspace(a,b,n=50,base=10)
## logspace(a,b,n) generates n log spaced points in [a,b].
## base specifies the based log used.
	if a>b
		tmp=a;
		a=b;
		b=tmp;
	endif;
	return base^(linspace(logbase(a,base),logbase(b,base),n-1));
endfunction


.. ### linear algebra things ###

function id (n)
## id(n) creates a nxn identity matrix.
	return diag([n n],0,1);
endfunction

function inv (a)
## inv(a) produces the inverse of a matrix.
	return a\id(cols(a));
endfunction

function fit (a,b)
## fit(a,b) computes x such that ||a.x-b||_2 is minimal.
## a is a nxm matrix, and b is a mx1 vector.
## For badly conditioned a, you should use svdsolve instead.
	A=conj(a');
	return symmult(A,a)\(A.b)
endfunction

function norm (A)
## Compute the maximal row sum of A
	return max(sum(abs(A))');
endfunction

function kernel (A)
## kernel(a) computes the kernel of the quadratic matrix a.
## You might add eps=..., if a is almost regular.
	if isvar("eps"); localepsilon(eps); endif;
	a=A/norm(A);
	{aa,r,c,d}=lu(a);
	n=size(a); nr=size(r);
	if nr[2]==n[2]; return zeros([1,n[2]])'; endif;
	if nr[2]==0; return id(n[2]); endif;
	c1=nonzeros(c); c2=nonzeros(!c);
	b=lusolve(aa[r,c1],a[r,c2]);
	m=size(b);
	x=zeros([n[2] m[2]]);
	x[c1,:]=-b;
	x[c2,:]=id(cols(c2));
	return x
endfunction

function image (A)
## image(a) computes the image of the quadratic matrix a.
	if isvar("eps"); localepsilon(eps); endif;
	a=A/norm(A);
	{aa,r,c,d}=lu(a);
	return a[:,nonzeros(c));
endfunction

function det (a)
## det(A) returns the determinant of A
	if isvar("eps"); localepsilon(eps); endif;
	r=norm(a);
	{aa,rows,c,d}=lu(a/r);
	return d*r^cols(a);
endfunction

function eigenremove(l)
## helping function.
	return l(nonzeros(!(l[1]~=l)))
endfunction

function eigenvalues (a)
## eigenvalues(A) returns the eigenvalues of A.
	return polysolve(charpoly(a));
endfunction

function eigenspace (a,l)
## eigenspace(A,l) returns the eigenspace of A to the eigenvaue l.
	k=kernel(a-l*id(cols(a)));
	if k==0; error("No eigenvalue found!"); endif;
	si=size(k);
	loop 1 to si[2];
		x=k[:,index()];
		k[:,index()]=x/sqrt(x'.x);
	end;
	return k;
endfunction

function eigen (A)
## eigen(A) returns the eigenvectors and a basis of eigenvectors of A.
## {l,x,ll}=eigen(A), where l is a vector of eigenvalues,
## x is a basis of eigenvectors,
## and ll is a vector of distinct eigenvalues.
	l=eigenvalues(A);
	s=eigenspace(A,l[1]);
	si=size(s); v=dup(l[1],si[2])'; vv=l[1];
	l=eigenremove(l,si[2]);
	repeat;
		if min(size(l))==0; break; endif;
		ll=l[1]; sp=eigenspace(A,ll);
		si=size(sp); l=eigenremove(l,si[2]);
		s=s|sp; v=v|dup(ll,si[2])'; vv=vv|ll;
	end;
	return {v,s,vv}
endfunction

hilbertfactor=3*3*3*5*5*7*11*13*17*19*23*29;

function hilbert (n,f=hilbertfactor)
## hilbert(n) produces the nxn-Hilbert matrix.
## It is accurate up to the 30x30 Hilbert matrix.
    {i,j}=field(1:n,1:n);
    return f/(i+j-1);
endfunction

function hilb (n,f=hilbertfactor)
	return hilbert(n,f);
endfunction

.. ### polynomial fit ##

function polyfit (xx,yy,n)
## fit(x,y,degree) fits a polynomial in L_2-norm to (x,y).
	A=ones(size(xx))_dup(xx,n); A=cumprod(A');
	return fit(A,yy')';
endfunction

.. ### Some Functions ###

function field (x,y)
## field(x,y) returns {X,Y} such that the X+i*Y is a rectangular
## grid in C containing the points x(n)+i*y(m). x and y must be
## 1xN and 1xM vectors.
	return {dup(x,cols(y)),dup(y',cols(x))};
endfunction

function totalsum (A)
## totalsum(a) computes the sum of all elements of a
	return sum(sum(A)');
endfunction

function totalmin (A)
## Returns the total minimum of the elements of a
	return min(min(A)')'
endfunction

function totalmax (A)
## Returns the total maximum of the elements of a
	return max(max(A)')'
endfunction

function sinh
## sinh(x) = (exp(x)-exp(-x))/2
	h=exp(arg1);
	return (h-1/h)/2;
endfunction

function cosh
## cosh(x) = (exp(x)+exp(-x))/2
	h=exp(arg1);
	return (h+1/h)/2;
endfunction

function arsinh
## arsinh(z) = log(z+sqrt(z^2+1))
	return log(arg1+sqrt(arg1*arg1+1))
endfunction

function arcosh
## arcosh(z) = log(z+(z^2-1))
	return log(arg1+sqrt(arg1*arg1-1))
endfunction

function logbase (x,a)
## returns the logarithm to base a
	return log(x)/log(a);
endfunction

function log10 (x)
## returns the logarithm to base 10
	return log(x)/log(10)
endfunction

function beta (a,b)
## returns the beta function
	return gamma(a)*gamma(b)/gamma(a+b);
endfunction

function betai (x,a,b)
## returns the incomplete beta function.
	return map("betai1",x,a,b);
endfunction

.. ### Differential equations ###

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

function bisect (ffunction,a,b)
## bisect("f",a,b;...) uses the bisection method to find a root of
## f(x,...) in [a,b]. ffunction may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	if isfunction(ffunction);
## function
	if ffunction(b,args())<0;
		b1=a; a1=b;
		if ffunction(a,args())<0 error("No zero in interval"); endif;
	else;
		a1=a; b1=b;
		if ffunction(a,args())>0 error("No zero in interval"); endif;
	endif;
	repeat
		m=(a1+b1)/2;
		if a1~=b1; break; endif;
		if ffunction(m,args())>0; b1=m; else a1=m; endif;
	end;
	else
## expression
	if expreval(ffunction,b)<0;
		b1=a; a1=b;
		if expreval(ffunction,a)<0 error("No zero in interval"); endif;
	else;
		a1=a; b1=b;
		if expreval(ffunction,a)>0 error("No zero in interval"); endif;
	endif;
	repeat
		m=(a1+b1)/2;
		if a1~=b1; break, endif;
		if expreval(ffunction,m)>0; b1=m; else a1=m; endif;
	end;
	endif;
	return m;
endfunction

function secant (ffunction,a,b)
## secant("f",a,b;...) uses the secant method to find
## a root of f(x,...) in [a,b]
## ffunction may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	if isfunction(ffunction)
## function
	x0=a; x1=b; 
	y0=ffunction(x0,args()); y1=ffunction(x1,args());
	repeat
		x2=x1-y1*(x1-x0)/(y1-y0);
		if x2~=x1; break; endif;
		x0=x1; y0=y1; x1=x2; y1=ffunction(x2,args());
	end;
	else
## expression
	x0=a; x1=b; 
	y0=expreval(ffunction,x0); y1=expreval(ffunction,x1);
	repeat
		x2=x1-y1*(x1-x0)/(y1-y0);
		if x2~=x1; break; endif;
		x0=x1; y0=y1; x1=x2; y1=expreval(ffunction,x2);
	end;
	endif;
	return x2
endfunction

function simpson (ffunction,a,b,n=50)
## simpson("f",a,b) or simpson("f",a,b,n;...) integrates 
## f(x,...) in [a,b] using the Simpson method.
## f must be able to evaluate a vector.
## ffunction may be an expression in x.
	t=linspace(a,b,2*n);
	if isfunction(ffunction);
		s=ffunction(t,args());
	else
		s=expreval(ffunction,t);
	endif;
	ff=4-mod(1:2*n+1,2)*2; ff[1]=1; ff[2*n+1]=1;
	return sum(ff*s)/3*(t[2]-t[1]);
endfunction

function romberg(ffunction,a,b,m=10)
## romberg(f,a,b) computes the Romberg integral of f(x,...) in [a,b].
## romberg(f,a,b,m;...) specifies h=(b-a)/m/2^k for k=1,...
## ffunction may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	if isfunction(ffunction)
		y=ffunction(linspace(a,b,m),args()); h=(b-a)/m;
	else
		y=expreval(ffunction,linspace(a,b,m)); h=(b-a)/m;
	endif
	y[1]=y[1]/2; y[m+1]=y[m+1]/2; I=sum(y);
	S=I*h; H=h^2; Intalt=S;
	repeat;
		if isfunction(ffunction)
			I=I+sum(ffunction(a+h/2:h:b,args())); h=h/2;
		else
			I=I+sum(expreval(ffunction,a+h/2:h:b)); h=h/2;
		endif;
		S=S|I*h;
		H=H|h^2;
		Int=interpval(H,interp(H,S),0);
		if Int~=Intalt; break; endif;
		Intalt=Int;
	end;
	return Intalt
endfunction

function iterate (ffunction,x0)
## iterate("f",x0;...) iterates the function f(x,...), starting from
## x0.
## The iteration stops at a fixed point.
## Returns the fixed point.
## ffunction may be an expression in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	x=x0;
	if isinterval(x);
		repeat
			if isfunction(ffunction); x=ffunction(x,args());
			else; x=expreval(ffunction,x);
			endif;
			if left(x)~=right(x); return x; endif;
		end;
	else;
		repeat
			if isfunction(ffunction); xn=ffunction(x,args());
			else; xn=expreval(ffunction,x);
			endif;
			if (x~=xn); return xn; endif;
			x=xn;
		end;
	endif;
endfunction

function niterate (ffunction,x0,n)
## iterate("f"x0,n;...) Iterate the function f(x,...) n times,
## starting with the point x0.
## Returns the vector of iterants.
## ffunction may be an expression in x.
	x=x0; y=zeros(n,cols(x));
	loop 1 to n
		if isfunction(ffunction); x=ffunction(x,args());
		else x=expreval(ffunction,x);
		endif;
		y[#,:]=x;
	end;
	return y;
endfunction

function newton (ffunc,fder,x)
## newton("f","df",x;...) seeks a zero of f(x,...).
## Starts the Newton iteration from x.
## df is the derivative of f.
## Additional parameters are passed to ffunc and fder.
## ffunc and fder may be expressions in x.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	repeat
		if isfunction(ffunc); a=ffunc(x,args());
		else a=expreval(ffunc,x);
		endif;
		if isfunction(fder); b=fder(x,args());
		else b=expreval(fder,x);
		endif;
		xnew=x-a/b;
		if (xnew~=x) return xnew; endif;
		x=xnew;
	end;
endfunction

function newton2 (fff,fff1,x)
## newton2("f","Df",x;...)
## Newton method for several parameter.
## fff is the function, fff1 computes the Jacobian.
## Additional parameters are passed to fff and fff1.
## You can specify an epsilon eps with eps=... as last parameter.
	if (isvar("eps")); localepsilon(eps); endif;
	repeat
		d=(fff1(x,args())\fff(x,args())')';
		xn=x-d;
		if xn~=x; return xn; endif;
		x=xn;
	end
endfunction

function evalfff (fff)
	useglobal;
	return evaluate(fff);
endfunction

function root (fff, x)
## Find the root of an expression fff (of string type)
## by changing the variable x. Other variables may be
## set globally. Note that the actual name of the variable
## x may be different from "x".
	if (isvar("eps")); localepsilon(eps); endif;
	if (x==0) x1=0.00001;
	else x1=1.00001*x;
	endif;
	x0=x;
	a=evalfff(fff);
	repeat
		x=x1; b=evalfff(fff);
		xn=(x0*b-x1*a)/(b-a);
		if xn~=x; break; endif;
		x0=x1; x1=xn; a=b;
	end;
	return x;
endfunction

function setupdif (n)
	A=zeros([n,2*n+1]);
	loop 1 to n;
	b=zeros([2*#+1,1]); b[#+1]=fak(#);
	a=((-#:#)^(0:2*#)')\b;
	A[#,1:2*#+1]=a';
	end;
	return A;
endfunction

DifMatrix=setupdif(5);

function dif (fff, x, n=1, e=epsilon()^(1/2))
## Compute the n-th derivative of fff in the points x.
## fff may be an expression in x or a function.
## For n>=2, x must be a 1xn vector.
	global DifMatrix;
	eps=e^(1/(n+1));
	if n==0;
		if isfunction(fff); fff(x); else return expreval(fff,x); endif;
	elseif (n==1)
		if isfunction(fff);
			return (fff(x+eps)-fff(x-eps))/(2*eps);
		else
			return (expreval(fff,x+eps)-expreval(fff,x-eps))/(2*eps);
		endif;
	elseif n<=5;
		h=(-n:n)*eps; xh=h'+x; a=DifMatrix[n,0:2*n+1];
		if isfunction(fff); return (a.fff(xh))/eps^n;
		else return (a.expreval(fff,xh))/eps^n;
		endif;
	else
		error("Too high a derivative!");
	endif;
endfunction

.. ### use zeros,... the usual way ###

function ctext
## ctext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Also ctext(s,c,r).
	if argn()==3; return _ctext(arg1,[arg2,arg3]);
	else return _ctext(arg1,arg2);
	endif;
endfunction

function text
## text(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Also ctext(s,c,r).
	if argn()==3; return _text(arg1,[arg2,arg3]);
	else return _text(arg1,arg2);
	endif;
endfunction

function vctext
## vtext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn downwards, centered.
	if argn()==3; return _vctext(arg1,[arg2,arg3]);
	else return _vctext(arg1,arg2);
	endif;
endfunction

function vrtext
## vtext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn downwards, right justified.
	if argn()==3; return _vrtext(arg1,[arg2,arg3]);
	else return _vrtext(arg1,arg2);
	endif;
endfunction

function vtext
## vtext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn downwards, left justified.
## Also vctext(s,c,r).
	if argn()==3; return _vtext(arg1,[arg2,arg3]);
	else return _vtext(arg1,arg2);
	endif;
endfunction

function vutext
## vutext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn upwards.
## Also vcutext(s,c,r).
	if argn()==3; return _vutext(arg1,[arg2,arg3]);
	else return _vutext(arg1,arg2);
	endif;
endfunction

function vcutext
## vutext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn upwards, centered.
## Also vcutext(s,c,r).
	if argn()==3; return _vcutext(arg1,[arg2,arg3]);
	else return _vcutext(arg1,arg2);
	endif;
endfunction

function vrutext
## vutext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn upwards, right justified.
## Also vcutext(s,c,r).
	if argn()==3; return _vrutext(arg1,[arg2,arg3]);
	else return _vrutext(arg1,arg2);
	endif;
endfunction

function diag
## diag([n,m],k,v) returns a nxm matrix A, containing v on its k-th
## diagonal. v may be a vector or a real number. Also diag(n,m,k,v);
## diag(A,k) returns the k-th diagonal of A.
	if argn()==4; return _diag([arg1,arg2],arg3,arg4);
	elseif argn()==3; return _diag(arg1,arg2,arg3);
	else return _diag(arg1,arg2);
	endif;
endfunction

function format
## format(n,m) sets the number output format to m digits and a total
## width of n.
	if argn()==2; return _format([arg1,arg2]);
	else return _format(arg1);
	endif;
endfunction

function goodformat
## goodformat(n,m) sets the number output format to m digits and a
## total width of n. Prints less than m digits, when not necessary.
	if argn()==2; return _goodformat([arg1,arg2]);
	else return _goodformat(arg1);
	endif;
endfunction

function expformat
## expformat(n,m) sets the number output format to m digits and a 
## total width of n. Always uses exponential format
	if argn()==2; return _expformat([arg1,arg2]);
	else return _expformat(arg1);
	endif;
endfunction

function fixedformat
## expformat(n,m) sets the number output format to m digits and a 
## total width of n. Always uses fixed point format
	if argn()==2; return _fixedformat([arg1,arg2]);
	else return _fixedformat(arg1);
	endif;
endfunction

function redim
## redim(A,[n,m]) returns a matrix with the numbers in A but different
## dimension filling with 0 if necessary. Also redim(A,n,m).
	if argn()==3; return _redim(arg1,[arg2,arg3]);
	else return _redim(arg1,arg2);
	endif;
endfunction

function normal
## normal([n,m]) returns a nxm matrix of unit normal distributed random
## values. Also normal(n,m).
	if argn()==2; return _normal([arg1,arg2]);
	else return _normal(arg1);
	endif;
endfunction

function random
## random([n,m]) returns a nxm matrix of uniformly distributed random 
## values in [0,1]. Also random(n,m).
	if argn()==2; return _random([arg1,arg2]);
	else return _random(arg1);
	endif;
endfunction

function ones
## ones([n,m]) returns a nxm matrix with all elements set to 1.
## Also ones(n,m).
	if argn()==2; return _ones([arg1,arg2]);
	else return _ones(arg1);
	endif;
endfunction

function zeros
## zeros([n,m]) returns a nxm matrix with all elements set to 0.
## Also zeros(n,m).
	if argn()==2; return _zeros([arg1,arg2]);
	else return _zeros(arg1);
	endif;
endfunction

function matrix
## matrix([n,m],x) returns a nxm matrix with all elements set to x.
## Also matrix(n,m,x).
	if argn()==3; return _matrix([arg1,arg2],arg3);
	else return _matrix(arg1,arg2);
	endif;
endfunction

function view
## view([distance, tele, angle1, angle2]) sets the perspective for
## solid and view. distance is the eye distance, tele a zooming factor.
## angle1 is the angle from the negativ y-axis to the positive x-axis.
## angle2 is the angle to the positive z-axis (the height of the eye).
## Also view(d,t,a1,a2).
## view() returns the values of view.
	if argn()==4; return _view([arg1,arg2,arg3,arg4]);
	else if argn()==1; return _view(arg1);
	else return _view();
	endif;
endfunction

function window
## window([c1,r1,c2,r2]) sets a plotting window. The coordinates must
## be screen coordinates. Also window(c1,r1,c2,r2).
## window() returns the active window coordinates.
	global logredraw;
	if argn()==4; logredraw=1; return _window([arg1,arg2,arg3,arg4]);
	else if argn()==1; logredraw=1; return _window(arg1);
	else return _window();
	endif;
endfunction

function unclip
## Clips to the complete graphics screen
	return _clip([0,0,1023,1023]);
endfunction	

function clip
## clip([c1,r1,c2,r2]) sets the clipping window. The coordinates must
## be screen coordinates. Also clip(c1,r1,c2,r2).
## clip() returns the active clipping window coordinates.
	if argn()==4; return _clip([arg1,arg2,arg3,arg4]);
	else if argn()==1; return _clip(arg1);
	else return _clip();
	endif;
endfunction

function menu ()
## menu(string1,string2,...) displays a menu and returns the
## number of the chosen item.
	loop 1 to argn();
		char(#+ascii("a")-1)|" : "|args(#),
	end;
	""
	"Please, press return to exit or any of these keys!"
	k=key();
	n=ascii("a");
	if k-n>=0 && k-n<=argn(); return k-n+1; endif;
	if k==13; return -1; endif;
	return 0;
endfunction

function drawbutton (text,x,y,w)
## draw a button with the text at x,y with
## w character width. x centered and y top
## of the label. return the button rectangle.
	st=barstyle("#O");
	bc=barcolor(0);
	tw=textwidth(); th=textheight();
	b=[x-tw*w/2,y-th/3,tw*w,th*5/3];
	bar(b);
	ctext(text,[x,y]);
	barstyle(st);
	barcolor(bc);
	return b
endfunction

function inbutton (b,s)
## test if the screen coordinates s are with in
## the button rectangle b. get s e.g. with
## toscreen(mouse())
	return s[1]>b[1] && s[1]<b[1]+b[3] && s[2]>b[2] && s[2]<b[2]+b[4];
endfunction

function printformat (x,f="%g",sep1=", ",sep2="; ")
## Print the vector x formatted with format f.
## See: printf
## The format is applied to real and imaginary part,
## or lower and upper part of the elements of x.
	s=size(x);
	loop 1 to s[1];
		i=#;
		loop 1 to s[2]
		if iscomplex(x);
			write(printf(f|sep1,re(x[i,#])));
			write(printf(f|sep2,im(x[i,#])));
		elseif isinterval(x);
			write(printf(f|sep1,left(x[i,#])));
			write(printf(f|sep2,right(x[i,#])));
		else write(printf(f|sep2,x[i,#]));
		endif;
		end;
		putchar(10);
	end;
	return "";
endfunction

function getmatrix (n,m,filename="")
## Read a real nxm Matrix from the file.
## If the filename is "", the file must be opened before.
## Else the file will open and close for read.
## Matrix values must be decimal fix point numbers with
## a dot (not a comma). They must be stored row after row.
	if filename<>""; open(filename,"r"); endif;
	{v,N}=getvector(n*m);
	if filename<>""; close(); endif;
	if (N<n*m); error("Incorrect values in file"); endif;
	v=v[1:n*m];
	return redim(v,n,m);
endfunction

function writematrix (v,filename="",format="%0.16f")
## Write a real nxm Matrix to the file row after row.
## If the filename is not "", the file will be created (beware!).
	if typeof(v)<>2; error("Can write only real matrices"); endif;
	if filename<>""; open(filename,"w"); endif;
	f=format|" ";
	for i=1 to rows(v);
		for j=1 to cols(v);
			write(printf(f,v[i,j]));
		end;
		putchar(10);
	end;
	if filename<>""; close(); endif;
	return "";
endfunction

function triangles
## Obsolete Function.
	return 1
endfunction

function upperwindow (title="")
## select the upper window for plot
	window(150,860,75,500);
	title(title);
	return title
endfunction

function lowerwindow (title="")
## select the upper window for plot
	window(150,860,575,1010);
	ctext(title,512,510);
	return title
endfunction

function caseone (b,x,y)
	if b return x; else return y; endif
endfunction

function case (b,x,y)
## case(condition,x,y)
## gibt x zurück, wenn condition!=null ist, sonst y.
	return map("caseone",b,x,y);
endfunction
