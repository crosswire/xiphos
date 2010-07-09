/*
 * Xiphos Bible Study Tool
 * previewer.cc -
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <swmgr.h>
#include <swmodule.h>
#include <stringmgr.h>
#include <localemgr.h>

#ifdef USE_GTKMOZEMBED
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "gecko/gecko-html.h"
#endif
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

#include "gui/debug_glib_null.h"

//#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><STYLE type=\"text/css\"><!-- A { text-decoration:none } --></STYLE></head>"

using namespace std;

static GtkWidget *previewer_html_widget;

void main_set_previewer_widget(int in_sidebar)
{
    if (in_sidebar)
    	previewer_html_widget = sidebar.html_viewer_widget;
    else
	previewer_html_widget = widgets.html_previewer_text;
}

GtkWidget *main_get_previewer_widget(void)
{
	return 	settings.show_previewer_in_sidebar
		? sidebar.html_viewer_widget
		: widgets.html_previewer_text;
}

/******************************************************************************
 * Name
 *   main_init_previewer
 *
 * Synopsis
 *   #include "main/previewer.h"
 *
 *   void main_init_previewer(void)
 *
 * Description
 *   clear the information viewer
 *
 * Return value
 *   void
 */

void main_init_previewer(void)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	gchar *buf;

#ifdef USE_GTKMOZEMBED
	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(previewer_html_widget)))
		return;
	GeckoHtml *html = GECKO_HTML(previewer_html_widget);
	gecko_html_open_stream(html, "text/html");
#else
	/* setup gtkhtml widget */
	GtkHTML *html = GTK_HTML(previewer_html_widget);
	gboolean was_editable = gtk_html_get_editable(html);

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
			"<b>%s</b><hr>", buf);
	str = g_string_append(str, tmp_str->str);

	g_string_printf(tmp_str, " %s", "</font></body></html>");
	str = g_string_append(str, tmp_str->str);

#ifdef USE_GTKMOZEMBED
	if (str->len)
		gecko_html_write(html, str->str, str->len);
	gecko_html_close(html);
#else
	if (str->len)
		gtk_html_load_from_string(html, str->str, str->len);
	gtk_html_set_editable(html, was_editable);
#endif
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}


/******************************************************************************
 * Name
 *   main_clear_viewer
 *
 * Synopsis
 *   #include "main/previewer.h"
 *
 *   void main_clear_viewer(void)
 *
 * Description
 *   clear the information viewer
 *
 * Return value
 *   void
 */

void main_clear_viewer(void)
{
#ifdef USE_PREVIEWER_AUTOCLEAR
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	gchar *buf;

#ifdef USE_GTKMOZEMBED
	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(previewer_html_widget)))
		return;
	GeckoHtml *html = GECKO_HTML(previewer_html_widget);
	gecko_html_open_stream(html, "text/html");
#else
	/* setup gtkhtml widget */
	GtkHTML *html = GTK_HTML(previewer_html_widget);
	gboolean was_editable = gtk_html_get_editable(html);

	if (was_editable)
		gtk_html_set_editable(html, FALSE);
#endif
	g_string_printf(tmp_str,
			HTML_START
			"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
			"",
			settings.bible_bg_color, settings.bible_text_color,
			settings.link_color);

	str = g_string_new(tmp_str->str);
	g_string_printf(tmp_str,
			"<b>%s</b><hr></body></html>", _("Previewer"));
	str = g_string_append(str, tmp_str->str);

#ifdef USE_GTKMOZEMBED
	if (str->len)
		gecko_html_write(html, str->str, str->len);
	gecko_html_close(html);
#else
	if (str->len)
		gtk_html_load_from_string(html, str->str, str->len);
	gtk_html_set_editable(html, was_editable);
#endif
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
#endif /* USE_PREVIEWER_AUTOCLEAR */
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

void main_information_viewer(const gchar * mod_name, const gchar * text, const gchar * key,
			     const gchar * action, const gchar * type,
			     const gchar * morph_text, const gchar * morph)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	MOD_FONT *mf = get_font((gchar*)mod_name);
#ifdef USE_GTKMOZEMBED
	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(previewer_html_widget)))
		return;
	GeckoHtml *html = GECKO_HTML(previewer_html_widget);
	gecko_html_open_stream(html, "text/html");
#else
	GtkHTML *html = GTK_HTML(previewer_html_widget);
	PangoContext* pc = gtk_widget_create_pango_context(GTK_WIDGET(html));
	PangoFontDescription *desc = pango_context_get_font_description(pc);
	pango_font_description_set_family(
	    desc, ((mf->old_font) ? mf->old_font : "Serif"));
	gtk_widget_modify_font(GTK_WIDGET(html), desc);
#endif

	g_string_printf(tmp_str,
			HTML_START
			"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			"<font face=\"%s\" size=\"%+d\">",
			settings.bible_bg_color, settings.bible_text_color,
			settings.link_color,
			(mf->old_font ? mf->old_font : "none"),
			(mf->old_font_size
			 ? atoi(mf->old_font_size) + settings.base_font_size
			 : settings.base_font_size) - 1);

	str = g_string_new(tmp_str->str);

	if (type) {
		if (*type == 'n') {
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s</font><hr>",
					_("Footnote"));
			str = g_string_append(str, tmp_str->str);
		}
		else if (*type == 'u') {
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s: %s</font><hr>",
					_("User Annotation"), key);
			str = g_string_append(str, tmp_str->str);
		}
		else if (*type == 'x') {
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s</font><hr>",
					_("Cross Reference"));
			str = g_string_append(str, tmp_str->str);
		}
		else if (!strcmp(action, "showStrongs")) {
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s: %s</font><hr>",
					_("Strongs"), key);
			str = g_string_append(str, tmp_str->str);
		}
		else if (!strcmp(action, "showMorph")) {
			g_string_printf(tmp_str,
					"<font color=\"grey\">%s: %s</font><hr>",
					_("Morphology"), key);
			str = g_string_append(str, tmp_str->str);
		}
	} else {
		g_string_printf(tmp_str,
				"<font color=\"grey\">%s: %s</font><hr>",
				mod_name, key);
		str = g_string_append(str, tmp_str->str);
	}

	if (action && (!strcmp(action, "showStrongsMorph"))) {
		g_string_printf(tmp_str,
				"<font color=\"grey\">%s: %s</font><hr>",
				_("Strongs"), key);
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);

		g_string_printf(tmp_str,
				"<font color=\"grey\"><br><br>%s: %s</font><hr>",
				_("Morphology"), morph);
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, morph_text);
	} else {
		str = g_string_append(str, text);
	}

	str = g_string_append(str, "</font></body></html>");

#ifdef USE_GTKMOZEMBED
	if (str->len) {
		// manage too-large text blobs (e.g. NaveLinked "GOD, CREATOR").
		int len = str->len, offset = 0, write_size;

		while (len > 0) {
			write_size = min(10000, len);
			gecko_html_write(html, str->str+offset, write_size);
			offset += write_size;
			len -= write_size;
		}
	}
	gecko_html_close(html);
#else
	if (str->len)
		gtk_html_load_from_string(html, str->str, str->len);
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
 *   void mark_search_words(GString *str, gboolean eliminate)
 *
 * Description
 *    purplifies search terms in results.
 *    "eliminate" indicates whether to eliminate internal markup.
 *
 * Return value
 *   void
 */

void mark_search_words(GString * str)
{
	gchar *tmpbuf, *buf, *searchbuf;
	gint len_overall, len_word, len_tail, len_prefix;
	gchar closestr[40], openstr[40];

	/* regular expression search results         **fixme** */
	if ((settings.searchType == 0) ||
	    (settings.searchText[0] == '\0')) {
		return;
	}
	GS_message(("%s",settings.searchText));
	/* close tags */
	sprintf(closestr, "</b></font>");
	/* open tags */
	sprintf(openstr, "<font color=\"%s\"><b>", settings.found_color);
	searchbuf = g_utf8_casefold(g_strdup(settings.searchText),-1);
	if (g_str_has_prefix(searchbuf, "\"")) {
		searchbuf = g_strdelimit(searchbuf, "\"", ' ');
		g_strstrip(searchbuf);
	}

	buf = g_utf8_casefold(str->str,-1);

	/* if we have a muti word search go here */
	if (settings.searchType == -2 || settings.searchType == -4) {
		char *token;
		GList *list, *listbase;
		gint count = 0, i = 0;

		list = NULL;
		/* seperate the search words and add them to a glist */
		if ((token = strtok(searchbuf, " ")) != NULL) {
			if (!isalnum(*token) && isalnum(*(token+1)))
				token++;
			list = g_list_append(list, token);
			++count;
			while ((token = strtok(NULL, " ")) != NULL) {
				if (!isalnum(*token) && isalnum(*(token+1)))
					token++;
				list = g_list_append(list, token);
				++count;
			}
			/* if we have only one word */
		} else {
			list = g_list_append(list, searchbuf);
			count = 1;
		}

		listbase = list = g_list_first(list);

		for (i = 0; i < count; i++) {
			len_overall = strlen(buf);
			if (settings.searchType == -4) {
				// remove metachars and anything following ("WORD*")
				for (tmpbuf = (gchar *)list->data;
				     *tmpbuf && isalnum(*tmpbuf);
				     ++tmpbuf)
					; // nothing, just skipping to end or non-alnum
				if ((tmpbuf != list->data) && *tmpbuf)
					*tmpbuf = '\0';
				else if (!sword::stricmp((gchar *)list->data, "and") ||
					 !sword::stricmp((gchar *)list->data, "or")  ||
					 !sword::stricmp((gchar *)list->data, "not")) {
						// don't color boolean ops.
						goto next_word;
				}
			}
			len_word = strlen((gchar *)list->data);

			/* find search word in verse */
			while ((tmpbuf = g_strrstr(buf, (gchar *)list->data)) != NULL) {
				char *angle_open, *angle_close;

				len_tail = strlen(tmpbuf);
				len_prefix = len_overall - len_tail;

				// find html <> delimiters preceding this word.
				*tmpbuf = '\0';
				angle_open = strrchr(buf, '<');
				angle_close = strrchr(buf, '>');

				// contortions about that preceding markup.
				// note placement of <> in good/bad examples.
				// good: anything WORD anything [none]
				// good: <token> anything WORD anything [outside]
				// bad:  token> anything <token WORD anything [inside]
				if (!angle_open ||		  // no open (safe), or
				    (angle_close &&		  // there exists close and
				     (angle_open < angle_close))) // open is before close
				{
					// add end tag first
					str = g_string_insert(str,
							      (len_prefix + len_word),
							      closestr);
					// then add start tag
					str = g_string_insert(str,
							      len_prefix,
							      openstr);
				}

				len_overall = len_prefix;
			}
		next_word:
			g_free(buf);
			if ((list = g_list_next(list)))
				buf = g_utf8_casefold(str->str,-1);
		}
		g_list_free(listbase);

		/* else we have a phrase and only need to mark it */
	} else {
		len_overall = strlen(buf);
		len_word = strlen(searchbuf);
		tmpbuf = strstr(buf, searchbuf);
		if (tmpbuf) {
			len_tail = strlen(tmpbuf);
			len_prefix = len_overall - len_tail;
			/* place end tag first */
			str = g_string_insert(str, (len_prefix + len_word),
					      closestr);
			/* then place start tag */
			str = g_string_insert(str, len_prefix, openstr);
		}
	}
	g_free(searchbuf);
}


/******************************************************************************
 * Name
 *   main_entry_display
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_entry_display(gpointer data, gchar * mod_name,
 *		   gchar * text, gchar * key, gboolean show_key)
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
	MOD_FONT *mf = get_font(mod_name);
#ifdef USE_GTKMOZEMBED
	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(html_widget)))
		return;
	GeckoHtml *html = GECKO_HTML(html_widget);
	gecko_html_open_stream(html, "text/html");
#else
	gboolean was_editable = FALSE;
	GtkHTML *html = GTK_HTML(html_widget);
	PangoContext* pc = gtk_widget_create_pango_context(html_widget);
	PangoFontDescription *desc = pango_context_get_font_description(pc);
	pango_font_description_set_family(
	    desc, ((mf->old_font) ? mf->old_font : "Serif"));
	gtk_widget_modify_font(html_widget, desc);

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

	g_string_printf(tmp_str,
			"<font face=\"%s\" size=\"%+d\">",
			(mf->old_font ? mf->old_font : "none"),
			(mf->old_font_size
			 ? atoi(mf->old_font_size) + settings.base_font_size
			 : settings.base_font_size) - 1);
	str = g_string_append(str, tmp_str->str);

	/* show key in html widget  */
	if (show_key) {
		if ((settings.displaySearchResults)) {
			g_string_printf(tmp_str,
					"<a href=\"sword://%s/%s\">"
					"<font color=\"%s\">[%s] %s </font></a><br />",
					mod_name,
					key,
					settings.link_color,
					mod_name, key);
		} else {
			g_string_printf(tmp_str,
					"<a href=\"passagestudy.jsp?action=showModInfo&value=%s&module=%s\">"
					"<font color=\"%s\">[%s]</a></font>[%s]<br />",
					backend->module_description(mod_name),
					mod_name,
					settings.link_color,
					mod_name, key);
		}
		str = g_string_append(str, tmp_str->str);
	}

	if (settings.displaySearchResults) {
		search_str = g_string_new(text);
		mark_search_words(search_str);
		str = g_string_append(str, search_str->str);
		g_string_free(search_str, TRUE);
	} else {
		str = g_string_append(str, text);
	}

	g_string_printf(tmp_str, " %s", "</font></body></html>");
	str = g_string_append(str, tmp_str->str);

#ifdef USE_GTKMOZEMBED
	if (str->len)
		gecko_html_write(html, str->str, str->len);
	gecko_html_close(html);
#else
	if (str->len)
		gtk_html_load_from_string(html, str->str, str->len);
	gtk_html_set_editable(html, was_editable);
#endif

	free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}
