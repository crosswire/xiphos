/***************************************************************************
                     osishtmlhref.cpp  -  OSIS to HTML with hrefs filter
                             -------------------
    begin                : 2003-06-24
    copyright            : 2003 by CrossWire Bible Society
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
#include "backend/gs_osishtmlhref.h"
#include <utilxml.h>
#include <versekey.h>
#include <swmodule.h>
#include <url.h>

#include "main/settings.h"

SWORD_NAMESPACE_START 
GS_OSISHTMLHREF::MyUserData::MyUserData(const SWModule * module, const SWKey * key):
BasicFilterUserData(module, key)
{
	osisQToTick = ((!module->getConfigEntry("OSISqToTick"))
		       || (strcmp(module->getConfigEntry("OSISqToTick"), "false")));
	if (module)
		version = module->Name();
	inRed = false;
}


GS_OSISHTMLHREF::GS_OSISHTMLHREF()
{
	setTokenStart("<");
	setTokenEnd(">");

	setEscapeStart("&");
	setEscapeEnd(";");

	setEscapeStringCaseSensitive(true);

	addEscapeStringSubstitute("amp", "&");
	addEscapeStringSubstitute("apos", "'");
	addEscapeStringSubstitute("lt", "<");
	addEscapeStringSubstitute("gt", ">");
	addEscapeStringSubstitute("quot", "\"");
	addTokenSubstitute("lg",  (!settings.versestyle)?"<br />":"");
	addTokenSubstitute("/lg", (!settings.versestyle)?"<br />":"");

	setTokenCaseSensitive(true);
}


bool
GS_OSISHTMLHREF::handleToken(SWBuf & buf, const char *token, BasicFilterUserData * userData) {
	// manually process if it wasn't a simple substitution
	if (!substituteToken(buf, token)) {
		MyUserData     *u = (MyUserData *) userData;
		XMLTag tag(token);

		if (!tag.getName()) {
			return false;
		}
		// <q> quote
		if (!strcmp(tag.getName(), "q")) {
			SWBuf type = tag.getAttribute("type");
			SWBuf who = tag.getAttribute("who");
			const char     *lev = tag.getAttribute("level");
			int
			                level = (lev) ? atoi(lev) : 1;
			u->inRed = false;
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				/*
				 * buf += "{";
				 */

				// alternate " and '
				if (u->osisQToTick)
					buf += (level % 2) ? '\"' : '\'';

				if (who == "Jesus") {
					u->inRed = true;
					buf += "<font color=\"red\"> ";			
				}
			} else if (tag.isEndTag()) {
				// alternate " and '
				if (u->osisQToTick)
					buf += (level % 2) ? '\"' : '\'';
				// buf += "</font>";
			} else {	// empty quote marker
				// alternate " and '
				if (u->osisQToTick)
					buf += (level % 2) ? '\"' : '\'';
			}
		}
		// <w> tag
		else if (!strcmp(tag.getName(), "w")) {
			// start <w> tag
			if ((!tag.isEmpty()) && (!tag.isEndTag())) {
				const char     *attrib;
				const char     *val;

				SWBuf           outTag;
				SWBuf           attrValue;
				SWBuf 		gh;
				outTag = "<span sword_url=\"passagestudy.jsp?";

				if ((attrib = tag.getAttribute("xlit"))) {
					val = strchr(attrib, ':');
					val = (val) ? (val + 1) : attrib;
					outTag.appendFormatted("xlit=%s&", val);
				}
				if ((attrib = tag.getAttribute("gloss"))) {
					val = strchr(attrib, ':');
					val = (val) ? (val + 1) : attrib;
					outTag.appendFormatted("gloss=%s&", val);
				}
				if ((attrib = tag.getAttribute("lemma"))) {
					const int
					                count = tag.getAttributePartCount("lemma");
					int
					                i = (count > 1) ? 0 : -1;	
					attrValue = "";

					do {
						if (attrValue.length()) {
							attrValue += "|";
						}

						attrib = tag.getAttribute("lemma", i);
						if (i < 0) {	// to handle our -1 condition
							i = 0;
						}
						val = strchr(attrib, ':');
						val = (val) ? (val + 1) : attrib;

						if(*val == 'G')
							gh = "Greek";
						if(*val == 'H')
							gh = "Hebrew";
							
						if ((*val == 'H') || (*val == 'G')) {
							++val;
							attrValue.append(val);
						}
					}
					while (++i < count);
					

					if (attrValue.length()) {
						outTag.appendFormatted("type=%s&lemma=%s&",
							(gh.length()) ? URL::encode(gh.c_str()).c_str() : "", 
							URL::encode(attrValue.c_str()).c_str());
					}
				}
				if ((attrib = tag.getAttribute("morph"))) {
					//printf("%s\n",token);
					const int  count = tag.getAttributePartCount("morph");
					int i = (count > 1) ? 0 : -1;	// -1 for whole value cuz it's faster, but does the same thing as 0
					attrValue = "";
					do {
						if (attrValue.length()) {
							attrValue += "|";
						}

						attrib = tag.getAttribute("morph", i);
						if (i < 0) {
							i = 0;	// to handle our -1 condition
						}
						val = strchr(attrib, ':');
						val = (val) ? (val + 1) : attrib;
						if (!strncmp(attrib, "x-Robinson", 10)) {	// robinson 
							// 
							// 
							// codes
							attrValue.append(val);
						} else if ((*val == 'T') && ((val[1] == 'G') || (val[1] == 'H'))) {
							attrValue.append(val + 1);
						} else if ((*val == 'T')) {
							attrValue.append(val);
						}
					}
					while (++i < count);

					if (attrValue.length()) {
						outTag.appendFormatted("morph=%s&", attrValue.c_str());
					}
				}
				if ((attrib = tag.getAttribute("POS"))) {
					val = strchr(attrib, ':');
					val = (val) ? (val + 1) : attrib;
					outTag.appendFormatted("pos=%s&", val);
				}

				buf.appendFormatted("%s\">",outTag.c_str());
			} else if (tag.isEndTag()) {	// end or empty <w> tag
				buf += "</span>";
			}
		}
/*************************************************************************************************************/
		// <note> tag
		else if (!strcmp(tag.getName(), "note")) {
			if (!tag.isEndTag()) {
				if (!tag.isEmpty()) {
					SWBuf type = tag.getAttribute("type");

					if (type != "strongsMarkup") {	// leave strong's
						// markup notes
						// out, in the
						// future we'll
						// probably have
						// different
						// option filters
						// to turn
						// different note
						// types on or off
						SWBuf footnoteNumber = tag.getAttribute("swordFootnote");
						VerseKey       *vkey;
						// see if we have a VerseKey * or descendant
						SWTRY {
							vkey = SWDYNAMIC_CAST(VerseKey, u->key);
						}
						SWCATCH(...) {
						}
						if (vkey) {
							// printf("URL =
							// %s\n",URL::encode(vkey->getText()).c_str());
							char
							                ch = ((tag.getAttribute("type")
									       && ((!strcmp(tag.getAttribute("type"),
											    "crossReference"))
										   || (!strcmp(tag.getAttribute("type"),
											       "x-cross-ref")))) ? 'x' : 'n');
							buf.appendFormatted
							    ("<a href=\"passagestudy.jsp?action=showNote&type=%c&value=%s&module=%s&passage=%s\"><small><sup>*%c</sup></small></a> ",
							     ch, URL::encode(footnoteNumber.c_str()).c_str(), URL::encode(u->version.c_str()).c_str(), URL::encode(vkey->getText()).c_str(), ch);
						}
					}
				}
				u->suspendTextPassThru = true;
			}
			if (tag.isEndTag()) {
				u->suspendTextPassThru = false;
			}
		}
		// <p> paragraph tag
		else if (!strcmp(tag.getName(), "p")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty()) && !settings.versestyle) {	// non-empty 
				// 
				// 
				// start
				// tag
				buf += "<p>";
			} else if (tag.isEndTag() && !settings.versestyle) {	// end tag
				buf += "</p>";
				userData->supressAdjacentWhitespace = true;
			} else if (!settings.versestyle) {	// empty paragraph break marker
				buf += "<p>";
				userData->supressAdjacentWhitespace = true;
			}
		}
		// <reference> tag
		else if (!strcmp(tag.getName(), "reference")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				buf += "<a href=\"\">";
			} else if (tag.isEndTag()) {
				buf += "</a>";
			}
		}
		// <l> poetry, etc
		else if (!strcmp(tag.getName(), "l")) {
			if (tag.isEmpty() && !settings.versestyle) {
				buf += "<br />";
			} else if (tag.isEndTag() && !settings.versestyle) {
				buf += "<br />";
			} else if (tag.getAttribute("sID") && !settings.versestyle) {	// empty line
				// marker
				buf += "<br />";
			}
		}
		// <milestone type="line"/>
		else if ((!strcmp(tag.getName(), "milestone"))
			 && (tag.getAttribute("type"))
			 && (!strcmp(tag.getAttribute("type"), "line")) 
			 && !settings.versestyle) {
			buf += "<br />";
			userData->supressAdjacentWhitespace = true;
		}
		// <title>
		else if (!strcmp(tag.getName(), "title")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				buf += "<b>";
			} else if (tag.isEndTag()) { 
				if(!settings.versestyle)
					buf += "</b><br />";
				else 
					buf += "</b>";					
			}
		}
		// <hi> hi? hi contrast?
		else if (!strcmp(tag.getName(), "hi")) {
			SWBuf type = tag.getAttribute("type");
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				if (type == "b" || type == "x-b") {
					buf += "<b> ";
					u->inBold = true;
				} else {	// all other types
					buf += "<i> ";
					u->inBold = false;
				}
			} else if (tag.isEndTag()) {
				if (u->inBold) {
					buf += "</b>";
					u->inBold = false;
				} else
					buf += "</i>";
			} else {	// empty hi marker
				// what to do? is this even valid?
			}
		}
		// <transChange>
		else if (!strcmp(tag.getName(), "transChange")) {
			SWBuf type = tag.getAttribute("type");

			if ((!tag.isEndTag()) && (!tag.isEmpty())) {

				// just do all transChange tags this way for now
				// if (type == "supplied")
				buf += "<i>";
			} else if (tag.isEndTag()) {
				buf += "</i>";
			} else {	// empty transChange marker?
			}
		}
		// image
		else if (!strcmp(tag.getName(), "figure")) {
			const char     *src = tag.getAttribute("src");
			if (!src)	// assert we have a src attribute
				return false;

			char           *filepath = new char[strlen(u->module->getConfigEntry("AbsoluteDataPath"))
							    + strlen(token)];
			*filepath = 0;
			strcpy(filepath, userData->module->getConfigEntry("AbsoluteDataPath"));
			strcat(filepath, src);

			// we do this because BibleCS looks for this EXACT format for
			// an image tag
			buf += "<image src=\"";
			buf += filepath;
			buf += "\" />";
			/*
			 * char imgc; for (c = filepath + strlen(filepath); c >
			 * filepath && *c != '.'; c--); c++; FILE* imgfile; if
			 * (stricmp(c, "jpg") || stricmp(c, "jpeg")) { imgfile =
			 * fopen(filepath, "r"); if (imgfile != NULL) { buf +=
			 * "{\\nonshppict {\\pict\\jpegblip "; while (feof(imgfile) != 
			 * EOF) { buf.appendFormatted("%2x", fgetc(imgfile)); }
			 * fclose(imgfile); buf += "}}"; } } else if (stricmp(c,
			 * "png")) { buf += "{\\*\\shppict {\\pict\\pngblip ";
			 * 
			 * buf += "}}"; } 
			 */
			delete[]filepath;
		}

		else {
			return false;	// we still didn't handle token
		}
	}
	return true;
}


SWORD_NAMESPACE_END
