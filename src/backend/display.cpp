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

extern "C" {
#include <gtkhtml/gtkhtml.h>
}

#include "backend/display.h"

/*
 *  EntryDisp - for displaying modules in a GtkHTML widget wo/displaying the key
 *  imodule - the Sword module to display
 */
char EntryDisp::Display(SWModule & imodule)
{
	gchar tmpBuf[255], *utf8str;
	GString *strbuf;
	gint utf8len;
	GtkHTMLStream *htmlstream;
	GtkHTMLStreamStatus status1;
			
	(const char *) imodule;	/* snap to entry */
	s->percomverse = (gchar*)imodule.KeyText();
	/* show module text for current key */
	GtkHTML *html = GTK_HTML(gtkText);
	gboolean was_editable = gtk_html_get_editable (html);
	
	if (was_editable)
	   gtk_html_set_editable (html, FALSE); 
	htmlstream = gtk_html_begin_content(html, "text/html; charset=utf-8");
	
	sprintf(tmpBuf,
		HTML_START "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		s->bible_bg_color, s->bible_text_color,
		s->link_color);	
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	strbuf = g_string_new((const char *) imodule);	
	if (strbuf->len) {
		gtk_html_write(GTK_HTML(html), htmlstream, strbuf->str, strbuf->len);
	}
	g_string_free(strbuf, TRUE);
	
	gtk_html_end(GTK_HTML(html), htmlstream, status1);	
	gtk_html_set_editable (html, was_editable); 
	return 0;
}

/*
 * GtkHTMLEntryDisp - for displaying dict/lex modules in a GtkHTML 
 * widget the mods need to be filtered to html first
 */
char GtkHTMLEntryDisp::Display(SWModule & imodule)
{
	gchar 
		tmpBuf[500], 
		*buf,  
		*strkey, 
		*font, 
		*use_font, 
		*use_font_size = NULL, 
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
		
	bool 
		isGBS = false,
		isPerCom = false;
		
	if(!strcmp(imodule.Type(),"Generic Books"))
		isGBS = true;
	if(!strcmp((gchar*)imodule.getConfigEntry("ModDrv"),"RawFiles"))
		isPerCom = true;
	use_gtkhtml_font = false;
	use_font = g_strdup(pick_font(imodule));
	use_font_size = (gchar*)imodule.getConfigEntry("GSFont size");
	
	if(!use_font_size) {
		use_font_size = s->bible_font_size;
	}
	//-- setup gtkhtml widget
	GtkHTMLStreamStatus status1;	
	GtkHTML *html = GTK_HTML(gtkText);
	gboolean was_editable = gtk_html_get_editable (html);	
	if (was_editable)
	   gtk_html_set_editable (html, FALSE); 
	GtkHTMLStream *htmlstream = gtk_html_begin_content(html, "text/html; charset=utf-8");
	
	//-- snap to entry
	(const char *) imodule;	
	
	sprintf(tmpBuf,
		HTML_START "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		s->bible_bg_color, 
		s->bible_text_color,
		s->link_color);
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}	
	
	str = g_string_new("");
	
	/* show verse ref in html widget  */
	strkey = g_strdup((const char *)imodule.KeyText());
	if((s->displaySearchResults)){	
		g_string_sprintf(str,
			"<A HREF=\"version=%s passage=%s\"><FONT COLOR=\"%s\">[%s] %s </font></A>",
			(const char *)imodule.Name(), 
			strkey,
			s->bible_verse_num_color, 
			(const char *)imodule.Name(),
			strkey);
	}
	
	else if(isGBS) {
	s->percomverse = (gchar*)imodule.KeyText();
		g_string_sprintf(str,
			"<FONT COLOR=\"%s\">[%s]</font>",
			s->bible_verse_num_color, 
			strkey);
	}
	
	else if(isPerCom) {
		s->percomverse = (gchar*)imodule.KeyText();		
	}
	
	else {
		g_string_sprintf(str,
			"<A HREF=\"[%s] %s\"><FONT COLOR=\"%s\">[%s]</A></font>[%s] ",
			(const char *)imodule.Name(), 
			(const char *)imodule.Description(),
			s->bible_verse_num_color, 
			(const char *)imodule.Name(),
			strkey);
	}
	g_free(strkey);
	
	if(!isPerCom) {
		utf8str = e_utf8_from_gtk_string(gtkText, str->str);
		utf8len = strlen(utf8str);	
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}		
		g_string_free(str,TRUE);
	}
	
	if(use_gtkhtml_font)
		sprintf(tmpBuf, "<font size=\"%s\">", use_font_size);
	else
		sprintf(tmpBuf, "<font face=\"%s\" size=\"%s\">", use_font, use_font_size);
	
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}		
	
	if((s->displaySearchResults)){	
		str = g_string_new((const char *) imodule);
		marksearchwords(str);
		utf8str = str->str;			
	} else {
		str = g_string_new((const char *) imodule);		
		utf8str = str->str;			
	}
	utf8len = strlen(utf8str);		
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	sprintf(tmpBuf, " %s", "</font></body></html>");
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;		
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable); 
	
	g_string_free(str,TRUE);
	if(use_font) g_free(use_font);		
	return 0;
}

gchar* GtkHTMLEntryDisp::pick_font(SWModule & imodule)
{
	gchar *font, *lang, *use_font, *gsfont,*retval = "";
	string gsfontsize;
	
	gsfont = NULL;
	lang = NULL;
	gsfont = (gchar*)imodule.getConfigEntry("Font"); //load_module_font((gchar*)imodule.Name(),"GSFont");
	lang = (gchar*)imodule.getConfigEntry("Lang"); //get_module_lang_UTILITY((gchar*)imodule.Name());	
	
	font = g_strdup("-adobe-helvetica-*-*");
	
	if (gsfont) {
		retval = gsfont;
		use_gtkhtml_font = false;
	} else {
		if ((!lang) || !stricmp(lang, "en") || !stricmp(lang, "de")) {
			use_gtkhtml_font = true;
			font = g_strdup(s->default_font);
		} else if (!stricmp(lang, "grc")) {
			font = g_strdup(s->greek_font);
			use_gtkhtml_font = false;
		} else if (!stricmp(lang, "he")) {
			font = g_strdup(s->hebrew_font);
			use_gtkhtml_font = false;
		} else {
			font = g_strdup(s->unicode_font);
			use_gtkhtml_font = false;
		}
		gchar *xfontname = font;
		++xfontname;
		gchar *token = strtok(xfontname,"-");
		token = strtok(NULL,"-");
		retval = token;
	}
	g_free(font);	
	return retval;
}

/*
 * GtkHTMLChapDisp - for displaying text modules 
 * in a GtkHTML widget a chapter at a time 
 * - the mods need to be filtered to html first
 * imodule - the Sword module to display
 */
char GtkHTMLChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[80], *buf, *mybuf, versecolor[80];
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gint len;
	char *Buf, *paragraphMark;
	gchar *utf8str, *use_font, *use_font_size, *token, *bufstr;
	gint mybuflen, utf8len;
	const gchar **end;
	bool newparagraph = false;
	string gsfontsize;
	GString *str;
	
	use_gtkhtml_font = false;
	str = g_string_new("");
	  
	paragraphMark = "&para;";  
	use_font = g_strdup(pick_font(imodule));
	
	use_font_size = NULL;
	use_font_size = (gchar*)imodule.getConfigEntry("GSFont size"); //load_module_font((gchar*)imodule.Name(),"GSFont size");
	if (!use_font_size){ 
		use_font_size = s->bible_font_size;
	}
	
	//-- setup gtkhtml widget
	GtkHTMLStreamStatus status1;	
	GtkHTML *html = GTK_HTML(gtkText);
	gboolean was_editable = gtk_html_get_editable (html);	
	if (was_editable)
	   gtk_html_set_editable (html, FALSE); 
	GtkHTMLStream *htmlstream = gtk_html_begin_content(html, "text/html; charset=utf-8");
	
	g_string_sprintf(str,
		HTML_START "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		s->bible_bg_color, s->bible_text_color,
		s->link_color);
	utf8str = e_utf8_from_gtk_string(gtkText, str->str);
	utf8len = strlen(utf8str);		
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}

	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error()); imodule++) {
		if (key->Verse() == curVerse)			
			sprintf(versecolor, "%s", s->currentverse_color);
		else
			sprintf(versecolor, "%s", s->bible_text_color);
		g_string_sprintf(str,
			"&nbsp; <A HREF=\"*[%s] %s\" NAME=\"%d\"><FONT COLOR=\"%s\"><B>  %d</B></font></A> ",
			imodule.Description(), imodule.KeyText(),
			key->Verse(), s->bible_verse_num_color,
			key->Verse());
		utf8str = e_utf8_from_gtk_string(gtkText, str->str);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}

		if(use_gtkhtml_font)
			g_string_sprintf(str, "<font color=\"%s\" size=\"%s\">", versecolor, use_font_size);    
		else 
			g_string_sprintf(str, "<font face=\"%s\" color=\"%s\" size=\"%s\">", use_font, versecolor, use_font_size);   
		
		utf8str = e_utf8_from_gtk_string(gtkText, str->str);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}	
						
		if(newparagraph && s->versestyle) {
			newparagraph = false;
			sprintf(tmpBuf,  "%s ", paragraphMark);
			utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
			utf8len = strlen(utf8str);		
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}	
		} 
		if((s->displaySearchResults) && (key->Verse() == curVerse)){			
			g_string_free(str,TRUE);
			str = g_string_new((const char *) imodule);
			marksearchwords(str);
			utf8str = str->str;			
		} else {
			g_string_free(str,TRUE);
			str = g_string_new((const char *) imodule);		
			utf8str = str->str;			
		}
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}
		
		if (s->versestyle) {
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
		} 
		
		else
			if (strstr((const char *) imodule, "<!P>") == NULL)
				sprintf(tmpBuf, " %s", "</font>");
			else 
				sprintf(tmpBuf, " %s", "</font><p>");
		utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}	
	}
	
	sprintf(tmpBuf, " %s", "</body></html>");
	utf8str = e_utf8_from_gtk_string(gtkText, tmpBuf);
	utf8len = strlen(utf8str);		
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}		
	
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	sprintf(tmpBuf, "%d", curVerse);
	
	gtk_html_end(html, htmlstream, status1);
	gtk_html_set_editable(html, was_editable); 
	gtk_html_jump_to_anchor(html, tmpBuf);
	
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
	if(s->searchType == 0) {
		return;
	}
	//-- close tags
	sprintf(closestr,"</b></font>");
	//-- open tags
	sprintf(openstr,"<font color=\"%s\"><b>",s->found_color);
	//-- point buf to found verse
	buf = str->str;
	searchbuf = g_strdup(s->searchText);
		
	//-- if we have a muti word search go here
	if(s->searchType == -2) {
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


/*
 * to display Sword module about information
 */
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

/******   end of file   ******/
