form=shortformat();

"Test suite for EULER implementations."
""

function isone (x)
	if !(any(x)!=1); "passed",
	else "*************** failed ****************",
	endif;
	return ""
endfunction

"--- Linear Sytems"
a=random(5,5);
isone(a\sum(a)~=1);
isone(ones(5,1) <<= (~a~\~sum(a)~));
a=a+random(5,5)*1i;
isone(a\sum(a)~=1);

"--- Intervals",
x=dup([~-2,-1~,~-2,3~,~1,2~],3); y=x'; z=x*y;
h1=left(x)*left(y); h2=right(x)*right(y);
h3=left(x)*right(y); h4=right(x)*left(y);
isone(left(z)<>min(min(min(h1,h2),h3),h4));
isone(right(z)<>max(max(max(h1,h2),h3),h4));

"--- Accumulator"
x=exp(random(1,100)*30); x=x|(-x)|1;
isone(accuload(x));
accuload(x,x);
isone(accuadd(x,-x)==0);
x=exp(random(1,10)*60); x=x|(-x)|1;
isone(accuload(x));
isone(middle(accuload(~x~))==1);
x=exp(random(1,10)*60)+1i*exp(random(1,10)*60);
isone(abs(accuload(x|-x|1))==1);
isone(abs(accuload(x|x,-x|x))==0);

"--- Residuum"
{i,j}=field(1:8,1:8); h=fak(15)/(i+j-1); b=sum(h); x=h\b;
x=x-h\residuum(h,x,b); x=x-h\residuum(h,x,b);
isone(x~=ones(8,1));

"--- Functions"
function test
	i=0;
	loop -10 to 11;
		i=i+#;
	end;
	return i;
endfunction
isone(test()==11);
function test
	i=0;
	for j=-10 to 11 step 1;
		i=i+j;
	end;
	return i;
endfunction
isone(test()==11);
function test
	i=0;
	j=-10;
	repeat;
		i=i+j;
		if j>=11; return i; endif;
		j=j+1;
	end;
endfunction
isone(test()==11);
function test (k=3)
	l=6;
	if (k==3); return 1; endif;
	if (k<2); return 2;
	else
		if (k>4);
			if k>5; return 3; endif;
		else l=5;
		endif;
	endif
	return l;
endfunction
isone([test()==1,test(3)==1,test(1)==2, ..
test(3.5)==5,test(6)==3,test(4.5)==6]);

"--- Strings"
isone(''Affe''<"Berta");
isone("Affen">"Affe");
isone("test"==''test'');
function test
	return ''3+4*''=="3+4*"
endfunction
isone(test());

"--- Operators"
p=1:5; isone(p[1]==p[1]); isone(p[1]<p[2]); isone(p[1]<>p[2]);
isone(!(~1,2~<~1,3~)); isone(~2,3~>~0,1~); isone(1+0i==1);
isone((pi()<4)&&(pi>3));

"--- FFT and Complex Polynomials"
z=exp(2*Pi*(0:15)/16);
isone(polyval(1:16,z)~=fft(1:16));
p=random(1,256)+1i*random(1,256);
isone(p~=ifft(fft(p)));
isone(p~=fft(ifft(p)));

"--- Statistics"
n=10000; h=normal(1,n);
isone(abs(sum(h)/n)<=0.1);
isone(abs(sum(h*h)/n-1)<=0.1);
t=-2:0.1:2;
isone(abs(invnormaldis(normaldis(t))-t)<0.001);

format(form);

.. EOF

