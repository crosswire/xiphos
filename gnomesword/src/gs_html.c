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
#include <gal/widgets/e-unicode.h>

#include "gs_html.h"
#include "support.h"
#include "gs_sword.h"
#include "gs_gnomesword.h"
#include "sw_verselist_sb.h"

GtkHTMLStream *htmlstream;
GtkHTMLStreamStatus status1;

GtkWidget *htmlCommentaries;
GtkWidget *htmlTexts;
GtkWidget *htmlDict;
GtkWidget *textDict;
GtkWidget *htmlComments;
GtkWidget *usehtml;

extern GtkWidget *textDict;
extern GString *gs_clipboard; /* declared in gs_gnomesword.c, freed in gs_sword.cpp */
extern SETTINGS *settings;

/***************************************************************************************************
 *on_url taken form gtkhtml project
 ***************************************************************************************************/
void
on_url (GtkHTML *html, const gchar *url, gpointer data)
{
	GnomeApp *app;
	gchar buf[255];
	
	app = GNOME_APP (data);
	if (url == NULL)
		gnome_appbar_set_status (GNOME_APPBAR (settings->appbar), "");
	else{
		if (*url == '@') {
			++url;
			//str = showfirstlineStrongsSWORD(atoi(url));
			sprintf(buf,"Show %s in main window",url);
		}else if (*url == '#') {
			++url;
			if(*url == 'T') ++url;
			if(*url == 'G'){
				gchar *tmpbuf;
				++url; 
				tmpbuf = g_strdup(url);
				displaydictlexSBSWORD(settings->lex_greek, tmpbuf, settings);
			//gotoBookmarkSWORD(settings->lex_greek, buf);  
				g_free(tmpbuf);
			}
			if(*url == 'H'){
				gchar *tmpbuf;
				++url;  		
				tmpbuf = g_strdup(url);
				displaydictlexSBSWORD(settings->lex_hebrew, tmpbuf, settings);
				//gotoBookmarkSWORD(settings->lex_hebrew, buf);  
				g_free(tmpbuf);
			}		  		
			//str = showfirstlineStrongsSWORD(atoi(url));
			sprintf(buf,"Go to Strongs %s",url);
		}else if (*url == 'M') {
			gchar *tmpbuf;
			++url;
			tmpbuf = g_strdup(url);
			displaydictlexSBSWORD("Packard", tmpbuf, settings);
			g_free(tmpbuf);
			sprintf(buf,"Morph Tag: %s",url);	
		} else if(*url == '[') {
			++url;
			while(*url != ']') {
				++url;
			}
			++url;
			sprintf(buf,"%s",url);	
		} else if(*url == '*') {
			++url;
			sprintf(buf,"%s",url);		
		} else 
			sprintf(buf,"Go to %s",url);
		gnome_appbar_set_status (GNOME_APPBAR (settings->appbar), buf);
	}
}

/***************************************************************************************************
 *link in commentary module clicked
 ***************************************************************************************************/
void
on_link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar 
		*buf,
		*modbuf=NULL,
		tmpbuf[255];
	gchar 
		newmod[80], 
		newref[80];
	gint 
		i=0,
		havemod=0;
	
	if(*url == '@')   {
		++url;
		swapmodsSWORD((gchar*)url);
	 } else if(*url == '[')   {
		++url;
		while(*url != ']') {
			tmpbuf[i++] = *url;
			tmpbuf[i+1] = '\0';
			++url;
		}		
		showmoduleinfoSWORD(tmpbuf); 
	 /*** let's seperate mod version and passage ***/	
	} else if(!strncmp(url, "version=", 7) || !strncmp(url, "passage=", 7)) {	 	
		gchar *mybuf = NULL;
		mybuf = strstr(url, "version=") ;
		if(mybuf){
			mybuf = strchr(mybuf,'=');
			++mybuf;
			i=0;
			while(mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i+1] = '\0';
				++i;
				++havemod;
			}
			//g_warning(newmod);
		}
		mybuf = NULL;
		mybuf = strstr(url, "passage=") ;
		i = 0;
		if(mybuf){
			mybuf = strchr(mybuf,'=');
			++mybuf;
			while(i < strlen(mybuf)) {
				newref[i] = mybuf[i];
				newref[i+1] = '\0';
				++i;
			}
			//g_warning(newref);
		} 
		if(havemod>2){ 
			modbuf = newmod;
			//g_warning("newmod = %s",newmod);
		}else{ 
			modbuf = settings->MainWindowModule;
			//g_warning("modbuf = %s",modbuf);
		}
		buf = g_strdup(newref);
		getVerseListSBSWORD(modbuf, buf, settings);
		g_free(buf);		
	 /*** let's seperate mod version and passage ***/	
	} else if(!strncmp(url, "type=", 5)) {	 	
		gchar *mybuf = NULL;
		gint type=0;
		mybuf = strstr(url, "type=") ;
		if(mybuf){
			mybuf = strchr(mybuf,'=');
			++mybuf;
			i=0;
			while(mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i+1] = '\0';
				++i;
				++havemod;
			}
			//g_warning(newmod);
		}
		mybuf = NULL;
		mybuf = strstr(url, "value=") ;
		i = 0;
		if(mybuf){
			mybuf = strchr(mybuf,'=');
			++mybuf;
			if(mybuf[0] == 'H') type = 0;
			if(mybuf[0] == 'G') type = 1;	
			++mybuf;
			sprintf(newref,"%5.5d",atoi(mybuf));
			//g_warning(newref);
		} 
		if(havemod>2){ 
			if(!strcmp(newmod,"Strongs") && type == 0)
				modbuf = "StrongsHebrew";
			if(!strcmp(newmod,"Strongs") && type == 1)
				modbuf = "StrongsGreek";
			
		}else{ 
			modbuf = "";
			return;
		}
		buf = g_strdup(newref);
		//g_warning("newmod = %s newvalue = %s",modbuf,buf);
		displaydictlexSBSWORD(modbuf, buf, settings);
		g_free(buf);
			
	}else if (*url == '#') {
		++url;		/* remove # */
		if(*url == 'T') ++url;
		if(*url == 'G'){
			++url; 
			buf = g_strdup(url);
			displaydictlexSBSWORD(settings->lex_greek, buf, settings);
			//gotoBookmarkSWORD(settings->lex_greek, buf);  
			g_free(buf);
		}
		if(*url == 'H'){
			++url;  		
			buf = g_strdup(url);
			displaydictlexSBSWORD(settings->lex_hebrew, buf, settings);
			//gotoBookmarkSWORD(settings->lex_hebrew, buf);  
			g_free(buf);
		}		  		
	} else if (*url == 'M') {
		++url;		/* remove M */
		buf = g_strdup(url); 
		displaydictlexSBSWORD("Packard", buf, settings);
		//gotoBookmarkSWORD("Packard", buf);  
		g_free(buf);
	} 
	
}

/***************************************************************************************************
 *link in text window clicked
 ***************************************************************************************************/
static void
on_link2_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar *buf, tmpbuf[255];
	gint i = 0;
	
	if (*url == '#') {
		++url;		/* remove # */
		if(*url == 'T') ++url;
		if(*url == 'G'){
			++url; 
			buf = g_strdup(url);
			//displaydictlexSBSWORD(settings->lex_greek, buf, settings);
			gotoBookmarkSWORD(settings->lex_greek, buf);  
			g_free(buf);
		}
		if(*url == 'H'){
			++url;  		
			buf = g_strdup(url);
			//displaydictlexSBSWORD(settings->lex_hebrew, buf, settings);
			gotoBookmarkSWORD(settings->lex_hebrew, buf);  
			g_free(buf);
		}		  		
	} else if (*url == 'M') {
		++url;		/* remove M */
		buf = g_strdup(url);
		//displaydictlexSBSWORD("Packard", buf, settings);
		gotoBookmarkSWORD("Packard", buf);  
		g_free(buf);
	} else  if(*url == '*')   {
		++url;
		while(*url != ']') {			
			++url;
		} 
		++url;
		buf = g_strdup(url);
		changeVerseSWORD(buf);
		g_free(buf);
	} else if(*url == '[')   {
		++url;
		while(*url != ']') {
			tmpbuf[i++] = *url;
			tmpbuf[i+1] = '\0';
			++url;
		}		
		showmoduleinfoSWORD(tmpbuf);  
	} else {		
		buf = g_strdup(url);
		changeVerseSWORD(buf);
		g_free(buf);
	}
}

/******************************************************************************
 * 
 ******************************************************************************/
static gint
html_button_pressed(GtkWidget * html, GdkEventButton * event,
		    gpointer *data)
{
	usehtml = html;
	settings->whichwindow = GPOINTER_TO_INT(data);
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		//gtk_html_select_word(GTK_HTML(html));
		
		break;
	}
	return 0;
	
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

	widget = lookup_widget(settings->app, (gchar *) user_data);

	html = GTK_HTML(widget);
	gtk_html_copy(html);
	buf = html->engine->clipboard
	    ? html_object_get_selection_string(html->engine->clipboard)
	    : html_engine_get_selection_string(html->engine);
	g_string_free(gs_clipboard,TRUE);
	gs_clipboard = g_string_new(buf);
	//g_print(gs_clipboard->str);
}

/***************************************************************************************************
 *lookup word in dict/lex module
 ***************************************************************************************************/
void on_html_lookup_word_activate(GtkMenuItem * menuitem,
				       gpointer user_data)
{
	GtkWidget *entry, *notebook;
	gchar *buf;
	GtkHTML *html;
	gint page;
		
	page = GPOINTER_TO_INT(user_data);	
	if(page < 1000) {		
		/* set notebook page */		
		notebook = lookup_widget(settings->app,"notebook4");
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),page ); 
	}	
	html = GTK_HTML(usehtml);
	gtk_html_select_word(GTK_HTML(html));
	buf = NULL;
	buf = html->engine->clipboard
	    ? html_object_get_selection_string(html->engine->clipboard)
	    : html_engine_get_selection_string(html->engine);
	if (buf){
		entry = lookup_widget(settings->app,"dictionarySearchText"); 
		gtk_entry_set_text(GTK_ENTRY(entry), buf);	
		//dictSearchTextChangedSWORD(buf);
	}
}
/***************************************************************************************************
 *lookup selection in current dict/lex module
 ***************************************************************************************************/
void on_html_lookup_selection_activate(GtkMenuItem * menuitem,
				       gpointer user_data)
{
	GtkWidget *entry, *notebook;
	gchar *buf;
	GtkHTML *html;
	gint page;
	
	page = GPOINTER_TO_INT(user_data);
	if(page < 1000) {		
		notebook = lookup_widget(settings->app,"notebook4");
		/* set notebook page */
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),page ); 
	}
	//widget = lookup_widget(settings->app, htmlname);	
	html = GTK_HTML(usehtml);
	//gtk_html_select_word(GTK_HTML(html));
	buf = NULL;
	buf = html->engine->clipboard
	    ? html_object_get_selection_string(html->engine->clipboard)
	    : html_engine_get_selection_string(html->engine);
	if (buf){
		entry = lookup_widget(settings->app,"dictionarySearchText"); 
		gtk_entry_set_text(GTK_ENTRY(entry), buf);	
		//dictSearchTextChangedSWORD(buf);
	}
}

/***************************************************************************************************
 *goto selected Bible reference
 ***************************************************************************************************/
void on_html_goto_reference_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{
	GtkWidget *widget;
	gchar *buf;
	GtkHTML *html;

	widget = lookup_widget(settings->app, (gchar *) user_data);
	html = GTK_HTML(widget);
	buf = NULL;
	buf = html->engine->clipboard
	    ? html_object_get_selection_string(html->engine->clipboard)
	    : html_engine_get_selection_string(html->engine);
	if (buf)
		changeVerseSWORD(buf);
}

/***************************************************************************************************
 *add_gtkhtml_widgets -- add the gthhtml widgets
 ***************************************************************************************************/
void add_gtkhtml_widgets(GtkWidget * app)
{
	GtkWidget *textComp1;

	htmlTexts = gtk_html_new();
	gtk_widget_ref(htmlTexts);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "htmlTexts", htmlTexts,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlTexts);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app, "swHtmlBible")),
			  htmlTexts);
	gtk_html_load_empty(GTK_HTML(htmlTexts));

	htmlCommentaries = gtk_html_new();
	gtk_widget_ref(htmlCommentaries);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "htmlCommentaries", htmlCommentaries,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlCommentaries);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app, "swHtmlCom")),
			  htmlCommentaries);

	usehtml = htmlTexts;
	
	htmlComments = gtk_html_new();
	gtk_widget_ref(htmlComments);
	gtk_object_set_data_full(GTK_OBJECT(app), "htmlComments",
				 htmlComments,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlComments);
	gtk_container_add(GTK_CONTAINER
			  (lookup_widget(app, "swHtmlPerCom")),
			  htmlComments);

	textComp1 = gtk_html_new();
	gtk_widget_ref(textComp1);
	gtk_object_set_data_full(GTK_OBJECT(app), "textComp1",
				 textComp1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp1);
	gtk_container_add(GTK_CONTAINER
			  (lookup_widget(app, "scrolledwindow15")),
			  textComp1);
			  
	htmlDict = gtk_html_new();
	gtk_widget_ref(htmlDict);	
	gtk_object_set_data_full(GTK_OBJECT(app), "htmlDict",
				 htmlDict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlDict);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app, "scrolledwindow8")), htmlDict);


	gtk_signal_connect(GTK_OBJECT(htmlTexts), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link2_clicked), NULL);			   
	gtk_signal_connect (GTK_OBJECT (htmlTexts), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);
	gtk_signal_connect(GTK_OBJECT(htmlTexts), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), GINT_TO_POINTER(0));

	gtk_signal_connect(GTK_OBJECT(htmlCommentaries), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect (GTK_OBJECT (htmlCommentaries), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);			   
	gtk_signal_connect(GTK_OBJECT(htmlCommentaries), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), GINT_TO_POINTER(1));
			   
	gtk_signal_connect(GTK_OBJECT(htmlComments), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect (GTK_OBJECT (htmlComments), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);	
	gtk_signal_connect(GTK_OBJECT(htmlComments), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), NULL);		    

	gtk_signal_connect (GTK_OBJECT (textComp1), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);		   
	gtk_signal_connect(GTK_OBJECT(textComp1), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);	
	gtk_signal_connect(GTK_OBJECT(textComp1), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), NULL);		   
			   
	gtk_signal_connect (GTK_OBJECT (htmlDict), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);		   
	gtk_signal_connect(GTK_OBJECT(htmlDict), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);	
	gtk_signal_connect(GTK_OBJECT(htmlDict), "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), GINT_TO_POINTER(2));		   
			   		   
}

/***************************************************************************************************
 *beginHTML - start loading html widget
 ***************************************************************************************************/
void beginHTML(GtkWidget *html_widget, gboolean isutf8)
{
	GtkHTML *html;
	
	html = GTK_HTML(html_widget);
	//was_editable = gtk_html_get_editable (html);
	/*if (was_editable)
		gtk_html_set_editable (html, FALSE);*/	
	if(isutf8){
		htmlstream = gtk_html_begin_content (html, "text/html; charset=utf-8");		
	}else{
		htmlstream = gtk_html_begin(html);
	}
}

/***************************************************************************************************
 *endHTML
 ***************************************************************************************************/
void endHTML(GtkWidget * html)
{
	gtk_html_end(GTK_HTML(html), htmlstream, status1);
}

/***************************************************************************************************
 *displayHTML
 ***************************************************************************************************/
void displayHTML(GtkWidget * html, const gchar * txt, gint lentxt)
{
	if (strlen(txt)) {
		gtk_html_write(GTK_HTML(html), htmlstream, txt, lentxt);
	}
}

/***************************************************************************************************
 *gotoanchorHTML
 ***************************************************************************************************/
void gotoanchorHTML(GtkWidget *html_widget, gchar * verse)
{
	gtk_html_jump_to_anchor(GTK_HTML(html_widget), verse);
}

/***************************************************************************************************
 *sethtmltoeditHTML
 ***************************************************************************************************/
void sethtmltoeditHTML(gboolean choice)
{
	/* gtk_html_set_default_background_color (GTK_HTML (html_widget), &bgColor); */
	gtk_html_set_editable (GTK_HTML (htmlComments), choice);
}

