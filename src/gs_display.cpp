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
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbCom")), 0);
	(const char *) imodule;	/* snap to entry */
	strbuf = g_string_new("<B><FONT COLOR=\"#000FCF\">");
	sprintf(tmpBuf, "<A HREF=\"[%s]%s\"> [%s]</a>[%s] </b>",
		imodule.Name(), buf, imodule.Name(), imodule.KeyText());
	strbuf = g_string_append(strbuf, tmpBuf);
	/* show verse ref in text widget  */
	/* show module text for current key */
	beginHTML(GTK_WIDGET(gtkText),FALSE);
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
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbCom")), 0);
	(const char *) imodule;	/* snap to entry */
	beginHTML(GTK_WIDGET(gtkText),FALSE);
	strbuf = g_string_new("");
	/* show verse ref in text widget  */
	g_string_sprintf(strbuf,
			 "<B><FONT COLOR=\"#000FCF\"><A HREF=\"[%s]%s\"> [%s]</a>[%s] </b>",
			 imodule.Name(), buf, imodule.Name(),
			 imodule.KeyText());
	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);
	/* show module text for current key */
	if (!strcmp(font, "Symbol")) {	/* greek symbol font */
		strbuf = g_string_new("<FONT FACE=\"symbol\">");
		strbuf = g_string_append(strbuf, (const char *) imodule);
		strbuf = g_string_append(strbuf, "</font>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else if (!strcmp(font, "Greek")) {	/* greek -wingreek */
		strbuf =
		    g_string_new
		    ("<I> </I><FONT COLOR=\"#000000\" FACE=\"greek1\">");
		strbuf = g_string_append(strbuf, (const char *) imodule);
		strbuf = g_string_append(strbuf, "</font><I> </I>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} else if (!strcmp(font, "BSTHebrew")) {	/* hebrew -wingreek */
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
	GString *strbuf;
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gint len;
	char *Buf, c;
	bool newparagraph = false;
	gint mybuflen;

	c = 182;
	font = "Roman";
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbText")), 1);
	if ((sit = mainMgr->config->Sections.find(imodule.Name())) !=
	    mainMgr->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    (*sit).second.end()) {
			font = (char *) (*eit).second.c_str();
		}
	}
	beginHTML(GTK_WIDGET(gtkText),FALSE);
	strbuf =
	    g_string_new("");
	    g_string_sprintf(strbuf,"<HTML><body text=\"#151515\" link=\"#898989\"><font  size=\"%s\">","+2");
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
			} else if (!stricmp(font, "greek1")) {
				g_string_sprintf(strbuf,
						 "<FONT SIZE=\"+5\" COLOR=\"%s\" FACE=\"greek1\">",
						 mycolor);
			
			} else if (!stricmp(font, "BSTHebrew")) {
				g_string_sprintf(strbuf,
						 "<FONT COLOR=\"%s\" FACE=\"bsthebrew\">",
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
			} else if (!stricmp(font, "greek1")) {
				strbuf = g_string_new("<FONT SIZE=\"+5\"  COLOR=\"#000000\" FONT FACE=\"greek1\">");
			} else if (!stricmp(font, "BSTHebrew")) {
				strbuf = g_string_new("<FONT COLOR=\"#000000\" FONT FACE=\"bsthebrew\">");					
			} else {
				strbuf = g_string_new("");
				if (bVerseStyle && newparagraph) {
					g_string_sprintf(strbuf, "<FONT COLOR=\"#000000\" >%c",c);
					newparagraph = false;
				} else
					g_string_sprintf(strbuf, "<FONT COLOR=\"#000000\" >");
			}
			strbuf = g_string_append(strbuf, (const char *)imodule);
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
	gotoanchorHTML(gtkText, tmpBuf);
	return 0;
}


/* --------------------------------------------------------------------------------------------- */
char InterlinearDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[800], *font, *buf;
	gint i;
	//ModMap::iterator it;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	GString *strbuf;
	bool utf = false;
	gint len;
	gchar *sourceformat;
	char *Buf, *modName, *thebuf;	
		
	font = "Roman";
	buf = (char *) imodule.Description();
	if ((sit = mainMgr1->config->Sections.find(comp1Mod->Name())) !=
	    		mainMgr1->config->Sections.end()) {
		if ((eit = (*sit).second.find("Font")) !=
		    	(*sit).second.end()) {
				font = (char *) (*eit).second.c_str();				
		}
	} 
	(const char *) imodule;
	strbuf = g_string_new("");
	g_string_sprintf(strbuf,"<B><FONT COLOR=\"#000FCF\" SIZE=\"%s\">","+5");
	sprintf(tmpBuf, "<A HREF=\"[%s]%s\"> [%s]</a>[%s] </font></b>",
		imodule.Name(), buf, imodule.Name(), imodule.KeyText());
	strbuf = g_string_append(strbuf, tmpBuf);
	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);
	/* heading */
	if (font) {
		
		g_warning(font);
		strbuf = g_string_new("");
		g_string_sprintf(strbuf,"<font face=\"%s\" size=\"%s\">", font, "+5");
	} 
	/* body */
	/*if(utf){
		gchar *tmpstr;		
		tmpstr = e_utf8_to_gtk_string (gtkText, (const char *) imodule);
		strbuf = g_string_append(strbuf, tmpstr);
		g_warning("unicode\\n");
	} else 
	if(!stricmp(font,"caslon")){
			//if(g_utf8_validate((const char *) imodule)){
			g_warning("caslon is here");
			thebuf = e_utf8_to_gtk_string (gtkText, (const char *) imodule);
			strbuf = g_string_append(strbuf, thebuf);	
			//}
		
	} else */
		strbuf = g_string_append(strbuf, (const char *) imodule);
	/* closing */
	if(font){
		strbuf = g_string_append(strbuf, "</font><br><hr>");
		displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
		g_string_free(strbuf, TRUE);
	} 
	return 0;
}




