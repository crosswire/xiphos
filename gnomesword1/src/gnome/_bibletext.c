/*
 * GnomeSword Bible Study Tool
 * _bibletext.c - gui for Bible text modules
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

/* frontend */
#include "_bibletext.h"

/* main */
#include "cipher_key_dialog.h"
#include "bibletext.h"

#include "gs_shortcutbar.h"
#include "gs_html.h"
#include "gs_viewtext_dlg.h"
#include "settings.h"

/******************************************************************************
 * externs
 */

extern gboolean isrunningVT;
extern GList *options;
extern TEXT_DATA *cur_t;
extern gboolean display_change;

/******************************************************************************
 * global to this file only 
 */


/******************************************************************************
 * Name
 *  gui_set_text_frame_label
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void gui_set_text_frame_label(void)	
 *
 * Description
 *   sets text frame label to module name or null
 *
 * Return value
 *   void
 */

void gui_set_text_frame_label(void)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */	
	if (settings.text_tabs)
		gtk_frame_set_label(GTK_FRAME(cur_t->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(cur_t->frame), cur_t->mod_name);
	
}

/******************************************************************************
 * Name
 *  on_notebook_text_switch_page
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_notebook_text_switch_page(GtkNotebook * notebook,
 *				  GtkNotebookPage * page,
 *				  gint page_num, GList * tl)	
 *
 * Description
 *   sets gui to new sword module
 *
 * Return value
 *   void
 */

void on_notebook_text_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, GList * tl)
{
	TEXT_DATA *t;
	gchar title[200];
	/*
	 * get data structure for new module 
	 */
	t = (TEXT_DATA *) g_list_nth_data(tl, page_num);
	/*
	 * do work that's non gui
	 */
	text_page_changed(page_num, t);
	/*
	 * set program title to GnomeSWORD + current text module name 
	 */
	sprintf(title, "GnomeSWORD - %s", t->mod_description);
	gtk_window_set_title(GTK_WINDOW(settings.app), title);
	/*
	 *  keep showtabs menu item current 
	 */
	GTK_CHECK_MENU_ITEM(t->showtabs)->active = settings.text_tabs;
	
	gui_set_text_frame_label();	
}

/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, TEXT_DATA * t)	
 *
 * Description
 *   copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	copyGS_HTML(t->html);
}

/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "gs_bibletext.h"
 *
 *  void on_find_activate(GtkMenuItem * menuitem, TEXT_DATA * t)	
 *
 * Description
 *   opens find dialog
 *
 * Return value
 *   
 */

static void on_find_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	//searchGS_FIND_DLG(c, FALSE, NULL);
}

/******************************************************************************
 * Name
 *  on_lookup_selection_activate
 *
 * Synopsis
 *   #include "gs_bibletext.h"
 *   void on_lookup_selection_activate(GtkMenuItem * menuitem,
 *				  gchar * modDescription)   	
 *
 * Description
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */

static void on_lookup_selection_activate(GtkMenuItem * menuitem,
				  gchar * dict_mod_description)
{
	gchar *dict_key, *dict_mod;

	dict_mod =
	    get_module_name_from_description(dict_mod_description);
	dict_key = get_word_or_selection(cur_t->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			display_dictlex_in_viewer(dict_mod, dict_key,
						  &settings);
		if (settings.inDictpane)
			change_module_and_key(dict_mod, dict_key);
		g_free(dict_key);
		g_free(dict_mod);
	}
}

/******************************************************************************
 * Name
 *  on_same_lookup_selection_activate				       
 *
 * Synopsis
 *  #include "_bibletext.h"
 *
 *  void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
 *                                         TEXT_DATA * t) 	
 *
 * Description
 *   lookup seledtion in current dict/lex module
 *
 * Return value
 *   void
 */

static void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
				       TEXT_DATA * t)
{
	gchar *dict_key = get_word_or_selection(t->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			display_dictlex_in_viewer(settings.DictWindowModule,
						  dict_key, &settings);
		if (settings.inDictpane)
			change_module_and_key(settings.DictWindowModule,
					      dict_key);
		g_free(dict_key);
	}
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
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
	gint page;

	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_text),
			      page);
}

/******************************************************************************
 * Name
 *  on_text_showtabs_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void on_text_showtabs_activate(GtkMenuItem * menuitem,SETTINGS * s)	
 *
 * Description
 *   display text module notebook tabs
 *
 * Return value
 *   void
 */

static void on_text_showtabs_activate(GtkMenuItem * menuitem,
				      SETTINGS * s)
{
	s->text_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_text),
				   s->text_tabs); 
	gui_set_text_frame_label();
}

/******************************************************************************
 * Name
 *  on_view_new_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void on_view_new_activate(GtkMenuItem * menuitem, SETTINGS * s)	
 *
 * Description
 *   opens view text dialog
 *
 * Return value
 *   void
 */

static void on_view_new_activate(GtkMenuItem * menuitem, SETTINGS * s)
{
	static GtkWidget *dlg;
	GdkCursor *cursor;

	gtk_widget_show(s->app);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(s->app->window, cursor);

	if (!isrunningVT) {
		//dlg = create_dlgViewText(s);
		isrunningVT = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(s->app);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(s->app->window, cursor);
}

/******************************************************************************
 * Name
 *  on_unlock_key_activate
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void on_unlock_key_activate(GtkMenuItem * menuitem,TEXT_DATA * t) 	
 *
 * Description
 *   opens add cipher key dialog
 *
 * Return value
 *   void
 */

static void on_unlock_key_activate(GtkMenuItem * menuitem,
				   TEXT_DATA * t)
{
	GtkWidget *dlg;
	dlg = gui_create_cipher_key_dialog(t->mod_name);
	gnome_dialog_set_default(GNOME_DIALOG(dlg), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(dlg));
}

/******************************************************************************
 * Name
 *  gui_create_pm_text
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *   GtkWidget *gui_create_pm_text(TEXT_DATA * t)
 *
 * Description
 *   create popup menu for text module - attached to html widget
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *gui_create_pm_text(TEXT_DATA * t)
{
	GtkWidget *pm_text;
	GtkAccelGroup *pm_text_accels;
	GtkWidget *copy;
	GtkWidget *separator;
	GtkWidget *lookup_selection;
	GtkWidget *lookup_selection_menu;
	GtkAccelGroup *lookup_selection_menu_accels;
	GtkWidget *usecurrent;
	GtkWidget *item3;
	GtkWidget *item4;
	GtkWidget *view_new;
	GtkWidget *view_text;
	GtkWidget *view_text_menu;
	GtkAccelGroup *view_text_menu_accels;
	GtkWidget *find;
	GList *tmp;
	gint i;

	tmp = NULL;

	pm_text = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pm_text), "pm_text", pm_text);
	pm_text_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pm_text));

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_ref(copy);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "copy", copy,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(pm_text), copy);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_ref(find);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "find", find,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(pm_text), find);

	t->showtabs =
	    gtk_check_menu_item_new_with_label(_("Show Tabs"));
	gtk_widget_ref(t->showtabs);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "t->showtabs",
				 t->showtabs, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(t->showtabs);
	gtk_container_add(GTK_CONTAINER(pm_text), t->showtabs);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm_text), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	view_new = gtk_menu_item_new_with_label("View in new window");
	gtk_widget_ref(view_new);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "view_new",
				 view_new, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(view_new);
	gtk_container_add(GTK_CONTAINER(pm_text), view_new);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm_text), separator);
	gtk_widget_set_sensitive(separator, FALSE);


	lookup_selection =
	    gtk_menu_item_new_with_label(_("Lookup Selection"));
	gtk_widget_ref(lookup_selection);
	gtk_object_set_data_full(GTK_OBJECT(pm_text),
				 "lookup_selection", lookup_selection,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(lookup_selection);
	gtk_container_add(GTK_CONTAINER(pm_text), lookup_selection);

	lookup_selection_menu = gtk_menu_new();
	gtk_widget_ref(lookup_selection_menu);
	gtk_object_set_data_full(GTK_OBJECT(pm_text),
				 "lookup_selection_menu",
				 lookup_selection_menu,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(lookup_selection),
				  lookup_selection_menu);
	lookup_selection_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (lookup_selection_menu));

	usecurrent =
	    gtk_menu_item_new_with_label("Use Current Dictionary");
	gtk_widget_ref(usecurrent);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "usecurrent",
				 usecurrent, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);


	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm_text), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm_text), separator);
	gtk_widget_set_sensitive(separator, FALSE);


	view_text = gtk_menu_item_new_with_label(_("View Text"));
	gtk_widget_ref(view_text);
	gtk_object_set_data_full(GTK_OBJECT(pm_text),
				 "view_text", view_text,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(view_text);
	gtk_container_add(GTK_CONTAINER(pm_text), view_text);

	view_text_menu = gtk_menu_new();
	gtk_widget_ref(view_text_menu);
	gtk_object_set_data_full(GTK_OBJECT(pm_text),
				 "view_text_menu",
				 view_text_menu, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_text),
				  view_text_menu);
	view_text_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_text_menu));
	/*
	 * if module has cipher key include this item
	 */
	if (t->is_locked) {
		GtkWidget *add_module_key;
		separator = gtk_menu_item_new();
		gtk_widget_ref(separator);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "separator", separator,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(separator);
		gtk_container_add(GTK_CONTAINER(pm_text), separator);
		gtk_widget_set_sensitive(separator, FALSE);

		add_module_key =
		    gtk_menu_item_new_with_label(_
						 ("Unlock This Module"));
		gtk_widget_ref(add_module_key);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "add_module_key",
					 add_module_key,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(add_module_key);
		gtk_container_add(GTK_CONTAINER(pm_text),
				  add_module_key);

		gtk_signal_connect(GTK_OBJECT(add_module_key),
				   "activate",
				   GTK_SIGNAL_FUNC
				   (on_unlock_key_activate), t);
	}

	tmp = mod_lists->dict_descriptions;
	while (tmp != NULL) {
		item4 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item4);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "item4", item4,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item4);
		gtk_signal_connect(GTK_OBJECT(item4), "activate",
				   GTK_SIGNAL_FUNC
				   (on_lookup_selection_activate),
				   (gchar *) tmp->data);
		gtk_container_add(GTK_CONTAINER
				  (lookup_selection_menu), item4);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	i = 0;
	tmp = mod_lists->text_descriptions;
	while (tmp != NULL) {
		item3 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pm_text),
					 "item3", item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_view_mod_activate),
				   GINT_TO_POINTER(i));
		gtk_container_add(GTK_CONTAINER(view_text_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	/*
	 * for using the current dictionary for lookup 
	 */
	gtk_signal_connect(GTK_OBJECT(usecurrent), "activate",
			   GTK_SIGNAL_FUNC
			   (on_same_lookup_selection_activate), t);

	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			   GTK_SIGNAL_FUNC(on_copy_activate), t);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			   GTK_SIGNAL_FUNC(on_find_activate), t);
	gtk_signal_connect(GTK_OBJECT(t->showtabs), "activate",
			   GTK_SIGNAL_FUNC
			   (on_text_showtabs_activate), &settings);
	gtk_signal_connect(GTK_OBJECT(view_new), "activate",
			   GTK_SIGNAL_FUNC
			   (on_view_new_activate), &settings);
	return pm_text;
}

/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  gboolean on_button_release_event(GtkWidget * widget,
			    GdkEventButton * event, TEXT_DATA * t)	
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					TEXT_DATA * t)
{
	extern gboolean in_url;
	gchar *key;

	settings.whichwindow = MAIN_TEXT_WINDOW;

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = buttonpresslookupGS_HTML(t->html);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict = g_strdup(settings.DefaultDict);
				else
					dict = g_strdup(settings.DictWindowModule);
				if (settings.inViewer)
					display_dictlex_in_viewer(dict,
								  key,
								  &settings);
				if (settings.inDictpane)
					change_module_and_key(dict,
							      key);
				g_free(key);
				if (dict)
					g_free(dict);
			}
		}
		return TRUE;
		break;
	case 2:
		return TRUE;
		break;
	case 3:
		return TRUE;
		break;
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *  on_t_btn_toggled
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  void on_t_btn_toggled(GtkToggleButton *togglebutton, gchar *option)	
 *
 * Description
 *   called when global option toggle button is press
 *
 * Return value
 *   void
 */

static void on_t_btn_toggled(GtkToggleButton * togglebutton,
			     gchar * option)
{
	set_text_module_global_option(option, togglebutton->active);
}

void
on_primary_reading_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	set_text_variant_global_option("Textual Variants",
				       "Primary Reading");
}

void
on_secondary_reading_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
	set_text_variant_global_option("Textual Variants",
				       "Secondary Reading");
}

void
on_all_readings_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	set_text_variant_global_option("Textual Variants",
				       "All Readings");
}

static GnomeUIInfo primary_reading_uiinfo[] = {
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
	{
	 GNOME_APP_UI_ITEM, N_("All Readings"),
	 NULL,
	 (gpointer) on_all_readings_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo textual_variants_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_RADIOITEMS, NULL, NULL, primary_reading_uiinfo,
	 NULL, NULL, GNOME_APP_PIXMAP_NONE, NULL, 0,
	 (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo variant_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Textual Variants"),
	 NULL,
	 textual_variants_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

/******************************************************************************
 * Name
 *  gui_create_text_pane
 *
 * Synopsis
 *   #include "_bibletext.h"
 *   void gui_create_text_pane(SETTINGS * s, TEXT_DATA * t)
 *  	
 *
 * Description
 *   create a text pane(window) for each text module
 *
 * Return value
 *   void
 */

void gui_create_text_pane(SETTINGS * s, TEXT_DATA * t)
{
	GtkWidget *vbox;
	GtkWidget *frame;
	GtkWidget *toolbar;
	GtkWidget *scrolledwindow;
	GtkWidget *label;
	GtkWidget *variant_menu;

	t->frame = gtk_frame_new(NULL);
	gtk_widget_ref(t->frame);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "t->frame",
				 t->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(t->frame);
	gtk_container_add(GTK_CONTAINER(s->notebook_text), t->frame);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox", vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(t->frame), vbox);

	frame = gtk_frame_new(NULL);
	gtk_widget_ref(frame);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame", frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame);
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 0);

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_TEXT);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbar",
				 toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_container_add(GTK_CONTAINER(frame), toolbar);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);

	if (t->gbfstrongs || t->thmlstrongs) {
		t->t_btn_strongs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("S"),
					       _
					       ("Toggle Strongs Numbers"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_strongs);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_strongs",
					 t->t_btn_strongs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_strongs);
		gtk_widget_set_usize(t->t_btn_strongs, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_strongs),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Strong's Numbers");
	}
	if (t->gbfmorphs || t->thmlmorphs) {
		t->t_btn_morphs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("M"),
					       _("Toggle Morph Tags"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_morphs);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_morphs",
					 t->t_btn_morphs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_morphs);
		gtk_widget_set_usize(t->t_btn_morphs, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_morphs),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Morphological Tags");
	}
	if (t->gbffootnotes || t->thmlfootnotes) {
		t->t_btn_footnotes =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("F"),
					       _("Toggle Footnotes"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_footnotes);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_footnotes",
					 t->t_btn_footnotes,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_footnotes);
		gtk_widget_set_usize(t->t_btn_footnotes, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_footnotes),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Footnotes");
	}
	if (t->greekaccents) {
		t->t_btn_accents =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("A"),
					       _
					       ("Toggle Greek Accents"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_accents);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_accents",
					 t->t_btn_accents,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_accents);
		gtk_widget_set_usize(t->t_btn_accents, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_accents),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Greek Accents");

	}
	if (t->lemmas) {
		t->t_btn_lemmas =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("L"),
					       _("Toggle Lemmas"), NULL,
					       NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_lemmas);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_lemmas",
					 t->t_btn_lemmas,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_lemmas);
		gtk_widget_set_usize(t->t_btn_lemmas, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_lemmas),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Lemmas");

	}
	if (t->scripturerefs) {
		t->t_btn_scripturerefs =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("R"),
					       _
					       ("Toggle Scripture References"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_scripturerefs);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_scripturerefs",
					 t->t_btn_scripturerefs,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_scripturerefs);
		gtk_widget_set_usize(t->t_btn_scripturerefs, 24, 24);


		gtk_signal_connect(GTK_OBJECT(t->t_btn_scripturerefs),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Scripture Cross-references");

	}
	if (t->hebrewpoints) {
		t->t_btn_points =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("P"),
					       _
					       ("Toggle Hebrew Vowel Points"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_points);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_points",
					 t->t_btn_points,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_points);
		gtk_widget_set_usize(t->t_btn_points, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_points),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Hebrew Vowel Points");

	}
	if (t->hebrewcant) {
		t->t_btn_cant =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("C"),
					       _
					       ("Toggle Hebrew Cantillation"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_cant);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_cant", t->t_btn_cant,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_cant);
		gtk_widget_set_usize(t->t_btn_cant, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_cant), "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Hebrew Cantillation");

	}
	if (t->headings) {
		t->t_btn_headings =
		    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
					       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
					       NULL, _("H"),
					       _("Toggle Headings"),
					       NULL, NULL, NULL, NULL);
		gtk_widget_ref(t->t_btn_headings);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "t->t_btn_headings",
					 t->t_btn_headings,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(t->t_btn_headings);
		gtk_widget_set_usize(t->t_btn_headings, 24, 24);

		gtk_signal_connect(GTK_OBJECT(t->t_btn_headings),
				   "toggled",
				   GTK_SIGNAL_FUNC(on_t_btn_toggled),
				   "Headings");
	}
	if (t->variants) {
		variant_menu = gtk_menu_bar_new();
		gtk_widget_ref(variant_menu);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "variant_menu", variant_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(variant_menu);
		gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar),
					  variant_menu, NULL, NULL);
		gnome_app_fill_menu(GTK_MENU_SHELL(variant_menu),
				    variant_menu_uiinfo, NULL, FALSE,
				    0);

		gtk_widget_ref(variant_menu_uiinfo[0].widget);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "textual_variants",
					 variant_menu_uiinfo[0].widget,
					 (GtkDestroyNotify)
					 gtk_widget_unref);

		gtk_widget_ref(primary_reading_uiinfo[0].widget);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "primary_reading",
					 primary_reading_uiinfo[0].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
					       (primary_reading_uiinfo
						[0].widget), TRUE);

		t->t_btn_primary = primary_reading_uiinfo[0].widget;

		gtk_widget_ref(primary_reading_uiinfo[1].widget);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "secondary_reading",
					 primary_reading_uiinfo[1].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);

		t->t_btn_secondary = primary_reading_uiinfo[1].widget;

		gtk_widget_ref(primary_reading_uiinfo[2].widget);
		gtk_object_set_data_full(GTK_OBJECT(s->app),
					 "all_readings",
					 primary_reading_uiinfo[2].
					 widget, (GtkDestroyNotify)
					 gtk_widget_unref);

		t->t_btn_all = primary_reading_uiinfo[2].widget;
	}

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindow",
				 scrolledwindow,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	t->html = gtk_html_new();
	gtk_widget_ref(t->html);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "t->html", t->html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(t->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), t->html);
	gtk_html_load_empty(GTK_HTML(t->html));

	label = gtk_label_new(t->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label",
				 label, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->notebook_text),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(s->notebook_text),
				    t->mod_num), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (s->notebook_text),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (s->notebook_text),
					  t->mod_num),
					 (gchar *) t->mod_name);

	gtk_signal_connect(GTK_OBJECT(t->html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(t->html), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) s->app);
	gtk_signal_connect(GTK_OBJECT(t->html),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_button_release_event), (TEXT_DATA *) t);
}
