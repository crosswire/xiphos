/***************************************************************************
                          gbfhtml.cpp  -  description
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
#include "sw_gbfhtml.h"

SW_GBFHTML::SW_GBFHTML()
{
}


char SW_GBFHTML::ProcessText(char *text, int maxlen, const SWKey *key)
{
	char *to, *from, token[2048];
	int tokpos = 0;
	bool intoken 	= false;
	bool hasFootnotePreTag = false;
	bool isRightJustified = false;
	bool isCentered = false;
	int len;

	len = strlen(text) + 1;		// shift string to right of buffer
	if (len < maxlen) {
		memmove(&text[maxlen - len], text, len);
		from = &text[maxlen - len];
	}
	else
		from = text;			// -------------------------------
	
	for (to = text; *from; from++)
	{
		if (*from == '\n') {
			*from = ' ';
		}			
		if (*from == '<') {
			intoken = true;
			tokpos = 0;
			memset(token, 0, 2048);
			continue;
		}
		if (*from == '>') {
			unsigned int i;
			intoken = false;
			// process desired tokens
			switch (*token) {
				case 'W':	// Strongs
					switch(token[1]){
						case 'G':               // Greek
						case 'H':               // Hebrew
							strcpy(to," <small><em>&lt;<a href=\"#");
							to += strlen(to);					
							for (i = 1; i < strlen(token); i++)
								*to++ = token[i];
							strcpy(to,"\">");
							to += strlen(to);
							for (i = 2; i < strlen(token); i++)								
								if(isdigit(token[i])) *to++ = token[i];
							strcpy(to,"</a>&gt;</em></small> ");
							to += strlen(to);
							continue;
						case 'T':               // Tense	-- morphological tags		
							switch (token[2]) {
									case 'G':
									case 'H':
										strcpy(to," <small><em>(<a href=\"#");
										to += strlen(to);					
										for (i = 1;	i < strlen(token); i++)
											*to++ = token[i];
										strcpy(to," \">");
										to += strlen(to);
										for (i = 2;	i < strlen(token); i++)
											if(isdigit(token[i])) *to++ = token[i];
										strcpy(to,"</a>)</em></small> ");
										to += strlen(to);              
										break;
									default: //-- morphological tags
										strcpy(to," <small><em>(");	
										to += strlen(to);
										for (i=2; i < strlen(token); i++) 
							       				*to++ = token[i];
										strcpy(to,")</em></small> ");
										to += strlen(to);  
							}
						continue; 	
					}										
					break;
				case 'R':
					switch(token[1])
					{
					  case 'B':								//word(s) explained in footnote
							*to++ = '<';
							*to++ = 'I';					
							*to++ = '>';						
							hasFootnotePreTag = true; //we have the RB tag
							continue;
						case 'F':               // footnote begin
							if (hasFootnotePreTag) {
								*to++ = '<';
								*to++ = '/';
								*to++ = 'I';
								*to++ = '>';						
								*to++ = ' ';
							}
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
							*to++ = '\"';
							*to++ = '#';
							*to++ = '8';
							*to++ = '0';
							*to++ = '0';
							*to++ = '0';
							*to++ = '0';
							*to++ = '0';
							*to++ = '\"';
							*to++ = '>';
							
							*to++ = ' ';
							*to++ = '<';
							*to++ = 'S';
							*to++ = 'M';
							*to++ = 'A';
							*to++ = 'L';
							*to++ = 'L';
							*to++ = '>';
							*to++ = '(';
													
							continue;
						case 'f':               // footnote end
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
							*to++ = '<';
							*to++ = '/';
							*to++ = 'F';
							*to++ = 'O';
							*to++ = 'N';
							*to++ = 'T';
							*to++ = '>';
							hasFootnotePreTag = false;
							continue;
					}
					break;
				
				case 'F':			// font tags
					switch(token[1])
					{
						case 'I':		// italic start
							*to++ = '<';
							*to++ = 'I';
							*to++ = '>';
							continue;
						case 'i':		// italic end
							*to++ = '<';
							*to++ = '/';
							*to++ = 'I';
							*to++ = '>';
							continue;
						case 'B':		// bold start
							*to++ = '<';
							*to++ = 'B';
							*to++ = '>';
							continue;
						case 'b':		// bold end
							*to++ = '<';
							*to++ = '/';
							*to++ = 'B';
							*to++ = '>';
							continue;
						case 'R':		// words of Jesus begin
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
							*to++ = 'F';
							*to++ = 'F';
							*to++ = '0';
							*to++ = '0';
							*to++ = '0';
							*to++ = '0';
							*to++ = '>';
							continue;
						case 'r':		// words of Jesus end
							*to++ = '<';
							*to++ = '/';
							*to++ = 'F';
							*to++ = 'O';
							*to++ = 'N';
							*to++ = 'T';
							*to++ = '>';
							continue;
						case 'U':		// Underline start
							*to++ = '<';
							*to++ = 'U';
							*to++ = '>';
							continue;
							case 'u':		// Underline end
							*to++ = '<';
							*to++ = '/';
							*to++ = 'U';
							*to++ = '>';
							continue;
						case 'O':		// Old Testament quote begin
							*to++ = '<';
							*to++ = 'C';
							*to++ = 'I';
							*to++ = 'T';
							*to++ = 'E';
							*to++ = '>';
							continue;
						case 'o':		// Old Testament quote end
							*to++ = '<';
							*to++ = '/';
							*to++ = 'C';
							*to++ = 'I';
							*to++ = 'T';
							*to++ = 'E';
							*to++ = '>';
							continue;
						case 'S':		// Superscript begin
							*to++ = '<';
							*to++ = 'S';
							*to++ = 'U';
							*to++ = 'P';
							*to++ = '>';
							continue;
						case 's':		// Superscript end
							*to++ = '<';
							*to++ = '/';
							*to++ = 'S';
							*to++ = 'U';
							*to++ = 'P';
							*to++ = '>';
							continue;
						case 'V':		// Subscript begin
							*to++ = '<';
							*to++ = 'S';
							*to++ = 'U';
	  						*to++ = 'B';
							*to++ = '>';
							continue;
						case 'v':		// Subscript end
							*to++ = '<';
							*to++ = '/';
							*to++ = 'S';
							*to++ = 'U';
							*to++ = 'B';
							*to++ = '>';
							continue;
					        case 'N':
						        *to++ = '<';
							*to++ = 'f';
							*to++ = 'o';
							*to++ = 'n';
							*to++ = 't';
							*to++ = ' ';
							*to++ = 'f';
							*to++ = 'a';
							*to++ = 'c';
							*to++ = 'e';
							*to++ = '=';
							*to++ = '"';
						        for (i = 2; i < strlen(token); i++)
								*to++ = token[i];
							*to++ = '"';
							*to++ = '>';
							continue;
					        case 'n':
						        *to++ = '<';
							*to++ = '/';
							*to++ = 'f';
							*to++ = 'o';
							*to++ = 'n';
							*to++ = 't';
							*to++ = '>';
							continue;
					}
					break;
				case 'C':			// special character tags
					switch(token[1])
					{
						case 'A':               // ASCII value
							*to++ = (char)atoi(&token[2]);
							continue;
						case 'G':
							//*to++ = ' ';
							continue;
						case 'L':               // line break
							*to++ = '<';
							*to++ = 'B';
							*to++ = 'R';
							*to++ = '>';
							*to++ = ' ';
							continue;
						case 'M':               // new paragraph
							*to++ = '<';
							*to++ = 'B';
							*to++ = 'R';
							*to++ = '>';
							continue;
						case 'T':
							//*to++ = ' ';
							continue;
					}
					break;
				case 'J':	//Justification
					switch(token[1]) 
					{
						case 'R':	//right
							*to++ = '<';
							*to++ = 'D';
							*to++ = 'I';
							*to++ = 'V';
							*to++ = ' ';
							*to++ = 'A';
							*to++ = 'L';
							*to++ = 'I';
							*to++ = 'G';
							*to++ = 'N';
							*to++ = '=';
							*to++ = '\"';
							*to++ = 'R';
							*to++ = 'I';
							*to++ = 'G';
							*to++ = 'H';
							*to++ = 'T';
							*to++ = '\"';
							*to++ = '>';
							isRightJustified = true;
							continue;
	
						case 'C':	//center
							*to++ = '<';
							*to++ = 'D';
							*to++ = 'I';
							*to++ = 'V';
							*to++ = ' ';
							*to++ = 'A';
							*to++ = 'L';
							*to++ = 'I';
							*to++ = 'G';
							*to++ = 'N';
							*to++ = '=';
							*to++ = '\"';
							*to++ = 'C';
							*to++ = 'E';
							*to++ = 'N';
							*to++ = 'T';
							*to++ = 'E';
							*to++ = 'R';
							*to++ = '\"';
							*to++ = '>';
							isCentered = true;
							continue;
	
						case 'L': //left, reset right and center
							if (isCentered) {
								*to++ = '<';
								*to++ = '/';
								*to++ = 'C';
								*to++ = 'E';
								*to++ = 'N';
								*to++ = 'T';
								*to++ = 'E';
								*to++ = 'R';
								*to++ = '>';
								isCentered = false;
							}
							if (isRightJustified) {
								*to++ = '<';
								*to++ = '/';
								*to++ = 'D';
								*to++ = 'I';
								*to++ = 'V';
								*to++ = '>';
								isRightJustified = false;
							}
							continue;
					}
					break;
				case 'T':			// title formatting
					switch(token[1])
					{
						case 'T':               // Book title begin
							*to++ = '<';
							*to++ = 'b';
							*to++ = 'i';
							*to++ = 'g';
							*to++ = '>';
							continue;
						case 't':
							*to++ = '<';
							*to++ = '/';
							*to++ = 'b';
							*to++ = 'i';
							*to++ = 'g';
							*to++ = '>';
							continue;/*
                    				case 'S':
						        *to++ = '<';
						        *to++ = 'b';
						        *to++ = 'r';
						        *to++ = ' ';
						        *to++ = '/';
						        *to++ = '>';
						        *to++ = '<';
						        *to++ = 'b';
						        *to++ = 'i';
							*to++ = 'g';
						        *to++ = '>';
							continue;
                    				case 's':
						        *to++ = '<';
						        *to++ = '/';
						        *to++ = 'b';
						        *to++ = 'i';
						        *to++ = 'g';
						        *to++ = '>';
						        *to++ = '<';
						        *to++ = 'b';
						        *to++ = 'r';
						        *to++ = ' ';
						        *to++ = '/';
						        *to++ = '>';
							continue;*/
					}
					break;
	
				case 'P': // special formatting
					switch(token[1])
					{
						case 'P': // Poetry begin
							*to++ = '<';
							*to++ = 'C';
							*to++ = 'I';
							*to++ = 'T';
							*to++ = 'E';
							*to++ = '>';
							continue;
						case 'p':
							*to++ = '<';
							*to++ = '/';
							*to++ = 'C';
							*to++ = 'I';
							*to++ = 'T';
							*to++ = 'E';
							*to++ = '>';
							continue;
					}
					break;
			}
			continue;
		}
		if (intoken) {
		 	if (tokpos < 2047) {
		 		token[tokpos] = *from;
		 		tokpos++;
		 	}
		 }
		else
			*to++ = *from;
	}
	*to = 0;
	return 0;
}
