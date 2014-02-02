

function saite0 (x,t)
c=0;
a=2*pi;
if x>0 && x <t;
c= x/t;
elseif x <a && x > t;
c= (a-x)/(a-t);
endif;
return c;
endfunction



function saite(x,t)
return map("saite0",x;t);
endfunction;



function afunct (x,n)
return sin(n*x/2.0)*f(x);
endfunction;



function an1(n)
return simpson("afunct",0,2*pi;n)/pi;
endfunction;

function an(n)
    return map("an1",n);
endfunction;


function makesound(an,tg,name)
    n=length(an);
    t=0:1/22050:tg;
    om=2*pi*220;
    w=0*t;
    for i=1 to n;
       w=w+an[i]*sin(i*om*t);
    end;
    savewave(name,w);
    return 0;
endfunction;


function calcfuncts(fn)
    n=length(fn);
    x=(0:5e-3:2*pi);
    f=ones(n+1,length(x));
    f[1;]=fn[1]*sin(1*x/2.0);
    for i=2 to n
        f[i;]=f[i-1;]+fn[i]*sin(i*x/2);
    end;
    f[n+1;]=f(x);
    plot(x,f);shg;
    return 0;
endfunction;