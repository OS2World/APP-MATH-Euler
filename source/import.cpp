#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{	int i,c;
	FILE *in,*out;
	for (i=1; i<argc; i++)
	{   printf("Processing %s\n",argv[i]);
		in=fopen(argv[i],"r");
		if (!in)
		{	fprintf(stderr,"Unable to open %s\n",argv[i]);
			exit(1);
		}
		out=fopen("import$$.$$$","w");
		printf("Importing %s\n",argv[i]);
		if (!out)
		{	fprintf(stderr,"Unable to open temporary import$$.$$$");
			fclose(in);
			exit(1);
		}
		while (!feof(in))
		{	c=fgetc(in);
			if (c==EOF) break;
			if (c==13) continue;
			if (c==10) c='\n';
			fputc(c,out);
		}
		if (ferror(in))
		{	fprintf(stderr,"Read error with %s\n",argv[i]);
			fclose(in); fclose(out); exit(1);
		}
		if (ferror(out))
		{	fprintf(stderr,"Write error import$$.$$$\n");
			fclose(in); fclose(out); exit(1);
		}
		fclose(out);
		fclose(in);
		if (remove(argv[i]))
		{	fprintf(stderr,"Could not remove %s\n",argv[i]);
			exit(1);
		}
		if (rename("import$$.$$$",argv[i]))
		{	fprintf(stderr,"Could not copy import$$.$$$ to %s\n",argv[i]);
			exit(1);
		}
	}
	return 0;
}
