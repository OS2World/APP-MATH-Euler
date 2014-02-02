.. Bench function.

comment

Run: test

Times for bench

OS/2, 486 DX 50/2, 8 MB
  40   9  88 209  49 262 311 175 131 143 105 585
OS/2, P90, 16 MB
  35   4  64 113  45 123 141  92  79  88  62 216
Windows 95, P75, 32 MB
   3   4  42 110  22 111 134  72  63  69  50 208
Windows NT, K6-2 500, 128 MB
  0.5  0.4 4 10  2.4 10 12  7.4  5.6  6.3  5.2  129

Benchmark is running ...

endcomment

function leer ()
	return 0;
endfunction

leert=0;

function bench (ffunction)
## bench("f",...) returns the time needed by f in milliseconds.
## ... is passed to f.
	global leert;
	func=ffunction;
	t=time(); count=0;
	repeat;
		if (time()>t+5); break; endif;
		func(args(2)); count=count+1;
	end;
	argt=(time()-t)/count;
	return argt;
endfunction

leert=bench("leer");

function f1
	loop 1 to 1000;
	end;
	return 0;
endfunction

function f2
	loop 1 to 1000;
		endif;
	end;
	return 0;
endfunction

function f3
	x=0;
	loop 1 to 1000;
		x=3;
	end;
	return 0;
endfunction

function f4
	x=0;
	loop 1 to 1000;
		x;
	end;
	return 0;
endfunction

function f5
	A=zeros([2,2]);
	loop 1 to 1000;
		A=[1,2;3,4];
	end;
	return 0;
endfunction

function f6
	A=zeros([2,2]);
	loop 1 to 1000;
		A[1,1]=A[1,2];
	end;
	return 0;
endfunction

function f7
	loop 1 to 1000;
		v=1:10;
	end;
	return 0;
endfunction

function f8
	v=1:10;
	loop 1 to 1000;
		v{4}=5;
	end;
	return 0;
endfunction

function f9
	x=3;
	loop 1 to 1000;
		(x+x)*(x+x);
	end;
	return 0;
endfunction

function f10
	v=1:10;
	x=0;
	loop 1 to 1000;
		x=v{4};
	end;
	return 0;
endfunction

function f11
	v=1:10;
	x=0;
	loop 1 to 1000;
		v*v;
	end;
	return 0;
endfunction

function f12
	x=2;
	loop 1 to 1000;
		x=sqrt(sin(log(exp(x))));
	end;
	return 0;
endfunction

looptime=bench("f1");

function musec (ffunction)
	global looptime;
	res=bench(ffunction,args(2))*1000-looptime;
	return {printf("%12.3f musec ",res),res};
endfunction

function test
	global looptime;
	looptime=0;
	shortformat();
	{text,looptime}=musec("f1");
	text|" for one loop.",
	musec("f2")|" for: endif;",
	musec("f3")|" for: x=3;",
	musec("f9")|" for: (x+x)*(x+x);",
	musec("f4")|" for: x;",
	musec("f5")|" for: A=[1,2;3,4];",
	musec("f6")|" for: A[1,1]=A[1,2];",
	musec("f7")|" for: v=1:10;",
	musec("f8")|" for: v{4}=3;",
	musec("f10")|" for: x=v{4};",
	musec("f11")|" for: v*v;",
	musec("f12")|" for: x=sqrt(sin(log(exp(x))));",
	looptime=0;
	return 0;
endfunction

test();

