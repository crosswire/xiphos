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
#include <gnome.h>
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
	bool greek = false;
	int len;

	len = strlen(text) + 1;	// shift string to right of buffer
	if (len < maxlen) {
		memmove(&text[maxlen - len], text, len);
		from = &text[maxlen - len];
	} else
		from = text;	// -------------------------------
	for (to = text; *from; from++) {
		if (*from == '\\') {
			greek = true;
			++from;
			*to++ = '<';
			*to++ = 'I';
			*to++ = '>';
			*to++ = ' ';
			*to++ = '<';
			*to++ = '/';
			*to++ = 'I';
			*to++ = '>';
			*to++ = ' ';
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
			*to++ = 's';
			*to++ = 'y';
			*to++ = 'm';
			*to++ = 'b';
			*to++ = 'o';
			*to++ = 'l';
			*to++ = '\"';
			*to++ = '>';
			first_letter = true;
			gint i = 0;
			greek_str[0] = '\0';
			while (*from != '\\') {
				greek_str[i++] = *from;
				greek_str[i + 1] = '\0';
				from++;
			}
			//g_warning("str = %s , num = %d\n", greek_str, i);
			for (int j = 0; j < i; j++) {
				if ((first_letter)
				    && (greek_str[j] == 'h')) {
					if (greek_str[j + 1] == 'o') {
						*to++ = greek_str[j];
						first_letter = false;
						continue;
					} else if (greek_str[j + 1] == 'a') {
						*to++ = greek_str[j];
						first_letter = false;
						continue;
					} else if (greek_str[j + 1] == 'w') {
						*to++ = greek_str[j];
						first_letter = false;
						continue;
					} else if (greek_str[j + 1] == 'u') {
						*to++ = greek_str[j];
						first_letter = false;
						continue;
					} else if (greek_str[j + 1] ==
						   -120) {
						*to++ = 'h';
						first_letter = false;
						continue;
					} else if (greek_str[j + 1] == 'i') {
						*to++ = greek_str[j];
						first_letter = false;
						continue;
					}
					first_letter = false;
				}
				if (*from == 39)
					continue;
				if ((greek_str[j] == 't')
				    && (greek_str[j + 1] == 'h')) {
					*to++ = 'q';
					continue;
				}
				if ((greek_str[j] == 'c')
				    && (greek_str[j + 1] == 'h')) {
					*to++ = 'c';
					continue;
				}
				if ((greek_str[j] == 'p')
				    && (greek_str[j + 1] == 'h')) {
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
					*to++ = 'w';
					continue;
				}
				if (greek_str[j] == ' ')
					first_letter = true;
				if (greek_str[j] == 's') {
					switch (greek_str[j + 1]) {
					case '\\':
						*to++ = 'V';
						break;
					case ' ':
					case '.':
					case ',':
					case ')':
						*to++ = 'V';
						break;
					default:
						*to++ = 's';
						break;
					}
					continue;
				}
				*to++ = greek_str[j];
			}
			*to++ = ' ';
			*to++ = '<';
			*to++ = '/';
			*to++ = 'F';
			*to++ = 'O';
			*to++ = 'N';
			*to++ = 'T';
			*to++ = '>';
			*to++ = '<';
			*to++ = 'I';
			*to++ = '>';
			*to++ = ' ';
			*to++ = '<';
			*to++ = '/';
			*to++ = 'I';
			*to++ = '>';
			*to++ = ' ';
			continue;
		}
		if (*from == '#') {	// verse markings (e.g. "#Mark 1:1|")
			inverse = true;
			*to++ = '<';
			*to++ = 'F';
			*to++ = 'O';
			*to++ = 'N';
			*to++ = 'T';
			*to++ = ' ';
			*to++ = 'C';
			*to++ = 'O';
			*to++ = 'L';
			*to++ = 'O';
			*to++ = 'R';
			*to++ = '=';
			*to++ = '#';
			*to++ = '0';
			*to++ = '0';
			*to++ = '0';
			*to++ = '0';
			*to++ = 'F';
			*to++ = 'F';
			*to++ = '>';
			continue;
		}
		if ((*from == '|') && (inverse)) {
			inverse = false;
			*to++ = '<';
			*to++ = '/';
			*to++ = 'F';
			*to++ = 'O';
			*to++ = 'N';
			*to++ = 'T';
			*to++ = '>';
			continue;
		}

		if (*from == '{') {
			*to++ = '<';
			*to++ = 'B';
			*to++ = 'R';
			*to++ = '>';
			*to++ = '<';
			*to++ = 'S';
			*to++ = 'T';
			*to++ = 'R';
			*to++ = 'O';
			*to++ = 'N';
			*to++ = 'G';
			*to++ = '>';
			if ((from - &text[maxlen - len]) > 10) {	// not the beginning of the entry
				*to++ = '<';
				*to++ = 'P';
				*to++ = '>';
			}
			continue;
		}

		if (*from == '}') {
			// this is kinda neat... DO NOTHING
			*to++ = ' ';
			*to++ = '<';
			*to++ = '/';
			*to++ = 'S';
			*to++ = 'T';
			*to++ = 'R';
			*to++ = 'O';
			*to++ = 'N';
			*to++ = 'G';
			*to++ = '>';
			continue;
		}
		if ((*from == '\n') && (from[1] == '\n')) {
			*to++ = '<';
			*to++ = 'P';
			*to++ = '>';
			continue;
		}

		*to++ = *from;
	}
	*to = 0;
	return 0;
}
