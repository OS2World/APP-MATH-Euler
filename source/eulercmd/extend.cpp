#include <stdio.h>

#include "header.h"
#include "extend.h"

builtintyp builtin_list[] =
	{{"index",0,mindex},
	 {"pi",0,mpi},
	 {"time",0,mtime},
	 {"epsilon",0,mepsilon},
	 {"sin",1,msin},
	 {"cos",1,mcos},
	 {"tan",1,mtan},
	 {"atan",1,matan},
	 {"acos",1,macos},
	 {"asin",1,masin},
	 {"exp",1,mexp},
	 {"log",1,mlog},
	 {"sqrt",1,msqrt},
	 {"re",1,mre},
	 {"im",1,mim},
	 {"complex",1,mcomplex},
	 {"iscomplex",1,miscomplex},
	 {"isinterval",1,misinterval},
	 {"isreal",1,misreal},
	 {"isfunction",1,misfunction},
	 {"isvar",1,misvar},
	 {"round",2,mround},
	 {"arg",1,marg},
	 {"abs",1,mabs},
	 {"sum",1,msum},
	 {"prod",1,mprod},
	 {"conj",1,mconj},
	 {"size",-1,msize},
	 {"rows",1,mrows},
	 {"cols",1,mcols},
	 {"zeros",1,mzerosmat},
	 {"ones",1,mones},
	 {"diag",3,mdiag},
	 {"diag",2,mdiag2},
	 {"band",3,mband},
	 {"extrema",1,mextrema},
	 {"mesh",1,mmesh},
	 {"meshbar",1,mmeshflat},
	 {"view",1,mview},
	 {"view",0,mview0},
	 {"wait",1,mwait},
	 {"fastrandom",1,mfastrandom},
	 {"random",1,mrandom},
	 {"normal",1,mnormalnew},
	 {"fastnormal",1,mnormal},
	 {"seed",1,mseed},
	 {"shuffle",1,mshuffle},
	 {"text",2,mtext},
	 {"ctext",2,mctext},
	 {"rtext",2,mrtext},
	 {"vtext",2,mvtext},
	 {"vctext",2,mvctext},
	 {"vrtext",2,mvrtext},
	 {"vutext",2,mvutext},
	 {"vcutext",2,mvcutext},
	 {"vrutext",2,mvrutext},
	 {"textsize",0,mtextsize},
	 {"wire",3,mwire},
	 {"solid",3,msolid},
	 {"solid",4,msolid1},
	 {"plot",2,mplot},
	 {"plotarea",2,mplotarea},
	 {"plot",0,mplot1},
	 {"pixel",0,mpixel},
	 {"mark",2,mmark},
	 {"contour",2,mcontour},
	 {"dup",2,mdup},
	 {"mod",2,mmod},
	 {"format",1,mformat},
	 {"goodformat",1,mgformat},
	 {"expformat",1,meformat},
	 {"fixedformat",1,mfformat},
	 {"iformat",1,miformat},
	 {"fracformat",1,mfracformat},
	 {"color",1,mcolor},
	 {"framecolor",1,mfcolor},
	 {"wirecolor",1,mwcolor},
	 {"textcolor",1,mtcolor},
     {"fillcolor",1,mfillcolor},
	 {"style",1,mstyle},
	 {"markerstyle",1,mmstyle},
	 {"linestyle",1,mlstyle},
	 {"linewidth",1,mlinew},
	 {"window",1,mwindow},
	 {"window",0,mwindow0},
	 {"normaldis",1,mgauss},
	 {"invnormaldis",1,minvgauss},
	 {"fak",1,mfak},
	 {"bin",2,mbin},
	 {"logfak",1,mlogfak},
	 {"logbin",2,mlogbin},
	 {"tdis",2,mtd},
	 {"invtdis",2,minvtd},
	 {"chidis",2,mchi},
	 {"fdis",3,mfdis},
	 {"max",2,mmax},
	 {"min",2,mmin},
	 {"scale",1,mscale},
	 {"sort",1,msort},
	 {"nonzeros",1,mnonzeros},
	 {"count",2,mstatistics},
	 {"floor",1,mfloor},
	 {"ceil",1,mceil},
	 {"cumsum",1,mcumsum},
	 {"cumprod",1,mcumprod},
	 {"free",0,mfree},
#ifndef NOSHRINK
	 {"shrink",1,mshrink},
#endif
	 {"input",1,minput},
	 {"max",1,mmax1},
	 {"min",1,mmin1},
	 {"eval",-1,mdo},
	 {"polyval",2,polyval},
	 {"polyadd",2,polyadd},
	 {"polymult",2,polymult},
	 {"polydiv",2,polydiv},
	 {"interp",2,dd},
	 {"divdif",2,dd},
	 {"interpval",3,ddval},
	 {"divdifval",3,ddval},
	 {"polytrans",2,polydd},
	 {"polycons",1,polyzeros},
	 {"polytrunc",1,polytrunc},
	 {"char",1,mchar},
	 {"ascii",1,mascii},
	 {"lu",1,mlu},
	 {"lusolve",2,mlusolve},
	 {"fft",1,mfft},
	 {"ifft",1,mifft},
	 {"polysolve",1,mzeros},
	 {"error",1,merror},
	 {"printf",2,mprintf},
	 {"sign",1,msign},
	 {"mouse",0,mmouse},
	 {"hb",1,mtridiag},
	 {"charpoly",1,mcharpoly},
	 {"stringcompare",2,mscompare},
	 {"find",2,mfind},
	 {"setdiag",3,msetdiag},
	 {"polyroot",2,mzeros1},
	 {"argn",0,margn},
	 {"setkey",2,msetkey},
	 {"any",1,many},
	 {"changedir",1,mcd},
	 {"searchfile",1,mdir},
	 {"searchfile",0,mdir0},
	 {"bandmult",2,wmultiply},
	 {"symmult",2,smultiply},
	 {"project",3,mproject},
	 {"args",1,margs},
	 {"args",0,margs0},
	 {"setplot",1,msetplot},
	 {"scaling",1,mscaling},
	 {"holding",1,mholding},
	 {"keepsquare",1,mkeepsquare},
	 {"holding",0,mholding0},
	 {"lineinput",1,mlineinput},
	 {"interpret",1,minterpret},
	 {"evaluate",1,mevaluate},
	 {"name",1,mname},
	 {"twosides",1,mtwosides},
	 {"meshfactor",1,mmeshfactor},
	 {"setepsilon",1,msetepsilon},
	 {"localepsilon",1,mlocalepsilon},
	 {"flipx",1,mflipx},
	 {"flipy",1,mflipy},
     {"rotleft",1,mrotleft},
     {"rotright",1,mrotright},
     {"shiftleft",1,mshiftleft},
     {"shiftright",1,mshiftright},
	 {"matrix",2,mmatrix},
	 {"jacobi",1,mjacobi},
	 {"frame",0,mframe},
#ifndef SPLIT_MEM
	 {"store",1,mstore},
	 {"restore",1,mrestore},
#endif
	 {"key",0,mkey},
	 {"errorlevel",1,merrlevel},
	 {"density",1,mdensity},
	 {"huecolor",1,mdcolor},
	 {"huegrid",1,mdgrid},
	 {"solidhue",4,msolidh},
	 {"redim",2,mredim},
	 {"resize",2,mresize},
	 {"open",2,mopen},
	 {"close",0,mclose},
	 {"putchar",1,mputchar},
	 {"putword",1,mputword},
	 {"putlongword",1,mputlongword},
	 {"getchar",0,mgetchar},
	 {"getchar",1,mgetchar1},
	 {"getword",0,mgetword},
	 {"getword",1,mgetword1},
	 {"getlongword",0,mgetlongword},
	 {"getlongword",1,mgetlongword1},
	 {"putuchar",1,mputchar},
	 {"putuword",1,mputword},
	 {"putulongword",1,mputlongword},
	 {"getuchar",0,mgetchar},
	 {"getuchar",1,mgetchar1},
	 {"getuword",0,mgetword},
	 {"getuword",1,mgetword1},
	 {"getulongword",0,mgetlongword},
	 {"getulongword",1,mgetlongword1},
	 {"getstring",1,mgetstring},
	 {"write",1,mwrite},
	 {"getvector",1,mgetvector},
	 {"eof",0,meof},
	 {"simplex",3,msimplex},
	 {"left",1,mleft},
	 {"right",1,mright},
	 {"middle",1,mmiddle},
	 {"diameter",1,mdiameter},
	 {"interval",2,minterval},
	 {"accuload",1,maccuload},
	 {"accuadd",1,maccuadd},
	 {"accuload",2,maccuload2},
	 {"accuadd",2,maccuadd2},
	 {"residuum",3,mresiduum},
	 {"accu",0,maccu1},
	 {"accure",0,maccu1},
	 {"accua",0,maccu1},
	 {"accuim",0,maccu2},
	 {"accub",0,maccu2},
	 {"expand",2,mexpand},
	 {"intersects",2,mintersects},
	 {"typeof",1,mtype},
	 {"markersize",1,mmarkersize},
	 {"bar",1,mbar},
	 {"barcolor",1,mbarcolor},
	 {"barstyle",1,mbarstyle},
	 {"map",-1,mmap1},
	 {"gammaln",1,mgammaln},
	 {"gamma",1,mgamma},
	 {"gamma",2,mgammai},
	 {"brent",-1,mbrent},
	 {"nelder",-1,mnelder},
     {"runge1",-1,mrunge1},
     {"runge2",-1,mrunge2},
	 {"testkey",0,mcode},
	 {"svd",1,msvd},
	 {"toeplitz",1,mtoeplitz},
	 {"toeplitzsolve",2,msolvetoeplitz},
	 {"betai1",3,mbetai},
     {"besselj",2,mbesselj},
	 {"bessely",2,mbessely},
	 {"besselallr",2,mbesselall},
	 {"besseli",2,mbesseli},
	 {"besselk",2,mbesselk},
	 {"besselmodallr",2,mbesselmodall},
     {"pswindow",1,mpswindow},
#ifdef WAVES
	 {"playwave",1,mplaywav},
#endif
#ifdef FRAMES
	 {"dll",3,mdll},
	 {"addpage",0,maddframe},
	 {"addpage",2,maddframe2},
	 {"showpage",1,mshowframe},
	 {"copytopage",1,mcopyto},
	 {"copyfrompage",1,mcopyfrom},
	 {"pages",0,mframes},
	 {"deletepages",0,mdeleteframes},
#endif
	 {"clip",1,mclip},
	 {"clip",0,mclip0},
	 {"antialiasing",0,mantialiasing0},
	 {"antialiasing",1,mantialiasing},
	 {(char *)0,0,0} };
