/***************************************************************************
                          gs_html.h  -  description
                             -------------------
    begin                : Thu Feb 8 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

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

#ifdef __cplusplus
extern "C" {
#endif
	
#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
	
	void savenoteHTML(GtkWidget *app);
	void boldHTML(GtkWidget *widget, GtkWidget *html_widget);
	void italicHTML(GtkWidget *widget, GtkWidget *html_widget);
	void underlineHTML(GtkWidget *widget, GtkWidget *html_widget);
	void strikeoutHTML(GtkWidget *widget, GtkWidget *html_widget);
	void linkHTML(GtkWidget *html_widget);
	void symbolHTML(GtkWidget *html_widget);   
	void on_url(GtkHTML * html, const gchar * url, gpointer data);
	void on_link_clicked(GtkHTML * html, const gchar * url,
			     	gpointer data);
	void on_link2_clicked(GtkHTML * html, const gchar * url, 
				gpointer data);
	void beginHTML(GtkWidget * html, gboolean isutf8); 
	void endHTML(GtkWidget * html);
	void displayHTML(GtkWidget * html, gchar * txt, gint lentxt);
	void on_copyhtml_activate(GtkMenuItem * menuitem,
				  gpointer user_data);
	void pasteHTML(GtkWidget *html_widget);

	void on_html_lookup_selection_activate(GtkMenuItem * menuitem,
					       gpointer user_data);
	void on_html_goto_reference_activate(GtkMenuItem * menuitem,
					     gpointer user_data);
	void gotoanchorHTML(gchar * verse, GtkWidget *html_widget);
	void sethtmltoeditHTML(GtkWidget *html_widget, gboolean choice);
	gboolean on_htmlComments_key_press_event(GtkWidget * widget,
				GdkEventKey * event, gpointer user_data);
				
				
#ifdef __cplusplus
}
#endif
