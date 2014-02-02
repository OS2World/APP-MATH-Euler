comment
xlogplot, xylogplot defined
endcomment

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

function xlogplot(x=1,y=0,xbase=10)
## xlogplot draws y(x) in a semilog paper with base xbase
	if argn()==0;
		p=plot();
	else
	if !holding();clg;endif;
	if iscomplex(x); xe=abs(x); else xe=x; endif;
	if iscomplex(y); ye=abs(y); else ye=y; endif;
	xl=logbase(xe,xbase);
	expi=floor(logbase(min(xe),xbase));
	expf=ceil(logbase(max(xe),xbase));

	xt=xlogticks(expi,expf,xbase);
	yt=xticks(min(ye),max(ye));

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

	for i=1 to length(xt)-10;
		plot([xt[i],xt[i]],[p[3],p[4]]);
	end;

	for i=expi to expf;
		d=toscreen([i,p[3]]);
..		ctext(printf("%g",xbase)|"^"|printf("%g",i),[d[1],d[2]+h/2]);
		rtext(printf("%g",xbase),[d[1]+w,d[2]+0.75*h]);
		text(printf("%g",i),[d[1]+w,d[2]+h/4]);
	end;

	color(c);
	linewidth(lw);
	linestyle(ls);
	plot(xl,ye);
	holding(hd);
	endif;
	return p;
endfunction;

function ylogplot(x=0,y=1,ybase=10)
## xlogplot draws y(x) in a semilog paper with base ybase
	if argn()==0;
		p=plot();
	else
	if !holding();clg;endif;
	if iscomplex(x); xe=abs(x); else xe=x; endif;
	if iscomplex(y); ye=abs(y); else ye=y; endif;
	yl=logbase(ye,ybase);
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
	for i=1 to length(yt)-10;
		plot([p[1],p[2]],[yt[i],yt[i]]);
	end;
	for i=expi to expf;
		d=toscreen([p[1],i]);
..		rtext(printf("%g",ybase)|"^"|printf("%g",i),[d[1]-w/2,d[2]-h/2]);
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
	plot(xe,yl);
	holding(hd);
	endif;
	return p;
endfunction;

function xylogplot(x=1,y=1,xbase=10,ybase=10)
## xylogplot draws y(x) in a loglog paper with base xbase and ybase
	if argn()==0;
		p=plot();
	else
	if !holding();clg;endif;
	if iscomplex(x); xe=abs(x); else xe=x; endif;
	if iscomplex(y); ye=abs(y); else ye=y; endif;
	xl=logbase(xe,xbase);
	yl=logbase(ye,ybase);
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

	for i=1 to length(xt)-10;
		plot([xt[i],xt[i]],[p[3],p[4]]);
	end;
	for i=expix to expfx;
		d=toscreen([i,p[3]]);
..		ctext(printf("%g",xbase)|"^"|printf("%g",i),[d[1],d[2]+h/2]);
		rtext(printf("%g",xbase),[d[1]+w,d[2]+0.75*h]);
		text(printf("%g",i),[d[1]+w,d[2]+h/4]);
	end;

	for i=1 to length(yt)-10;
		plot([p[1],p[2]],[yt[i],yt[i]]);
	end;
	for i=expiy to expfy;
		d=toscreen([p[1],i]);
..		rtext(printf("%g",ybase)|"^"|printf("%g",i),[d[1]-w/2,d[2]-h/2]);
		rtext(printf("%g",ybase),[d[1]-40,d[2]-h/2]);
		text(printf("%g",i),[d[1]-40,d[2]-h]);
	end;

	color(c);
	linewidth(lw);
	linestyle(ls);
	plot(xl,yl);
	holding(hd);
	endif;
	return p;
endfunction;
