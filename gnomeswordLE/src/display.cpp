/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          display.cpp  -  description
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
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
**  this code was taken from the Sword Cheatah program 	               **
**  and modified to handle some of the GBF stuff. Also added            **
**  suport for the x symbol font when using greek modules.               **
**  2000/07/10 - added some support for the RWP module               **
**  2001-02-25 added support for html                                               ** 
*********************************************************************
*********************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <swmodule.h>
#include <swmgr.h>
#include <versekey.h>
#include <thmlgbf.h>
#include <gbfplain.h>

#include "display.h"
#include "support.h"
#include "interface.h"
#include "gs_sword.h"
#include "gs_gnomesword.h"
GdkColor GTKEntryDisp::colourBlue;
GdkColor GTKEntryDisp::colourGreen;
GdkColor GTKEntryDisp::colourRed;
GdkColor GTKEntryDisp::colourCur;

GdkColor myGreen;
GdkColor BGcolor;
GdkFont *roman_font, *italic_font, *versenum_font, *bold_font;
gchar *mycolor;

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
extern bool bVerseStyle;
extern GtkWidget *MainFrm;	/* pointer to app -- declared in GnomeSword.cpp */
extern SWModule *comp1Mod;
extern gchar *current_verse;
extern INTERLINEAR interlinearMods;


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

	/*gtk_notebook_set_page(GTK_NOTEBOOK
	   (lookup_widget(MainFrm, "nbCom")), 1); */

	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete(GTK_TEXT(gtkText),
				gtk_text_get_length((GTK_TEXT(gtkText))));

	int curPos = 0;
	(const char *) imodule;	/* snap to entry */
	gtk_text_freeze(GTK_TEXT(gtkText));
	/* let's find out if we have a comment or dict module */
	it = mainMgr->Modules.find(imodule.Name());
	sprintf(tmpBuf, "[%s] ", imodule.KeyText());
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

/*****************************************************************************
* For displaying text mods on the interlinear page in gtktext widgets
* this will handle some of the GBF format 
* imodule - Sword module to work with
******************************************************************************/
char GTKInterlinearDisp::Display(SWModule & imodule)
{
	char tmpBuf[800];
	GdkFont *sword_font, *greek_font;
	gchar *myname;
	bool greek, findclose, italics_on = FALSE;
	char verseBuf[800], buf[800];
	char *myverse, *font, *sourceType;
	int i, j, len;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;


	font = "Roman";
	if ((sit = mainMgr1->config->Sections.find(imodule.Name())) !=
	    mainMgr1->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
	}

	/* Load a italic font */
	italic_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a bold font */
	bold_font =
	    gdk_font_load
	    ("-adobe-helvetica-bold-r-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a roman font */
	roman_font = gdk_font_load(font_interlinear);
	/* Load a verse number font */
	versenum_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
	/* Load a greek font */
	greek_font =
	    gdk_font_load
	    ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");

	/********************* check for greek module that use symbol font */
	greek = FALSE;

	if (!strcmp(font, "Symbol")) {
		greek = TRUE;
		sword_font = greek_font;
	} else if (!strcmp(font, "Greek")) {
		sword_font = gdk_font_load
		    ("-unknown-greekregular-medium-r-normal--15-140-75-75-p-74-adobe-fontspecific");
		if (sword_font == NULL)
			sword_font = greek_font;
		greek = TRUE;
	} else {
		sword_font = roman_font;
		greek = FALSE;
	}
	findclose = FALSE;
	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete(GTK_TEXT(gtkText),
				gtk_text_get_length((GTK_TEXT(gtkText))));
	int curPos = 0;
	(const char *) imodule;	/* snap to entry */
	gtk_text_freeze(GTK_TEXT(gtkText));
	sprintf(tmpBuf, "[%s][ %s] ", imodule.Name(), imodule.KeyText());
	gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL, tmpBuf,
			-1);

	if (greek)
		gtk_text_insert(GTK_TEXT(gtkText), sword_font,
				&gtkText->style->black, NULL,
				(const char *) imodule, -1);
	else {
		i = j = 0;
		verseBuf[0] = '\0';
		myverse = g_strdup((const char *) imodule);
		len = strlen(myverse);
		while (i < len) {
			if (myverse[i] == '<' && myverse[i + 1] == 'F'
			    && myverse[i + 2] == 'I') {
				i = i + 4;
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'F'
			    && myverse[i + 2] == 'i') {
				i = i + 4;
				gtk_text_insert(GTK_TEXT(gtkText),
						italic_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'P'
			    && myverse[i + 2] == 'P') {
				i = i + 4;
				sprintf(buf, "%s\n\n", verseBuf);
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, buf, -1);
				j = 0;
				verseBuf[0] = '\0';
				italics_on = TRUE;
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'P'
			    && myverse[i + 2] == 'p') {
				i = i + 4;
				sprintf(buf, "%s\n", verseBuf);
				gtk_text_insert(GTK_TEXT(gtkText),
						italic_font,
						&gtkText->style->black,
						NULL, buf, -1);
				j = 0;
				verseBuf[0] = '\0';
				italics_on = FALSE;
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'F'
			    && myverse[i + 2] == 'B') {
				i = i + 4;
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
				myverse[i] = '[';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'F'
			    && myverse[i + 2] == 'b') {
				i = i + 4;
				gtk_text_insert(GTK_TEXT(gtkText),
						bold_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
				myverse[i] = ']';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'C') {
				while (myverse[i] != '>') {
					++i;
				}
				++i;
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'R'
			    && myverse[i + 2] == 'F') {
				i = i + 3;
				myverse[i] = '{';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'R'
			    && myverse[i + 2] == 'f') {
				i = i + 3;
				myverse[i] = '}';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'R'
			    && myverse[i + 2] == 'B') {
				i = i + 3;
				myverse[i] = '*';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'W'
			    && myverse[i + 2] == 'G') {
				i = i + 2;
				myverse[i] = '<';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'W'
			    && myverse[i + 2] == 'H') {
				i = i + 2;
				myverse[i] = '<';
			}
			if (myverse[i] == '<' && myverse[i + 1] == 'W'
			    && myverse[i + 2] == 'T') {
				i = i + 3;
				myverse[i] = '(';
				findclose = TRUE;
			}
			if (findclose) {
				if (myverse[i] == '>') {
					myverse[i] = ')';
					findclose = FALSE;
				}
			}
			verseBuf[j] = myverse[i];
			++i;
			verseBuf[j + 1] = '\0';
			++j;
		}
		if (italics_on)
			sword_font = italic_font;
		else
			sword_font = roman_font;
		gtk_text_insert(GTK_TEXT(gtkText), sword_font,
				&gtkText->style->black, NULL, verseBuf,
				-1);
		verseBuf[0] = '\0';
		g_free(myverse);
	}
	/* --------------------------------------------------------------------- */
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
	return 0;
}

/*****************************************************************************
* original chapter display form cheatah - no text formating
* imodule - text to display in gtktext widget
******************************************************************************/
char GTKChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[255];

	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete(GTK_TEXT(gtkText),
				gtk_text_get_length((GTK_TEXT(gtkText))));
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	gtk_text_freeze(GTK_TEXT(gtkText));
	for (key->Verse(1);
	     (key->Book() == curBook && key->Chapter() == curChapter
	      && !imodule.Error()); imodule++) {
		sprintf(tmpBuf, "%d. ", key->Verse());
		gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL,
				tmpBuf, -1);
		if (key->Verse() == curVerse) {
			gtk_text_thaw(GTK_TEXT(gtkText));
			adjVal = GTK_TEXT(gtkText)->vadj->upper;
			curPos = gtk_text_get_length(GTK_TEXT(gtkText));
			gtk_text_insert(GTK_TEXT(gtkText), NULL,
					&colourGreen, NULL,
					(const char *) imodule, -1);
			gtk_text_freeze(GTK_TEXT(gtkText));
		} else {
			gtk_text_insert(GTK_TEXT(gtkText), NULL,
					&gtkText->style->black, NULL,
					(const char *) imodule, -1);
		}
		gtk_text_insert(GTK_TEXT(gtkText), NULL,
				&gtkText->style->black, NULL, "  ", -1);
	}
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
	adjVal -= (GTK_TEXT(gtkText)->vadj->page_size / 2);
	gtk_adjustment_set_value(GTK_TEXT(gtkText)->vadj,
				 (adjVal > 0) ? adjVal : 0);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	return 0;
}

/***************************************************************************** 
* this will handle some of the html formatting for displaying comm mods
* imodule - text format and display for gtktext widget
******************************************************************************/
char HTMLentryDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[256];
	GdkFont *sword_font, *greek_font, *foreign_font;
	gchar *myname;
	bool greek, greek_on, findclose, italics_on = FALSE, scriptureref =
	    FALSE, fontsize = FALSE;
	gchar *verseBuf, *buf, *myverse, *font, *sourceType, tag[256];
	int i, j, len, taglen;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;

	font = "Roman";

	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbCom")), 1);

	if ((sit = mainMgr->config->Sections.find(imodule.Name())) !=
	    mainMgr->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
	}
	//cout << gtk_widget_get_name(gtkText) << '\n';
	/* Load a italic font */
	italic_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a bold font */
	bold_font =
	    gdk_font_load
	    ("-adobe-helvetica-bold-r-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a roman font */
	roman_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a verse number font */
	versenum_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
	/* Load a greek font  (symbol) */
	greek_font =
	    gdk_font_load
	    ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");

	if (!strcmp(font, "Symbol")) {
		greek_on = true;
	} else if (!strcmp(font, "Greek")) {
		greek_on = true;
	} else {
		greek_on = false;
	}

	sword_font = roman_font;
	findclose = FALSE;
	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete(GTK_TEXT(gtkText),
				gtk_text_get_length((GTK_TEXT(gtkText))));
	int curPos = 0;
	(const char *) imodule;	/* snap to entry */
	gtk_text_freeze(GTK_TEXT(gtkText));
	sprintf(tmpBuf, "[%s] ", imodule.KeyText());
	gtk_text_insert(GTK_TEXT(gtkText),
			NULL, &colourBlue, NULL, tmpBuf, -1);
	i = j = 0;
	len = strlen((const char *) imodule);
	myverse = new char[len + 1];
	myverse[0] = '\0';
	verseBuf = new char[len + 1];
	verseBuf[0] = '\0';
	if ((!strcmp(imodule.Name(), "TFG"))
	    && (!strcmp(imodule.KeyText(), "Romans 1:1"))) {	/* gnomeswrod will crash in TFG mod at Rom 1:1 */
		sprintf(myverse, "%s", " ");
	} else {
		sprintf(myverse, "%s", (const char *) imodule);
		if (greek_on) {
			gtk_text_insert(GTK_TEXT(gtkText), greek_font,
					&gtkText->style->black, NULL,
					myverse, -1);
			delete[]verseBuf;
			verseBuf = NULL;
			delete[]myverse;
			myverse = NULL;
			gtk_text_set_point(GTK_TEXT(gtkText), curPos);
			gtk_text_thaw(GTK_TEXT(gtkText));
			return 't';
		}
	}
	while (i < len) {
		if (myverse[i] == '\n')
			myverse[i] = ' ';
		if ((myverse[i] == '&') && (myverse[i + 1] == 'n')
		    && (myverse[i + 2] == 'b') && (myverse[i + 3] == 's')
		    && (myverse[i + 4] == 'p') && (myverse[i + 5] == ';')) {
			i = i + 6;
		}
		//g_warning("%c",myverse[i]);
		if (myverse[i] == '<') {
			tag[0] = '\0';
			taglen = gettags(myverse, tag, i);	/* get html tags */
			i = i + taglen;	/* remove tags (we do not want to see them) */
			/* italic */
			if (!strcmp(tag, "<I>")
			    || !strcmp(tag, "<TR><TD><FONT SIZE=+1><I>")) {
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
			} else if (!strncmp(tag, "</I>", 4)) {
				gtk_text_insert(GTK_TEXT(gtkText),
						italic_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
			} else if (!strcmp(tag, "<U>")) {	/* ------------- underline ???? */
				i = i + 3;
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
			} else if (!strcmp(tag, "</U>")) {
				i = i + 4;
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
			} else if ((!strncmp(tag, "<A HREF", 6))
				   || (!strncmp(tag, "<SCRIPREF", 9))) {	/*  reference */
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				scriptureref = TRUE;
				j = 0;
				verseBuf[0] = '\0';
			} else if ((!strcmp(tag, "</A>"))
				   || (!strcmp(tag, "</SCRIPREF>"))) {	/*  end reference */
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font, &colourRed,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
				scriptureref = FALSE;
			} else
			    if (!strncmp
				(tag, "<TD ALIGN=RIGHT><FONT SIZE=", 26)) {
				fontsize = TRUE;
			} else
			    if (!strncmp(tag, "<TR><TD><FONT SIZE=", 18)) {
				fontsize = TRUE;
			} else if (!strncmp(tag, "<FONT SIZE=", 10)) {
				fontsize = TRUE;
			} else if (!strcmp(tag, "<FONT FACE=\"SYMBOL\">")) {	/*  greek */
				foreign_font = greek_font;
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
				fontsize = FALSE;
			} else if (!strcmp(tag, "</FONT>")) {
				if (!fontsize) {
					gtk_text_insert(GTK_TEXT(gtkText),
							foreign_font,
							&gtkText->style->
							black, NULL,
							verseBuf, -1);
					//verseBuf[0] = '\0';
					//fontsize = TRUE;      

				} else {
					   gtk_text_insert(GTK_TEXT(gtkText),
					   roman_font,
					   &gtkText->style->black,
					   NULL, verseBuf, -1);                          
					   }  
				j = 0;
				verseBuf[0] = '\0';                           
				fontsize = TRUE;
			} else if (!strcmp(tag, "<B>")) {	/*  bold */
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
			} else if (!strcmp(tag, "</B>")) {
				gtk_text_insert(GTK_TEXT(gtkText),
						bold_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;
				verseBuf[0] = '\0';
			} else if (!strcmp(tag, "<BR>")
				   || !strcmp(tag, "<BR><B>")
				   || !strcmp(tag, "<P>")
				   || !strcmp(tag, "</P>")) {	/* new line */
				--i;
				myverse[i] = '\n';
			}
		}
		verseBuf[j] = myverse[i];
		++i;
		verseBuf[j + 1] = '\0';
		++j;
	}
	gtk_text_insert(GTK_TEXT(gtkText), roman_font,
			&gtkText->style->black, NULL, verseBuf, -1);
	delete[]verseBuf;
	verseBuf = NULL;
	delete[]myverse;
	myverse = NULL;
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
	return 0;
}

/***************************************************************************** 
* this will handle some html formating a chapter at a time for displaying 
* Bible text
* imodule - text to format for gtktext widget
******************************************************************************/
char HTMLChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[255];
	char *verseBuf, tag[255];
	char *myverse, *font;
	int i, j, taglen;
	bool greek_on = FALSE,
	    italics_on = FALSE,
	    poetry_on = FALSE, Fo_on = false, cite_on = false;
	GdkFont *sword_font, *greek_font, *fo_font, *fo_italic_font,
	    *cite_font, *cite_italic_font;;
	ModMap::iterator it;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;

	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbText")), 0);

	font = "Roman";

	if ((sit = mainMgr->config->Sections.find(imodule.Name())) !=
	    mainMgr->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
	}
	/* Load an italic font */
	italic_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a roman font */
	roman_font = gdk_font_load(font_mainwindow);
	/* Load FO font */
	fo_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-r-normal-*-*-80-*-*-p-*-iso8859-1");
	/* Load FO italic font */
	fo_italic_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-o-normal-*-*-80-*-*-p-*-iso8859-1");
	/* Load cite font */
	cite_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
	/* Load cite italic font */
	cite_italic_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-o-normal-*-*-100-*-*-p-*-iso8859-1");
	/* Load a verse number font */
	if (bVerseStyle)
		versenum_font =
		    gdk_font_load
		    ("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
	else
		versenum_font =
		    gdk_font_load
		    ("-adobe-helvetica-medium-r-normal-*-*-80-*-*-p-*-iso8859-1");
	/* Load a greek font if needed */
	if (!strcmp(font, "Symbol")) {
		sword_font = gdk_font_load
		    ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");
		greek_on = true;
	} else if (!strcmp(font, "Greek")) {
		sword_font = gdk_font_load
		    ("-unknown-greekregular-medium-r-normal--15-140-75-75-p-74-adobe-fontspecific");
		if (sword_font == NULL)
			sword_font = gdk_font_load
			    ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");
		greek_on = true;
	} else {
		sword_font = roman_font;
		greek_on = false;
	}
	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete(GTK_TEXT(gtkText),
				gtk_text_get_length((GTK_TEXT(gtkText))));
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	int len;

	gtk_text_freeze(GTK_TEXT(gtkText));
	for (key->Verse(1);
	     (key->Book() == curBook && key->Chapter() == curChapter
	      && !imodule.Error()); imodule++) {
		sprintf(tmpBuf, " %d. ", key->Verse());
		gtk_text_insert(GTK_TEXT(gtkText), versenum_font,
				&colourBlue, NULL, tmpBuf, -1);
		if (greek_on) {
			if (key->Verse() == curVerse) {
				gtk_text_thaw(GTK_TEXT(gtkText));
				adjVal = GTK_TEXT(gtkText)->vadj->upper;
				curPos =
				    gtk_text_get_length(GTK_TEXT(gtkText));
				gtk_text_freeze(GTK_TEXT(gtkText));	// GDB    
				gtk_text_insert(GTK_TEXT(gtkText),
						sword_font, &myGreen, NULL,
						(const char *) imodule,
						-1);
// GDB                    gtk_text_freeze(GTK_TEXT(gtkText));
			} else
				gtk_text_insert(GTK_TEXT(gtkText),
						sword_font,
						&gtkText->style->black,
						NULL,
						(const char *) imodule,
						-1);
		} else {
			i = j = 0;
			myverse = g_strdup((const char *) imodule);
			len = strlen(myverse);
			verseBuf = new char[len + 1];
			verseBuf[0] = '\0';
			while (i < len) {
				if (myverse[i] == '<') {
					tag[0] = '\0';
					taglen = gettags(myverse, tag, i);	/* get html tags */
					if (tag) {
						i = i + taglen;	/* remove tags (we do not want to see them) */
						if (!strcmp
						    (tag, "<SMALL><EM>")) {	/* strongs numbers -  */
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							--i;
							myverse[i] = '<';
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "</EM></SMALL>")) {	/* end strongs numbers -   */
							verseBuf[j] = '>';
							verseBuf[j + 1] =
							    '\0';
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     fo_italic_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     fo_italic_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "<SMALL><I>")) {	/* strongs numbers - tense */
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							--i;
							myverse[i] = '(';
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "</I></SMALL>")) {	/* end strongs numbers - tense */
							verseBuf[j] = ')';
							verseBuf[j + 1] =
							    '\0';
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     fo_italic_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     fo_italic_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "<CITE><CITE><I>")) {	/* special format used in KJV for Psalms titles -- start italic */
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							Fo_on = true;
							j = 0;
							verseBuf[0] = '\0';
							i = i + 15;
						} else
						    if (!strcmp(tag, "<CITE><CITE>")) {	/* special format used in KJV for Psalms titles */
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							Fo_on = true;
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "</CITE></CITE>")) {	/* end special format used in KJV for Psalms titles  */
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     fo_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     fo_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							Fo_on = false;
							j = 0;
							verseBuf[0] = '\0';
							myverse[i] = '\n';
						} else
						    if (!strcmp(tag, "<CITE><P>")) {	/* quote OT or Poetry */
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							cite_on = true;
							j = 0;
							verseBuf[0] = '\0';
							if ((!bVerseStyle)
							    || poetry_on) {	/* we only need new line if we are not in versestyle or poetry is on */
								myverse[i]
								    = '\t';	/* add a tab to beginning of next line */
								verseBuf[j]
								    = '\n';	/* add new line to end of this line */
								++j;
							}
						} else
						    if (!strcmp(tag, "</CITE><P>")) {	/* end quote OT or Poetry  */
							verseBuf[j] = '\n';
							++j;
							verseBuf[j] = '\0';
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     cite_italic_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     cite_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							cite_on = false;
							j = 0;
							verseBuf[0] = '\0';
							if ((!bVerseStyle)
							    || poetry_on) {	/* we only need new line if we are not in versestyle or poetry is on */
								myverse[i]
								    = '\t';	/* add a tab to beginning of next line */
								verseBuf[j]
								    = '\n';	/* add new line to end of this line */
								++j;
							}
						} else
						    if (!strcmp(tag, "<P>")) {	/* new paragraph  */
							if ((!bVerseStyle)
							    || poetry_on) {	/* we only need new line if we are not in versestyle or poetry is on  */
								myverse[i]
								    = '\t';	/* add a tab to beginning of next line */
								verseBuf[j]
								    = '\n';	/* add new line to end of this line */
								++j;
							}
						} else
						    if (!strcmp(tag, "<I>")) {	/* italic */
							if (Fo_on) {
								if (key->
								    Verse()
								    ==
								    curVerse)
									    gtk_text_insert
									    (GTK_TEXT
									     (gtkText),
									     fo_font,
									     &myGreen,
									     NULL,
									     verseBuf,
									     -1);
								else
									gtk_text_insert
									    (GTK_TEXT
									     (gtkText),
									     fo_font,
									     &gtkText->
									     style->
									     black,
									     NULL,
									     verseBuf,
									     -1);
							} else if (cite_on) {
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     cite_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							} else {
								if (key->
								    Verse()
								    ==
								    curVerse)
									    gtk_text_insert
									    (GTK_TEXT
									     (gtkText),
									     roman_font,
									     &myGreen,
									     NULL,
									     verseBuf,
									     -1);
								else
									gtk_text_insert
									    (GTK_TEXT
									     (gtkText),
									     roman_font,
									     &gtkText->
									     style->
									     black,
									     NULL,
									     verseBuf,
									     -1);
							}
							j = 0;
							verseBuf[0] = '\0';
							italics_on = TRUE;
						} else
						    if (!strcmp(tag, "</I>")) {	/* end italic */
							if (Fo_on) {
								if (key->
								    Verse()
								    ==
								    curVerse)
									    gtk_text_insert
									    (GTK_TEXT
									     (gtkText),
									     fo_italic_font,
									     &myGreen,
									     NULL,
									     verseBuf,
									     -1);
								else
									gtk_text_insert
									    (GTK_TEXT
									     (gtkText),
									     fo_italic_font,
									     &gtkText->
									     style->
									     black,
									     NULL,
									     verseBuf,
									     -1);
							} else if (cite_on) {
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     cite_italic_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							} else {
								if (key->
								    Verse()
								    ==
								    curVerse)
									    gtk_text_insert
									    (GTK_TEXT
									     (gtkText),
									     italic_font,
									     &myGreen,
									     NULL,
									     verseBuf,
									     -1);
								else
									gtk_text_insert
									    (GTK_TEXT
									     (gtkText),
									     italic_font,
									     &gtkText->
									     style->
									     black,
									     NULL,
									     verseBuf,
									     -1);
							}
							j = 0;
							verseBuf[0] = '\0';
							italics_on = false;
						} else
						    if (!strcmp(tag, "<B>")) {	/* bold */
							gtk_text_insert
							    (GTK_TEXT
							     (gtkText),
							     roman_font,
							     &gtkText->
							     style->black,
							     NULL,
							     verseBuf, -1);
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "</B>")) {	/* end bold */
							gtk_text_insert
							    (GTK_TEXT
							     (gtkText),
							     bold_font,
							     &gtkText->
							     style->black,
							     NULL,
							     verseBuf, -1);
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "<FONT COLOR=\"#800000\">")) {	/* foot note color  */
							if (key->Verse() ==
							    curVerse)
								    gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &myGreen,
								     NULL,
								     verseBuf,
								     -1);
							else
								gtk_text_insert
								    (GTK_TEXT
								     (gtkText),
								     roman_font,
								     &gtkText->
								     style->
								     black,
								     NULL,
								     verseBuf,
								     -1);
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "</FONT>")) {	/* end foot note color */
							gtk_text_insert
							    (GTK_TEXT
							     (gtkText),
							     cite_font,
							     &colourBlue,
							     NULL,
							     verseBuf, -1);
							j = 0;
							verseBuf[0] = '\0';
						} else
						    if (!strcmp(tag, "<BR>")) {	/*** new line ***/
							if ((!bVerseStyle)
							    || poetry_on) {
								/*** we only need new line if we are using paragraph style ***/
								verseBuf[j]
								    = '\n';
								++j;
							}
						}
					}
				}
				verseBuf[j] = myverse[i];
				++i;
				verseBuf[j + 1] = '\0';
				++j;
			}
			if (italics_on)
				sword_font = italic_font;
			else
				sword_font = roman_font;
			if (key->Verse() == curVerse) {
				gtk_text_thaw(GTK_TEXT(gtkText));
				adjVal = GTK_TEXT(gtkText)->vadj->upper;
				curPos =
				    gtk_text_get_length(GTK_TEXT(gtkText));
				gtk_text_freeze(GTK_TEXT(gtkText));	// GDB    
				gtk_text_insert(GTK_TEXT(gtkText),
						sword_font, &myGreen, NULL,
						verseBuf, -1);
// GDB                      gtk_text_freeze(GTK_TEXT(gtkText));
			} else {
				gtk_text_insert(GTK_TEXT(gtkText),
						sword_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
			}
			delete[]verseBuf;
			verseBuf = NULL;
			g_free(myverse);
		}
		/*** toggle paragraph style ***/
		if (bVerseStyle && (!poetry_on))
			gtk_text_insert(GTK_TEXT(gtkText), roman_font,
					&gtkText->style->black, NULL, "\n",
					-1);

	}
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
	adjVal -= (GTK_TEXT(gtkText)->vadj->page_size / 2);
	gtk_adjustment_set_value(GTK_TEXT(gtkText)->vadj,
				 (adjVal > 0) ? adjVal : 0);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	return 0;
}

/*****************************************************************************
* to handle Robertson's Word Pictures in the New Testament (RWP module)
* imodule - text to format for gtktext widget
******************************************************************************/
char GTKRWPDisp::Display(SWModule & imodule)
{
	GdkFont *sword_font,	/* pointers to fonts */
	*greek_font;
	gchar *myname;
	bool italics_on = FALSE,	/* boolean switches */
	greek_on = false, bold_on = false, first_time =
	    true, first_letter = true;
	char *verseBuf,		/* work strings */
	tmpBuf[800], buf[800];
	char *myverse,		/* pointers to strings */
	*font;
	int i, j, len;		/* integer vars */

	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbCom")), 1);
	/* Load a italic font */
	italic_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a bold font */
	bold_font =
	    gdk_font_load
	    ("-adobe-helvetica-bold-r-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a roman font */
	roman_font = gdk_font_load(font_interlinear);
	/* Load a verse number font */
	versenum_font =
	    gdk_font_load
	    ("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
	/* Load a greek font */
	greek_font =
	    gdk_font_load
	    ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");
	/* prepare text widget  */
	gtk_text_set_point(GTK_TEXT(gtkText), 0);	/* set position to begining of text */
	gtk_text_forward_delete(GTK_TEXT(gtkText),
				gtk_text_get_length((GTK_TEXT(gtkText))));	/* clear to end of text */
	gtk_text_freeze(GTK_TEXT(gtkText));	/* to pervent flicker and movement */

	(const char *) imodule;	/*  snap to entry */

	i = j = 0;		/* set counters to 0 - i is counter for myverse[] - j is counter for verseBuf */

	myverse = g_strdup((const char *) imodule);	/* get string from sword module */
	len = strlen(myverse);	/* get length of string */
	verseBuf = new char[len + 1];	/* init a string the size of myverse */
	verseBuf[0] = '\0';
	while (i < len) {	/* loop until we reach the end of the text string   */
		if (myverse[i] == 32 || myverse[i] == '\\')
			first_letter = true;	/* we need to know if we have a space or a backslash -- first letter of a word follows */
		if (myverse[i] == '{') {	/* remove the open curly bracket ({)  */
			++i;	/* increment i to revome the { */
			if (first_time) {	/* this is the first line to go to the text widget - we don't want any line feed here */
				sprintf(tmpBuf, "%s", verseBuf);
				first_time = false;
			} else {	/* else add two line feeds to seperate sections of text */

				sprintf(tmpBuf, "%s\n\n", verseBuf);
			}
			gtk_text_insert(GTK_TEXT(gtkText), roman_font,
					&gtkText->style->black, NULL,
					tmpBuf, -1);	/* print what is in the buffer  so we can bold the what follows (text between {}) */
			j = 0;	/* set verseBuf counter to 0 */
			verseBuf[0] = '\0';	/* empty verseBuf  */
			bold_on = true;
		}
		if (myverse[i] == '}') {	/* remove the close curly bracket (}) and print bold text */
			++i;	/* increment i to revome the } */
			gtk_text_insert(GTK_TEXT(gtkText), bold_font,
					&gtkText->style->black, NULL,
					verseBuf, -1);	/* print what is in the buffer */
			j = 0;	/* set verseBuf counter to 0 */
			verseBuf[0] = '\0';	/* empty verseBuf  */
			bold_on = false;	/* turn bold font off */
		}
		if (myverse[i] == '\\') {	/* remove first backslash and turn greek font on */
			if (!greek_on) {	/* we don't want to be here is greek is already on */
				++i;	/* increment i to remove the backslash */
				gtk_text_insert(GTK_TEXT(gtkText),
						roman_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);	/* print what is in the buffer so we can use greek font */
				j = 0;	/* set verseBuf counter to 0 */
				verseBuf[0] = '\0';	/* empty verseBuf  */
				greek_on = TRUE;
			}
		}
		if (myverse[i] == '\\') {	/* remove secound backslash and turn greek font off */
			if (greek_on) {	/* only go here is greek is on */
				++i;	/* increment i to remove the backslash */
				gtk_text_insert(GTK_TEXT(gtkText),
						greek_font,
						&gtkText->style->black,
						NULL, verseBuf, -1);
				j = 0;	/* set verseBuf counter to 0; */
				verseBuf[0] = '\0';	/* empty verseBuf */
				greek_on = FALSE;	/* turn greet font off */
			}
		}
		if (greek_on) {	/* if greek is on let's try to replace characters that don't print right with the symbol font with some that will */
			/*gchar buftext[80];                            
			   sprintf(buftext,"%3.3d\n",myverse[i]);
			   cout << buftext;  */
			if (myverse[i] == 't' && myverse[i + 1] == 'h') {	/* replace th with q (theta) */
				++i;
				myverse[i] = 'q';
			}
			if (myverse[i] == 's') {	/* end of word s with V *//* sigma with end of word sigma */
				if (myverse[i + 1] == ' '
				    || myverse[i + 1] == ')'
				    || myverse[i + 1] == '\\')
					myverse[i] = 'V';
			}
			if (myverse[i] == 'c' && myverse[i + 1] == 'h') {	/* ch with c (chi) */
				++i;
				myverse[i] = 'c';
			}
			if (myverse[i] == 'p' && myverse[i + 1] == 'h') {	/* ph with f (phi) */
				++i;
				myverse[i] = 'f';
			}
			if (myverse[i] == 39) {	/* skip ' (39)  - i don't know what this is */
				++i;
			}
			if (myverse[i] == -120) {	/* replace nonprintable with h (eta) */
				if (myverse[i + 1] == 'i')
					++i;
				myverse[i] = 'h';
			}
			if (myverse[i] == -125) {	/* replace nonprintable with a (alpha) */
				if (myverse[i + 1] == 'i')
					++i;
				myverse[i] = 'a';
			}
			if (myverse[i] == -109) {	/* replace nonprintable with w (omega) */
				if (myverse[i + 1] == 'i')
					++i;
				myverse[i] = 'w';
			}
			if (first_letter) {	/* if first letter of greek word -- if a vowel we must remove the h 
						   which i guess to be a rough breathing 
						   we can not show this with the symbol font so we remove it */
				if (myverse[i] == 'h'
				    && myverse[i + 1] == 'o') {
					++i;
					first_letter = false;
				}
				if (myverse[i] == 'h'
				    && myverse[i + 1] == 'a') {
					++i;
					first_letter = false;
				}
				if (myverse[i] == 'h'
				    && myverse[i + 1] == 'w') {
					++i;
					first_letter = false;
				}
				if (myverse[i] == 'h'
				    && myverse[i + 1] == 'u') {
					++i;
					first_letter = false;
				}
				if (myverse[i] == 'h'
				    && myverse[i + 1] == -120) {
					++i;
					myverse[i] = 'h';
					first_letter = false;
				}
				if (myverse[i] == 'h'
				    && myverse[i + 1] == 'i') {
					++i;
					first_letter = false;
				}
			}
		}
		if (myverse[i] == '#') {	/* remove # and start reference and red font */
			++i;	/* remove # */
			gtk_text_insert(GTK_TEXT(gtkText), roman_font,
					&gtkText->style->black, NULL,
					verseBuf, -1);	/* print current buffer and start new for reference */
			j = 0;
			verseBuf[0] = '\0';
		}
		if (myverse[i] == '|') {	/* remove | and end reference and red font */
			++i;	/* remove # */
			gtk_text_insert(GTK_TEXT(gtkText), roman_font,
					&colourRed, NULL, verseBuf, -1);	/* show ref */
			j = 0;
			verseBuf[0] = '\0';
		}
		verseBuf[j] = myverse[i];	/* move current char form myverse to verseBuf */
		++i;
		verseBuf[j + 1] = '\0';	/* put null at end of verseBuf */
		++j;
	}
	/* lets print what ever is left  */
	if (greek_on)
		sword_font = greek_font;
	else if (bold_on)
		sword_font = bold_font;
	else
		sword_font = roman_font;
	gtk_text_insert(GTK_TEXT(gtkText), sword_font,
			&gtkText->style->black, NULL, verseBuf, -1);
	delete[]verseBuf;
	verseBuf = NULL;
	g_free(myverse);
	/* finish with the text widget */
	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_thaw(GTK_TEXT(gtkText));
	return 0;
}

/***************************************************************************** 
* read html tags for formatting text in gtktext widget
* text - text from (const char*)imodule filtered for html
* tag - html tag
* pos - position in text to start looking for tag
******************************************************************************/
gint GTKEntryDisp::gettags(gchar * text, gchar * tag, gint pos)
{
	gint i, j, len;

	j = 0;
	len = strlen(text);
	tag[0] = '\0';
	for (i = pos; i < len; i++) {
		if (isalpha(text[i])) {
			tag[j] = toupper(text[i]); /*** make all tags upper case ***/
		} else {
			tag[j] = text[i];
		}
		++j;
		//g_warning("tag length = %d",j);
		tag[j] = '\0';
		if (text[i] == '>' && text[i + 1] != '<') {
			return j;
		}
	}
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
	if (
	    ((*mainMgr->
	      config->Sections[imodule.Name()].find("ModDrv")).second ==
	     "RawFiles") &&	/* check for personal comments by finding ModDrv=RawFiles */
	    (GTK_TOGGLE_BUTTON(lookup_widget(MainFrm, "btnEditNote"))->active)) {	/* check for edit mode */
		GtkWidget *statusbar;	/* pointer to comments statusbar */
		gint context_id2;	/* statusbar context_id ??? */
		sprintf(tmpBuf, "[%s] ", imodule.KeyText());	/* add module name and verse to edit note statusbar */
		/* setup statusbar for personal comments */
		statusbar = lookup_widget(MainFrm, "sbNotes");	/*        get stutusbar */
		context_id2 =
		    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
						 "GnomeSword");	/* get context id */
		gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);	/* ready status */
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2, tmpBuf);	/* show modName and verse ref in statusbar */
		gtk_text_insert(GTK_TEXT(gtkText), sword_font,
				&gtkText->style->black, NULL, " ", -1);	/* change font color to black for editing */
		gtk_text_set_point(GTK_TEXT(gtkText), 0);
		gtk_text_forward_delete(GTK_TEXT(gtkText),
					gtk_text_get_length(
							    (GTK_TEXT
							     (gtkText))));
	} else {		/* not useing personal comment module in edit mode */
		gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL,
				tmpBuf, -1);	/* show modName and verse ref in text widget */
	}
	/* show module text for current key */
	gtk_text_insert(GTK_TEXT(gtkText), sword_font,
			&gtkText->style->black, NULL,
			(const char *) imodule, -1);
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
	return 0;
}

/* ----------------------------------------------------------------------------------------- */
void AboutModsDisplay(GtkWidget * text, gchar * aboutinfo)
{				/* to display Sword module about information */
	gchar *textBuf;		/* text buffer to play with */
	gint i,			/* counter */
	    j,			/* counter */
	    len;		/* length of string aboutinfo */
	bool printnow = false;	/* tells us when to put our text to the screen */

	i = j = 0;		/* set to 0  */
	gtk_text_set_point(GTK_TEXT(text), 0);	/* set position to begining of text widget */
	gtk_text_forward_delete(GTK_TEXT(text),
				gtk_text_get_length((GTK_TEXT(text))));	/* clear text widget */
	len = strlen(aboutinfo);	/* set len to length of aboutinfo */
	textBuf = new char[len + 1];
	textBuf[0] = '\0';	/* empty text buffer */
	while (i < len) {	/* loop through string aboutinfo */
		if (aboutinfo[i] == '\\' && aboutinfo[i + 1] == 'p'
		    && aboutinfo[i + 2] == 'a') {	/* if we find \par replace with /n */
			if (aboutinfo[i + 4] == 'd')
				i = i + 4;	/* fix our counter to jump over \pard */
			else
				i = i + 3;	/* fix our counter to jump over \par */
			aboutinfo[i] = '\n';	/* add the \n (new line) */
			printnow = true;	/* we want to show this much now */
		}
		if (aboutinfo[i] == '\\' && aboutinfo[i + 1] == 'q'
		    && aboutinfo[i + 2] == 'c') {	/* replace \qc with nothing */

			i = i + 3;	/* fix counter to jump over \qc */
		}
		textBuf[j] = aboutinfo[i];	/* move aboutinfo to textBuf one char at a time */
		++i;		/* add one to i */
		++j;		/* add one to j */
		textBuf[j] = '\0';	/* terminate our string with \0 (null) */
		if (printnow) {	/* if true we print to text widget  */
			printnow = false;	/* set to false now */
			gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,
					textBuf, -1);	/* put string to text widget */
			textBuf[0] = '\0';	/* set string to null */
			j = 0;	/* set j to 0 */
		}
	}
	gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, textBuf, -1);	/* incase there is no \par at end of info */
	delete[]textBuf;
	textBuf = NULL;
}

