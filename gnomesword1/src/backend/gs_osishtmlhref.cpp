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
#include "gs_osishtmlhref.h"
#include <utilxml.h>
#include <versekey.h>
#include <swmodule.h>

SWORD_NAMESPACE_START


GSOSISHTMLHREF::MyUserData::MyUserData(const SWModule *module, const SWKey *key) : BasicFilterUserData(module, key) {
	osisQToTick = ((!module->getConfigEntry("OSISqToTick")) || (strcmp(module->getConfigEntry("OSISqToTick"), "false")));
	if (module) 
		version = module->Name();
}


GSOSISHTMLHREF::GSOSISHTMLHREF() {
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
	addTokenSubstitute("lg", "<br />");
	addTokenSubstitute("/lg", "<br />");

	setTokenCaseSensitive(true);
}


bool GSOSISHTMLHREF::handleToken(SWBuf &buf, const char *token, BasicFilterUserData *userData) {
  // manually process if it wasn't a simple substitution
	if (!substituteToken(buf, token)) {
		MyUserData *u = (MyUserData *)userData;
		XMLTag tag(token);
		// <w> tag
		if (!strcmp(tag.getName(), "w")) {

			// start <w> tag
			if ((!tag.isEmpty()) && (!tag.isEndTag())) {
				u->w = token;
			}

			// end or empty <w> tag
			else {
				bool endTag = tag.isEndTag();
				SWBuf lastText;
				bool show = true;	// to handle unplaced article in kjv2003-- temporary till combined

				if (endTag) {
					tag = u->w.c_str();
					lastText = u->lastTextNode.c_str();
				}
				else lastText = "stuff";

				const char *attrib;
				const char *val;
				if (attrib = tag.getAttribute("xlit")) {
					val = strchr(attrib, ':');
					val = (val) ? (val + 1) : attrib;
					buf.appendFormatted(" %s", val);
				}
				if (attrib = tag.getAttribute("gloss")) {
					val = strchr(attrib, ':');
					val = (val) ? (val + 1) : attrib;
					buf.appendFormatted(" %s", val);
				}
				if (attrib = tag.getAttribute("lemma")) {
					int count = tag.getAttributePartCount("lemma");
					int i = (count > 1) ? 0 : -1;		// -1 for whole value cuz it's faster, but does the same thing as 0
					do {
						//printf("\ni = %d count = %d\n", i,count);
						attrib = tag.getAttribute("lemma", i);
						//printf("tag = %s\n",attrib);
						if (i < 0) i = 0;	// to handle our -1 condition
						val = strchr(attrib, ':');
						val = (val) ? (val + 1) : attrib;
						SWBuf gh;
						if(*val == 'G')
							gh = "Greek";
						if(*val == 'H')
							gh = "Hebrew";
						const char *val2 = val;
						if ((strchr("GH", *val)) && (isdigit(val[1])))
							val2++;
						if ((!strcmp(val2, "3588")) && (lastText.length() < 1))
							show = false;
						else	buf.appendFormatted(" <small><em>&lt;<a href=\"strongs://%s/%s\">%s</a>&gt;</em></small> ", (gh.length()) ? gh.c_str() : "", val2, val2);
					} while (++i < count);
					//printf("new\n");
				}
				if ((attrib = tag.getAttribute("morph")) && (show)) {
					SWBuf savelemma = tag.getAttribute("savlm");
					if ((strstr(savelemma.c_str(), "3588")) && (lastText.length() < 1))
						show = false;
					if (show) {
						int count = tag.getAttributePartCount("morph");
						int i = (count > 1) ? 0 : -1;		// -1 for whole value cuz it's faster, but does the same thing as 0
						do {
							attrib = tag.getAttribute("morph", i);
							if (i < 0) i = 0;	// to handle our -1 condition
							val = strchr(attrib, ':');
							val = (val) ? (val + 1) : attrib;
							const char *val2 = val;
							if ((*val == 'T') && (strchr("GH", val[1])) && (isdigit(val[2])))
								val2+=2;
							buf.appendFormatted(" <small><em>(<a href=\"morph://%s/%s\">%s</a>)</em></small> ", tag.getAttribute("morph"), val, val2);
						} while (++i < count);
					}
				}
				if (attrib = tag.getAttribute("POS")) {
					val = strchr(attrib, ':');
					val = (val) ? (val + 1) : attrib;
					buf.appendFormatted(" %s", val);
				}

				/*if (endTag)
					buf += "}";*/
			}
		}

		// <note> tag
		else if (!strcmp(tag.getName(), "note")) {
			if (!tag.isEndTag()) {
				if (!tag.isEmpty()) {
					SWBuf type = tag.getAttribute("type");

					if (type != "strongsMarkup") {	// leave strong's markup notes out, in the future we'll probably have different option filters to turn different note types on or off
						SWBuf footnoteNumber = tag.getAttribute("swordFootnote");
						VerseKey *vkey;
						// see if we have a VerseKey * or descendant
						try {
							vkey = SWDYNAMIC_CAST(VerseKey, u->key);
						}
						catch ( ... ) {	}
						if (vkey) {
							char ch = ((tag.getAttribute("type") && ((!strcmp(tag.getAttribute("type"), "crossReference")) || (!strcmp(tag.getAttribute("type"), "x-cross-ref")))) ? 'x':'n');
							if(!strcmp(u->version.c_str(),"WLC")) {
								//remove notes for the WLC
							}
							else
								buf.appendFormatted("<a href=\"noteID://%s/%s/%c/%s\"><small><sup>*%c</sup></small></a> ", u->version.c_str(), vkey->getText(), ch, footnoteNumber.c_str(), ch);
						}
					}
					u->suspendTextPassThru = true;
				}
			}
			if (tag.isEndTag()) {
				u->suspendTextPassThru = false;
			}
		}

		// <p> paragraph tag
		else if (!strcmp(tag.getName(), "p")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {	// non-empty start tag
				buf += "<!P><br />";
			}
			else if (tag.isEndTag()) {	// end tag
				buf += "<!/P><br />";
				userData->supressAdjacentWhitespace = true;
			}
			else {					// empty paragraph break marker
				buf += "<!P><br />";
				userData->supressAdjacentWhitespace = true;
			}
		}

		// <reference> tag
		else if (!strcmp(tag.getName(), "reference")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				buf += "<a href=\"\">";
			}
			else if (tag.isEndTag()) {
				buf += "</a>";
			}
		}

		// <l> poetry, etc
		else if (!strcmp(tag.getName(), "l")) {
			if (tag.isEmpty()) {
				buf += "<br />";
			}
			else if (tag.isEndTag()) {
				buf += "<br />";
			}
			else if (tag.getAttribute("sID")) {	// empty line marker
				buf += "<br />";
			}
		}

		// <milestone type="line"/>
		else if ((!strcmp(tag.getName(), "milestone")) && (tag.getAttribute("type")) && (!strcmp(tag.getAttribute("type"), "line"))) {
			buf += "<br />";
			userData->supressAdjacentWhitespace = true;
		}

		// <title>
		else if (!strcmp(tag.getName(), "title")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				buf += "<b>";
			}
			else if (tag.isEndTag()) {
				buf += "</b><br />";
			}
		}

		// <hi> hi?  hi contrast?
		else if (!strcmp(tag.getName(), "hi")) {
			SWBuf type = tag.getAttribute("type");
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				if (type == "b" || type == "x-b") {
					buf += "<b> ";
					u->inBold = true;
				}
				else {	// all other types
					buf += "<i> ";
					u->inBold = false;
				}
			}
			else if (tag.isEndTag()) {
				if(u->inBold) {
					buf += "</b>";
					u->inBold = false;
				}
				else
				      buf += "</i>";
			}
			else {	// empty hi marker
				// what to do?  is this even valid?
			}
		}

		// <q> quote
		else if (!strcmp(tag.getName(), "q")) {
			SWBuf type = tag.getAttribute("type");
			SWBuf who = tag.getAttribute("who");
			const char *lev = tag.getAttribute("level");
			int level = (lev) ? atoi(lev) : 1;
			
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				/*buf += "{";*/

				//alternate " and '
				if (u->osisQToTick)
					buf += (level % 2) ? '\"' : '\'';
				
				if (who == "Jesus") {
					buf += "<font color=\"red\"> ";
				}
			}
			else if (tag.isEndTag()) {
				//alternate " and '
				if (u->osisQToTick)
					buf += (level % 2) ? '\"' : '\'';
				//buf += "</font>";
			}
			else {	// empty quote marker
				//alternate " and '
				if (u->osisQToTick)
					buf += (level % 2) ? '\"' : '\'';
			}
		}

		// <transChange>
		else if (!strcmp(tag.getName(), "transChange")) {
			SWBuf type = tag.getAttribute("type");
			
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {

// just do all transChange tags this way for now
//				if (type == "supplied")
					buf += "<i>";
			}
			else if (tag.isEndTag()) {
				buf += "</i>";
			}
			else {	// empty transChange marker?
			}
		}

		// image
		else if (!strcmp(tag.getName(), "figure")) {
			const char *src = tag.getAttribute("src");
			if (!src)		// assert we have a src attribute
				return false;

			char* filepath = new char[strlen(u->module->getConfigEntry("AbsoluteDataPath")) + strlen(token)];
			*filepath = 0;
			strcpy(filepath, userData->module->getConfigEntry("AbsoluteDataPath"));
			strcat(filepath, src);

// we do this because BibleCS looks for this EXACT format for an image tag
			buf+="<image src=\"";
			buf+=filepath;
			buf+="\" />";
/*
			char imgc;
			for (c = filepath + strlen(filepath); c > filepath && *c != '.'; c--);
			c++;
			FILE* imgfile;
				    if (stricmp(c, "jpg") || stricmp(c, "jpeg")) {
						  imgfile = fopen(filepath, "r");
						  if (imgfile != NULL) {
								buf += "{\\nonshppict {\\pict\\jpegblip ";
								while (feof(imgfile) != EOF) {
									   buf.appendFormatted("%2x", fgetc(imgfile));
								}
								fclose(imgfile);
								buf += "}}";
						  }
				    }
				    else if (stricmp(c, "png")) {
						  buf += "{\\*\\shppict {\\pict\\pngblip ";

						  buf += "}}";
				    }
*/
			delete [] filepath;
		}
		
		else {
		      return false;  // we still didn't handle token
		}
	}
	return true;
}


SWORD_NAMESPACE_END
