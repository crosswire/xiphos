/***************************************************************************
                          gs_html.h  -  description
                             -------------------
    begin                : Thu Feb 8 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
	
#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

	
	void on_url(GtkHTML * html, const gchar * url, gpointer data);
	void on_link_clicked(GtkHTML * html, const gchar * url,
			     gpointer data);
	void add_gtkhtml_widgets(GtkWidget * app);
	void beginHTML(GtkWidget * html, gboolean utf8);
	void endHTML(GtkWidget * html);
	void displayHTML(GtkWidget * html, const gchar * txt, gint lentxt);
	void on_copyhtml_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
	void on_html_lookup_word_activate(GtkMenuItem * menuitem,
				       gpointer user_data);
	void on_html_lookup_selection_activate(GtkMenuItem * menuitem,
					       gpointer user_data);
	void on_html_goto_reference_activate(GtkMenuItem * menuitem,
					     gpointer user_data);
	void gotoanchorHTML(GtkWidget *html_widget, gchar * verse);
	void sethtmltoeditHTML(gboolean choice);
	gchar *gethtmlfontnameHTML(gchar *xfontname);

#ifdef __cplusplus
}
#endif
