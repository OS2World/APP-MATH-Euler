.. Berechnet die Rendite eines Wertpapieres

function rendite (x)
## berechnet die Rendite von Zahlungen x zu Perioden 0,1,2,3,...
    if sum(x)<0; error("Rendite negativ?"); endif;
    q=polydif(x);
    c=1;
    repeat
        cnew=c-polyval(x,c)/polyval(q,c);
        if c~=cnew; break; endif;
        c=cnew;
    end;
    return (1/cnew-1)*100;
endfunction

function rendite2 (x,n)
## berechnet die Rendite von Zahlungen x zu gebrochenen Perioden n.
    if sum(x)<0; error("Rendite negativ?"); endif;
    k2=x*n; n2=n-1;
    k2=k2[2:length(x)]; n2=n2[2:length(x)];
    c=1;
    repeat
        cnew=c-sum(x*c^n)/sum(k2*c^n2);
        if c~=cnew; break; endif;
        c=cnew;
    end;
    return (1/cnew-1)*100;
endfunction

function anlageformel (f,k0,r,k1,n,i0,i1)
	return k0*f^n+r*f^i1*(f^(n-i0-i1+1)-1)/(f-1)+k1
endfunction

function anlage1 (kapital,rate,endkapital,laufzeit,start,halten)
	f=bisect("anlageformel",1.0000001,2;kapital,rate, ..
			endkapital,laufzeit,start,halten);
	return (f-1)*100;
endfunction

function anlage (kapital,rate,endkapital,laufzeit,start=1,halten=1)
## Berechnet die Rendite einer Anlage oder eines Darlehens in %.
## kapital : Zahlung zu Beginn
## rate : Zahlung zu Beginn von Periode "start"
##        bis Beginn Periode "laufzeit"-"halten"
## endkapital : Auszahlung bzw. Endkapital nach laufzeit
## Zahlung m?ssen negativ eingegeben werden !
## Die Rendite muá zwischen 0 und 100 % liegen.
## Beispiel: anlage(-104,8.75,100,8)
	return map("anlage1",kapital,rate,endkapital,laufzeit,start,halten);
endfunction

function effzins (kurs,laufzeit,zins)
## berechnet den effektiven Zins eines Wertpapieres.
## kurs und zins in %, laufzeit in Zinsperioden.
## z.B. effzins(104,8,8.75).
	return anlage(-kurs,zins,100,laufzeit,1,0);
endfunction

function rate (darlehen,laufzeit,zins,rest=0,faellig=1)
## Berechnet die Rate f?r ein Darlehen mit Restschuld.
## Der Zins wird in % angegeben.
## Die erste Zahlung wird nach der faellig-sten Zinsperiode f„llig.
## Die letzte Rate wird nach der n-ten Zinsperiode f„llig.
## Die Restschuld ist die Schuld nach der n-ten Periode.
    f=1+zins/100;
    return (darlehen*f^laufzeit-rest)*(f-1)/(f^(laufzeit-faellig+1)-1);
endfunction

function darlzins (darlehen,laufzeit,rate,rest=0,faellig=1)
## Berechnet den Zinssatz eines Darlehens in %.
## Die anderen Parameter sind wie bei "rate".
	return anlage(darlehen,-rate,-rest,laufzeit,faellig,0);
endfunction

function restschuld (darlehen,laufzeit,zins,rate,faellig=1)
## Berechnet die Restschuld.
## Die Parameter sind wie bei "rate".
	f=1+zins/100;
	return darlehen*f^laufzeit-rate*(f^(laufzeit-faellig+1)-1)/(f-1);
endfunction

function restschulden (darlehen,laufzeit,zins,rate,faellig=1)
## Berechnet einen Vektor mit Restschulden.
## Die Parameter sind wie bei "rate".
## Die Funktion kann nicht mit Vektoren aufgerufen werden.
	r=dup(darlehen,laufzeit+1)';
	f=1+zins/100;
	if faellig==0; r[1]=r[1]-rate; endif;
	loop 2 to laufzeit+1;
		r[#]=r[#-1]*f;
		if #>faellig;
			r[#]=r[#]-rate;
		endif;
	end;
	return r;
endfunction

function sparzins (rate,laufzeit,endkapital)
## Berechnet den effektiven Zins eines Sparvertrages.
    return darlzins(0,-rate,endkapital,laufzeit,0,1);
endfunction

function endkap (rate,laufzeit,zins)
## Berechnet das Endkapital eines Sparvertrages.
	p=1+zins/100;
    return p*rate*(p^laufzeit-1)/(p-1);
endfunction

"rendite(zahlungen) definiert."
"rendite2(zeiten,zahlungen) definiert."
"anlage(startkapital,rate,endkapital,laufzeit,start,halten) definiert"
"effzins(kurs%,laufzeit,zins%) definiert."
"rate(darlehen,laufzeit,zins%,rest=0,faellig=1) definiert."
"darlzins(darlehen,laufzeit,rate,rest=0,faellig=1) definiert."
"restschuld(darlehen,laufzeit,rate,faellig=1) definiert."
"restschulden(darlehen,laufzeit,rate,faellig=1) definiert."
"sparzins(rate,laufzeit,endkapital) definiert."
"endkap(rate,laufzeit,zins%) definiert."

"Benutzen Sie help ... f?r weitere Informationen."


