#include <stdio.h>

// G.Casse : compilation Feb 2004
extern void exit(int); 
//

void
regerror(char *s)
{
#ifdef ERRAVAIL
	error("regexp: %s", s);
#else
	fprintf(stderr, "regexp(3): %s", s);
	exit(1);
#endif
	/* NOTREACHED */
}
