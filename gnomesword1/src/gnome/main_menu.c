/*
 * GnomeSword Bible Study Tool
 * main_menu.c - creation of and call backs for gnomesword main menu
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
#  include <config.h>
#endif

#include <gnome.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/e-paned/e-vpaned.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>

#include "gs_gnomesword.h"
#include "support.h"
#include "gs_html.h"
#include "gs_shortcutbar.h"
#include "bibletext.h"
#include "gs_menu.h"
#include "gs_file.h"
#include "about_gnomesword.h"
#include "about_sword.h"
#include "gs_preferences_dlg.h"
#include "gs_history.h"

/*
 * gnome
 */
#include "main_menu.h"

/******************************************************************************
 * static - global to this file only
 */

				
/******************************************************************************
 *on_help_contents_activate - user chose help item
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


/*** display - The SWORD Project - about information ***/
void
on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{
	GtkWidget *dlg, *version_label;
	const char *ver;
	gchar version[40];
	
	dlg = gui_create_about_sword();
	version_label = lookup_widget(dlg, "version_label");
	/* get sword version */
	ver = backend_get_sword_verion();	
	sprintf(version,"Sword-%s",ver);
	gtk_label_set_text(GTK_LABEL(version_label),version);
	gtk_widget_show(dlg);
}


/*** display daily devotion in shortcut bar viewer ***/
void
on_daily_devotion1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{	
	backend_display_devotional(settings);
}


/*** open preferences dialog ***/
void on_preferences1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	setup_preferences_dlg(settings);
}


/*** display search group in shortcut bar ***/
void on_search1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	SETTINGS *s;
	EShortcutBar *bar1;
		
	s = settings;
	bar1 = E_SHORTCUT_BAR(s->shortcut_bar);
	if(!s->showshortcutbar){
		on_btnSB_clicked(NULL,s);
	}
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar1),
						 s->searchbargroup,
						 TRUE);
}


/*** toogle between verse and paragraph style ***/
void on_verse_style1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	set_verse_style(GTK_CHECK_MENU_ITEM(menuitem)->active); 
}


/*** do a nice orderly shut down and exit gnomesword ***/
void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_widget_destroy(settings->app); 
	/* this will send us to on_mainwindow_destroy() */
}

/*** remove all items from history list ***/
void on_clear1_activate(GtkMenuItem * menuitem,	//-- clear history menu
			gpointer user_data)
{
	clearhistory(settings->app,GTK_WIDGET(settings->shortcut_bar));
}


/*** display gnomesword about dialog ***/
void
on_about_gnomesword1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *AboutBox;

	AboutBox = gui_create_about_gnomesword();
	gtk_widget_show(AboutBox);
}



static GnomeUIInfo file1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM(on_exit1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo edit1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Copy"),
	 N_("Copy highlighted text from main window"),
	 on_copyhtml_activate, "htmlTexts", NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Search"),
	 N_("Open search dialog"),
	 on_search1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo history1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("C_lear"),
	 N_("Clear history list"),
	 on_clear1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo view1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Daily Devotion"),
	 N_("Show the Daily Devotion for Today"),
	 (gpointer) on_daily_devotion1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo settings1_menu_uiinfo[] = {
	
	GNOMEUIINFO_MENU_PREFERENCES_ITEM(on_preferences1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo bible_texts1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo commentaries1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo dictionaries_lexicons1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo books_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo about_sword_modules1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Bible Texts"),
	 N_("Information about Bible texts"),
	 bible_texts1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Commentaries"),
	 NULL,
	 commentaries1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Dictionaries-Lexicons"),
	 NULL,
	 dictionaries_lexicons1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Books"),
	 NULL,
	 books_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo help1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Contents"),
	 N_("Help Contents"),
	 on_help_contents_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_HELP,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("About the Sword Project..."),
	 N_("Infotmation About The Sword Project"),
	 on_about_the_sword_project1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("About Sword Modules"),
	 N_("Information about the installed modules"),
	 about_sword_modules1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("About GnomeSword..."),
	 N_("About GnomeSword"),
	 on_about_gnomesword1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] = {
	GNOMEUIINFO_MENU_FILE_TREE(file1_menu_uiinfo),
	GNOMEUIINFO_MENU_EDIT_TREE(edit1_menu_uiinfo),
	/*{
	 GNOME_APP_UI_SUBTREE, N_("_Quickmarks"),
	 NULL,
	 quickmarks_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},*/
	{
	 GNOME_APP_UI_SUBTREE, N_("_History"),
	 NULL,
	 history1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_MENU_VIEW_TREE(view1_menu_uiinfo),
	GNOMEUIINFO_MENU_SETTINGS_TREE(settings1_menu_uiinfo),
	GNOMEUIINFO_MENU_HELP_TREE(help1_menu_uiinfo),
	GNOMEUIINFO_END
};






void gui_create_main_menu(GtkWidget *app)
{
	gnome_app_create_menus(GNOME_APP(app), menubar1_uiinfo);

	gtk_widget_ref(menubar1_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "file1",
				 menubar1_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(file1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "separator4",
				 file1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(file1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "exit1",
				 file1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "edit1",
				 menubar1_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "copy1",
				 edit1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "separator3",
				 edit1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(edit1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "search1",
				 edit1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "quickmarks",
				 menubar1_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
				 
	gtk_widget_ref(menubar1_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "history1",
				 menubar1_uiinfo[3].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(history1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "clear1",
				 history1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(history1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "separator5",
				 history1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "view1",
				 menubar1_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(view1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "daily_devotion1",
				 view1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "settings1",
				 menubar1_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(settings1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "preferences1",
				 settings1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(menubar1_uiinfo[5].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "help1",
				 menubar1_uiinfo[5].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_right_justify(GTK_MENU_ITEM
				    (menubar1_uiinfo[5].widget));

	gtk_widget_ref(help1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "about_the_sword_project1",
				 help1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "about_gnomesword1",
				 help1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(help1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "about_sword_modules1",
				 help1_menu_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "bible_texts1",
				 about_sword_modules1_menu_uiinfo
				 [0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(bible_texts1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "separator15",
				 bible_texts1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "commentaries1",
				 about_sword_modules1_menu_uiinfo[1].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(commentaries1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "separator16",
				 commentaries1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "dictionaries_lexicons1",
				 about_sword_modules1_menu_uiinfo
				 [2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(dictionaries_lexicons1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "separator17",
				 dictionaries_lexicons1_menu_uiinfo
				 [0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(about_sword_modules1_menu_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(app),
				 "books",
				 about_sword_modules1_menu_uiinfo[3].
				 widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(books_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "separator17",
				 books_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);	
}

void gui_install_menu_hints(GtkWidget *app)
{
	gnome_app_install_menu_hints(GNOME_APP(app),
				     menubar1_uiinfo);
}
/******   end of file   ******/
