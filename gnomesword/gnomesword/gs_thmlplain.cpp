/******************************************************************************
 *
 * thmlplain -	SWFilter decendant to strip out all ThML tags or convert to
 *		ASCII rendered symbols.
 */


#include <stdlib.h>
#include <string.h>
#include "gs_thmlplain.h"


GS_ThMLPlain::GS_ThMLPlain() {
}


char GS_ThMLPlain::ProcessText(char *text, int maxlen)
{
  char *to, *from, token[150], tokpos = 0;
  bool intoken = false;
  int len;

  len = strlen(text) + 1;						// shift string to right of buffer
  if (len < maxlen) {
    memmove(&text[maxlen - len], text, len);
    from = &text[maxlen - len];
  }
  else	from = text;							// -------------------------------
	
  for (to = text; *from; from++) {
    if (*from == '<') {
      intoken = true;
      tokpos = 0;
      memset(token, 0, 150);
      continue;
    }
    if (*from == '>') {
      intoken = false;
      // process desired tokens

      if (!strncmp("note", token, 4)) {
	*to++ = ' ';
	*to++ = '(';
      }
      else if (!strncmp("br", token, 2))
	*to++ = '\n';
      else if (!strncmp("/p", token, 2))
	*to++ = '\n';
      else if (!strncmp("/note", token, 5)) {
	*to++ = ')';
	*to++ = ' ';
      }
      continue;
    }
    if (intoken)
      token[tokpos++] = *from;
    else	*to++ = *from;
  }
  *to = 0;
  return 0;
}
