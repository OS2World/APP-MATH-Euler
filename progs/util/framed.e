comment
framed views
endcomment

function getframe (x,y,z)
## gets a box around all points in (x,y,z).
	ex=extrema(x); ey=extrema(y); ez=extrema(z);
	return [min(ex[:,1]'),max(ex[:,3]'),
		min(ey[:,1]'),max(ey[:,3]'),min(ez[:,1]'),max(ez[:,3]')]
endfunction

function framez0 (f)
	wire([f[1],f[2],f[2],f[1],f[1]], ..
		[f[3],f[3],f[4],f[4],f[3]],dup(f[5],5)');
	return 0	
endfunction

function framez1 (f)
	wire([f[1],f[2],f[2],f[1],f[1]], ..
		[f[3],f[3],f[4],f[4],f[3]],dup(f[6],5)');
	return 0
endfunction

function framexpyp (f)
	wire([f[2],f[2]],[f[4],f[4]],[f[5],f[6]]);
	return 0
endfunction

function framexpym (f)
	wire([f[2],f[2]],[f[3],f[3]],[f[5],f[6]]);
	return 0
endfunction

function framexmyp (f)
	wire([f[1],f[1]],[f[4],f[4]],[f[5],f[6]]);
	return 0
endfunction

function framexmym (f)
	wire([f[1],f[1]],[f[3],f[3]],[f[5],f[6]]);
	return 0
endfunction

function frame1 (f)
## draws the back part of the box (considering view)
	v=view();
	y=sin(v[4])*v[1];
	if y>f[5]; framez0(f); endif;
	if y<f[6]; framez1(f); endif;
	x=sin(v[3])*v[1]; y=-cos(v[3])*v[1];
	if x<=f[2]; framexpyp(f); framexpym(f); endif;
	if x>=f[1]; framexmyp(f); framexmym(f); endif;
	if y<=f[4]; framexmyp(f); framexpyp(f); endif;
	if y>=f[3]; framexmym(f); framexpym(f); endif;
	return 0
endfunction

function frame2 (f)
## draws the front part of the box (considering view).
	v=view();
	y=sin(v[4])*v[1];
	if y<=f[5]; framez0(f); endif;
	if y>=f[6]; framez1(f); endif;
	x=sin(v[3])*v[1]; y=-cos(v[3])*v[1];
	if x>=f[2]; framexpyp(f); framexpym(f); endif;
	if x<=f[1]; framexmyp(f); framexmym(f); endif;
	if y>=f[4]; framexmyp(f); framexpyp(f); endif;
	if y<=f[3]; framexmym(f); framexpym(f); endif;
	return 0
endfunction

function scaleframe (x,y,z,f,m)
	s=max([f[2]-f[1],f[4]-f[3],f[6]-f[5]]);
	xm=(f[2]+f[1])/2;
	ym=(f[4]+f[3])/2;
	zm=(f[6]+f[5])/2;
	ff=m/s*2;
	f=[f[1]-xm,f[2]-xm,f[3]-ym,f[4]-ym,f[5]-zm,f[6]-zm]*ff;
	return {(x-xm)*ff,(y-ym)*ff,(z-zm)*ff}
endfunction

function framedsolid (x,y,z,scale=0)
## works like solid and draws a frame around the plot
## if scale is specified, then the plot is scaled to fit into a cube of
## side length 2*scale centered at 0
	frame=getframe(x,y,z);
	if !holding(); clg; endif;
	h=holding(1);
	if scale; {x1,y1,z1}=scaleframe(x,y,z,frame,scale);
	else; {x1,y1,z1}={x,y,z}; endif;
	color(2); frame1(frame);
	color(1); solid(x1,y1,z1);
	color(2); frame2(frame);
	color(1);
	holding(h);
	return frame
endfunction

function framedsolidhue (x,y,z,hue,scale=0,f=1)
## works like solidhue and draws a frame around the plot
## if scale is specified, then the plot is scaled to fit into a cube of
## side length 2*scale centered at 0
	frame=getframe(x,y,z);
	if !holding(); clg; endif;
	h=holding(1);
	if scale; {x1,y1,z1}=scaleframe(x,y,z,frame,scale);
	else; {x1,y1,z1}={x,y,z}; endif;
	color(2); frame1(frame);
	color(1);
	if f; solidhue(x1,y1,z1,hue,f);
	else; solidhue(x1,y1,z1,hue);
	endif;
	color(2); frame2(frame);
	color(1);
	holding(h);
	return frame
endfunction

function framedwire (x,y,z,scale=0)
## works like framedsolid
	frame=getframe(x,y,z);
	if !holding(); clg; endif;
	h=holding(1);
	if scale; {x1,y1,z1}=scaleframe(x,y,z,frame,scale);
	else; {x1,y1,z1}={x,y,z}; endif;
	color(2); frame1(frame);
	color(1); wire(x1,y1,z1);
	color(2); frame2(frame);
	color(1);
	holding(h);
	return frame
endfunction


function framedmark3 (x,y,z,scale=0)
## Plots points in three dimensions with frame. 
## x,y,z must be 1xn vectors.
	frame=getframe(x,y,z);
	if !holding(); clg; endif;
	h=holding(1);
	if scale; {x1,y1,z1}=scaleframe(x,y,z,frame,scale);
	else {x1,y1,z1}={x,y,z}; endif
	color(2); frame1(frame);
	color(1);
	{c0,r0}=project(x1,y1,z1);
	{x0,y0}=fromscreen(c0,r0);
	mark(x0,y0);
	color(2); frame2(frame);
	color(1);
	holding(h);
	return frame;
endfunction
