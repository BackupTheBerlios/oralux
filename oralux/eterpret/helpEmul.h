#ifdef DEBUG
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#define HELPEMULFILE "/home/gilles/eterpret.log"

extern struct timeval myOriginTime;
extern double timeDiff(struct timeval *t);

#define HELPEMULINIT \
{\
  unlink(HELPEMULFILE);\
  gettimeofday(&myOriginTime, NULL);\
}

#define ENTER(a) \
{\
  FILE *fd=fopen(HELPEMULFILE, "a");\
  fprintf( fd, "== ENTER %s (%s:%d - %3.3f)\n", a,__FILE__,__LINE__, timeDiff(&myOriginTime));\
  fclose(fd);\
}
#define SHOW(a)\
{\
  FILE *fd=fopen(HELPEMULFILE, "a");\
  fprintf( fd, "%s\n", a);\
  fclose(fd);\
}
#define SHOW1(a)\
{\
  FILE *fd=fopen(HELPEMULFILE, "a");\
  fprintf( fd, "%s\n", a);\
  fclose(fd);\
}
#define SHOW2(a,b)\
{\
  FILE *fd=fopen(HELPEMULFILE, "a");\
  fprintf( fd, a, b);\
  fclose(fd);\
}
#define SHOW3(a,b,c)\
{\
  FILE *fd=fopen(HELPEMULFILE, "a");\
  fprintf( fd, a, b, c);\
  fclose(fd);\
}
#define SHOW4(a,b,c,d)\
{\
  FILE *fd=fopen(HELPEMULFILE, "a");\
  fprintf( fd, a, b, c, d);\
  fclose(fd);\
}
#else
#define HELPEMULINIT
#define ENTER(a)
#define SHOW(a)
#define SHOW1(a)
#define SHOW2(a,b)
#define SHOW3(a,b,c)
#define SHOW4(a,b,c,d)
#endif
