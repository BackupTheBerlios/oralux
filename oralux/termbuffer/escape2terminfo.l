	#include "escape2terminfo.h"
	#include "linuxconsole.h"

	int myDefaultForegroundColor;
	int myDefaultBackgroundColor;
	int myParameters[MAXPARAM];
	int myNumberOfParameters;

ESC	\x1b
H	\x08
J	\x0a
M	\x0d
DIGIT	[0-9]

%%
{ESC}\[{DIGIT}+(;{DIGIT}+)*m	{/* Change mode (affect also the bg, fg colors) */
				getStyle( (struct t_style*)myParameters);
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
			myParameters[0]=1;
			return DL1;
			}
{ESC}\[L		return IL1;
{ESC}\[H{ESC}\[2?J	return CLEAR;
{ESC}\[J		return ED;
{ESC}\[P		{
			myParameters[0]=1;
			return DCH1;
			}
{ESC}\[[12]?K		{
			if (yyleng==3)
			{
			  myParameters[0]=0;
			}
			else
			{
			  myParameters[0]=yytext[2]-'0';
			}
			return EL;
			}
{ESC}\[H		return HOME;
{ESC}\[{DIGIT}+L	{ 
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			return IL;
			}
{ESC}\[{DIGIT}+G	{
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			return HPA;
			}
{ESC}\[{DIGIT}+d	{ /* Line number */
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			
			return VPA;
			}
{ESC}\[{DIGIT}+P	{
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			return DCH;
			}
{ESC}\[{DIGIT}+M	{
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			return DL;
			}
{ESC}\[{DIGIT}*X	{
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			if (myParameters[0]==0)
			{
			  myParameters[0]=1;
			}
			return ECH;
			}
{ESC}\[{DIGIT}+A	{
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			return CUU;
			}
{ESC}\[{DIGIT}+;{DIGIT}+H	{
				int i=0;
				for (i=2;i<yyleng;i++)
				{
				  if (yytext[i]==';')
				  {
				    myParameters[0]=getDigit(yytext+2,i-2);
				    break;
				  }
				}
				myParameters[1]=getDigit(yytext+i+1, yyleng-1-i-1);	
				return CUP;
				}
.			return TEXTFIELD;

%%
