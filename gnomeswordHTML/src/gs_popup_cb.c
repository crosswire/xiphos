/***************************************************************************
                          gs_popup_cb.c  -  description
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
#include <gtk/gtk.h>
#include <gtkhtml.h>
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>

#include "gs_popup_cb.h"
#include "gs_gnomesword.h"
#include "gssword/gs_sword.h"
#include "gs_html.h"
#include "gs_viewdict_dlg.h"
#include "gs_viewcomm_dlg.h"
#include "gs_viewtext_dlg.h"
#include "support.h"


extern SETTINGS *settings;	/* pointer to settings structure - (declared in gs_gnomesword.c) */
extern GtkWidget *MainFrm;	/* GnomeSword widget (gnome app)(declared and set in gs_sword.cpp) */
extern GtkWidget *NEtext;
extern gboolean noteModified;	/* personal comments window changed */
extern GString *gs_clipboard; /* declared in gs_gnomesword.c, freed in gs_sword.cpp */
extern gboolean autoscroll;
extern gboolean isrunningSD;    /* is the view dictionary dialog runing */
extern gboolean isrunningVC;    /* is the view dictionary dialog runing */
extern gchar current_verse[80];

static gint page_num;
static GnomeFont *font;

/*
 * This code taken form GtkHTML /src/testgtkhtml.c 
 *
 * adds footer with page number to printed output
 */
static void
print_footer (GtkHTML *html, GnomePrintContext *context,
	      gdouble x, gdouble y, gdouble width, gdouble height, gpointer user_data)
{
	gchar *text = g_strdup_printf ("- %d -", page_num);
	gdouble tw = gnome_font_get_width_string (font, "text");

	if (font) {
		gnome_print_newpath     (context);
		gnome_print_setrgbcolor (context, .0, .0, .0);
		gnome_print_moveto      (context, x + (width - tw)/2, y - (height + gnome_font_get_ascender (font))/2);
		gnome_print_setfont     (context, font);
		gnome_print_show        (context, text);
	}

	g_free (text);
	page_num++;
}

/*
 * This code taken form GtkHTML /src/testgtkhtml.c 
 *
 * print_preview_cb- print preview of current commentary module
 */
void print_preview_cb (GtkMenuItem * menuitem,  gpointer data)
{
	GnomePrintMaster *print_master;
	GnomePrintContext *print_context;
	GtkWidget *preview;
	GtkHTML *html;
	
	html = GTK_HTML(lookup_widget(MainFrm, (gchar *) data));
	print_master = gnome_print_master_new ();
	/*  gnome_print_master_set_paper (master, gnome_paper_with_name ("A4")); */

	print_context = gnome_print_master_get_context (print_master);

	page_num = 1;
	font = gnome_font_new_closest ("Helvetica", GNOME_FONT_BOOK, FALSE, 12);
	gtk_html_print_with_header_footer (html, print_context, .0, .03, NULL, print_footer, NULL);
	if (font) gtk_object_unref (GTK_OBJECT (font));

	preview = GTK_WIDGET (gnome_print_master_preview_new (print_master, "GnomeSword Print Preview"));
	gtk_widget_show (preview);

	gtk_object_unref (GTK_OBJECT (print_master));
}

//----------------------------------------------------------------------------------------------
//void on
//----------------------------------------------------------------------------------------------
void on_boldNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	
	boldHTML( lookup_widget(MainFrm ,"btnBold"), lookup_widget(MainFrm ,(gchar *)user_data));
	
}

//----------------------------------------------------------------------------------------------
void on_italicNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?	
	italicHTML( lookup_widget(MainFrm ,"btnItalic"), lookup_widget(MainFrm ,(gchar *)user_data));

}

//----------------------------------------------------------------------------------------------
void on_referenceNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{

	if (!settings->formatpercom)
		return;		//-- do we want formatting?	
	linkHTML(lookup_widget(MainFrm ,(gchar *)user_data));
}

//----------------------------------------------------------------------------------------------
void on_underlineNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	if(GTK_EDITABLE(NEtext)->has_selection)  //-- do we have a selection?
	{
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "<U>", -1);
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "</u>", -1);
		noteModified = TRUE;
	}
}

//----------------------------------------------------------------------------------------------
void on_greekNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	
	symbolHTML(lookup_widget(MainFrm ,(gchar *)user_data));
	/*
	if(GTK_EDITABLE(NEtext)->has_selection)  //-- do we have a selection?
	{
	  gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "<FONT FACE=\"symbol\">", -1);
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "</font>", -1);
		noteModified = TRUE;
	}
	*/
}

//----------------------------------------------------------------------------------------------
void on_goto_reference_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *buf;

	if (!GTK_EDITABLE(NEtext)->has_selection)
		return;		//-- do we have a selection?
	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(NEtext),
				   GTK_EDITABLE(NEtext)->selection_start_pos,
				   GTK_EDITABLE(NEtext)->selection_end_pos);
	changeVerseSWORD(buf);
		
}

//----------------------------------------------------------------------------------------------
void
on_goto_reference2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*
	GtkWidget *text;
	gchar *buf;

	if (!GTK_EDITABLE(text)->has_selection)
		return;		//-- do we have a selection?
	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(text),
				   GTK_EDITABLE(text)->selection_start_pos,
				   GTK_EDITABLE(text)->selection_end_pos);

	changeVerseSWORD(buf);
*/
}

void
on_viewtext_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg;
	
	dlg = create_dlgViewText ();
	gtk_widget_show(dlg);
}
//----------------------------------------------------------------------------------------------
void
on_about_this_module5_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(2));
}

//----------------------------------------------------------------------------------------------
void on_lookup_word1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(MainFrm, "dictionarySearchText");
	text = lookup_widget(MainFrm, "textDict");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
		dictSearchTextChangedSWORD(buf);
	}
}
//----------------------------------------------------------------------------------------------
void
on_lookup_selection_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(MainFrm, "dictionarySearchText");
	text = lookup_widget(MainFrm, "moduleText");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		dictSearchTextChangedSWORD(buf);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
	}
}

//----------------------------------------------------------------------------------------------
void
on_lookup_selection2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(MainFrm, "dictionarySearchText");
	text = lookup_widget(MainFrm, "textCommentaries");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
		dictSearchTextChangedSWORD(buf);
	}*/
}


//----------------------------------------------------------------------------------------------
void on_john_3_1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	changeVerseSWORD((gchar *) user_data);
}

//----------------------------------------------------------------------------------------------
void on_mainText_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *modName;

	modName = (gchar *) user_data;
	changecurModSWORD(modName,TRUE);
}

//----------------------------------------------------------------------------------------------
void on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	if (GTK_TOGGLE_BUTTON(lookup_widget(MainFrm, "btnEditNote"))->
	    active) {
		text = lookup_widget(MainFrm, "textComments");
		gtk_editable_cut_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
	}
}

//----------------------------------------------------------------------------------------------
void on_copy4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(MainFrm, "textComments");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;

	text = lookup_widget(MainFrm, "htmlComments");
	pasteHTML(text);
}

//----------------------------------------------------------------------------------------------
void on_kjv1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD((char *) user_data);
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(0));
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(3));
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module3_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(4));
}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_about_this_module4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(5));
}
		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_about_this_module6_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD(getmodnameSWORD(1));
}
		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_auto_scroll1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *toolbar;

	toolbar = lookup_widget(MainFrm, "handlebox17");

	autoscroll = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (autoscroll)
		gtk_widget_hide(toolbar);
	else
		gtk_widget_show(toolbar);
}
		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_lookup_selection4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(MainFrm, "dictionarySearchText");
	text = lookup_widget(MainFrm, "textComments");
	if (GTK_EDITABLE(text)->has_selection) {
		buf =
		    gtk_editable_get_chars(GTK_EDITABLE(text),
					   GTK_EDITABLE(text)->
					   selection_start_pos,
					   GTK_EDITABLE(text)->
					   selection_end_pos);
		gtk_entry_set_text(GTK_ENTRY(entry), buf);
		dictSearchTextChangedSWORD(buf);
	}
}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_goto_reference3_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	gchar *buf;

	text = lookup_widget(MainFrm, "textDict");
	if (!GTK_EDITABLE(text)->has_selection)
		return;		//-- do we have a selection?
	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(text),
				   GTK_EDITABLE(text)->selection_start_pos,
				   GTK_EDITABLE(text)->selection_end_pos);
	
	changeVerseSWORD(buf);
}
		

/*******************************************************************************
 * show hide commentary notebook tabs - popup menu choice
 *******************************************************************************/	
void on_show_tabs1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		gtk_widget_show(lookup_widget(MainFrm, "notebook1"));
	else
		gtk_widget_hide(lookup_widget(MainFrm, "notebook1"));
}
		

/*******************************************************************************
 * show hide dict/lex notebook tabs - popup menu choice
 *******************************************************************************/	
void on_show_tabs2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		gtk_widget_show(lookup_widget(MainFrm, "notebook4"));
	else
		gtk_widget_hide(lookup_widget(MainFrm, "notebook4"));
}

/*******************************************************************************
 *
 *******************************************************************************/
void on_view_in_new_window_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	extern GtkWidget *frameShowDict;
	static GtkWidget *dlg;
	gchar *modName;
        GdkCursor *cursor;	
	gtk_widget_show(MainFrm);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(MainFrm->window,cursor);
	
	if(!isrunningSD) {
		dlg = create_dlgViewDict(MainFrm);
		modName = getdictmodSWORD();
		gtk_frame_set_label(GTK_FRAME(frameShowDict),modName);  /* set frame label to current Module name  */				
		//initSD(modName);
		isrunningSD = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(MainFrm);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(MainFrm->window,cursor);
}

/*******************************************************************************
 *
 *******************************************************************************/
void on_view_in_new_window2_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	static GtkWidget *dlg;
        GdkCursor *cursor;	
	
	gtk_widget_show(MainFrm);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(MainFrm->window,cursor);
	
	if(!isrunningVC) {
		dlg = create_dlgViewComm(MainFrm);
		isrunningVC = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(MainFrm);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(MainFrm->window,cursor);
}

/*******************************************************************************
 *
 *******************************************************************************/
void on_change_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	changepercomModSWORD((gchar *) user_data);
}		

/*******************************************************************************
 *
 *******************************************************************************/		
void on_changeint1mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	sprintf(settings->Interlinear1Module,"%s",(gchar *)user_data);
	changeVerseSWORD(current_verse);
}	
		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint2mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	sprintf(settings->Interlinear2Module,"%s",(gchar *)user_data);
	changeVerseSWORD(current_verse);	

}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint3mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	sprintf(settings->Interlinear3Module,"%s",(gchar *)user_data);
	changeVerseSWORD(current_verse);

}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint4mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	sprintf(settings->Interlinear4Module,"%s",(gchar *)user_data);
	changeVerseSWORD(current_verse);

}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint5mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	sprintf(settings->Interlinear5Module,"%s",(gchar *)user_data);
	changeVerseSWORD(current_verse);

}

