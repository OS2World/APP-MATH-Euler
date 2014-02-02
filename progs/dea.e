comment
Data Envelopment Approach
solveDEA Computes the efficiency for one site.
solveAllDEA Computes all efficiencies.
showDEAOutput, showDEAInput, graphDEA
for two inputs and outputs
showDEA print DEA results.
endcomment

function solveDEA (Output,Input,i,scale=0)
## Output, Input have a row for each site.
## i is the site number to compute.
## Returns the efficiency and the complete weights {e,x}.
	n=rows(Output); no=cols(Output); ni=cols(Input);
	A=-Output'|zeros(no,1);
	A=A_(Input'|-Input[i]');
	b=(-Output[i]')_zeros(ni,1);
	if scale;
		A=A_(ones(1,n)|0);
		A=A_(-ones(1,n)|0);
		b=b_1_(-1);
	endif;
	{x,r}=simplex(A,b,zeros(1,n)|1);
	if r==0; return {x[n+1],x[1:n]}; endif;
	error "Simplex failed!";
endfunction

function solveAllDEA (Output,Input,scale=0)
## Output, Input have a row for each site.
## Returns the efficiencies for each site.
	n=rows(Output); x=zeros(1,n);
	loop 1 to n;
		x[#]=solveDEA(Output,Input,#,scale);
	end;
	return x;
endfunction

function showDEAOutput (Output,Eff)
## Graphically prints both Outputs with attached efficiencies.
	max=totalmax(Output)*1.2;
	o=Output';
	setplot(0,max,0,max); 
	mark(o[1],o[2]);
	loop 1 to rows(Output);
		label(printf("%g",#)| .. 
			printf(",%g",Eff[#]),Output[#,1],Output[#,2],20);
	end
	return ""
endfunction

function showDEAInput (Output)
	max=totalmax(Output)*1.2;
	o=Output';
	setplot(0,max,0,max); 
	mark(o[1],o[2]);
	loop 1 to rows(Output);
		label(printf("%g",#),Output[#,1],Output[#,2],-30);
	end
return ""
endfunction

function graphDEA (Output,Input,scale=0)
## Solve and show both inputs and outputs and the efficencies.
	{eff,w}=solveAllDEA(Output,Input,scale);
	style ("m[]"); showDEAOutput(Output,eff);
	hold on; style ("mx"); showDEAInput(Input); hold off;
	wait(180);
	return "Done";
endfunction

function showDEA (Output,Input,scale=0)
## Show both outputs and inputs, and the necessary replacements.
	n=rows(Output);
	loop 1 to n
		showDEAone(Output,Input,#,scale);
	end;
	return "done"
endfunction

function showDEAone (Output,Input,n,scale=0)
	{e,w}=solveDEA(Output,Input,n,scale);
	if e~=1; return e; endif;
	"Site: "|printf("%g",n),
	"Efficiency: "|printf("%g",e),
	index=nonzeros(w);
	"Replace with:"
	loop 1 to length(index)
		printf("%g%",w[index[#]]*100)|" of "|printf("%g",index[#])
	end
	"",
	return e
endfunction

