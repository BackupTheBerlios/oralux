	#include "escape2terminfo.h"
	#include "linuxconsole.h"

	int TheParameter[MAXPARAM];
	int TheNumberOfParameter;
	struct t_style TheDefaultStyle;
	struct t_style TheCurrentStyle;

ESC	\x1b
H	\x08
J	\x0a
M	\x0d
DIGIT	[0-9]
ALT	[\x80-\xff]

%x graphics
%s alternate
%%

{ESC}\(0		BEGIN(graphics);
<graphics>{ESC}{ESC}\(B	{
			unput(0x1b); /* ESC #1: new future sequence, ESC #2 to return to iso 8859-1 mapping */
			BEGIN(INITIAL); 
			}
<graphics>{ESC}\(B	BEGIN(INITIAL); /* return to iso 8859-1 mapping */
<graphics>.		/* remove any character in graphics mapping */

<alternate>{ESC}\[10m	BEGIN(INITIAL);
<alternate>{ALT}	/* remove a (guessed) graphical character */
			/* <alternate>.		/ * return a non-graphical character */

{ESC}\[11m		BEGIN(alternate);

{ESC}\[{DIGIT}+(;{DIGIT}+)*m	{/* Change mode (affect also the bg, fg colors) */
				getStyle( &TheCurrentStyle, &TheDefaultStyle);
				return SGR; /* Group also SETF, SETB, OP,...*/
				}
{M}{J}			return NEL;
{J}			return CUD1;
{M}			return CR;
{H}			return CUB1;
{ESC}8			return RC;
{ESC}7			return SC;
{ESC}\[C		return CUF1;
{ESC}\[M		{
			TheParameter[0]=1;
			return DL1;
			}
{ESC}\[L		return IL1;
{ESC}\[H{ESC}\[2?J	return CLEAR;
{ESC}\[[12]?J		{
			if (yyleng==3)
			{
			  TheParameter[0]=0;
			}
			else
			{
			  TheParameter[0]=yytext[2]-'0';
			}
			return ED;
			}
{ESC}\[P		{
			TheParameter[0]=1;
			return DCH1;
			}
{ESC}\[[12]?K		{
			if (yyleng==3)
			{
			  TheParameter[0]=0;
			}
			else
			{
			  TheParameter[0]=yytext[2]-'0';
			}
			return EL;
			}
{ESC}\[H		return HOME;
{ESC}\[{DIGIT}+L	{ 
			TheParameter[0]=getDigit(yytext+2, yyleng-1-2);
			return IL;
			}
{ESC}\[{DIGIT}+G	{
			TheParameter[0]=getDigit(yytext+2, yyleng-1-2);
			return HPA;
			}
{ESC}\[{DIGIT}+d	{ /* Line number */
			TheParameter[0]=getDigit(yytext+2, yyleng-1-2);
			
			return VPA;
			}
{ESC}\[{DIGIT}+P	{
			TheParameter[0]=getDigit(yytext+2, yyleng-1-2);
			return DCH;
			}
{ESC}\[{DIGIT}+M	{
			TheParameter[0]=getDigit(yytext+2, yyleng-1-2);
			return DL;
			}
{ESC}\[{DIGIT}*X	{
			TheParameter[0]=getDigit(yytext+2, yyleng-1-2);
			if (TheParameter[0]==0)
			{
			  TheParameter[0]=1;
			}
			return ECH;
			}
{ESC}\[{DIGIT}+A	{
			TheParameter[0]=getDigit(yytext+2, yyleng-1-2);
			return CUU;
			}
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
