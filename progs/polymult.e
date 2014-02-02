comment
Multiplies two complex polynomials with FFT very fast
endcomment

function polymultfft (p,q)
	n=cols(p)-1; m=cols(q)-1; nm=n*m+1;
	k=2^(floor(log(nm)/log(2))+1);
	p1=fft(p|zeros(1,k-(n+1)));
	q1=fft(q|zeros(1,k-(m+1)));
	res=ifft(p1*q1);
	return res[1:nm];
endfunction

