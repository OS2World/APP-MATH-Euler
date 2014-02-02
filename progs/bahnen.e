function bahn (x,u,dt,n)
res=[x];
loop 1 to n;
ax=x;
x=x+u*dt+ax*dt^2/2;
u=u+ax*dt;
res=res_[x];
end;
return res;
endfunction

n=100;
res=bahn(1,0,1/n,n)';
xplot(1:n+1,res);
res[n+1],
cosh(1)

