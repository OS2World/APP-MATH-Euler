comment
Histogram plots
endcomment

function histogram (d,n=10,grid=0,ticks=1,width=4,integer=0)
## Plots a histogram of the data d with n intervals.
## The minimal and maximal value of form an interval,
## which is divided into n points. The number of data
## in each of the n subintervals is displayed.
## d is an 1xm vector.
	mi=min(d); ma=max(d);
	if mi~=ma; ma=mi+1; endif;
	if integer; n=floor(ma-mi)+1; ma=ma+0.9999; endif;
	x=zeros(1,2*n+2); y=zeros(1,2*n+2);
	h=(d-mi)/(ma-mi)*n;
	c=count(h,n+1); c[n]=c[n]+c[n+1]; c=c[1:n];
	y[2:2:2*n]=c; y[3:2:2*n+1]=c;
	xx=linspace(mi,ma,n);
	x[2:2:2*n]=xx[1:n]; x[3:2:2*n+1]=xx[2:n+1];
	x[1]=mi; x[2*n+2]=ma;
	w=linewidth(width); xplot(x,y,grid,ticks); linewidth(w);
	return plot;
endfunction

