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
/*
#ifdef USE_GTKHTML30
#include <gal/widgets/e-unicode.h>
#endif
*/
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
#include "gui/hints.h"
#include "gui/widgets.h"
#include "gui/tabbed_browser.h"

#include "main/parallel_view.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/url.hh"
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
	gchar buf[500];
	extern gboolean shift_key_presed;
	
	if(shift_key_presed)
		return;
	
	if (url == NULL) { /* moved out of url - clear appbar - info viewer*/
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), "");
		in_url = FALSE;
		if(GPOINTER_TO_INT(data) == TEXT_TYPE)
			main_clear_viewer();
		/*
		if (hint.in_popup) {
			gtk_widget_destroy(hint.hint_window);
			hint.in_popup = FALSE;
		}
		*/
	} else {
		in_url = TRUE;	/* we need this for html_button_released */
		if(main_url_handler(url, FALSE))
			return;
		
		if (*url == 'I') {
			return;
		} else if (*url == 'U') {
			++url;
			sprintf(buf, "%s %s", _("Unlock "), url);
		} else /* any other link */
			sprintf(buf, "%s", "");
			
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
					buf);

#ifdef DEBUG	
	g_warning("link not handled");
#endif		
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
	gchar *buf = NULL;
	gchar tmpbuf[255];
	gint i = 0;
	
	if(main_url_handler(url, TRUE))
		return;
	
	if (*url == '@') {
		++url;
		main_swap_parallel_with_main((gchar *) url);
		return;
	}
	
//#ifdef DEBUG	
//	g_warning("link not handled");
//#endif
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
 *   gchar *   must be freed by calling function 
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
	info->local_font = gnome_font_find_closest("Luxi Sans", 10.0);

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
