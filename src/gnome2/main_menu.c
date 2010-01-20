/*
 * Xiphos Bible Study Tool
 * main_menu.c - creation of and call backs for xiphos main menu
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <unistd.h>

#include <glade/glade-xml.h>

#include "editor/slib-editor.h"

#include "gui/about_xiphos.h"
#include "gui/about_sword.h"
#include "gui/about_modules.h"
#include "gui/about_trans.h"
#include "gui/xiphos.h"
#include "gui/main_window.h"
#include "gui/main_menu.h"
#include "gui/mod_mgr.h"
#include "gui/preferences_dialog.h"
#include "gui/parallel_tab.h"
#include "gui/sidebar.h"
#include "gui/sidebar_dialog.h"
#include "gui/search_dialog.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/lists.h"
#include "main/sword.h"
#include "main/search_dialog.h"
#include "main/tab_history.h"
#include "main/url.hh"
#include "main/xml.h"

#include <glib/gstdio.h>

#include "gui/debug_glib_null.h"

//global function to handle gtk_uri calls
void
link_uri_hook(GtkLinkButton *button,
	      const gchar *link,
	      gpointer user_data)
{
	xiphos_open_default(link);
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
G_MODULE_EXPORT void
on_help_contents_activate(GtkMenuItem * menuitem, gpointer user_data)
{	
	GError *error = NULL;
#ifdef WIN32
	const char *lang = g_getenv("LANG");
	gchar *help_file = g_win32_get_package_installation_directory_of_module(NULL);
	help_file = g_strconcat(help_file, "\0", NULL);
	if (!strncmp(lang, "fr", 2))
		help_file = g_build_filename(help_file, "share", "help", "xiphos_fr.chm", NULL);
	else if (!strncmp(lang, "fa", 2))
		help_file = g_build_filename(help_file, "share", "help", "xiphos_fa.chm", NULL);
	else
		help_file = g_build_filename(help_file, "share", "help", "xiphos.chm", NULL);
	xiphos_open_default(help_file);
	g_free(help_file);
#else
	gtk_show_uri (NULL, "ghelp:xiphos", gtk_get_current_event_time(), &error);
	if (error != NULL) {
		GS_warning(("%s", error->message));
		g_error_free(error);
	}

#endif /* WIN32 */
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
G_MODULE_EXPORT void
on_mailing_list_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	xiphos_open_default("https://lists.sourceforge.net/lists/listinfo/gnomesword-users/");
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
 *   open web browser to freenode irc chat
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_live_chat_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *user = g_strdup_printf("%s", g_get_user_name()), *s, *url;
	for (s = strchr(user, ' '); s; s = strchr(s, ' '))
		*s = '_';
	url = g_strdup_printf(
	    "http://webchat.freenode.net/?nick=%s|xiphos&channels=xiphos&prompt=1",
	    user);
	xiphos_open_default(url);
	g_free(url);
	g_free(user);
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
G_MODULE_EXPORT void
on_report_bug_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	xiphos_open_default(PACKAGE_BUGREPORT);
}


/******************************************************************************
 * Name
 *  on_about_the_sword_project_activate
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
G_MODULE_EXPORT void
on_about_the_sword_project_activate(GtkMenuItem * menuitem, gpointer user_data)
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
G_MODULE_EXPORT void
on_about_translation_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gui_create_about_trans();
	gtk_widget_show(dlg);
}

/******************************************************************************
 * Name
 *  on_daily_devotion_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_daily_devotion_activate(GtkMenuItem *menuitem, 
 *						gpointer user_data)	
 *
 * Description
 *   display daily devotion in shortcut bar viewer
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_daily_devotion_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	main_display_devotional();
}

/******************************************************************************
 * Name
 *  on_preferences_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_preferences_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   open preferences dialog
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_preferences_activate(GtkMenuItem * menuitem, gpointer user_data)
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
G_MODULE_EXPORT void on_search_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	if (!settings.showshortcutbar) 
		gui_sidebar_showhide();
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),2);
}

/******************************************************************************
 * Name
 *  on_verse_style_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_verse_style_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle between verse and paragraph style
 *
 * Return value
 *   void
 */

extern gboolean style_display;

G_MODULE_EXPORT void
on_verse_style_activate(GtkCheckMenuItem * menuitem, gpointer user_data)
{
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
G_MODULE_EXPORT void
on_linked_tabs_activate(GtkCheckMenuItem * menuitem, gpointer user_data)
{
	settings.linkedtabs = menuitem->active;
	xml_set_value("Xiphos", "misc", "pinnedtabs",
		      (settings.linkedtabs ? "1" : "0"));
	if (settings.showparatab)
		gui_force_parallel_tab_sync();
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
G_MODULE_EXPORT void
on_read_aloud_activate(GtkCheckMenuItem * menuitem, gpointer user_data)
{
	settings.readaloud = menuitem->active;
	xml_set_value("Xiphos", "misc", "readaloud",
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
G_MODULE_EXPORT void
on_show_verse_numbers_activate(GtkCheckMenuItem * menuitem, gpointer user_data)
{
	settings.showversenum = menuitem->active;
	xml_set_value("Xiphos", "misc", "showversenum",
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
G_MODULE_EXPORT void
on_versehighlight_activate(GtkCheckMenuItem * menuitem, gpointer user_data)
{
	settings.versehighlight = menuitem->active;
	xml_set_value("Xiphos", "misc", "versehighlight",
		      (settings.versehighlight ? "1" : "0"));
	main_display_bible(NULL, settings.currentverse);
}

/******************************************************************************
 * Name
 *  gui_parallel_tab_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void gui_parallel_tab_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle special current verse highlight.
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
gui_parallel_tab_activate(GtkCheckMenuItem * menuitem, gpointer user_data)
{
	if(!settings.browsing) return;
	if(!settings.showparatab && !menuitem->active) {
		xml_set_value("Xiphos", "misc", "showparatab", "0");
		return;
	}
		
	settings.showparatab = menuitem->active;
	xml_set_value("Xiphos", "misc", "showparatab",
		      (settings.showparatab ? "1" : "0"));
	if(settings.showparatab) {
		gui_open_parallel_view_in_new_tab();
		gui_force_parallel_tab_sync();
	} else 		
		gui_close_passage_tab  (gtk_notebook_page_num (
					GTK_NOTEBOOK (widgets.notebook_main),
					widgets.parallel_tab));
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
G_MODULE_EXPORT void
on_side_preview_activate(GtkCheckMenuItem * menuitem, gpointer user_data)
{
	settings.show_previewer_in_sidebar = menuitem->active;
	xml_set_value("Xiphos", "misc", "show_side_preview",
		      (settings.show_previewer_in_sidebar ? "1" : "0"));
	gui_show_previewer_in_sidebar(settings.show_previewer_in_sidebar);
}

/******************************************************************************
 * Name
 *   on_double_space_text_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_double_space_text_activate(GtkMenuItem *menuitem, gpointer user_data)	
 *
 * Description
 *   toggle double-spacing of text.
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_double_space_text_activate(GtkCheckMenuItem * menuitem, gpointer user_data)
{
	settings.doublespace = menuitem->active;
	xml_set_value("Xiphos", "misc", "doublespace",
		      (settings.doublespace ? "1" : "0"));
	//main_display_bible(NULL, settings.currentverse);
	main_refresh_all();
}

/******************************************************************************
 * Name
 *  on_quit_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_quit_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   do a nice orderly shut down and exit xiphos
 *   by calling gtk_widget_destroy() which will call
 *   on_mainwindow_destroy()
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_quit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	shutdown_frontend();
	/* shutdown the sword stuff */
	main_shutdown_backend();
	gtk_main_quit();
	gtk_widget_destroy(widgets.app);
	exit(0);
}


/******************************************************************************
 * Name
 *  on_about_xiphos_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_about_xiphos_activate(GtkMenuItem * menuitem, 
 *					gpointer user_data)	
 *
 * Description
 *   display xiphos about dialog 
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_about_xiphos_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *AboutBox;

	AboutBox = gui_create_about_xiphos();
	gtk_widget_show(AboutBox);
}


/******************************************************************************
 * Name
 *  on_save_session_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_save_session_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   ask for a file name (with file-chooser) and save the current tabs to that file
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_save_session_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dialog;
	gchar *tabs_dir;

	tabs_dir = g_strdup_printf("%s/tabs/",settings.gSwordDir);
	
	if (g_access(tabs_dir, F_OK) == -1) {
		if ((g_mkdir(tabs_dir, S_IRWXU)) == -1) {
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
 *  redisplay_to_realign
 *
 * Synopsis
 *   void redisplay_to_realign()	
 *
 * Description
 *    when en/disabling panes, we must redisplay in order that text
 *    (especially current verse) not suddenly find themselves out of view.
 *
 * Return value
 *   void
 */
static void
redisplay_to_realign()
{
	static int realign_busy = FALSE;

	if (realign_busy || change_tabs_no_redisplay)
		return;
	realign_busy = TRUE;

	/* first realize the pane size updates. */
	while (gtk_events_pending())
		gtk_main_iteration();
	/* then just redisplay everything as-is. */
	gchar *url = g_strdup_printf("sword://%s/%s",
				     settings.MainWindowModule,
				     settings.currentverse);
	main_url_handler(url, TRUE);		
	g_free(url);
	if (settings.DictWindowModule) {
		url = g_strdup_printf("sword://%s/%s",
				      settings.DictWindowModule,
				      settings.dictkey);
		main_url_handler(url, TRUE);		
		g_free(url);
	}

	realign_busy = FALSE;
}

/******************************************************************************
 * Name
 *  on_open_session_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_open_session_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   ask for file name (with file-chooser) and load tabs from that file
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_open_session_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dialog;
	gchar *tabs_dir;

	tabs_dir = g_strdup_printf("%s/tabs/",settings.gSwordDir);
	
	if (g_access(tabs_dir, F_OK) == -1) {
		if ((g_mkdir(tabs_dir, S_IRWXU)) == -1) {
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
		redisplay_to_realign();
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

/******************************************************************************
 * Name
 *  on_show_bible_text_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_show_bible_text_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    toggle Bibletext window
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_show_bible_text_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_show_hide_texts(GTK_CHECK_MENU_ITEM(menuitem)->active);
	redisplay_to_realign();
}

 
G_MODULE_EXPORT void
on_preview_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_show_hide_preview(GTK_CHECK_MENU_ITEM(menuitem)->active);
	redisplay_to_realign();
}


/******************************************************************************
 * Name
 *  on_show_commentary_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_show_commentary_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    toggle commentary window *** we need to change the name of this
 *    function because there no longer an upper_workbook
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_show_commentary_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_show_hide_comms(GTK_CHECK_MENU_ITEM(menuitem)->active);
	redisplay_to_realign();
}


/******************************************************************************
 * Name
 *   on_show_dictionary_lexicon_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_show_dictionary_lexicon_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    toggle lower_workbook view (on or off)
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_show_dictionary_lexicon_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_show_hide_dicts(GTK_CHECK_MENU_ITEM(menuitem)->active);
	redisplay_to_realign();
}


/******************************************************************************
 * Name
 *   on_module_manager_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_module_manager_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_module_manager_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_open_mod_mgr();
}

/******************************************************************************
 * Name
 *   on_open_studypad_activate
 *
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 *   void on_open_studypad_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    open studypad editor - if studypad dialog exist bring it to the
 *    top
 *
 * Return value
 *   void
 */
G_MODULE_EXPORT void
on_open_studypad_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	editor_create_new(settings.studypadfilename, NULL, FALSE);
}


G_MODULE_EXPORT void
on_advanced_search_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	main_open_search_dialog ();
}


G_MODULE_EXPORT void
on_attach_detach_sidebar_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	gui_attach_detach_sidebar ();
}


G_MODULE_EXPORT void
on_sidebar_showhide_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	gui_sidebar_showhide ();
}


GtkWidget *gui_create_main_menu(void)
{
	gchar *glade_file;
	GladeXML *gxml;
 //   	const gchar *mname = NULL;
	
	glade_file = gui_general_user_file ("xi-menus.glade", FALSE);
	g_return_val_if_fail (glade_file != NULL, NULL);
	
	gxml = glade_xml_new (glade_file, "menu_main", NULL);
		
	g_free (glade_file);
	g_return_val_if_fail (gxml != NULL, NULL);
	
	GtkWidget *menu 	= glade_xml_get_widget (gxml, "menu_main");
	
	widgets.viewtexts_item = glade_xml_get_widget (gxml, "show_bible_text");
	widgets.viewpreview_item = glade_xml_get_widget (gxml, "preview");
	widgets.viewcomms_item = glade_xml_get_widget (gxml, "commentary");
	widgets.viewdicts_item = glade_xml_get_widget (gxml, "show_dictionary_lexicon");
	widgets.versestyle_item = glade_xml_get_widget (gxml, "verse_per_line");
	widgets.linkedtabs_item = glade_xml_get_widget (gxml, "link_tabs");
	widgets.readaloud_item = glade_xml_get_widget (gxml, "read_aloud");
	widgets.showversenum_item = glade_xml_get_widget (gxml, "show_verse_numbers");
	widgets.versehighlight_item = glade_xml_get_widget (gxml, "highlight_current_verse");
	widgets.parallel_tab_item = glade_xml_get_widget (gxml, "show_parallel_view_in_a_tab");
	widgets.side_preview_item = glade_xml_get_widget (gxml, "show_previewer_in_sidebar");
	
	widgets.doublespace_item = glade_xml_get_widget (gxml, "double_space_text");
#ifndef USE_GTKMOZEMBED
	gtk_widget_hide(widgets.doublespace_item);
#endif /* !USE_GTKMOZEMBED */
	
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
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.parallel_tab_item),
				       settings.showparatab);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.side_preview_item),
				       settings.show_previewer_in_sidebar);
		
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
	
    	/* connect signals and data */
	glade_xml_signal_autoconnect_full
		(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func, NULL);

	//set up global function to handle all link buttons
	gtk_link_button_set_uri_hook (link_uri_hook, NULL, NULL);
	
	return menu;
}
