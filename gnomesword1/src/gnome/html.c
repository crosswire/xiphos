/*
 * GnomeSword Bible Study Tool
 * html.c - GtkHtml gui stuff
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine-print.h>
#include <gtkhtml/htmlselection.h>
#include <gal/widgets/e-unicode.h>
#include <libgnomeprint/gnome-printer.h>
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-printer-dialog.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>
#include <libgnomeprint/gnome-print-preview.h>

#include "gui/html.h"
#include "gui/_editor.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/shortcutbar_main.h"
#include "gui/interlinear.h"
#include "gui/utilities.h"
#include "gui/about_modules.h"
#include "gui/main_window.h"

#include "main/gs_gnomesword.h"
#include "main/settings.h"

static GtkHTMLStream *htmlstream;
static GtkHTMLStreamStatus status1;

gboolean in_url;



/******************************************************************************
 * Name
 *   gui_url
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_url(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   called when mouse moves over an url (link)
 *
 * Return value
 *   void
 */
 
void gui_url(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar buf[255];

	/***  moved out of url - clear appbar  ***/
	if (url == NULL) {
		gnome_appbar_set_status(GNOME_APPBAR(settings.appbar),
					"");
		in_url = FALSE;
	}
	/***  we are in an url  ***/
	else {
		in_url = TRUE;	/* we need this for html_button_released */
		/***  swap interlinear and main text mods link ***/
		if (*url == '@') {
			++url;
			sprintf(buf, _("Show %s in main window"), url);
		}
		/***  module name link  ***/
		else if (*url == '[') {
			++url;
			while (*url != ']') {
				++url;
			}
			++url;
			sprintf(buf, "%s", url);
		}
		/***  verse number link  ***/
		else if (*url == '*') {
			++url;
			sprintf(buf, "%s", url);
		}
		/***  any other link  ***/
		else
			sprintf(buf, _("Go to %s"), url);

		gnome_appbar_set_status(GNOME_APPBAR(settings.appbar),
					buf);
	}
}


/******************************************************************************
 * Name
 *   gui_link_clicked
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_link_clicked(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

void gui_link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar *buf = NULL, *modbuf = NULL, tmpbuf[255];
	gchar newmod[80], newref[80];
	gint i = 0, havemod = 0;


	if (*url == '@') {
		++url;
		gui_swap_interlinear_with_main((gchar *) url);
	}
	/***  verse numbers in Bible Text window  ***/
	else if (*url == '*') {
		++url;
		buf = g_strdup(url);
		gui_change_verse(buf);
		g_free(buf);

	}

	else if (*url == 'I') {
		++url;
		buf = g_strdup(url);
		gui_change_verse(buf);
		g_free(buf);
	}
	/***  module name  ***/
	else if (*url == '[') {
		++url;
		while (*url != ']') {
			tmpbuf[i++] = *url;
			tmpbuf[i + 1] = '\0';
			++url;
		}
		gui_display_about_module_dialog(tmpbuf, FALSE);

	}
	/*** thml verse reference ***/
	else if (!strncmp(url, "version=", 7)
		 || !strncmp(url, "passage=", 7)) {
		gchar *mybuf = NULL;
		mybuf = strstr(url, "version=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			i = 0;
			while (mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i + 1] = '\0';
				++i;
				++havemod;
			}
		}
		mybuf = NULL;
		mybuf = strstr(url, "passage=");
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			while (i < strlen(mybuf)) {
				newref[i] = mybuf[i];
				newref[i + 1] = '\0';
				++i;
			}
		}
		if (havemod > 2) {
			modbuf = newmod;
		} else {
			modbuf = settings.MainWindowModule;
		}
		buf = g_strdup(newref);
		sprintf(settings.groupName, "%s", "Verse List");

		if (get_mod_type(modbuf) == DICTIONARY_TYPE) {
			/* we have a dict/lex module 
			   so we don't need to get a verse list */
			gui_display_dictlex_in_viewer(modbuf, buf);
		} else {
			gui_display_verse_list(modbuf, buf);
		}
		g_free(buf);

	}
	/***  thml morph tag  ***/
	else if (!strncmp(url, "type=morph", 10)) {
		gchar *modbuf = NULL;
		gchar *mybuf = NULL;
		buf = g_strdup(url);
		mybuf = strstr(url, "class=");
		if (mybuf) {
			gint i;
			modbuf = strchr(mybuf, '=');
			++modbuf;
			for (i = 0; i < strlen(modbuf); i++) {
				if (modbuf[i] == ' ') {
					modbuf[i] = '\0';
					break;
				}
			}
		}

		mybuf = NULL;
		mybuf = strstr(buf, "value=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
		}
		buf = g_strdup(mybuf);
		if (settings.inDictpane)
			gui_change_module_and_key(modbuf, buf);
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(modbuf, buf);
		g_free(buf);
	}
	/*** thml strongs ***/
	else if (!strncmp(url, "type=Strongs", 12)) {
		gchar *modbuf = NULL;
		gchar *mybuf = NULL;
		gint type = 0;
		//buf = g_strdup(url);
		mybuf = NULL;
		mybuf = strstr(url, "value=");
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			if (mybuf[0] == 'H')
				type = 0;
			if (mybuf[0] == 'G')
				type = 1;
			++mybuf;
			sprintf(newref, "%5.5d", atoi(mybuf));
		}
		if (type)
			modbuf = settings.lex_greek;
		else
			modbuf = settings.lex_hebrew;

		buf = g_strdup(newref);
		if (settings.inDictpane)
			gui_change_module_and_key(modbuf, buf);
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(modbuf, buf);
		g_free(buf);

	}
	/***  gbf strongs  ***/
	else if (*url == '#') {
		++url;		/* remove # */
		if (*url == 'T') {
			++url;	/* remove T */
			if (*url == 'G') {
				++url;	/* remove G */
				if (settings.havethayer) {
					buf = g_strdup(url);
					if (settings.inDictpane)
						gui_change_module_and_key("Thayer",
								  buf);
					if (settings.inViewer)
						gui_display_dictlex_in_viewer
						    ("Thayer", buf
						     );
					g_free(buf);
					return;
				}

				else
					return;
			}

			if (*url == 'H') {
				++url;	/* remove H */
				if (settings.havebdb) {
					buf = g_strdup(url);
					if (settings.inDictpane)
						gui_change_module_and_key("BDB",
								  buf);
					if (settings.inViewer)
						gui_display_dictlex_in_viewer("BDB",
								   buf
								   );
					g_free(buf);
					return;
				}

				else
					return;
			}
		}

		if (*url == 'G') {
			++url;	/* remove G */
			buf = g_strdup(url);
			if (atoi(buf) > 5624) {
				if (settings.havethayer) {
					buf = g_strdup(url);
					if (settings.inDictpane)
						gui_change_module_and_key("Thayer",
								  buf);
					if (settings.inViewer)
						gui_display_dictlex_in_viewer
						    ("Thayer", buf
						     );
					g_free(buf);
					return;
				} else
					return;

			}

			else {
				if (settings.inDictpane)
					gui_change_module_and_key(settings.lex_greek, buf);
				if (settings.inViewer)
					gui_display_dictlex_in_viewer(settings.lex_greek_viewer,
							   buf);
				g_free(buf);
			}
		}

		if (*url == 'H') {
			++url;	/* remove H */
			buf = g_strdup(url);
			if (atoi(buf) > 8674) {
				if (settings.havebdb) {
					buf = g_strdup(url);
					if (settings.inDictpane)
						gui_change_module_and_key("BDB",
								  buf);
					if (settings.inViewer)
						gui_display_dictlex_in_viewer("BDB",
								   buf
								   );
					g_free(buf);
					return;
				}

				else
					return;
			}

			else {
				if (settings.inDictpane)
					gui_change_module_and_key(settings.lex_hebrew, buf);
				if (settings.inViewer)
					gui_display_dictlex_in_viewer(settings.lex_hebrew_viewer,
							   buf);
				g_free(buf);
			}
		}
	}
	/***  gbf morph tag  ***/
	else if (*url == 'M') {
		++url;		/* remove M */
		buf = g_strdup(url);
		if (settings.inDictpane)
			gui_change_module_and_key("Packard", buf);
		if (settings.inViewer)
			gui_display_dictlex_in_viewer("Packard", buf);
		g_free(buf);
	}
}


/******************************************************************************
 * Name
 *   gui_copy_html
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_copy_html(GtkWidget * html_widget)
 *
 * Description
 *   copy menu item clicked in gbs
 *   html_widget - (GtkHTML widget) to copy from
 *
 * Return value
 *   
 */

void gui_copy_html(GtkWidget * html_widget)
{
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	gtk_html_copy(html);
}


/******************************************************************************
 * Name
 *   gui_copyhtml_activate
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   copy menu item clicked in any html window
 *   user_data - window (GtkHTML widget) to copy from   
 *
 * Return value
 *   void
 */

void gui_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkHTML *html;

	switch(settings.whichwindow) {
		case MAIN_TEXT_WINDOW:
			html = GTK_HTML(settings.html_text);
		break;
		case INTERLINEAR_WINDOW:
			html = GTK_HTML(settings.htmlInterlinear);
		break;
		case COMMENTARY_WINDOW:
			html = GTK_HTML(settings.html_comm);
		break;
		case DICTIONARY_WINDOW:
			html = GTK_HTML(settings.html_dict);
		break;
		case BOOK_WINDOW:
			html = GTK_HTML(settings.html_book);
		break;
		case PERCOMM_WINDOW:
			html = GTK_HTML(settings.html_percomm);
		break;
		case STUDYPAD_WINDOW:
			html = GTK_HTML(settings.html_studypad);
		break;
		default: 
			html = GTK_HTML(settings.html_text);
	}
		
	gtk_html_copy(html);
}


/******************************************************************************
 * Name
 *   gui_get_word_or_selection
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   gchar *gui_get_word_or_selection(GtkWidget * html_widget, gboolean word)
 *
 * Description
 *   get word or selection from html widget
 *
 * Return value
 *   gchar *
 */

gchar *gui_get_word_or_selection(GtkWidget * html_widget, gboolean word)
{
	gchar *key = NULL;
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	if (word)
		gtk_html_select_word(html);

	if (html_engine_is_selection_active(html->engine)) {
		key = html_engine_get_selection_string(html->engine);
		key = g_strdelimit(key, ".,\"<>;:?", ' ');
		key = g_strstrip(key);
		return g_strdup(key);	/* must be freed by calling function */
	}
	return key;
}


/******************************************************************************
 * Name
 *   gui_button_press_lookup
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   gchar *gui_button_press_lookup(GtkWidget * html_widget)
 *
 * Description
 *   lookup word in dict/lex module
 *
 * Return value
 *   gchar *
 */

gchar *gui_button_press_lookup(GtkWidget * html_widget)
{
	gchar *key = NULL;
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	if (!html_engine_is_selection_active(html->engine)) {
		gtk_html_select_word(html);
		if (html_engine_is_selection_active(html->engine)) {
			key =
			    html_engine_get_selection_string(html->engine);
			key = g_strdelimit(key, ".,\"<>;:?", ' ');
			key = g_strstrip(key);
			return g_strdup(key);	/* must be freed by calling function */
		}

	}
	return key;
}


/******************************************************************************
 * Name
 *   gui_begin_html
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_begin_html(GtkWidget * html_widget, gboolean isutf8)
 *
 * Description
 *   start loading html widget
 *
 * Return value
 *   void
 */

void gui_begin_html(GtkWidget * html_widget, gboolean isutf8)
{
	GtkHTML *html;
	gboolean was_editable;

	html = GTK_HTML(html_widget);
	was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	if (isutf8) {
		htmlstream =
		    gtk_html_begin_content(html,
					   "text/html; charset=utf-8");
	} else {
		htmlstream = gtk_html_begin(html);
	}
	gtk_html_set_editable(html, was_editable);
}


/******************************************************************************
 * Name
 *   gui_end_html
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_end_html(GtkWidget * html)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_end_html(GtkWidget * html)
{
	gtk_html_end(GTK_HTML(html), htmlstream, status1);
}


/******************************************************************************
 * Name
 *   gui_display_html
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_display_html(GtkWidget * html, const gchar * txt, gint lentxt)
 *
 * Description
 *   display text using gtk_html_write()
 *
 * Return value
 *   void
 */

void gui_display_html(GtkWidget * html, const gchar * txt, gint lentxt)
{
	if (strlen(txt)) {
		gtk_html_write(GTK_HTML(html), htmlstream, txt, lentxt);
	}
}


/******************************************************************************
 * Name
 *   print_footer
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void print_footer(GtkHTML * html, GnomePrintContext * context,
 *		gdouble x, gdouble y, gdouble width, gdouble height,
 *						gpointer user_data)
 *
 * Description
 *   printing stuff
 *
 * Return value
 *   void
 */

static gint page_num;
static GnomeFont *font;
static void print_footer(GtkHTML * html, GnomePrintContext * context,
		gdouble x, gdouble y, gdouble width, gdouble height,
						gpointer user_data)
{
	gchar *text = g_strdup_printf("- %d -", page_num);
	gdouble tw = gnome_font_get_width_string(font, "text");

	if (font) {
		gnome_print_newpath(context);
		gnome_print_setrgbcolor(context, .0, .0, .0);
		gnome_print_moveto(context, x + (width - tw) / 2,
				   y - (height +
					gnome_font_get_ascender(font)) /
				   2);
		gnome_print_setfont(context, font);
		gnome_print_show(context, text);
	}

	g_free(text);
	page_num++;
}


/******************************************************************************
 * Name
 *   gui_html_print
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_html_print(GtkWidget * htmlwidget)
 *
 * Description
 *   printing using gtk_html_print_with_header_footer()
 *
 * Return value
 *   void
 */
 
void gui_html_print(GtkWidget * htmlwidget)
{
	GnomePrintMaster *print_master;
	GnomePrintContext *print_context;
	GtkWidget *preview;
	GtkHTML *html;

	html = GTK_HTML(htmlwidget);

	print_master = gnome_print_master_new();
	print_context = gnome_print_master_get_context(print_master);

	page_num = 1;
	font =
	    gnome_font_new_closest("Times", GNOME_FONT_BOOK, FALSE,
				   12);
	gtk_html_print_with_header_footer(html, print_context, .0, .03,
					  NULL, print_footer, NULL);
	if (font)
		gtk_object_unref(GTK_OBJECT(font));

	preview =
	    GTK_WIDGET(gnome_print_master_preview_new
		       (print_master, "GnomeSword Print Preview"));
	gtk_widget_show(preview);

	gtk_object_unref(GTK_OBJECT(print_master));
}



