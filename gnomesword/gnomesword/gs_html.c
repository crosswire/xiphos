/***************************************************************************
                          gs_html.c  -  description
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif



#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include "gs_html.h"
#include "support.h"
#include "gs_sword.h"

GtkHTMLStream *htmlstream;
GtkHTMLStreamStatus status1; 
GtkWidget *htmlCommentaries;
GtkWidget *htmlTexts;

extern GtkWidget *MainFrm;


/***************************************************************************************************
 *link in commentary module clicked
 ***************************************************************************************************/
static void
on_link_clicked (GtkHTML *html, const gchar *url, gpointer data)
{	
	changeVerseSWORD(url);
}

/***************************************************************************************************
 *link in text window clicked
 ***************************************************************************************************/
static void
on_link2_clicked (GtkHTML *html, const gchar *url, gpointer data)
{	
	//g_warning(url);
	changeVerseSWORD(url);
}

/***************************************************************************************************
 *copy menu item clicked in any html window
 *user_data - window (GtkHTML widget) to copy from
 ***************************************************************************************************/
void on_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *html;
	html = lookup_widget(MainFrm, (gchar *) user_data);
	//g_warning((gchar *) user_data);
	gtk_html_copy(GTK_HTML(html)); 	
}

/***************************************************************************************************
 *add_gtkhtml_widgets -- add the gthhtml widgets
 ***************************************************************************************************/
void add_gtkhtml_widgets(GtkWidget *app)
{       	
	GtkWidget *htmlComments;
	GtkWidget *textComp1;
	GtkWidget *textComp2;
	GtkWidget *textComp3;	
/*	GdkColor   bgColor = {0, 0xc7ff, 0xc7ff, 0xc7ff};
	
	gdk_rgb_init ();
	
	gtk_widget_set_default_colormap (gdk_rgb_get_cmap ());
	gtk_widget_set_default_visual (gdk_rgb_get_visual ());
*/	
	htmlTexts = gtk_html_new();
	gtk_widget_ref(htmlTexts);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "htmlTexts", htmlTexts,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlTexts);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"swHtmlBible")),
			  	htmlTexts);
	gtk_html_load_empty(GTK_HTML(htmlTexts));
/*	gtk_html_set_default_background_color(GTK_HTML(htmlTexts), &bgColor);
*/
			  	
	htmlCommentaries = gtk_html_new();
	gtk_widget_ref(htmlCommentaries);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "htmlCommentaries", htmlCommentaries,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlCommentaries);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"swHtmlCom")),
			  	htmlCommentaries);

	
        htmlComments = gtk_html_new();
	gtk_widget_ref(htmlComments);
	gtk_object_set_data_full(GTK_OBJECT(app), "htmlComments",
				 htmlComments,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlComments);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"swHtmlPerCom")),
				htmlComments);
				
	textComp1 = gtk_html_new();
	gtk_widget_ref(textComp1);
	gtk_object_set_data_full(GTK_OBJECT(app), "textComp1",
				 textComp1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp1);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"scrolledwindow15")), textComp1);
				
	textComp2 = gtk_html_new();
	gtk_widget_ref(textComp2);
	gtk_object_set_data_full(GTK_OBJECT(app), "textComp2",
				 textComp2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp2);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"scrolledwindow16")), textComp2);
	
	textComp3 = gtk_html_new();
	gtk_widget_ref(textComp3);
	gtk_object_set_data_full(GTK_OBJECT(app), "textComp3",
				 textComp3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp3);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"scrolledwindow21")), textComp3);
	
	gtk_signal_connect (GTK_OBJECT (htmlTexts), "link_clicked",
			    GTK_SIGNAL_FUNC (on_link2_clicked), NULL);			
				
	gtk_signal_connect (GTK_OBJECT (htmlCommentaries), "link_clicked",
			    GTK_SIGNAL_FUNC (on_link_clicked), NULL);	
			    		
	gtk_signal_connect (GTK_OBJECT (htmlComments), "link_clicked",
			    GTK_SIGNAL_FUNC (on_link_clicked), NULL);
}

/***************************************************************************************************
 *beginHTML
 ***************************************************************************************************/
void beginHTML(GtkWidget *html)
{
        htmlstream = gtk_html_begin(GTK_HTML(html));
}

/***************************************************************************************************
 *endHTML
 ***************************************************************************************************/
void endHTML(GtkWidget *html)
{
	gtk_html_end(GTK_HTML(html),
		htmlstream,
		status1);
}

/***************************************************************************************************
 *displayHTML
 ***************************************************************************************************/
void displayHTML(GtkWidget *html, gchar *txt, gint lentxt)
{        
	if(strlen(txt)){		
	 	gtk_html_write(GTK_HTML(html),
			htmlstream,
			txt,
			lentxt);
	}
}

/***************************************************************************************************
 *gotoanchorHTML
 ***************************************************************************************************/
void gotoanchorHTML(gchar *verse)
{
	gtk_html_jump_to_anchor(GTK_HTML(htmlTexts),verse);
}

