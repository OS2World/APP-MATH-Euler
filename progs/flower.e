.. recursive flower

function flower (x=0,y=0,a=90,l=1,dl=0.8,n=5,d=10,t=0.98)
	r=random([1,5]);
	if n==0 || r[5]>t;
		style("m<>"); mark([x],[y]);
	else
		a1=a-(1+r[1]/2)*d;
		l1=l*(1+r[2]/2);
		x1=x+cos(a1*pi/180)*l1;
		y1=y+sin(a1*pi/180)*l1;
		plot([x,x1],[y,y1]);
		flower(x1,y1,a1,dl*l,dl,n-1,d);
		a1=a+(1+r[3]/2)*d;
		l1=l*(1+r[4]/2);
		x1=x+cos(a1*pi/180)*l1;
		y1=y+sin(a1*pi/180)*l1;
		plot([x,x1],[y,y1]);
		flower(x1,y1,a1,dl*l,dl,n-1,d);
	endif;
	return "";
endfunction

setplot(-3,3,-1,5); clg; hold on; flower(); hold off; wait(180);
