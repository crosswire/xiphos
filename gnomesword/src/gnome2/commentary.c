/*
 * GnomeSword Bible Study Tool
 * commentary.c - gui for commentary modules
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#include <errno.h>
#include <gnome.h>


#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#include "editor/html-editor.h"
#endif

#include "gui/dialog.h"
#include "gui/commentary.h"
#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/sidebar.h"
#include "gui/main_window.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"


#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/global_ops.hh"
#include "main/search_sidebar.h"
#include "main/display.hh"

//static void create_menu(GdkEventButton * event);

/******************************************************************************
 * Name
 *  global_option_red_words
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_global_option(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_global_option(GtkMenuItem * menuitem, gpointer data)
{
	
	main_save_module_options(settings.MainWindowModule, (gchar *) data,
			    GTK_CHECK_MENU_ITEM(menuitem)->active);
	main_display_commentary(settings.CommWindowModule,settings.comm_key);
}


/******************************************************************************
 * Name
 *  on_comm_button_press_event
 *
 * Synopsis
 *   #include ".h"
 *
 *  gboolean on_comm_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */
static gboolean on_comm_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		gui_create_pm_commentary(); //gui_popup_pm_comm(event);
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *  gboolean on_button_release_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_comm_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	extern gboolean in_url;
	gchar *key;
	const gchar *url;
	gchar *buf = NULL;
	
	settings.whichwindow = COMMENTARY_WINDOW;

#ifdef GTKHTML	
	switch (event->button) {
	case 1:
		if (in_url) 
			break;
		key = gui_button_press_lookup(widgets.html_comm);
		if (key) {
			if(g_strstr_len(key,strlen(key),"*")) {
				key = g_strdelimit(key, "*", ' ');
				key = g_strstrip(key);
				url = g_strdup_printf(
					"gnomesword.url?action=showModInfo&value=1&module=%s",
					key);
				main_url_handler(url,TRUE);
				g_free((gchar*)url);
				g_free(key);
				break;
			}
			gchar *dict = NULL;
			if (settings.useDefaultDict)
				dict =
				    g_strdup(settings.
					     DefaultDict);
			else
				dict =
				    g_strdup(settings.
					     DictWindowModule);
			main_display_dictionary(dict, key);
			g_free(key);
			if (dict)
				g_free(dict);
		}
		break;
	case 2:
		if (!in_url)
			break;
#ifdef USE_GTKHTML38
		url = gtk_html_get_url_at (GTK_HTML(widgets.html_comm),		
								event->x,
								event->y);
#else
		url = html_engine_get_link_at (GTK_HTML(widgets.html_comm)->engine,
					 event->x,
					 event->y);
#endif
		if (url) {
			if(strstr(url,"sword://")) {
				gchar **work_buf = g_strsplit (url,"/",4);
				gui_open_passage_in_new_tab(work_buf[3]);
				g_strfreev(work_buf);
			}
		} else {
			gui_generic_warning("Middle-click invalid.");
		}
		break;
	case 3:
		break;
	}
#endif /* GTKHTML */
	return FALSE;
}


static gboolean on_enter_notify_event(GtkWidget * widget,
				      GdkEventCrossing * event,
				      gpointer user_data)
{
	//shift_key_presed = FALSE;
	//gtk_widget_grab_focus (widgets.html_comm);
	//settings.whichwindow = COMMENTARY_WINDOW;
	//gui_change_window_title(settings.CommWindowModule);
  	return FALSE;
}


#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (GeckoHtml *html,
			     gchar *uri,
			     gpointer user_data)
{	
	gui_create_pm_commentary(); 
}
#endif

/******************************************************************************
 * Name
 *   gui_create_commentary_pane
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   GtkWidget *gui_create_commentary_pane(void)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_commentary_pane(void)
{
	GtkWidget *box_comm;
	GtkWidget *scrolledwindow;
	GtkWidget *eventbox1;
	box_comm = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box_comm);
/*
	widgets.label_comm = gtk_label_new(settings.CommWindowModule);
	gtk_widget_show(widgets.label_comm);
	gtk_box_pack_start(GTK_BOX(box_comm),
			   widgets.label_comm, FALSE,
			   FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (widgets.label_comm), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment (GTK_MISC (widgets.label_comm), 0, 0.5);
*/
#ifdef USE_GTKMOZEMBED	
	eventbox1 = gtk_event_box_new ();
	gtk_widget_show (eventbox1);
	gtk_box_pack_start(GTK_BOX(box_comm),
			   eventbox1, TRUE,
			   TRUE, 0);
	widgets.html_comm = GTK_WIDGET(gecko_html_new(NULL, FALSE, COMMENTARY_TYPE));
	gtk_widget_show(widgets.html_comm);
	gtk_container_add(GTK_CONTAINER(eventbox1),
			 widgets.html_comm);
	
	g_signal_connect((gpointer)widgets.html_comm,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      NULL);
	g_signal_connect ((gpointer) eventbox1, "enter_notify_event",
		    G_CALLBACK (on_enter_notify_event),
		    NULL);

#else
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box_comm),
			   scrolledwindow, TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	/*gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow,
                                             settings.shadow_type);*/

	widgets.html_comm = gtk_html_new();
	gtk_widget_show(widgets.html_comm);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_comm);
	g_signal_connect(GTK_OBJECT(widgets.html_comm), "link_clicked",
				   G_CALLBACK(gui_link_clicked),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_comm), "on_url",
				   G_CALLBACK(gui_url),
				   GINT_TO_POINTER(COMMENTARY_TYPE));
	g_signal_connect(GTK_OBJECT(widgets.html_comm),
				   "button_press_event",
				   G_CALLBACK
				   (on_comm_button_press_event),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_comm),
				   "button_release_event",
				   G_CALLBACK
				   (on_comm_button_release_event),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_comm),
			   "url_requested",
			   G_CALLBACK(url_requested), NULL);
#endif
	return box_comm;
}



/**  Main Commentary menu stuff
 **
 **
 **
 **
 **
 **
 **/

static void on_about_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_display_about_module_dialog(settings.CommWindowModule, FALSE);
}


static void on_item1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void on_print1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
#ifdef USE_GTKMOZEMBED
	gecko_html_print_document (GTK_WINDOW(widgets.app), 
				   settings.CommWindowModule, 
				   GECKO_HTML(widgets.html_comm));
#else
	gui_html_print(widgets.html_comm, FALSE);
#endif
}


static void on_copy2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
#ifdef USE_GTKMOZEMBED	
	gecko_html_copy_selection(GECKO_HTML(widgets.html_comm));
#else
	gui_copy_html(widgets.html_comm);
#endif
}


static void on_find1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_find_dlg(widgets.html_comm, settings.CommWindowModule, 
				FALSE, NULL);
}


static void on_item2_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_set_module_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_set_module_font(settings.CommWindowModule);
	main_display_commentary(settings.CommWindowModule,settings.currentverse);
}

static void on_read_selection_aloud(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	gchar *dict_key;
	int len;

#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(widgets.html_comm));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	dict_key = 
		g_strdup(gtk_editable_get_chars(
			(GtkEditable *)widgets.entry_dict,0,-1));
	len = (dict_key ? strlen(dict_key) : 0);
#else
	GtkHTML *html = GTK_HTML(widgets.html_comm);
	dict_key = gtk_html_get_selection_html(html, &len);
#endif /* !USE_GTKMOZEMBED */
 
	if (dict_key && len && *dict_key) {
		ReadAloud(0, dict_key);
		g_free(dict_key);
	} else
		gui_generic_warning("No selection made");
}

static void
on_use_current_dictionary_activate(GtkMenuItem * menuitem,
				   gpointer user_data)
{
	gchar *dict_key =NULL;
#ifdef USE_GTKMOZEMBED	
	gecko_html_copy_selection(GECKO_HTML(widgets.html_comm));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);	
#else
	dict_key = gui_get_word_or_selection(widgets.html_comm, FALSE);
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), dict_key);
	gtk_widget_activate(widgets.entry_dict);
	g_free(dict_key);
#endif
}


static void
on_unlock_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *cipher_key;
	gchar *cipher_old;
	
/*	cipher_old = get_cipher_key((settings.comm_showing)?
			settings.CommWindowModule:settings.book_mod);*/
	cipher_key = gui_add_cipher_key(settings.CommWindowModule, cipher_old);
	if (cipher_key) 
		main_display_commentary(settings.CommWindowModule, settings.currentverse);
}

static void
on_rename_perscomm_activate(GtkMenuItem * menuitem, gpointer user_data)
{
#ifdef __CYGWIN__
	gui_generic_warning(_("Renaming is not available in Cygwin.\n\n"
			      "GnomeSword is limited by Windows' filesystem,\n"
			      "because it disallows the renaming of filename\n"
			      "components of currently-open files,\n"
			      "such as the contents of this commentary.\n"
			      "Therefore, personal commentary renaming is\n"
			      "not available in the Windows environment."));
#else
	GS_DIALOG *info;
	GString *workstr;
	char *s;
	char *datapath_old, *datapath_new;
	const char *conf_old;
	char *conf_new;
	char *sworddir, *modsdir;
	FILE *result;

	// get a new name for the module.
	info = gui_new_dialog();
	info->title = _("Rename Commentary");
	workstr = g_string_new("");
	g_string_printf(workstr, "<span weight=\"bold\">%s</span>",
			_("Choose Commentary Name"));
	info->label_top = workstr->str;
	info->text1 = g_strdup(_("New Name"));
	info->label1 = N_("Name: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	if (gui_gs_dialog(info) != GS_OK)
		goto out1;

	for (s = info->text1; *s; ++s) {
		if (!isalnum(*s)) {
			gui_generic_warning("Names must be letters+digits only.");
			goto out1;
		}
	}

	if (main_is_module(info->text1)) {
		gui_generic_warning("Duplicate name");
		goto out1;
	}

	sworddir = g_new(char, strlen(settings.homedir) + 8);
	sprintf(sworddir, "%s/.sword", settings.homedir);
	modsdir  = g_new(char, strlen(sworddir) + 8);
	sprintf(modsdir,  "%s/mods.d", sworddir);

	conf_old =
	    main_get_mod_config_file(settings.CommWindowModule, sworddir);

	conf_new = g_strdup(info->text1);	// dirname is lowercase.
	for (s = conf_new; *s; ++s)
		if (isupper(*s))
			*s = tolower(*s);

	datapath_old =
	    main_get_mod_config_entry(settings.CommWindowModule, "DataPath");
	datapath_new = g_strdup(datapath_old);
	if ((s = strstr(datapath_new, "rawfiles/")) == NULL) {
	    gui_generic_warning("Malformed datapath in old configuration!");
	    goto out2;
	}

	*(s+9) = '\0';		// skip past "rawfiles/".
	s = g_strdup_printf("%s%s", datapath_new, conf_new);
	g_free(datapath_new);	// out with the old...
	datapath_new = s;	// ..and in with the new.

	// move old data directory to new.
	if ((chdir(sworddir) != 0) ||
	    (rename(datapath_old, datapath_new) != 0)) {
		gui_generic_warning("Failed to rename directory.");
		goto out2;
	}

	// manufacture new .conf from old.
	g_string_printf(workstr,
			"( cd %s && sed -e '/^\\[/s|^.*$|[%s]|' -e '/^DataPath=/s|rawfiles/.*$|rawfiles/%s/|' < %s > %s.conf ) 2>&1",
			modsdir, info->text1, conf_new, conf_old, conf_new);
	if ((result = popen(workstr->str, "r")) == NULL) {
		g_string_printf(workstr,
				_("Failed to create new configuration:\n%s"),
				strerror(errno));
		gui_generic_warning(workstr->str);
		goto out2;
	} else {
		gchar output[258];
		if (fgets(output, 256, result) != NULL) {
			g_string_truncate(workstr, 0);
			g_string_append(workstr,
					_("Configuration build error:\n\n"));
			g_string_append(workstr, output);
			gui_generic_warning(workstr->str);
			goto out2;	// necessary?  advisable?
		}
		pclose(result);
	}

	// unlink old conf.
	g_string_printf(workstr, "%s/%s", modsdir, conf_old);
	if (unlink(workstr->str) != 0) {
		g_string_printf(workstr,
				"Unlink of old configuration failed:\n%s",
				strerror(errno));
		gui_generic_warning(workstr->str);
		goto out2;
	}
	main_update_module_lists();
	settings.CommWindowModule = g_strdup(info->text1);
	main_display_commentary(info->text1, settings.currentverse);

out2:
	g_free(conf_new);
	g_free((char*)conf_old);
	g_free(datapath_old);
	g_free(datapath_new);
	g_free(modsdir);
	g_free(sworddir);
out1:
	g_free(info->text1);
	g_free(info);
	g_string_free(workstr, TRUE);
#endif /* !__CYGWIN__ */
}


static void
on_dump_perscomm_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	main_sidebar_perscomm_dump();
}


static void on_show_tabs_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_all_readings_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_primary_reading_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_secondary_reading_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{

}


/******************************************************************************
 * Name
 *  gui_lookup_bibletext_selection
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 * void gui_lookup_bibletext_selection(GtkMenuItem * menuitem,
					 gchar * dict_mod_description)
 *
 * Description
 *   lookup selection in a dict/lex module
 *
 * Return value
 *   void
 */

void gui_lookup_comm_selection(GtkMenuItem * menuitem,
				    gchar * dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;
		
	mod_name = main_module_name_from_description(dict_mod_description);
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(widgets.html_comm));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	dict_key = 
		g_strdup(gtk_editable_get_chars(
			(GtkEditable *)widgets.entry_dict,0,-1));
#else	
	dict_key = gui_get_word_or_selection(widgets.html_comm, FALSE);
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), dict_key);
#endif
	if (dict_key && mod_name)
		main_display_dictionary(mod_name, dict_key);
	if (dict_key)
		g_free(dict_key);
	if (mod_name)
		g_free(mod_name);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   void (GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void edit_percomm(GtkMenuItem * menuitem, gpointer user_data)
{
#ifdef USE_GTKHTML38
	editor_create_new((gchar *)user_data,(gchar *)settings.currentverse,TRUE);
#else
//	gui_open_commentary_editor((gchar *) user_data);
	main_dialogs_open((gchar *)user_data, (gchar *)settings.currentverse);
#endif
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   void on_view_mod_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   show a different text module by changing the notebook page
 *
 * Return value
 *   void
 */

static void on_view_mod_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{

	gchar *module_name = NULL;

	module_name = main_module_name_from_description((gchar *) user_data);
	if(module_name) {
		main_display_commentary(module_name, settings.currentverse);
		//gui_change_module_and_key(module_name, settings.currentverse);
		g_free(module_name);
	}
}


/******************************************************************************
 * Name
 *  on_book_heading_activate
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
 *
 *  void on_book_heading_activate(GtkMenuItem * menuitem, DIALOG_DATA * vt)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_book_heading_activate(GtkMenuItem * menuitem, gpointer data)
{
	main_book_heading(settings.CommWindowModule);
}


/******************************************************************************
 * Name
 *  on_chapter_heading_activate
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
 *
 *  void on_chapter_heading_activate(GtkMenuItem * menuitem, DIALOG_DATA * vt)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_chapter_heading_activate(GtkMenuItem * menuitem, gpointer data)
{
	main_chapter_heading(settings.CommWindowModule);
}


static void on_add_bookmark_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
	
	gchar *label = g_strdup_printf("%s, %s",settings.currentverse,
					settings.CommWindowModule);

	gui_bookmark_dialog(label,
			settings.CommWindowModule, settings.currentverse);
	g_free(label);	
	
}

static GnomeUIInfo view_text_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("item1"),
	 NULL,
	 (gpointer) on_item1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-book-green",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo file3_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Open Module"),
	 NULL,
	 view_text_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_PRINT_ITEM(on_print1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo note_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("item2"),
	 NULL,
	 (gpointer) on_item2_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo edit3_menu_uiinfo[] = {
	GNOMEUIINFO_MENU_COPY_ITEM(on_copy2_activate, NULL),
	GNOMEUIINFO_MENU_FIND_ITEM(on_find1_activate, NULL),
	{
	 GNOME_APP_UI_SUBTREE, N_("Note"),
	 NULL,
	 note_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-dnd",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo all_readings_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("All Readings"),
	 NULL,
	 (gpointer) on_all_readings_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Primary Reading"),
	 NULL,
	 (gpointer) on_primary_reading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Secondary Reading"),
	 NULL,
	 (gpointer) on_secondary_reading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo variants_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_RADIOITEMS, NULL, NULL, all_readings_uiinfo,
	 NULL, NULL, GNOME_APP_PIXMAP_NONE, NULL, 0,
	 (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo module_options_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Set Module Font"),
	 NULL,
	 (gpointer) on_set_module_font_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-select-font",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Words of Christ in Red"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Words of Christ in Red",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM,
	 N_("Strong's Numbers"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Strong's Numbers",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE,
	 NULL,
	 0,
	 (GdkModifierType) 0,
	 NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Morphological Tags"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Morphological Tags",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Footnotes"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Footnotes",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Greek Accents"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Greek Accents",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Lemmas"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Lemmas",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Scripture Cross-references"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Scripture Cross-references",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Vowel Points"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Hebrew Vowel Points",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Cantillation"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Hebrew Cantillation",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Headings"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Headings",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Variants"),
	 NULL,
	 variants_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo lookup_selection_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Use Current Dictionary"),
	 NULL,
	 (gpointer) on_use_current_dictionary_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-find",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo menu1_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("About"),
	 NULL,
	 (gpointer) on_about_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-about",
	 0, (GdkModifierType) 0, NULL},
	//GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Bookmark"),
	 NULL,
	 (gpointer) on_add_bookmark_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-add",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_SUBTREE, N_("File"),
	 NULL,
	 file3_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-open",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Edit"),
	 NULL,
	 edit3_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-dnd",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("_Module Options"),
	 NULL,
	 module_options_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-ok",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Lookup Selection"),
	 NULL,
	 lookup_selection_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-find",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Display Book Heading"),
	 NULL,
	 (gpointer) on_book_heading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-open",
	 0, (GdkModifierType) 0, NULL},
        {
	 GNOME_APP_UI_ITEM, N_("Display Chapter Heading"),
	 NULL,
	 (gpointer) on_chapter_heading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-open",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Unlock This Module"),		// 9
	 NULL,
	 (gpointer) on_unlock_module_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-authentication",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Rename Pers.Comm."),		// 10
	 NULL,
	 (gpointer) on_rename_perscomm_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-convert",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Dump Pers.Comm."),		// 11
	 NULL,
	 (gpointer) on_dump_perscomm_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-paste",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Read Selection Aloud"),
	 NULL,
	 (gpointer) on_read_selection_aloud, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-mic",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

void gui_create_pm_commentary(void)
{
	GtkWidget *menu1;
	GtkWidget *lookup_selection_menu;
	GtkWidget *usecurrent;
	GtkWidget *view_menu;
	GtkWidget *separator;
	GtkWidget *edit_per_menu;
	GnomeUIInfo *menuitem;
	gchar *mod_name = 
	    (settings.comm_showing)?settings.CommWindowModule:settings.book_mod;
	GLOBAL_OPS *ops = main_new_globals(mod_name);
	menu1 = gtk_menu_new();
	gnome_app_fill_menu(GTK_MENU_SHELL(menu1), menu1_uiinfo,
			    NULL, FALSE, 0);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (all_readings_uiinfo[0].widget),
				       TRUE);
	gtk_widget_hide(module_options_menu_uiinfo[2].widget);	//"words_in_red"         
	gtk_widget_hide(module_options_menu_uiinfo[3].widget);	//"strongs_numbers"      
	gtk_widget_hide(module_options_menu_uiinfo[4].widget);	//"/morph_tags"  
	gtk_widget_hide(module_options_menu_uiinfo[5].widget);	//"footnotes"    
	gtk_widget_hide(module_options_menu_uiinfo[6].widget);	// "greek_accents"       
	gtk_widget_hide(module_options_menu_uiinfo[7].widget);	//"lemmas"       
	gtk_widget_hide(module_options_menu_uiinfo[8].widget);	//"cross_references"    
	gtk_widget_hide(module_options_menu_uiinfo[9].widget);	//"hebrew_vowel_points" 
	gtk_widget_hide(module_options_menu_uiinfo[10].widget);	//"hebrew_cantillation"        
	gtk_widget_hide(module_options_menu_uiinfo[11].widget);	//"headings"    
	gtk_widget_hide(module_options_menu_uiinfo[12].widget);	//"variants"   
	//gtk_widget_hide(menu1_uiinfo[7].widget);	//"unlock_module"
	//gtk_widget_hide(menu1_uiinfo[8].widget);
	gtk_widget_hide(menu1_uiinfo[9].widget);	// unlock
	gtk_widget_hide(menu1_uiinfo[10].widget);	// rename pers.comm
	gtk_widget_hide(menu1_uiinfo[11].widget);	// dump pers.comm

	
	view_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file3_menu_uiinfo[0].widget),
				  view_menu);
	
	gui_add_mods_2_gtk_menu(COMM_DESC_LIST, view_menu,
				(GCallback) on_view_mod_activate);
	
	edit_per_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit3_menu_uiinfo[2].widget),
				  edit_per_menu);
				  
	gui_add_mods_2_gtk_menu(PERCOMM_LIST, edit_per_menu,
				(GCallback) edit_percomm);
	
				
								
	
	lookup_selection_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu1_uiinfo[6].widget),
				  lookup_selection_menu);
	
	usecurrent =
	    gtk_menu_item_new_with_label(_("Use Current Dictionary"));
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);

	g_signal_connect(GTK_OBJECT(usecurrent),
			   "activate",
			   G_CALLBACK(on_use_current_dictionary_activate), 
			   NULL);
			   
	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  separator);
	gtk_widget_set_sensitive(separator, FALSE);
	
	gui_add_mods_2_gtk_menu(DICT_DESC_LIST, lookup_selection_menu,
				(GCallback)gui_lookup_comm_selection);
			
				
	if ((main_check_for_global_option(mod_name,
				     "GBFRedLetterWords")) ||
	    (main_check_for_global_option(mod_name,
				     "OSISRedLetterWords"))) {
		gtk_widget_show(module_options_menu_uiinfo[2].widget);	//"words_in_red");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[2].
				    widget)->active = ops->words_in_red;
	}
	if ((main_check_for_global_option
	     (mod_name, "GBFStrongs"))
	    ||
	    (main_check_for_global_option
	     (mod_name, "ThMLStrongs"))
	    ||
	    (main_check_for_global_option
	     (mod_name, "OSISStrongs"))) {
		gtk_widget_show(module_options_menu_uiinfo[3].widget);	//"strongs_numbers");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[3].
				    widget)->active = ops->strongs;
	}
	if (main_check_for_global_option(mod_name, "GBFMorph") ||
	    main_check_for_global_option(mod_name, "ThMLMorph") ||
	    main_check_for_global_option(mod_name, "OSISMorph")) {
		gtk_widget_show(module_options_menu_uiinfo[4].widget);	//"/morph_tags");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[4].
				    widget)->active = ops->morphs;
	}
	if (main_check_for_global_option(mod_name, "GBFFootnotes") ||
	    main_check_for_global_option(mod_name, "ThMLFootnotes") ||
	    main_check_for_global_option(mod_name, "OSISFootnotes")) {
		gtk_widget_show(module_options_menu_uiinfo[5].widget);	//"footnotes");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[5].
				    widget)->active = ops->footnotes;
	}
	if (main_check_for_global_option(mod_name, "UTF8GreekAccents")) {
		gtk_widget_show(module_options_menu_uiinfo[6].widget);	// "greek_accents");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[6].
				    widget)->active = ops->greekaccents;
	}
	if (main_check_for_global_option(mod_name, "ThMLLemma") ||
	    main_check_for_global_option(mod_name, "OSISLemma")) {
		gtk_widget_show(module_options_menu_uiinfo[7].widget);	//"lemmas");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[7].
				    widget)->active = ops->lemmas;
	}
	if (main_check_for_global_option(mod_name, "ThMLScripref") ||
	    main_check_for_global_option(mod_name, "OSISScripref")) {
		gtk_widget_show(module_options_menu_uiinfo[8].widget);	//"cross_references");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[8].
				    widget)->active = ops->scripturerefs;
	}
	if (main_check_for_global_option(mod_name, "UTF8HebrewPoints")) {
		gtk_widget_show(module_options_menu_uiinfo[9].widget);	//"hebrew_vowel_points");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[9].
				    widget)->active = ops->hebrewpoints;
	}
	if (main_check_for_global_option(mod_name, "UTF8Cantillation")) {
		gtk_widget_show(module_options_menu_uiinfo[10].widget);	//"hebrew_cantillation");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[10].
				    widget)->active = ops->hebrewcant;
	}
	if (main_check_for_global_option(mod_name, "ThMLHeadings") ||
	    main_check_for_global_option(mod_name, "OSISHeadings")) {
		gtk_widget_show(module_options_menu_uiinfo[11].widget);	//"headings");
		 GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[11].
				    widget)->active = ops->headings;   
	}
	if (main_check_for_global_option(mod_name, "ThMLVariants")) {
		gtk_widget_show(module_options_menu_uiinfo[12].widget);	//"variants");

		gtk_widget_show(all_readings_uiinfo[0].widget);	//"all_readings");

		gtk_widget_show(all_readings_uiinfo[1].widget);	//"primary_reading");

		gtk_widget_show(all_readings_uiinfo[2].widget);	//"secondary_reading");
		
	}
	if (main_has_cipher_tag(mod_name))
		gtk_widget_show(menu1_uiinfo[9].widget);	// unlock

	if (main_get_mod_type(mod_name) == PERCOM_TYPE) {
		gtk_widget_show(menu1_uiinfo[10].widget);	// rename
		gtk_widget_show(menu1_uiinfo[11].widget);	// dump
	}
	
	/* 
	 * menu1_uiinfo[0].widget, "about");
	 * menu1_uiinfo[1].widget, "separator4");
	 * menu1_uiinfo[2].widget, "file3");
	 * file3_menu_uiinfo[0].widget, "view_text");
	 * view_text_menu_uiinfo[0].widget, "item1");
	 * file3_menu_uiinfo[1].widget, "separator8");
	 * file3_menu_uiinfo[2].widget, "print1");
	 * menu1_uiinfo[3].widget, "edit3");
	 * edit3_menu_uiinfo[0].widget, "copy2");
	 * edit3_menu_uiinfo[1].widget, "find1");
	 * edit3_menu_uiinfo[2].widget, "note");
	 * note_menu_uiinfo[0].widget, "item2");
	 * menu1_uiinfo[4].widget, "module_options");
	 * module_options_menu_uiinfo[0].widget, "set_module_font");
	 * module_options_menu_uiinfo[1].widget, "separator5");
	 * menu1_uiinfo[5].widget, "lookup_selection");
	 * lookup_selection_menu_uiinfo[0].widget, "use_current_dictionary");
	 * lookup_selection_menu_uiinfo[1].widget, "separator6");
	 * menu1_uiinfo[7].widget, "separator7");
	 * menu1_uiinfo[8].widget, "show_tabs");
	 */
	gtk_menu_popup((GtkMenu*)menu1, NULL, NULL, NULL, NULL, 2,
		     			gtk_get_current_event_time());
	/*gnome_popup_menu_do_popup_modal(menu1, NULL,
					NULL, event, NULL,
					widgets.html_text);*/
	//gtk_widget_destroy(menu1);
	g_free(ops);
}



//******  end of file  ******/
