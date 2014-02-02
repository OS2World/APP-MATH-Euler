.. ==========================================================================
..              Library for Theory systems and Automatic controls

..                            Version 0.999 BETA


..                           By Palma Alessio 1996.

..    This software is FREEWARE, copy and spread it but not modify NOTHING
            
..===========================================================================


j=sqrt(complex(-1)); .. Before all we must define the pure imaginary


"-< CONTROL V0.999>-  for Dr. Grothmann's EULER."
"Made by P.Alessio, University of L'Aquila (26 oct 1996)"
"------------------------------------------------"
"Bugs report and new idea:"
"                               Palma Alessio"
"                               Via Musone 29"
"                               65129 Pescara"
"                               Italy"


"j pure complex number definited"
"Wait the loading please.."



function fbode(n,d,w)
## Compute the value of a give function at the given w (omega)
##
## result = W(S) | 
##               | s=j*W
## 
## n=num. d=den., w=puls.

     global j;
     return (polyval(n , j*w) / polyval( d, j*w));

endfunction;




function xgridlog(inizio,fine,base=10,rn=1,d=1)
## Draw a Bode's Paper. there are not help! This because I'll have in
## project its full rewrite.

        nd=abs(fine-inizio);

        x=[ ]; 

        for k = 0 to nd - 1;
                x = x | logbase(1:base,base) + k;
        end


        coords=setplot(); 
        coords{1}=0;  
        coords{2}=nd; 
                     
        setplot( coords );

        xgrid(x,ticks=0);



        dots=window();
                      

        h = textheight();  w = textwidth();

        ordinate = abs(dots[4] - dots[2]) - h;

        nvalori = floor(ordinate / (d*h)); 
        intervallo = abs(coords[4]-coords[3]);
        passo  = intervallo / nvalori; 
        passod = ordinate / nvalori; 

        hl=holding(1);
        st=linestyle("."); 
        colore=color(3);
 
       for k=0 to nvalori;
                row=dots[2] + (passod * k);
                va=coords[4] - passo * k;
                rtext(printf("%g",round(va,rn)),[ dots[1]-(w/2), row]);
                plot(  [0 , nd] ,  [va,va] );
        end;


        passox=floor( abs( dots[1] - dots[3] ) / nd);

        for k=0 to nd;
                ctext( printf("%g",(inizio+k)), [ dots[1] + (k*passox) , dots[4] ]);
        end;

        holding(hl);
        linestyle(st);
        color(colore);

        return 0;

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
                if ( re(p[k])>0) && ( im(p[k]) ~=0);
                        ad=pi;
                else
                        ad=0;
                endif;
                pha=pha+arg2( fbode( 1 , polycons( p[k] ) , w) )+ad;
        end;
       
        return pha+arg2(gain);

endfunction;




function pbode(n,d,expi=-2,expf=2,pt=100,base=10,width=1,what=1)
## draw the bode's diagrams of a given W(s)
##
## Example:
## {Magnitude,Phase}=Pbode(n,d,int(logbase(10,10)),int(logbase(1892,10))...);
##
## draw the digrams from 10 and 1000
## n=num
## d=den
## wi=omega start at wi = base^expi, expi MUST BE INTEGER!
## wf=omega end at   wf = base^expf, expf must be integer!
## pt=samples
## base = 10
## what = 1 magnitude and phase both on the same graphic.
##        2 fase only
##        3 magnitude only
##        0 BEFORE Phase and then Magnitude one for time.
## width

        wwv=base^linspace(expi,expf,pt);

        t=1:length(wwv);

        setplot(); .. autoscaling on

        ma=20*logbase(abs(fbode(n,d,wwv)),base);
        pha=phase(n,d,wwv);

        oldwindow=window(); .. save graphic windows status

    ..  * Magnitude of function transfer in decibels

        if what == 1;
                window(96,10+textheight(),1000,500);
                t1="Magnitude (dB), Phase (Deg)";
                t2="";
        else
                t1="Magnitude (dB)";
                t2="Phase (Deg)";
        endif;

        if (( what==0) || ( what == 3)) || (what == 1 );
                old=linewidth(width);
                plot(t,ma);
                linewidth(old);
                title(t1);
                hold on;
                xgridlog(expi,expf,base); .. Bode paper, of course...
                hold off;
        endif;

        if what == 1;
                hold on;
        else if what == 3;
                       wait();
        endif;
        endif;

..  * and Phase of it

        if what == 1;
                window(96,502+textheight(),1000,1024-textheight());
        endif;

        if ( ( what==0) || (what==2) ) || (what == 1);
                old=linewidth(width);
                plot(t,pha*180/pi);
                linewidth(old);
                title(t2);
                hold on;
                xgridlog(expi,expf); .. Bode paper, of course...
                hold off;
                wait();
        endif;

        hold off;
        window(oldwindow);

        return 0;

endfunction;



function place(n,d,zita=0,alfa=0,pstep=.1,kp=40,mstep=.1,km=40)
## Draw the root's place of a given W(s) transfer function
##
## mark the root of  p(s,k) = Nw(s)+k Dw(s)
##
## when k go from -ABS(km) to 0 Negative place
## and   k go from 0 to ABS(kp) Positive place
##
## zita and alfa are use for show forbitten zone of complex plain
## n=num. d=den.
## kp = ABSolute ending value of the k-parameter for the positive place
## km = ABSolute ending value of the k-parameter for the negative place
## mstep = Negative increasing step
## pstep  = Positive Inc. step.


        kplus=0:pstep:kp;   .. Where solve p(s,k) for pos. place
        kmin=0:mstep:km;    .. and neg. place.

        Lplus=[];      .. roots list for k+
        Lmin=[];       .. and k-
 
        zeri=[ ]; .. zeros of Nw(s)
        poli=[ ]; .. zeros of Dw(s) (poles)


        .. If there are zeros compute they

        if length(n)>1;
                zeri=polysolve(n);
        endif;

        .. and, if there are, poles

        if length(d)>1;
                poli=polysolve(d);
        endif;

        ..------------------------------------------
        ..    All points of the place pos. and neg.
        ..------------------------------------------

        .. k+ dots

        for i=1 to length(kplus);
                Lplus=Lplus | polysolve(polyadd(d,kplus[i]*n));
        end;

        .. k- dots

        for i=1 to length(kmin);
                Lmin= Lmin  | polysolve(polyadd(d,-kmin[i]*n));
        end;

        ..------------------------------------------
        ..               Plot Negative place
        ..------------------------------------------

        plotplace("m.","Negative place",zeri,poli,Lmin);
    
        ..--------------------------------------------------------
        ..  Positive place, and hi-fi copy of the neg. procedure
        ..--------------------------------------------------------
 
        plotplace("m.","positive place",zeri,poli,Lplus);
 
        return 0;

endfunction;








function plotplace(marchio,titolo,zeri,poli,lzeri)
## this function is called from place.
## internal function, NO Help sorry.

        correzione=[];

        disegnaZeri = 0;  .. if 1 then there are zeros to plot
        disegnaPoli = 0;  .. the same but for the poles

        markerstyle(marchio);

       .. scaling setup

        if length(zeri)>0;
                correzione=correzione | zeri;
                disegnaZeri=1;
        endif;

        if length(poli) > 0;
                disegnaPoli=1;
                correzione=correzione | poli;
        endif;

        ..if correzione is empty we must change the way

        if length(correzione) > 0;
               c=xmark(lzeri|correzione);
        else
               c=xmark(lzeri);
        endif;

        hold on; 

        ..can we draw the zeros?
 
        if disegnaZeri;
                markerstyle("mo");
                xmark(zeri);
        endif;
 
        ..  poles ?

        if disegnaPoli;
                markerstyle("m*");
                xmark(poli);
        endif;

        title(titolo);
 
        .. ----------------------
        ..      mark place
        .. ----------------------
        markerstyle("m+");
        repeat
                x=mouse();

                x
               
                if (x[1]>c[2]);
                        break;
                endif;
                if (x[1]<c[1]);
                        break;
                endif;
                if (x[2]>c[4]);
                        break;
                endif;
                if (x[2]<c[3]);
                        break;
                endif;

                mark(x[1],x[2]);
        end;

        hold off;

        return 0;

endfunction;

.. ---------------------------------------------------------
"Ok."

