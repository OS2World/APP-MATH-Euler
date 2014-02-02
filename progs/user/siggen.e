comment
Funktionsname siggen1
Generiern eines Sinussignals mit fester Anzahl der
Punkte pro Periode
amplitude - Amplitude des Signals
phase     - Phasenverschiebung
freq      - Schwingungen innerhalb ppunkte
samples   - Anzahl der Punkte des Endsignals
            Wenn freq=1, dann gibt samples die
            Stuetzstellenzahl pro Schwingung an
faktor    - Multiplikator der Samples/Perioden fuer komfortable
handhabung bei freq=1 inpi      - Pi, mit dem die Funktion rechnen
soll => z.B. pi() 
endcomment
function siggen1 (amplitude=1,phase=0,freq=1,samples=100,faktor=1,inpi=3.14)
 samples=samples*faktor;
 freq=freq*faktor;
 lambda=2*inpi*freq/samples;
 n=[0:(samples-1)];
 y=amplitude*sin(lambda*n+phase*2*inpi/360);
 xplot(n,y);
 return y;
endfunction
