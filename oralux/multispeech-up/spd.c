/* 
----------------------------------------------------------------------------
spd.c
$Id: spd.c,v 1.1 2006/04/17 09:11:43 gcasse Exp $
$Author: gcasse $
Description: Multipeech calls from a Speech-Dispatcher API.
$Date: 2006/04/17 09:11:43 $ |
$Revision: 1.1 $ |
Copyright (C) 2006 Gilles Casse (gcasse@oralux.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
----------------------------------------------------------------------------
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <libspeechd.h>
#include <ctype.h>
#include "multispeechd.h"

SPDConnection* spd_open(const char* client_name, const char* connection_name, const char *user_name, SPDConnectionMode mode)
{
    ENTER("spd_open");
    int fd=0;
    int flags;
    struct stat aFileInfo;
    SPDConnection* aConnection = (SPDConnection*)malloc(sizeof(SPDConnection));
    aConnection->stream = NULL;

    if (stat( myFifoInName, &aFileInfo) == -1)
      {
        perror("stat");
        goto endConnect;
      }

    fd = open( myFifoInName, O_RDWR);
    if (fd == -1)
      {
        perror("open");
        goto endConnect;
      }

    aConnection->stream = fdopen(fd, "w");

    if (aConnection->stream)
    {
      setvbuf(aConnection->stream, NULL, _IONBF, 0);
    }

    {
      FILE* fp_char = fopen ("/proc/speakup/characters", "w");
      if (fp_char)
	{
	  int i=0;
	    
	  fprintf(fp_char,"32 space\n");

	  for (i=33;i<256;i++)
	    {
	      fprintf(fp_char,"%d %c\n", i, i );
	    }
	  fclose(fp_char);
	}
      
      fp_char = fopen ("/proc/speakup/chartab", "w");
      if (fp_char)
	{
	  int i=0;
	  for (i='A'; i<='Z'; i++)
	    {
	      fprintf(fp_char,"%d\tALPHA\n", i);
	    }
	  for (i=128; i<256; i++)
	    {
	      fprintf(fp_char,"%d\tALPHA\n", i);
	    }
	  fclose(fp_char);
	}
    }
 
endConnect:
    return aConnection;
}

void spd_close(SPDConnection* connection)
{
  ENTER("spd_close");
  if(connection)
    {
      fclose(connection->stream);
      free(connection);
    }
}

static char* my_symbol[]=
  {
    "control at",
    "control a",
    "control b",
    "control c",
    "control d",
    "control e",
    "control f",
    "control g",
    "control h",
    "tab",
    "newline",
    "control k",
    "control l",
    "control m",
    "control n",
    "control o",
    "control p",
    "control q",
    "control r",
    "control s",
    "control t",
    "control u",
    "control v",
    "control w",
    "control x",
    "control y",
    "control z",
    "escape",
    "control[*]backslash",
    "control[*]right bracket",
    "control[*]caret",
    "control[*]underscore",
    "space",
    "exclamation",
    "quotes",
    "pound",
    "dollar",
    "percent",
    "ampersand",
    "apostrophe",
    "left[*]paren",
    "right[*]paren",
    "star",
    "plus",
    "comma",
    "dash",
    "dot",
    "slash",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "colon",
    "semi",
    "less[*]than",
    "equals",
    "greater[*]than",
    "question[*]mark",
    "at",
    "cap[*]a",
    "cap[*]b",
    "cap[*]c",
    "cap[*]d",
    "cap[*]e",
    "cap[*]f",
    "cap[*]g",
    "cap[*]h",
    "cap[*]i",
    "cap[*]j",
    "cap[*]k",
    "cap[*]l",
    "cap[*]m",
    "cap[*]m",
    "cap[*]o",
    "cap[*]p",
    "cap[*]q",
    "cap[*]r",
    "cap[*]s",
    "cap[*]t",
    "cap[*]u",
    "cap[*]v",
    "cap[*]w",
    "cap[*]x",
    "cap[*]y",
    "cap[*]z",
    "left[*]bracket",
    "backslash",
    "right[*]bracket",
    "caret",
    "underscore",
    "backquote",
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    "r",
    "s",
    "t",
    "u",
    "v",
    "w",
    "x",
    "y",
    "z",
    "left[*]brace",
    "pipe",
    "right[*]brace ",
    "tilde",
    "backspace",
  };


/* punclevel command: "punclevel x" */
#define PUNCLEVEL "punclevel x "
#define PUNCLEVEL_DIGIT 10
#define PUNCLEVEL_SIZE (sizeof(PUNCLEVEL)-1)

int spd_say(SPDConnection* connection, SPDPriority priority, const char* text)
{
  ENTER("spd_say");
  int aNotEmptyString=0;
  int aBlankHeader=strspn(text," ");
  int aBlankTrailer=0;
  int aLength=strlen(text);
  char *pt;
  
  if (aLength && (aBlankHeader != aLength))
    {
      int i=aLength-1;
      while(i && (text[i] == 0x20))
	{
	  i--;
	  aBlankTrailer++;
	}

      if (aBlankHeader+aBlankTrailer+1==aLength)
	{
	  unsigned char c = text[aBlankHeader];
	  if ((c < 128) && my_symbol[c])
	    {
	      fprintf (connection->stream, "tts_say {%s}\n", my_symbol[c]);
	    }
	  else
	    {
	      fprintf (connection->stream, "l { %c }\n", c);
	    }
	}
      else if ((aLength == PUNCLEVEL_SIZE) 
	       && (strncmp(text, PUNCLEVEL, PUNCLEVEL_DIGIT) == 0))
	{
	  SPDPunctuation type;
	  switch(text[ PUNCLEVEL_DIGIT]-'0')
	    {
	    case 0:
	      type=SPD_PUNCT_ALL;
	      break;
	    case 1:
	    case 2:
	      type=SPD_PUNCT_SOME;
	      break;
	    default:
	      type=SPD_PUNCT_NONE;
	      break;
	    }
	  spd_set_punctuation(connection, type);
	}
      else
	{
	  fprintf (connection->stream, "q {%s}\nd\n", text);
	}
    } 
  return 0;
}

int spd_set_punctuation(SPDConnection* connection, SPDPunctuation type)
{
  char* text=NULL;

  ENTER("spd_set_punctuation");

  switch( type)
    {
    case SPD_PUNCT_ALL:
      text="all";
      break;

    case SPD_PUNCT_NONE:
      text="none";
      break;

    default:
      text="some";
      break;
    }

  fprintf (connection->stream, "tts_set_punctuations %s\n", text);
  fprintf (connection->stream, "q {set punctuation to %s}\nd\n", text);

  return 0;
}

int spd_set_voice_type(SPDConnection* connection, SPDVoiceType type)
{
  ENTER("spd_set_voice_type");
  return 0;
}

int spd_set_voice_pitch(SPDConnection* connection, signed int pitch)
{
  ENTER("spd_set_voice_pitch");
  return 0;
}

int spd_set_voice_rate(SPDConnection* connection, signed int rate)
{
  ENTER("spd_set_voice_rate");
  int a_new_rate=(rate+125)*2;
  fprintf (connection->stream, "tts_set_speech_rate %d\n",a_new_rate);
  return 0;
}

int spd_cancel(SPDConnection* connection)
{
  ENTER("spd_cancel");
  fprintf (connection->stream, "s\n");
  return 0;
}

int spd_set_notification_on(SPDConnection* connection, SPDNotification notification)
{
  ENTER("spd_set_notification_on");
  return 0;
}


/* > */
