/***************************************************************************
                     thmlhtml.cpp  -  ThML to HTML filter
                             -------------------
    begin                : 1999-10-27
    copyright            : 1999 by Chris Little
    email                : chrislit@chiasma.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "gs_thmlhtml.h"


GS_ThMLHTML::GS_ThMLHTML()
{
}


char GS_ThMLHTML::ProcessText(char *text, int maxlen)
{
  char *to, *from, token[150], tokpos = 0;
  bool intoken 	= false;
  int len;

  len = strlen(text) + 1;						// shift string to right of buffer
  if (len < maxlen) {
    memmove(&text[maxlen - len], text, len);
    from = &text[maxlen - len];
  }
  else	from = text;							// -------------------------------
  for (to = text; *from; from++)
    {
      if (*from == '<') {
	intoken = true;
	tokpos = 0;
	memset(token, 0, 150);
	continue;
      }
      if (*from == '>')	{
	intoken = false;
	// process desired tokens
	if (!strncmp(token, "sync type=\"Strongs\" value=\"G", 28) || \
	    !strncmp(token, "sync type=\"Strongs\" value=\"H", 28)) {
	  *to++ = '<';
	  *to++ = 'S';
	  *to++ = 'M';
	  *to++ = 'A';
	  *to++ = 'L';
	  *to++ = 'L';
	  *to++ = '>';
	  *to++ = '<';
	  *to++ = 'E';
	  *to++ = 'M';
	  *to++ = '>';
	  for (unsigned int i = 28; token[i] != '\"'; i++)
	    *to++ = token[i];
	  *to++ = '<';
	  *to++ = '/';
	  *to++ = 'E';
	  *to++ = 'M';
	  *to++ = '>';
	  *to++ = '<';
	  *to++ = '/';
	  *to++ = 'S';
	  *to++ = 'M';
	  *to++ = 'A';
	  *to++ = 'L';
	  *to++ = 'L';
	  *to++ = '>';
	}
	else if (!strncmp(token, "scripRef version", 16)) {
	  *to++ = '<';
	  *to++ = 'A';
	  *to++ = ' ';
	  *to++ = 'H';
	  *to++ = 'R';
	  *to++ = 'E';	
	  *to++ = 'F';	
	  *to++ = '=';
	  *to++ = '\"';	
	  for (unsigned int i = 32; token[i] != '\"'; i++)
	    *to++ = token[i];
	  *to++ = '\"';
	  *to++ = '>';
  	} 
	else if (!strncmp(token, "/scripRef", 9)) { 
	  *to++ = ' ';
	  *to++ = '<';
	  *to++ = '/';
	  *to++ = 'A';
	  *to++ = '>';	  
	}	
	else if (!strncmp(token, "sync type=\"Strongs\" value=\"T", 28)) {
	  *to++ = '<';
	  *to++ = 'S';
	  *to++ = 'M';
	  *to++ = 'A';
	  *to++ = 'L';
	  *to++ = 'L';
	  *to++ = '>';
	  *to++ = '<';
	  *to++ = 'I';
	  *to++ = '>';
	  for (unsigned int i = 29; token[i] != '\"'; i++)
	    *to++ = token[i];
	  *to++ = '<';
	  *to++ = '/';
	  *to++ = 'I';
	  *to++ = '>';
	  *to++ = '<';
	  *to++ = '/';
	  *to++ = 'S';
	  *to++ = 'M';
	  *to++ = 'A';
	  *to++ = 'L';
	  *to++ = 'L';
	  *to++ = '>';
	}
	else if (!strncmp(token, "note place=\"foot\"", 17)) {
	  *to++ = ' ';
	  *to++ = '<';
	  *to++ = 'S';
	  *to++ = 'M';
	  *to++ = 'A';
	  *to++ = 'L';
	  *to++ = 'L';
	  *to++ = '>';
	  *to++ = '(';
	}
	else if (!strncmp(token, "/note", 5)) {
	  *to++ = ')';
	  *to++ = '<';
	  *to++ = '/';
	  *to++ = 'S';
	  *to++ = 'M';
	  *to++ = 'A';
	  *to++ = 'L';
	  *to++ = 'L';
	  *to++ = '>';
	  *to++ = ' ';
	}
	else if (!strncmp(token, "foreign lang=\"el\"", 17)) {
	  *to++ = '<';
	  *to++ = 'F';
	  *to++ = 'O';
	  *to++ = 'N';
	  *to++ = 'T';
	  *to++ = ' ';
	  *to++ = 'F';
	  *to++ = 'A';
	  *to++ = 'C';
	  *to++ = 'E';
	  *to++ = '=';
	  *to++ = '\"';
	  *to++ = 'S';
	  *to++ = 'I';
	  *to++ = 'L';
	  *to++ = ' ';
	  *to++ = 'G';
	  *to++ = 'a';
	  *to++ = 'l';
	  *to++ = 'a';
	  *to++ = 't';
	  *to++ = 'i';
	  *to++ = 'a';
	  *to++ = '\"';
	  *to++ = '>';
	}
	else if (!strncmp(token, "foreign lang=\"he\"", 17)) {
	  *to++ = 'F';
	  *to++ = 'O';
	  *to++ = 'N';
	  *to++ = 'T';
	  *to++ = ' ';
	  *to++ = 'F';
	  *to++ = 'A';
	  *to++ = 'C';
	  *to++ = 'E';
	  *to++ = '=';
	  *to++ = '\"';
	  *to++ = 'S';
	  *to++ = 'I';
	  *to++ = 'L';
	  *to++ = ' ';
	  *to++ = 'E';
	  *to++ = 'z';
	  *to++ = 'r';
	  *to++ = 'a';
	  *to++ = '\"';
	  *to++ = '>';
	}
	else if (!strncmp(token, "/foreign", 8)) {
	  *to++ = '<';
	  *to++ = '/';
	  *to++ = 'F';
	  *to++ = 'O';
	  *to++ = 'N';
	  *to++ = 'T';
	  *to++ = '>';
	}
	else {
	  *to++ = '<';
	  for (unsigned int i = 0; i < strlen(token); i++)
	    *to++ = token[i];
	  *to++ = '>';
	}
        continue;
      }
      if (intoken)
	token[tokpos++] = *from;
      else
	*to++ = *from;
    }
  *to = 0;
  return 0;
}



