comment
2D plots
endcomment

logredraw=1;
subwindows=zeros([20,4]);

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
	if (tick>0.00001) && (tick<10000); tick=1; endif;
	return {(round(a/tick1,0):round(b/tick1,0))*tick1,tick}
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
		ho=holding(1); plot(xh,y); holding(ho);
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

function fplot (ffunction,a=0,b=0,n=200)
## fplot("f",a,b,n;...) plots the function f(x,...) in [a,b].
## fplot("f") or fplot("f",,,n,...) plots f in the old interval.
## fplot uses map for the evaluation of the function "f".
## ffunction may be an expression in x.
	if a~=b; s=plot(); a=s[1]; b=s[2]; endif;
	t=linspace(a,b,n);
	if isfunction(ffunction); s=map(ffunction,t;args());
	else s=expreval(ffunction,t);
	endif;
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
	ee=(c[2]-c[1])/1000;
	loop 1 to n;
		x=xx[#];
		if (x<=c[2]+ee)&&(x>=c[1]-ee); 
			if grid; _plot([x,x],[c[3],c[4]]); endif;
			if ticks;
				col=w[1]+(x-c[1])/(c[2]-c[1])*(w[3]-w[1]);
				stra=niceform(xt[#]/f);
				if stra!=str || #==n;
					_ctext(stra,[col,w[4]+0.2*ht]);
					str=stra;
				endif
			endif;
		endif;
	end;
	if ticks && !(f~=1);
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
	ee=(c[4]-c[3])/1000;
	loop 1 to n;
		y=yy[#];
		if (y>=c[3]-ee)&&(y<=c[4]+ee);
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
	text(printf("%0.10g",c[1]),[w[1],w[4]+0.2*ht]);
	rtext(printf("%0.10g",c[2]),[w[3],w[4]+0.2*ht]);
	holding(h); scaling(s);
	return 0;
endfunction

function xlabel(text,l=1)
## Puts the label text at the x-axis at point l in [0,1].
	w=window();
	ht=textheight();
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

function triangle
## Obsolete function
	return 	1
endfunction

function fwebplot (fff, a, b, xstart, n)
## plots an iteration of fff on [a,b] starting
## from xstart, n steps
## Example: fwebplot("cos(x)",0,1,0.5,10)
	fplot("x",a,b);
	hold on; color(9); fplot(fff,a,b;args()); color(1); hold off;
	y=niterate(fff,xstart,n+1;args());
	v=[xstart,0,xstart]|redim(dup(y[1:n],4),[1,4*n])|[y[n+1]];
	v=redim(v,[cols(v)/2,2])';
	hold on; color(10); plot(v[1],v[2]); color(1); hold off;
	wait(180);
	return y;
endfunction
