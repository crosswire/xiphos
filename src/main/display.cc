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
#include "main/sword.h"

#include "gui/utilities.h"
#include "gui/widgets.h"

#include "backend/sword_main.hh"

	
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"

using namespace sword;
using namespace std;

	
//static gchar *f_message = "main/display.cc line #%d \"%s\" = %s";


char GTKEntryDisp::Display(SWModule &imodule) 
{
	GString *str = g_string_new(NULL);
	gchar *keytext = NULL;
	int curPos = 0;                                         
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;
	GtkHTML *html = GTK_HTML(gtkText);
	MOD_FONT *mf = get_font(imodule.Name());
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	
	(const char *)imodule;	// snap to entry
	main_set_global_options(ops);
	if(backend->module_type(imodule.Name()) == BOOK_TYPE)
		keytext = g_convert(backend->treekey_get_local_name(
				settings.book_offset),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);
	else
		keytext = g_convert((char*)imodule.KeyText(),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);
		//keytext = imodule.KeyText();
	
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
	if(keytext) g_free(keytext);
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
	gchar *utf8_key;
	gchar *buf;
	gchar *buf2;
	gchar *preverse = NULL;
	gchar *paragraphMark = "&para;";
	gchar *br = NULL;
	gchar heading[32];                                          
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;
		
		
	gboolean newparagraph = FALSE;
	//gboolean is_rtol = main_is_mod_rtol(imodule.Name());
	gboolean was_editable = gtk_html_get_editable(html);
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_text), 0);
	
	g_string_printf(str,	HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
				settings.bible_bg_color, 
				settings.bible_text_color,
				settings.link_color);
	//if(is_rtol) 
	//	str = g_string_append(str,"<DIV ALIGN=right>");			
	


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
		utf8_key = g_convert((char*)key->getText(),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);
		buf = g_strdup_printf(
			"&nbsp; <A HREF=\"sword:///%s\" NAME=\"%d\">"
			"<font size=\"%s\" color=\"%s\">%d</font></A> ",
			utf8_key,
			key->Verse(),
			settings.verse_num_font_size,
			settings.bible_verse_num_color, 
			key->Verse());
		g_free(utf8_key);
		//g_message(f_message,163,"buf",buf);
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
		//g_warning((const char *)imodule.RenderText());
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
	//if(is_rtol) 
	//	str = g_string_append(str,"</DIV>");
		
	//buf = g_strdup_printf("%s", "</body></html>");
	str = g_string_append(str, "</body></html>");
	//g_free(buf);
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

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/gtkhtml_display.h"
 *
 *   
 *
 * Description
 *   display Sword Bible texts a chapter at a time
 *
 * Return value
 *   void
 */

char GTKTextviewChapDisp::Display(SWModule &imodule)
{	
	char tmpBuf[255];
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	GtkTextMark   *mark = NULL;
	GtkTextIter iter, startiter, enditer;
	static GtkTextTag *font_tag = NULL;
	MOD_FONT *mf = get_font(imodule.Name());
	GtkTextView *textview = GTK_TEXT_VIEW(gtkText);
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
	GString *str = g_string_new(NULL);
	const gchar *mark_name = "CurrentVerse";
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_text), 1);
	if(font_tag)
		gtk_text_tag_table_remove(gtk_text_buffer_get_tag_table (buffer),
                                font_tag);
	
	font_tag = gtk_text_buffer_create_tag (buffer, "rtl_font", NULL);
        g_object_set (G_OBJECT (font_tag),
		"font", mf->old_font,
		NULL);
	
	//* clear the buffer *
	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_delete(buffer, &startiter, &enditer);
	
	
	main_set_global_options(ops);
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() 
				== curChapter && !imodule.Error()); imodule++) {
		
		g_string_printf(str, "%d", key->Verse());
	        gtk_text_buffer_get_end_iter(buffer, &iter);
		gtk_text_buffer_insert_with_tags(buffer, 
				&iter,
				str->str,
				str->len,
				gtk_text_tag_table_lookup (
				gtk_text_buffer_get_tag_table (buffer),
						       "verseNumber"),
				NULL);
		
		g_string_printf(str, " %s", (const char *)imodule);
		
		if(key->Verse() == curVerse) {
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			mark = gtk_text_buffer_create_mark (buffer,
						mark_name,
						&iter,
						TRUE);
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_currentverse"),
                                        font_tag,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),
					/*gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"verse"),*/			
                                        NULL);		
		} else {
			gtk_text_buffer_get_end_iter(buffer, &iter);			
			gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_verse"),
                                        font_tag,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),
					/*gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"verse"),*/		
                                        NULL);		
		} 
		gtk_text_buffer_get_end_iter(buffer, &iter);
		if (settings.versestyle) 
			gtk_text_buffer_insert(buffer, &iter, "\n", strlen("\n"));
		else
			gtk_text_buffer_insert(buffer, &iter, " ", strlen(" "));
		
	}
	
	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_apply_tag_by_name(buffer, 
			"rtl_text", 
			&startiter, 
			&enditer);
		
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(textview),
				mark,
				FALSE,
				TRUE,
				0.0,
				0.0);
	gtk_text_buffer_delete_mark(buffer,mark);
	
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
	int type = be->module_type(imodule.Name());
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

char DialogTextviewChapDisp::Display(SWModule &imodule)
{	
	char tmpBuf[255];
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	GtkTextMark   *mark = NULL;
	GtkTextIter iter, startiter, enditer;
	static GtkTextTag *font_tag = NULL;
	MOD_FONT *mf = get_font(imodule.Name());
	GtkTextView *textview = GTK_TEXT_VIEW(gtkText);
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
	GString *str = g_string_new(NULL);
	const gchar *mark_name = "CurrentVerse";
	
	if(!font_tag) {
		font_tag = gtk_text_buffer_create_tag (buffer, "rtl_font", NULL);
		g_object_set (G_OBJECT (font_tag), "font", mf->old_font,
					NULL);
	}
	
	//* clear the buffer *
	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_delete(buffer, &startiter, &enditer);
	
	
	main_set_global_options(ops);
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() 
				== curChapter && !imodule.Error()); imodule++) {
		
		g_string_printf(str, "%d", key->Verse());
	        gtk_text_buffer_get_end_iter(buffer, &iter);
		gtk_text_buffer_insert_with_tags(buffer, 
				&iter,
				str->str,
				str->len,
				gtk_text_tag_table_lookup (
				gtk_text_buffer_get_tag_table (buffer),
						       "verseNumber"),
				NULL);
		
		g_string_printf(str, " %s", (const char *)imodule);
		
		if(key->Verse() == curVerse) {
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			mark = gtk_text_buffer_create_mark (buffer,
						mark_name,
						&iter,
						TRUE);
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_currentverse"),
                                        font_tag,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),			
                                        NULL);		
		} else {
			gtk_text_buffer_get_end_iter(buffer, &iter);			
			gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_verse"),
                                        font_tag,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),		
                                        NULL);		
		} 
		gtk_text_buffer_get_end_iter(buffer, &iter);
		if (settings.versestyle) 
			gtk_text_buffer_insert(buffer, &iter, "\n", strlen("\n"));
		else
			gtk_text_buffer_insert(buffer, &iter, " ", strlen(" "));
		
	}
	
	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_apply_tag_by_name(buffer, 
			"rtl_text", 
			&startiter, 
			&enditer);
		
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(textview),
				mark,
				FALSE,
				TRUE,
				0.0,
				0.0);
	gtk_text_buffer_delete_mark(buffer,mark);
	
	if(font_tag)
		gtk_text_tag_table_remove(gtk_text_buffer_get_tag_table (buffer),
                                font_tag);
	font_tag = NULL;
	g_string_free(str, TRUE);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	free_font(mf);	
	g_free(ops);
}
