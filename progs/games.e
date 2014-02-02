comment
Optimization examples (two person games).
gambling, scissors, poker.
game(A), computes an optimal strategy for the game with matrix A.
endcomment

function opti (A,b,z)
## Minimizes z.x' with contraints A.x'>=b and x'>=0.
## Returns x.
	{x,f}=simplex(-A,-b,z)';
	if f!=0; error("Simplex failed!"); endif;
	return x;
endfunction

function game (A)
## Maximizes G such that A'.q>=G, sum q_i = 1, q_i>=0.
## Returns {q,G}.
	m=max(max(A)');
	B=A'-m; si=size(B);
	M=(B|ones(si[1],1))_(ones(1,si[2])|0)_(-1*ones(1,si[2])|0);
	r=zeros(si[1],1)_1_-1;
	z=zeros(1,si[2])|1;
	{q,K}=opti(M,r,z);
	return {q[1:si[2]],m-q[si[2]+1]};
endfunction

function gambling (n=2)
## Computes an optimal strategy for the following game
## player A and B take 0 to n stones each.
## A claims a number and B another,
## The number, which is closer to the sum of the stones, wins.
## The win is the stones of the opponent plus 1.
	eps=setepsilon(1e-5);
	m=2*n+1;
	A=zeros((n+1)*m,(n+1)*(m-1));
	for i=0 to n;
		for ik=0 to m-1;
			for j=0 to n;
				for jk=1 to m-1;
					if i+j==ik;
						A[i*m+ik+1,j*(m-1)+jk]=j+1;
					else
						if (i+j>ik && jk<=ik) || (i+j<ik && jk>ik);
							A[i*m+ik+1,j*(m-1)+jk]=j+1;
						else
							A[i*m+ik+1,j*(m-1)+jk]=-i-1; 
						endif;
					endif;
				end;
			end;
		end;
	end;
	{q,G}=game(A);
	"Win for A : "|printf("%10.5f",G),
	for i=0 to n;
		for ik=0 to m-1;
			if q[i*m+ik+1]~=0;
			else
				printf("%7.5f : Take ",q[i*m+ik+1])| ..
				printf("%2.0f, say ",i)|printf("%2.0f",ik),
			endif;
		end;
	end;
	{p,G}=game(-A');
	"Win for B : "|printf("%10.5f",G),
	for j=0 to n;
		printf("Take : %2.0f",j),
		for jk=1 to m-1;
			if p[j*(m-1)+jk]~=0;
			else
				printf("%7.5f : say less at ",p[j*(m-1)+jk])|..
				printf("%2.0f or more.",jk),
			endif;
		end;
	end;
	setepsilon(eps);
	return "";
endfunction

function scissors
## Computes the optimal strategy for the following game.
## A and B have a secret choice from scissors, paper, well or stone.
## scissors wins against paper.
## paper wins against well and stone.
## well wins against scissors and stone.
## stone wins againts scissors.
	q=game([0,1,-1,-1;-1,0,1,1;1,-1,0,1;1,-1,-1,0]);
	"Choose with probability",
	"scissors  : ", q[1],
	"paper  : ", q[2],
	"well : ", q[3],
	"stone   : ", q[4],
	return q;
endfunction

function poker (w=5,e=5)
## Solves a primitive poker game.
## A and B get a card valued from 1 to w.
## A and B place 1.
## A places e or passes. B can pass or hold.
	A=zeros(w,w);
	for i=1 to w; ## Setze bei Karte i bis w
		for j=1 to w; ## Halte bei Karte j bis w
			if j<=i;
				A[i,j]=(w-i+1)*(j-1)+(e+1)*(w-i+1)*(i-j)-w*(i-1);
			else
				A[i,j]=(w-i+1)*(j-1)-(e+1)*(w-j+1)*(j-i)-w*(i-1);
			endif;
		end;
	end;
	A=A/(w*w);
	{p,G}=game(A);
	"Win for A : "|printf("%10.5f",G),
	"Strategy for A:"
	for i=1 to w;
		if !(p[i]~=0);
		printf("%7.5f : ",p[i])|printf("Place from %2.0f to ",i)|..
		printf("%2.0f",w),
		endif;
	end;
	{q,G}=game(-A');
	"Win for B : "|printf("%10.5f",G),
	"Strategy for B:"
	for i=1 to w;
		if !(q[i]~=0);
		printf("%7.5f : ",q[i])|printf("Hold from %2.0f to ",i)|..
		printf("%2.0f",w),
		endif;
	end;
	return {p,q};
endfunction

function simulhelp (w,a,k1,b,k2)
	return -(a<k1)+(a>=k1)*((b<k2)+(b>=k2)*(w+1)*((a>b)-(a<b)))
endfunction

function simul (k,w,pa,k1,k2,pb,k3,k4,n=1000)
## Simulates a poker game.
## A places with probabilty pa from k1,with (1-pa) from k2.
## B holds with probability pb from k3, with (1-pb) from k4.
	a=floor(random(1,n)*k)+1;
	b=floor(random(1,n)*k)+1;
	ra=random(1,n);
	rb=random(1,n);
	win = sum( ..
		(ra<pa) * (rb<pb) * simulhelp(w,a,k1,b,k3) + ..
		(ra>=pa) * (rb<pb) * simulhelp(w,a,k2,b,k3) + ..
		(ra<pa) * (rb>=pb) * simulhelp(w,a,k1,b,k4) + ..
		(ra>=pa) * (rb>=pb) * simulhelp(w,a,k2,b,k4)),
	return win
endfunction

