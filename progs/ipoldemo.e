comment
demo of xpolyval
This file computes the correct values of a badly
conditioned polynomial, after showing that the
normal values are completely wrong.
endcomment

comments off
load "x.e"

p=[-945804881,1753426039,-1083557822,223200658];
t=linspace(1.61801916,1.61801917,100);
s=polyval(p,t);
xplot(t-1.61801916,s);
title("Incorrect polynomial values"); wait(180);

s=xpolyval(p,t);
xplot(t-1.61801916,s);
title("Correct polynomial values"); wait(180);
