#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "header.h"
#include "sysdep.h"
#include "stack.h"
#include "help.h"

header *running;

char *type_udfline (char *start)
{	char outline[1024],*p=start,*q;
	double x;
	int comn;
	q=outline;
	while (*p)
	{	if (*p==2)
		{	p++; memmove((char *)(&x),p,sizeof(double));
			p+=sizeof(double);
			sprintf(q,"%g",x);
			q+=strlen(q);
		}
		else if (*p==3)
		{	p++;
			memmove((char *)(&comn),p,sizeof(int));
			p+=sizeof(int);
			sprintf(q,"%s",command_list[comn].name);
			q+=strlen(q);
		}
		else *q++=*p++;
		if (q>outline+1022)
		{	q=outline+1023;
			break;
		}
	}
	*q=0;
	output(outline); output("\n");
	return p+1;
}

void minput (header *);

void trace_udfline (char *next)
{	int scan,oldtrace;
	extern header *running;
	header *hd,*res;
	output1("%s: ",running->name); type_udfline(next);
	again: wait_key(&scan);
	switch (scan)
	{	case fk1 :
		case cursor_down :
			break;
		case fk2 :
		case cursor_up :
			trace=2; break;
		case fk3 :
		case cursor_right :
			trace=0; break;
		case fk4 :
		case help :
			hd=(header *)newram;
			oldtrace=trace; trace=0;
			new_string("Expression",12,""); if (error) goto cont;
			minput(hd); if (error) goto cont;
			res=getvalue(hd); if (error) goto cont;
			give_out(res);
			cont : newram=(char *)hd;
			trace=oldtrace;
			goto again;
		case fk9 :
		case escape :
			output("Trace interrupted\n"); error=11010; break;
		case fk10 :
		case cursor_left :
			trace=-1; break;
		default :
			output(
				"\nKeys :\n"
				"cursor_down   Single step\n"
				"cursor_up     Step over subroutines\n"
				"cursor_right  Go until return\n"
				"insert        Evaluate expression\n"
				"escape        Abort execution\n"
				"cursor_left   End trace\n\n");
			goto again;
	}
}

header *searchudf (char *name)
/***** searchudf
	search a udf, named "name".
	return 0, if not found.
*****/
{	header *hd;
	int r;
	r=xor(name);
	hd=(header *)ramstart;
	while ((char *)hd<udfend && hd->type==s_udf)
	{	if (r==hd->xor && !strcmp(hd->name,name)) return hd;
		hd=nextof(hd);
	}
	return 0;
}

commandtyp *preview_command (ULONG *l);

void get_udf (void)
/***** get_udf
	define a user defined function.
*****/
{	char name[16],argu[16],*p,*firstchar,*startp;
	int *ph,*phh,count=0,n;
	ULONG l;
	header *var,*result,*hd;
	FILE *actfile=infile;
	commandtyp *com;
	int comn;
	double x;
	if (udfon==1)
	{	output("Cannot define a function in a function!\n");
		error=60; return;
	}
	scan_space(); scan_name(name); if (error) return;
	kill_udf(name);
	var=new_reference(0,name); if (error) return;
	result=new_udf(""); if (error) return;
	p=udfof(result); udf=1; /* udf is for the prompt! */
	scan_space();
	ph=(int *)p; p+=sizeof(inttyp);
	if (*next=='(')
	{	while(1)
		{	next++;
			scan_space();
			if (*next==')') break;
			phh=(int *)p; *phh=0; p+=sizeof(inttyp);
			scan_name(argu); if (error) goto aborted;
			count++;
			strcpy(p,argu); p+=16;
			*((int *)p)=xor(argu); p+=sizeof(inttyp);
			test: scan_space();
			if (*next==')') break;
			else if (*next=='=')
			{	next++;
				*phh=1;
				newram=p;
				hd=(header *)p;
				scan_value(); if (error) goto aborted;
				strcpy(hd->name,argu);
				hd->xor=xor(argu);
				p=newram;
				goto test;
			}
			else if (*next==',') continue;
			else
			{	output("Syntax error in parameter list!\n"); error=701;
				goto aborted;
			}
		}
		next++;
	}
	*ph=count;
	if (*next==0) { next_line(); }
	while (1) /* help section of the udf */
	{	if (*next=='#' && *(next+1)=='#')
		{	while (*next)
			{	*p++=*next++;
				if (!freeramfrom(p,16))
				{	output("Memory overflow while defining a function!\n");
					error=210; goto stop;
				}
			}
			*p++=0; next_line();
		}
		else break;
		if (actfile!=infile)
		{	output("End of file reached in function definition!\n");
			error=2200; goto stop;
		}
	}
	*udfstartof(result)=(p-(char *)result);
	startp=p;
	firstchar=next;
	while (1)
	{	if (error) goto stop;
		if (!strncmp(next,"endfunction",strlen("endfunction")))
		{	if (p==startp || *(p-1)) *p++=0;
			*p++=1; next+=strlen("endfunction"); break;
		}
		if (actfile!=infile)
		{	output("End of file reached in function definition!\n");
			error=2200; goto stop;
		}
		if (*next=='#' && *(next+1)=='#')
		{	*p++=0; next_line(); firstchar=next;
		}
		else
		if (*next)
		{	if (*next=='"')
			{	*p++=*next++;
				while (*next!='"' && *next) *p++=*next++;
				if (*next=='"') *p++=*next++;
				else { output("\" missing.\n"); error=2200; goto stop; }
			}
			else if (*next=='\'' && *(next+1)=='\'')
			{	*p++=*next++; *p++=*next++;
				while (*next && (*next!='\'' || *(next+1)!='\''))
					*p++=*next++;				
				if (*next=='\'') { *p++=*next++; *p++=*next++; }
				else { output("\'\' missing.\n"); error=2200; goto stop; }
			}
			else if (isdigit(*next) ||				     	
				(*next=='.' && isdigit(*(next+1))) )
			{	if (next!=firstchar && isalpha(*(next-1)))
				{	*p++=*next++;
					while (isdigit(*next)) *p++=*next++;
				}
				else
				{
					if ((p-(char *)result)%2==0) *p++=' ';
					*p++=2;
					sscanf(next,"%lg%n",&x,&n);
					next+=n;
					memmove(p,(char *)(&x),sizeof(double));
					p+=sizeof(double);
				}
			}
			else if (isalpha(*next) &&
				(next==firstchar || !isalpha(*(next-1))) &&
				(com=preview_command(&l))!=0)
			/* Try to find a builtin command */
			{
				if ((p-(char *)result)%2==0) *p++=' ';
				*p++=3;
				comn=com-command_list;
				memmove(p,(char *)(&comn),sizeof(int));
				p+=sizeof(int);
				next+=l;
			}
			else if (*next=='.' && *(next+1)=='.')
			{	*p++=' '; next_line(); firstchar=next;
			}
			else *p++=*next++;
		}
		else { *p++=0; next_line(); firstchar=next; }
		if (!freeramfrom(p,80))
		{	output("Memory overflow while defining a function!\n");
			error=210; goto stop;
		}
	}
	stop:
	udf=0; if (error) return;
	result->size=(((p-(char *)result)-1)/ALIGNMENT+1)*ALIGNMENT;
	newram=(char *)result+result->size;
	assign(var,result);
	aborted:
	udf=0;
}

builtintyp *find_builtin (char *name);

void do_type (void)
{	char name[16];
	header *hd;
	char *p,*pnote;
	int i,count,defaults;
	builtintyp *b;
	scan_space();
	scan_name(name); hd=searchudf(name);
	b=find_builtin(name);
	if (b)
	{   if (b->nargs>=0)
			output1(
				"%s is a builtin function with %d argument(s).\n"
				,name,b->nargs);
		else
			output1(
				"%s is a builtin function.\n"
				,name);
	}
	if (hd && hd->type==s_udf)
	{   if (b) output1("%s is also a user defined function.\n",name);
		output1("function %s (",name);
		p=helpof(hd);
		memmove(&count,p,sizeof(inttyp));
		p+=sizeof(inttyp);
		pnote=p;
		for (i=0; i<count; i++)
		{	memmove(&defaults,p,sizeof(inttyp)); p+=sizeof(inttyp);
			output1("%s",p);
			p+=16+sizeof(inttyp);
			if (defaults)
			{	output("=...");
				p=(char *)(nextof((header *)p));
			}
			if (i!=count-1) output(",");
		}
		output(")\n");
		p=pnote;
		for (i=0; i<count; i++)
		{	memmove(&defaults,p,sizeof(inttyp)); p+=sizeof(inttyp);
			if (defaults) output1("## Default for %s :\n",p);
			p+=16+sizeof(inttyp);
			if (defaults)
			{	give_out((header *)p);
				p=(char *)nextof((header *)p);
			}
		}
		p=udfof(hd);
		while (*p!=1 && p<(char *)nextof(hd))
			p=type_udfline(p);
		output("endfunction\n");
	}
	else
	{	output("No such function!\n"); error=173;
	}
}

int printudf (char *name, char *buffer, long maxbuf)
{	header *hd;
	char *p,*pnote,*end;
	int i,count,defaults;
	outputbuffer=buffer; outputbufferend=buffer+maxbuf;
	outputbuffererror=0;
	hd=searchudf(name);
	if (hd && hd->type==s_udf)
	{   output1("function %s (",name);
		p=helpof(hd);
		memmove(&count,p,sizeof(inttyp));
		p+=sizeof(inttyp);
		pnote=p;
		for (i=0; i<count; i++)
		{	memmove(&defaults,p,sizeof(inttyp)); p+=sizeof(inttyp);
			output1("%s",p);
			p+=16+sizeof(inttyp);
			if (defaults)
			{	output("=...");
				p=(char *)(nextof((header *)p));
			}
			if (i!=count-1) output(",");
		}
		output(")\n");
		p=pnote;
		for (i=0; i<count; i++)
		{	memmove(&defaults,p,sizeof(inttyp)); p+=sizeof(inttyp);
			if (defaults) output1("## Default for %s :\n",p);
			p+=16+sizeof(inttyp);
			if (defaults)
			{	give_out((header *)p);
				p=(char *)nextof((header *)p);
			}
		}
		end=udfof(hd);
		while (*p!=1 && p<end)
		{	output(p); output("\n");
			p+=strlen(p); p++;
		}
		p=udfof(hd);
		while (*p!=1 && p<(char *)nextof(hd))
			p=type_udfline(p);
		output("endfunction\n");
	}
	else return 0;
	*outputbuffer=0; outputbuffer=0;
	if (outputbuffererror) return 0;
	else return 1;
}

void do_help (void)
{	char name[256];
	header *hd;
	int count,i,defaults;
	char *p,*end,*pnote;
	builtintyp *b;
	scan_space();
	p=name;
	while (*next!=0 && *next!=' ')
	{	*p++=*next++;
    	if (p-name>254) break;
	}
	*p=0;
	if (!*name) strcpy(name,"help");
	b=find_builtin(name);
	if (b)
	{   output1(
				"%s is a builtin function.\n"
				,name);
	}
    externhelp(name);
	hd=searchudf(name);
	if (hd && hd->type==s_udf)
	{   if (b) output1("%s is also a user defined function.\n\n",name);
		output1("function %s (",name);
		end=udfof(hd);
		p=helpof(hd);
		memmove(&count,p,sizeof(inttyp));
		p+=sizeof(inttyp);
		pnote=p;
		for (i=0; i<count; i++)
		{	memmove(&defaults,p,sizeof(inttyp)); p+=sizeof(inttyp);
			output1("%s",p);
			p+=16+sizeof(inttyp);
			if (defaults)
			{	output("=...");
				p=(char *)nextof((header *)p);
			}
			if (i!=count-1) output(",");
		}
		output(")\n");
		p=pnote;
		for (i=0; i<count; i++)
		{	memmove(&defaults,p,sizeof(inttyp)); p+=sizeof(inttyp);
			if (defaults) output1("## Default for %s :\n",p);
			p+=16+sizeof(inttyp);
			if (defaults)
			{	give_out((header *)p);
				p=(char *)nextof((header *)p);
			}
		}
		while (*p!=1 && p<end)
		{	output(p); output("\n");
			p+=strlen(p); p++;
		}
	}
}

/****************** udf ************************/

int udfcount=0;

#ifndef MAXUDF
#define MAXUDF 400
#endif

void interpret_udf (header *var, header *args, int argn, int sp)
/**** interpret_udf
	interpret a user defined function.
****/
{	int udfold,nargu,i,oldargn,defaults,oldtrace,oldindex,
		oldsearchglobal,oldsp,n;
	char *oldnext=next,*oldstartlocal,*oldendlocal,*udflineold,
		*p,*name;
	header *result,*st=args,*hd=args,*hd1,*oldrunning;
	double oldepsilon,oldchanged;
	p=helpof(var);
	nargu=*((int *)p); p+=sizeof(inttyp);
	if (sp!=0) n=sp;
	else n=argn;
	for (i=0; i<n; i++)
	{	if (hd->type==s_reference && !referenceof(hd))
		{	if (i<nargu && hd->name[0]==0 && *(int *)p)
			{	p+=16+2*sizeof(inttyp);
				moveresult((header *)newram,(header *)p);
				p=(char *)nextof((header *)p);
				hd=nextof(hd);
				continue;
			}
			else
			{	hd1=getvalue(hd); if (error) return;
			}
		}
		else hd1=hd;
		if (i<nargu)
		{	defaults=*(int *)p; p+=sizeof(inttyp);
			strcpy(hd1->name,p); hd1->xor=*((int *)(p+16));
			p+=16+sizeof(inttyp);
			if (defaults) p=(char *)nextof((header *)p);
		}
		else
		{	strcpy(hd1->name,argname[i]);
			hd1->xor=xors[i];
		}
		hd=nextof(hd);
	}
	for (i=n; i<nargu; i++)
	{	defaults=*(int *)p;
		name=p+sizeof(inttyp);
		p+=16+2*sizeof(inttyp);
		if (defaults)
		{	moveresult((header *)newram,(header *)p);
			p=(char *)nextof((header *)p);
		}
		else
		{	output1("Argument %s undefined.\n",name);
			error=1; return;
		}
	}
	for (i=n; i<argn; i++)
	{   strcpy(hd->name,""); hd->xor=0;
		hd=nextof(hd);
	}
	if (sp==0 && argn>nargu) sp=nargu;
	udflineold=udfline;
	oldargn=actargn; oldsp=actsp;
	actargn=argn; actsp=sp;
	udfline=next=udfof(var); udfold=udfon; udfon=1;
	oldstartlocal=startlocal; oldendlocal=endlocal;
	startlocal=(char *)args; endlocal=newram;
	oldrunning=running; running=var;
	oldindex=loopindex;
	oldsearchglobal=searchglobal; searchglobal=0;
	oldepsilon=epsilon; oldchanged=epsilon=changedepsilon;
	if ((oldtrace=trace)>0)
	{	if (trace==2) trace=0;
		if (trace>0) trace_udfline(next);
	}
	else if (var->flags&1)
	{	trace=1;
		if (trace>0) trace_udfline(next);
	}
	udfcount++;
	if (udfcount>MAXUDF)
	{	output("To many recursions!\n");
		error=1;
	}
	while (!error && udfon==1)
	{	command();
		if (udfon==2)
		{	result=scan_value();
			if (error)
			{	output1("Error in function %s\n",var->name);
				print_error(udfline);
				break;
			}
			moveresult1(st,result);
			break;
		}
		if (test_key()==escape)
		{	output("User interrupted!\n"); error=58; break;
		}
	}
	udfcount--;
	endlocal=oldendlocal; startlocal=oldstartlocal;
	running=oldrunning;
	loopindex=oldindex;
	if (oldchanged==changedepsilon) epsilon=oldepsilon;
	else epsilon=changedepsilon;
	if (trace>=0) trace=oldtrace;
	if (error) output1("Error in function %s\n",var->name);
	if (udfon==0)
	{	output1("Return missing in %s!\n",var->name); error=55; }
	udfon=udfold; next=oldnext; udfline=udflineold;
	actargn=oldargn; actsp=oldsp;
	searchglobal=oldsearchglobal;
}


