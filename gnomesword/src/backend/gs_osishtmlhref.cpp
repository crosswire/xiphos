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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdlib.h>
#include "backend/gs_osishtmlhref.h"
#include <utilxml.h>
#include <versekey.h>
#include <swmodule.h>
#include <url.h>

#include "main/settings.h"

extern int strongs_on;
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
	//g_message(token);
	if (!substituteToken(buf, token)) {
		MyUserData     *u = (MyUserData *) userData;
		XMLTag tag(token);
		
		
		if (!tag.getName()) {
			return false;
		}
		// <q> quote
		//g_message("\ntoken: %s", token);
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

#ifdef USE_GTKMOZEMBED		// <w> tag
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
					const int  count = tag.getAttributePartCount("lemma", ' ');
					int  i = (count > 1) ? 0 : -1;	
					attrValue = "";

					do {
						if (attrValue.length()) {
							attrValue += "|";
						}

						attrib = tag.getAttribute("lemma", i,' ');
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
						}
							attrValue.append(val);
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
					const int  count = tag.getAttributePartCount("morph", ' ');
					int i = (count > 1) ? 0 : -1;	// -1 for whole value cuz it's faster, but does the same thing as 0
					attrValue = "";
					do {
						if (attrValue.length()) {
							attrValue += "|";
						}

						attrib = tag.getAttribute("morph", i, ' ');
						if (i < 0) {
							i = 0;	// to handle our -1 condition
						}
						val = strchr(attrib, ':');
						val = (val) ? (val + 1) : attrib;
						if (!strncmp(attrib, "robinson", 8)) {	// robinson 
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
#else
		// <w> tag
		else if (!strcmp(tag.getName(), "w")) {
			// start <w> tag
			if ((!tag.isEmpty()) && (!tag.isEndTag())) {
				u->w = token;
			}

			// end or empty <w> tag
			else {
				bool endTag = tag.isEndTag();
				SWBuf lastText;
				//bool show = true;	// to handle unplaced article in kjv2003-- temporary till combined

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
					int mycount = 0;
				if ((attrib = tag.getAttribute("lemma")) && strongs_on) {
					int count = tag.getAttributePartCount("lemma", ' ');
				//	g_message("AttributePartCount = %d\nmycount = %d",count,++mycount);
					int i = (count > 1) ? 0 : -1;		// -1 for whole value cuz it's faster, but does the same thing as 0
					do {
						attrib = tag.getAttribute("lemma", i, ' ');
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
						if (!strcmp(val2, "3588"))  gh = "Greek"; // untill kjv module adds G to 3588
						
						if (!u->suspendTextPassThru)
						buf.appendFormatted(" <small><em>&lt;<a href=\"passagestudy.jsp?action=showStrongs&type=%s&value=%s\">%s</a>&gt;</em></small> ", 
							(gh.length()) ? gh.c_str() : "", 
							URL::encode(val2).c_str(), 
							val2);
						
					} while (++i < count);
				}
				if ((attrib = tag.getAttribute("morph"))) { // && (show)) {
					/*SWBuf savelemma = tag.getAttribute("savlm");
					if ((strstr(savelemma.c_str(), "3588")) && (lastText.length() < 1))
						show = false;
					if (show) {*/
						int count = tag.getAttributePartCount("morph", ' ');
						int i = (count > 1) ? 0 : -1;		// -1 for whole value cuz it's faster, but does the same thing as 0
						do {
							attrib = tag.getAttribute("morph", i, ' ');
							if (i < 0) i = 0;	// to handle our -1 condition
							val = strchr(attrib, ':');
							val = (val) ? (val + 1) : attrib;
							const char *val2 = val;
							if ((*val == 'T') && (strchr("GH", val[1])) && (isdigit(val[2])))
								val2+=2;
						if (!u->suspendTextPassThru)
							buf.appendFormatted(" <small><em>(<a href=\"passagestudy.jsp?action=showMorph&type=%s&value=%s\">%s</a>)</em></small> ", 
								URL::encode(tag.getAttribute("morph")).c_str(),
								URL::encode(val).c_str(), 
								val2);
						} while (++i < count);
					//}
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
#endif		
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
						u->inNote = true;
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
							char ch = ((tag.getAttribute("type")
									      && ((!strcmp(tag.getAttribute("type"),
											    "crossReference"))
									      || (!strcmp(tag.getAttribute("type"),
											     "x-cross-ref")))) ? 'x' : 'n');
							buf.appendFormatted
							    ("<a href=\"passagestudy.jsp?action=showNote&type=%c&value=%s&module=%s&passage=%s\"><small><sup>*%c</sup></small></a> ",
							     	ch, 
								URL::encode(footnoteNumber.c_str()).c_str(), 
								URL::encode(u->version.c_str()).c_str(), 
								URL::encode(vkey->getText()).c_str(), ch);
						}
					}
				}
				u->suspendTextPassThru = true;
			}
			if (tag.isEndTag()) {
				u->inNote = false;
				u->suspendTextPassThru = false;
			}
		}
		// <p> paragraph tag
		else if (!strcmp(tag.getName(), "p")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {	// non-empty 
				buf += (settings.versestyle)?"<!p>":"<p>";
			} else if (tag.isEndTag() && !settings.versestyle) {	// end tag
				buf += "</p>";
				userData->supressAdjacentWhitespace = true;
			} else {	// empty paragraph break marker
				if(!strcmp(u->version.c_str(), "KJV")) {
					//g_message(u->version.c_str());
					VerseKey       *vkey;
					// see if we have a VerseKey * or descendant
					SWTRY {
						vkey = SWDYNAMIC_CAST(VerseKey, u->key);
					}
					SWCATCH(...) {
					}
					if (vkey) {					
						int curBook = vkey->Book();
						int curTestament = vkey->Testament();
					
						if(curTestament == 2 && curBook < 6)
							buf += (settings.versestyle)?"<!p>":"<p>";
					}
				} else 
					buf += (settings.versestyle)?"<!p>":"<p>";
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
		// <milestone type="x-p" marker="&#182;"/>
		else if (!strcmp(tag.getName(), "milestone")&& tag.getAttribute("type")) {
			if(!strcmp(tag.getAttribute("type"),"x-p")) 
				if( tag.getAttribute("marker"))
					buf += tag.getAttribute("marker");
				else
					buf +=  "<!p>";
			else if(!strcmp(tag.getAttribute("type"), "line"))  {
				buf += "<br />";
				//g_message("\ntoken: %s\ntag: %s\ntype: %s",token,tag.getName(),tag.getAttribute("type"));
				userData->supressAdjacentWhitespace = true;
			}
		}
		// <title>
		else if (!strcmp(tag.getName(), "title")) {
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				buf += "<br /><br /><b>";
			} else if (tag.isEndTag()) {
				buf += (settings.versestyle)?"</b>":"</b><br />";
			}
		}
		// divineName  
		else if (!strcmp(tag.getName(), "divineName")) {
			SWBuf type = tag.getAttribute("type");
			if ((!tag.isEndTag()) && (!tag.isEmpty())) {
				if (type == "x-yhwh") {
					u->inName = true;
					u->suspendTextPassThru = true;
				} else {
					u->inName = false;
				}
			} else if (tag.isEndTag()) {
				if(u->inName) {
					char firstChar = *u->lastTextNode.c_str();
					const char *name = u->lastTextNode.c_str();
			//g_message("\nverse: %s\ntoken: %s\nname: %s",(char*)u->key->getText(), token,name);
					++name;
					buf += firstChar;
					buf += "<font size=\"-1\">";
					for(int i=0;i<strlen(name);i++)
						buf += toupper(name[i]);
					buf += "</font>";
					u->inName = false;
					u->suspendTextPassThru = false;
				}
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
