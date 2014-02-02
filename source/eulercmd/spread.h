void spread1 (double f (double),
	void fc (double *, double *, double *, double *),
	header *hd);
void spread1i (double f (double),
	void fc (double *, double *, double *, double *),
	void fi (double *, double *, double *, double *),
	header *hd);
void spread1r (double f (double),
	void fc (double *, double *, double *),
	header *hd);
void spread1ir (double f (double),
	void fc (double *, double *, double *),
	void fi (double *, double *, double *, double *),
	header *hd);
void spread2r (void f (double *, double *, double *),
	void fc (double *, double *, double *, double *, double *, double *),
	header *hd);
void spreadir (
	void fi (double *, double *, double *),
	header *hd);
void spreadir1 (
	void f (double *, double *),
	void fi (double *, double *, double *),
	header *hd);

header *map1 (void f(double *, double *),
	void fc(double *, double *, double *, double *),
	header *hd);
header *map1i (void f(double *, double *, double *, double *),
	header *hd);
header *map1ir (
	void f(double *, double *, double *),
	header *hd);
header *map1r (void f(double *, double *),
	void fc(double *, double *, double *),
	header *hd);



header * mapf2 (void f (double *, double *, double *),
	void fc (double *, double *, double *, double *, double *, double *),
	void fi (double *, double *, double *, double *, double *, double *),
	header *hd1, header *hd2);
void spreadf2 (void f (double *, double *, double *),
	void fc (double *, double *, double *, double *, double *, double *),
	void fi (double *, double *, double *, double *, double *, double *),
	header *hd);
header * mapf2r (void f (double *, double *, double *),
	void fc (double *, double *, double *, double *, double *),
	void fi (double *, double *, double *, double *, double *),
	header *hd1, header *hd2);
void spreadf2r (void f (double *, double *, double *),
	void fc (double *, double *, double *, double *, double *),
	void fi (double *, double *, double *, double *, double *),
	header *hd);


