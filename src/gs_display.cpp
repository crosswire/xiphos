/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          gs_display.cpp  -  description
                             -------------------
    begin                : Fri Mar 16 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

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
 *  You should have received a copy of the GNU Library General Public License
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

#include "gs_display.h"
#include "support.h"
#include "gs_sword.h"
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
	gchar tmpBuf[500], *buf, *font, *use_font, *token;
	gchar *utf8str;
	gint mybuflen, utf8len;
	const gchar **end;

	font = "-adobe-helvetica-*-*";
	font = g_strdup(settings->greek_font);	
	++font;
	token=strtok(font,"-");
	use_font = token;
	use_font = strtok(NULL,"-");
	//g_warning("use_font = %s",use_font);
	
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
		use_font, settings->bible_font_size);
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
	return 0;
}

/****************************************************************************** 
 * GTKhtmlChapDisp - for displaying text modules 
 * in a GtkHTML widget a chapter at a time 
 * - the mods need to be filtered to html first
 * imodule - the Sword module to display
 ******************************************************************************/
char GTKhtmlChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[500], *buf, *mybuf, *use_font;
	string font, lang;
	SWMgr *Mgr;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	GString *strbuf;
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gint len;
	gchar *Buf, c;
	bool newparagraph = false;
	gint mybuflen;
	ConfigEntMap::iterator entry;
  	
	Mgr = new SWMgr();	//-- create sword mgr
	c = 182;
	font = "Roman";
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(settings->app, "nbText")), 1);
	if ((sit = Mgr->config->Sections.find(imodule.Name())) != Mgr->config->Sections.end()) {
		ConfigEntMap &section = (*sit).second;
		font = ((entry = section.find("Font")) != section.end()) ? (*entry).second : (string) "";
		lang = ((entry = section.find("Lang")) != section.end()) ? (*entry).second : (string) "";
	} 
	beginHTML(GTK_WIDGET(gtkText), TRUE);
	strbuf = g_string_new("");
	g_string_sprintf(strbuf,
			 "<HTML><body text=\"#151515\" link=\"#898989\">");
	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);

	//g_warning(settings->default_font);
	for (key->Verse(1);
	     (key->Book() == curBook && key->Chapter() == curChapter
	      && !imodule.Error()); imodule++) {
		/* verse number */
		strbuf = g_string_new("");
		if (key->Verse() == curVerse) {
			g_string_sprintf(strbuf,
					 "&nbsp;&nbsp;<A HREF=\"*[%s] %s\" NAME=\"%d\"><FONT COLOR=\"#000FCF\"><B>%d</B></font></A> ",
					 imodule.Description(),
					 imodule.KeyText(), key->Verse(),
					 key->Verse());
		} else {
			g_string_sprintf(strbuf,
					 "&nbsp;&nbsp;<A HREF=\"*[%s] %s\" NAME=\"%d\"><FONT COLOR=\"#000FCF\"><B>%d</B></font></A> ",
					 imodule.Description(),
					 imodule.KeyText(), key->Verse(),
					 key->Verse());
		}
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
		if (key->Verse() == curVerse) {
			strbuf = g_string_new("");
		    if (!stricmp(lang.c_str(), "")  || !stricmp(lang.c_str(), "en")) {
			if (!stricmp(font.c_str(), "Symbol")) {
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\" FACE=\"symbol\">", 
						 mycolor);
			} else if (!stricmp(font.c_str(), "greek1")) {
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\" FACE=\"greek1\">",
						 mycolor);
			} else if (!stricmp(lang.c_str(), "en")) {
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\" FACE=\"helvetica\">",
						 mycolor);
			} else if (!stricmp(font.c_str(), "BSTHebrew")) {
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\" FACE=\"bsthebrew\">",
						 mycolor);
			} else {
				if (settings->versestyle && newparagraph) {
					g_string_sprintf(strbuf,
							 "<FONT COLOR=\"%s\">%c",
							 mycolor, 182);
					newparagraph = false;
				} else
					g_string_sprintf(strbuf,
							 "<FONT COLOR=\"%s\">",
							 mycolor);
			}
		   }else{
			   	strbuf = g_string_new("");
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\" FACE=\"%s\">",
						 mycolor,settings->default_font);
		   }
			if (newparagraph) {
				newparagraph = false;
			}
			strbuf =
			    g_string_append(strbuf,
					    (const char *) imodule);
			if (settings->versestyle) {
				if (strstr(strbuf->str, "<BR>") == NULL
				    && strstr(strbuf->str, "<P>") == NULL)
					strbuf =
					    g_string_append(strbuf,
							    "</font><br>");
				else if (strstr(strbuf->str, "<P>") !=
					 NULL) {
					mybuf = strstr(strbuf->str, "<P>");
					mybuflen = strlen(mybuf);
					mybuflen = strbuf->len - mybuflen;
					strbuf =
					    g_string_truncate(strbuf,
							      mybuflen);
					g_string_append(strbuf,
							"</font><br>");
					newparagraph = true;
				} else
					strbuf =
					    g_string_append(strbuf,
							    "</font>");
			} else
				strbuf =
				    g_string_append(strbuf, "</font>");
			displayHTML(GTK_WIDGET(gtkText), strbuf->str,
				    strbuf->len);
			g_string_free(strbuf, TRUE);
		} else {
		    if (!stricmp(lang.c_str(), "")  || !stricmp(lang.c_str(), "en")) {
			if (!stricmp(font.c_str(), "Symbol")) {
				strbuf = g_string_new("<FONT COLOR=\"#000000\" FONT FACE=\"symbol\">");	/* we had to add font color to get the symbol font to work */
			} else if (!stricmp(font.c_str(), "greek1")) {
				strbuf =
				    g_string_new
				    ("<FONT SIZE=\"+5\"  COLOR=\"#000000\" FONT FACE=\"greek1\">");
			} else if (!stricmp(lang.c_str(), "en")) {
				strbuf =
				    g_string_new("<FONT COLOR=\"#000000\" FACE=\"helvetica\">");
			} else if (!stricmp(font.c_str(), "BSTHebrew")) {
				strbuf =
				    g_string_new
				    ("<FONT COLOR=\"#000000\" FONT FACE=\"bsthebrew\">");
			} else {
				strbuf = g_string_new("");
				if (settings->versestyle && newparagraph) {
					g_string_sprintf(strbuf,
							 "<FONT COLOR=\"#000000\" >%c",
							 c);
					newparagraph = false;
				} else
					g_string_sprintf(strbuf,
							 "<FONT COLOR=\"#000000\" >");
			}
		}else{
			strbuf = g_string_new("");
			g_string_sprintf(strbuf, "<FONT COLOR=\"#000000\" FACE=\"%s\">",settings->default_font);
		} 
			strbuf =
			    g_string_append(strbuf,
					    (const char *) imodule);
			if (settings->versestyle) {
				if (strstr(strbuf->str, "<BR>") == NULL
				    && strstr(strbuf->str, "<P>") == NULL)
					strbuf =
					    g_string_append(strbuf,
							    "</font><br>");
				else if (strstr(strbuf->str, "<P>") !=
					 NULL) {
					mybuf = strstr(strbuf->str, "<P>");
					mybuflen = strlen(mybuf);
					mybuflen = strbuf->len - mybuflen;
					strbuf =
					    g_string_truncate(strbuf,
							      mybuflen);
					g_string_append(strbuf,
							"</font><br>");
					newparagraph = true;
				} else
					strbuf =
					    g_string_append(strbuf,
							    "</font>");
			} else
				strbuf =
				    g_string_append(strbuf, "</font>");
			displayHTML(GTK_WIDGET(gtkText), strbuf->str,
				    strbuf->len);
			g_string_free(strbuf, TRUE);
		}
	}
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	strbuf = g_string_new("</body></html>");
	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);
	sprintf(tmpBuf, "%d", curVerse);
	endHTML(GTK_WIDGET(gtkText));
	gotoanchorHTML(gtkText, tmpBuf);
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
	gchar *utf8str, *use_font, *font, *token;
	gint mybuflen, utf8len;
	const gchar **end;

	string lang;
	SWMgr *Mgr;
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;
	//c = 182;  
 	Mgr = new SWMgr();	//-- create sword mgr
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(settings->app, "nbText")), 1);
	
	if ((sit = Mgr->config->Sections.find(imodule.Name())) != Mgr->config->Sections.end()) {
		ConfigEntMap &section = (*sit).second;
		//font = ((entry = section.find("Font")) != section.end()) ? (*entry).second : (string) "";
		lang = ((entry = section.find("Lang")) != section.end()) ? (*entry).second : (string) "";
	} 
	font = "-adobe-helvetica-*-*";
	if (!stricmp(lang.c_str(), "") || 
				!stricmp(lang.c_str(), "en") || 
					!stricmp(lang.c_str(), "de" )) {
		font = g_strdup(settings->default_font);		
	}else if (!stricmp(lang.c_str(), "grc")) {
		font = g_strdup(settings->greek_font);		
	}else if (!stricmp(lang.c_str(), "he")) {
		font = g_strdup(settings->hebrew_font);		
	}else{
		font = g_strdup(settings->unicode_font);		
	}
	++font;
	token=strtok(font,"-");
	use_font = token;
	use_font = strtok(NULL,"-");
	//g_warning("use_font = %s",use_font);
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

		sprintf(tmpBuf, "<font face=\"%s\" color=\"%s\" size=\"%s\">",
			use_font, versecolor, settings->bible_font_size);
		utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

		displayHTML(GTK_WIDGET(gtkText), (const char *) imodule,
			    strlen((const char *) imodule));
		if (settings->versestyle) {
			if(strstr((const char *) imodule, "<BR>") == NULL){
				sprintf(tmpBuf, " %s", "</font><br>");
			}else{
				sprintf(tmpBuf, " %s", "</font>");
				//g_warning(tmpBuf);
			}
		} else
			sprintf(tmpBuf, " %s", "</font>");
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
	//g_free(font);
	delete Mgr;
	return 0;
}

/* --------------------------------------------------------------------------------------------- */
char InterlinearDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[800], *buf;
	gint i;
	bool utf = false;
	gint len;
	gchar *utf8str, *use_font, *font, *token;
	gint utf8len;
	string lang;
	SWMgr *Mgr;
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;
	
	Mgr = new SWMgr();	//-- create sword mgr
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(settings->app, "nbText")), 1);
	
	if ((sit = Mgr->config->Sections.find(imodule.Name())) != Mgr->config->Sections.end()) {
		ConfigEntMap &section = (*sit).second;
		lang = ((entry = section.find("Lang")) != section.end()) ? (*entry).second : (string) "";
	} 
	font = "-adobe-helvetica-*-*";
	if (!stricmp(lang.c_str(), "") || 
				!stricmp(lang.c_str(), "en") || 
					!stricmp(lang.c_str(), "en" )) {
		font = g_strdup(settings->default_font);		
	}else if (!stricmp(lang.c_str(), "grc")) {
		font = g_strdup(settings->greek_font);		
	}else if (!stricmp(lang.c_str(), "he")) {
		font = g_strdup(settings->hebrew_font);		
	}else{
		font = g_strdup(settings->unicode_font);		
	}
	++font;
	token=strtok(font,"-");
	use_font = token;
	use_font = strtok(NULL,"-");
	//g_warning("use_font = %s",use_font);
	

	//buf = (char *) imodule.Description(); 
	(const char *) imodule;
	sprintf(tmpBuf,
		"<B><A HREF=\"[%s]%s\"><FONT COLOR=\"%s\" SIZE=\"%s\"> [%s]</font></a></b><FONT COLOR=\"%s\">[%s] </font>",
		imodule.Name(),
		imodule.Description(),
		settings->bible_verse_num_color,
		settings->bible_font_size,
		imodule.Name(), 
		settings->bible_verse_num_color,
		imodule.KeyText());
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

	sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">",
		use_font, settings->interlinear_font_size);
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

	displayHTML(GTK_WIDGET(gtkText), (const char *) imodule,
		    strlen((const char *) imodule));

	sprintf(tmpBuf,
		" [<A HREF=\"@%s\">view context</a>]</font><br><hr>",
		imodule.Name());
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	displayHTML(GTK_WIDGET(gtkText), utf8str, strlen(utf8str));
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
