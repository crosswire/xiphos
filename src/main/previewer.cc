/*
 *  GnomeSword Bible Study Tool
 *  previewer.cc - 
 *
 *  Copyright (C) 2000,2001,2002,2003,2004,2005 GnomeSword Developer Team
 *
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

#include <gnome.h>
#include <swmgr.h>
#include <swmodule.h>
#include <stringmgr.h>
#include <localemgr.h>

#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>
#include "gui/bibletext.h"
#ifdef __cplusplus
}
#endif

#include <ctype.h>
#include <time.h>


#include "gui/widgets.h"
#include "gui/sidebar.h"
#include "gui/utilities.h"

#include "main/previewer.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"
 
#include "backend/sword_main.hh"

	
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"



/******************************************************************************
 * Name
 *   main_clear_viewer
 *
 * Synopsis
 *   #include "main/previewer.h"
 *
 *   void main_clear_viewer(VOID)
 *
 * Description
 *   clear the information viewer
 *
 * Return value
 *   void
 */

void main_clear_viewer(void)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	GString *search_str;
	gboolean was_editable = FALSE;
	gchar *buf;

#ifdef USE_GTKMOZEMBED
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sidebar.html_viewer_widget))) return;
	GeckoHtml *html = GECKO_HTML(sidebar.html_viewer_widget);
	gecko_html_open_stream(html,"text/html");
#else	
	/* setup gtkhtml widget */
	GtkHTML *html = GTK_HTML(sidebar.html_viewer_widget);
	was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
#endif	
	g_string_printf(tmp_str,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);

	str = g_string_new(tmp_str->str);
	buf = _("Previewer");
	g_string_printf(tmp_str,
	"<b>%s</b><br><font color=\"grey\">" "<HR></font><br>", buf);
	str = g_string_append(str, tmp_str->str);
		
	g_string_printf(tmp_str, " %s", "</font></body></html>");
	str = g_string_append(str, tmp_str->str);

#ifdef USE_GTKMOZEMBED
	if (str->len)
		gecko_html_write(html,str->str,str->len);
	gecko_html_close(html);
#else	
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
	gtk_html_set_editable(html, was_editable);
#endif	
	//free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}


/******************************************************************************
 * Name
 *   main_information_viewer
 *
 * Synopsis
 *   #include "main/previewer.h"
 *
 *   void main_information_viewer(GtkWidget * html_widget, gchar * mod_name, 
 *		    gchar * text, gchar *key, gchar * type)
 *
 * Description
 *   display information in the information previewer
 *
 * Return value
 *   void
 */

void main_information_viewer(gchar * mod_name, gchar * text, gchar * key,
		             gchar * action ,gchar * type ,gchar * morph_text,
			     gchar * morph)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	GString *search_str;
	MOD_FONT *mf = get_font(mod_name);
#ifdef USE_GTKMOZEMBED
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sidebar.html_viewer_widget))) return;
	GeckoHtml *html = GECKO_HTML(sidebar.html_viewer_widget);
	gecko_html_open_stream(html,"text/html");
#else	
	GtkHTML *html = GTK_HTML(sidebar.html_viewer_widget);
#endif

	g_string_printf(tmp_str,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);

	str = g_string_new(tmp_str->str);
	if(type) {
		if(!strcmp(type,"n")) {
			g_string_printf(tmp_str,
				"<font color=\"grey\">%s<HR></font><br>",
					_("Footnote"));
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(type,"x")) {
			g_string_printf(tmp_str,
				"<font color=\"grey\">%s<HR></font><br>",
					_("Cross Reference"));
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(action ,"showStrongs")) {  //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str,
				"<font color=\"grey\">%s: %s<HR></font><br>",
					_("Strongs"),key);
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(action ,"showMorph")) {  //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str,
				"<font color=\"grey\">%s: %s<HR></font><br>",
					_("Morphology"),key);
			str = g_string_append(str, tmp_str->str);
		}
	}
	
	if(!strcmp(action ,"showStrongsMorph")) {  //&& !strcmp(type,"Greek")
		g_string_printf(tmp_str,
			"<font color=\"grey\">%s: %s<HR></font>",
				_("Strongs"),key);
		str = g_string_append(str, tmp_str->str);
		g_string_printf(tmp_str, 
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font)?mf->old_font:"none", 
				(mf->old_font_size)?mf->old_font_size:"+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);
		
		g_string_printf(tmp_str,
			"<font color=\"grey\"><br><br>%s: %s<HR></font>",
					_("Morphology"),morph);
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, morph_text);
		g_string_printf(tmp_str, " %s<br>", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);
		
		
	} else {
		g_string_printf(tmp_str, 
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font)?mf->old_font:"none", 
				(mf->old_font_size)?mf->old_font_size:"+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);
	
		g_string_printf(tmp_str, " %s", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);
	
	}
	
#ifdef USE_GTKMOZEMBED
	if (str->len)
		gecko_html_write(html,str->str,str->len);
	gecko_html_close(html);
#else	
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
#endif
	free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}


/******************************************************************************
 * Name
 *   mark_search_words
 *
 * Synopsis
 *   #include "main/previewer.h"
 *
 *   void mark_search_words( GString *str )	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void mark_search_words(GString * str)
{
	gchar *tmpbuf, *buf, *searchbuf;
	gint len1, len2, len3, len4;
	gchar closestr[40], openstr[40];

	/* regular expression search results         **fixme** */
	if (settings.searchType == 0) {
		return;
	}
	g_message(settings.searchText);
	/* close tags */
	sprintf(closestr, "</b></font>");
	/* open tags */
	sprintf(openstr, "<font color=\"%s\"><b>",
		settings.found_color);
	/* point buf to found verse */
	buf = str->str;
	searchbuf = g_strdup(settings.searchText);
	if(g_str_has_prefix(searchbuf,"\"")) {
		searchbuf = g_strdelimit(searchbuf, "\"", ' ');
		g_strstrip( searchbuf );
	}
	
	/* if we have a muti word search go here */
	if (settings.searchType == -2 || settings.searchType == -4) {
		char *token;
		GList *list;
		gint count = 0, i = 0;

		list = NULL;
		/* seperate the search words and add them to a glist */
		if ((token = strtok(searchbuf, " ")) != NULL) {
			list = g_list_append(list, token);
			++count;
			while ((token = strtok(NULL, " ")) != NULL) {
				list = g_list_append(list, token);
				++count;
			}
			/* if we have only one word */
		} else {
			list = g_list_append(list, searchbuf);
			count = 1;
		}
		list = g_list_first(list);
		/* find each word in the list and mark it */
		for (i = 0; i < count; i++) {
			/* set len1 to length of verse */
			len1 = strlen(buf);
			/* set len2 to length of search word */
			len2 = strlen((gchar *) list->data);
			/* find search word in verse */
			if ((tmpbuf =
			     strstr(buf,
				    (gchar *) list->data)) != NULL) {
				/* set len3 to length of tmpbuf 
				   (tmpbuf points to first occurance of 
				   search word in verse) */
				len3 = strlen(tmpbuf);
				//-- set len4 to diff between len1 and len3
				len4 = len1 - len3;
				/* add end tags first 
				   (position to add tag to is len4 + len2) */
				str =
				    g_string_insert(str, (len4 + len2),
						    closestr);
				/* then add start tags 
				   (position to add tag to is len4) */
				str =
				    g_string_insert(str, len4, openstr);
			}
			/* point buf to changed str */
			buf = str->str;
			list = g_list_next(list);
		}
		g_list_free(list);

		/* else we have a phrase and only need to mark it */
	} else {
		len1 = strlen(buf);
		len2 = strlen(searchbuf);
		tmpbuf = strstr(buf, searchbuf);
		if (tmpbuf) {
			len3 = strlen(tmpbuf);
			len4 = len1 - len3;
			/* place end tag first */
			str =
			    g_string_insert(str, (len4 + len2),
					    closestr);
			/* then place start tag */
			str = g_string_insert(str, len4, openstr);
		}
	}
	/* free searchbuf */
	g_free(searchbuf);
}


/******************************************************************************
 * Name
 *   main_entry_display
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_entry_display(GtkWidget * html_widget, gchar * mod_name, 
 *					      gchar * text, gchar *key)
 *
 * Description
 *   display Sword modules one verse (entry) at a time
 *
 * Return value
 *   void
 */

void main_entry_display(gpointer data, gchar * mod_name,
		   gchar * text, gchar * key, gboolean show_key)
{
	GtkWidget *html_widget = (GtkWidget *) data;
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	GString *search_str;
	gboolean was_editable = FALSE;
	MOD_FONT *mf = get_font(mod_name);
#ifdef USE_GTKMOZEMBED	
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(html_widget))) return;
	GeckoHtml *html = GECKO_HTML(html_widget);
	gecko_html_open_stream(html,"text/html");
#else	
	GtkHTML *html = GTK_HTML(html_widget);

	/* setup gtkhtml widget */
	was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
#endif
	g_string_printf(tmp_str,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);

	str = g_string_new(tmp_str->str);
	/* show key in html widget  */
	if (show_key) {
		if ((settings.displaySearchResults)) {
			g_string_printf(tmp_str,
				"<a href=\"sword://%s/%s\">"
				"<font color=\"%s\">[%s] %s </font></a><br />",
				mod_name,
				key,
				settings.bible_verse_num_color,
				mod_name, key);
		} else {
			g_string_printf(tmp_str,
				"<a href=\"gnomesword.url?action=showModInfo&value=%s&module=%s\">"
				"<font color=\"%s\">[%s]</a></font>[%s] ",
				backend->module_description(mod_name),
				mod_name,
				settings.bible_verse_num_color,
				mod_name, 
				key);
		}
		str = g_string_append(str, tmp_str->str);
	}
	
	g_string_printf(tmp_str, 
			"<font face=\"%s\" size=\"%s\">",
			(mf->old_font)?mf->old_font:"none", 
			(mf->old_font_size)?mf->old_font_size:"+0");
	str = g_string_append(str, tmp_str->str);
	
	if (settings.displaySearchResults) {
		search_str = g_string_new(text);
		mark_search_words(search_str);
		str = g_string_append(str, search_str->str);
	} else {
		str = g_string_append(str, text);
	}

	g_string_printf(tmp_str, " %s", "</font></body></html>");
	str = g_string_append(str, tmp_str->str);

#ifdef USE_GTKMOZEMBED
	if (str->len)
		gecko_html_write(html,str->str,str->len);
	gecko_html_close(html);
#else	
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
	gtk_html_set_editable(html, was_editable);
#endif	
	/* andyp - inserted for debugging, remove */
	//g_print(str->str); 
	
	free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}
