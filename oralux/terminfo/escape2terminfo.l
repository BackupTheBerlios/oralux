	#include "escape2terminfo.h"
	static int getDigit(char* theBuffer, int theLength)
	{
	int aValue=0;
	int i;
	for (i=0; i<theLength; i++)
	{
	  aValue = aValue*10 + theBuffer[i]-'0';
	}
	return aValue;
	}
	int myParameters[MAXPARAM];
	int myNumberOfParameters=0;

ESC	\x1b
H	\x08
J	\x0a
DIGIT	[0-9]



%%
{ESC}\[([01];)?3{DIGIT}m	{/* ESC [0;34m */
				myParameters[0]=yytext[yyleng-2]-'0';
				return SETF;
				}
{ESC}\[([01];)?4{DIGIT}m	{/* ESC [0;44m */
				myParameters[0]=yytext[yyleng-2]-'0';
				return SETB;
				}
{J}			return CUD1;
{H}			return CUB1;
{ESC}8			return RC;
{ESC}7			return SC;
{ESC}\[C		return CUF1;
{ESC}\[M		return DL1;
{ESC}\[L		return IL1;
{ESC}\[H{ESC}\[2?J	return CLEAR;
{ESC}\[J		return ED;
{ESC}\[P		return DCH1;
{ESC}\[1K		return EL1;
{ESC}\[H		return HOME;
{ESC}\[{DIGIT}+L	{ 
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			return IL;
			}
{ESC}\[{DIGIT}+G	{
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
			return HPA;
			}
{ESC}\[{DIGIT}+d	{
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
{ESC}\[{DIGIT}+X	{
			myParameters[0]=getDigit(yytext+2, yyleng-1-2);
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
				  myParameters[0]=getDigit(yytext+2, i-2);
				}
				myParameters[1]=getDigit(yytext+i+1, yyleng-1-i-1);	
				return CUP;
				}

{ESC}\[0;10(;7)?(;4)?(;7)?(;5)?(;2)?(;1)?(;8)?(;11)?(;)?m	{
								//myNumberOfParameters=1;
								return SGR;
								}
.			return TEXTFIELD;

%%
