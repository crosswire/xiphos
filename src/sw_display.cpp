/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_display.cpp
    * -------------------
    * Fri Mar 16 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
    *
 */
 
 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#include <gnome.h>
#include <swmodule.h>
#include <swmgr.h>
#include <versekey.h>
#include <gal/widgets/e-unicode.h>

#include "sw_display.h"
#include "support.h"
#include "sw_gnomesword.h"
#include "gs_html.h"
#include "gs_gnomesword.h"

gchar *mycolor;

/****************************************************************************************
 * externs
 ****************************************************************************************/
extern SETTINGS *settings;

/***************************************************************************** 
 * ComEntryDisp - for displaying commentary modules in a GtkHTML widget
 * the mods need to be filtered to html first
 * imodule - the Sword module to display
 *****************************************************************************/
char ComEntryDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[255], *font, *buf;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	GString *strbuf;
	SWMgr *Mgr;

	Mgr = new SWMgr();	//-- create sword mgr
	font = "Roman";
	buf = (char *) imodule.Description();
	if ((sit = Mgr->config->Sections.find(imodule.Name())) !=
	    Mgr->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
	}
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(settings->app, "nbCom")), 0);
	(const char *) imodule;	/* snap to entry */
	strbuf = g_string_new("<B><FONT COLOR=\"#000FCF\">");
	sprintf(tmpBuf, "<A HREF=\"[%s]%s\"> [%s]</a>[%s] </b>",
		imodule.Name(), buf, imodule.Name(), imodule.KeyText());
	strbuf = g_string_append(strbuf, tmpBuf);
	/* show verse ref in text widget  */
	/* show module text for current key */
	beginHTML(GTK_WIDGET(gtkText), TRUE);
	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);
	if (!strcmp(font, "Symbol")) {
		strbuf = g_string_new("<FONT FACE=\"symbol\">");
		strbuf = g_string_append(strbuf, (const char *) imodule);
		strbuf = g_string_append(strbuf, "</font>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else if (!strcmp(font, "Greek")) {
		strbuf = g_string_new("<FONT FACE=\"greek1\">");
		strbuf = g_string_append(strbuf, (const char *) imodule);
		strbuf = g_string_append(strbuf, "</font>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else {
		strbuf = g_string_new((const char *) imodule);
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	}
	endHTML(GTK_WIDGET(gtkText));
	delete Mgr;
	return 0;
}

/***************************************************************************** 
 * GtkHTMLEntryDisp - for displaying dict/lex modules in a GtkHTML 
 * widget the mods need to be filtered to html first
 *****************************************************************************/
char GtkHTMLEntryDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[500], *buf, *font, *use_font, *use_font_size, *token;
	gchar *utf8str;
	gint mybuflen, utf8len;
	const gchar **end;
	string lang, swfont, swfontsize;
	SWMgr *Mgr;
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;

	Mgr = new SWMgr();	//-- create sword mgr

	if ((sit = Mgr->config->Sections.find(imodule.Name())) != Mgr->config->Sections.end()) {
		ConfigEntMap & section = (*sit).second;
		swfont = ((entry = section.find("GSFont")) != section.end())? (*entry).second : (string) "";
		swfontsize = ((entry = section.find("GSFont size")) != section.end())? (*entry).second : (string) "";
		lang = ((entry = section.find("Lang")) != section.end())? (*entry).second : (string) "";
	}
	font = g_strdup("-adobe-helvetica-*-*");
	if (strcmp(swfontsize.c_str(), "")) {
		use_font_size = (gchar *) swfontsize.c_str();
	} else {
		use_font_size = settings->bible_font_size;
	}
	if (strcmp(swfont.c_str(), "")) {
		use_font = (gchar *) swfont.c_str();
	} else {
		font = g_strdup(settings->default_font);
		use_font = gethtmlfontnameHTML(font);
	}
	(const char *) imodule;	/* snap to entry */
	beginHTML(GTK_WIDGET(gtkText), TRUE);
	sprintf(tmpBuf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings->bible_bg_color, settings->bible_text_color,
		settings->link_color);
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	/* show verse ref in text widget  */
	sprintf(tmpBuf,
		"<A HREF=\"[%s] %s\"><FONT COLOR=\"%s\"><B>[%s]</B></A></font>[%s] ",
		imodule.Name(), imodule.Description(),
		settings->bible_verse_num_color, imodule.Name(),
		imodule.KeyText());
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

	sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">",
		use_font, use_font_size);
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	displayHTML(GTK_WIDGET(gtkText), (const char *) imodule,
		    strlen((const char *) imodule));
	sprintf(tmpBuf, " %s", "</font></body></html>");
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	endHTML(GTK_WIDGET(gtkText));
	g_free(font);
	delete Mgr;
	return 0;
}

/****************************************************************************** 
 * GTKutf8ChapDisp - for displaying text modules 
 * in a GtkHTML widget a chapter at a time 
 * - the mods need to be filtered to html first
 * imodule - the Sword module to display
 ******************************************************************************/
char GTKutf8ChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[500], *buf, *mybuf, versecolor[80];
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gint len;
	char *Buf, c;
	gchar *utf8str, *use_font, *use_font_size, *font, *token, *bufstr;
	gint mybuflen, utf8len;
	const gchar **end;
	bool newparagraph = false;
	string lang, swfont, swfontsize;
	SWMgr *Mgr;
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;
	
	c = 182;  
	Mgr = new SWMgr();	//-- create sword mgr
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(settings->app, "nbText")), 1);

	if ((sit =
	     Mgr->config->Sections.find(imodule.Name())) !=
	    Mgr->config->Sections.end()) {
		ConfigEntMap & section = (*sit).second;
		swfont =
		    ((entry =
		      section.find("GSFont")) !=
		     section.end())? (*entry).second : (string) "";
		swfontsize =
		    ((entry =
		      section.find("GSFont size")) !=
		     section.end())? (*entry).second : (string) "";
		lang =
		    ((entry =
		      section.find("Lang")) !=
		     section.end())? (*entry).second : (string) "";
	}
	font = g_strdup("-adobe-helvetica-*-*");
	if (strcmp(swfontsize.c_str(), "")) {
		use_font_size = (gchar *) swfontsize.c_str();
	} else {
		use_font_size = settings->bible_font_size;
	}
	if (strcmp(swfont.c_str(), "")) {
		use_font = (gchar *) swfont.c_str();
	} else {
		if (!stricmp(lang.c_str(), "") ||
		    !stricmp(lang.c_str(), "en") ||
		    !stricmp(lang.c_str(), "de")) {
			font = g_strdup(settings->default_font);
		} else if (!stricmp(lang.c_str(), "grc")) {
			font = g_strdup(settings->greek_font);
		} else if (!stricmp(lang.c_str(), "he")) {
			font = g_strdup(settings->hebrew_font);
		} else {
			font = g_strdup(settings->unicode_font);
		}
		use_font = gethtmlfontnameHTML(font);
	}
	beginHTML(GTK_WIDGET(gtkText), TRUE);
	sprintf(tmpBuf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings->bible_bg_color, settings->bible_text_color,
		settings->link_color);
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

	for (key->Verse(1);
	     (key->Book() == curBook && key->Chapter() == curChapter
	      && !imodule.Error()); imodule++) {
		if (key->Verse() == curVerse)
			sprintf(versecolor, "%s",
				settings->currentverse_color);
		else
			sprintf(versecolor, "%s",
				settings->bible_text_color);
		sprintf(tmpBuf,
			"&nbsp; <A HREF=\"*[%s] %s\" NAME=\"%d\"><FONT COLOR=\"%s\"><B>  %d</B></font></A> ",
			imodule.Description(), imodule.KeyText(),
			key->Verse(), settings->bible_verse_num_color,
			key->Verse());
		utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

		sprintf(tmpBuf,
			"<font face=\"%s\" color=\"%s\" size=\"%s\">",
			use_font, versecolor, use_font_size);
		utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
		
		if(newparagraph && settings->versestyle) {
			newparagraph = false;
			sprintf(tmpBuf,  "%c ", c);
			utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
			utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
			displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
		} 
		
		utf8str = (char *) imodule;
		displayHTML(GTK_WIDGET(gtkText), utf8str,
			    strlen(utf8str));
		if (settings->versestyle) {
			if (strstr((const char *) imodule, "<BR>") == NULL ) {
				sprintf(tmpBuf, " %s", "</font><br>");
			} else {
				sprintf(tmpBuf, " %s", "</font>");
			}
			if (strstr((const char *) imodule, "<!P>") == NULL) {
				newparagraph = false;
			} else {
				newparagraph = true;
			}
		} else
			if (strstr((const char *) imodule, "<!P>") == NULL)
				sprintf(tmpBuf, " %s", "</font>");
			else 
				sprintf(tmpBuf, " %s", "</font><p>");
		utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	}
	sprintf(tmpBuf, " %s", "</body></html>");
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	sprintf(tmpBuf, "%d", curVerse);
	endHTML(GTK_WIDGET(gtkText));
	gotoanchorHTML(gtkText, tmpBuf);
	g_free(font);
	delete Mgr;
	return 0;
}

/* --------------------------------------------------------------------------------------------- */
char InterlinearDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[800], *buf, *rowcolor;
	gint i;
	bool utf = false;
	gint len;
	gchar *utf8str, *use_font, *use_font_size, *font, *token;
	gint utf8len;
	string lang, swfont, swfontsize;
	SWMgr *Mgr;
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;
	static gint row = 1;

	Mgr = new SWMgr();	//-- create sword mgr
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(settings->app, "nbText")), 1);

	if ((sit =
	     Mgr->config->Sections.find(imodule.Name())) !=
	    Mgr->config->Sections.end()) {
		ConfigEntMap & section = (*sit).second;
		swfont =
		    ((entry =
		      section.find("GSFont")) !=
		     section.end())? (*entry).second : (string) "";
		swfontsize =
		    ((entry =
		      section.find("GSFont size")) !=
		     section.end())? (*entry).second : (string) "";
		lang =
		    ((entry =
		      section.find("Lang")) !=
		     section.end())? (*entry).second : (string) "";
	}
	if (strcmp(swfontsize.c_str(), "")) {
		use_font_size = (gchar *) swfontsize.c_str();
	} else {
		use_font_size = settings->interlinear_font_size;
	}
	font = g_strdup("-adobe-helvetica-*-*");
	if (strcmp(swfont.c_str(), "")) {
		use_font = (gchar *) swfont.c_str();
	} else {
		font = "-adobe-helvetica-*-*";
		if (!stricmp(lang.c_str(), "") ||
		    !stricmp(lang.c_str(), "en") ||
		    !stricmp(lang.c_str(), "de")) {
			font = g_strdup(settings->default_font);
		} else if (!stricmp(lang.c_str(), "grc")) {
			font = g_strdup(settings->greek_font);
		} else if (!stricmp(lang.c_str(), "he")) {
			font = g_strdup(settings->hebrew_font);
		} else {
			font = g_strdup(settings->unicode_font);
		}
		use_font = gethtmlfontnameHTML(font);
	}
	if (row == 6)
		row = 1;
	if (row == 1 || row == 3 || row == 5)
		rowcolor = "#F1F1F1";
	else
		rowcolor = settings->bible_bg_color;
	(const char *) imodule;
	if (row == 1) {
		sprintf(tmpBuf,
			"<tr><td><i><FONT COLOR=\"%s\" SIZE=\"%s\">[%s]</font></i></td></tr>",
			settings->bible_verse_num_color,
			settings->verse_num_font_size, imodule.KeyText());
		utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	}
	++row;
	sprintf(tmpBuf,
		"<tr bgcolor=\"%s\"><td><B><A HREF=\"[%s]%s\"><FONT COLOR=\"%s\" SIZE=\"%s\"> [%s]</font></a></b>",
		rowcolor,
		imodule.Name(),
		imodule.Description(),
		settings->bible_verse_num_color,
		settings->verse_num_font_size,
		imodule.Name(), settings->bible_verse_num_color);
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

	sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">", use_font, use_font_size);	//, settings->interlinear_font_size);
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	displayHTML(GTK_WIDGET(gtkText), (const char *) imodule,
		    strlen((const char *) imodule));
	sprintf(tmpBuf,
		"</font><small>[<A HREF=\"@%s\">view context</a>]</small></td></tr>",
		imodule.Name());
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	displayHTML(GTK_WIDGET(gtkText), utf8str, strlen(utf8str));
	g_free(font);
	delete Mgr;
	return 0;
}

/* ***************************************************************************
 * to display Sword module about information
 *****************************************************************************/
void AboutModsDisplayHTML(char *to, char *text)
{
	int len, i;
	bool center = false;
	// shift string to right of buffer
	// -------------------------------
	for (i = 0; i < strlen(text) - 1; i++) {
		if (text[i] == '\\')	// a RTF command
		{
			if ((text[i + 1] == 'p') && (text[i + 2] == 'a') && (text[i + 3] == 'r') && (text[i + 4] == 'd')) {	// switch all modifier off

				if (center) {
					*to++ = '<';
					*to++ = '/';
					*to++ = 'C';
					*to++ = 'E';
					*to++ = 'N';
					*to++ = 'T';
					*to++ = 'E';
					*to++ = 'R';
					*to++ = '>';
					center = false;
				}
				i += 4;
				continue;
			}
			if ((text[i + 1] == 'p') && (text[i + 2] == 'a')
			    && (text[i + 3] == 'r')) {
				*to++ = '<';
				*to++ = 'b';
				*to++ = 'r';
				*to++ = '>';
				*to++ = '\n';
				i += 3;
				continue;
			}
			if (text[i + 1] == ' ') {
				i += 1;
				continue;
			}
			if (text[i + 1] == '\n') {
				i += 1;
				continue;
			}
			if ((text[i + 1] == 'q') && (text[i + 2] == 'c'))	// center on
			{
				if (!center) {
					*to++ = '<';
					*to++ = 'C';
					*to++ = 'E';
					*to++ = 'N';
					*to++ = 'T';
					*to++ = 'E';
					*to++ = 'R';
					*to++ = '>';
					center = true;
				}
				i += 2;
				continue;
			}
		}
		*to++ = text[i];
	}
	*to++ = 0;
	//return to;

}

GdkColor GTKEntryDisp::colourBlue;
GdkColor GTKEntryDisp::colourGreen;
GdkColor GTKEntryDisp::colourRed;
GdkColor GTKEntryDisp::colourCur;

GdkColor myGreen;
GdkColor BGcolor;
GdkFont *roman_font, *italic_font, *versenum_font, *bold_font;


gchar *font_mainwindow =
    "-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_italic_mainwindow =
    "-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_interlinear =
    "-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_italic_interlinear =
    "-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_currentverse =
    "-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_italic_currentverse =
    "-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1";

extern SWMgr *mainMgr;
extern SWMgr *mainMgr1;
//extern bool bVerseStyle;
extern GtkWidget *MainFrm;	/* pointer to app -- declared in GnomeSword.cpp */
extern SWModule *comp1Mod;
extern gchar *current_verse;
//extern INTERLINEAR interlinearMods;
extern SETTINGS *settings;

/* --------------------------------------------------------------------------------------------- */
char GTKEntryDisp::Display(SWModule & imodule)
{
	char tmpBuf[255], *sourceType;
	GdkFont *sword_font;
	ModMap::iterator it;


	/* Load a  font */
	sword_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");
	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete(GTK_TEXT(gtkText),
				gtk_text_get_length((GTK_TEXT(gtkText))));

	int curPos = 0;
	(const char *) imodule;	/* snap to entry */
	gtk_text_freeze(GTK_TEXT(gtkText));
	/* let's find out if we have a comment or dict module */
	it = mainMgr->Modules.find(imodule.Name());
	sprintf(tmpBuf, "[%s][%s] ", imodule.Name(), imodule.KeyText());
	/* show verse ref in text widget  */
	gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL, tmpBuf,
			-1);
	/* show module text for current key */
	gtk_text_insert(GTK_TEXT(gtkText), sword_font,
			&gtkText->style->black, NULL,
			(const char *) imodule, -1);
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
	return 0;
}

/* --------------------------------------------------------------------------------------------- */
char GTKPerComDisp::Display(SWModule & imodule)
{
	char tmpBuf[255];
	GdkFont *sword_font;
	ModMap::iterator it;

	/* Load a  font */
	sword_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");

	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete(GTK_TEXT(gtkText),
				gtk_text_get_length((GTK_TEXT(gtkText))));
	int curPos = 0;
	(const char *) imodule;	/*  snap to entry */
	gtk_text_freeze(GTK_TEXT(gtkText));

	/* let's find out if we have a comment or dict module */
	it = mainMgr->Modules.find(imodule.Name());
	sprintf(tmpBuf, "[%s] ", imodule.KeyText());	/* else just the keytext */
	if (((*mainMgr->config->Sections[imodule.Name()].find("ModDrv")).second == "RawFiles") &&	/* check for personal comments by finding ModDrv=RawFiles */
	    (GTK_TOGGLE_BUTTON(lookup_widget(MainFrm, "btnEditNote"))->active)) {	/* check for edit mode */
		GtkWidget *statusbar;	/* pointer to comments statusbar */
		gint context_id2;	/* statusbar context_id ??? */
		sprintf(tmpBuf, "[%s] ", imodule.KeyText());	/* add module name and verse to edit note statusbar */
		/* setup statusbar for personal comments */
		statusbar = lookup_widget(MainFrm, "sbNotes");	/*        get stutusbar */
		context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "GnomeSword");	/* get context id */
		gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);	/* ready status */
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2, tmpBuf);	/* show modName and verse ref in statusbar */
		gtk_text_insert(GTK_TEXT(gtkText), sword_font, &gtkText->style->black, NULL, " ", -1);	/* change font color to black for editing */
		gtk_text_set_point(GTK_TEXT(gtkText), 0);
		gtk_text_forward_delete(GTK_TEXT(gtkText),
					gtk_text_get_length((GTK_TEXT
							     (gtkText))));
	} else {		/* not useing personal comment module in edit mode */
		gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL, tmpBuf, -1);	/* show modName and verse ref in text widget */
	}
	/* show module text for current key */
	gtk_text_insert(GTK_TEXT(gtkText), sword_font,
			&gtkText->style->black, NULL,
			(const char *) imodule, -1);
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
	return 0;
}


