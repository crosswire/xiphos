/*
 * GnomeSword Bible Study Tool
 * display.cc - 
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <swmgr.h>
#include <versekey.h>
#include <regex.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>

#ifdef __cplusplus
}
#endif
	
#include "main/display.hh"
#include "main/settings.h"
#include "main/global_ops.hh"

#include "gui/utilities.h"
//#include "gui/widgets.h"

#include "backend/module.hh"
#include "backend/sword_main.hh"
#include "backend/tree.hh"

	
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"

using namespace sword;
using namespace std;

	
char GTKEntryDisp::Display(SWModule &imodule) 
{
	GString *str = g_string_new(NULL);
	const gchar *keytext = NULL;
	int curPos = 0;
	GtkHTML *html = GTK_HTML(gtkText);
	MOD_FONT *mf = get_font(imodule.Name());
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	
	(const char *)imodule;	// snap to entry
	main_set_global_options(ops);
	if(backend_get_mod_type(imodule.Name()) == 3)
		keytext = backend_gbs_get_treekey_local_name(
			backend_gbs_get_treekey_offset());
	else
		keytext = imodule.KeyText();
	
	g_string_printf(str, 	HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
				"<a href=\"about://%s/%s\"><font color=\"%s\">"
				"[%s]</font></a>[%s]<br>"
				"<font face=\"%s\" size=\"%s\">%s"
				"</font></body></html>",
				settings.bible_bg_color, 
				settings.bible_text_color,
				settings.link_color,
				imodule.Name(),
				imodule.Description(),
				settings.bible_verse_num_color,
				imodule.Name(),
				(gchar*)keytext,
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				(const char *)imodule);	
	
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
	gtk_html_set_editable(html, was_editable);
	g_string_free(str, TRUE);
	free_font(mf);	
	g_free(ops);
}



char GTKChapDisp::Display(SWModule &imodule) 
{
	char tmpBuf[255];
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	MOD_FONT *mf = get_font(imodule.Name());
	GtkHTML *html = GTK_HTML(gtkText);
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	GString *str = g_string_new(NULL);
	gchar *buf;
	gchar *buf2;
	gchar *preverse = NULL;
	gchar *paragraphMark = "&para;";
	gchar *br = NULL;
	gchar heading[32];
	gboolean newparagraph = FALSE;
	gboolean was_editable = gtk_html_get_editable(html);
	
	g_string_printf(str,	HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
				settings.bible_bg_color, 
				settings.bible_text_color,
				settings.link_color);

	main_set_global_options(ops);
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() 
				== curChapter && !imodule.Error()); imodule++) {
		int x = 0;
		sprintf(heading,"%d",x);
		while((preverse 
			= backend->get_entry_attribute("Heading","Preverse",
							    heading)) != NULL) {
			buf = g_strdup_printf("<br><b>%s</b><br><br>",preverse);
			str = g_string_append(str, buf);
			g_free(preverse);
			g_free(buf);						     
			++x;
			sprintf(heading,"%d",x);
		}
		buf = g_strdup_printf(
			"&nbsp; <A HREF=\"sword:///%s\" NAME=\"%d\">"
			"<font size=\"%s\" color=\"%s\">%d</font></A> ",
			(char*)key->getText(),
			key->Verse(),
			settings.verse_num_font_size,
			settings.bible_verse_num_color, 
			key->Verse());
		str = g_string_append(str,buf);
		g_free(buf);
		buf = g_strdup_printf(
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				(mf->old_font)?mf->old_font:"", 
				(mf->old_font_size)?mf->old_font_size:"+0", 
				(key->Verse() == curVerse)
				?settings.currentverse_color
				:settings.bible_text_color);
		
		str = g_string_append(str,buf);
		g_free(buf);	

		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			str = g_string_append(str, paragraphMark);
		}
		str = g_string_append(str, (const char *)imodule);
		buf = g_strdup_printf("%s",(const char *)imodule);
		
		if (settings.versestyle) {
			if ((strstr(buf, "<BR>") == NULL) &&
			    (strstr(buf, "<br />") == NULL) &&
			     (strstr(buf, "<!P>") == NULL) &&
			     (strstr(buf, "<p>") == NULL) &&
			     (strstr(buf, "</p>") == NULL)  ) {
				buf2 = g_strdup_printf(" %s", "</font><br>");
			} else {
				br = g_strrstr(buf, "<br"); /* last occurance */
				if(strlen(br) > 6) /* we have a new line that's
						      not at the end of the string */					
					buf2 = g_strdup_printf(" %s", 
								"</font><br>");
				else
					buf2 = g_strdup_printf(" %s", "</font>");
			}
			if ((strstr(buf, "<!P>") == NULL) &&
			     (strstr(buf, "<p>") == NULL) ) {
				newparagraph = FALSE;
			} else {
				newparagraph = TRUE;
			}
		} else {
			if (strstr(buf, "<!P>") == NULL)
				buf2 = g_strdup_printf(" %s", "</font>");
			else
				buf2 = g_strdup_printf(" %s", "</font><p>");
		}
		str = g_string_append(str, buf2);
		g_free(buf);
		g_free(buf2);
	}
	buf = g_strdup_printf("%s", "</body></html>");
	str = g_string_append(str, buf);
	g_free(buf);
	if (str->len) {
		gtk_html_load_from_string(html,str->str,str->len);
	}
	gtk_html_set_editable(html, was_editable);
	if(curVerse > 1) {
		buf = g_strdup_printf("%d", curVerse - 1);
		gtk_html_jump_to_anchor(html, buf);
		g_free(buf);
	}	

	g_string_free(str, TRUE);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	free_font(mf);	
	g_free(ops);
}

char DialogEntryDisp::Display(SWModule &imodule) 
{
	GString *str = g_string_new(NULL);
	const gchar *keytext = NULL;
	int curPos = 0;
	int type = be->module_type();
	GtkHTML *html = GTK_HTML(gtkText);
	MOD_FONT *mf = get_font(imodule.Name());
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	
	(const char *)imodule;	// snap to entry
	main_set_global_options(ops);
	
	if(type == 3)
		keytext = be->treekey_get_local_name(be->get_treekey_offset());
	else
		keytext = imodule.KeyText();
	
	if(type == 4)
		
		g_string_printf(str, 	HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
				"<font face=\"%s\" size=\"%s\">%s"
				"</font></body></html>",
				settings.bible_bg_color, 
				settings.bible_text_color,
				settings.link_color,
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				(const char *)imodule);	
		
	else
		g_string_printf(str, 	HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
				"<a href=\"about://%s/%s\"><font color=\"%s\">"
				"[%s]</font></a>[%s]<br>"
				"<font face=\"%s\" size=\"%s\">%s"
				"</font></body></html>",
				settings.bible_bg_color, 
				settings.bible_text_color,
				settings.link_color,
				imodule.Name(),
				imodule.Description(),
				settings.bible_verse_num_color,
				imodule.Name(),
				(gchar*)keytext,
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				(const char *)imodule);	
	
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
	gtk_html_set_editable(html, was_editable);
	g_string_free(str, TRUE);
	free_font(mf);	
	g_free(ops);
}



char DialogChapDisp::Display(SWModule &imodule) 
{
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	MOD_FONT *mf = get_font(imodule.Name());
	GtkHTML *html = GTK_HTML(gtkText);
	GString *str = g_string_new(NULL);
	gchar *buf;
	gchar *buf2;
	gchar *preverse = NULL;
	gchar *paragraphMark = "&para;";
	gchar *br = NULL;
	gchar heading[32];
	gboolean newparagraph = FALSE;
	gboolean was_editable = gtk_html_get_editable(html);
	
	g_string_printf(str,	HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
				settings.bible_bg_color, 
				settings.bible_text_color,
				settings.link_color);

	for (key->Verse(1); (key->Book() == curBook && key->Chapter() 
				== curChapter && !imodule.Error()); imodule++) {
		int x = 0;
		sprintf(heading,"%d",x);
		while((preverse 
			= be->get_entry_attribute("Heading","Preverse",
							    heading)) != NULL) {
			buf = g_strdup_printf("<br><b>%s</b><br><br>",preverse);
			str = g_string_append(str, buf);
			g_free(preverse);
			g_free(buf);						     
			++x;
			sprintf(heading,"%d",x);
		}
		buf = g_strdup_printf(
			"&nbsp; <A HREF=\"sword:///%s\" NAME=\"%d\">"
			"<font size=\"%s\" color=\"%s\">%d</font></A> ",
			(char*)key->getText(),
			key->Verse(),
			settings.verse_num_font_size,
			settings.bible_verse_num_color, 
			key->Verse());
		str = g_string_append(str,buf);
		g_free(buf);
		buf = g_strdup_printf(
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				(mf->old_font)?mf->old_font:"", 
				(mf->old_font_size)?mf->old_font_size:"+0", 
				(key->Verse() == curVerse)
				?settings.currentverse_color
				:settings.bible_text_color);
		
		str = g_string_append(str,buf);
		g_free(buf);	

		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			str = g_string_append(str, paragraphMark);
		}
		str = g_string_append(str, (const char *)imodule);
		buf = g_strdup_printf("%s",(const char *)imodule);
		
		if (settings.versestyle) {
			if ((strstr(buf, "<BR>") == NULL) &&
			    (strstr(buf, "<br />") == NULL) &&
			     (strstr(buf, "<!P>") == NULL) &&
			     (strstr(buf, "<p>") == NULL) &&
			     (strstr(buf, "</p>") == NULL)  ) {
				buf2 = g_strdup_printf(" %s", "</font><br>");
			} else {
				br = g_strrstr(buf, "<br"); /* last occurance */
				if(strlen(br) > 6) /* we have a new line that's
						      not at the end of the string */					
					buf2 = g_strdup_printf(" %s", 
								"</font><br>");
				else
					buf2 = g_strdup_printf(" %s", "</font>");
			}
			if ((strstr(buf, "<!P>") == NULL) &&
			     (strstr(buf, "<p>") == NULL) ) {
				newparagraph = FALSE;
			} else {
				newparagraph = TRUE;
			}
		} else {
			if (strstr(buf, "<!P>") == NULL)
				buf2 = g_strdup_printf(" %s", "</font>");
			else
				buf2 = g_strdup_printf(" %s", "</font><p>");
		}
		str = g_string_append(str, buf2);
		g_free(buf);
		g_free(buf2);
	}
	buf = g_strdup_printf("%s", "</body></html>");
	str = g_string_append(str, buf);
	g_free(buf);
	if (str->len) {
		gtk_html_load_from_string(html,str->str,str->len);
	}
	gtk_html_set_editable(html, was_editable);
	if(curVerse > 1) {
		buf = g_strdup_printf("%d", curVerse - 1);
		gtk_html_jump_to_anchor(html, buf);
		g_free(buf);
	}	

	g_string_free(str, TRUE);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	free_font(mf);	
}
