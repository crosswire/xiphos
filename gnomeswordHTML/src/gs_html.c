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

#include "gs_html.h"
#include "support.h"
#include "gs_sword.h"
#include "gs_editor.h"


GtkHTMLStream *htmlstream;
GtkHTMLStreamStatus status1;

GtkWidget *htmlCommentaries;
GtkWidget *htmlTexts;
GtkWidget *htmlDict;
GtkWidget *textDict;
GtkWidget *htmlComments;
GtkWidget *noteEditor;
GtkWidget *statusbarNE;

static gboolean was_editable;

GString *gstr;
extern GtkWidget *MainFrm;
extern GtkWidget *textDict;
GString *gs_clipboard; /* declared in gs_gnomesword.c, freed in gs_sword.cpp */
extern GtkWidget *appbar1;
extern gboolean noteModified;
extern gboolean block_font_style_change;
typedef struct _GtkHTMLEditTextProperties GtkHTMLEditTextProperties;
	
/*
#define STYLES 4
static GtkHTMLFontStyle styles [STYLES] = {
	GTK_HTML_FONT_STYLE_BOLD,
	GTK_HTML_FONT_STYLE_ITALIC,
	GTK_HTML_FONT_STYLE_UNDERLINE,
	GTK_HTML_FONT_STYLE_STRIKEOUT,
};
*/

static gboolean
save_receiver  (const HTMLEngine *engine,
		const char *data,
		unsigned int len,
		void *user_data)
{
	static gboolean startgrabing = FALSE;
		
	if(!strncmp(data,"</BODY>",7)) startgrabing = FALSE;
	if(startgrabing)
		gstr = g_string_append(gstr,data);
	if(!strcmp(data,"<BODY>")) startgrabing = TRUE;
	
	return TRUE;
}

/*****************************************************************************
 * savenoteGS - someone clicked save personal  comments
 * choice
*****************************************************************************/
void savenoteHTML(GtkWidget *app)    
{
	GtkWidget *html_widget;
	GtkHTML *html;
	
	html_widget = lookup_widget(app,"htmlComments");
	html = GTK_HTML(html_widget);
	gtk_html_set_editable(html,FALSE); 
	gstr = g_string_new("");
	if (!gtk_html_save(html, (GtkHTMLSaveReceiverFn)save_receiver, GINT_TO_POINTER (0)))
		g_warning("file not writen");		
	else
		g_warning("file writen");
	g_warning(gstr->str);
	savenoteSWORD(gstr->str);
	g_string_free(gstr,1);
	gtk_html_set_editable(html,TRUE); 		
}

/*****************************************************************************
 * boldHTML - 
 * 
*****************************************************************************/
void boldHTML(GtkWidget *widget, GtkWidget *html_widget)    
{
/*	if(!block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (html_widget),
						 GTK_HTML_FONT_STYLE_MAX,
						 GTK_HTML_FONT_STYLE_BOLD);
		else
			gtk_html_set_font_style (GTK_HTML (html_widget), ~GTK_HTML_FONT_STYLE_BOLD, 0);
		noteModified = TRUE;
	}*/
}

/*****************************************************************************
 * linkHTML - 
 * 
*****************************************************************************/
void linkHTML(GtkWidget *html_widget)    
{
	GtkHTML *html;
	gchar *url;
	gchar *target = "";
	
	html = GTK_HTML(html_widget);
	url = NULL;
	url = html->engine->clipboard
	    ? html_object_get_selection_string(html->engine->clipboard)
	    : html_engine_get_selection_string(html->engine);	
	if (*url)
		html_engine_insert_link (html->engine, url, target);
	else
		html_engine_insert_link (html->engine, NULL, NULL);
	noteModified = TRUE;
}

/*****************************************************************************
 * symbolHTML - 
 * 
*****************************************************************************/
void symbolHTML(GtkWidget *html_widget)    
{
	GtkHTML *html;
	gchar *buf;
	gchar tagit[256];
	
	html = GTK_HTML(html_widget);
	buf = NULL;
	buf = html->engine->clipboard
	    ? html_object_get_selection_string(html->engine->clipboard)
	    : html_engine_get_selection_string(html->engine);	
	sprintf(tagit,"<B> </B><FONT FACE=\"symbol\">%s</FONT> ", buf);
	/*
	if (*tagit) {
		html_engine_insert_object(html->engine, HTML_OBJECT(tagit), strlen(tagit)); 
		noteModified = TRUE;
	}
	*/
}

/*****************************************************************************
 * italicHTML - 
 * 
*****************************************************************************/
void italicHTML(GtkWidget *widget, GtkWidget *html_widget)    
{
/*	if(!block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (html_widget),
					GTK_HTML_FONT_STYLE_MAX,
					GTK_HTML_FONT_STYLE_ITALIC);
		else
			gtk_html_set_font_style (GTK_HTML (html_widget), ~GTK_HTML_FONT_STYLE_ITALIC, 0);
		noteModified = TRUE;
	}
*/	
}

/*****************************************************************************
 * underlineHTML - 
 * 
*****************************************************************************/
void underlineHTML(GtkWidget *widget, GtkWidget *html_widget)    
{
/*	if(!block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (html_widget),
					GTK_HTML_FONT_STYLE_MAX,
					GTK_HTML_FONT_STYLE_UNDERLINE);
		else
			gtk_html_set_font_style (GTK_HTML (html_widget), ~GTK_HTML_FONT_STYLE_UNDERLINE, 0);
		noteModified = TRUE;	
	} */
}

/*****************************************************************************
 * strikeoutHTML - 
 * 
*****************************************************************************/
void strikeoutHTML(GtkWidget *widget, GtkWidget *html_widget)    
{
/*	if(!block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (html_widget),
					GTK_HTML_FONT_STYLE_MAX,
					GTK_HTML_FONT_STYLE_STRIKEOUT);
		else
			gtk_html_set_font_style (GTK_HTML (html_widget), ~GTK_HTML_FONT_STYLE_STRIKEOUT, 0);
		noteModified = TRUE;	
	} */
}

/***************************************************************************************************
 *
 ***************************************************************************************************/
static gboolean
on_htmlComments_key_press_event(GtkWidget * widget,
				GdkEventKey * event, gpointer user_data)
{
//	g_warning("key_press_event");
	noteModified = TRUE;	//-- noteeditor.cpp
	return TRUE;
}

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
		gnome_appbar_set_status (GNOME_APPBAR (appbar1), "");
	else{
		if (*url == '#') {
			++url;
			//str = showfirstlineStrongsSWORD(atoi(url));
			sprintf(buf,"Go to Strongs %s",url);
			
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
		gnome_appbar_set_status (GNOME_APPBAR (appbar1), buf);
	}
}

/***************************************************************************************************
 *link in commentary module clicked
 ***************************************************************************************************/
void
on_link_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar *buf, tmpbuf[255];
	gint i=0;
	
	if(*url == '[')   {
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
		lookupStrongsSWORD(atoi(url));
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

/***************************************************************************************************
 *copy menu item clicked in any html window
 *user_data - window (GtkHTML widget) to copy from
 ***************************************************************************************************/
void on_copyhtml_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *widget;
	gchar *buf;
	GtkHTML *html;

	widget = lookup_widget(MainFrm, (gchar *) user_data);

	html = GTK_HTML(widget);
	buf = html->engine->clipboard
	    ? html_object_get_selection_string(html->engine->clipboard)
	    : html_engine_get_selection_string(html->engine);
	g_string_free(gs_clipboard,TRUE);
	gs_clipboard = g_string_new(buf);
	//g_print(gs_clipboard->str);
}
/***************************************************************************************************
 * pasteHTML - 
 *
 ***************************************************************************************************/
void pasteHTML(GtkWidget *html_widget)
{
	GtkHTML *html;	

	html = GTK_HTML(html_widget);
	gtk_html_paste(html);
	//g_print(gs_clipboard->str);
}

/***************************************************************************************************
 *lookup selection in current dict/lex module
 ***************************************************************************************************/
void on_html_lookup_selection_activate(GtkMenuItem * menuitem,
				       gpointer user_data)
{
	GtkWidget *widget;
	gchar *buf;
	GtkHTML *html;

	widget = lookup_widget(MainFrm, (gchar *) user_data);
	html = GTK_HTML(widget);
	buf = NULL;
	buf = html->engine->clipboard
	    ? html_object_get_selection_string(html->engine->clipboard)
	    : html_engine_get_selection_string(html->engine);
	if (buf)
		dictSearchTextChangedSWORD(buf);
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

	widget = lookup_widget(MainFrm, (gchar *) user_data);
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
	EDITOR ed;
	
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
	gtk_html_load_empty(GTK_HTML(htmlCommentaries));	
	 
	ed.vbox = lookup_widget(app,"vbox8"); 
	ed.htmlwidget = gtk_html_new();
	ed.statusbar = gtk_statusbar_new();
	 
	htmlComments = create_editor(app, ed);	  
	statusbarNE = ed.statusbar;
	
	textComp1 = gtk_html_new();
	gtk_widget_ref(textComp1);
	gtk_object_set_data_full(GTK_OBJECT(app), "textComp1",
				 textComp1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(textComp1);
	gtk_container_add(GTK_CONTAINER
			  (lookup_widget(app, "scrolledwindow15")),
			  textComp1);
	gtk_html_load_empty(GTK_HTML(textComp1));		  
			  
	htmlDict = gtk_html_new();
	gtk_widget_ref(htmlDict);	
	gtk_object_set_data_full(GTK_OBJECT(app), "htmlDict",
				 htmlDict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(htmlDict);
	gtk_container_add(GTK_CONTAINER(lookup_widget(app, "scrolledwindow8")), htmlDict);
	gtk_html_load_empty(GTK_HTML(htmlDict));

	gtk_signal_connect(GTK_OBJECT(htmlTexts), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link2_clicked), NULL);			   
	gtk_signal_connect (GTK_OBJECT (htmlTexts), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);			   

	gtk_signal_connect(GTK_OBJECT(htmlCommentaries), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect (GTK_OBJECT (htmlCommentaries), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);			   

	gtk_signal_connect(GTK_OBJECT(ed.htmlwidget), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect (GTK_OBJECT (ed.htmlwidget), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);
	gtk_signal_connect(GTK_OBJECT(ed.htmlwidget), "key_press_event",
			   GTK_SIGNAL_FUNC(on_htmlComments_key_press_event), NULL);		   

	gtk_signal_connect (GTK_OBJECT (textComp1), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);		   
	gtk_signal_connect(GTK_OBJECT(textComp1), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);	
			   
	gtk_signal_connect (GTK_OBJECT (htmlDict), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)app);		   
	gtk_signal_connect(GTK_OBJECT(htmlDict), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);	

}

/***************************************************************************************************
 *beginHTML
 ***************************************************************************************************/
void beginHTML(GtkWidget *html_widget)
{
	GtkHTML *html;
	
	html = GTK_HTML(html_widget);
	was_editable = gtk_html_get_editable (html);
	if (was_editable)
		gtk_html_set_editable (html, FALSE);
	htmlstream = gtk_html_begin(html);
}

/***************************************************************************************************
 *endHTML
 ***************************************************************************************************/
void endHTML(GtkWidget * html_widget)
{
	GtkHTML *html;
	
	html = GTK_HTML(html_widget);	
	
	gtk_html_end(html, htmlstream, status1);
	if (was_editable)
		gtk_html_set_editable (html, TRUE);
}

/***************************************************************************************************
 *displayHTML
 ***************************************************************************************************/
void displayHTML(GtkWidget * html, gchar * txt, gint lentxt)
{
	if (strlen(txt)) {
		gtk_html_write(GTK_HTML(html), htmlstream, txt, lentxt);
	}
}

/***************************************************************************************************
 *gotoanchorHTML
 ***************************************************************************************************/
void gotoanchorHTML(gchar * verse)
{
	gtk_html_jump_to_anchor(GTK_HTML(htmlTexts), verse);
}

/***************************************************************************************************
 *sethtmltoeditHTML
 ***************************************************************************************************/
void sethtmltoeditHTML(gboolean choice)
{
	/* gtk_html_set_default_background_color (GTK_HTML (html_widget), &bgColor); */
	gtk_html_set_editable (GTK_HTML (htmlComments), choice);
}

