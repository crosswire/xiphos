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


char GS_RWPHTML::ProcessText(char *text, int maxlen, const SWKey *key)
{
	char *to, *from;
	bool inverse = false;
	bool first_letter = false;
	int len;

	len = strlen(text) + 1;	// shift string to right of buffer
	if (len < maxlen) {
		memmove(&text[maxlen - len], text, len);
		from = &text[maxlen - len];
	} else	from = text;	// -------------------------------
	for (to = text; *from; from++) {
		if (*from == '\\')
		{
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
				while(*from != '\\')
				{
					
					if((first_letter) && (*from == 'h')){
						if ( from[1] == 'o') {
							++from;
							*to++ = *from;
							++from;
							first_letter = false;
							continue;
						} else if ( from[1] == 'a') {
							++from;
							*to++ = *from;
							++from;
							first_letter = false;
							continue;
						} else if ( from[1] == 'w') {
							++from;
							*to++ = *from;
							++from;
							first_letter = false;
							continue;
						} else if ( from[1] == 'u') {
							++from;
							*to++ = *from;
							++from;
							first_letter = false;
							continue;
						} else if ( from[1] == -120) {
							++from;
							*to++  = 'h';
							++from;
							first_letter = false;
							continue;
						} else if ( from[1] == 'i') {
							++from;
							*to++ = *from;
							++from;
							first_letter = false;
							continue;
						}
						first_letter = false;
					}
					if((*from == 't') && (from[1] == 'h')) {
						*to++ = 'q';
						++from;
						++from;
						continue;
					}
					if((*from == 'c') && (from[1] == 'h')) {
						*to++ = 'c';
						++from;
						++from;
						continue;
					}			
					if((*from == 'p') && (from[1] == 'h')) {
						*to++ = 'f';
						++from;
						++from;
						continue; 
					}			
					if(*from == 39) ++from;
					if (*from == 's') {	
						if (from[1] == ' '
				    		    || from[1] == ')'
				    		    || from[1] == '\\'){
					 		*to++ = 'V';
						} else *to++ = 's';
						++from;
						continue;						
					}	
					if (*from == -120) {					
						++from;					
					 	*to++ = 'h';						
						continue; 
					}		
					if (*from == -125) {
						++from;
						*to++ = 'a';
						continue;
					}
					if (*from == -109) {
						++from;
						*to++ = 'w';
						continue; 
					}
					if(*from == '\\') continue;
					if(*from == ' ') first_letter = true;
					*to++ = *from;
					from++;					
				}
				*to++ = ' ';
				*to++ = '<';
				*to++ = '/';
				*to++ = 'F';
				*to++ = 'O';
				*to++ = 'N';
				*to++ = 'T';
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

		if (*from == '}')
		{
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


