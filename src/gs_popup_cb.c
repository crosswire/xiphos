/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
 /*
    * GnomeSword Bible Study Tool
    * gs_popup_cb.c
    * -------------------
    * Thu Feb 8 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
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
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "gs_popup_cb.h"
#include "gs_unlockkey_dlg.h"
#include "gs_gnomesword.h"
#include "sw_gnomesword.h"
#include "gs_viewdict_dlg.h"
#include "gs_viewcomm_dlg.h"
#include "gs_viewtext_dlg.h"
#include "gs_bookmarks.h"
#include "support.h"


/******************************************************************************
 * externals
******************************************************************************/
extern SETTINGS 
	*settings;	/* pointer to settings structure - (declared in gs_gnomesword.c) */
extern GtkWidget 
	*NEtext;
extern GString 
	*gs_clipboard; /* declared in gs_gnomesword.c, freed in gs_sword.cpp */
extern gboolean 
	autoscroll,  /* is commentary window set to scroll with text window */
	isrunningSD,    /* is the view dictionary dialog runing */
	isrunningVC,    /* is the view commentary dialog runing */
	isrunningVT,    /* is the view text dialog runing */
	noteModified;	/* personal comments window changed */
extern gchar 
	current_verse[80];
	
	

//----------------------------------------------------------------------------------------------
void on_unlock_key_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg;
	
	dlg = create_dlgUnlockKey();
	gtk_widget_show(dlg);
}

//----------------------------------------------------------------------------------------------
void on_boldNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?
	if(GTK_EDITABLE(NEtext)->has_selection)  //-- do we have a selection?
	{
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "<B>", -1);
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "</b>", -1);
	}
}

//----------------------------------------------------------------------------------------------
void on_italicNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings->formatpercom)
		return;		//-- do we want formatting?	
	if(GTK_EDITABLE(NEtext)->has_selection)  //-- do we have a selection?
	{
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "<I>", -1);
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "</i>", -1);
	}

}

//----------------------------------------------------------------------------------------------
void on_referenceNE_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar	*buf,
			tmpbuf[256];
	
	if (!settings->formatpercom)
		return;		//-- do we want formatting?	
	if(GTK_EDITABLE(NEtext)->has_selection)  //-- do we have a selection?
	{
		buf = gtk_editable_get_chars(GTK_EDITABLE(NEtext), GTK_EDITABLE(NEtext)->selection_start_pos, GTK_EDITABLE(NEtext)->selection_end_pos);	
		sprintf(tmpbuf,"<a href=\"passage=%s\">",buf);
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, tmpbuf, -1);
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "</a>", -1);
	}
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
	if(GTK_EDITABLE(NEtext)->has_selection)  //-- do we have a selection?
	{
	  gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "<FONT FACE=\"symbol\">", -1);
		gtk_text_set_point(GTK_TEXT(NEtext), GTK_EDITABLE(NEtext)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(NEtext), NULL, &NEtext->style->black, NULL, "</font>", -1);
		noteModified = TRUE;
	}
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
	GtkWidget *text;
	gchar *buf;

	text = lookup_widget(settings->app, "textCommentaries");
	if (!GTK_EDITABLE(text)->has_selection)
		return;		//-- do we have a selection?
	buf =
	    gtk_editable_get_chars(GTK_EDITABLE(text),
				   GTK_EDITABLE(text)->selection_start_pos,
				   GTK_EDITABLE(text)->selection_end_pos);

	changeVerseSWORD(buf);
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

	entry = lookup_widget(settings->app, "dictionarySearchText");
	text = lookup_widget(settings->app, "textDict");
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

	entry = lookup_widget(settings->app, "dictionarySearchText");
	text = lookup_widget(settings->app, "moduleText");
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
	GtkWidget *entry, *text;
	gchar *buf;

	entry = lookup_widget(settings->app, "dictionarySearchText");
	text = lookup_widget(settings->app, "textCommentaries");
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
void on_john_3_1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	changeVerseSWORD((gchar *) user_data);
}

//----------------------------------------------------------------------------------------------
void on_mainText_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings->app, "nbTextMods");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page	
}

//----------------------------------------------------------------------------------------------
void on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	if (GTK_TOGGLE_BUTTON(lookup_widget(settings->app, "btnEditNote"))->
	    active) {
		text = lookup_widget(settings->app, "textComments");
		gtk_editable_cut_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
	}
}

//----------------------------------------------------------------------------------------------
void on_copy4_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(settings->app, "textComments");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;

	text = lookup_widget(settings->app, "textComments");
	gtk_text_insert(GTK_TEXT(text), NULL, //&gtkText->style->black
			NULL, NULL,
			gs_clipboard->str, -1);
}

//----------------------------------------------------------------------------------------------
void on_kjv1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	showmoduleinfoSWORD((char *) user_data);
}

//----------------------------------------------------------------------------------------------
void
on_about_this_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if(!strcmp((gchar *)user_data,"Bible")) showmoduleinfoSWORD(getmodnameSWORD(0));
	else if (!strcmp((gchar *)user_data,"commentary")) showmoduleinfoSWORD(getmodnameSWORD(1));
	else if (!strcmp((gchar *)user_data,"dictionary")) showmoduleinfoSWORD(getmodnameSWORD(2));	
	
}

/*******************************************************************************
 *
 *******************************************************************************/	
void on_auto_scroll1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *toolbar;

	toolbar = lookup_widget(settings->app, "handlebox17");

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

	entry = lookup_widget(settings->app, "dictionarySearchText");
	text = lookup_widget(settings->app, "textComments");
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
 * show hide notebook tabs - popup menu choice
 *******************************************************************************/	
void on_show_tabs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		gtk_widget_show(lookup_widget(settings->app, (gchar *)user_data));		
	else
		gtk_widget_hide(lookup_widget(settings->app, (gchar *)user_data));
	if(!strcmp((gchar *)user_data, "nbTextMods")) 
		settings->text_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if(!strcmp((gchar *)user_data, "notebook1")) 
		settings->comm_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if(!strcmp((gchar *)user_data, "notebook4")) 
		settings->dict_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
}
	
/*******************************************************************************
 * display new dictionary window
 *******************************************************************************/
void on_view_in_new_window_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	extern GtkWidget *frameShowDict;
	static GtkWidget *dlg;
	gchar *modName;
        GdkCursor *cursor;	
	gtk_widget_show(settings->app);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(settings->app->window,cursor);
	
	if(!isrunningSD) {
		dlg = create_dlgViewDict(settings->app);
		modName = getdictmodSWORD();
		gtk_frame_set_label(GTK_FRAME(frameShowDict),modName);  /* set frame label to current Module name  */				
		//initSD(modName);
		isrunningSD = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(settings->app);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(settings->app->window,cursor);
}

/*******************************************************************************
 * display new commentary window
 *******************************************************************************/
void on_view_in_new_window2_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	static GtkWidget *dlg;
        GdkCursor *cursor;	
	
	gtk_widget_show(settings->app);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(settings->app->window,cursor);
	
	if(!isrunningVC) {
		dlg = create_dlgViewComm(settings->app);
		isrunningVC = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(settings->app);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(settings->app->window,cursor);
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
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
	g_warning(modName);
	sprintf(settings->Interlinear1Module,"%s",modName);
	changeVerseSWORD(current_verse);
}	
		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint2mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
	g_warning(modName);
	sprintf(settings->Interlinear2Module,"%s",modName);
	changeVerseSWORD(current_verse);
}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint3mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{	
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
	g_warning(modName);
	sprintf(settings->Interlinear3Module,"%s",modName);
	changeVerseSWORD(current_verse);
}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint4mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
	g_warning(modName);
	sprintf(settings->Interlinear4Module,"%s",modName);
	changeVerseSWORD(current_verse);
}		

/*******************************************************************************
 *
 *******************************************************************************/	
void on_changeint5mod_activate(GtkMenuItem * menuitem,
		gpointer user_data)
{
	gchar modName[16];
	
	memset(modName,0,16); 
	modNameFromDesc(modName, (gchar *) user_data);
	g_warning(modName);
	sprintf(settings->Interlinear5Module,"%s",modName);
	changeVerseSWORD(current_verse);
}



void
on_viewtext_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	static GtkWidget *dlg;
	
	if(!isrunningVT) {
		dlg = create_dlgViewText ();
	}
	gtk_widget_show(dlg);
}

void
on_show_morphs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	morphsSWORD( INTERLINEAR_WINDOW, GTK_CHECK_MENU_ITEM(menuitem)->active);	
}
void
on_show_strongs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	strongsSWORD( INTERLINEAR_WINDOW, GTK_CHECK_MENU_ITEM(menuitem)->active);	
}
void
on_show_footnotes_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	footnotesSWORD( INTERLINEAR_WINDOW, GTK_CHECK_MENU_ITEM(menuitem)->active);	
}

/***  ***/
void on_dict_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings->app, "notebook4");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page
}

/***  ***/
void on_com_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings->app, "notebook1");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page
}

