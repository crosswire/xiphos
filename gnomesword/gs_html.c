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


#ifdef USE_GTKHTML
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlobject.h>
#include <gtkhtml/htmlengine-edit-clueflowstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-rule.h>
#include <gtkhtml/htmlengine-edit-movement.h>
#include <gtkhtml/htmlengine-edit-cursor.h>
#include <gtkhtml/htmlengine-edit-text.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-print.h>
#include <gtkhtml/htmlengine-save.h>
#include <gtkhtml/htmlsettings.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlcursor.h>
#endif /* GTK_HTML */

#include "gs_html.h"
#include "support.h"
#include "gs_sword.h"

#ifdef USE_GTKHTML	
GtkHTMLStream *htmlstream;
GtkHTMLStreamStatus status1; 
#endif /* GTK_HTML */

GtkWidget *htmlCommentaries;
GtkWidget *htmlTexts;

extern  GtkWidget *MainFrm;

/***************************************************************************************************
 *link in commentary module clicked
 ***************************************************************************************************/
#ifdef USE_GTKHTML	 
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
	if(*url == '#') {
		++url;
	     	lookupStrongsSWORD(atoi(url));
	}else changeVerseSWORD(url);
}


/***************************************************************************************************
 *copy menu item clicked in any html window
 *user_data - window (GtkHTML widget) to copy from
 ***************************************************************************************************/
void on_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *widget;
	gchar *buf;
	GtkHTML *html;
	
	widget= lookup_widget(MainFrm, (gchar *) user_data);
	html = GTK_HTML (widget);
	buf = html->engine->clipboard	
		? html_object_get_selection_string(html->engine->clipboard)
		: html_engine_get_selection_string (html->engine);
	//g_warning(buf);
}

/***************************************************************************************************
 *lookup selection in current dict/lex module
 ***************************************************************************************************/
void on_html_lookup_selection_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *widget;
	gchar *buf;
	GtkHTML *html;
	
	widget= lookup_widget(MainFrm, (gchar *) user_data);
	html = GTK_HTML (widget);
	buf = NULL; 
	buf = html->engine->clipboard	
		? html_object_get_selection_string(html->engine->clipboard)
		: html_engine_get_selection_string (html->engine);
	if(buf)dictSearchTextChangedSWORD(buf);
}

/***************************************************************************************************
 *goto selected Bible reference
 ***************************************************************************************************/
void on_html_goto_reference_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *widget;
	gchar *buf;
	GtkHTML *html;
	
	widget= lookup_widget(MainFrm, (gchar *) user_data);
	html = GTK_HTML (widget);
	buf = NULL; 
	buf = html->engine->clipboard	
		? html_object_get_selection_string(html->engine->clipboard)
		: html_engine_get_selection_string (html->engine);
	if(buf)changeVerseSWORD(buf);
}
#endif /* GTK_HTML */

/***************************************************************************************************
 *add_gtkhtml_widgets -- add the gthhtml widgets
 ***************************************************************************************************/
void add_gtkhtml_widgets(GtkWidget *app)
{       	
	GtkWidget *textComp1;
	GtkWidget *textComp2;
	GtkWidget *textComp3;	

#ifdef USE_GTKHTML		
	GtkWidget *htmlComments;

	htmlTexts = gtk_html_new();
	gtk_widget_ref(htmlTexts);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "htmlTexts", htmlTexts,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlTexts);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"swHtmlBible")),
			  	htmlTexts);
	gtk_html_load_empty(GTK_HTML(htmlTexts));
			  	
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
				
#else				
	textComp1 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textComp1);
	gtk_object_set_data_full(GTK_OBJECT(app), "textComp1",
				 textComp1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp1);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"scrolledwindow15")), textComp1);
	
	textComp2 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textComp2);
	gtk_object_set_data_full(GTK_OBJECT(app), "textComp2",
				 textComp2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp2);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"scrolledwindow16")), textComp2);		
	
	textComp3 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(textComp3);
	gtk_object_set_data_full(GTK_OBJECT(app), "textComp3",
				 textComp3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp3);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app,"scrolledwindow21")), textComp3);	
	
					
		
#endif /* GTK_HTML */

}

/***************************************************************************************************
 *beginHTML
 ***************************************************************************************************/
void beginHTML(GtkWidget *html)
{
#ifdef USE_GTKHTML	
        htmlstream = gtk_html_begin(GTK_HTML(html));
#endif /* GTK_HTML */        
}

/***************************************************************************************************
 *endHTML
 ***************************************************************************************************/
void endHTML(GtkWidget *html)
{
#ifdef USE_GTKHTML	
	gtk_html_end(GTK_HTML(html),
		htmlstream,
		status1);
#endif /* GTK_HTML */		
}

/***************************************************************************************************
 *displayHTML
 ***************************************************************************************************/
void displayHTML(GtkWidget *html, gchar *txt, gint lentxt)
{  
#ifdef USE_GTKHTML	      
	if(strlen(txt)){		
	 	gtk_html_write(GTK_HTML(html),
			htmlstream,
			txt,
			lentxt);
	}
#endif /* GTK_HTML */	
}

/***************************************************************************************************
 *gotoanchorHTML
 ***************************************************************************************************/
void gotoanchorHTML(gchar *verse)
{
#ifdef USE_GTKHTML	
	gtk_html_jump_to_anchor(GTK_HTML(htmlTexts),verse);
#endif /* GTK_HTML */	
}

