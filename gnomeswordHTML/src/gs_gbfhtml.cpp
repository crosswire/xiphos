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
#include <gs_gbfhtml.h>

GS_GBFHTML::GS_GBFHTML()
{
}


char GS_GBFHTML::ProcessText(char *text, int maxlen, const SWKey *key)
{
	char *to, *from, token[2048];
	int tokpos = 0;
	bool intoken 	= false;
	bool hasFootnotePreTag = false;
	bool isRightJustified = false;
	bool isCentered = false;
	int len;
		
	len = strlen(text) + 1;
	if (len < maxlen) {
		memmove(&text[maxlen - len], text, len);
		from = &text[maxlen - len];
	} else
		from = text;
	for (to = text; *from; from++) {
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
			intoken = false;
			// process desired tokens
			switch (*token) {
			case 'W':	// Strongs
				switch (token[1]) {
				case 'G':	// Greek
				case 'H':	// Hebrew
					strcpy(to," <A HREF=\"#");
					to += strlen(to);					
					for (unsigned int i = 2;
					     i < strlen(token); i++)
						*to++ = token[i];
					strcpy(to," \"><FONT SIZE=\"-1\">");
					to += strlen(to);
					for (unsigned int i = 2;
					     i < strlen(token); i++)
						*to++ = token[i];
					strcpy(to,"</FONT></A> ");
					to += strlen(to);
					continue;
				case 'T':	// Tense
					strcpy(to," <A HREF=\"#");
					to += strlen(to);
					for (unsigned int i = 3;
					     i < strlen(token); i++)
						*to++ = token[i];					
					strcpy(to,"\"> <FONT SIZE=\"-1\"><I>");
					to += strlen(to);
					for (unsigned int i = 3;
					     i < strlen(token); i++)
						*to++ = token[i];					
					strcpy(to,"</I></FONT></A> ");
					to += strlen(to);
					continue;
				}
				break;
			case 'R':
				switch (token[1]) {
				case 'B':	//word(s) explained in footnote
					strcpy(to,"<I>");
					to += strlen(to);
					hasFootnotePreTag = true;	//we have the RB tag
					continue;
				case 'F':	// footnote begin
					if (hasFootnotePreTag) {
						strcpy(to,"</I>");
						to += strlen(to);
					}
					strcpy(to,"<FONT COLOR=\"800000\"><SMALL>(");
					to += strlen(to);
					continue;
				case 'f':	// footnote end
					strcpy(to,")</SMALL></FONT>");
					to += strlen(to);
					hasFootnotePreTag = false;
					continue;
				}
				break;
			case 'F':	// font tags
				switch (token[1]) {
				case 'I':	// italic start
					strcpy(to,"<I>");
					to += strlen(to);
					continue;
				case 'i':	// italic end
					strcpy(to,"</I>");
					to += strlen(to);
					continue;
				case 'B':	// bold start
					strcpy(to,"<B>");
					to += strlen(to);
					continue;
				case 'b':	// bold end
					strcpy(to,"</B>");
					to += strlen(to);
					continue;
				case 'R':	// words of Jesus begin
					strcpy(to,"<FONT COLOR=\"FF0000\">");
					to += strlen(to);
					continue;
				case 'r':	// words of Jesus end
					strcpy(to,"</FONT>");
					to += strlen(to);
					continue;
				case 'U':	// Underline start
					strcpy(to,"<U>");
					to += strlen(to);
					continue;
				case 'u':	// Underline end
					strcpy(to,"</U>");
					to += strlen(to);
					continue;
				case 'O':	// Old Testament quote begin
					strcpy(to,"<CITE>");
					to += strlen(to);
					continue;
				case 'o':	// Old Testament quote end
					strcpy(to,"</CITE>");
					to += strlen(to);
					continue;
				case 'S':	// Superscript begin
					strcpy(to,"<SUP>");
					to += strlen(to);
					continue;
				case 's':	// Superscript end
					strcpy(to,"</SUP>");
					to += strlen(to);
					continue;
				case 'V':	// Subscript begin
					strcpy(to,"<SUB>");
					to += strlen(to);				
					continue;
				case 'v':	// Subscript end
					strcpy(to,"</SUB>");
					to += strlen(to);
					continue;
				}
				break;
			case 'C':	// special character tags
				switch (token[1]) {
				case 'A':	// ASCII value
					*to++ = (char) atoi(&token[2]);
					continue;
				case 'G':
					//*to++ = ' ';
					continue;
				case 'L':	// line break
					strcpy(to,"<BR>");
					to += strlen(to);
					continue;
				case 'M':	// new paragraph
					strcpy(to,"<P>");
					to += strlen(to);
					continue;
				case 'T':
					//*to++ = ' ';
					continue;
				}
				break;
			case 'J':	//Justification
				switch (token[1]) {
				case 'R':	//right
					strcpy(to,"<DIV ALIGN=\"RIGHT\">");
					to += strlen(to);
					isRightJustified = true;
					continue;
				case 'C':	//center
					strcpy(to,"<DIV ALIGN=\"CENTER\">");
					to += strlen(to);
					isCentered = true;
					continue;

				case 'L':	//left, reset right and center
					if (isCentered) {
						strcpy(to,"</CENTER>");
						to += strlen(to);
						isCentered = false;
					}
					if (isRightJustified) {
						strcpy(to,"</DIV>");
						to += strlen(to);
						isRightJustified = false;
					}
					continue;
				}
				break;
			case 'T':	// title formatting
				switch (token[1]) {
				case 'T':	// Book title begin
					strcpy(to,"<BIG>");
					to += strlen(to);
					continue;
				case 't':
					strcpy(to,"</BIG>");
					to += strlen(to);
					continue;
				}
				break;

			case 'P':	// special formatting
				switch (token[1]) {
				case 'P':	// Poetry begin
					strcpy(to,"<CITE>");
					to += strlen(to);
					continue;
				case 'p':
					strcpy(to,"</CITE>");
					to += strlen(to);
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
		} else
			*to++ = *from;
	}
	*to = 0;
	return 0;
}
