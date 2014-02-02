comment
Remez algorithm
endcomment

.. ### Remez algorithm ###

function remezhelp (x,y,n)
## {t,d,h}=remezhelp(x,y,deg) is the case, where x are exactly deg+2
## points.
	d1=interp(x,y); d2=interp(x,mod(1:n+2,2)*2-1);
	h=d1[n+2]/d2[n+2];
	d=d1-d2*h;
	return {x[1:n+1],d[1:n+1],h};
endfunction

remezeps=0.00001;

function remez(x,y,deg,tracing=0)
## {t,d,h,r}=remez(x,y,deg) computes the divided difference form
## of the polynomial best approximation to y at x of degree deg.
## remez(x,y,deg,1) does the same thing with tracing.
## h is the discrete error (with sign), and r the rightmost 
## alternation point, which is missing in t. To evaluate the
## polynomial in v use interpval(t,d,v).
	global remezeps;
	n=cols(x);
	ind=linspace(1,n,deg+1); h=0;
	repeat
		{t,d,hnew}=remezhelp(x[ind],y[ind],deg);
		r=y-interpval(t,d,x); hh=hnew;
		if tracing;
			plot(x,r); xgrid(x[ind],ticks=0,color=9);
			ygrid([-hnew,0,hnew],ticks=0);
			title("Weiter mit Taste"); wait(180);
		endif;
		indnew=ind;
		e=extrema(r[1:ind[2]]);
		if (hh<0);
			indnew[1]=e[2];
		else
			indnew[1]=e[4];
		endif;
		hh1=hh;
		loop 2 to deg+1;
			e=extrema(r[ind[#-1]:ind[#+1]]);
			if (hh>0);
				indnew[#]=e[2]+ind[#-1]-1;
			else
				indnew[#]=e[4]+ind[#-1]-1;
			endif;
			hh=-hh;
		end;
		e=extrema(r[ind[deg+1]:cols(r)]);
		if (hh>0);
			indnew[deg+2]=e[2]+ind[deg+1]-1;
		else
			indnew[deg+2]=e[4]+ind[deg+1]-1;
		endif;
		hh=-hh;
		mm=max(abs(r[indnew]));
		if (indnew[deg+2]<cols(r));
			e=extrema(r[ind[deg+2]:cols(r)]);
			if (hh>0)
				if (abs(r[e[1]])>mm);
					indnew=indnew[2:deg+2]|(e[2]+ind[deg+2]-1);
				endif;
			else
				if (abs(r[e[3]])>mm);
					indnew=indnew[2:deg+2]|(e[4]+ind[deg+2]-1);
				endif;
			endif;
		endif;
		mm=max(abs(r[indnew]));
		if (indnew[1]>1);
			e=extrema(r[1:ind[1]]);
			if (hh1>0)
				if (abs(r[e[1]])>mm); indnew=e[2]|indnew[1:deg+1]; endif;
			else
				if (abs(r[e[3]])>mm); indnew=e[4]|indnew[1:deg+1]; endif;
			endif;
		endif;
		ind=indnew;
		if (abs(hnew)<(1+remezeps)*abs(h)) break; endif;
		h=hnew;
	end;
	{t,d,h}=remezhelp(x[ind],y[ind],deg);
	if tracing;
		xplot(x,y-interpval(t,d,x));
		title("Weiter mit Taste"); wait(180);
	endif;
	return {t,d,h,x[ind[deg+2]]};
endfunction


