comment
abbreviations
endcomment

function ctext
## ctext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Also ctext(s,c,r).
	if argn()==3; return _ctext(arg1,[arg2,arg3]);
	else return _ctext(arg1,arg2);
	endif;
endfunction

function text
## text(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Also ctext(s,c,r).
	if argn()==3; return _text(arg1,[arg2,arg3]);
	else return _text(arg1,arg2);
	endif;
endfunction

function vctext
## vtext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn downwards, centered.
	if argn()==3; return _vctext(arg1,[arg2,arg3]);
	else return _vctext(arg1,arg2);
	endif;
endfunction

function vrtext
## vtext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn downwards, right justified.
	if argn()==3; return _vrtext(arg1,[arg2,arg3]);
	else return _vrtext(arg1,arg2);
	endif;
endfunction

function vtext
## vtext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn downwards, left justified.
## Also vctext(s,c,r).
	if argn()==3; return _vtext(arg1,[arg2,arg3]);
	else return _vtext(arg1,arg2);
	endif;
endfunction

function vutext
## vutext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn upwards.
## Also vcutext(s,c,r).
	if argn()==3; return _vutext(arg1,[arg2,arg3]);
	else return _vutext(arg1,arg2);
	endif;
endfunction

function vcutext
## vutext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn upwards, centered.
## Also vcutext(s,c,r).
	if argn()==3; return _vcutext(arg1,[arg2,arg3]);
	else return _vcutext(arg1,arg2);
	endif;
endfunction

function vrutext
## vutext(s,[c,r]) plots the centered string s at the coordinates (c,r).
## Text will be drawn upwards, right justified.
## Also vcutext(s,c,r).
	if argn()==3; return _vrutext(arg1,[arg2,arg3]);
	else return _vrutext(arg1,arg2);
	endif;
endfunction

function diag
## diag([n,m],k,v) returns a nxm matrix A, containing v on its k-th
## diagonal. v may be a vector or a real number. Also diag(n,m,k,v);
## diag(A,k) returns the k-th diagonal of A.
	if argn()==4; return _diag([arg1,arg2],arg3,arg4);
	elseif argn()==3; return _diag(arg1,arg2,arg3);
	else return _diag(arg1,arg2);
	endif;
endfunction

function format
## format(n,m) sets the number output format to m digits and a total
## width of n.
	if argn()==2; return _format([arg1,arg2]);
	else return _format(arg1);
	endif;
endfunction

function goodformat
## goodformat(n,m) sets the number output format to m digits and a
## total width of n. Prints less than m digits, when not necessary.
	if argn()==2; return _goodformat([arg1,arg2]);
	else return _goodformat(arg1);
	endif;
endfunction

function expformat
## expformat(n,m) sets the number output format to m digits and a 
## total width of n. Always uses exponential format
	if argn()==2; return _expformat([arg1,arg2]);
	else return _expformat(arg1);
	endif;
endfunction

function fixedformat
## expformat(n,m) sets the number output format to m digits and a 
## total width of n. Always uses fixed point format
	if argn()==2; return _fixedformat([arg1,arg2]);
	else return _fixedformat(arg1);
	endif;
endfunction

function redim
## redim(A,[n,m]) returns a matrix with the numbers in A but different
## dimension filling with 0 if necessary. Also redim(A,n,m).
	if argn()==3; return _redim(arg1,[arg2,arg3]);
	else return _redim(arg1,arg2);
	endif;
endfunction

function normal
## normal([n,m]) returns a nxm matrix of unit normal distributed random
## values. Also normal(n,m).
	if argn()==2; return _normal([arg1,arg2]);
	else return _normal(arg1);
	endif;
endfunction

function random
## random([n,m]) returns a nxm matrix of uniformly distributed random 
## values in [0,1]. Also random(n,m).
	if argn()==2; return _random([arg1,arg2]);
	else return _random(arg1);
	endif;
endfunction

function ones
## ones([n,m]) returns a nxm matrix with all elements set to 1.
## Also ones(n,m).
	if argn()==2; return _ones([arg1,arg2]);
	else return _ones(arg1);
	endif;
endfunction

function zeros
## zeros([n,m]) returns a nxm matrix with all elements set to 0.
## Also zeros(n,m).
	if argn()==2; return _zeros([arg1,arg2]);
	else return _zeros(arg1);
	endif;
endfunction

function matrix
## matrix([n,m],x) returns a nxm matrix with all elements set to x.
## Also matrix(n,m,x).
	if argn()==3; return _matrix([arg1,arg2],arg3);
	else return _matrix(arg1,arg2);
	endif;
endfunction

