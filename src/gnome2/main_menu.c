/*
 * GnomeSword Bible Study Tool
 * main_menu.c - creation of and call backs for gnomesword main menu
 *
 * Copyright (C) 2000-2008 GnomeSword Developer Team
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

#ifdef USE_GTKHTML38
#include "editor/html-editor.h"
#endif

#include "gui/about_gnomesword.h"
#include "gui/about_sword.h"
#include "gui/about_trans.h"
#include "gui/gnomesword.h"
#include "gui/main_window.h"
#include "gui/main_menu.h"
#include "gui/preferences_dialog.h"
#include "gui/sidebar.h"
#include "gui/sidebar_dialog.h"
#include "gui/search_dialog.h"
#include "gui/studypad.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/lists.h"
#include "main/sword.h"
#include "main/search_dialog.h"
#include "main/tab_history.h"



/******************************************************************************
 * Name
 *  gui_about_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void gui_about_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   display module about information
 *
 * Return value
 *   void
 */

void
gui_about_activate(GtkMenuItem * menuitem,
		   gpointer user_data)
{
	gui_display_about_module_dialog((char *) user_data, FALSE);
}


/******************************************************************************
 * Name
 *  on_help_contents_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
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

void
on_help_contents_activate(GtkMenuItem * menuitem,
			  gpointer user_data)
{	
#ifdef __CYGWIN__
	gui_generic_warning("Cygwin does not have the\n"
			    "documentation viewer needed to\n"
			    "view the GnomeSword manual.\n"
			    "Install and reference the\n"
			    "GnomeSword manual module instead.");
#else
	GError *error = NULL;
# ifdef WIN32
	gchar *help_file = g_win32_get_package_installation_directory_of_module(NULL);
	help_file = g_strconcat(help_file, "\0", NULL);
	help_file = g_build_filename(help_file, "share", "help", "C", "gnomesword.chm", NULL);
	if (gnome_url_show(g_strconcat("file:///", help_file, NULL),
			   &error) == FALSE) {
		GS_warning((error->message));
		g_error_free(error);
	}
	g_free(help_file);
# else
	
	if (gnome_help_display((const gchar*)"gnomesword.xml", 
			       NULL, &error) == FALSE) {
		GS_warning((error->message));
		g_error_free(error);        
	}
# endif /* WIN32 */
#endif /* __CYGWIN__ */
}

/******************************************************************************
 * Name
 *  on_mailing_list_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_mailing_list_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   open web browser to the mailing list signup page
 *
 * Return value
 *   void
 */

void
on_mailing_list_activate(GtkMenuItem * menuitem,
			gpointer user_data)
{
	GError *error = NULL;
	if (gnome_url_show("https://lists.sourceforge.net/lists/listinfo/gnomesword-users/",
				&error) == FALSE) {
		GS_warning((error->message));
		g_error_free (error);
	}
}


/******************************************************************************
 * Name
 *  on_live_chat_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_live_chat_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   open web browser to mibbit irc chat
 *
 * Return value
 *   void
 */

void
on_live_chat_activate(GtkMenuItem * menuitem,
			gpointer user_data)
{
	GError *error = NULL;
	if (gnome_url_show("http://embed.mibbit.com/?server=irc.freenode.net&channel=%23gnomesword",
				&error) == FALSE) {
		GS_warning((error->message));
		g_error_free (error);
	}
}

/******************************************************************************
 * Name
 *  on_report_bug_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_report_bug_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   open web browser to sourceforge bug tracker
 *
 * Return value
 *   void
 */

void
on_report_bug_activate(GtkMenuItem * menuitem,
			gpointer user_data)
{
	GError *error = NULL;
	if (gnome_url_show(PACKAGE_BUGREPORT,
				&error) == FALSE) {
		GS_warning((error->message));
		g_error_free (error);
	}
}

/******************************************************************************
 * Name
 *  on_menuHistoryitem1_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_menuHistoryitem1_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   change text module to chosen histor item
 *
 * Return value
 *   void
 */

void
on_menuHistoryitem1_activate(GtkMenuItem * menuitem,
			     gpointer user_data)
{
	main_change_verse_tab_history(GPOINTER_TO_INT(user_data));
}

/******************************************************************************
 * Name
 *  on_about_the_sword_project1_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
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

void
on_about_the_sword_project1_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gui_create_about_sword();
	gtk_widget_show(dlg);
}

/******************************************************************************
 * Name
 *  on_about_translation_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_about_translation_activate(GtkMenuItem * menuitem,
 *					gpointer user_data)	
 *
 * Description
 *   display - The SWORD Project - about information
 *
 * Return value
 *   void
 */

void
on_about_translation_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gui_create_about_trans();
	gtk_widget_show(dlg);
}

/******************************************************************************
 * Name
 *  on_daily_devotion1_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
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

void
on_daily_devotion1_activate(GtkMenuItem * menuitem,
			    gpointer user_data)
{
	main_display_devotional();
}

/******************************************************************************
 * Name
 *  on_preferences1_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_preferences1_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   open preferences dialog
 *
 * Return value
 *   void
 */

void
on_preferences1_activate(GtkMenuItem * menuitem,
			 gpointer user_data)
{
	gui_setup_preferences_dialog();
}

/******************************************************************************
 * Name
 *  on_search_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_search_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   display search group in shortcut bar
 *
 * Return value
 *   void
 */

void
on_search_activate(GtkMenuItem * menuitem,
		   gpointer user_data)
{
	if (!settings.showshortcutbar) 
		gui_sidebar_showhide();
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),2);
}

/******************************************************************************
 * Name
 *  on_verse_style1_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_verse_style1_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle between verse and paragraph style
 *
 * Return value
 *   void
 */

static void
on_verse_style1_activate(GtkCheckMenuItem * menuitem,
			 gpointer user_data)
{
	extern gboolean style_display;
	if(style_display) {
		gchar *file = g_strdup_printf("%s/modops.conf", 
					      settings.gSwordDir);
		gchar *url = g_strdup_printf("sword://%s/%s",
					     settings.MainWindowModule,
					     settings.currentverse);
		/* remember our choice for the next program startup */
		settings.versestyle = menuitem->active;
		save_conf_file_item(file, settings.MainWindowModule, "style",
				    (menuitem->active
				     ? "verse"
				     : "paragraph"));
		if (settings.havebible) {
			main_url_handler(url, TRUE);		
		}
		g_free(url);
		g_free(file);
	}
}

/******************************************************************************
 * Name
 *  on_linked_tabs_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_linked_tabs_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle linking tabs together.
 *
 * Return value
 *   void
 */

static void
on_linked_tabs_activate(GtkCheckMenuItem * menuitem,
			gpointer user_data)
{
	settings.linkedtabs = menuitem->active;
	xml_set_value("GnomeSword", "misc", "pinnedtabs",
		      (settings.linkedtabs ? "1" : "0"));
}

/******************************************************************************
 * Name
 *  on_read_aloud_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_read_aloud_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle reading scripture out loud.
 *
 * Return value
 *   void
 */

static void
on_read_aloud_activate(GtkCheckMenuItem * menuitem,
		       gpointer user_data)
{
	settings.readaloud = menuitem->active;
	xml_set_value("GnomeSword", "misc", "readaloud",
		      (settings.readaloud ? "1" : "0"));
	if (settings.readaloud)
		main_display_bible(NULL, settings.currentverse);
}

/******************************************************************************
 * Name
 *  on_show_verse_numbers_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_show_verse_numbers_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle showing verse numbers together.
 *
 * Return value
 *   void
 */

static void
on_show_verse_numbers_activate(GtkCheckMenuItem * menuitem,
			       gpointer user_data)
{
	settings.showversenum = menuitem->active;
	xml_set_value("GnomeSword", "misc", "showversenum",
		      (settings.showversenum ? "1" : "0"));
	main_display_bible(NULL, settings.currentverse);
}

/******************************************************************************
 * Name
 *  on_versehighlight_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_versehighlight_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle special current verse highlight.
 *
 * Return value
 *   void
 */

static void
on_versehighlight_activate(GtkCheckMenuItem * menuitem,
			   gpointer user_data)
{
	settings.versehighlight = menuitem->active;
	xml_set_value("GnomeSword", "misc", "versehighlight",
		      (settings.versehighlight ? "1" : "0"));
	main_display_bible(NULL, settings.currentverse);
}

#ifdef USE_PARALLEL_TAB	
/******************************************************************************
 * Name
 *  on_parallel_tab_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_parallel_tab_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle special current verse highlight.
 *
 * Return value
 *   void
 */

static void
on_parallel_tab_activate(GtkCheckMenuItem * menuitem,
			   gpointer user_data)
{
	settings.showparatab = menuitem->active;
	xml_set_value("GnomeSword", "misc", "showparatab",
		      (settings.versehighlight ? "1" : "0"));
	if(settings.showparatab)
		gui_open_parallel_view_in_new_tab();
	else
		gui_close_passage_tab(1);
}

/******************************************************************************
 * Name
 *  on_side_preview_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_side_preview_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle special previewer in sidebar.
 *
 * Return value
 *   void
 */

static void
on_side_preview_activate(GtkCheckMenuItem * menuitem,
			   gpointer user_data)
{
	settings.show_previewer_in_sidebar = menuitem->active;
	xml_set_value("GnomeSword", "misc", "show_side_preview",
		      (settings.show_previewer_in_sidebar ? "1" : "0"));
	gui_show_previewer_in_sidebar(settings.show_previewer_in_sidebar);
}
#endif /*  USE_PARALLEL_TAB  */

/******************************************************************************
 * Name
 *  on_doublespace_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_doublespace_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle double-spacing of text.
 *
 * Return value
 *   void
 */

static void
on_doublespace_activate(GtkCheckMenuItem * menuitem,
			gpointer user_data)
{
	settings.doublespace = menuitem->active;
	xml_set_value("GnomeSword", "misc", "doublespace",
		      (settings.doublespace ? "1" : "0"));
	//main_display_bible(NULL, settings.currentverse);
	main_refresh_all();
}

/******************************************************************************
 * Name
 *  on_exit_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_exit_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   do a nice orderly shut down and exit gnomesword
 *   by calling gtk_widget_destroy() which will call
 *   on_mainwindow_destroy()
 *
 * Return value
 *   void
 */

void
on_exit_activate(GtkMenuItem * menuitem,
		 gpointer user_data)
{
	shutdown_frontend();
	/* shutdown the sword stuff */
	main_shutdown_backend();
	gtk_main_quit();
	gtk_exit(0);
	gtk_widget_destroy(widgets.app);
}

/******************************************************************************
 * Name
 *  on_clear1_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_clear1_activate(GtkMenuItem * menuitem,
 *			     gpointer user_data)	
 *
 * Description
 *   remove all items from history list by calling
 *   gui_clear_history();
 *
 * Return value
 *   void
 */

void
on_clear1_activate(GtkMenuItem * menuitem,
		   gpointer user_data)
{
	main_clear_tab_history();
}

/******************************************************************************
 * Name
 *  on_about_gnomesword1_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_about_gnomesword1_activate(GtkMenuItem * menuitem, 
 *					gpointer user_data)	
 *
 * Description
 *   display gnomesword about dialog 
 *
 * Return value
 *   void
 */

void
on_about_gnomesword1_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
	GtkWidget *AboutBox;

	AboutBox = gui_create_about_gnomesword();
	gtk_widget_show(AboutBox);
}


/******************************************************************************
 * Name
 *  on_save_tabs
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_save_tabs(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   ask for a file name (with file-chooser) and save the current tabs to that file
 *
 * Return value
 *   void
 */
void
on_save_tabs(GtkMenuItem * menuitem,
	     gpointer user_data)
{
	GtkWidget *dialog;
	gchar *tabs_dir;

	tabs_dir = g_strdup_printf("%s/tabs/",settings.gSwordDir);
	
	if (access(tabs_dir, F_OK) == -1) {
		if ((Mkdir(tabs_dir, S_IRWXU)) == -1) {
			fprintf(stderr, "can't create tabs dir");
			return;
		}
	}
	
	dialog = gtk_file_chooser_dialog_new ("Save Session",
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_set_current_folder((GtkFileChooser*)dialog, tabs_dir);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
  	{
    		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		filename = g_path_get_basename(filename);
		gui_save_tabs(filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}


/******************************************************************************
 * Name
 *  on_load_tabs
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_load_tabs(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   ask for file name (with file-chooser) and load tabs from that file
 *
 * Return value
 *   void
 */
void
on_load_tabs(GtkMenuItem * menuitem,
	     gpointer user_data)
{
	GtkWidget *dialog;
	gchar *tabs_dir;

	tabs_dir = g_strdup_printf("%s/tabs/",settings.gSwordDir);
	
	if (access(tabs_dir, F_OK) == -1) {
		if ((Mkdir(tabs_dir, S_IRWXU)) == -1) {
			fprintf(stderr, "can't create tabs dir");
			return;
		}
	}

	dialog = gtk_file_chooser_dialog_new ("Open Session",
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_set_current_folder((GtkFileChooser*)dialog, tabs_dir);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
  	{
    		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		filename = g_path_get_basename(filename);
		gui_close_all_tabs();
		gui_load_tabs(filename);
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}


/******************************************************************************
 * Name
 *  view_hints
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void view_hints(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    toggle view hints in texts window
 *
 * Return value
 *   void
 */

static void
view_hints(GtkMenuItem * menuitem,
	   gpointer user_data)
{
/*	hint.use_hints = GTK_CHECK_MENU_ITEM(menuitem)->active;
	
	if(!hint.use_hints)
		gui_destroy_hint_window();*/
	/*
	if(hint.use_hints)
		gui_open_hint_viewer();*/
}


/******************************************************************************
 * Name
 *  view_bible_texts
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void view_bible_texts(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    toggle Bibletext window
 *
 * Return value
 *   void
 */

static void
view_bible_texts(GtkMenuItem * menuitem,
		 gpointer user_data)
{
	gui_show_hide_texts(GTK_CHECK_MENU_ITEM(menuitem)->active);
}

static void
view_previewer(GtkMenuItem * menuitem,
	       gpointer user_data)
{
	gui_show_hide_preview(GTK_CHECK_MENU_ITEM(menuitem)->active);
}


/******************************************************************************
 * Name
 *  view_upper_workbook
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void view_upper_workbook(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    toggle commentary window *** we need to change the name of this
 *    function because there no longer an upper_workbook
 *
 * Return value
 *   void
 */

static void
view_upper_workbook(GtkMenuItem * menuitem,
		    gpointer user_data)
{
	gui_show_hide_comms(GTK_CHECK_MENU_ITEM(menuitem)->active);
}


/******************************************************************************
 * Name
 *   view_lower_workbook
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void view_lower_workbook(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    toggle lower_workbook view (on or off)
 *
 * Return value
 *   void
 */

static void
view_lower_workbook(GtkMenuItem * menuitem,
		    gpointer user_data)
{
	gui_show_hide_dicts(GTK_CHECK_MENU_ITEM(menuitem)->active);
}


/******************************************************************************
 * Name
 *   on_mod_mgr
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_mod_mgr(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void
on_mod_mgr(GtkMenuItem * menuitem,
	   gpointer user_data)
{
	gui_open_mod_mgr();
}

/******************************************************************************
 * Name
 *   open_studypad
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void open_studypad(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    open studypad editor - if studypad dialog exist bring it to the
 *    top
 *
 * Return value
 *   void
 */

static void
open_studypad(GtkMenuItem * menuitem,
	      gpointer user_data)
{

#ifdef USE_GTKHTML38
	editor_create_new(settings.studypadfilename, NULL, FALSE);
#else
	if (settings.studypad_dialog_exist) {
		gtk_widget_show(widgets.studypad_dialog);
		gdk_window_raise(GTK_WIDGET(widgets.studypad_dialog)->
				 window);
	} else {
		settings.studypad_dialog_exist =
			  gui_open_studypad(settings.studypadfilename);
	}
#endif
}


/******************************************************************************
 * gnome menu structures
 */

static GnomeUIInfo file1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Open Study_Pad"),
	 N_("Open the StudyPad editor"),
	 open_studypad, NULL, NULL,
	 GNOME_APP_PIXMAP_FILENAME, NULL /* init'd in menu creation */,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{ GNOME_APP_UI_ITEM, N_("_Open Session"),
	  N_("Open a saved session"),
	  on_load_tabs, NULL, NULL,
	  GNOME_APP_PIXMAP_STOCK, GTK_STOCK_OPEN,
	  0, 0, NULL},
  	{ GNOME_APP_UI_ITEM, N_("_Save Session"),
	  N_("Save the current session to a file"),
	  on_save_tabs, NULL, NULL,
	  GNOME_APP_PIXMAP_STOCK, GTK_STOCK_SAVE,
	  0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_EXIT_ITEM(on_exit_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo edit1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("_Copy"),
	 N_("Copy highlighted text from main window"),
	 NULL/*gui_copyhtml_activate*/, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_COPY,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("_Search"),
	 N_("Search using the shortcut bar"),
	 on_search_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_FIND,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Advanced Search"),
	 N_("Advanced search using the search dialog"),
	 main_open_search_dialog, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_BOOK_OPEN,
	 GDK_F3, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	GNOME_APP_UI_ITEM, N_("_Preferences"),
	N_("Set user preferences"),
	on_preferences1_activate, NULL, NULL,
	GNOME_APP_PIXMAP_STOCK, "gtk-preferences",
	GDK_F2, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Module Manager"),
	 N_("Use module manager to install and remove Sword modules"),
	 on_mod_mgr, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-preferences",
	 GDK_F4, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo history1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("C_lear"),
	 N_("Clear history list"),
	 on_clear1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_CLEAR,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo view1_menu_uiinfo[] = {
	{ /* 0 */
	 GNOME_APP_UI_ITEM, N_("_Daily Devotion"),
	 N_("Show the Daily Devotion for today"),
	 (gpointer) on_daily_devotion1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_BOOK_OPEN,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,  /* 1 */
	{ /* 2 */
	 GNOME_APP_UI_TOGGLEITEM, N_("_Bible Texts"),
	 N_("Show or hide Bible texts window"),
	 (gpointer) view_bible_texts, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
 	{ /* 3 */
	 GNOME_APP_UI_TOGGLEITEM, N_("_Preview"),
	 N_("Show or hide Preview window"),
	 (gpointer) view_previewer, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 4 */
	 GNOME_APP_UI_TOGGLEITEM, N_("_Commentary"),
	 N_("Show or hide commentaries"),
	 (gpointer) view_upper_workbook, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 5 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Dictionary/_Lexicon"),
	 N_("Show or hide - dictionaries and lexicons"),
	 (gpointer) view_lower_workbook, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR, /* 6 */
	{ /* 7 */
	 GNOME_APP_UI_TOGGLEITEM, N_("_Verse Per Line"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 8 */
	 GNOME_APP_UI_TOGGLEITEM, N_("L_ink Tabs"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 9 */
	 GNOME_APP_UI_TOGGLEITEM, N_("_Read Aloud"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 10 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Show Verse _Numbers"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 11 */
	 GNOME_APP_UI_TOGGLEITEM, N_("_Highlight Current Verse"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
#ifdef USE_GTKMOZEMBED
	{ /* 12 */
	 GNOME_APP_UI_TOGGLEITEM, N_("D_ouble-Space Text"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
#endif /* USE_GTKMOZEMBED */
	GNOMEUIINFO_SEPARATOR, /* 13 */
	{ /* 14 */
	 GNOME_APP_UI_ITEM, N_("_Attach/Detach Sidebar"),
	 NULL,
	 gui_attach_detach_sidebar, NULL, NULL, 
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 15 */
	 GNOME_APP_UI_ITEM, N_("_Show/Hide Sidebar"),
	 NULL,
	 gui_sidebar_showhide, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	
#ifdef USE_PARALLEL_TAB	
	{ /* 16 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Show Parallel View in a tab"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},	
	{ /* 17 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Show Previewer in the Sidebar"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
#endif /*  USE_PARALLEL_TAB  */	
	GNOMEUIINFO_END
};

static GnomeUIInfo help1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("_Contents"),
	 N_("Open the GnomeSword manual"),
	 on_help_contents_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_HELP,
	 GDK_F1, 0, NULL},
	{
	GNOME_APP_UI_ITEM, N_("_Mailing List"),
	N_("Sign up for the users' mailing list"),
	on_mailing_list_activate, NULL, NULL,
	GNOME_APP_PIXMAP_STOCK, GTK_STOCK_ABOUT,
	0, 0, NULL},
	{
	GNOME_APP_UI_ITEM, N_("_Live Chat"),
	N_("Chat with other users and developers"),
	on_live_chat_activate, NULL, NULL,
	GNOME_APP_PIXMAP_STOCK, GTK_STOCK_ABOUT,
	0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Report Bug"),
	 N_("Report bug to SourceForge"),
	 on_report_bug_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("About _Translation"),
	 N_("Translating GnomeSword to other languages"),
	 on_about_translation_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("_About the SWORD Project"),
	 N_("More information about the SWORD Project"),
	 on_about_the_sword_project1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_ABOUT,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("About _GnomeSword"),
	 N_("About this application"),
	 on_about_gnomesword1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GTK_STOCK_ABOUT,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("_File"),
	 NULL,
	 file1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("_Edit"),
	 NULL,
	 edit1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("_View"),
	 NULL,
	 view1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Hi_story"),
	 NULL,
	 history1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("_Help"),
	 NULL,
	 help1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};


/******************************************************************************
 * Name
 *   gui_create_main_menu
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void gui_create_main_menu(GtkWidget *app)	
 *
 * Description
 *   create the main menu for gnomesword
 *
 * Return value
 *   void
 */

void
gui_create_main_menu(GtkWidget * app)
{
	/*
	 * this is total magic.  set up menu before using it.
	 * indices are direct from GnomeUIInfo above.
	 */
	if (!file1_menu_uiinfo[0].pixmap_info)
		file1_menu_uiinfo[0].pixmap_info =
		    image_locator("silk-edit-bookmarks.png");
	/* end magic */

	gnome_app_create_menus(GNOME_APP(app), menubar1_uiinfo);
	
	widgets.viewtexts_item = view1_menu_uiinfo[2].widget;
	widgets.viewpreview_item = view1_menu_uiinfo[3].widget; 
	widgets.viewcomms_item = view1_menu_uiinfo[4].widget;
	widgets.viewdicts_item = view1_menu_uiinfo[5].widget;
	widgets.versestyle_item = view1_menu_uiinfo[7].widget;
	widgets.linkedtabs_item = view1_menu_uiinfo[8].widget;
	widgets.readaloud_item = view1_menu_uiinfo[9].widget;
	widgets.showversenum_item = view1_menu_uiinfo[10].widget;
	widgets.versehighlight_item = view1_menu_uiinfo[11].widget;
#ifdef USE_PARALLEL_TAB	
	widgets.parallel_tab_item = view1_menu_uiinfo[16].widget;
	widgets.side_preview_item = view1_menu_uiinfo[17].widget;
#endif /*  USE_PARALLEL_TAB  */	
	gtk_widget_hide(edit1_menu_uiinfo[0].widget);	/* disable Copy */
	gtk_widget_hide(edit1_menu_uiinfo[1].widget);	/* disable Copy */
#ifdef USE_GTKMOZEMBED
	widgets.doublespace_item = view1_menu_uiinfo[12].widget;
#endif /* USE_GTKMOZEMBED */
#if defined(__CYGWIN__) || defined(WIN32)
	gtk_widget_hide(view1_menu_uiinfo[9].widget);
#endif /* __CYGWIN */

	/* map tab's show state into view menu. */
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewtexts_item),
				       settings.showtexts);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewcomms_item),
				       settings.showcomms);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewdicts_item),
				       settings.showdicts);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewpreview_item),
				       settings.showpreview);
#ifdef USE_PARALLEL_TAB	
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.parallel_tab_item),
				       settings.showparatab);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.side_preview_item),
				       settings.show_previewer_in_sidebar);
#endif /*  USE_PARALLEL_TAB  */	
	
	/* update other status toys */
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.versestyle_item),
				       settings.versestyle);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.linkedtabs_item),
				       settings.linkedtabs);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.readaloud_item),
				       settings.readaloud);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.showversenum_item),
				       settings.showversenum);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.versehighlight_item),
				       settings.versehighlight);
#ifdef USE_GTKMOZEMBED
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.doublespace_item),
				       settings.doublespace);
#endif /* USE_GTKMOZEMBED */
	
	g_signal_connect(GTK_OBJECT(widgets.versestyle_item),
			   "toggled",
			   G_CALLBACK(on_verse_style1_activate),
			   NULL);
	g_signal_connect(GTK_OBJECT(widgets.linkedtabs_item),
			   "toggled",
			   G_CALLBACK(on_linked_tabs_activate),
			   NULL);
	g_signal_connect(GTK_OBJECT(widgets.readaloud_item),
			   "toggled",
			   G_CALLBACK(on_read_aloud_activate),
			   NULL);
	g_signal_connect(GTK_OBJECT(widgets.showversenum_item),
			   "toggled",
			   G_CALLBACK(on_show_verse_numbers_activate),
			   NULL);
	g_signal_connect(GTK_OBJECT(widgets.versehighlight_item),
			   "toggled",
			   G_CALLBACK(on_versehighlight_activate),
			   NULL);
			   
#ifdef USE_PARALLEL_TAB	
	g_signal_connect(GTK_OBJECT(widgets.parallel_tab_item),
			   "toggled",
			   G_CALLBACK(on_parallel_tab_activate),
			   NULL);
	g_signal_connect(GTK_OBJECT(widgets.side_preview_item),
			   "toggled",
			   G_CALLBACK(on_side_preview_activate),
			   NULL);
#endif /*  USE_PARALLEL_TAB  */	
			   
#ifdef USE_GTKMOZEMBED
	g_signal_connect(GTK_OBJECT(widgets.doublespace_item),
			   "toggled",
			   G_CALLBACK(on_doublespace_activate),
			   NULL);
#endif /* USE_GTKMOZEMBED */
}

/******************************************************************************
 * Name
 *  gui_install_menu_hints
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void gui_install_menu_hints(GtkWidget *app)	
 *
 * Description
 *   install hints - display menu hints in status bar
 *
 * Return value
 *   void
 */

void
gui_install_menu_hints(GtkWidget * app)
{
	gnome_app_install_menu_hints(GNOME_APP(app), menubar1_uiinfo);
}
