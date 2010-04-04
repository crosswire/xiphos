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
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *   toggle between verse and paragraph style
 */
extern gboolean style_display;

G_MODULE_EXPORT void
on_verse_style_activate(GtkToggleAction * action, gpointer user_data)
{
        if (style_display) {
                gchar *file = g_strdup_printf("%s/modops.conf",
                                              settings.gSwordDir);
                gchar *url = g_strdup_printf("sword://%s/%s",
                                             settings.MainWindowModule,
                                             settings.currentverse);
                /* remember our choice for the next program startup */
                settings.versestyle = gtk_toggle_action_get_active(action);
                save_conf_file_item(file, settings.MainWindowModule, "style",
                                    (settings.versestyle
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
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *   toggle linking tabs together.
 */
G_MODULE_EXPORT void
on_linked_tabs_activate(GtkToggleAction * action, gpointer user_data)
{
        settings.linkedtabs = gtk_toggle_action_get_active(action);
        xml_set_value("Xiphos", "misc", "pinnedtabs",
                      (settings.linkedtabs ? "1" : "0"));
        if (settings.showparatab)
                gui_force_parallel_tab_sync();
}

/******************************************************************************
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *   toggle reading scripture out loud.
 */
G_MODULE_EXPORT void
on_read_aloud_activate(GtkToggleAction * action, gpointer user_data)
{
        settings.readaloud = gtk_toggle_action_get_active (action);
        xml_set_value("Xiphos", "misc", "readaloud",
                      (settings.readaloud ? "1" : "0"));
        if (settings.readaloud)
                main_display_bible(NULL, settings.currentverse);
}

/******************************************************************************
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *   toggle showing verse numbers together.
 */
G_MODULE_EXPORT void
on_show_verse_numbers_activate(GtkToggleAction * action, gpointer user_data)
{
        settings.showversenum = gtk_toggle_action_get_active(action);
        xml_set_value("Xiphos", "misc", "showversenum",
                      (settings.showversenum ? "1" : "0"));
        main_display_bible(NULL, settings.currentverse);
}

/******************************************************************************
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *   toggle special current verse highlight.
 */
G_MODULE_EXPORT void
on_versehighlight_activate(GtkToggleAction * action, gpointer user_data)
{
        settings.versehighlight = gtk_toggle_action_get_active(action);
        xml_set_value("Xiphos", "misc", "versehighlight",
                      (settings.versehighlight ? "1" : "0"));
        main_display_bible(NULL, settings.currentverse);
}

/******************************************************************************
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *   toggle special current verse highlight.
 */
G_MODULE_EXPORT void
gui_parallel_tab_activate(GtkToggleAction * action, gpointer user_data)
{
        if (!settings.browsing) return;

        if (!settings.showparatab && !(gtk_toggle_action_get_active(action))) {
                xml_set_value("Xiphos", "misc", "showparatab", "0");
                return;
        }

        settings.showparatab = gtk_toggle_action_get_active(action);
        xml_set_value("Xiphos", "misc", "showparatab",
                      (settings.showparatab ? "1" : "0"));
        if (settings.showparatab) {
                gui_open_parallel_view_in_new_tab();
                gui_force_parallel_tab_sync();
        } else
                gui_close_passage_tab  (gtk_notebook_page_num (
                                        GTK_NOTEBOOK (widgets.notebook_main),
                                        widgets.parallel_tab));
}

/******************************************************************************
 * Synopsis
 *   #include "gui/main_menu.h"
 *   void on_side_preview_activate(GtkMenuItem *menuitem, gpointer user_data)
 * Description
 *   toggle special previewer in sidebar.
 */
G_MODULE_EXPORT void
on_side_preview_activate(GtkToggleAction * action, gpointer user_data)
{
        settings.show_previewer_in_sidebar = gtk_toggle_action_get_active(action);
        xml_set_value("Xiphos", "misc", "show_side_preview",
                      (settings.show_previewer_in_sidebar ? "1" : "0"));
        gui_show_previewer_in_sidebar(settings.show_previewer_in_sidebar);
}

/******************************************************************************
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *   toggle double-spacing of text.
 */
G_MODULE_EXPORT void
on_double_space_text_activate(GtkToggleAction * action, gpointer user_data)
{
        settings.doublespace = gtk_toggle_action_get_active (action);
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
 *    (especially current verse) not suddenly find itself out of view.
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
        sync_windows();

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

/***
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *    toggle Bibletext window
 */
G_MODULE_EXPORT void
on_show_bible_text_activate(GtkToggleAction * action, gpointer user_data)
{
        gui_show_hide_texts(gtk_toggle_action_get_active (action));
        redisplay_to_realign();
}


G_MODULE_EXPORT void
on_preview_activate(GtkToggleAction *action, gpointer user_data)
{
        gui_show_hide_preview(gtk_toggle_action_get_active(action));
        redisplay_to_realign();
}


/******************************************************************************
 * Synopsis
 *   #include "gui/main_menu.h"
 * Description
 *    toggle commentary window *** we need to change the name of this
 *    function because there no longer an upper_workbook
 */
G_MODULE_EXPORT void
on_show_commentary_activate(GtkToggleAction *action, gpointer user_data)
{
        gui_show_hide_comms(gtk_toggle_action_get_active (action));
        redisplay_to_realign();
}


/******************************************************************************
 * Synopsis
 *   #include "gui/main_menu.h"
 *
 * Description
 *    toggle lower_workbook view (on or off)
 */
G_MODULE_EXPORT void
on_show_dictionary_lexicon_activate(GtkToggleAction * action, gpointer user_data)
{
        gui_show_hide_dicts(gtk_toggle_action_get_active(action));
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


GtkWidget *gui_create_main_menu(GtkWidget * main_window)
{
        GtkActionGroup * action_group = gtk_action_group_new("MainMenuActions");
        gchar * fname = gui_get_system_data_file("main-menus-new.ui");
        GError *error;

        widgets.app_actions = gtk_action_group_new("AppActions");
        widgets.view_actions = gtk_action_group_new("ViewActions");
        widgets.main_menu = gtk_ui_manager_new();

        GtkActionEntry app_entries[] = {
                { "OpenStudypad", NULL,
                  N_("Open Study_pad"), NULL, NULL,
                  G_CALLBACK (on_open_studypad_activate) },
                { "OpenSession", GTK_STOCK_OPEN,
                  N_("_Open Session"), NULL, NULL,
                  G_CALLBACK (on_open_session_activate) },
                { "SaveSession", GTK_STOCK_SAVE,
                  N_("_Save Session"), NULL, NULL,
                  G_CALLBACK (on_save_session_activate) },
                { "Quit", GTK_STOCK_QUIT,
                  NULL, NULL, NULL,
                  G_CALLBACK (on_quit_activate) },
                { "Search", GTK_STOCK_FIND,
                  N_("_Search"), "", NULL,
                  G_CALLBACK (on_search_activate) },
                { "AdvancedSearch", GTK_STOCK_FIND,
                  N_("_Advaned Search"), "F3", NULL,
                  G_CALLBACK (on_advanced_search_activate) },
                { "Preferences", GTK_STOCK_PREFERENCES,
                  NULL, "F2", NULL,
                  G_CALLBACK (on_preferences_activate) },
                { "ModuleManager", GTK_STOCK_PREFERENCES,
                  N_("_Module Manager"), "F4",
                  N_("Use module manager to install and remove Sword modules"),
                  G_CALLBACK (on_module_manager_activate) },
                { "Help", GTK_STOCK_HELP,
                  N_("_Contents"), "F1", NULL,
                  G_CALLBACK (on_help_contents_activate) },
                { "MailingList", NULL,
                  N_("_Mailing List"), NULL, NULL,
                  G_CALLBACK (on_mailing_list_activate) },
                { "LiveChat", NULL,
                  N_("_Live Chat"), NULL, NULL,
                  G_CALLBACK (on_live_chat_activate) },
                { "ReportBug", GTK_STOCK_DIALOG_ERROR,
                  N_("_Report Bug"), NULL, NULL,
                  G_CALLBACK (on_report_bug_activate) },
                { "AboutTranslation", NULL,
                  N_("About _Translation"), NULL, NULL,
                  G_CALLBACK (on_about_translation_activate) },
                { "AboutSword", NULL,
                  N_("_About The SWORD Project"), NULL, NULL,
                  G_CALLBACK (on_about_the_sword_project_activate) },
                { "AboutXiphos", NULL,
                  N_("About _Xiphos"), NULL, NULL,
                  G_CALLBACK (on_about_xiphos_activate) },
                { "AttachDetachSidebar", NULL,
                  N_("_Attach/Detach Sidebar"), NULL, NULL,
                  G_CALLBACK (on_attach_detach_sidebar_activate) },
                { "ShowHideSidebar", NULL,
                  N_("_Show/Hide Attached Sidebar"), NULL, NULL,
                  G_CALLBACK (on_sidebar_showhide_activate) },
                { "DailyDevotion", NULL,
                  N_("Preview _Daily Devotion"), NULL, NULL,
                  G_CALLBACK (on_daily_devotion_activate) }
        };

        gtk_action_group_add_actions (widgets.app_actions, app_entries,
                                      G_N_ELEMENTS(app_entries), NULL);

        GtkActionEntry local_entries[] = {
                { "FileMenu", NULL, N_("_File") },
                { "EditMenu", NULL, N_("_Edit") },
                { "ViewMenu", NULL, N_("_View") },
                { "HelpMenu", NULL, N_("_Help") },

        };

        gtk_action_group_add_actions (action_group, local_entries,
                                      G_N_ELEMENTS(local_entries), NULL);


        GtkToggleActionEntry t_entries[] = {
                { "BibleText", NULL,
                  N_("_Bible"), NULL, NULL,
                  G_CALLBACK (on_show_bible_text_activate),
                  FALSE },
                { "Preview", NULL,
                  N_("_Preview"), NULL, NULL,
                  G_CALLBACK (on_preview_activate),
                  FALSE },
                { "Commentary", NULL,
                  N_("_Commentary/Book"), NULL, NULL,
                  G_CALLBACK (on_show_commentary_activate),
                  FALSE },
                { "Dictionary", NULL,
                  N_("Dictionar_y"), NULL, NULL,
                  G_CALLBACK (on_show_dictionary_lexicon_activate),
                  FALSE },
                { "VersePerLine", NULL,
                  N_("_Verse Per Line"), NULL, NULL,
                  G_CALLBACK (on_verse_style_activate),
                  FALSE },
                { "LinkTabs", NULL,
                  N_("L_ink Tabs"), NULL, NULL,
                  G_CALLBACK (on_linked_tabs_activate),
                  FALSE },
                { "ReadAloud", NULL,
                  N_("_Read Aloud"), NULL, NULL,
                  G_CALLBACK (on_read_aloud_activate),
                  FALSE },
                { "ShowVerseNumbers", NULL,
                  N_("Show Verse _Numbers"), NULL, NULL,
                  G_CALLBACK (on_show_verse_numbers_activate),
                  FALSE },
                { "HighlightCurrentVerse", NULL,
                  N_("_Highlight Current Verse"), NULL, NULL,
                  G_CALLBACK (on_versehighlight_activate),
                  FALSE },
                { "DoubleSpaceText", NULL,
                  N_("D_ouble-Space Text"), NULL, NULL,
                  G_CALLBACK (on_double_space_text_activate),
                  FALSE },
                { "ShowParallelViewTab", NULL,
                  N_("Show Parallel View in a  _Tab"), NULL, NULL,
                  G_CALLBACK(gui_parallel_tab_activate),
                  FALSE },
                { "ShowPreviewerSidebar", NULL,
                  N_("Show Previe_wer in the Sidebar"), NULL, NULL,
                  G_CALLBACK (on_side_preview_activate),
                  FALSE },
        };

        /* map tab's show state into view menu. */
        t_entries[0].is_active = (gboolean) settings.showtexts;
        t_entries[1].is_active = (gboolean) settings.showpreview;
        t_entries[2].is_active = (gboolean) settings.showcomms;
        t_entries[3].is_active = (gboolean) settings.showdicts;
        t_entries[4].is_active = (gboolean) settings.versestyle;
        t_entries[5].is_active = (gboolean) settings.linkedtabs;
        t_entries[6].is_active = (gboolean) settings.readaloud;
        t_entries[7].is_active = (gboolean) settings.showversenum;
        t_entries[8].is_active = (gboolean) settings.versehighlight;
        t_entries[9].is_active = (gboolean) settings.doublespace;
        t_entries[10].is_active = (gboolean) settings.showparatab;
        t_entries[11].is_active = (gboolean) settings.show_previewer_in_sidebar;

        gtk_action_group_add_toggle_actions (widgets.view_actions, t_entries,
                                             G_N_ELEMENTS(t_entries), NULL);

        gtk_ui_manager_insert_action_group (widgets.main_menu, widgets.app_actions, 0);
        gtk_ui_manager_insert_action_group (widgets.main_menu, action_group, 0);
        gtk_ui_manager_insert_action_group (widgets.main_menu, widgets.view_actions, 0);

        /* Read in the UI from our XML file */
        error = NULL;
        gtk_ui_manager_add_ui_from_file (widgets.main_menu, fname, &error);

        if (error)
        {
                g_message ("building menus failed: %s", error->message);
                g_error_free (error);
        }
        g_free(fname);


        widgets.viewtexts_item = gtk_ui_manager_get_widget(widgets.main_menu, "/ViewMenu/BibleText");
        widgets.viewpreview_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/Preview");
        widgets.viewcomms_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/Commentary");
        widgets.viewdicts_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/Dictionary");
        widgets.versestyle_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/VersePerLine");
        widgets.linkedtabs_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/LinkTabs");
        widgets.readaloud_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/ReadAloud");
        widgets.showversenum_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/ShowVerseNumbers");
        widgets.versehighlight_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/HighlightCurrentVerse");
        widgets.parallel_tab_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/ShowParallelViewTab");
        widgets.side_preview_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/ShowPreviewerSidebar");
        widgets.doublespace_item = gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu/ViewMenu/DoubleSpaceText");

#ifndef USE_GTKMOZEMBED
        gtk_widget_hide(widgets.doublespace_item);
#endif /* !USE_GTKMOZEMBED */

        //set up global function to handle all link buttons
        gtk_link_button_set_uri_hook (link_uri_hook, NULL, NULL);
        gtk_window_add_accel_group (GTK_WINDOW (main_window),
                                    gtk_ui_manager_get_accel_group (widgets.main_menu));

        return gtk_ui_manager_get_widget (widgets.main_menu, "/MainMenu");
}
