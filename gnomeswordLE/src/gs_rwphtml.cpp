/***************************************************************************
                          rwphtml.cpp  -  description
                             -------------------
    begin                : Thu Jun 24 1999
    copyright            : (C) 1999 by Torsten Uhlmann
    email                : TUhlmann@gmx.de
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
#include "gs_rwphtml.h"


GS_RWPHTML::GS_RWPHTML()
{
}

char GS_RWPHTML::ProcessText(char *text, int maxlen, const SWKey * key)
{
	char *to, *from, greek_str[500];
	bool inverse = false;
	bool first_letter = false;
	int len;

	len = strlen(text) + 1;	// shift string to right of buffer
	if (len < maxlen) {
		memmove(&text[maxlen - len], text, len);
		from = &text[maxlen - len];
	} else
		from = text;	
	for (to = text; *from; from++) {
		if (*from == '\\') {
			++from;
			int i=0;
			first_letter = true;
			greek_str[0] = '\0';			
			while (*from != '\\') { /* get the greek word or phrase */
				greek_str[i++] = *from;
				greek_str[i + 1] = '\0';
				from++;
			} /* convert to symbol font as best we can */
			strcpy(to,"<I> </I><FONT FACE=\"symbol\">");
			to += strlen(to);
			for (int j = 0; j < i; j++) {
				if ((first_letter)
				    && (greek_str[j] == 'h')) {
					if (greek_str[j + 1] == 'o') {
						*to++ = 'o';
						first_letter = false;
						++j;
						continue;
					} else if (greek_str[j + 1] == 'a') {
						*to++ = 'a';
						first_letter = false;
						++j;
						continue;
					} else if (greek_str[j + 1] == 'w') {
						*to++ = 'w';
						first_letter = false;
						++j;
						continue;
					} else if (greek_str[j + 1] == 'u') {
						*to++ = 'u';
						first_letter = false;
						++j;
						continue;
					} else if (greek_str[j + 1] ==
						   -109) {
						*to++ = 'w';
						first_letter = false;
						++j;
						continue;
					} else if (greek_str[j + 1] ==
						   -120) {
						*to++ = 'h';
						first_letter = false;
						++j;
						continue;
					} else if (greek_str[j + 1] == 'i') {
						*to++ = 'i';
						first_letter = false;
						++j;
						continue;
					}
					first_letter = false;
				}
				if ((greek_str[j] == 't')
				    && (greek_str[j + 1] == 'h')) {
					*to++ = 'q';
					++j;
					continue;
				}
				if ((greek_str[j] == 'c')
				    && (greek_str[j + 1] == 'h')) {
					*to++ = 'c';
					++j;
					continue;
				}
				if ((greek_str[j] == 'p')
				    && (greek_str[j + 1] == 'h')) {
				    	++j;
					*to++ = 'f';
					continue;
				}
				if (greek_str[j] == -120) {
					*to++ = 'h';
					continue;
				}
				if (greek_str[j] == -125) {
					*to++ = 'a';
					continue;
				}
				if (greek_str[j] == -109) {
					if(greek_str[j+1] == 'i') ++j;
					*to++ = 'w';
					continue;
				}
				if (greek_str[j] == ' ')
					first_letter = true;
				if (greek_str[j] == 's') {
					if(isalpha(greek_str[j + 1])) *to++ = 's';
					else *to++ = 'V';
					continue;					
				}
				if (greek_str[j] == '\'') {					
					continue;
				}
				*to++ = greek_str[j];
			}
			strcpy(to,"</FONT><I> </I>");
			to += strlen(to);
			continue;
		}
		if ((*from == '#') || (*from == -81)) {	// verse markings (e.g. "#Mark 1:1|")
			inverse = true;
			strcpy(to,"<FONT COLOR=#0000FF>");
			to += strlen(to);			
			continue;
		}
		if ((*from == '|') && (inverse)) {
			inverse = false;
			strcpy(to,"</FONT>");
			to += strlen(to);
			continue;
		}
		if (*from == '{') {
			strcpy(to,"<BR><STRONG>");
			to += strlen(to);
			if ((from - &text[maxlen - len]) > 10) { // not the beginning of the entry
				strcpy(to,"<P>");
				to += strlen(to);
			}
			continue;
		}
		if (*from == '}') {
			strcpy(to," </STRONG>");
			to += strlen(to);
			continue;
		}
		if ((*from == '\n') && (from[1] == '\n')) {
			strcpy(to,"<P>");
			to += strlen(to);
			continue;
		}
		*to++ = *from;
	}
	*to = 0;
	return 0;
}
