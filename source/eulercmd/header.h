#ifndef HEADER
#define HEADER

extern char *ramstart,*ramend,*startlocal,*endlocal,
	*newram,*udfend,*varstart,*udframend;

#define freeram(n) (((unsigned long)(newram)+(unsigned long)(n))<=(unsigned long)ramend)
#define freeramfrom(r,n) (((unsigned long)(r)+(unsigned long)(n))<=(unsigned long)ramend)

#if defined __MSDOS__
#define SPLIT_MEM
#define FLOAT_TEST
#endif

#define LONG long
#define ULONG unsigned long
#define TAB 9

#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

#ifndef ALIGNMENT
#define ALIGNMENT 8
#endif

#ifdef unix
#define PATH_DELIM_CHAR '/'
#define PATH_DELIM_STR "/"
#else
#define PATH_DELIM_CHAR '\\'
#define PATH_DELIM_STR "\\"
#endif

extern char fktext[10][64];

extern int error,surpressed,udf,udfon,linelength,stringon;
extern long loopindex;
extern char *next,*udfline;
extern int actargn,actsp;
extern int searchglobal;

extern int linew,fieldw,hchar,wchar;
extern double maxexpo,minexpo;
extern char expoformat[],fixedformat[];

extern FILE *infile,*outfile;
void output(char *s);
void output1(char *form, ...);
void output1hold(int s, char *form, ...);
#define output2(form,s,t) output1(form,s,t)

extern char input_line[1024];

extern char *outputbuffer,*outputbufferend;
extern int outputbuffererror;
extern int trace,outputing,errorout;

typedef enum { s_real,s_complex,s_matrix,s_cmatrix,
	s_reference,s_command,s_submatrix,s_csubmatrix,s_string,s_udf,
	s_interval,s_imatrix,s_isubmatrix }
	stacktyp;

typedef struct
{	LONG size; char name[16]; int xor; stacktyp type; int flags;
	}
	headertest;

typedef struct
{	LONG size; char name[16]; int xor; stacktyp type; int flags;
#ifdef HEADER_ALIGNMENT
	char dummy[((sizeof(headertest)-1)/HEADER_ALIGNMENT+1)*HEADER_ALIGNMENT-
		sizeof(headertest)];
#endif
	}
	header;

typedef struct { int c,r;
	} dimstest;

typedef struct { int c,r;
#ifdef DIMS_ALIGNMENT
	char dummy[((sizeof(dimstest)-1)/DIMS_ALIGNMENT+1)*DIMS_ALIGNMENT-
		sizeof(dimstest)];
#endif
	} dims;

typedef struct { ULONG s; } inttyp;

typedef struct { header hd; double val; } realtyp;

extern double epsilon,changedepsilon;

typedef enum { c_none, c_allv, c_quit, c_hold,
		c_shg, c_load, c_udf, c_return, c_for, c_end, c_break,
		c_loop, c_if, c_repeat, c_endif, c_else, c_elseif,
		c_clear, c_clg, c_cls, c_exec, c_forget, c_global }
	comtyp;

typedef struct { char *name; comtyp nr; void (*f)(void); } commandtyp;
extern commandtyp command_list[];

typedef struct { char *name; int nargs;  void (*f) (header *); }
	builtintyp;
extern builtintyp builtin_list[];

/* edit.c */

void edit (char *s);
void prompt (void);

/* mainloop.c */

extern char *argname[];
extern int xors[];

void print_error (char *p);

void main_loop (int argc, char *argv[]);

void scan_space (void);
void scan_name (char *name);
void next_line (void);
void trace_udfline(char *);
void getmatrix (header *hd, int *r, int *c, double **x);
header *searchvar (char *name);
header *searchudf (char *name);

#define realof(hd) ((double *)((hd)+1))
#define matrixof(hd) ((double *)((char *)((hd)+1)+sizeof(dims)))
#define dimsof(hd) ((dims *)((hd)+1))
#define commandof(hd) ((int *)((hd)+1))
#define referenceof(hd) (*((header **)((hd)+1)))
#define imagof(hd) ((double *)((hd)+1)+1)
#define rowsof(hd) ((int *)((dims *)((header **)((hd)+1)+1)+1))
#define colsof(hd) ((int *)((dims *)((header **)((hd)+1)+1)+1)+submdimsof((hd))->r)
#define submrefof(hd) (*((header **)((hd)+1)))
#define submdimsof(hd) ((dims *)((header **)((hd)+1)+1))
#define stringof(hd) ((char *)((hd)+1))
#define udfof(hd) ((char *)(hd)+(*((ULONG *)((hd)+1))))
#define udfstartof(hd) ((ULONG *)((hd)+1))
#define helpof(hd) ((char *)(hd)+sizeof(header)+sizeof(ULONG))
#define nextof(hd) ((header *)((char *)(hd)+(hd)->size))

#define mat(m,c,i,j) (m+(((LONG)(c))*(i)+(j)))
#define cmat(m,c,i,j) (m+(2*(((LONG)(c))*(i)+(j))))
#define imat(m,c,i,j) (m+(2*(((LONG)(c))*(i)+(j))))

#define matrixsize(c,r) (sizeof(header)+sizeof(dims)+(c)*(LONG)(r)*sizeof(double))
#define cmatrixsize(c,r) (sizeof(header)+sizeof(dims)+2l*(c)*(LONG)(r)*sizeof(double))
#define imatrixsize(c,r) (sizeof(header)+sizeof(dims)+2l*(c)*(LONG)(r)*sizeof(double))

#define aof(hd) ((double *)((hd)+1))
#define bof(hd) ((double *)((hd)+1)+1)

#define isreal(hd) (((hd)->type==s_real || (hd)->type==s_matrix))
#define isinterval(hd) (((hd)->type==s_interval || (hd)->type==s_imatrix))
#define iscomplex(hd) (((hd)->type==s_complex || (hd)->type==s_cmatrix))

header *next_param (header *hd);

void give_out (header *hd);

int command (void);

/* express.c */

header *getvalue (header *);
header *getvariable (header *);
header *getvariablesub (header *);
header *scan(void);
header *scan_value(void);
void moveresult (header *stack, header *result);
void moveresult1 (header *stack, header *result);
void copy_complex (double *, double *);
void complex_divide (double *, double *, double *, double *, double *,
	double *);
void complex_multiply (double *, double *, double *, double *, double *,
	double *);

void interpret_udf (header *var, header *args, int nargs, int sp);

/* several */

void mnot (header *hd);
void mand (header *hd);
void mor (header *hd);
void mvconcat (header *hd);
void mhconcat (header *hd);

void sort_builtin (void);
void sort_commands (void);

void make_xors (void);

void get_udf (void);

extern FILE *metafile;

header *assign (header *var, header *value);

#define wrong_arg() { error=26; output("Wrong argument\n"); return; }
#define wrong_arg_in(x) { error=26; output1("Wrong arguments for %s\n",x); return; }
#define test_error(x) { if (error) { output1("Error in %s\n",x); } }
#define varnotfound(x) { output1("Variable not found in %s\n",x); }
#define outofram() { output("Out of Memory!\n"); error=120; return; }

#endif
