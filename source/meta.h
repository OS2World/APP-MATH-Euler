//********** Internal meta file *************************

class Meta
{   public :
	char *Start,*End;
	size_t Size;
	int Active,Full;
	Meta (int size) : Active(1),Full(0)
	{   Size=size*1024l*1024l;
		End=Start=new char[Size];
	}
   Meta (Meta *meta) : Active(1),Full(1)
   {	Size=meta->End-meta->Start;
		Start=new char[Size];
		memcpy(Start,meta->Start,Size);
  		End=Start+Size;
   }
	void clear () { End=Start; Full=0; }
	void write (void *l, int n);
	void commandwrite (int n);
    void longwrite (double s);
    void shortwrite (short n);
    void stringwrite (char *s);
	int nextcommand (char * &p);
	double nextlong (char * &p);
	int nextint (char * &p);
	virtual int gclear (void *p);
	virtual int gclip (void *p, double c, double r, double c1, double r1);
 	virtual int gline (void *p, double c, double r, double c1, double r1,
    	int color, int st, int width);
	virtual int gmarker (void *p, double c, double r, int color, int st);
	virtual int gfill (void *p, double c[], int st, int n, int connect[]);
	virtual int gfillh (void *p, double c[], double hue,
    	int color, int connect);
	virtual int gbar (void *p, double c, double r, double c1, double r1,
    	double hue,	int color, int connect);
	virtual int gbar1 (void *p, double c, double r, double c1, double r1,
		int color, int connect);
	virtual int gtext (void *p, double c, double r, char *text,
    	int color, int centered);
	virtual int gvtext (void *p, double c, double r, char *text,
    	int color, int centered);
	virtual int gvutext (void *p, double c, double r, char *text,
    	int color, int centered);
	virtual int gscale (void *p, double s);
    virtual void replay (void *p, int clip=1);
    virtual void dump (FILE *out);
    virtual void postscript (FILE *out);
};

extern Meta *meta;

void gclear (void);
void gclip(double c, double r, double c1, double r1);
void gline (double c, double r, double c1, double r1, int color,
	int st, int width);
void gtext (double c, double r, char *text, int color, int centered);
void gvtext (double c, double r, char *text, int color, int centered);
void gvutext (double c, double r, char *text, int color, int centered);
void gmarker (double c, double r, int color, int st);
void gfill (double c[], int st, int n, int connect[]);
void gfillh (double c[], double hue, int color, int connect);
void gbar (double c, double r, double c1, double r1, double hue,
	int color, int connect);
void gbar1 (double c, double r, double c1, double r1,
	int color, int connect);
void gscale (double s);

void dump_meta (FILE *out);
void dump_postscript (FILE *out);

void pswindow (double w, double h);

