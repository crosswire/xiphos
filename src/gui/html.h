/*
 * GnomeSword Bible Study Tool
 * html.h - GtkHtml gui stuff
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
 
#ifndef __HTML_H_
#define __HTML_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

void
url_requested (GtkHTML *html, const gchar *url, GtkHTMLStream *handle);	
void gui_url(GtkHTML * html, const gchar * url, gpointer data);
void gui_link_clicked(GtkHTML * html, const gchar * url, gpointer data);
void gui_begin_html(GtkWidget * html, gboolean utf8);
void gui_end_html(GtkWidget * html);
void gui_display_html(GtkWidget * html, const gchar * txt, gint lentxt);
void gui_copy_html(GtkWidget *html_widget);
void gui_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data);
gchar *gui_get_word_or_selection(GtkWidget *html_widget, gboolean word);
gchar *gui_button_press_lookup(GtkWidget *html_widget);
void gui_html_print(GtkWidget * htmlwidget, gboolean preview);

#ifdef __cplusplus
}
#endif

#endif
