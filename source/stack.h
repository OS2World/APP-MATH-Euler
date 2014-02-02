void clear (void);

header *new_matrix (int c, int r, char *name);
header *new_cmatrix (int c, int r, char *name);
header *new_imatrix (int c, int r, char *name);
header *new_real (double x, char *name);
header *new_complex (double x, double y, char *name);
header *new_reference (header *hd, char *name);
header *new_submatrix (header *hd, header *cols, header *rows,
	char *name);
header *new_csubmatrix (header *hd, header *cols, header *rows,
	char *name);
header *new_isubmatrix (header *hd, header *cols, header *rows,
	char *name);
header *new_command (int no);
header *new_string (char *s, ULONG size, char *name);
header *new_udf (char *name);
header *new_subm (header *var, LONG l, char *name);
header *new_csubm (header *var, LONG l, char *name);
header *new_isubm (header *var, LONG l, char *name);
header *new_interval (double x, double y, char *name);

int xor (char *n);

void kill_udf (char *name);

