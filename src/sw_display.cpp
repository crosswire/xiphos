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
#include <markupfiltmgr.h>
#include <versekey.h>
#include <gal/widgets/e-unicode.h>

#include "sw_display.h"
#include "support.h"
#include "sw_sword.h"
#include "gs_html.h"
#include "gs_gnomesword.h"
#include "sw_module_options.h"
#include "sw_utility.h"

gchar *mycolor;

/****************************************************************************************
 * externs
 ****************************************************************************************/
extern SETTINGS *settings;

/***************************************************************************** 
 * EntryDisp - for displaying modules in a GtkHTML widget wo/displaying the key
 * imodule - the Sword module to display
 *****************************************************************************/
char EntryDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[255], *utf8str;
	GString *strbuf;
	gint utf8len;

			
	(const char *) imodule;	/* snap to entry */
	settings->percomverse = (gchar*)imodule.KeyText();
	/* show module text for current key */
	beginHTML(GTK_WIDGET(gtkText), TRUE);
	sprintf(tmpBuf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings->bible_bg_color, settings->bible_text_color,
		settings->link_color);
	
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	strbuf = g_string_new((const char *) imodule);
	displayHTML(GTK_WIDGET(gtkText), strbuf->str, strbuf->len);
	g_string_free(strbuf, TRUE);
	
	endHTML(GTK_WIDGET(gtkText));
	return 0;
}

/***************************************************************************** 
 * GtkHTMLEntryDisp - for displaying dict/lex modules in a GtkHTML 
 * widget the mods need to be filtered to html first
 *****************************************************************************/
char GtkHTMLEntryDisp::Display(SWModule & imodule)
{
	gchar 
		tmpBuf[500], 
		*buf,  
		*strkey, 
		*font, 
		*use_font, 
		*use_font_size, 
		*token,
		*utf8str;
	gint 
		mybuflen, 
		utf8len;
	const gchar 
		**end;
	string 
		swfontsize;
	GString 
		*str;
	
	use_gtkhtml_font = false;
	use_font = g_strdup(pick_font(imodule));
	swfontsize = load_module_font((gchar*)imodule.Name(),"GSFont size");	
	
	if (strcmp(swfontsize.c_str(), "")) {
		use_font_size = (gchar *) swfontsize.c_str();
	} else {
		use_font_size = settings->bible_font_size;
	}
	
	(const char *) imodule;	/* snap to entry */
	beginHTML(GTK_WIDGET(gtkText), TRUE);
	sprintf(tmpBuf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings->bible_bg_color, 
		settings->bible_text_color,
		settings->link_color);
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	str = g_string_new("");
	
	/* show verse ref in html widget  */
	strkey = g_strdup((const char *)imodule.KeyText());
	if((settings->displaySearchResults)){	
		g_string_sprintf(str,
			"<A HREF=\"version=%s passage=%s\"><FONT COLOR=\"%s\">[%s] %s </font></A>",
			(const char *)imodule.Name(), 
			strkey,
			settings->bible_verse_num_color, 
			(const char *)imodule.Name(),
			strkey);
	}
	
	else {
		g_string_sprintf(str,
			"<A HREF=\"[%s] %s\"><FONT COLOR=\"%s\">[%s]</A></font>[%s] ",
			(const char *)imodule.Name(), 
			(const char *)imodule.Description(),
			settings->bible_verse_num_color, 
			(const char *)imodule.Name(),
			strkey);
	}
	g_free(strkey);
	
	utf8str = e_utf8_from_gtk_string(gtkText, str->str);
	utf8len = strlen(utf8str);
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	g_string_free(str,TRUE);
	
	if(use_gtkhtml_font)
		sprintf(tmpBuf, "<font size=\"%s\">", use_font_size);
	else
		sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">", use_font, use_font_size);
	
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	
	if((settings->displaySearchResults)){	
		str = g_string_new((const char *) imodule);
		marksearchwords(str);
		utf8str = str->str;			
	} else {
		str = g_string_new((const char *) imodule);		
		utf8str = str->str;			
	}
		
	displayHTML(GTK_WIDGET(gtkText), utf8str,
		    strlen(utf8str));
	sprintf(tmpBuf, " %s", "</font></body></html>");
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	endHTML(GTK_WIDGET(gtkText));
	g_string_free(str,TRUE);
	g_free(use_font);
	return 0;
}

gchar* GtkHTMLEntryDisp::pick_font(SWModule & imodule)
{
	gchar *font, *use_font, *retval = "";
	string lang, gsfont, gsfontsize;	
	
	gsfont = load_module_font((gchar*)imodule.Name(),"GSFont");
	lang = get_module_lang_UTILITY((gchar*)imodule.Name());	
	
	font = g_strdup("-adobe-helvetica-*-*");
	
	if (strcmp(gsfont.c_str(), "")) {
		retval = (gchar *)gsfont.c_str();
		//g_warning("retval = %s",retval);
		use_gtkhtml_font = false;
	} else {
		if (!stricmp(lang.c_str(), "") || !stricmp(lang.c_str(), "en") || !stricmp(lang.c_str(), "de")) {
			use_gtkhtml_font = true;
			font = g_strdup(settings->default_font);
		} else if (!stricmp(lang.c_str(), "grc")) {
			font = g_strdup(settings->greek_font);
			use_gtkhtml_font = false;
		} else if (!stricmp(lang.c_str(), "he")) {
			font = g_strdup(settings->hebrew_font);
			use_gtkhtml_font = false;
		} else {
			font = g_strdup(settings->unicode_font);
			use_gtkhtml_font = false;
		}
		retval = gethtmlfontnameHTML(font);
	}
	g_free(font);	
	return retval;
}

/****************************************************************************** 
 * GTKutf8ChapDisp - for displaying text modules 
 * in a GtkHTML widget a chapter at a time 
 * - the mods need to be filtered to html first
 * imodule - the Sword module to display
 ******************************************************************************/
char GTKutf8ChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[80], *buf, *mybuf, versecolor[80];
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gint len;
	char *Buf, c;
	gchar *utf8str, *use_font, *use_font_size, *token, *bufstr;
	gint mybuflen, utf8len;
	const gchar **end;
	bool newparagraph = false;
	string gsfontsize;
	GString *str;
	
	use_gtkhtml_font = false;
	str = g_string_new("");
	//c = 182;  
	c = 0x00b6;  
	use_font = g_strdup(pick_font(imodule));
	
	
	gsfontsize = load_module_font((gchar*)imodule.Name(),"GSFont size");
	if (strcmp(gsfontsize.c_str(), "")) {
		use_font_size = (gchar *)gsfontsize.c_str();
	} else {
		use_font_size = settings->bible_font_size;
	}
	
	beginHTML(GTK_WIDGET(gtkText), TRUE);
	g_string_sprintf(str,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings->bible_bg_color, settings->bible_text_color,
		settings->link_color);
	utf8str = e_utf8_from_gtk_string(gtkText, str->str);
	utf8len = strlen(utf8str);	
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error()); imodule++) {
		if (key->Verse() == curVerse)			
			sprintf(versecolor, "%s", settings->currentverse_color);
		else
			sprintf(versecolor, "%s", settings->bible_text_color);
		g_string_sprintf(str,
			"&nbsp; <A HREF=\"*[%s] %s\" NAME=\"%d\"><FONT COLOR=\"%s\"><B>  %d</B></font></A> ",
			imodule.Description(), imodule.KeyText(),
			key->Verse(), settings->bible_verse_num_color,
			key->Verse());
		utf8str = e_utf8_from_gtk_string(gtkText, str->str);
		utf8len = strlen(utf8str);	
		displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

		if(use_gtkhtml_font)
			g_string_sprintf(str, "<font color=\"%s\" size=\"%s\">", versecolor, use_font_size);    
		else 
			g_string_sprintf(str, "<font face=\"%s\" color=\"%s\" size=\"%s\">", use_font, versecolor, use_font_size);   
		
		utf8str = e_utf8_from_gtk_string(gtkText, str->str);
		utf8len = strlen(utf8str);	
		displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
		
		if(newparagraph && settings->versestyle) {
			newparagraph = false;
			sprintf(tmpBuf,  "%c ", c);
			utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
			utf8len = strlen(utf8str);	
			displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
		} 
		if((settings->displaySearchResults) && (key->Verse() == curVerse)){			
			g_string_free(str,TRUE);
			str = g_string_new((const char *) imodule);
			marksearchwords(str);
			utf8str = str->str;			
		} else {
			g_string_free(str,TRUE);
			str = g_string_new((const char *) imodule);		
			utf8str = str->str;			
		}
		displayHTML(GTK_WIDGET(gtkText), utf8str,
			    strlen(utf8str));
		if (settings->versestyle) {
			if ((strstr((const char *) imodule, "<BR>") == NULL ) && (strstr((const char *) imodule, "<!P>") == NULL))  {
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
		utf8len = strlen(utf8str);	
		displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	}
	sprintf(tmpBuf, " %s", "</body></html>");
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	sprintf(tmpBuf, "%d", curVerse);
	endHTML(GTK_WIDGET(gtkText));
	gotoanchorHTML(gtkText, tmpBuf);
	g_string_free(str,TRUE);
	g_free(use_font);
	return 0;
}

/*
 * marks search word(s) or phrase in results view 
 * str is current verse 
*/
void GtkHTMLEntryDisp::marksearchwords( GString *str )
{
	gchar *tmpbuf, *buf, *searchbuf;
	gint len1, len2, len3, len4;
	gchar closestr[40], openstr[40];
	
	//-- regular expression search results         ***fixme***
	if(settings->searchType == 0) {
		return;
	}
	//-- close tags
	sprintf(closestr,"</b></font>");
	//-- open tags
	sprintf(openstr,"<font color=\"%s\"><b>",settings->found_color);
	//-- point buf to found verse
	buf = str->str;
	searchbuf = g_strdup(settings->searchText);
		
	//-- if we have a muti word search go here
	if(settings->searchType == -2) {
		char *token;
		GList *list;
		gint count=0, i=0;
		
		list = NULL;
		//-- seperate the search words and add them to a glist
		if((token=strtok(searchbuf ," ")) != NULL) {
			list = g_list_append(list,token);
			++count;				
			while((token=strtok(NULL," ")) != NULL) {
				list = g_list_append(list,token);
				++count;
			}
		//-- if we have only one word
		} else {
			list = g_list_append(list,searchbuf);
			count = 1;
		}
		list = g_list_first (list);
		//-- find each word in the list and mark it
		for(i=0; i<count;i++) {
			//-- set len1 to length of verse
			len1 = strlen(buf);
			//-- set len2 to length of search word
			len2 = strlen((gchar*)list->data);
			//-- find search word in verse
			if((tmpbuf = strstr(buf,(gchar*)list->data)) != NULL) {
				//-- set len3 to length of tmpbuf (tmpbuf points to first occurance of search word in verse)
				len3 = strlen(tmpbuf);
				//-- set len4 to diff between len1 and len3
				len4 = len1 - len3;
				//-- add end tags first (position to add tag to is len4 + len2)
				str = g_string_insert (str, (len4+len2), closestr); 
				//-- then add start tags (position to add tag to is len4)
				str = g_string_insert (str, len4 , openstr);
			}
			//-- point buf to changed str
			buf = str->str;
			list = g_list_next(list);
		}	
		g_list_free(list);
	
	//-- else we have a phrase and only need to mark it
	} else {
		len1 = strlen(buf);
		len2 = strlen(searchbuf);
		tmpbuf = strstr(buf,searchbuf);
		if(tmpbuf) {
			len3 = strlen(tmpbuf);		
			len4 = len1 - len3;
			//-- place end tag first
			str = g_string_insert (str, (len4+len2) , closestr);
			//-- then place start tag
			str = g_string_insert (str, len4 , openstr);	
		}
	}	
	//-- free searchbuf
	g_free(searchbuf);
}

char IntDisplay(SETTINGS *s)
{
	gchar 
		*utf8str,
		*bgColor,
		*textColor,
		buf[500], 
		*tmpkey;
	
	SWMgr 	*mgr;
	
	bool 
		evenRow = FALSE;
	
	extern SWModule 
		*curMod;
	
	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	
	SWModule *mod = mgr->Modules[curMod->Name()];
	
	tmpkey = g_strdup(settings->currentverse);
	mod->SetKey(tmpkey);
	VerseKey *key = (VerseKey *) (SWKey *) *mod;
	
	beginHTML(settings->htmlInterlinear, TRUE);
	sprintf(buf,"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
			settings->bible_bg_color,
			settings->bible_text_color, settings->link_color);
	utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, buf);
	displayHTML(settings->htmlInterlinear, utf8str, strlen(utf8str));

	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear1Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	displayHTML(s->htmlInterlinear, utf8str, strlen(utf8str));
	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear2Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	displayHTML(s->htmlInterlinear, utf8str, strlen(utf8str));
	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear3Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	displayHTML(s->htmlInterlinear, utf8str, strlen(utf8str));
	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear4Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	displayHTML(s->htmlInterlinear, utf8str, strlen(utf8str));
	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear5Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	displayHTML(s->htmlInterlinear, utf8str, strlen(utf8str));


	sprintf(buf,"%s","</tr>");		
	utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, buf);
	displayHTML(settings->htmlInterlinear, utf8str, strlen(utf8str));
	bgColor = "#f1f1f1";
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter && !mod->Error()); (*mod)++) {
		sprintf(buf,"%s","<tr valign=\"top\">");		
		utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, buf);
		displayHTML(settings->htmlInterlinear, utf8str, strlen(utf8str));
				
		if(key->Verse() == curVerse)
			textColor = settings->currentverse_color;
		else 
			textColor = settings->bible_text_color;
		
		if(evenRow) {
			evenRow = false;
			bgColor = settings->bible_bg_color;
		}
			
		else {
			evenRow = true;
			bgColor = "#f1f1f1";				
		}
		
		for(int i=0;i< 5; i++) {
			SWModule *m;
			
			switch(i) {
				case 0: m = mgr->Modules[s->Interlinear1Module];
					break;
				case 1: m = mgr->Modules[s->Interlinear2Module];
					break;
				case 2: m = mgr->Modules[s->Interlinear3Module];
					break;
				case 3: m = mgr->Modules[s->Interlinear4Module];
					break;
				case 4: m = mgr->Modules[s->Interlinear5Module];
					break;
			}
			
			mgr->setGlobalOption("Strong's Numbers", "Yes");
			
			m->SetKey(key);
			sprintf(buf,
				"<td width=\"20%\" bgcolor=\"%s\"><A HREF=\"I%s\" NAME=\"%d\"><font color=\"%s\">%d. </font></A><font color=\"%s\">", 
				bgColor,
				m->KeyText(),
				key->Verse(),				 
				settings->bible_verse_num_color, 
				key->Verse(),
				textColor);
			utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, buf);
			displayHTML(settings->htmlInterlinear, utf8str, strlen(utf8str));
			utf8str = (gchar*)m->RenderText();
			displayHTML(settings->htmlInterlinear, utf8str, strlen(utf8str));
			sprintf(buf, "%s", "</font></td>");	
			utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, buf);			
			displayHTML(settings->htmlInterlinear, utf8str, strlen(utf8str));
		}	
		sprintf(buf,"%s","</tr>");		
		utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, buf);
		displayHTML(settings->htmlInterlinear, utf8str, strlen(utf8str));
	}
	
	sprintf(buf,"%s","</table></body></html>");		
	utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, buf);
	displayHTML(settings->htmlInterlinear, utf8str, strlen(utf8str));
	endHTML(settings->htmlInterlinear);	
	sprintf(buf, "%d", curVerse);
	gotoanchorHTML(settings->htmlInterlinear, buf);
	g_free(tmpkey);
	delete mgr;		//-- delete Sword manager   
	return 0;
}


// ---------------------------------------------
char InterlinearDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[800], *buf, *rowcolor;
	gint i;
	bool utf = false,
		use_gtkhtml_font = false;
	gint len;
	gchar *utf8str, *use_font, *use_font_size, *font, *token;
	gint utf8len;
	string lang, swfont, swfontsize;
	static gint row = 1;
	
	swfont = load_module_font((gchar*)imodule.Name(),"GSFont");
	swfontsize = load_module_font((gchar*)imodule.Name(),"GSFont size");
	lang = get_module_lang_UTILITY((gchar*)imodule.Name());	
	
	if (strcmp(swfontsize.c_str(), "")) {
		use_font_size = (gchar *) swfontsize.c_str();
	} else {
		use_font_size = settings->interlinear_font_size;
	}
	
	font = g_strdup("-adobe-helvetica-*-*");
	if (strcmp(swfont.c_str(), "")) {
		use_font = (gchar *) swfont.c_str();
		use_gtkhtml_font = false;
	} else {
		font = "-adobe-helvetica-*-*";
		if (!stricmp(lang.c_str(), "") ||
			    !stricmp(lang.c_str(), "en") ||
				    !stricmp(lang.c_str(), "de")) {
			font = g_strdup(settings->default_font);
			use_gtkhtml_font = true;			    
		} else if (!stricmp(lang.c_str(), "grc")) {
			font = g_strdup(settings->greek_font);
			use_gtkhtml_font = false;
		} else if (!stricmp(lang.c_str(), "he")) {
			font = g_strdup(settings->hebrew_font);
			use_gtkhtml_font = false;
		} else {
			font = g_strdup(settings->unicode_font);
			use_gtkhtml_font = false;
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
		utf8len = strlen(utf8str);	
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
	utf8len = strlen(utf8str);
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);

	if(use_gtkhtml_font)
		sprintf(tmpBuf, "<font size=\"%s\">", use_font_size);
	else
		sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">", use_font, use_font_size);	
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);
	displayHTML(GTK_WIDGET(gtkText), utf8str, utf8len);
	displayHTML(GTK_WIDGET(gtkText), (const char *) imodule,
		    strlen((const char *) imodule));
	sprintf(tmpBuf,
		"</font><small>[<A HREF=\"@%s\">view context</a>]</small></td></tr>",
		imodule.Name());
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	displayHTML(GTK_WIDGET(gtkText), utf8str, strlen(utf8str));
	g_free(font);
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
}


