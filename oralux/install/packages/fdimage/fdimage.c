/*
 * Copyright (c) 1996-7 Robert Nordier
 * All rights reserved.
 *
 * $Id: fdimage.c,v 1.1 2004/09/27 20:30:07 gcasse Exp $
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>

const char SCCSID[] = "@(#)fdimage.c 1.2 rnordier";

#define FAR  __far
#define ASM  __asm

#define BYTE   unsigned char
#define WORD   unsigned short
#define DWORD  unsigned long

#define IOSETDEV   0x40         /* IOCTL: set device parameters */
#define IOWRITE    0x41         /* IOCTL: write track */
#define IOFORMAT   0x42         /* IOCTL: format/verify track */
#define IOGETDEV   0x60         /* IOCTL: get device parameters */

#define DEV4009    0            /* 5.25-inch 360K drive */
#define DEV8015    1            /* 5.25-inch 1.2M drive */
#define DEV8009    2            /* 3.5-inch 720K drive */
#define DEVHARD    5            /* hard drive */
#define DEV8018    7            /* 3.5-inch 1.44M drive */
#define DEV8036    9            /* 3.5-inch 2.88M drive */

struct bpb {
   WORD secsiz;                 /* sector size */
   BYTE spc;                    /* sectors per cluster */
   WORD ressec;                 /* reserved sectors */
   BYTE fats;                   /* FATs */
   WORD dirents;                /* root directory entries */
   WORD secs;                   /* total sectors */
   BYTE media;                  /* media descriptor */
   WORD spf;                    /* sectors per FAT */
   WORD spt;                    /* sectors per track */
   WORD heads;                  /* drive heads */
   DWORD hidsec;                /* hidden sectors */
   DWORD lsecs;                 /* huge sectors */
};

struct deviceparams {
   BYTE func;                   /* special functions */
   BYTE dev;                    /* device type */
   WORD attr;                   /* device attributes */
   WORD cyls;                   /* cylinders */
   BYTE media;                  /* media type */
   struct bpb bpb;              /* bpb */
};

struct fvblock {
   BYTE func;                   /* special functions */
   WORD head;                   /* head */
   WORD cyl;                    /* cylinder */
   WORD trks;                   /* tracks */
};

struct rwblock {
   BYTE func;                   /* special functions */
   WORD head;                   /* head */
   WORD cyl;                    /* cylinder */
   WORD sec;                    /* starting sector */
   WORD nsecs;                  /* number of sectors */
   void FAR *buf;               /* buffer */
};

#define E_DOSERR  0x80
#define E_UNKNWN  (E_DOSERR + 14)

static char *dos_errlist[] = {
   "DOS Error 0",                    /*  0 */
   "Invalid function",               /*  1 */
   "File not found",                 /*  2 */
   "Path not found",                 /*  3 */
   "Too many open files",            /*  4 */
   "Access denied",                  /*  5 */
   "Invalid handle",                 /*  6 */
   "Memory arena trashed",           /*  7 */
   "Not enough memory",              /*  8 */
   "Invalid block",                  /*  9 */
   "Bad environment",                /* 10 */
   "Bad format",                     /* 11 */
   "Invalid access",                 /* 12 */
   "Invalid data",                   /* 13 */
   "Unknown DOS error",              /* 14 */
   "Invalid drive",                  /* 15 */
   "Current directory",              /* 16 */
   "Not same device",                /* 17 */
   "No more files",                  /* 18 */
   "Drive is write protected",       /* 19 */
   "Bad unit",                       /* 20 */
   "Drive not ready",                /* 21 */
   "Bad command",                    /* 22 */
   "CRC error",                      /* 23 */
   "Bad request structure length",   /* 24 */
   "Seek error",                     /* 25 */
   "Media error",                    /* 26 */
   "Sector not found",               /* 27 */
   "Out of paper",                   /* 28 */
   "Write fault",                    /* 29 */
   "Read fault",                     /* 30 */
   "General failure"                 /* 31 */
};
static int dos_nerr = sizeof(dos_errlist) / sizeof(dos_errlist[0]);

#define E_PRGERR  0x100
#define E_INTRNL  (E_PRGERR +  0)
#define E_DOSVER  (E_PRGERR +  1)
#define E_INVDRV  (E_PRGERR +  2)
#define E_DEVDEV  (E_PRGERR +  3)
#define E_NOTFLP  (E_PRGERR +  4)
#define E_SUBDRV  (E_PRGERR +  5)
#define E_DRVOPT  (E_PRGERR +  6)
#define E_TOOBIG  (E_PRGERR +  7)

static char *errlist[] = {
   "Internal error",
   "This program requires DOS 3.30 or greater",
   "Invalid drive specification",
   "Source and target refer to same device",
   "Not a floppy drive",
   "ASSIGNed or SUBSTed drive",
   "Parameters not supported by drive",
   "File is too big"
};
static int nerr = sizeof(errlist) / sizeof(errlist[0]);

static int optind = 1;
static char *optarg = NULL;

#define SWITCHAR  '-'           /* option indicator */
#define DOSMINVER  0x031e       /* minimum DOS version (3.30) */
#define FDMAXSPT   36           /* floppy maximum sectors per track */
#define SECSIZ     0x200        /* sector size */
#define SIZBUF     0x1000       /* copy buffer size */

#define K_ARGS     8
#define M_ARGS     3
#define F_ARGS     (K_ARGS + M_ARGS)

static char *f_args[F_ARGS] = {
   "160", "180",  "320",  "360",      /* K_ARGS */
   "720", "1200", "1440", "2880",
   "1.2", "1.44", "2.88"              /* M_ARGS */
};

#define BPBCNT     8

static struct bpb stdbpb[BPBCNT] = {
   {512, 1, 1, 2,  64,  320, 0xfe, 1,  8, 1, 0, 0},   /* 160K */
   {512, 1, 1, 2,  64,  360, 0xfc, 2,  9, 1, 0, 0},   /* 180K */
   {512, 2, 1, 2, 112,  640, 0xff, 1,  8, 2, 0, 0},   /* 320K */
   {512, 2, 1, 2, 112,  720, 0xfd, 2,  9, 2, 0, 0},   /* 360K */
   {512, 2, 1, 2, 112, 1440, 0xf9, 3,  9, 2, 0, 0},   /* 720K */
   {512, 1, 1, 2, 224, 2400, 0xf9, 7, 15, 2, 0, 0},   /* 1.2M */
   {512, 1, 1, 2, 224, 2880, 0xf0, 9, 18, 2, 0, 0},   /* 1.44M */
   {512, 2, 1, 2, 240, 5760, 0xf0, 9, 36, 2, 0, 0}    /* 2.88M */
};

static jmp_buf jmpbuf;

static void ctrlc(int sig);
static void help(void);
static void usage(void);
static void error(int err, const char *locus);
static void warn(int err, const char *locus);
static int argfmt(char *s);
static int setfmt(int opt_f, struct deviceparams *dp);
static int chkdrv(int drv);
static int getopt(int argc, char *argv[], char *opts);
static int erropt(char *msg, int c);
static int dos_getver(void);
static int dos_remote(int drive, int *flags);
static int dos_truename(char *d, const char *s);
static int dos_gioctl(int func, int drive, void FAR *data);
static int dos_getlogdrv(int drive, int *drvptr);
static int dos_setlogdrv(int drive);

int main(int argc, char *argv[])
{
   struct stat sb;
   struct {
      struct deviceparams dp;
      BYTE reserved[6];
      WORD spt;
      struct {
         WORD sectno;
         WORD secsiz;
      } tkl[FDMAXSPT];
   } dptkl;
   struct deviceparams dp;
   struct fvblock fv;
   struct rwblock rw;
   char *buf;
   int opt_f = -1, opt_q = 0, opt_v = 0;
   int retc, fd, drv, map, c, e, i, n;

   if (dos_getver() < DOSMINVER)
      error(E_DOSVER, NULL);
   if (argc < 2)
      help();
   while ((c = getopt(argc, argv, "f:qv")) != -1)
      switch(c) {
      case 'f':
         if ((opt_f = argfmt(optarg)) == -1) {
            fprintf(stderr, "Invalid argument -- %s\n", optarg);
            usage();
         }
         break;
      case 'q':
         opt_q++;
         break;
      case 'v':
         opt_v++;
         break;
      default:
         usage();
      }
   if (argc - optind != 2)
      usage();
   if (!(buf = malloc(SIZBUF)))
      error(errno, NULL);
   if ((fd = open(argv[optind], O_RDONLY | O_BINARY)) == -1 ||
       fstat(fd, &sb))
      error(errno, argv[optind]);
   if ((e = dos_truename(buf, argv[optind])) != 0)
      error(E_DOSERR + e, NULL);
   if (!isalpha(*argv[optind + 1]) || strcmp(argv[optind + 1] + 1, ":"))
      error(E_INVDRV, argv[optind + 1]);
   drv = 1 + toupper(*argv[optind + 1]) - 'A';
   if (dos_getlogdrv(drv, &map))
      map = 0;
   if (*buf == 'A' + drv - 1 || map && *buf == 'A' + map - 1)
      error(E_DEVDEV, NULL);
   if ((e = chkdrv(drv)) != 0)
      error(e, argv[optind + 1]);
   memset(&dptkl, 0, sizeof(dptkl));
   if ((e = dos_gioctl(IOGETDEV, drv, &dptkl)) != 0)
      error(E_DOSERR + e, NULL);
   if (dptkl.dp.dev == DEVHARD)
      error(E_NOTFLP, argv[optind + 1]);
   dp = dptkl.dp;
   if (opt_f >= 0 && (e = setfmt(opt_f, &dptkl.dp)) != 0)
      error(e, NULL);
   if (sb.st_size > (DWORD)dptkl.dp.bpb.secs * SECSIZ)
      error(E_TOOBIG, argv[optind]);
   if (opt_v) {
      printf("Insert new diskette for drive %c:\n"
             "and press ENTER when ready", 'A' + drv - 1);
      fgetc(stdin);
      putchar('\n');
   }
   if (map && map != drv && (e = dos_setlogdrv(drv)) != 0)
      error(E_DOSERR + e, argv[optind + 1]);
   retc = 1;
   if ((e = setjmp(jmpbuf)) == 0) {
      signal(SIGINT, ctrlc);
      dptkl.spt = dptkl.dp.bpb.spt;
      for (i = 0; i < dptkl.spt; i++) {
         dptkl.tkl[i].sectno = 1 + i;
         dptkl.tkl[i].secsiz = SECSIZ;
      }
      dptkl.dp.func = 5;
      if ((e = dos_gioctl(IOSETDEV, drv, &dptkl)) != 0)
         warn(E_DOSERR + e, NULL);
      else {
         if (!opt_q) {
            fv.func = fv.cyl = fv.head = 0;
            c = dptkl.dp.bpb.secs / dptkl.dp.bpb.spt;
            for (i = 0; i < c; i++) {
               if (opt_v)
                  fprintf(stderr, "Formatting: cyl %u  hd %u\r",
                          fv.cyl, fv.head);
               if ((e = dos_gioctl(IOFORMAT, drv, &fv)) != 0)
                  break;
               if (++fv.head == dptkl.dp.bpb.heads) {
                  fv.cyl++;
                  fv.head = 0;
               }
            }
            if (opt_v)
               fputc('\n', stderr);
            if (e)
               warn(E_DOSERR + e, NULL);
         }
         if (!e) {
            c = i = 0;
            for (;;) {
               if (!c)
                  c = dptkl.dp.bpb.heads * dptkl.dp.bpb.spt * SECSIZ;
               n = c < SIZBUF ? c : SIZBUF;
               c -= n;
               if ((n = read(fd, buf, n)) <= 0) {
                  e = errno;
                  break;
               }
               rw.nsecs = (n + SECSIZ - 1) / SECSIZ;
               e = i;
               i += rw.nsecs;
               rw.sec = e % dptkl.dp.bpb.spt;
               e /= dptkl.dp.bpb.spt;
               rw.head = e % dptkl.dp.bpb.heads;
               rw.cyl = e / dptkl.dp.bpb.heads;
               rw.buf = buf;
               rw.func = 0;
               if (opt_v)
                  fprintf(stderr, "Writing: cyl %u  hd %u\r",
                          rw.cyl, rw.head);
               if ((e = dos_gioctl(IOWRITE, drv, &rw)) != 0)
                  break;
            }
            if (opt_v)
               fputc('\n', stderr);
            if (n == -1)
               warn(e, argv[optind]);
            else if (e)
               warn(E_DOSERR + e, NULL);
            else {
               if (opt_v)
                  fprintf(stderr, "Done\n");
               retc = 0;
            }
         }
      }
   }
   dptkl.dp = dp;
   dptkl.spt = 0;
   dptkl.dp.func = 4;
   dos_gioctl(IOSETDEV, drv, &dptkl);
   return retc;
}

static void ctrlc(int sig)
{
   signal(sig, SIG_IGN);
   longjmp(jmpbuf, 1);
}

static void help(void)
{
   static const char helpst[] =
      "FDIMAGE - Write disk image to floppy disk\n"
      "Copyright (c) 1996-7 Robert Nordier\n\n"
      "Usage: fdimage [-f size] [-q] [-v] file drive\n\n"
      "  -f size  Specify the floppy disk format by capacity, eg:\n"
      "           160K, 180K, 320K, 360K, 720K, 1.2M, 1.44M, 2.88M\n"
      "  -q       Quick mode: don't format the disk\n"
      "  -v       Verbose\n";

   fprintf(stderr, "%s", helpst);
   exit(1);
}

static void usage(void)
{
   fprintf(stderr, "Usage: fdimage [-f size] [-q] [-v] file drive\n");
   exit(1);
}

static void error(int err, const char *locus)
{
   warn(err, locus);
   exit(1);
}

static void warn(int err, const char *locus)
{
   char *msg;

   if (err >= 0 && err < sys_nerr)
      msg = sys_errlist[err];
   else if (err >= E_DOSERR && err < E_PRGERR) {
      if (err >= E_DOSERR + dos_nerr)
	 err = E_UNKNWN;
      msg = dos_errlist[err - E_DOSERR];
   } else {
      if (err < E_PRGERR || err >= E_PRGERR + nerr)
	 err = E_INTRNL;
      msg = errlist[err - E_PRGERR];
   }
   if (locus)
      fprintf(stderr, "%s - ", locus);
   fputs(msg, stderr);
   fputc('\n', stderr);
}

static int argfmt(char *s)
{
   int i;

   for (i = 0; i < F_ARGS; i++)
      if (!strncmp(s, f_args[i], strlen(f_args[i]))) {
         s += strlen(f_args[i]);
         break;
      }
   if (i < F_ARGS) {
      if (toupper(*s) == (i < K_ARGS ? 'K' : 'M')) {
         s++;
         if (toupper(*s) == 'B')
            s++;
      }
      if (*s)
         i = F_ARGS;
      else if (i >= K_ARGS)
         i -= M_ARGS;
   }
   return i < F_ARGS ? i : -1;
}

static int setfmt(int i, struct deviceparams *dp)
{
   switch (dp->dev) {
      case DEV4009:
         if (i == 5)
            return E_DRVOPT;
         /* falls through */
      case DEV8015:
         if (i == 4 || i >= 6)
            return E_DRVOPT;
         break;
      case DEV8009:
         if (i == 6)
            return E_DRVOPT;
         /* falls through */
      case DEV8018:
         if (i == 7)
            return E_DRVOPT;
         /* falls through */
      case DEV8036:
         if (i <= 3 || i == 5)
            return E_DRVOPT;
         break;
      default:
         return E_DRVOPT;
   }
   if (dp->dev == DEV8015 && i <= 3) {
      dp->media = 1;
      dp->cyls = 40;
   }
   dp->bpb = stdbpb[i];
   return 0;
}

static int chkdrv(int drv)
{
   char str[6], buf[7];
   int e, flags;

   if ((e = dos_remote(drv, &flags)) != 0)
      return E_DOSERR + e;
   if (flags & 0x1000)
      return E_NOTFLP;
   if (flags & 0x200)
      return E_SUBDRV;
   *strcpy(str, "A:CON") += (char)(drv - 1);
   if ((e = dos_truename(buf, str)) != 0)
      return E_DOSERR + e;
   return *buf != *str ? E_SUBDRV : 0;
}

static int getopt(int argc, char *argv[], char *opts)
{
   static char *p = NULL;
   char *q;
   int c;

   if (!p) {
      if (optind >= argc)
         return -1;
      p = argv[optind];
      if (*p++ != SWITCHAR || !*p)
         return -1;
      if (*p == SWITCHAR && !p[1]) {
         optind++;
         return -1;
      }
   }
   c = *p++;
   if (!(q = strchr(opts, c)) || c == ':')
      c = erropt("Illegal option", c);
   else if (*++q == ':')
      if (!*p)
         if (optind == argc - 1)
	    c = erropt("Option requires an argument", c);
         else
            optarg = argv[++optind];
      else
         optarg = p;
   else
      optarg = NULL;
   if (q && *q == ':' || !*p) {
      optind++;
      p = NULL;
   }
   return c;
}

static int erropt(char *msg, int c)
{
   fprintf(stderr, "%s -- %c\n", msg, c);
   return '?';
}

static int dos_getver(void)
{
   int ver;

   ASM  mov ax,3000h
   ASM  int 21h
   ASM  xchg ah,al
   ASM  mov ver,ax
   return ver;
}

static int dos_remote(int drive, int *flags)
{
   int err;

   ASM  mov bx,drive
   ASM  mov ax,4409h
   ASM  int 21h
   ASM  jc done
   ASM  mov drive,dx
   *flags = drive;
   ASM  xor ax,ax
done:
   ASM  mov err,ax
   return err;
}

static int dos_truename(char *d, const char *s)
{
   int err;

   ASM  push ds
   ASM  pop es
   ASM  mov si,s
   ASM  mov di,d
   ASM  mov ah,60h
   ASM  int 21h
   ASM  jc done
   ASM  xor ax,ax
done:
   ASM  mov err,ax
   return err;
}

static int dos_gioctl(int func, int drive, void FAR *data)
{
   int err;

   ASM  push ds
   ASM  mov bx,drive
   ASM  mov cl,byte ptr func
   ASM  mov ch,08h
   ASM  lds dx,data
   ASM  mov ax,440dh
   ASM  int 21h
   ASM  mov ax,0
   ASM  jnc done
   ASM  xor bx,bx
   ASM  push bp
   ASM  push di
   ASM  push si
   ASM  mov ah,59h
   ASM  int 21h
   ASM  pop si
   ASM  pop di
   ASM  pop bp
done:
   ASM  mov err,ax
   ASM  pop ds
   return err;
}

static int dos_getlogdrv(int drive, int *drvptr)
{
   int err;

   ASM  mov bx,drive
   ASM  mov ax,440eh
   ASM  int 21h
   ASM  jc done
   ASM  cbw
   ASM  mov drive,ax
   *drvptr = drive;
   ASM  xor ax,ax
done:
   ASM  mov err,ax
   return err;
}

static int dos_setlogdrv(int drive)
{
   int err;

   ASM  mov bx,drive
   ASM  mov ax,440fh
   ASM  int 21h
   ASM  jc done
   ASM  xor ax,ax
done:
   ASM  mov err,ax
   return err;
}
