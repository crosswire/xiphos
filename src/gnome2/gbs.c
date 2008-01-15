/*
 * GnomeSword Bible Study Tool
 * gbs.c - generic book support - the gui
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

#include <gnome.h>

#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_treeview.h"
#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/find_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/bookmarks.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"
#include "gui/navbar_book.h"

#include "main/settings.h"
#include "main/lists.h"
#include "main/global_ops.hh"
#include "main/sword.h"
#include "main/xml.h"
#include "main/display.hh"

static void create_menu(GdkEventButton * event);


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
static gboolean on_book_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	if (!settings.havebook)
		return FALSE;	
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		gui_popup_menu_gbs();
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
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

static gboolean on_book_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	extern gboolean in_url;
	gchar *key;
	const gchar *url;
	gchar *buf = NULL;
	
	if (!settings.havebook)
		return FALSE;
	settings.whichwindow = BOOK_WINDOW;
	
#ifdef GTKHTML
	switch (event->button) {
	case 1:
		if (in_url) 
			break;
		key = gui_button_press_lookup(widgets.html_book);
		if (key) {
			if (g_strstr_len(key,strlen(key),"*")) {
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
				dict = g_strdup(settings.DictWindowModule);
			
			main_display_dictionary(dict, key);
			g_free(key);
			if (dict)
				g_free(dict);
		}
		break;
	case 2:
		if (!in_url)
			break;
		url = gtk_html_get_url_at (GTK_HTML(widgets.html_text),		
								event->x,
								event->y);
		if (url && (strstr(url,"sword://"))) {
			gchar **work_buf = g_strsplit (url,"/",4);
			gui_open_passage_in_new_tab(work_buf[3]);
			g_strfreev(work_buf);
		}
		break;
	case 3:
		break;
	}
#endif
	return FALSE;
}



/******************************************************************************
 * Name
 *    
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_popup_menu_gbs(void)
{
	create_menu(NULL);
}


/******************************************************************************
 * Name
 *  gui_set_gbs_frame_label
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_set_gbs_frame_label(void)	
 *
 * Description
 *   sets gbs label to module name
 *
 * Return value
 *   void
 */

static void set_gbs_label(gchar * mod_name)
{
	/*
	 * set label to module name
	 */
	gtk_label_set_text (GTK_LABEL(widgets.label_comm),mod_name);
}

/******************************************************************************
 * Name
 *  gui_set_book_page_and_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void gui_set_book_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_set_book_mod_and_key(gchar * mod_name, gchar * key)
{
	main_display_book(mod_name, key);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include ".h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_gbs_global_ops(gchar * option, gboolean choice)
{
	/*save_module_options(cur_t->mod_name, option, 
				    choice);*/
	//gbs_display(cur_g, tree_level);
}



static gboolean on_enter_notify_event(GtkWidget * widget,
				      GdkEventCrossing * event,
				      gpointer user_data)
{
	//shift_key_presed = FALSE;
	//gtk_widget_grab_focus (widgets.html_book);
	//settings.whichwindow = BOOK_WINDOW;
  	return FALSE;
}

#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (GeckoHtml *html,
			     gchar *uri,
			     gpointer user_data)
{	
	gui_popup_menu_gbs(); 
}
#endif

/******************************************************************************
 * Name
 *   gui_create_book_pane
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   GtkWidget *gui_create_book_pane(void)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_book_pane(void)
{
	GtkWidget *box;
	GtkWidget *scrolledwindow;
	GtkWidget *eventbox;	
	GtkWidget *navbar;
	
	box = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box);

	navbar = gui_navbar_book_new();
	gtk_box_pack_start(GTK_BOX(box), navbar, FALSE, FALSE, 0);

#ifdef USE_GTKMOZEMBED	
	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);
	gtk_box_pack_start(GTK_BOX(box), eventbox, TRUE, TRUE, 0);
	widgets.html_book = GTK_WIDGET(gecko_html_new(NULL, FALSE, BOOK_TYPE)); //embed_new(BOOK_TYPE);
	gtk_widget_show(widgets.html_book);
	gtk_container_add(GTK_CONTAINER(eventbox),
			 widgets.html_book);
	
	g_signal_connect((gpointer)widgets.html_book,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      NULL);
	g_signal_connect ((gpointer) eventbox, "enter_notify_event",
		    G_CALLBACK (on_enter_notify_event),
		    NULL);

#else
	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(box), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
/*	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow,
                                             settings.shadow_type);*/

	widgets.html_book = gtk_html_new();
	gtk_widget_show(widgets.html_book);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  widgets.html_book);

	g_signal_connect(GTK_OBJECT(widgets.html_book), "link_clicked",
				   G_CALLBACK(gui_link_clicked),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_book), "on_url",
				   G_CALLBACK(gui_url),
				   GINT_TO_POINTER(BOOK_TYPE));
	g_signal_connect(GTK_OBJECT(widgets.html_book),
				   "button_press_event",
				   G_CALLBACK
				   (on_book_button_press_event),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_book),
				   "button_release_event",
				   G_CALLBACK
				   (on_book_button_release_event),
				   NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_book),
			   "url_requested",
			   G_CALLBACK(url_requested), NULL);
#endif
	return box;
}



/**  Main Book menu stuff
 **
 **
 **
 **
 **
 **
 **/

/******************************************************************************
 * Name
 *  on_global_option
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_global_option(GtkMenuItem * menuitem, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_global_option(GtkMenuItem * menuitem, gpointer data)
{
	main_save_module_options(settings.book_mod, (gchar *) data,
				 GTK_CHECK_MENU_ITEM(menuitem)->active);
	main_display_book(settings.book_mod, settings.book_key);
	//gui_set_book_mod_and_key(settings.book_mod, settings.book_key);
}



static void on_about_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_display_about_module_dialog(settings.book_mod, FALSE);
}


static void on_item1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void on_print1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
#ifdef USE_GTKMOZEMBED
	gecko_html_print_document (GTK_WINDOW(widgets.app), 
				   settings.book_mod, 
				   GECKO_HTML(widgets.html_book));
#else
	gui_html_print(widgets.html_book, FALSE);
#endif
}


static void on_copy2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(widgets.html_book));
#else
	gui_copy_html(widgets.html_book);
#endif
}


static void on_find1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_find_dlg(widgets.html_book, settings.book_mod, FALSE, NULL);
}


static void on_item2_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_set_module_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_set_module_font(settings.book_mod);
	main_display_book(settings.book_mod,xml_get_value( "keys", "offset"));
}

static void on_read_selection_aloud(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	gchar *dict_key;
	int len;

#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(widgets.html_book));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	dict_key = 
		g_strdup(gtk_editable_get_chars(
			(GtkEditable *)widgets.entry_dict,0,-1));
	len = (dict_key ? strlen(dict_key) : 0);
#else
	GtkHTML *html = GTK_HTML(widgets.html_book);
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
	GS_message(("book on_use_current_dictionary_activate"));
	gchar *dict_key =NULL;
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(widgets.html_book));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);	
#else
	dict_key = gui_get_word_or_selection(widgets.html_book, FALSE);
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
	
//	cipher_old = get_cipher_key(settings.book_mod);
	cipher_key = gui_add_cipher_key(settings.book_mod, cipher_old);
	if (cipher_key) 
		main_display_book(settings.book_mod, "o");
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

void gui_lookup_gbs_selection(GtkMenuItem * menuitem,
				    gchar * dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;
		
	mod_name = main_module_name_from_description(dict_mod_description);
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(widgets.html_book));
	//embed_copy_selection(GTK_MOZ_EMBED(widgets.html_book));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	dict_key = 
		g_strdup(gtk_editable_get_chars(
			(GtkEditable *)widgets.entry_dict,0,-1));
#else	
	dict_key = gui_get_word_or_selection(widgets.html_book, FALSE);
	gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), dict_key);
#endif
	if (dict_key && mod_name) {
		main_display_dictionary(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}
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
	//gui_open_commentary_editor((gchar *) user_data);
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
	if (module_name) {
		main_display_book(module_name, "0");
		g_free(module_name);
	}
}


static void on_add_bookmark_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
	gchar *key = g_strdup_printf("%d",settings.book_offset);
	gchar *local_name = main_get_treekey_local_name(settings.book_offset);
	gchar *label = g_strdup_printf("%s, %s",local_name,settings.book_mod);

	gui_bookmark_dialog(label, settings.book_mod, key);
	g_free(label);	
	g_free(local_name);		
	g_free(key);	
	
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
	{ /* 0 */
	 GNOME_APP_UI_ITEM, N_("Set Module Font"),
	 NULL,
	 (gpointer) on_set_module_font_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-select-font",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR, /* 1 */
	{ /* 2 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Words of Christ in Red"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Words of Christ in Red",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 3 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Strong's Numbers"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Strong's Numbers",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 4 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Morphological Tags"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Morphological Tags",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 5 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Footnotes"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Footnotes",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 6 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Greek Accents"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Greek Accents",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 7 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Lemmas"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Lemmas",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 8 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Scripture Cross-references"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Scripture Cross-references",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 9 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Vowel Points"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Hebrew Vowel Points",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 10 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Cantillation"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Hebrew Cantillation",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 11 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Headings"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Headings",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 12 */
	 GNOME_APP_UI_SUBTREE, N_("Variants"),
	 NULL,
	 variants_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{ /* 13 */
	 GNOME_APP_UI_TOGGLEITEM, N_("Image Content"),
	 NULL,
	 (gpointer) on_global_option,
	 (gpointer) "Image Content",	/* not seen by user */
	 NULL,
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
	 GNOME_APP_UI_TOGGLEITEM, N_("Show Tabs"),
	 NULL,
	 (gpointer) on_show_tabs_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Unlock This Module"),
	 NULL,
	 (gpointer) on_unlock_module_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-authentication",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Read Selection Aloud"),
	 NULL,
	 (gpointer) on_read_selection_aloud, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-mic",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static void create_menu(GdkEventButton * event)
{
	GtkWidget *menu1;
	GtkWidget *lookup_selection_menu;
	GtkWidget *usecurrent;
	GtkWidget *view_menu;
	GtkWidget *separator;
	GtkWidget *edit_per_menu;
	GnomeUIInfo *menuitem;
	gchar *mod_name = settings.book_mod;
	GLOBAL_OPS *ops = main_new_globals(mod_name);
	menu1 = gtk_menu_new();
	gnome_app_fill_menu(GTK_MENU_SHELL(menu1), menu1_uiinfo,
			    NULL, FALSE, 0);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (all_readings_uiinfo[0].widget),
				       TRUE);
	gtk_widget_hide(module_options_menu_uiinfo[2].widget);	// words_in_red
	gtk_widget_hide(module_options_menu_uiinfo[3].widget);	// strongs_numbers
	gtk_widget_hide(module_options_menu_uiinfo[4].widget);	// morph_tags
	gtk_widget_hide(module_options_menu_uiinfo[5].widget);	// footnotes
	gtk_widget_hide(module_options_menu_uiinfo[6].widget);	// greek_accents
	gtk_widget_hide(module_options_menu_uiinfo[7].widget);	// lemmas
	gtk_widget_hide(module_options_menu_uiinfo[8].widget);	// cross_references
	gtk_widget_hide(module_options_menu_uiinfo[9].widget);	// hebrew_vowel_points
	gtk_widget_hide(module_options_menu_uiinfo[10].widget);	// hebrew_cantillation
	gtk_widget_hide(module_options_menu_uiinfo[11].widget);	// headings
	gtk_widget_hide(module_options_menu_uiinfo[12].widget);	// variants
	gtk_widget_hide(module_options_menu_uiinfo[13].widget);	// image content
	gtk_widget_hide(menu1_uiinfo[7].widget);	// unlock_module
	gtk_widget_hide(menu1_uiinfo[8].widget);
	gtk_widget_hide(menu1_uiinfo[9].widget);
	//gtk_widget_hide(file3_menu_uiinfo[0].widget);
	//gtk_widget_hide(file3_menu_uiinfo[1].widget);
	//gtk_widget_hide(edit3_menu_uiinfo[2].widget);
				
	
	
	view_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file3_menu_uiinfo[0].widget),
				  view_menu);
	
	gui_add_mods_2_gtk_menu(GBS_DESC_LIST, view_menu,
				(GCallback) on_view_mod_activate);
								
	
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
			   G_CALLBACK(on_use_current_dictionary_activate), NULL);

	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  separator);
	gtk_widget_set_sensitive(separator, FALSE);
	
	gui_add_mods_2_gtk_menu(DICT_DESC_LIST, lookup_selection_menu,
				(GCallback)gui_lookup_gbs_selection);
			
				
	if ((main_check_for_global_option(mod_name, "GBFRedLetterWords")) ||
	    (main_check_for_global_option(mod_name, "OSISRedLetterWords"))) {
		gtk_widget_show(module_options_menu_uiinfo[2].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[2].
				    widget)->active = ops->words_in_red;
	}
	if ((main_check_for_global_option(mod_name, "GBFStrongs")) ||
	    (main_check_for_global_option(mod_name, "ThMLStrongs")) ||
	    (main_check_for_global_option
	     (mod_name, "OSISStrongs"))) {
		gtk_widget_show(module_options_menu_uiinfo[3].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[3].
				    widget)->active = ops->strongs;
	}
	if (main_check_for_global_option(mod_name, "GBFMorph") ||
	    main_check_for_global_option(mod_name, "ThMLMorph") ||
	    main_check_for_global_option(mod_name, "OSISMorph")) {
		gtk_widget_show(module_options_menu_uiinfo[4].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[4].
				    widget)->active = ops->morphs;
	}
	if (main_check_for_global_option(mod_name, "GBFFootnotes") ||
	    main_check_for_global_option(mod_name, "ThMLFootnotes") ||
	    main_check_for_global_option(mod_name, "OSISFootnotes")) {
		gtk_widget_show(module_options_menu_uiinfo[5].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[5].
				    widget)->active = ops->footnotes;
	}
	if (main_check_for_global_option(mod_name, "UTF8GreekAccents")) {
		gtk_widget_show(module_options_menu_uiinfo[6].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[6].
				    widget)->active = ops->greekaccents;
	}
	if (main_check_for_global_option(mod_name, "ThMLLemma") ||
	    main_check_for_global_option(mod_name, "OSISLemma")) {
		gtk_widget_show(module_options_menu_uiinfo[7].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[7].
				    widget)->active = ops->lemmas;
	}
	if (main_check_for_global_option(mod_name, "ThMLScripref") ||
	    main_check_for_global_option(mod_name, "OSISScripref")) {
		gtk_widget_show(module_options_menu_uiinfo[8].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[8].
				    widget)->active = ops->scripturerefs;
	}
	if (main_check_for_global_option(mod_name, "UTF8HebrewPoints")) {
		gtk_widget_show(module_options_menu_uiinfo[9].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[9].
				    widget)->active = ops->hebrewpoints;
	}
	if (main_check_for_global_option(mod_name, "UTF8Cantillation")) {
		gtk_widget_show(module_options_menu_uiinfo[10].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[10].
				    widget)->active = ops->hebrewcant;
	}
	if (main_check_for_global_option(mod_name, "ThMLHeadings") ||
	    main_check_for_global_option(mod_name, "OSISHeadings")) {
		gtk_widget_show(module_options_menu_uiinfo[11].widget);
		 GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[11].
				    widget)->active = ops->headings;   
	}
	if (main_check_for_global_option(mod_name, "ThMLVariants")) {
		gtk_widget_show(module_options_menu_uiinfo[12].widget);
		gtk_widget_show(all_readings_uiinfo[0].widget);	// all_readings
		gtk_widget_show(all_readings_uiinfo[1].widget);	// primary_reading
		gtk_widget_show(all_readings_uiinfo[2].widget);	// secondary_reading
	}
	if (ops->image_content != -1) {
		gtk_widget_show(module_options_menu_uiinfo[13].widget);
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[13].
				    widget)->active = ops->image_content;
	}
	if (main_has_cipher_tag(mod_name))
		gtk_widget_show(menu1_uiinfo[7].widget);
	
	
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
	 * lookup_selection_menu_
	mod_type = backend->module_type((gchar*)module);uiinfo[1].widget, "separator6");
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
