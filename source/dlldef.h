enum { s_real,s_complex,s_matrix,s_cmatrix,
	s_reference,s_command,s_submatrix,s_csubmatrix,s_string,s_udf,
	s_interval,s_imatrix,s_isubmatrix };

typedef struct
{	long size; char name[16]; int xor; int type; int flags;
}
header;

typedef struct
{	int c,r;
}
dims;

#define stringof(hd) ((char *)((hd)+1))
#define realof(hd) ((double *)((hd)+1))
#define matrixof(hd) ((double *)((char *)((hd)+1)+sizeof(dims)))
#define dimsof(hd) ((dims *)((hd)+1))

header *make_header (int type, int size, char * &newram,
	char *ramend);

header *new_string (char *s,
	char * &newram, char *ramend);

header *new_real (double x,
	char * &newram, char *ramend);

header *new_complex (double x, double y,
	char * &newram, char *ramend);

header *new_interval (double a, double b,
	char * &newram, char *ramend);

header *new_matrix (int rows, int columns,
	char * &newram, char *ramend);

header *new_cmatrix (int rows, int columns,
	char * &newram, char *ramend);

header *new_imatrix (int rows, int columns,
	char * &newram, char *ramend);

