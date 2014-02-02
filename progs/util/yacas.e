comment
YACAS wrapper loaded
endcomment

function yeval (strng)
## Example: yeval("D(x) x^2",x=5)
## Evaluate the expression with the additional parameters.
## The expression will go through the yacas interpreter.
	return evaluate(yacas(strng));
endfunction

function ysimpl (strng)
## Simplify the expression in yacas.
	return yacas("Simplify("|strng|")");
endfunction

..==============================================================

function converty (x,format="%g%")
	if typeof(x)==0;
		return printf(format,x);
	elseif typeof(x)==1;
		return "Complex("|printf(format,re(x))|","| ..
			printf(format,im(x))|")";
	elseif typeof(x)==2 || typeof(x)==3;
		s="{";
		loop 1 to cols(x)-1;
			s=s|converty(x[#],format)|",";
		end;
		s=s|converty(x[cols(x)],format)|"}";
		return s;
	endif
	error("Conversion not possible!");
endfunction

function y2vector (strng)
## Turn a Yacas list into an Euler vector
	yacas("Set(VVV,"|strng|")");
	n=yeval("Length(VVV)");
	y=zeros(1,n);
	loop 1 to n;
		y[#]=evaluate(yacas("VVV["|printf("%0.0f",#)|"]"));
	end;
	return y;
endfunction

function y2matrix (strng)
## Turn a Yacas list into an Euler vector
	yacas("Set(AAA,"|strng|")");
	n=yeval("Length(AAA)");
	y=zeros(0,n);
	loop 1 to n;
		y=y_y2vector(yacas("AAA["|printf("%0.0f",#)|"]"));
	end;
	return y;
endfunction

function yset (var,x)
	return yacas("Set("|var|","|converty(x)|")");
endfunction

..=================================================================

function YacasScript (fl)
##
##  function to execute yacas script from file fl
##  note at present the path seperator \ has to be
##  entered as / - will need to sort this out
##
##
  quote=''"'';
  fl=quote|fl|quote; .. add quotes to file string
  xx="Load("|fl|")"; .. construct the command string
  rv=yacas(xx);       .. get yacas to execute the command string
  return rv;
endfunction

..========================================================================

function Sin(x)
return _sin(x)
endfunction

function Cos(x)
return _cos(x)
endfunction

function Tan(x)
return _tan(x)
endfunction

function Exp(x)
return _exp(x)
endfunction

function Ln(x)
return _log(x)
endfunction

function Pi
return pi
endfunction

function ArcSin(x)
return _asin(x)
endfunction

function ArcCos(x)
return _acos(x)
endfunction

function ArcTan(x)
return _atan(x)
endfunction

function Abs(x)
return _abs(x)
endfunction

function Sqrt(x)
return _sqrt(x)
endfunction

function Sign(x)
return _sign(x)
endfunction

function Complex(x,y)
return x+1i*y;
endfunction

..===========================

function ynewton (expr,start)
## Seek the zero of an expression in x, using Newtons
## method.
## Example: ynewton("x^x-2",1)
	df=yacas("D(x) "|expr);
	oldx=start;
	x=start;
	repeat
		x=x-evaluate(expr)/evaluate(df);
		if x~=oldx; return x; endif;
		oldx=x;
	end
endfunction

function ynewton2h (f1,f2,f1x,f1y,f2x,f2y,start)
	repeat
		x=start[1]; y=start[2];
		D=[evaluate(f1x),evaluate(f1y);evaluate(f2x),evaluate(f2y)];
		d=(D\[evaluate(f1);evaluate(f2)])';
		startn=start-d;
		if startn~=start; return startn; endif;
		start=startn;
	end;
endfunction

function ynewton2 (f1,f2,start)
## Try to iterate Newton's method to a solution of 
## f1=0, f2=0. Expression f1, f2 are passed to Yacas.
## Example: newto2y("Cos(x*y)-2*x","Sin(x+y)-3*y,[1,1])
	f1x=yacas("D(x) "|f1);
	f1y=yacas("D(y) "|f1);
	f2x=yacas("D(x) "|f2);
	f2y=yacas("D(y) "|f2);
	return ynewton2h(f1,f2,f1x,f1y,f2x,f2y,start);
endfunction
