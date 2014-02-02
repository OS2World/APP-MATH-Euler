comment
Save and load WAV sound files. Analyze frequencies.

{w,rate}=loadwave(filename); loads a WAV file.
w=loadwave(filename); loads a WAV file. r is saved to defaultrate.
savewave(filename,v[,rate,bits]); saves a WAV file.
analyze(v[,fmin,fmax,rate,points]); plots the frequencies of v
mapsound(w,[,dt,fmin,fmax,simpl,rate]); show the sound history
soundsec(n[,rate]); generates t values for n seconds.

Default sample rate is 22050 or the sample rate of the last read WAV file.
endcomment

defaultrate=22050;

function soundsec (n,rate=0)
## Return n seconds of t parameter.
	global defaultrate;
	if rate==0; rate=defaultrate; endif;
	return 0:2*pi/rate:2*n*pi;
endfunction

function savewave (filename,v,rate=0,bits=16)
## Save a vector of sound data in WAV format.
## Return the length of the sample in seconds.
## If rate=0, the default sampling rate is taken.
## bits may be 8 or 16.
	global defaultrate;
	if rate==0; rate=defaultrate; endif;
	open(filename,"wb");
	write("RIFF");
	if bits==8;
		n=cols(v);
		putlongword(n+37);
		write("WAVEfmt ");
		putlongword(16); putword(1); putword(1);
		putlongword(rate); putlongword(rate);
		putword(1); putword(8);
		write("data");
		putlongword(n);
		m=max(abs(v)); vv=v/m*64+128;
		putuchar(vv);
		putchar(0);
	elseif bits==16;
		n=cols(v);
		putlongword(2*n+36);
		write("WAVEfmt ");
		putlongword(16); putword(1); putword(1);
		putlongword(rate); putlongword(2*rate);
		putword(2); putword(16);
		write("data");
		putlongword(2*n);
		m=max(abs(v)); vv=v/m*64*256;
		putword(vv);
	else error("Bits must be 8 or 16");
	endif
	close();
	return cols(v)/rate;
endfunction

function loadwave (filename)
## Read a WAV file.
## The sample rate is stored to defaultrate.
	global defaultrate;
	open(filename,"rb");
	if getstring(4)<>"RIFF"; error("No Wave file!"); endif;
	getlongword(); .. This is the total file length minus 8
	if getstring(8)<>"WAVEfmt "; error("No Wave file!"); endif;
	offset=getlongword(); .. ??? Is always 16, sometimes 18
	getword(); .. ??? Is always 1.
	if (getword()<>1); error("Stereo sample!"); endif;
	rate=getlongword(); getlongword();
	byte=getword();
	bits=getword();
	if offset>16; getuchar(offset-16); endif;
	if getstring(4)<>"data"; error("No Wave file!"); endif;
	if byte==1;
		w=getuchar(getlongword());
	elseif byte==2;
		w=getword(getlongword()/2);
	else error("Not 8 or 16 bit!");
	endif;
	if cols(w)>0; w=w-sum(w)/cols(w); w=w/max(abs(w)); endif;
	close();
	defaultrate=rate;
	return w;
endfunction

function analyze (w,fmin=10,fmax=1500,rate=0,points=8192)
## Make a frequency plot of the signal w with sampling rate.
## The data must be at least points long.
## The maximal frequency plotted will be fmax, the minimal fmin.
	global defaultrate;
	if rate==0; rate=defaultrate; endif;
	v=w[1:points];
	f=abs(fft(v));
	i=fmin/rate*points:fmax/rate*points;
	setplot(fmin,fmax,0,max(f[i]));
	fr=i/points*rate;
	xplot(fr,f[i]);
	return "";
endfunction

function mapsound (w,dt=0.1,fmin=100,fmax=1500,simpl=1,rate=0)
## Plots a sound map for a sound.
## It does FFT at time increments dt.
## rate is the sampling rate.
## simpl points are collected for speed reasons.
	n=cols(w);
	global defaultrate;
	if rate==0; rate=defaultrate; endif;
	dp=dt*rate;
	points=2^floor(log(dp)/log(2));
	ind=fmin/rate*points/simpl:fmax/rate*points/simpl;
	f=abs(fft(w[1:points]));
	f=sum(redim(f,points/simpl,simpl))';
	M=f[ind];
	i=1;
	repeat
		i=i+dp;
		if i+points>n; break; endif;
		f=abs(fft(w[i:i+points-1]));
		f=sum(redim(f,points/simpl,simpl))';
		M=M_f[ind];
	end;
	setplot(fmin,fmax,0,n/rate);
	density(-M,0.99);
	xplot();
	return ""
endfunction


