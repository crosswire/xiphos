/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */ 

 /*
    * GnomeSword Bible Study Tool
    * gs_html.h
    * -------------------
    * Thu Feb 8 2001
    * copyright (C) 2001 by tbiggs
    * tbiggs@users.sourceforge.net
    *
 */
 
 /*
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

#ifndef __GS_HTML_H_
#define __GS_HTML_H_

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
	/*** set html default font ***/
	void set_gtkhtml_default_font_HTML(GtkWidget *html_widget, gchar *xfontname);
	
#ifdef __cplusplus
}
#endif

#endif /* __GS_HTML_H_ */
