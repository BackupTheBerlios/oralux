	#include "escape2terminfo.h"
	#include "linuxconsole.h"

	int TheParameter[MAXPARAM];
	int TheNumberOfParameter;
	struct t_style TheDefaultStyle;
	struct t_style TheCurrentStyle;
NUL	\x00
BEL	\x07
H	\x08
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
{M}{J}			return NEL;
{J}			return CUD1;
{M}			return CR;
{H}			return CUB1;
{ESC}8			return RC;
{ESC}7			return SC;
{ESC}\[C		return CUF1;
{ESC}\[H{ESC}\[2?J	return CLEAR;
{ESC}\[[12]?J		{
			TheParameter[0]=(yyleng==3) ? 0 : yytext[2]-'0';
			return ED;
			}
{ESC}\[[12]?K		{
			TheParameter[0]=(yyleng==3) ? 0 : yytext[2]-'0';
			return EL;
			}
{ESC}\[H		return HOME;
{ESC}\[{DIGIT}*@	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return ICH;
			}
{ESC}\[{DIGIT}*L	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return IL;
			}
{ESC}\[{DIGIT}*G	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return HPA;
			}
{ESC}\[{DIGIT}*d	{ /* Line number */
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return VPA;
			}
{ESC}\[{DIGIT}*P	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return DCH;
			}
{ESC}\[{DIGIT}*M	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return DL;
			}
{ESC}\[{DIGIT}*X	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return ECH;
			}
{ESC}\[{DIGIT}*A	{
			TheParameter[0]=(yyleng==3) ? 1 : getDigit(yytext+2, yyleng-1-2);
			return CUU;
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
				return CUP;
				}
.			return TEXTFIELD;

%%
