/*
 * GnomeSword Bible Study Tool
 * hints.c - create hint_window dialog and display hints
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

#include "gui/hints.h"
#include "gui/sidebar.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/module.h"
#include "main/xml.h"
#include "main/settings.h"

#define HINT_COLOR "#ffffbf"
HINT hint;

/******************************************************************************
 * Name
 *    
 *
 * Synopsis
 *   #include "gui/hints.h"
 *
 *    
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

void gui_display_hint_in_viewer(gchar * text)
{
	GtkHTMLStreamStatus status1 = 0;
	GtkHTML *html;
	GtkHTMLStream *htmlstream;
	GString *str;

	str = g_string_new("");
	if (hint.use_hints) {
		html = GTK_HTML(sidebar.html_viewer_widget);
		htmlstream =
		    gtk_html_begin_content(html,
					   "text/html; charset=utf-8");
		g_string_sprintf(str,
				 "<body bgcolor=\"yellow\">%s</body>",
				 text);
		gtk_html_write(GTK_HTML(html), htmlstream, str->str,
			       str->len);
		gtk_html_end(GTK_HTML(html), htmlstream, status1);
		g_string_free(str, TRUE);
	}
}


/******************************************************************************
 * Name
 *   create_hint_window
 *
 * Synopsis
 *   #include "gui/hints.h"
 *
 *   GtkWidget *create_hint_window (void)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget*
 */

static void create_hint_window(void)
{
	GtkWidget *hint_window;
	GtkWidget *frame;
	GtkWidget *scrolledwindow72;

	hint.hint_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
/*	gtk_object_set_data(GTK_OBJECT(hint_window), "hint_window",
			    hint_window);*/
	gtk_window_set_title(GTK_WINDOW(hint.hint_window), "window1");
	gtk_window_set_position(GTK_WINDOW(hint.hint_window),
				GTK_WIN_POS_MOUSE);
	gtk_window_set_default_size(GTK_WINDOW(hint.hint_window), 191, 83);
	gtk_window_set_resizable(GTK_WINDOW(hint.hint_window),FALSE);
	gtk_window_set_decorated(GTK_WINDOW(hint.hint_window), FALSE);
	gtk_widget_show(hint.hint_window);

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_container_add(GTK_CONTAINER(hint.hint_window), frame);

	scrolledwindow72 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow72);
	gtk_container_add(GTK_CONTAINER(frame), scrolledwindow72);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow72),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_NEVER);

	hint.html_widget = gtk_html_new();
	gtk_html_load_empty(GTK_HTML(hint.html_widget));
	gtk_widget_show(hint.html_widget);
	gtk_widget_set_size_request(hint.html_widget, 191, 83);
	gtk_container_add(GTK_CONTAINER(scrolledwindow72),
			  hint.html_widget);
	hint.in_popup = TRUE;
	//return hint.hint_window;
}


/******************************************************************************
 * Name
 *   gui_show_footnote
 *
 * Synopsis
 *   #include "gui/hints.h"
 *
 *   void gui_show_footnote(gchar *note)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_display_in_hint_window(gchar * note)
{
	GtkHTMLStreamStatus status1 = 0;
	GtkHTML *html;
	GtkHTMLStream *htmlstream;
	GString *str;
	gint x;
	gint y;
	gint size_of_note = 0;
	
	/* FIXME */
	size_of_note = strlen(note);
	size_of_note = ((size_of_note / 24) + 2);
	size_of_note = size_of_note * 18;
	
	hint.hint_window = NULL;
	
	if (!hint.in_popup) {
		//hint.hint_window = 
		create_hint_window();
		if(!hint.hint_window)
			return;
		
		gtk_window_set_modal(GTK_WINDOW(hint.hint_window), TRUE);
		str = g_string_new("");
		html = GTK_HTML(hint.html_widget);
		htmlstream =
		    gtk_html_begin_content(html, "text/html; charset=utf-8");
		g_string_sprintf(str, "<body bgcolor=\"%s\">%s</body>",
							 HINT_COLOR, note);
		
		gtk_html_write(GTK_HTML(html), htmlstream, str->str, str->len);
		
		gtk_html_end(GTK_HTML(html), htmlstream, status1);
		
		gtk_widget_set_size_request(hint.html_widget, 191, size_of_note);
		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
		
		gtk_window_get_position(GTK_WINDOW(hint.hint_window), &x, &y);
		x += 10;
		y += 10;
		
		gtk_window_move(GTK_WINDOW(hint.hint_window), x, y);
		g_string_free(str, TRUE);
		gtk_window_set_modal(GTK_WINDOW(hint.hint_window), FALSE);
	}
}


/******************************************************************************
 * Name
 *   gui_destroy_hint_window
 *
 * Synopsis
 *   #include "gui/hints.h"
 *
 *   void gui_destroy_hint_window(void) 
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_destroy_hint_window(void)
{
	if (hint.in_popup) {
		if(hint.hint_window)
			gtk_widget_destroy(hint.hint_window);
		hint.hint_window = NULL;
		hint.in_popup = FALSE;
	}
}

/******************************************************************************
 * Name
 *   gui_open_hint_viewer
 *
 * Synopsis
 *   #include "gui/hints.h"
 *
 *    void gui_open_hint_viewer(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_open_hint_viewer(void)
{
	gtk_option_menu_set_history(GTK_OPTION_MENU
				    (sidebar.optionmenu1), 4);
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_sidebar),
			      4);
}
