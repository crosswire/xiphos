/*
 * GnomeSword Bible Study Tool
 * gs_popup_cb.c - SHORT DESCRIPTION
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <libgnomeprint/gnome-printer.h>
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-printer-dialog.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>
#include <libgnomeprint/gnome-print-preview.h>

#include "gui/interlinear_dialog.h"
#include "gui/cipher_key_dialog.h"
#include "gui/_editor.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"

#include "main/gs_popup_cb.h"
#include "main/gs_html.h"
#include "main/gs_gnomesword.h"
#include "main/gs_bookmarks.h"
#include "main/gs_interlinear.h"
#include "main/support.h"
#include "main/settings.h"

#include "backend/sword.h"

/* is commentary window set to scroll with text window */
gboolean autoscroll;

/******************************************************************************
 * externals
 *****************************************************************************/

extern GtkWidget * NEtext, *htmlCommentaries;

extern gboolean isrunningSD,	/* is the view dictionary dialog runing */
 isrunningVC,			/* is the view commentary dialog runing */
 isrunningVT,			/* is the view text dialog runing */
 noteModified;			/* personal comments window changed */

extern GList * options;

/* undock/dock interlinear page */
void on_undockInt_activate(GtkMenuItem *menuitem)
{
	if (settings.dockedInt) {
		settings.dockedInt = FALSE;
		gui_undock_interlinear_page();

	} else {
		settings.dockedInt = TRUE;
		gui_btnDockInt_clicked(NULL, NULL);
	}
}

void on_print_item_activate(GtkMenuItem *menuitem, gchar *html)
{
	html_print(lookup_widget(settings.app, html));
}

void on_about_this_module5_activate(GtkMenuItem * menuitem, gpointer user_data)
{

	//display_about_module_dialog(getmodnameSWORD(2),FALSE);
}


void on_john_3_1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	change_verse((gchar *)user_data);
}

void on_copy4_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(settings.app, "textComments");
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

void on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	GtkWidget *text;

	text = lookup_widget(settings.app, "textComments");
	gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,
			gs_clipboard->str, -1); */
}

void on_kjv1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	display_about_module_dialog((char *) user_data, FALSE);
}

void on_about_this_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!strcmp((gchar *) user_data, "Bible"))
		settings.whichwindow = MAIN_TEXT_WINDOW;
	else if (!strcmp((gchar *) user_data, "commentary"))
		settings.whichwindow = COMMENTARY_WINDOW;
	else if (!strcmp((gchar *) user_data, "dictionary"))
		settings.whichwindow = DICTIONARY_WINDOW;

	display_about_module_dialog(get_module_name(), FALSE);
}

void on_auto_scroll1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *toolbar;

	toolbar = lookup_widget(settings.app, "handlebox17");

	autoscroll = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (autoscroll)
		gtk_widget_hide(toolbar);
	else
		gtk_widget_show(toolbar);
}

/*******************************************************************************
 * show hide notebook tabs - popup menu choice
 *******************************************************************************/
void on_show_tabs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		gtk_widget_show(lookup_widget
				(settings.app, (gchar *) user_data));
	else
		gtk_widget_hide(lookup_widget
				(settings.app, (gchar *) user_data));
	if (!strcmp((gchar *) user_data, "notebook4"))
		settings.dict_tabs =
		    GTK_CHECK_MENU_ITEM(menuitem)->active;
}


/*******************************************************************************
 *
 *******************************************************************************/
void on_changeint1mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gchar modName[16];

	memset(modName, 0, 16);
	backend_module_name_from_description(modName,
					     (gchar *) user_data);
//      g_warning(modName);
	sprintf(settings.Interlinear1Module, "%s", modName);
	//change_verse(current_verse);
	if (settings.dockedInt)
		update_interlinear_page();
	else
		update_interlinear_page_detached();
}


/*******************************************************************************
 *
 *******************************************************************************/
void on_changeint2mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gchar modName[16];

	memset(modName, 0, 16);
	backend_module_name_from_description(modName,
					     (gchar *) user_data);
//      g_warning(modName);
	sprintf(settings.Interlinear2Module, "%s", modName);
	//change_verse(current_verse);
	if (settings.dockedInt)
		update_interlinear_page();
	else
		update_interlinear_page_detached();
}

/*******************************************************************************
 *
 *******************************************************************************/
void on_changeint3mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gchar modName[16];

	memset(modName, 0, 16);
	backend_module_name_from_description(modName,
					     (gchar *) user_data);
//      g_warning(modName);
	sprintf(settings.Interlinear3Module, "%s", modName);
	//change_verse(current_verse);
	if (settings.dockedInt)
		update_interlinear_page();
	else
		update_interlinear_page_detached();
}

/*******************************************************************************
 *
 *******************************************************************************/
void on_changeint4mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gchar modName[16];

	memset(modName, 0, 16);
	backend_module_name_from_description(modName,
					     (gchar *) user_data);
//      g_warning(modName);
	sprintf(settings.Interlinear4Module, "%s", modName);
	//change_verse(settings.currentverse);
	if (settings.dockedInt)
		update_interlinear_page();
	else
		update_interlinear_page_detached();
}

/*******************************************************************************
 *
 *******************************************************************************/
void on_changeint5mod_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gchar modName[16];

	memset(modName, 0, 16);
	backend_module_name_from_description(modName,
					     (gchar *) user_data);
//      g_warning(modName);
	sprintf(settings.Interlinear5Module, "%s", modName);
	//change_verse(settings.currentverse);
	if (settings.dockedInt)
		update_interlinear_page();
	else
		update_interlinear_page_detached();
}


/*** toogle global options in interlinear window ***/
void on_int_global_options_activate(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	set_interlinear_module_global_options((gchar *) user_data,
					      GTK_CHECK_MENU_ITEM
					      (menuitem)->active);
}

/***  ***/
void on_dict_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings.app, "notebook4");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page
}

/***  ***/
void on_com_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings.app, "notebook1");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page
*/
}
