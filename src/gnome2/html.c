/*
 * GnomeSword Bible Study Tool
 * html.c - GtkHtml gui stuff
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprintui/gnome-print-dialog.h>
#include <libgnomeprintui/gnome-print-job-preview.h>
#include <fcntl.h>

#include "gui/html.h"
#include "gui/gnomesword.h"
#include "gui/editor.h"
#include "gui/sidebar.h"
#include "gui/shortcutbar_main.h"
#include "gui/parallel_view.h"
#include "gui/utilities.h"
#include "gui/about_modules.h"
#include "gui/main_window.h"
#include "gui/cipher_key_dialog.h"
#include "gui/dialog.h"
#include "gui/bibletext.h"
#include "gui/commentary_menu.h"
#include "gui/hints.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/xml.h"

static GtkHTMLStream *htmlstream;
static GtkHTMLStreamStatus status1;
gboolean in_url;



/******************************************************************************
 * Name
 *   url_requested
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void url_requested (GtkHTML *html, const gchar *url, 
 *					GtkHTMLStream *handle)
 *
 * Description
 *   needed for displaying images
 *
 * Return value
 *   void
 */

void url_requested(GtkHTML * html, const gchar * url,
		   GtkHTMLStream * handle)
{
	GtkHTMLStreamStatus status;
	gint fd;

	if (!strncmp(url, "file:", 5))
		url += 5;

	fd = open(url, O_RDONLY);
	status = GTK_HTML_STREAM_OK;
	if (fd != -1) {
		ssize_t size;
		void *buf = alloca(1 << 7);
		while ((size = read(fd, buf, 1 << 7))) {
			if (size == -1) {
				status = GTK_HTML_STREAM_ERROR;
				break;
			} else
				gtk_html_write(html, handle,
					       (const gchar *) buf,
					       size);
		}
	} else
		status = GTK_HTML_STREAM_ERROR;
	gtk_html_end(html, handle, status);
}

/******************************************************************************
 * Name
 *   show_in_appbar
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void show_in_appbar(GtkWidget * appbar, gchar * key, 
 *							gchar * mod)
 *
 * Description
 *   display information (morph or strongs) in appbar
 *
 * Return value
 *   void
 */

static void show_in_appbar(GtkWidget * appbar, gchar * key, gchar * mod)
{
	gchar *str;
	gchar *text;
	text = get_striptext(4, mod, key);
	str = remove_linefeeds(text);
	if (str) {
		gnome_appbar_set_status(GNOME_APPBAR(appbar), str);
		g_free(str);
	}
	g_free(text);
}


/******************************************************************************
 * Name
 *   deal_with_notes
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void deal_with_notes(const gchar *url)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void deal_with_notes(const gchar * url, gboolean clicked)
{
	gchar *buf = NULL;
	gchar *buf1 = NULL;
	gchar *tmpbuf = NULL;
	
	if(!in_url) {
		return;
	}
	if (hint.in_popup) {
		gtk_widget_destroy(hint.hint_window);
		hint.in_popup = FALSE;
	}
	buf1 = strchr(url, '=');
	++buf1;
	
	if(clicked) {
		tmpbuf = get_crossref(buf1);
		if (!*tmpbuf)
			return;
		else 
			gui_display_verse_list_in_sidebar(settings.
					  currentverse,
					  xml_get_value("modules", 
					  "bible"),
					  tmpbuf);
	} else {
		tmpbuf = get_footnote_body(buf1);
		if (tmpbuf == NULL)
			return;
		gui_display_in_hint_window(tmpbuf);
	}
	g_free(tmpbuf);
}


/******************************************************************************
 * Name
 *   deal_with_strongs
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void deal_with_strongs(const gchar *url)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void deal_with_storngs(const gchar * url, gboolean clicked)
{
	gchar *buf = NULL;
	gchar *tmpbuf = NULL;
	gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	gchar *modbuf_viewer = NULL;
	gchar newref[80];
	gint type = 0;
	
	mybuf = NULL;
	mybuf = strstr(url, "value=");
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
	if (type) {
		if ((atoi(mybuf) > 5624)
		    && (settings.havethayer)) {
			modbuf = "Thayer";
			modbuf_viewer = "Thayer";
		} else {
			modbuf_viewer = settings.lex_greek_viewer;
			modbuf = settings.lex_greek;
		}
	} else {
		modbuf = settings.lex_hebrew;
		modbuf_viewer = settings.lex_hebrew_viewer;
	}

	buf = g_strdup(newref);
	if (clicked) {
		if (settings.inDictpane)
			gui_change_module_and_key(modbuf, buf);
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(modbuf_viewer, buf);		
	} else {
		mybuf =
		    get_module_text(get_mod_type(modbuf), modbuf, buf);
		if (mybuf) {
			//gui_display_hint_in_viewer(mybuf);
			show_in_appbar(widgets.appbar, buf, modbuf);
			//gui_display_in_hint_window(mybuf);
			g_free(mybuf);
		}
	}
	g_free(buf);
}


/******************************************************************************
 * Name
 *   deal_with_morphs
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void deal_with_morphs(const gchar *url)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void deal_with_morphs(const gchar * url, gboolean clicked)
{
	gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	gchar *buf = NULL;
	gchar *myurl = NULL;
	gchar *oldnew = NULL;
	gboolean is_strongsmorph = FALSE;

	oldnew = g_strdup(url);
	myurl = g_strdup(url);
	buf = g_strdup(myurl);

	g_warning(url);
	mybuf = strstr(myurl, "class=");
	if (mybuf) {
		modbuf = strchr(mybuf, '=');
		++modbuf;
		if (modbuf[0] == 'x' && modbuf[1] == '-')
			modbuf += 2;
		if (!strncmp(modbuf, "Robinson", 7)) {
			if(check_for_module("Robinson"))
				modbuf = "Robinson";
			else
				return;
		} else if (!strncmp(modbuf, "none", 4)) {
			if(check_for_module("Packard"))
				modbuf = "Packard";
			else			
				return;
		} else
		    if (!strncmp
			(modbuf, "StrongsMorph", 11)) {
			is_strongsmorph = TRUE;
			if (strstr(oldnew, "value=TH"))
				modbuf =
				    settings.lex_hebrew;
			else
				modbuf =
				    settings.lex_greek;
		}
	} else
		modbuf = "Robinson";

	mybuf = NULL;
	mybuf = strstr(buf, "value=");
	if (mybuf) {
		mybuf = strchr(mybuf, '=');
		++mybuf;
	}
	if (is_strongsmorph) {
		++mybuf;
		++mybuf;
	}
	buf = g_strdup(mybuf);
	mybuf = NULL;
	if (clicked) {		
		if (settings.inDictpane)
			gui_change_module_and_key(modbuf, buf);
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(modbuf, buf);
	} else {
		mybuf =
		    get_module_text(get_mod_type(modbuf), modbuf, buf);
		if (mybuf) {
			//gui_display_in_hint_window(mybuf);
			show_in_appbar(widgets.appbar, buf, modbuf);
			//gui_display_hint_in_viewer(mybuf);
			g_free(mybuf);
		}
	}
	show_in_appbar(widgets.appbar, buf, modbuf);
	g_free(buf);
	g_free(myurl);
	g_free(oldnew);	
}


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
	gchar buf[255], *buf1;
	gchar *tmpbuf;
	gboolean is_strongsmorph = FALSE;
	gint i,j;

	if (url == NULL) { /***  moved out of url - clear appbar  ***/
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
					"");
		in_url = FALSE;
		if (hint.in_popup) {
			gtk_widget_destroy(hint.hint_window);
			hint.in_popup = FALSE;
		}
	} else {/***  we are in an url  ***/
		in_url = TRUE;	/* we need this for html_button_released */
		if (*url == '@') { /* swap parallel and main text mods link */
			++url;
			sprintf(buf, _("Show %s in main window"), url);
		} else if (!strncmp(url, "noteID=", 7)) { /***  osis footnote  ***/
			deal_with_notes(url, FALSE);
			return;
		} else if (*url == '[') {  /***  module name link  ***/
			++url;
			while (*url != ']') {
				++url;
			}
			++url;
			sprintf(buf, "%s", url);
		} else if (*url == '*') {/***  verse number link  ***/
			++url;
			sprintf(buf, "%s", url);
		} else if (!strncmp(url, "type=morph", 10)) {/***  thml and osis morph tag  ***/
			deal_with_morphs(url, FALSE);
			return;
		} else if (!strncmp(url, "type=Strongs", 12)) {/*** thml and osis strongs ***/
			deal_with_storngs(url, FALSE);			
			return;
		} else if (*url == 'U') {
			++url;
			sprintf(buf, "%s %s", _("Unlock "), url);
		} else /***  any other link  ***/
			sprintf(buf, "%s", "");

		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
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
	gchar *buf = NULL, *modbuf = NULL;
	gchar *buf1 = NULL;
	gchar newmod[80], newref[80], tmpbuf[255];
	gchar *oldnew = NULL;
	gint i = 0;
	//gboolean is_strongsmorph = FALSE;
	//const char *type;
	
	if (*url == '@') {
		++url;
		gui_swap_parallel_with_main((gchar *) url);
	} else if (!strncmp(url, "noteID=", 7)) {
		deal_with_notes(url, TRUE);
		return;
	} else if (*url == '*') {  /* verse numbers in Bible Text window */
		++url;
		buf = g_strdup(url);
		gui_change_verse(buf);
		g_free(buf);
	} else if (*url == 'I') {
		++url;
		buf = g_strdup(url);
		gui_change_verse(buf);
		g_free(buf);
	} else if (*url == '[') {/* module name  */
		++url;
		while (*url != ']') {
			tmpbuf[i++] = *url;
			tmpbuf[i + 1] = '\0';
			++url;
		}
		gui_display_about_module_dialog(tmpbuf, FALSE);

	} else if (!strncmp(url, "version=", 7)/* thml verse reference */
		 || !strncmp(url, "passage=", 7)) {
		gchar *mybuf = NULL;
		gchar *mod_name = NULL;
		mybuf = strstr(url, "version=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			i = 0;
			while (mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i + 1] = '\0';
				++i;
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
		if (check_for_module(newmod)) {
			modbuf = newmod;
		} else {
			modbuf = xml_get_value("modules", "bible");
		}
		buf = g_strdup(newref);
		mod_name = g_strdup(modbuf);

		if (get_mod_type(modbuf) == DICTIONARY_TYPE) {
			/* we have a dict/lex module 
			   so we don't need to get a verse list */
			gui_display_dictlex_in_sidebar(mod_name, buf);
		} else {
			gui_display_verse_list_in_sidebar(settings.
							  currentverse,
							  mod_name,
							  buf);
		}
		g_free(buf);
		g_free(mod_name);

	} else if (!strncmp(url, "type=morph", 10)) {/* thml and osis morph tag */
		deal_with_morphs(url, TRUE);
		return;
	} else if (!strncmp(url, "type=Strongs", 12)) {/* gbf thml and osis strongs */		
		deal_with_storngs(url, TRUE);
		return;
	} else if (*url == 'U') {
		++url;		/* remove U */
		buf = g_strdup(url);
		if (get_mod_type(buf) == TEXT_TYPE) {
			gui_unlock_bibletext(NULL, (TEXT_DATA *) data);
		} else if (get_mod_type(buf) == COMMENTARY_TYPE) {
			gui_unlock_commentary(NULL, (COMM_DATA *) data);
		}
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

	switch (settings.whichwindow) {
	case MAIN_TEXT_WINDOW:
		html = GTK_HTML(widgets.html_text);
		break;
	case parallel_WINDOW:
		html = GTK_HTML(widgets.html_parallel);
		break;
	case COMMENTARY_WINDOW:
		html = GTK_HTML(widgets.html_comm);
		break;
	case DICTIONARY_WINDOW:
		html = GTK_HTML(widgets.html_dict);
		break;
	case BOOK_WINDOW:
		html = GTK_HTML(widgets.html_book);
		break;
	case PERCOMM_WINDOW:
		html = GTK_HTML(widgets.html_percomm);
		break;
	case STUDYPAD_WINDOW:
		html = GTK_HTML(widgets.html_studypad);
		break;
	default:
		html = GTK_HTML(widgets.html_text);
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
	gint len;

	html = GTK_HTML(html_widget);
	if (!html_engine_is_selection_active(html->engine)) {
		gtk_html_select_word(html);
		if (html_engine_is_selection_active(html->engine)) {
			key =
			    html_engine_get_selection_string(html->
							     engine);
			key = g_strdelimit(key, ".,\"<>;:?", ' ');
			key = g_strstrip(key);
			len = strlen(key);
			/* g_warning("len = %d",len); */
			if (key[len - 1] == 's' || key[len - 1] == 'd')
				key[len - 1] = '\0';
			if (key[len - 1] == 'h' && key[len - 2] == 't'
			    && key[len - 3] == 'e')
				key[len - 3] = '\0';
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
 *   printing stuff - take from Evolution 1.4.x
 *
 * Return value
 *   void
 */

struct footer_info {
	GnomeFont *local_font;
	gint page_num, pages;
};

static void print_footer(GtkHTML * html,
			 GnomePrintContext * print_context, gdouble x,
			 gdouble y, gdouble width, gdouble height,
			 gpointer user_data)
{
	struct footer_info *info = (struct footer_info *) user_data;

	if (info->local_font) {
		char *text =
		    g_strdup_printf(_("Page %d of %d"), info->page_num,
				    info->pages);
		gdouble tw = strlen(text) * 8;

		gnome_print_gsave(print_context);
		gnome_print_newpath(print_context);
		gnome_print_setrgbcolor(print_context, .0, .0, .0);
		gnome_print_moveto(print_context, x + width - tw,
				   y -
				   gnome_font_get_ascender(info->
							   local_font));
		gnome_print_setfont(print_context, info->local_font);
		gnome_print_show(print_context, text);
		gnome_print_grestore(print_context);

		g_free(text);
		info->page_num++;
	}
}

static void footer_info_free(struct footer_info *info)
{
	if (info->local_font)
		gnome_font_unref(info->local_font);
	g_free(info);
}

static struct footer_info *footer_info_new(GtkHTML * html,
					   GnomePrintContext * pc,
					   gdouble * line)
{
	struct footer_info *info;

	info = g_new(struct footer_info, 1);
	info->local_font = gnome_font_find_closest("Helvetica", 10.0);

	if (info->local_font)
		*line =
		    gnome_font_get_ascender(info->local_font) -
		    gnome_font_get_descender(info->local_font);

	info->page_num = 1;
	info->pages =
	    gtk_html_print_get_pages_num(html, pc, 0.0, *line);

	return info;
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

void gui_html_print(GtkWidget * htmlwidget, gboolean preview)
{
	GtkHTML *html;
	GtkWidget *w = NULL;
	GnomePrintContext *print_context;
	GnomePrintJob *print_master;
	GnomePrintConfig *config = NULL;
	GtkDialog *dialog;
	gdouble line = 0.0;
	struct footer_info *info;

	if (!preview) {
		dialog =
		    (GtkDialog *) gnome_print_dialog_new(NULL,
						 _("Print"),
						 GNOME_PRINT_DIALOG_COPIES);
		gtk_dialog_set_default_response(dialog,
					GNOME_PRINT_DIALOG_RESPONSE_PRINT);
		gtk_window_set_transient_for((GtkWindow *) dialog,
					     (GtkWindow *)
					     gtk_widget_get_toplevel
					     (widgets.app));

		switch (gtk_dialog_run(dialog)) {
		case GNOME_PRINT_DIALOG_RESPONSE_PRINT:
			break;
		case GNOME_PRINT_DIALOG_RESPONSE_PREVIEW:
			preview = TRUE;
			break;
		default:
			gtk_widget_destroy((GtkWidget *) dialog);
			return;
		}

		config =
		    gnome_print_dialog_get_config((GnomePrintDialog *)
						  dialog);
		gtk_widget_destroy((GtkWidget *) dialog);
	}

	if (config) {
		print_master = gnome_print_job_new(config);
		gnome_print_config_unref(config);
	} else
		print_master = gnome_print_job_new(NULL);

	print_context = gnome_print_job_get_context(print_master);

	html = GTK_HTML(htmlwidget);

	gtk_html_print_set_master(html, print_master);

	info = footer_info_new(html, print_context, &line);
	gtk_html_print_with_header_footer(html, print_context, 0.0,
					  line, NULL, print_footer,
					  info);
	footer_info_free(info);

	gnome_print_job_close(print_master);

	if (preview) {
		GtkWidget *pw;

		pw = gnome_print_job_preview_new(print_master,
						 _("Print Preview"));
		gtk_widget_show(pw);
	} else {
		int result = gnome_print_job_print(print_master);

		if (result == -1)
			g_warning(_("Failed to print the document"));
	}

	g_object_unref(print_master);
}
