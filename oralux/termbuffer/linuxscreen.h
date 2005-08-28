#ifndef LINUX_SCR
#define LINUX_SCR 1

/* < VCSA data */
/* struct t_VCSA_attribute */
/* { */
/*   unsigned int isBlink : 1; */
/*   unsigned int myBackgroundColor : 3; */
/*   unsigned int isBold : 1; */
/*   unsigned int myForegroundColor : 3; */
/*   unsigned int : 0; */
/* }; */
/* typedef struct t_VCSA_attribute VCSA_attribute; */

#define getBlinkField(a) (((a)>>7) & 1)
#define getBackgroundField(a) (((a)>>4) & 7)
#define getBoldField(a) (((a)>>3) & 1)
#define getForegroundField(a) ((a) & 7)

/* #define VCSA_compareAttribute(a, b) ((a.isBlink==b.isBlink)\ */
/* 				     &&(a.myBackgroundColor=b.myBackgroundColor)\ */
/* 				     &&(a.isBold=b.isBold)\ */
/* 				     &&(a.myForegroundColor=b.myForegroundColor)) */

struct t_VCSA_Char
{
  unsigned char myChar; 
  unsigned char myAttribute;
};
typedef struct t_VCSA_Char VCSA_Char;

/* > */

int openLinuxScreen( );
int getCursorLinuxScreen( int* theLine, int* theCol);
int readLinuxScreen( int theLine, int theFirstCol, int theLastCol, VCSA_Char** theBuffer, int* theByteNumber);

unsigned char translateCharLinuxScreen( unsigned char theChar, unsigned char theAttribute);

void closeLinuxScreen();
#endif
