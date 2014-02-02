comment
Vertical grid ticks.
endcomment

function xgrid1(xx,ss,f=1,grid=1,ticks=1,color=3)
## xgrid([x0,x1,...]) draws vertical grid lines on the plot window at
## x0,x1,...
## xgrid([x0,x1,...],f) additionally writes x0/f to the axis.
## The ticks have name ss.
	c=plot(); n=cols(xx); s=scaling(0); h=holding(1);
	w=window();
	st=linestyle("."); color(color);
	ht=textheight();
	loop 1 to n;
		x=xx[index()];
		s=ss[index()];
		if (x<=c[2])&&(x>=c[1]); 
			if grid; plot([x,x],[c[3],c[4]]); endif;
			if ticks;
				col=w[1]+(x-c[1])/(c[2]-c[1])*(w[3]-w[1]);
				ctext(niceform(s/f),[col,w[4]+0.2*ht]);
			endif;
		endif;
	end;
	if ticks && !(f~=1);
		ctext("* "|printscale(f),[(w[1]+w[3])/2,w[4]+1.5*ht]);
	endif;
	linestyle(st); color(1); holding(h); scaling(s);
	return 0;
endfunction

function ygrid1(yy,ss,f=1,grid=1,ticks=1,color=3)
## ygrid([x0,x1,...]) draws horizontal grid lines on the plot window at
## x0,x1,...
## ygrid([x0,x1,...],f) additionally writes x0/f to the axis.
	global vertical
	c=plot(); n=cols(yy); s=scaling(0); h=holding(1);
	st=linestyle("."); color(color);
	w=window(); wd=textwidth(); ht=textheight();
	loop 1 to n;
		y=yy[index()];
		s=ss[index()];
		if (y>=c[3])&&(y<=c[4]);
			if ticks;
				row=w[4]-(y-c[3])/(c[4]-c[3])*(w[4]-w[2]);
				if vertical;
					vcutext(niceform(s,f),[w[1]-0.2*ht,row]);
				else
					rtext(niceform(s/f),[w[1]-wd/2,row-ht/2]);
				endif;
			endif;
			if grid; plot([c[1],c[2]],[y,y]); endif;
		endif;
	end;
	if ticks && !(f~=1);
		text("* "|printscale(f),[w[1]-6*wd,0]);
	endif;
	linestyle(st); color(1); holding(h); scaling(s);
	return 0;
endfunction


