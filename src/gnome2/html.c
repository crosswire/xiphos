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

#ifndef USE_GTKHTML38
#include <gtkhtml/htmlengine-print.h>
#include <gtkhtml/htmlselection.h>
#endif
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprintui/gnome-print-dialog.h>
#include <libgnomeprintui/gnome-print-job-preview.h>
#include <fcntl.h>

#include "gui/html.h"
#include "gui/gnomesword.h"
#include "gui/sidebar.h"
#include "gui/shortcutbar_main.h"
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
 * Descrg_warningiption
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

	GS_warning(("link not handled"));
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
	
	if(main_url_handler(url, TRUE))
		return;
	
	if (*url == '@') {
		++url;
		main_swap_parallel_with_main((gchar *) url);
		return;
	}
	
	GS_warning(("link not handled"));
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
	
	if (buf = strstr(url, "&value=")) {
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
	gchar *buf2 = NULL;
	GtkHTML *html;
	gint len, i, keylen;

	html = GTK_HTML(html_widget);
	if (word)
		gtk_html_select_word(html);	
#ifdef USE_GTKHTML38		
	key = gtk_html_get_selection_html (html, &len);
	if ((key == NULL) || (*key == '\0'))
	{
		gui_generic_warning("No selection provided\nSubstituting `Jesus'");
		key = ">Jesus<";
	}
	if (buf = strchr(key, '>'))	/* it might not have >< delimiters? */
		key = buf + 1;
	keylen = strlen(key);
	buf = g_new(gchar, keylen);
	
	for (i = 0; i < keylen; i++) {
		if(key[i] == '<')  {
			buf[i] = '\0';
			break;
		}
		buf[i] = key[i];
	}
	
	GS_message(("gui_get_word_or_selection\nkey: %s",key));
	key = g_strdelimit(buf, ".,\"<>;:?", ' ');
	key = g_strstrip(key);
	buf2= g_strdup(key);
	g_free(buf);
	return buf2; /* must be freed by calling function */
#else
	if (html_engine_is_selection_active(html->engine)) {
		key = html_engine_get_selection_string(html->engine);
		key = g_strdelimit(key, ".,\"<>;:?", ' ');
		key = g_strstrip(key);
		return g_strdup(key);	/* must be freed by calling function */
	}
#endif
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
	gchar *rtval = NULL;
	gchar *key = NULL;
	gchar *buf = NULL;
	GtkHTML *html;
	gint len;

	html = GTK_HTML(html_widget);
#ifdef USE_GTKHTML38
	if (!html->in_selection) {
		gtk_html_select_word(html);
		key = gtk_html_get_selection_html(html, &len);
		if(!len) {
			return NULL;
		}
		//converts encoding from ncr to utf8
		key = ncr_to_utf8(key);
		
		buf = main_get_striptext_from_string(settings.MainWindowModule, 
						key);	
		
		GS_message(("src/gnome2/html.c: buf=>%s<",buf));
		if(buf == NULL) return;
		key = g_strdelimit(buf, ".,\"<>;:?", ' ');
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
#else
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
			return g_strdup(key);	// * must be freed by calling function *
		}

	}
#endif
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





struct _info {
	GnomeFont *local_font;
	gint page_num, pages;
	gchar *header_title;
	gchar *footer_title;
	gboolean header_date;
	gboolean header_page_num;
	gboolean footer_date;
	gboolean footer_page_num;
};
/*void  print_header(		GtkHTML *html, 
				GtkPrintContext *print_context,				      
				gdouble x, gdouble y, 
				gdouble width, gdouble height, 
				gpointer user_data)*/

static void print_header(GtkHTML * html,
			 GnomePrintContext * print_context, gdouble x,
			 gdouble y, gdouble width, gdouble height,
			 gpointer user_data)
{
	struct _info *info = (struct  _info*) user_data;

#ifndef USE_GTKHTML3_14
#ifdef USE_GTKHTML38	
	if (info->local_font) {
		gnome_print_line_stroked(print_context, x, y, width, y);
		
		gnome_print_gsave(print_context);
		gnome_print_newpath(print_context);
		gnome_print_setrgbcolor(print_context, .0, .0, .0);		
		gnome_print_moveto(print_context, x,
				   y -
				   gnome_font_get_ascender(info->local_font));
		gnome_print_setfont(print_context, info->local_font);
		gnome_print_show(print_context, info->header_title);
		gnome_print_grestore(print_context);
		
	
		info->page_num++;
	}
#endif
#endif
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
static void print_footer(	GtkHTML *html, 
				GnomePrintContext *print_context,				      
				gdouble x, gdouble y, 
				gdouble width, gdouble height, 
				gpointer user_data)
{
	/*gdouble h = height - 10.0;
	struct _info *info = (struct _info *) user_data;

	if (info->local_font) {
		//gnome_print_setfont(print_context, info->local_font);
		//gnome_print_line_stroked(print_context, x, y, width, y);
		//gnome_print_gsave(print_context);
		//gnome_print_newpath(print_context);
		//gnome_print_setrgbcolor(print_context, .0, .0, .0);		
		gnome_print_moveto(print_context, x,
				   y -
				   gnome_font_get_ascender(info->local_font));
		gnome_print_setfont(print_context, info->local_font);
		gnome_print_show(print_context, info->footer_title);*/
		
		/*
		if(info->footer_page_num) {
			char *text =
			    g_strdup_printf(_("Page %d of %d"), info->page_num,
					    info->pages);
			gdouble tw = strlen(text) * 8;
			
			gnome_print_moveto(print_context, x + width - tw,
					   y -
					   gnome_font_get_ascender(info->
								   local_font));
			gnome_print_setfont(print_context, info->local_font);
			gnome_print_show(print_context, (const guchar *)text);
			g_free(text);
		}
		if(info->footer_date) {
			char *text =
			    g_strdup_printf("%s", "April 08, 2005");
			gdouble tw = strlen(text) * 6;
			
			gnome_print_moveto(print_context, x + width - tw,
					   y -
					   gnome_font_get_ascender(info->
								   local_font));
			gnome_print_setfont(print_context, info->local_font);
			gnome_print_show(print_context, (const guchar *)text);	
			g_free(text);
		}
		gnome_print_grestore(print_context);
		info->page_num++;
	}*/
}

static void info_free(struct _info *info)
{
	if (info->local_font)
		gnome_font_unref(info->local_font);
	if(info->header_title)
		g_free(info->header_title);
	if(info->footer_title)
		g_free(info->footer_title);
	g_free(info);
}

static 
struct _info *info_new(GtkHTML * html, GnomePrintContext * pc, gdouble * line)
{
#ifndef USE_GTKHTML3_14
#ifdef USE_GTKHTML38
	struct _info *info;

	info = g_new(struct _info, 1);
	info->local_font = gnome_font_find_closest((const guchar *)"Sans Regular", 10.0);

	if (info->local_font)
		*line =
		    gnome_font_get_ascender(info->local_font) -
		    gnome_font_get_descender(info->local_font);

	info->page_num = 1;
	info->pages = gtk_html_print_get_pages_num(html, pc, *line, *line);
	info->header_title = g_strdup("");
	info->footer_title = g_strdup("");
	info->header_date = FALSE;
	info->header_page_num = TRUE;
	info->footer_date = FALSE;
	info->footer_page_num = TRUE;
	return info;
#endif
#endif
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
#ifndef USE_GTKHTML3_14
#ifdef USE_GTKHTML38
	GtkHTML *html;
	GtkWidget *w = NULL;
	GnomePrintContext *print_context;
	GnomePrintJob *print_master;
	GnomePrintConfig *config = NULL;
	GtkDialog *dialog;
	gdouble line = 0.0;
	struct _info *info;

	if (!preview) {
		dialog =
		    (GtkDialog *) gnome_print_dialog_new(NULL,
						(const guchar *) _("Print"),
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

		//config = gnome_print_config_default();
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

	//gtk_html_print_set_master(html, print_master);

	info = info_new(html, print_context, &line);
	gtk_html_print_with_header_footer(html, print_context, line,
					  line, print_header, print_footer,
					  info);
/*	gtk_html_print_page_with_header_footer  (html,
						   (GtkPrintContext*)print_context,
						   line,
						   line,
						   print_header,
						   print_footer,
						   info);*/
	info_free(info);

	gnome_print_job_close(print_master);

	if (preview) {
		GtkWidget *pw;

		pw = gnome_print_job_preview_new(print_master,
						(const guchar *) _("Print Preview"));
		gtk_widget_show(pw);
	} else {
		int result = gnome_print_job_print(print_master);

		if (result == -1)
			g_warning(_("Failed to print the document"));
	}

	g_object_unref(print_master);
#endif
#endif
}
