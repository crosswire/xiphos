/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_mainmenu_cb.c
			-------------------
Tue Aug 21 2001
copyright (C) 2001 by Terry Biggs
tbiggs@users.sourceforge.net
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
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>

#include "gs_mainmenu_cb.h"
#include "gs_gnomesword.h"
#include "sw_gnomesword.h"
#include "gs_viewdict_dlg.h"
#include "gs_history.h"
#include "support.h"
#include "gs_file.h"
#include "gs_listeditor.h"
#include "gs_abouts.h"
#include "printstuff.h"

/******************************************************************************
 * externals
******************************************************************************/
extern SETTINGS 
	*settings;	/* pointer to settings structure - (declared in gs_gnomesword.c) */
extern gboolean 
	firstsearch,	/* search dialog is not running when TRUE */
	firstLE;	  /* ListEditor in not running when TRUE */
extern GtkWidget 
	*listeditor;		/* pointer to ListEditor */
	
/**********************************************************************************
 * main menu call backs
 * popup menu call backs are located in gs_popup_cb.c
 **********************************************************************************/
/******************************************************************************
 *on_help_contents_activate - user chose an history item
******************************************************************************/
void
on_help_contents_activate(GtkMenuItem * menuitem, gpointer user_data)
{       
	pid_t pid;	
	
	if ((pid = fork ()) == 0)
	{
		execlp ("gnome-help-browser", "gnome-help-browser",
			"ghelp:gnomesword");
		g_error (_("Cannot launch gnome-help-browser"));
	}	
}

/******************************************************************************
 *on_mnuHistoryitem1_activate - user chose an history item
******************************************************************************/
void
on_mnuHistoryitem1_activate(GtkMenuItem * menuitem, gpointer user_data)
{       	
	changeverseHistory(atoi((gchar *)user_data));
}

//----------------------------------------------------------------------------------------------
void
on_strongs_numbers1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	/* we change the state of strongs toogle buttn to match the menu item this will 
	    activate on_btnStrongs_toogled  which will do the work */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (lookup_widget
				      (settings->app, "btnStrongs")),
				     GTK_CHECK_MENU_ITEM(menuitem)->active);	/* set strongs toogle button */
}

//----------------------------------------------------------------------------------------------
void
on_morphs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	morphsSWORD(0, GTK_CHECK_MENU_ITEM(menuitem)->active);	
}

//----------------------------------------------------------------------------------------------
void
on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{
	GtkWidget *dlg, *text1, *text2, *label, *version_label;
	
	dlg = create_AboutSword();
	text1 = lookup_widget(dlg, "txtAboutSword");
	text2 = lookup_widget(dlg, "text6");
	label = lookup_widget(dlg, "label96");
	version_label = lookup_widget(dlg, "version_label");
	gtk_text_set_word_wrap(GTK_TEXT(text1), TRUE);
	gtk_text_set_word_wrap(GTK_TEXT(text2), TRUE);
	showinfoSWORD(text2, GTK_LABEL(label), GTK_LABEL(version_label));
	gtk_widget_show(dlg);
}


//----------------------------------------------------------------------------------------------
void on_footnotes1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	footnotesSWORD(0, GTK_CHECK_MENU_ITEM(menuitem)->active);	
}


//----------------------------------------------------------------------------------------------
void on_copy3_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *text;
	text = lookup_widget(settings->app, (gchar *) user_data);	
	gtk_editable_copy_clipboard(GTK_EDITABLE(GTK_TEXT(text)));
}

//----------------------------------------------------------------------------------------------
void on_preferences1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	loadpreferencemodsSWORD();
}


//----------------------------------------------------------------------------------------------
void on_add_quickmark_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	addQuickmark(settings->app);
}

//----------------------------------------------------------------------------------------------
void
on_edit_quickmarks_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (firstLE) {
		listeditor = createListEditor();
		editbookmarks(listeditor);
		firstLE = FALSE;
	}
	gtk_widget_show(listeditor);
}
//----------------------------------------------------------------------------------------------
void on_search1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	SETTINGS *s;
	EShortcutBar *bar1;
		
	s = settings;
	bar1 = E_SHORTCUT_BAR(s->shortcut_bar);
	e_paned_set_position (E_PANED(lookup_widget(s->app,"epaned")), s->shortcutbar_width);
	s->showshortcutbar = TRUE;		
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar1),
						 s->searchbargroup,
						 TRUE);
}
//----------------------------------------------------------------------------------------------
void
on_auto_save_notes1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	setautosave(GTK_CHECK_MENU_ITEM(menuitem)->active);
}


//----------------------------------------------------------------------------------------------
void on_verse_style1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		setversestyleSWORD(TRUE);
	else
		setversestyleSWORD(FALSE);
}

//----------------------------------------------------------------------------------------------
void
on_show_interlinear_page1_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{

	if (GTK_CHECK_MENU_ITEM(menuitem)->active)
		showIntPage(settings->app, TRUE);
	else
		showIntPage(settings->app, FALSE);
}

//----------------------------------------------------------------------------------------------
void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	shutdownSWORD();
}

//----------------------------------------------------------------------------------------------
void on_clear1_activate(GtkMenuItem * menuitem,	//-- clear history menu
			gpointer user_data)
{
	clearhistory(settings->app,GTK_WIDGET(settings->shortcut_bar));
}

//----------------------------------------------------------------------------------------------
void
on_about_gnomesword1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *AboutBox;

	AboutBox = create_about2();
	gtk_widget_show(AboutBox);
}

//----------------------------------------------------------------------------------------------
void on_com_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings->app, "notebook1");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page
}

//----------------------------------------------------------------------------------------------
void on_dict_select_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *notebook;
	gint modNum;

	modNum = GPOINTER_TO_INT(user_data);
	notebook = lookup_widget(settings->app, "notebook4");	//-- get notebook
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), modNum);	//-- set notebook page
}





