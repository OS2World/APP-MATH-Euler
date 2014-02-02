#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "header.h"
#include "stack.h"
#include "extend.h"
#include "interval.h"

int sametype (header *hd1, header *hd2)
/***** sametype
	returns true, if hd1 and hd2 have the same type and dimensions.
*****/
{	dims *d1,*d2;
	if (hd1->type==s_string && hd2->type==s_string)
    	return hd1->size>=hd2->size;
	if (hd1->type!=hd2->type || hd1->size!=hd2->size) return 0;
	if (hd1->type==s_matrix || hd1->type==s_cmatrix || hd1->type==s_imatrix)
	{	d1=dimsof(hd1); d2=dimsof(hd2);
			if (d1->r!=d2->r) return 0;
	}
	return 1;
}

header *assign (header *var, header *value)
/***** assign
	assign the value to the variable.
*****/
{	char name[16],*nextvar;
	ULONG size;
	long dif;
	double *m,*mv,*m1,*m2;
	int i,j,c,r,cv,rv,*rind,*cind;
	dims *d;
	header *help,*orig;
	if (error) return 0;
	size=value->size;
	if (var->type==s_reference && !referenceof(var))
		/* seems to be a new variable */
	{	strcpy(name,var->name);
		if (value->type==s_udf)
		{	strcpy(value->name,name);
			value->xor=xor(name);
#ifndef SPLIT_MEM
			if (!freeram(size))
			{	output("Memory overflow.\n"); error=500; return value;
			}
			memmove(ramstart+size,ramstart,newram-ramstart);
			newram+=size; endlocal+=size; startlocal+=size;
			value=(header *)((char *)value+size);
#else
			if ((long)udfend+size>(long)udframend)
			{	output1("Not enough memory for function %s!\n",name);
				error=500; return value;
			}
			memmove(ramstart+size,ramstart,udfend-ramstart);
#endif
			udfend+=size;
			memmove(ramstart,(char *)value,size);
			return (header *)ramstart;
		}
		if (!freeram(size))
		{	output("Memory overflow.\n"); error=500; return value;
		}
		memmove(endlocal+size,endlocal,newram-endlocal);
		value=(header *)((char *)value+size);
		newram+=size;
		memmove(endlocal,(char *)value,size);
		strcpy(((header *)endlocal)->name,name);
		((header *)endlocal)->xor=xor(name);
		value=(header *)endlocal;
		endlocal+=size;
		return value;
	}
	else
	{	while (var && var->type==s_reference) var=referenceof(var);
		if (!var)
		{	error=43; output("Internal variable error!\n"); return 0;
		}
		if (var->type!=s_udf && value->type==s_udf)
		{	output("Cannot assign a UDF to a variable!\n"); error=320;
			return var;
		}
		if (var->type==s_submatrix)
		{	d=submdimsof(var);
			if (value->type==s_complex || value->type==s_cmatrix)
			{	orig=submrefof(var);
				help=new_reference(orig,"");
				if (error) return 0;
					mcomplex(help); if (error) return 0;
				var->type=s_csubmatrix;
				submrefof(var)=help;
				assign(var,value); if (error) return 0;
				submrefof(var)=orig;
				assign(orig,help);
				return orig;
			}
			else if (value->type==s_interval || value->type==s_imatrix)
			{	orig=submrefof(var);
				help=new_reference(orig,"");
				if (error) return 0;
				minterval1(help); if (error) return 0;
				var->type=s_isubmatrix;
				submrefof(var)=help;
				assign(var,value); if (error) return 0;
				submrefof(var)=orig;
				assign(orig,help);
				return orig;
			}
			else if (value->type!=s_real && value->type!=s_matrix)
			{	output("Cannot assign this type!\n"); error=45; return 0;
			}
			getmatrix(value,&rv,&cv,&mv);
			getmatrix(submrefof(var),&r,&c,&m);
			if (d->r!=rv || d->c!=cv)
			{   if (rv==1 && cv==1)
				{	rind=rowsof(var); cind=colsof(var);
					for (i=0; i<d->r; i++)
					{	m1=mat(m,c,rind[i],0);
						for (j=0; j<d->c; j++)
						{	m1[cind[j]]=*mv;
						}
					}
					return submrefof(var);
				}
				output("Illegal assignment!\n"
					"Row or column numbers do not agree!\n");
				error=45; return 0;
			}
			rind=rowsof(var); cind=colsof(var);
			for (i=0; i<d->r; i++)
			{	m1=mat(m,c,rind[i],0);
				m2=mat(mv,cv,i,0);
				for (j=0; j<d->c; j++)
				{	m1[cind[j]]=*m2++;
				}
			}
			return submrefof(var);
		}
		else if (var->type==s_csubmatrix)
		{	d=submdimsof(var);
			if (value->type==s_real || value->type==s_matrix)
			{	help=new_reference(value,""); if (error) return 0;
				mcomplex(help); if (error) return 0;
				assign(var,help);
				return submrefof(var);
			}
			if (value->type!=s_complex && value->type!=s_cmatrix)
			{	output("Illegal assignment!\n"); error=45; return 0;
			}
			getmatrix(value,&rv,&cv,&mv);
			getmatrix(submrefof(var),&r,&c,&m);
			if (d->r!=rv || d->c!=cv)
			{   if (rv==1 && cv==1)
				{	rind=rowsof(var); cind=colsof(var);
					for (i=0; i<d->r; i++)
					{	m1=cmat(m,c,rind[i],0);
						for (j=0; j<d->c; j++)
						{	copy_complex(m1+(LONG)2*cind[j],mv);
						}
					}
					return submrefof(var);
				}
				output("Illegal assignment!\n"
					"Row or column numbers do not agree!\n");
				error=45; return 0;
			}
			rind=rowsof(var); cind=colsof(var);
			for (i=0; i<d->r; i++)
			{	m1=cmat(m,c,rind[i],0);
				m2=cmat(mv,cv,i,0);
				for (j=0; j<d->c; j++)
				{   copy_complex(m1+(LONG)2*cind[j],m2); m2+=2;
				}
			}
			return submrefof(var);
		}
		else if (var->type==s_isubmatrix)
		{   d=submdimsof(var);
			if (value->type==s_real || value->type==s_matrix)
			{	help=new_reference(value,""); if (error) return 0;
				minterval1(help); if (error) return 0;
				assign(var,help);
				return submrefof(var);
			}
			if (value->type!=s_interval && value->type!=s_imatrix)
			{	output("Cannot assign this type to intervals!\n");
				error=45; return 0;
			}
			getmatrix(value,&rv,&cv,&mv);
			getmatrix(submrefof(var),&r,&c,&m);
			if (d->r!=rv || d->c!=cv)
			{   if (rv==1 && cv==1)
				{	rind=rowsof(var); cind=colsof(var);
					for (i=0; i<d->r; i++)
					{	m1=imat(m,c,rind[i],0);
						for (j=0; j<d->c; j++)
						{	copy_interval(m1+(LONG)2*cind[j],mv);
						}
					}
					return submrefof(var);
				}
				output("Illegal assignment!\n"
					"Row or column numbers do not agree!\n");
				error=45; return 0;
			}
			rind=rowsof(var); cind=colsof(var);
			for (i=0; i<d->r; i++)
			{	m1=imat(m,c,rind[i],0);
				m2=imat(mv,cv,i,0);
				for (j=0; j<d->c; j++)
				{   copy_interval(m1+(LONG)2*cind[j],m2); m2+=2;
				}
			}
			return submrefof(var);
		}
		else
		{	if ((char *)var<startlocal || (char *)var>endlocal)
			/* its not a local variable! */
			{	if (!sametype(var,value))
				{	output1("Cannot change type of non-local variable %s!\n",
						var->name);
					error=12; return 0;
				}
				memcpy((char *)(var+1),(char *)(value+1),
					value->size-sizeof(header));
				return var;
			}
			dif=(long)value->size-var->size;
			if (dif>0 && !freeram(dif))
			{	output("Memory overflow in assignment.\n");
				error=501; return value;
			}
			nextvar=(char *)var+var->size;
			if (dif!=0)
				memmove(nextvar+dif,nextvar,newram-nextvar);
			newram+=dif; endlocal+=dif;
			value=(header *)((char *)value+dif);
			strcpy(value->name,var->name);
			value->xor=var->xor;
			memmove((char *)var,(char *)value,value->size);
		}
	}
	return var;
}

