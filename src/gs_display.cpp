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

#include <gnome.h>
#include <swmodule.h>
#include <swmgr.h>
#include <versekey.h>
//#include <thmlgbf.h>
//#include <gbfplain.h>
#include <gal/widgets/e-unicode.h>

#include "gs_display.h"
#include "support.h"
//#include "interface.h"
#include "gs_sword.h"
#include "gs_html.h"
#include "gs_gnomesword.h"


gchar *mycolor;

/****************************************************************************************
 * externs
 ****************************************************************************************/
extern SWMgr *mainMgr;
extern SWMgr *mainMgr1;
extern bool bVerseStyle;
extern GtkWidget *MainFrm;	/* pointer to app -- declared in GnomeSword.cpp */
extern SWModule *comp1Mod;
extern gchar *current_verse;
//extern INTERLINEAR interlinearMods;
extern GS_FONTS *gsfonts;

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
	beginHTML(GTK_WIDGET(gtkText),TRUE);
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
	gchar tmpBuf[500], *buf;
	//SectionMap::iterator sit;
	//ConfigEntMap::iterator eit;
	gchar *utf8str;
	gint mybuflen, utf8len;
	const gchar **end;
	
	/*gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbCom")), 0);*/
	
	(const char *) imodule;	/* snap to entry */
	beginHTML(GTK_WIDGET(gtkText),TRUE);
	sprintf(tmpBuf,"<html><body>");
	utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
	utf8len = g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 
	
	/* show verse ref in text widget  */
	sprintf(tmpBuf,"<A HREF=\"[%s] %s\"><B>[%s]</A>[%s] </B>",
					imodule.Name(),
					 imodule.Description(),
					imodule.Name(),
					 imodule.KeyText() );	
			utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
			utf8len = g_utf8_strlen (utf8str , -1) ;
			displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 
			
			 sprintf(tmpBuf, "<font size=\"%s\">",
					gsfonts->dictionary_font_size);	
			utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
			utf8len = g_utf8_strlen (utf8str , -1) ;
			displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 	 
			
			displayHTML(GTK_WIDGET(gtkText),(const char *)imodule, strlen((const char *)imodule) ); 
						 
			 sprintf(tmpBuf, " %s", "</font></body></html>");			 
			utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
			utf8len = g_utf8_strlen (utf8str , -1) ;
			displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 
			 /*
			 g_warning("utf8 string!");
		}else{ 
			 g_warning("not utf8 string!");*/
		//}		
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
	    g_string_sprintf(strbuf,"<HTML><body text=\"#151515\" link=\"#898989\"><font  size=\"%s\">","+1");
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

/****************************************************************************** 
 * GTKutf8ChapDisp - for displaying text modules 
 * in a GtkHTML widget a chapter at a time 
 * - the mods need to be filtered to html first
 * imodule - the Sword module to display
 ******************************************************************************/
char GTKutf8ChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[500], *buf, *mybuf, *encoding, versecolor[80];
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gint len;
	char *Buf, c;
	gchar *utf8str;
	gint mybuflen, utf8len;
	const gchar **end;
	
	//c = 182;	 
	
	gtk_notebook_set_page(GTK_NOTEBOOK
			      (lookup_widget(MainFrm, "nbText")), 1);
	
	beginHTML(GTK_WIDGET(gtkText),TRUE);	
	
	sprintf(tmpBuf,"<html><body text=\"#151515\" link=\"#898989\">",
		gsfonts->bible_font_size);
	utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
	utf8len = g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 
	
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter
	      && !imodule.Error()); imodule++) {
		
		if (key->Verse() == curVerse) {
			  sprintf(versecolor,"%s",mycolor);
		}else{
			  sprintf(versecolor,"%s", "#000000");
		}		
		sprintf(tmpBuf, "&nbsp; <A HREF=\"*[%s] %s\" NAME=\"%d\"><FONT COLOR=\"#000FCF\"><B>%d</B></font></A> ",
					 imodule.Description(),
					 imodule.KeyText(), key->Verse(),
					 key->Verse());	
		utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
		utf8len = g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 
			 
		sprintf(tmpBuf, "<font color=\"%s\" size=\"%s\">",
					versecolor ,
			 		gsfonts->bible_font_size);	
		utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
		utf8len = g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 	 
			
		displayHTML(GTK_WIDGET(gtkText),(const char *)imodule, strlen((const char *)imodule) ); 
		if(bVerseStyle){
			if (strstr((const char *)imodule, "<BR>") == NULL
					    && strstr((const char *)imodule, "<P>") == NULL)
					sprintf(tmpBuf, " %s", "</font><br>");	
			else sprintf(tmpBuf, " %s", "</font>");	
		} else sprintf(tmpBuf, " %s", "</font>");			 
		utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
		utf8len = g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 
	}
	sprintf(tmpBuf, " %s", "</body></html>");			 
	utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
	utf8len = g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	sprintf(tmpBuf, "%d", curVerse);
	endHTML(GTK_WIDGET(gtkText));
	gotoanchorHTML(gtkText, tmpBuf);
	return 0;
}
/* --------------------------------------------------------------------------------------------- */
char InterlinearDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[800], 
		*buf;
	gint i;
	bool utf = false;
	gint len;
	gchar *utf8str;
	gint utf8len;
	
	
	//buf = (char *) imodule.Description();	
	(const char *) imodule;		
	sprintf(tmpBuf, 
		"<B><A HREF=\"[%s]%s\"><FONT COLOR=\"#000FCF\" SIZE=\"%s\"> [%s]</font></a></b><FONT COLOR=\"#000FCF\">[%s] </font>",
				imodule.Name(), 
				 imodule.Description(),
				gsfonts->interlinear_font_size,
				imodule.Name(), 
				imodule.KeyText());	
	utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
	utf8len = g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);	
		
	sprintf(tmpBuf,	 "<font size=\"%s\">",
				gsfonts->interlinear_font_size);	
	utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
	utf8len = g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText),utf8str , utf8len ); 	
			
	displayHTML(GTK_WIDGET(gtkText), (const char *)imodule, strlen((const char *)imodule) );
		
	sprintf(tmpBuf," [<A HREF=\"@%s\">view context</a>]</font><br><hr>",imodule.Name());
	utf8str = e_utf8_from_gtk_string (gtkText, tmpBuf);
	displayHTML(GTK_WIDGET(gtkText), utf8str, strlen(utf8str));
	return 0;
}

/* ***************************************************************************
 * to display Sword module about information
 *****************************************************************************/
void AboutModsDisplayHTML(char *to, char *text)
{
    	int len,i;
	bool center = false;
								// shift string to right of buffer
								// -------------------------------
	for (i = 0; i < strlen(text) -1; i++) {
		if (text[i] == '\\') // a RTF command
		{
			if ((text[i+1] == 'p') && (text[i+2] == 'a') && (text[i+3] == 'r') && (text[i+4] == 'd'))
			{ // switch all modifier off
				
				if (center)
				{
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
			if ((text[i+1] == 'p') && (text[i+2] == 'a') && (text[i+3] == 'r'))
			{
				*to++ = '<';
				*to++ = 'b';
				*to++ = 'r';
				*to++ = '>';
				*to++ = '\n';
				i += 3;
				continue;
			}
			if (text[i+1] == ' ')
			{
				i += 1;
				continue;
			}
			if (text[i+1] == '\n')
			{
				i += 1;
				continue;
			}			
			if ((text[i+1] == 'q') && (text[i+2] == 'c')) // center on
			{
				if (!center)
				{
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


