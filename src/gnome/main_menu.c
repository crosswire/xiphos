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
#include <config.h>
#endif

#include <gnome.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/e-paned/e-vpaned.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>

#include "gui/main_window.h"
#include "gui/main_menu.h"
#include "gui/preferences_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/history.h"
#include "gui/html.h"
#include "gui/utilities.h"

#include "main/gs_gnomesword.h"
#include "main/bibletext.h"
#include "gui/about_gnomesword.h"
#include "gui/about_sword.h"
#include "main/lists.h"

GtkWidget *htmlTexts;

/******************************************************************************
 * static - global to this file only
 */

/******************************************************************************
 * Name
 *  on_help_contents_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_help_contents_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   display the help contents file
 *
 * Return value
 *   void
 */
 
void on_help_contents_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
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
 * Name
 *  on_mnuHistoryitem1_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_mnuHistoryitem1_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   change text module to chosen histor item
 *
 * Return value
 *   void
 */

void on_mnuHistoryitem1_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{       	
	changeverseHistory(atoi((gchar *)user_data));
}

/******************************************************************************
 * Name
 *  on_about_the_sword_project1_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   display - The SWORD Project - about information
 *
 * Return value
 *   void
 */

void on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
						gpointer user_data)
{
	GtkWidget *dlg, *version_label;
	const char *ver;
	gchar version[40];
	
	dlg = gui_create_about_sword();
	version_label = gui_lookup_widget(dlg, "version_label");
	/* 
	 * get sword version 
	 */
	ver = get_sword_version();
	sprintf(version,"Sword-%s",ver);
	gtk_label_set_text(GTK_LABEL(version_label),version);
	gtk_widget_show(dlg);
}

/******************************************************************************
 * Name
 *  on_daily_devotion1_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_daily_devotion1_activate(GtkMenuItem *menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   display daily devotion in shortcut bar viewer
 *
 * Return value
 *   void
 */

void on_daily_devotion1_activate(GtkMenuItem *menuitem, 
						gpointer user_data)
{	
	display_devotional();
}

/******************************************************************************
 * Name
 *  on_preferences1_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_preferences1_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   open preferences dialog
 *
 * Return value
 *   void
 */
 
void on_preferences1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	gui_setup_preferences_dialog();
}

/******************************************************************************
 * Name
 *  on_search_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_search_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   display search group in shortcut bar
 *
 * Return value
 *   void
 */
 
void on_search_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	EShortcutBar *bar1;
		
	bar1 = E_SHORTCUT_BAR(settings.shortcut_bar);
	if(!settings.showshortcutbar){
		gui_shortcutbar_showhide();
	}
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar1),
						 settings.searchbargroup,
						 TRUE);
}

/******************************************************************************
 * Name
 *  on_verse_style1_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_verse_style1_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toogle between verse and paragraph style
 *
 * Return value
 *   void
 */

void on_verse_style1_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	set_verse_style(GTK_CHECK_MENU_ITEM(menuitem)->active); 
}

/******************************************************************************
 * Name
 *  on_exit1_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   do a nice orderly shut down and exit gnomesword
 *   by calling gtk_widget_destroy() which will call
 *   on_mainwindow_destroy()
 *
 * Return value
 *   void
 */

void on_exit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_widget_destroy(settings.app); 
}

/******************************************************************************
 * Name
 *  on_clear1_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_clear1_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   remove all items from history list by calling
 *   clearhistory();
 *
 * Return value
 *   void
 */
 
void on_clear1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	clearhistory(settings.app, GTK_WIDGET(settings.shortcut_bar));
}

/******************************************************************************
 * Name
 *  on_about_gnomesword1_activate
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void on_about_gnomesword1_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   display gnomesword about dialog 
 *
 * Return value
 *   void
 */

void on_about_gnomesword1_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{
	GtkWidget *AboutBox;

	AboutBox = gui_create_about_gnomesword();
	gtk_widget_show(AboutBox);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   v	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_bibletext_item_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{
	gui_open_bibletext_dialog((gchar*)user_data);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   v	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_commentary_item_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{
	gui_open_commentary_dialog((gchar *)user_data);
}
/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   v	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_dictlex_item_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{
	gui_open_dictlex_dialog((gchar *)user_data);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   v	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_book_item_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{
	
}

/******************************************************************************
 * gnome menu structures
 */

static GnomeUIInfo file1_menu_uiinfo[] = {
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM(on_exit1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo edit1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Copy"),
	 N_("Copy highlighted text from main window"),
	 gui_copyhtml_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Search"),
	 N_("Open search dialog"),
	 on_search_activate, NULL, NULL,
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



static GnomeUIInfo new_bibletext_dialog1_menu_uiinfo[] =
{
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_END
};

static GnomeUIInfo new_commentary_dialog1_menu_uiinfo[] =
{
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_END
};

static GnomeUIInfo new_dict_lex_dialog1_menu_uiinfo[] =
{
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_END
};

static GnomeUIInfo new_book_dialog1_menu_uiinfo[] =
{
  GNOMEUIINFO_SEPARATOR,
  GNOMEUIINFO_END
};

static GnomeUIInfo view1_menu_uiinfo[] =
{
  {
    GNOME_APP_UI_ITEM, N_("Daily Devotion"),
    N_("Show the Daily Devotion for Today"),
    (gpointer) on_daily_devotion1_activate, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_OPEN,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_SEPARATOR,
  {
    GNOME_APP_UI_SUBTREE, N_("New Bibletext Dialog"),
    NULL,
    new_bibletext_dialog1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("New Commentary Dialog"),
    NULL,
    new_commentary_dialog1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_BLUE,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("New Dictionary Dialog"),
    NULL,
    new_dict_lex_dialog1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_GREEN,
    0, (GdkModifierType) 0, NULL
  },
  {
    GNOME_APP_UI_SUBTREE, N_("New Book Dialog"),
    NULL,
    new_book_dialog1_menu_uiinfo, NULL, NULL,
    GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_YELLOW,
    0, (GdkModifierType) 0, NULL
  },
  GNOMEUIINFO_END
};




static GnomeUIInfo settings1_menu_uiinfo[] = {
	{
	    GNOME_APP_UI_TOGGLEITEM, N_("Verse Style"),
	    NULL,
	    (gpointer) on_verse_style1_activate, NULL, NULL,
	    GNOME_APP_PIXMAP_NONE, NULL,
	    0, (GdkModifierType) 0, NULL
	},
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
	

/******************************************************************************
 * Name
 *   gui_create_main_menu
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void gui_create_main_menu(GtkWidget *app)	
 *
 * Description
 *   create the main menu for gnomesword
 *
 * Return value
 *   void
 */

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
	
	
	gtk_widget_ref (view1_menu_uiinfo[1].widget);
	gtk_object_set_data_full (GTK_OBJECT (app), "separator28",
			    view1_menu_uiinfo[1].widget,
			    (GtkDestroyNotify) gtk_widget_unref);
	
	gtk_widget_ref (view1_menu_uiinfo[2].widget);
	gtk_object_set_data_full (GTK_OBJECT (app), "new_bibletext_dialog1",
			    view1_menu_uiinfo[2].widget,
			    (GtkDestroyNotify) gtk_widget_unref);
			    
	
	
	gui_add_mods_to_menus(get_list(TEXT_LIST), _("_View/New Bibletext Dialog/"),
			  (GtkMenuCallback)on_bibletext_item_activate);
			  
			  
	
	gtk_widget_ref (view1_menu_uiinfo[3].widget);
	gtk_object_set_data_full (GTK_OBJECT (app), "new_commentary_dialog1",
			    view1_menu_uiinfo[3].widget,
			    (GtkDestroyNotify) gtk_widget_unref);
	
	
	gui_add_mods_to_menus(get_list(COMM_LIST), _("_View/New Commentary Dialog/"),
			  (GtkMenuCallback)on_commentary_item_activate);
	
	
	gtk_widget_ref (view1_menu_uiinfo[4].widget);
	gtk_object_set_data_full (GTK_OBJECT (app), "new_dict_lex_dialog1",
			    view1_menu_uiinfo[4].widget,
			    (GtkDestroyNotify) gtk_widget_unref);
	
	gui_add_mods_to_menus(get_list(DICT_LIST), _("_View/New Dictionary Dialog/"),
			  (GtkMenuCallback)on_dictlex_item_activate);
	
	
	gtk_widget_ref (view1_menu_uiinfo[5].widget);
	gtk_object_set_data_full (GTK_OBJECT (app), "new_book_dialog1",
			    view1_menu_uiinfo[5].widget,
			    (GtkDestroyNotify) gtk_widget_unref);

	gui_add_mods_to_menus(get_list(GBS_LIST), 
				_("_View/New Book Dialog/"),
				(GtkMenuCallback)on_book_item_activate);

	gtk_widget_ref(menubar1_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "settings1",
				 menubar1_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref (settings1_menu_uiinfo[0].widget);
	gtk_object_set_data_full (GTK_OBJECT(app), "verse_style",
                            settings1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);
	settings.versestyle_item = settings1_menu_uiinfo[0].widget;

	gtk_widget_ref(settings1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(app), "preferences1",
				 settings1_menu_uiinfo[1].widget,
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

/******************************************************************************
 * Name
 *  gui_install_menu_hints
 *
 * Synopsis
 *   #include "main_menu.h"
 *
 *   void gui_install_menu_hints(GtkWidget *app)	
 *
 * Description
 *   install hints - display menu hints in status bar
 *
 * Return value
 *   void
 */

void gui_install_menu_hints(GtkWidget *app)
{
	gnome_app_install_menu_hints(GNOME_APP(app),
				     menubar1_uiinfo);
}
