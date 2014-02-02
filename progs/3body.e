comment
Three body problem (demonstration).
>test
will run a test program
>setplanets
lets you define places and initial vectors
endcomment

function norm (v)
	return sqrt(sum(v*v))
endfunction

function f3body (x,y)
	y1=y[1,1:2]; y2=y[1,3:4]; y3=y[1,5:6];
	d21=(y2-y1)/norm(y2-y1)^3;
	d31=(y3-y1)/norm(y3-y1)^3;
	d32=(y3-y2)/norm(y3-y2)^3;
	return y[7:12]|(d21+d31)|(-d21+d32)|(-d31-d32);
endfunction

function test3body (y1)
	y=y1;
	x=0; h=0.01;
	setplot(-3,3,-3,3); clg; hold off; xplot(); frame();
	title("Any key stops the movement");
	repeat;
		ynew=runge2("f3body",x,x+h,y,0.0001,h);
		H=(y_ynew)';
		hold on;
		color(4); plot(H[1],H[2]);
		color(5); plot(H[3],H[4]);
		color(6); plot(H[5],H[6]);
		hold off;
		y=ynew; x=x+h;
		if (any(abs(y[1:6])>4));
			"One body left definition area!",
			break;
		endif;
		if testkey; break; endif;
	end;
	return x
endfunction

function test
	return test3body([-2,0,2,0,0,2,0.1,-0.2,0,0.2,0.1,0]);
endfunction

function setplanets
	"Set the places and vectors for all three bodies",
	"Press return now",
	wait(20);
	y=zeros(1,12);
	setplot(-3,3,-3,3); hold off; clg; xplot(); frame();
	loop 1 to 3;
		m=mouse; style("m[]");
		hold on; mark(m[1],m[2]); hold off;
		y[2*#-1:2*#]=m;
		m1=mouse;
		style("->"); color(3+#);
		hold on; plot([m[1],m1[1]],[m[2],m1[2]]); hold off;
		color(1); style("");
		y[6+2*#-1:6+2*#]=(m1-m)/10;
	end;
	return test3body(y);
endfunction


