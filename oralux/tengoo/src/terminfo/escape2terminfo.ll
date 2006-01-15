	#include "escape2terminfo.h"
	#include "linuxconsole.h"
	#include "debug.h"

	int TheParameter[MAXPARAM];
	int TheNumberOfParameter;
	struct t_style TheDefaultStyle;
	struct t_style TheCurrentStyle;
	YY_BUFFER_STATE TheBufferState;


NUL	\x00
BEL	\x07
H	\x08
I	\x09
J	\x0a
M	\x0d
ESC	\x1b
DIGIT	[0-9]
LETTER	[A-Za-z]
ALT	[\x80-\xff]

%s graphics alternate
%%

{ESC}\(0		BEGIN(graphics);
<graphics>{ESC}{ESC}\(B	{
			unput(0x1b); /* ESC #1: new future sequence, ESC #2 to return to iso 8859-1 mapping */
			BEGIN(INITIAL); 
			}
<graphics>{ESC}\(B	BEGIN(INITIAL); /* return to iso 8859-1 mapping */
<graphics>.		{ /* erase any character in graphics mapping */
			*yytext=0x20;
			return TEXTFIELD;
			}
			/* <alternate>{ESC}\[10m	BEGIN(INITIAL); */
<alternate>{ALT}	{ /* erase a (guessed) graphical character */
			*yytext=0x20;
			return TEXTFIELD;
			}
{ESC}\[{DIGIT}+(;{DIGIT}+)*m	{/* Change mode (affect also the bg, fg colors) */
				getStyle( &TheCurrentStyle, &TheDefaultStyle);
				if (TheCurrentStyle.isAlternate)
				{
				  BEGIN(alternate);
				}
				else
				{
				  BEGIN(INITIAL);
				}
				return SGR; /* Group also SETF, SETB, OP,...*/
				}
{NUL}			/* filtered */
{BEL}			return BEL;
{M}{J}			return NEL; /* cr lf */
{I}			return HT; /* horizontal tab */
{J}			return CUD1; /* down one line */
{M}			return CR; /* cr... */ 
{H}			return CUB1; /* move left */
{ESC}8			return RC;
{ESC}7			return SC;
{ESC}\[C		return CUF1; /* move right */
{ESC}\[H{ESC}\[2?J	return CLEAR;
{ESC}\[[12]?J		{
			TheParameter[0]=(yyleng==3) ? 0 : yytext[2]-'0';
			return ED; /* clear to end */
			}
{ESC}\[[12]?K		{
			TheParameter[0]=(yyleng==3) ? 0 : yytext[2]-'0';
			return EL;
			}
{ESC}\[H		return HOME;
{ESC}\[{DIGIT}*@	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return ICH; /* insert char */
			}
{ESC}\[{DIGIT}*L	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return IL; /* insert line */
			}
{ESC}\[{DIGIT}*G	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return HPA; /* horizontal pos */
			}
{ESC}\[{DIGIT}*d	{ /* Line number */
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return VPA; /* vertical pos */
			}
{ESC}\[{DIGIT}*P	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return DCH; /* delete one char */
			}
{ESC}\[{DIGIT}*M	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return DL; /* delete line */
			}
{ESC}\[{DIGIT}*X	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return ECH; /* erase char */
			}
{ESC}\[{DIGIT}*A	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return CUU; /* up */
			}
{ESC}\[\?{DIGIT}*{LETTER}	/* filtered */ 
{ESC}\[{DIGIT}+;{DIGIT}+H	{
				int i=0;
				for (i=2;i<yyleng;i++)
				{
				  if (yytext[i]==';')
				  {
				    TheParameter[0]=getDigit(yytext+2,i-2);
				    break;
				  }
				}
				TheParameter[1]=getDigit(yytext+i+1, yyleng-1-i-1);	
				return CUP; /* move to row/col */
				}
.			return TEXTFIELD;

%%
	void initEscape2Terminfo( char* theBuffer, int theLength)
	{
	  TheBufferState = yy_scan_bytes ( theBuffer, theLength);
	  yy_switch_to_buffer( TheBufferState);
	}
	void finishEscape2Terminfo()
	{
	  yy_delete_buffer( TheBufferState);
	}

	void getEquivalentStyle( style* theStyle, unsigned int* theBackground, unsigned int* theForeground)
	{
	  if (theStyle->isReverse)
	  {
	    *theBackground = theStyle->myForegroundColor;
	    *theForeground = theStyle->myBackgroundColor;
	  }
	  else
	  {
	    *theBackground = theStyle->myBackgroundColor;
	    *theForeground = theStyle->myForegroundColor;
	  }

	  if (theStyle->isDim)
	  {
	    *theForeground = 0x08; /* Grey */
	  }

	  if (theStyle->isUnderline)
	  {
	    *theForeground = 0x0F; /* Bold white */
	  }
	}

	int equivalentStyle( style* theStyle1, style* theStyle2)
	{
	style *s[2];
	unsigned int aBackground[2];
	unsigned int aForeground[2];
	int i;

	ENTER("equivalentStyle");

	s[0]=theStyle1;
	s[1]=theStyle2;

	for (i=0; i<2; i++)
	{
	  getEquivalentStyle( s[i], aBackground+i, aForeground+i);
	}
	i = ((aForeground[0]==aForeground[1])
	       && (aBackground[0]==aBackground[1])
	       && (s[0]->isBold==s[1]->isBold)
	       && (s[0]->isBlink==s[1]->isBlink));

	SHOW2("result=%d\n",i);

	return i;
	}
