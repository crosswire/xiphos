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

/*********************************************************************
**********************************************************************
**  this code was taken from the Sword Cheatah program 	    **
**  and modified to handle some of the GBF stuff. Also added**
**  suport for the x symbol font when using greek modules.    **
**  2000/07/10 - added some support for the RWP module    **
**  2001-02-25 added support for html                              ** 
**********************************************************************
*********************************************************************/

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
#include <thmlgbf.h>
#include <gbfplain.h>

#include "gs_display.h"
#include "support.h"
#include "interface.h"
#include "gs_sword.h"
#include "gs_html.h"
#include "gs_gnomesword.h"

/****************************************************************************************
 * externs
 ****************************************************************************************/
extern SWMgr *mainMgr;
extern SWMgr *mainMgr1;
extern bool bVerseStyle;
extern GtkWidget *MainFrm;	/* pointer to app -- declared in GnomeSword.cpp */
extern SWModule *comp1Mod;
extern gchar *current_verse;
extern INTERLINEAR interlinearMods;
extern gchar *mycolor;

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

	font = "Roman";
	buf = (char *) imodule.Description();
	if ((sit = mainMgr1->config->Sections.find(imodule.Name())) !=
	    mainMgr1->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
	}
/*	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbCom")), 0); */
	(const char *) imodule;	/* snap to entry */
	/* check for personal comments by finding ModDrv=RawFiles */
	if (((*mainMgr->config->Sections[imodule.Name()].find("ModDrv")).second == "RawFiles") &&	
	    		(GTK_TOGGLE_BUTTON(lookup_widget(MainFrm, "btnEditNote"))->active)) {	/* check for edit mode */
		GtkWidget *statusbar;	/* pointer to comments statusbar */
		gint context_id2;	/* statusbar context_id ??? */
		/* add module name and verse to edit note statusbar */		
		sprintf(tmpBuf, "[%s] ", imodule.KeyText());
		/* setup statusbar for personal comments */
		/*        get stutusbar */		
		statusbar = lookup_widget(MainFrm, "sbNotes");
		/* get context id */		
		context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "GnomeSword");
		/* ready status */
		gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);	
		/* show modName and verse ref in statusbar */		
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2, tmpBuf);
		beginHTML(GTK_WIDGET(gtkText));	
	} else {
		strbuf = g_string_new("<B><FONT COLOR=\"#000FCF\">");
		sprintf(tmpBuf, "<A HREF=\"[%s]%s\"> [%s]</a>[%s] </b>",
			imodule.Name(), buf, imodule.Name(), imodule.KeyText());
		strbuf = g_string_append(strbuf, tmpBuf);
		/* show verse ref in text widget  */
		/* show module text for current key */
		beginHTML(GTK_WIDGET(gtkText));
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	}
	if (!stricmp(font, "Symbol")) {
		strbuf = g_string_new("<FONT FACE=\"symbol\">");
		strbuf = g_string_append(strbuf, (const char *) imodule);
		strbuf = g_string_append(strbuf, "</font>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else if (!stricmp(font, "Greek")) {
		strbuf = g_string_new("<FONT FACE=\"SIL Galatia\">");
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
	return 0;
}


/***************************************************************************** 
 * GtkHTMLEntryDisp - for displaying dict/lex modules in a GtkHTML 
 * widget the mods need to be filtered to html first
 *****************************************************************************/
char GtkHTMLEntryDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[255], *font, *buf;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	GString *strbuf;

	font = "Roman";
	buf = (char *) imodule.Description();
	if ((sit = mainMgr1->config->Sections.find(imodule.Name())) !=
	    mainMgr1->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
	}
/*	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbCom")), 0);  */
	(const char *) imodule;	/* snap to entry */
	beginHTML(GTK_WIDGET(gtkText));
	strbuf = g_string_new("");
	/* show verse ref in text widget  */
	g_string_sprintf(strbuf,
			 "<B><FONT COLOR=\"#000FCF\"><A HREF=\"[%s]%s\"> [%s]</a>[%s] </b>",
			 imodule.Name(), buf, imodule.Name(),
			 imodule.KeyText());
	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);
	/* show module text for current key */
	if (!stricmp(font, "Symbol")) {	/* greek symbol font */
		strbuf = g_string_new("<FONT FACE=\"symbol\">");
		strbuf = g_string_append(strbuf, (const char *) imodule);
		strbuf = g_string_append(strbuf, "</font>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else if (!stricmp(font, "Greek")) {	/* greek -wingreek */
		strbuf =
		    g_string_new
		    ("<I> </I><FONT COLOR=\"#000000\" FACE=\"SIL Galatia\">");
		strbuf = g_string_append(strbuf, (const char *) imodule);
		strbuf = g_string_append(strbuf, "</font><I> </I>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else if (!stricmp(font, "BSTHebrew")) {	/* hebrew -wingreek */
		strbuf =
		    g_string_new
		    ("<I> </I><FONT COLOR=\"#000000\" FACE=\"hebrew1\">");
		strbuf = g_string_append(strbuf, (const char *) imodule);
		strbuf = g_string_append(strbuf, "</font><I> </I>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else {		/*  */
		strbuf = g_string_new((const char *) imodule);
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	}
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
	char tmpBuf[500], *buf, *font, *mybuf;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	//ConfigEntMap::iterator cit;	//-- iteratior
	GString *strbuf;
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gint len;
	gchar *sourceformat;
	bool gbf = false;
	char *Buf, c;
	bool newparagraph = false;
	gint mybuflen;

	c = 182;
	font = "Roman";
	sourceformat = "plain";

	if ((sit = mainMgr->config->Sections.find(imodule.Name())) !=
	    mainMgr->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
		eit = (*sit).second.find("SourceType");
		if (eit != (*sit).second.end())
			sourceformat = (char *) (*eit).second.c_str();
		if (!stricmp(sourceformat, "GBF"))
			gbf = true;
	}
	beginHTML(GTK_WIDGET(gtkText));
	strbuf =
	    g_string_new
	    ("<HTML><BODY><body text=\"#151515\" link=\"#898989\">");
	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);

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
			if (!stricmp(font, "Symbol")) {
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\" FACE=\"symbol\">",
						 mycolor);
			} else if (!stricmp(font, "Greek")) {
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\" FACE=\"SIL Galatia\">",
						 mycolor);
			} else {
				if (bVerseStyle && newparagraph) {				
					g_string_sprintf(strbuf, "<FONT COLOR=\"%s\">%c", mycolor,182);				
					newparagraph = false;
				} else
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\">",
						 mycolor);
			}
			if (newparagraph) {			
				newparagraph = false;
			} 	
			strbuf = g_string_append(strbuf, (const char *)imodule);
			if (bVerseStyle) {
				if (strstr(strbuf->str, "<BR>") == NULL
				    && strstr(strbuf->str, "<P>") == NULL)
					strbuf =
					    g_string_append(strbuf,
							    "</font><br>");
				else if(strstr(strbuf->str, "<P>") != NULL) {
					mybuf = strstr(strbuf->str, "<P>");
					mybuflen = strlen(mybuf);
					mybuflen =strbuf->len - mybuflen; 
					strbuf = g_string_truncate(strbuf,mybuflen);
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
			if (!stricmp(font, "Symbol")) {
				strbuf = g_string_new("<FONT COLOR=\"#000000\" FONT FACE=\"symbol\">");	/* we had to add font color to get the symbol font to work */				
			} else if (!stricmp(font, "Greek")) {
				strbuf = g_string_new("<FONT COLOR=\"#000000\" FONT FACE=\"SIL Galatia\">");					
			} else {
				strbuf = g_string_new("");
				if (bVerseStyle && newparagraph) {
					g_string_sprintf(strbuf, "<FONT COLOR=\"#000000\" >%c",c);
					newparagraph = false;
				} else
					g_string_sprintf(strbuf, "<FONT COLOR=\"#000000\" >");
				strbuf = g_string_append(strbuf, (const char *)imodule);
			}
			if (bVerseStyle) {
				if (strstr(strbuf->str, "<BR>") == NULL
					    && strstr(strbuf->str, "<P>") == NULL)
						strbuf = g_string_append(strbuf, "</font><br>");
				else if(strstr(strbuf->str, "<P>") != NULL) {
						mybuf = strstr(strbuf->str, "<P>");
						mybuflen = strlen(mybuf);
						mybuflen =strbuf->len - mybuflen; 
						strbuf = g_string_truncate(strbuf,mybuflen);
					    	g_string_append(strbuf, "</font><br>");
						newparagraph = true;
				} else
					strbuf = g_string_append(strbuf, "</font>"); 
			} else 
				strbuf = g_string_append(strbuf, "</font>");	
			displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
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
	gotoanchorHTML(tmpBuf);
	return 0;
}


/* --------------------------------------------------------------------------------------------- */
char InterlinearDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[800], *font, *buf;
	gint i;
	//GtkWidget *gtkText;
	ModMap::iterator it;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	GString *strbuf;
	bool gbf = false;
	gint len;
	gchar *sourceformat;
	char *Buf;

	font = "Roman";
	buf = (char *) imodule.Description();
	if ((sit = mainMgr1->config->Sections.find(comp1Mod->Name())) !=
	    mainMgr1->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
		eit = (*sit).second.find("SourceType");
		if (eit != (*sit).second.end())
			sourceformat = (char *) (*eit).second.c_str();
		if (!stricmp(sourceformat, "GBF"))
			gbf = true;
	}
	(const char *) imodule;
	strbuf = g_string_new("<B><FONT COLOR=\"#000FCF\">");
	sprintf(tmpBuf, "<A HREF=\"[%s]%s\"> [%s]</a>[%s] </font></b>",
		imodule.Name(), buf, imodule.Name(), imodule.KeyText());
	strbuf = g_string_append(strbuf, tmpBuf);

	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);
	/* heading */
	if (!stricmp(font, "Symbol")) {
		strbuf = g_string_new("<FONT FACE=\"symbol\">");
	} else if (!stricmp(font, "Greek")) {
		strbuf = g_string_new("<B> </B><FONT FACE=\"SIL Galatia\">");
	} else if (!stricmp(font, "BSTHebrew")) {
		strbuf = g_string_new("<B> </B><FONT FACE=\"bsthebrew\">");
	} else {
		strbuf = g_string_new("<FONT COLOR=\"#000000\" >");
	}
	/* body */
	/*
	if (gbf) {
		len = strlen((const char *) imodule);
		len = len * 5;
		Buf = new char[len];
		strcpy(Buf,
	       (const char *) imodule);
		gbftohtml(Buf, len);
		strbuf = g_string_append(strbuf, Buf);
		delete[]Buf;
		Buf = NULL;		
	} else  */
		strbuf = g_string_append(strbuf, (const char *) imodule);
	/* closing */
	if (!stricmp(font, "Symbol")) {
		strbuf = g_string_append(strbuf, "</font><BR><HR>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else if (!stricmp(font, "Greek")) {
		strbuf = g_string_append(strbuf, "</font><BR><HR>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else if (!stricmp(font, "BSTHebrew")) {
		strbuf = g_string_append(strbuf, "</font><BR><HR>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else {
		strbuf = g_string_append(strbuf, "<BR><HR>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	}
	return 0;
}


/********************************************************** 
* most of the code in this function is from the 
* Sword gbfhtml.cpp file
***********************************************************/
gchar *GtkHTMLEntryDisp::gbftohtml(gchar * text, gint maxlen)
{
	gchar *str, *to, *from, token[2048];
	gint len;
	gint tokpos = 0;
	bool intoken = false;
	bool hasFootnotePreTag = false;
	bool isRightJustified = false;
	bool isCentered = false;
	static bool newParagraph = false;

	len = strlen(text) + 1;
	if (len < maxlen) {
		memmove(&text[maxlen - len], text, len);
		from = &text[maxlen - len];
	} else
		from = text;
	for (to = text; *from; from++) {
		if (newParagraph) {
			*to++ = 182;
			newParagraph = false;
		}
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
					strcpy(to, " <A HREF=\"#");
					to += strlen(to);
					for (unsigned int i = 2;
					     i < strlen(token); i++)
						*to++ = token[i];
					strcpy(to,
					       " \"><FONT SIZE=\"-1\">");
					to += strlen(to);
					for (unsigned int i = 2;
					     i < strlen(token); i++)
						*to++ = token[i];
					strcpy(to, "</FONT></A> ");
					to += strlen(to);
					continue;

				case 'T':	// Tense
					strcpy(to, " <A HREF=\"#");
					to += strlen(to);
					for (unsigned int i = 3;
					     i < strlen(token); i++)
						*to++ = token[i];
					strcpy(to,
					       "\"> <FONT SIZE=\"-1\"><I>");
					to += strlen(to);
					for (unsigned int i = 3;
					     i < strlen(token); i++)
						*to++ = token[i];
					strcpy(to, "</I></FONT></A> ");
					to += strlen(to);
					continue;
				}
				break;
			case 'R':
				switch (token[1]) {
				case 'B':	//word(s) explained in footnote
					strcpy(to, "<I>");
					to += strlen(to);
					hasFootnotePreTag = true;	//we have the RB tag
					continue;
				case 'F':	// footnote begin
					if (hasFootnotePreTag) {
						strcpy(to, "</I>");
						to += strlen(to);
					}
					strcpy(to,
					       "<FONT COLOR=\"800000\"><SMALL>(");
					to += strlen(to);
					continue;
				case 'f':	// footnote end
					strcpy(to, ")</SMALL></FONT>");
					to += strlen(to);
					hasFootnotePreTag = false;
					continue;
				}
				break;
			case 'F':	// font tags
				switch (token[1]) {
				case 'I':	// italic start
					strcpy(to, "<I>");
					to += strlen(to);
					continue;
				case 'i':	// italic end
					strcpy(to, "</I>");
					to += strlen(to);
					continue;
				case 'B':	// bold start
					strcpy(to, "<B>");
					to += strlen(to);
					continue;
				case 'b':	// bold end
					strcpy(to, "</B>");
					to += strlen(to);
					continue;
				case 'R':	// words of Jesus begin
					strcpy(to,
					       "<FONT COLOR=\"FF0000\">");
					to += strlen(to);
					continue;
				case 'r':	// words of Jesus end
					strcpy(to, "</FONT>");
					to += strlen(to);
					continue;
				case 'U':	// Underline start
					strcpy(to, "<U>");
					to += strlen(to);
					continue;
				case 'u':	// Underline end
					strcpy(to, "</U>");
					to += strlen(to);
					continue;
				case 'O':	// Old Testament quote begin
					strcpy(to, "<CITE>");
					to += strlen(to);
					continue;
				case 'o':	// Old Testament quote end
					strcpy(to, "</CITE>");
					to += strlen(to);
					continue;
				case 'S':	// Superscript begin
					strcpy(to, "<SUP>");
					to += strlen(to);
					continue;
				case 's':	// Superscript end
					strcpy(to, "</SUP>");
					to += strlen(to);
					continue;
				case 'V':	// Subscript begin
					strcpy(to, "<SUB>");
					to += strlen(to);
					continue;
				case 'v':	// Subscript end
					strcpy(to, "</SUB>");
					to += strlen(to);
					continue;
				}
				break;
			case 'C':	// special character 
				switch (token[1]) {
				case 'A':	// ASCII value
					*to++ = (char) atoi(&token[2]);
					continue;
				case 'G':
					//*to++ = ' ';
					continue;
				case 'L':	// line break
					if (!bVerseStyle) {
						strcpy(to, "<BR>");
						to += strlen(to);
					}
					continue;
				case 'M':	// new paragraph
					if (bVerseStyle) {
						newParagraph = TRUE;
					} else {
						strcpy(to, "<P>");
						to += strlen(to);
					}
					continue;
				case 'T':
					//*to++ = ' ';
					continue;
				}
				break;
			case 'J':	//Justification
				switch (token[1]) {
				case 'R':	//right
					strcpy(to,
					       "<DIV ALIGN=\"RIGHT\">");
					to += strlen(to);
					isRightJustified = true;
					continue;
				case 'C':	//center
					strcpy(to,
					       "<DIV ALIGN=\"CENTER\">");
					to += strlen(to);
					isCentered = true;
					continue;

				case 'L':	//left, reset right and center
					if (isCentered) {
						strcpy(to, "</CENTER>");
						to += strlen(to);
						isCentered = false;
					}
					if (isRightJustified) {
						strcpy(to, "</DIV>");
						to += strlen(to);
						isRightJustified = false;
					}
					continue;
				}
				break;
			case 'T':	// title formatting
				switch (token[1]) {
				case 'T':	// Book title begin
					strcpy(to, "<BIG>");
					to += strlen(to);
					continue;
				case 't':
					strcpy(to, "</BIG>");
					to += strlen(to);
					continue;
				}
				break;

			case 'P':	// special formatting
				switch (token[1]) {
				case 'P':	// Poetry begin
					strcpy(to, "<CITE>");
					to += strlen(to);
					continue;
				case 'p':
					strcpy(to, "</CITE>");
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
