/******************************************************************************
 *
 * Latin1UTF8 -	SWFilter decendant to convert a Latin-1 character to UTF-8
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include "sw_latin1utf8.h"

SW_Latin1UTF8::SW_Latin1UTF8() {
}


char SW_Latin1UTF8::ProcessText(char *text, int maxlen, const SWKey *key)
{
    unsigned char *to, *from;
	int len;
	
	len = strlen(text) + 1;						// shift string to right of buffer
	if (len < maxlen) {
	  memmove(&text[maxlen - len], text, len);
	  from = (unsigned char*)&text[maxlen - len];
	}
	else	
	  from = (unsigned char*)text;							// -------------------------------

	for (to = (unsigned char*)text; *from; from++) {
	  if (*from < 0x80) {
	    *to++ = *from;
	  }
	  else if (*from < 0xc0) {
	    *to++ = 0xC2;
	    *to++ = *from;
	  }
	  else {
	    *to++ = 0xC3;
	    *to++ = (*from - 0x40);
	  }
	}
	*to++ = 0;
	*to = 0;
	return 0;
}
