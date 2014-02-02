#include "easy.h"
#include "header.h"
#include "stack.h"

class Lib
{	HANDLE Handle;
	String Name;
	public :
	Lib (char *name) : Name(name)
	{	Handle=LoadLibrary(name);
	}
	~Lib ()
	{	FreeLibrary(Handle);
	}
	char *name () { return Name; }
	HANDLE handle () { return Handle; }
};

int nlibs=0;
const int maxlibs=32;
static Lib *libs[maxlibs];

typedef char * (*LibFunction) (header * params [], int nparam,
	char *newram, char *ramend);

class LibFunctions
{	LibFunctions *Next;
	LibFunction F;
	String Name;
	int NArgs;
	public :
	LibFunctions (char *name, LibFunction f, LibFunctions *next,
		int nargs) :
		Next(next),Name(name),F(f),NArgs(nargs)
	{}
	char *name () { return Name; }
	LibFunction f () { return F; }
	LibFunctions *next () { return Next; }
	LibFunctions *find (char *s, int n);
	int nargs () { return NArgs; }
};

LibFunctions * LibFunctions::find (char *s, int n)
{	LibFunctions *p=this;
	while (p)
	{	if (!strcmp(p->name(),s) && p->nargs()==n) return p;
		p=p->next();
	}
	return 0;
}

LibFunctions* libfunctions=0;

void mdll (header *hd)
{	header *st=hd,*hd1=nextof(hd),*hd2=nextof(hd1);
	hd=getvalue(hd); if (error) return;
	hd1=getvalue(hd1); if (error) return;
	hd2=getvalue(hd2); if (error) return;
	if (hd->type!=s_string || hd1->type!=s_string || hd2->type!=s_real)
		wrong_arg_in("dll");
	char *p=stringof(hd);
	int i,n=(int)*realof(hd2);
	if (n<0 || n>16)
	{	output("DLL functions must have 0 to 16 parameters.\n");
		error=1; return;
	}
	HANDLE h=0;
	for (i=0; i<nlibs; i++)
		if (strcmp(p,libs[i]->name())==0)
		{    h=libs[i]->handle(); break;
		}
	if (!h)
	{   if (nlibs==maxlibs)
		{	output("Too many libraries!\n");
			error=1; return;
		}
		libs[nlibs]=new Lib(p);
		if (!libs[nlibs]->handle())
		{	output("Could not open the DLL library!\n");
			error=1; return;
		}
		h=libs[nlibs]->handle();
		nlibs++;
	}
	LibFunction f=(LibFunction)GetProcAddress(h,stringof(hd1));
	if (!f)
	{	output("Could not find the function!\n");
		error=1; return;
	}
	if (!libfunctions->find(stringof(hd1),n))
		libfunctions=new LibFunctions(stringof(hd1),f,libfunctions,n);
	else hd1=new_string("",4,"");
	moveresult(st,hd1);
}

void closelibs ()
{	int i;
	for (i=0; i<nlibs; i++) delete libs[i];
}

int exec_dll (char *name, int n, header *hd)
{   header *st=hd;
	LibFunctions *l=libfunctions->find(name,n);
	if (!l) return 0;
	static header *h[16];
	int i;
	for (i=0; i<n; i++)
	{	h[i]=getvalue(hd); if (error) return 1;
		hd=nextof(hd);
	}
	char *ram=l->f()(h,n,newram,ramend);
	if (ram>newram)
	{   memmove((char *)st,newram,ram-newram);
    	newram=(char *)st+(ram-newram);
	}
	else
	{   output1("%s returned an error:\n",l->name());
		output(newram); output("\n");
		error=1;
	}
	return 1;
}

