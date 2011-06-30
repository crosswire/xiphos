/*
 * Xiphos Bible Study Tool
 * html.c - GtkHtml gui stuff
 *
 * Copyright (C) 2000-2011 Xiphos Developer Team
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
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gtkhtml/gtkhtml.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <unistd.h>

#include <ctype.h>
#include <fcntl.h>

#include "gui/html.h"
#include "gui/xiphos.h"
#include "gui/sidebar.h"
#include "gui/parallel_view.h"
#include "gui/utilities.h"
#include "gui/about_modules.h"
#include "gui/main_window.h"
#include "gui/cipher_key_dialog.h"
#include "gui/dialog.h"
#include "gui/bibletext.h"
#include "gui/widgets.h"
#include "gui/tabbed_browser.h"

#include "main/parallel_view.h"
#include "main/previewer.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/url.hh"
#include "main/xml.h"

#include "gui/debug_glib_null.h"

static GtkHTMLStream *htmlstream;
static GtkHTMLStreamStatus status1;
gboolean in_url;


static void
handle_error (GError **error)
{
	if (*error != NULL) {
		g_warning ("%s", (*error)->message);
		g_clear_error (error);
	}
}


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

#ifndef O_BINARY
#define	O_BINARY	0
#endif

void url_requested(GtkHTML * html, const gchar * url,
		   GtkHTMLStream * handle)
{
	GtkHTMLStreamStatus status;
	gint fd;

	if (!strncmp(url, "file:", 5))
		url += 5;

	// fd = g_open(url, O_RDONLY | O_BINARY);
	fd = g_open(url, O_RDONLY | O_BINARY, 0);
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
 *   gui_url
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_url(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Descrg_warningiption
 *   called when mouse moves over an url (link)
 *
 * Return value
 *   void
 */

extern gboolean shift_key_pressed;

void gui_url(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar buf[500];

	if (shift_key_pressed)
		return;

	if (url == NULL) { /* moved out of url - clear appbar - info viewer*/
		gui_set_statusbar ("");
		in_url = FALSE;
		if (GPOINTER_TO_INT(data) == TEXT_TYPE)
			main_clear_viewer();
	} else {
		in_url = TRUE;	/* we need this for html_button_released */
		if (main_url_handler(url, FALSE))
			return;

		if (*url == 'I') {
			return;
		} else if (*url == 'U') {
			++url;
			sprintf(buf, "%s %s", _("Unlock "), url);
		} else /* any other link */
			sprintf(buf, "%s", "");

		gui_set_statusbar (buf);
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

	if (main_url_handler(url, TRUE))
		return;

	if (*url == '@') {
		++url;
		main_swap_parallel_with_main((gchar *) url);
		return;
	}
}


/******************************************************************************
 * Name
 *   gui_prefixable_link_clicked
 *
 * Synopsis
 *   #include "gui/html.h"
 *
 *   void gui_prefixable_link_clicked(GtkHTML * html,
 *				      const gchar * url,
 *				      gpointer data,
 *				      const gchar * book)
 *
 * Description
 *   html link clicked.  possibly prefix url with current book name.
 *
 * Return value
 *   void
 */


void gui_prefixable_link_clicked(GtkHTML * html,
				 const gchar * url,
				 gpointer data,
				 const gchar * book)
{
	gchar *buf, *place;
	gchar tmpbuf[1023];

	if ((buf = strstr(url, "&value="))) {
		buf += 7;
		place = buf;

		while (*buf && isdigit(*buf))
			buf++;
		if ((*buf == ':') || strncmp(buf, "%3A", 3) == 0) {
			/* url begins w/"digits:" only: fix */
			strncpy(tmpbuf, url, place - url);
			strcpy(tmpbuf+(place-url), book);
			strcat(tmpbuf, place);
			url = tmpbuf;
		}
	}
	if (main_url_handler(url, TRUE))
		return;

	if (*url == '@') {
		++url;
		main_swap_parallel_with_main((gchar *) url);
		return;
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
 *   gchar *   must be freed by calling function
 */

gchar *gui_get_word_or_selection(GtkWidget * html_widget, gboolean word)
{
	gchar *key = NULL;
	gchar *buf = NULL;
	gchar *s, *t;
	GtkHTML *html;
	gint len;

	html = GTK_HTML(html_widget);
	if (word)
		gtk_html_select_word(html);

	key = gtk_html_get_selection_html (html, &len);
	if (key && *key) {
		for (s = strchr(key, '<'); s; s = strchr(s, '<')) {
			if ((t = strchr(s, '>'))) {
				while (s <= t)
					*(s++) = ' ';
			} else {
				GS_message(("gui_get_word: Unmatched <> in %s\n", s));
				goto out;
			}
		}
		for (s = strstr(key, "*n"); s; s = strstr(s, "*n")) {
			*(s++) = ' ';
			*(s++) = ' ';
		}
		for (s = strstr(key, "*x"); s; s = strstr(s, "*x")) {
			*(s++) = ' ';
			*(s++) = ' ';
		}
		key = g_strdelimit(key, ".,\"<>;:?", ' ');
		key = g_strstrip(key);

		GS_message(("gui_get_word key: %s", key));
		buf = g_strdup(key);
		return buf; /* must be freed by calling function */
	}

out:
	return NULL;
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
 *   gchar *   must be freed by calling function
 */

gchar *gui_button_press_lookup(GtkWidget * html_widget)
{
	gchar *rtval = NULL;
	gchar *key = NULL;
	gchar *buf = NULL;
	GtkHTML *html;
	gint len;

	html = GTK_HTML(html_widget);
	if (!html->in_selection) {
		gtk_html_select_word(html);
		key = gtk_html_get_selection_html(html, &len);
		if ((key == NULL) || (len == 0)) {
			return NULL;
		}
		//converts encoding from ncr to utf8
		key = ncr_to_utf8(key);

		buf = main_get_striptext_from_string(settings.MainWindowModule,
						key);

		GS_message(("src/gnome2/html.c: buf=>%s<",buf));
		if (buf == NULL) return NULL;
		key = g_strdelimit(buf, "&.,\"<>;:?", ' ');
		key = g_strstrip(key);
		len = strlen(key);
		if (key[len - 1] == 's' || key[len - 1] == 'd')
			key[len - 1] = '\0';
		if (key[len - 1] == 'h' && key[len - 2] == 't'
		    && key[len - 3] == 'e')
			key[len - 3] = '\0';
		rtval = g_strdup(key);
		g_free(buf);
		return rtval;	// * must be freed by calling function *
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


static gint
_calc_header_height (GtkHTML *html, GtkPrintOperation *operation,
                         GtkPrintContext *context)
{
	PangoContext *pango_context;
	PangoFontDescription *desc;
	PangoFontMetrics *metrics;
	gint header_height;

	pango_context = gtk_print_context_create_pango_context (context);
	desc = pango_font_description_from_string ("Sans Regular 10");

	metrics = pango_context_get_metrics (
		pango_context, desc, pango_language_get_default ());
	header_height =
		pango_font_metrics_get_ascent (metrics) +
		pango_font_metrics_get_descent (metrics);
	pango_font_metrics_unref (metrics);

	pango_font_description_free (desc);
	g_object_unref (pango_context);

	return header_height;
}

static gint
_calc_footer_height (GtkHTML *html, GtkPrintOperation *operation,
                         GtkPrintContext *context)
{
	PangoContext *pango_context;
	PangoFontDescription *desc;
	PangoFontMetrics *metrics;
	gint footer_height;

	pango_context = gtk_print_context_create_pango_context (context);
	desc = pango_font_description_from_string ("Sans Regular 10");

	metrics = pango_context_get_metrics (
		pango_context, desc, pango_language_get_default ());
	footer_height =
		pango_font_metrics_get_ascent (metrics) +
		pango_font_metrics_get_descent (metrics);
	pango_font_metrics_unref (metrics);

	pango_font_description_free (desc);
	g_object_unref (pango_context);

	return footer_height;
}
static void
_draw_header (GtkHTML *html, GtkPrintOperation *operation,
                  GtkPrintContext *context,
                  gint page_nr,
                  PangoRectangle *rec,
		  gpointer data)
{
	PangoFontDescription *desc;
	PangoLayout *layout;
	gdouble x, y;
//	gint n_pages;
	gchar *text;
	cairo_t *cr;

	text = g_strdup ((gchar*) data);

	desc = pango_font_description_from_string ("Sans Regular 10");
	layout = gtk_print_context_create_pango_layout (context);
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	pango_layout_set_font_description (layout, desc);
	pango_layout_set_text (layout, text, -1);
	pango_layout_set_width (layout, rec->width);

	x = pango_units_to_double (rec->x);
	y = pango_units_to_double (rec->y);

	cr = gtk_print_context_get_cairo_context (context);

	cairo_save (cr);
	cairo_set_source_rgb (cr, .0, .0, .0);
	cairo_move_to (cr, x, y);
	pango_cairo_show_layout (cr, layout);
	cairo_restore (cr);

	g_object_unref (layout);
	pango_font_description_free (desc);

	g_free (text);
}


static void
_draw_footer (GtkHTML *html, GtkPrintOperation *operation,
                  GtkPrintContext *context,
                  gint page_nr,
                  PangoRectangle *rec,
		  gpointer data)
{
	PangoFontDescription *desc;
	PangoLayout *layout;
	gdouble x, y;
	gint n_pages;
	gchar *text;
	cairo_t *cr;

	g_object_get (operation, "n-pages", &n_pages, NULL);
	text = g_strdup_printf (_("Page %d of %d"), page_nr + 1, n_pages);

	desc = pango_font_description_from_string ("Sans Regular 10");
	layout = gtk_print_context_create_pango_layout (context);
	pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
	pango_layout_set_font_description (layout, desc);
	pango_layout_set_text (layout, text, -1);
	pango_layout_set_width (layout, rec->width);

	x = pango_units_to_double (rec->x);
	y = pango_units_to_double (rec->y);

	cr = gtk_print_context_get_cairo_context (context);

	cairo_save (cr);
	cairo_set_source_rgb (cr, .0, .0, .0);
	cairo_move_to (cr, x, y);
	pango_cairo_show_layout (cr, layout);
	cairo_restore (cr);

	g_object_unref (layout);
	pango_font_description_free (desc);

	g_free (text);
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
 * gtk_print
 * Return value
 *   void
 */

void gui_html_print(GtkWidget * htmlwidget, gboolean preview, const gchar * mod_name)
{
	GtkPrintOperation *operation;
//	GtkPrintSettings *psettings;
	GtkPageSetup *setup;
//	GtkPrintOperationResult result;
	GError *error = NULL;
	GtkPrintOperationAction action;

	if (preview)
		action = GTK_PRINT_OPERATION_ACTION_PREVIEW;
	else
		action = GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG;

	operation = gtk_print_operation_new ();
//	psettings = gtk_print_settings_new ();

//	psettings = gtk_print_operation_get_print_settings (operation);

	setup = gtk_page_setup_new ();
	gtk_page_setup_set_top_margin(setup, 30, GTK_UNIT_PIXEL);
	gtk_page_setup_set_left_margin(setup, 50, GTK_UNIT_PIXEL);

#ifdef WIN32
	gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
#endif


	gtk_print_operation_set_default_page_setup(operation, setup);

	//result = 
	gtk_html_print_operation_run (GTK_HTML(htmlwidget),
					       operation,
					       action,
					       NULL, //GTK_WINDOW (e->window),
					       (GtkHTMLPrintCalcHeight) _calc_header_height, /* GtkHTMLPrintCalcHeight  calc_header_height*/
					       (GtkHTMLPrintCalcHeight) _calc_footer_height, /* GtkHTMLPrintCalcHeight  calc_footer_height*/
					       (GtkHTMLPrintDrawFunc) _draw_header, /* GtkHTMLPrintDrawFunc draw_header */
					       (GtkHTMLPrintDrawFunc) _draw_footer, /* GtkHTMLPrintDrawFunc draw_footer */
					       (gchar*)mod_name, //e, /* gpointer user_data */
					       &error);

	g_object_unref (operation);
	handle_error (&error);
}
