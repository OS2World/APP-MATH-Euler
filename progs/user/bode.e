.. diagramme de Bode

load logplot
j=1i; .. Before all we must define the pure imaginary

function fbode(n,d,w)
## Compute the value of a give function at the given w (omega)
##
## result = G(s) |       = n(s)/d(s) |
##               | s=j*w             | s=j*w
## 
## n=num. d=den., w=puls.

     global j;
     return (polyval(n , j*w) / polyval( d, j*w));

endfunction;


function arg2(x)
## return the fase but arg(-1)=-180 and NOT 180
## x = complex number
	if abs(x) > 0;
		pha=arg(x);
		if (im(x) ~= 0) && (sign(re(x)) ==-1) ;
			pha=-pi;
		endif;
	else;
		pha=0;
	endif;
	return pha;
endfunction;



function phase(n,d,w)
## computer Phase for Bode diagrams
##
## phase vector = Phase(n,d,[start..end]);
##
## n=num.
## d=den.
## w=range where compute phase

        p=0;
        z=0;
        pha=[ ];

        .. if den is a poly then solve its roots.

        if length(d)>1;
                p=polysolve(d);
        else;
                pha=arg2(d);
        endif;
    
        .. if num ... roots

        if length(n)>1;
                z=polysolve(n);
        else;
                pha=arg2(n);
        endif;

        .. check for s=0 poles
 
        ni=0;

        for k=1 to length(d)-1;
                if abs(p[k]) ~=0;
                        ni=ni+1;
                endif;
        end;
        if  ( length(d)==(ni+1) ); .. if there are only poles
                gain=fbode(n, 1,0);
        else;
                gain=fbode( n, polycons(p[ni+1:length(p)]) ,0 );
        endif;

        .. adding all contrib.
 
        for k=1 to length(n)-1;
                if (re( z[k] ) > 0) && (im(z[k])~=0);
                        ad=-pi;
                else
                        ad=0;
                endif;
                pha=pha+arg2( fbode( polycons( z[k] ) , 1 ,w) )+ad;

        end; .. for ..

        for k=1 to length(d)-1;
                if (re(p[k])>0) && (im(p[k]) ~=0);
                        ad=pi;
                else
                        ad=0;
                endif;
                pha=pha+arg2(fbode(1,polycons(p[k]),w))+ad;
        end;
        return pha+arg2(gain);
endfunction;



function bode(n,d,expi=-2,expf=2,pt=100,width=1,what=0)
## draw the bode's diagrams of a given G(s)=n(s)/d(s)
##
## Example:
## bode(n,d,floor(logbase(10,10)),ceil(logbase(1892,10)));
##
## draw the diagrams from 10 and 1000
## n=num
## d=den
## wi=omega start at wi = base^expi, expi MUST BE INTEGER!
## wf=omega end at   wf = base^expf, expf must be integer!
## pt=samples
## what = 0 magnitude and phase both on the same graphic.
##        1 magnitude only
##        2 phase only
## width

	w   = (10)^linspace(expi,expf,pt);
	gdb = 20*logbase(abs(fbode(n,d,w)),10);
	ph  = phase(n,d,w);
	h=textheight();b=textwidth();

	if what==0;
		dh=floor((1023-7.5*h)/2);
		window([8*b,1.5*h,1023-2*b,1.5*h+dh]);
		xlogplot(w,gdb,10);
		hd=holding(1);
		window([8*b,4.5*h+dh,1023-2*b,1023-3*h]);
		xlogplot(w,ph/pi*180,10);
		holding(hd);
	elseif what==1;
		shrinkwindow();
		xlogplot(w,gdb,10);
	else
		shrinkwindow();
		xlogplot(w,ph/pi*180,10);
	endif;
	return 0;
endfunction;
