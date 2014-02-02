extern int nojump;

typedef enum { line_none, line_solid, line_dotted, line_dashed,
		line_arrow }
	linetyp;
typedef enum { marker_cross, marker_circle, marker_diamond, marker_dot,
	marker_plus, marker_square, marker_star }
	markertyp;
typedef enum { fill_blank, fill_filled } filltyp;
typedef enum { key_none, cursor_up, cursor_down, cursor_left, cursor_right,
	escape, deletekey, backspace, clear_home, switch_screen, enter,
	space, line_end, line_start, fk1, fk2, fk3, fk4, fk5, fk6, fk7,
	fk8, fk9, fk10, word_left, word_right, help }
	scantyp;
typedef enum { bar_solid, bar_framed, bar_frame, bar_vhatch,
	bar_hhatch, bar_diagonal1, bar_diagonal2, bar_cross } bartyp;

void mouse (double *, double *);

void getpixelsize (double *x, double *y);

void text_mode (void);
void graphic_mode (void);

int wait_key (int *scan);
int test_key (void);
int test_code (void);

char *cd (char *dir);
char *dir (char *pattern);

void clear_screen (void);
int execute (char *, char *);
int shrink (size_t size);

double myclock (void);

void gprint (char *s); /* print an output text (no newline) */
void gflush (void); /* flush out graphics */

void move_cr (void);
void move_cl (void);
void clear_eol (void);
void  cursor_off (void); 
void cursor_on (void);

void edit_on (void);
void edit_off (void);

void sys_wait (double delay, int *scan);

void set_editline (char *, int);

#ifdef WAVES
void sys_playwav (char *file);
#endif

#ifdef FRAMES
int nframes ();
void showframe (int n);
void addframe ();
void addframe (int w, int h);
void deleteframes ();
void copytoframe (int n);
void copyfromframe (int n);
#endif

double getcolor (int i, int j);

int setcolor(int i, double red, double green, double blue);
void resetcolors();
