/*
 * GnomeSword Bible Study Tool
 * gs_bibletext.c - support for Bible text modules
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


#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_html.h"
#include "sword.h"
#include "gs_bibletext.h"
#include "bibletext.h"
#include "shortcutbar.h"
#include "gs_viewtext_dlg.h"
#include "cipher_key_dialog.h"

extern SETTINGS *settings;
extern gboolean isrunningVT;  
extern GList *options;

GList *text_list;
static gboolean display_change = TRUE;

static void on_notebook_text_switch_page(GtkNotebook * notebook,
				 GtkNotebookPage * page,
				 gint page_num, GList * cl);
static TEXT_DATA *cur_t;

void set_text_module_global_options(gchar * option,
			       gboolean choice)
{
	gchar *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}

	backend_save_module_options(settings->MainWindowModule,
				    option, on_off);

	if (!strcmp(option, "Strong's Numbers")) {
		GTK_CHECK_MENU_ITEM(settings->strongsnum)->
		    active = choice;
		/* set strongs toogle button */
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (lookup_widget
					      (settings->app,
					       "btnStrongs")),
					     choice);
	}

	if (!strcmp(option, "Footnotes")) {
		GTK_CHECK_MENU_ITEM(settings->footnotes)->
		    active = choice;
		/* set footnotes toogle button */
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (lookup_widget
					      (settings->app,
					       "btnFootnotes")),
					     choice);
	}

	if (!strcmp(option, "Morphological Tags")) {
		GTK_CHECK_MENU_ITEM(settings->morphs)->active =
		    choice;
		/* set morphs toogle button */
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (lookup_widget
					      (settings->app,
					       "btnMorphs")),
					     choice);
	}

	if (!strcmp(option, "Hebrew Vowel Points")) {
		GTK_CHECK_MENU_ITEM(settings->hebrewpoints)->
		    active = choice;
	}

	if (!strcmp(option, "Hebrew Cantillation")) {
		GTK_CHECK_MENU_ITEM(settings->
				    cantillationmarks)->active =
		    choice;
	}

	if (!strcmp(option, "Greek Accents")) {
		GTK_CHECK_MENU_ITEM(settings->greekaccents)->
		    active = choice;
	}

	backend_set_text_global_option(option, on_off);
	gui_display_text(settings->currentverse);
}


static
TEXT_DATA *get_text(GList * tl)
{
	GList *tmp;
	TEXT_DATA *t = NULL;

	tmp = NULL;
	tmp = tl;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		t = (TEXT_DATA *) tmp->data;
		if (!strcmp(t->mod_name, settings->MainWindowModule)) {
			break;
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return t;
}
				 
				 
void gui_set_text_page_and_key(gint page_num, gchar *key)
{
	display_change = FALSE;
	if(settings->text_last_page != page_num) {
		gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebook_text), page_num);
	}
	
	if(cur_t->is_locked){
			
	} else 
		backend_display_text(page_num,key);
	display_change = TRUE;
}

static
void on_notebook_text_switch_page(GtkNotebook * notebook,
				 GtkNotebookPage * page,
				 gint page_num, GList * tl)
{
	GList *tmp = NULL;
	TEXT_DATA *t;
	gchar title[200];
	gboolean value;
	
	t = (TEXT_DATA *) g_list_nth_data(tl, page_num);

	sprintf(settings->MainWindowModule, "%s", t->mod_name);
	cur_t = t;
	
	settings->text_last_page = page_num;
	if(display_change) {
		if(t->is_locked){
			
		} else 
			backend_display_text(page_num,
				settings->currentverse);
	}
	/*** set global options here ***/
	tmp = options;
	while (tmp != NULL) {
		value =
		    backend_load_module_options(t->mod_name, (gchar *) tmp->data);
		set_text_module_global_options((gchar*)tmp->data,
					  value);
		//g_warning("%s = %d",(gchar*)tmp->data,value); 
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	
	sprintf(title, "GnomeSWORD - %s", t->mod_description);
	gtk_window_set_title(GTK_WINDOW(settings->app), title);
	GTK_CHECK_MENU_ITEM(t->showtabs)->active = settings->text_tabs;
	
	if (settings->text_tabs)
		gtk_frame_set_label(GTK_FRAME(t->frame), NULL);	//-- set frame label
	else
		gtk_frame_set_label(GTK_FRAME(t->frame), t->mod_name);
}


static void set_page_text(gchar * modname, GList * text_list,
			SETTINGS * s)
{
	gint page = 0;
	TEXT_DATA *t = NULL;

	text_list = g_list_first(text_list);
	while (text_list != NULL) {
		t = (TEXT_DATA *) text_list->data;
		if (!strcmp(t->mod_name, modname))
			break;
		++page;
		text_list = g_list_next(text_list);
	}
	cur_t = t;
	gtk_notebook_set_page(GTK_NOTEBOOK(s->notebook_text), page);
	s->text_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_text),
				   s->text_tabs);
}

/****  popup menu call backs  ****/

static
void on_copy_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	copyGS_HTML(t->html);
}

static
void on_find_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	//searchGS_FIND_DLG(c, FALSE, NULL);
}

static
void on_lookup_selection_activate(GtkMenuItem * menuitem,
				  gchar * modDescription)
{
	TEXT_DATA *t;
	gchar modName[16];
	gchar *key;

	t = get_text(text_list);
	memset(modName, 0, 16);
	backend_module_name_from_description(modName, modDescription);
	key = lookupGS_HTML(t->html, FALSE);
	if (key) {
		display_dictlex_in_viewer(modName, key, settings);
		g_free(key);
	}
}

static
void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
				       TEXT_DATA * t)
{
	gchar *key = lookupGS_HTML(t->html, FALSE);
	if (key) {
		display_dictlex_in_viewer(settings->DictWindowModule, key,
				   settings);
		g_free(key);
	}
}

static void
on_view_mod_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gint page;

	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebook_text),
			      page);
}

static void on_text_showtabs_activate(GtkMenuItem * menuitem,
				      SETTINGS * s)
{
	s->text_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_text),
				   s->text_tabs);
}

static void on_view_new_activate(GtkMenuItem * menuitem, SETTINGS * s)
{
	static GtkWidget *dlg;
        GdkCursor *cursor;	
	
	gtk_widget_show(s->app);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(s->app->window,cursor);
	
	if(!isrunningVT) {
		//dlg = create_dlgViewText(s);
		isrunningVT = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(s->app);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(s->app->window,cursor);
}

static void on_unlock_key_activate(GtkMenuItem * menuitem, TEXT_DATA * t)
{
	GtkWidget *dlg;
	gchar *key;
	
	dlg = gui_create_unlock_key_dialog(t->mod_name);
	gnome_dialog_set_default(GNOME_DIALOG(dlg), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(dlg));	
	/*
	t->is_locked = backend_module_is_locked(t->mod_name);
	if(!t->is_locked) {
		key = backend_get_cipher_key(t->mod_name);
		backend_set_module_unlocked(t->mod_name, t->mod_num, key);
		cur_t = t;
	}
	*/
}

static
GtkWidget *create_pm_text(TEXT_DATA * t)
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


	view_text =
	    gtk_menu_item_new_with_label(_("View Text"));
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
				 view_text_menu,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_text),
				  view_text_menu);
	view_text_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (view_text_menu));
	/*
	   if module has cipher key include this item
	 */
	if(t->is_locked) {
		GtkWidget *add_module_key;
		separator = gtk_menu_item_new();
		gtk_widget_ref(separator);
		gtk_object_set_data_full(GTK_OBJECT(pm_text), "separator",
					 separator, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(separator);	
		gtk_container_add(GTK_CONTAINER(pm_text), separator);
		gtk_widget_set_sensitive(separator, FALSE);
			
		add_module_key = gtk_menu_item_new_with_label (_("Unlock This Module"));
		gtk_widget_ref (add_module_key);
		gtk_object_set_data_full (GTK_OBJECT (pm_text), "add_module_key",add_module_key ,
				    (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show (add_module_key);
		gtk_container_add (GTK_CONTAINER (pm_text), add_module_key);
			
		gtk_signal_connect (GTK_OBJECT (add_module_key), "activate",
                      	GTK_SIGNAL_FUNC (on_unlock_key_activate),
                      	t);
	}
  	
	tmp = backend_get_mod_description_list_SWORD(DICT_MODS);
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
	tmp = backend_get_mod_description_list_SWORD(TEXT_MODS);
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
		gtk_container_add(GTK_CONTAINER
				  (view_text_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	
        /*
	   for using the current dictionary for lookup 
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
			   (on_text_showtabs_activate), settings);
	gtk_signal_connect(GTK_OBJECT(view_new), "activate",
			   GTK_SIGNAL_FUNC
			   (on_view_new_activate), settings);
	return pm_text;
}





static gboolean
on_button_release_event(GtkWidget * widget,
			GdkEventButton * event, TEXT_DATA * t)
{
	extern gboolean in_url;
	gchar *key;
	
	settings->whichwindow = MAIN_TEXT_WINDOW;
	
	switch (event->button) {
	case 1:
		if (!in_url) {
			key = buttonpresslookupGS_HTML(t->html);
			if (key) {
				gchar *dict = NULL;
				if (settings->useDefaultDict)
					dict = g_strdup(settings->
							DefaultDict);
				else
					dict = g_strdup(settings->
							DictWindowModule);
				if (settings->inViewer)
					display_dictlex_in_viewer(dict, key,
							   settings);
				if (settings->inDictpane)
					change_module_and_key(dict, key);
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


static 
void create_text_pane(SETTINGS *s, TEXT_DATA *t, gint count)
{
  GtkWidget *vbox;
  GtkWidget *toolbar31;
  GtkWidget *button7;
  GtkWidget *scrolledwindow58;
  GtkWidget *label;

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "vbox", vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox);
  gtk_container_add(GTK_CONTAINER(s->notebook_text), vbox);
	
  toolbar31 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar31);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "toolbar31", toolbar31,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar31);
  gtk_box_pack_start (GTK_BOX (vbox), toolbar31, FALSE, FALSE, 0);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar31), GTK_RELIEF_NONE);

  button7 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar31),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button7"),
                                NULL, NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (button7);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "button7", button7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button7);

  t->frame = gtk_frame_new (NULL);
  gtk_widget_ref (t->frame);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "t->frame", t->frame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (t->frame);
  gtk_box_pack_start (GTK_BOX (vbox), t->frame, TRUE, TRUE, 0);

  scrolledwindow58 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow58);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "scrolledwindow58", scrolledwindow58,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow58);
  gtk_container_add (GTK_CONTAINER (t->frame), scrolledwindow58);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow58), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

/*  gtk_signal_connect (GTK_OBJECT (button7), "clicked",
                      GTK_SIGNAL_FUNC (on_button_clicked),
                      NULL);*/

	t->html = gtk_html_new();
	gtk_widget_ref(t->html);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "t->html", t->html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(t->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow58),
			  t->html);
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
				    count), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (s->notebook_text),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (s->notebook_text),
					  count), (gchar *) t->mod_name);

	gtk_signal_connect(GTK_OBJECT(t->html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(t->html), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) s->app);
	gtk_signal_connect(GTK_OBJECT(t->html),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_button_release_event), (TEXT_DATA *) t);

  
}

void gui_display_text(gchar * key)
{
	if(!cur_t->is_locked)
		backend_display_text(settings->text_last_page, key);
}

GList* gui_setup_text(SETTINGS * s)
{
	GtkWidget *popupmenu;
	GList *tmp = NULL;
	GList *mods = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	TEXT_DATA *t;
	gint count = 0;

	text_list = NULL;

	mods = backend_get_list_of_mods_by_type(TEXT_MODS);
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		t = g_new(TEXT_DATA, 1);
		t->mod_name = modname;
		t->mod_description =
		    backend_get_module_description(modname);
		t->mod_num = count;
		t->search_string = NULL;
		t->key = NULL;
		t->find_dialog = NULL;		
		t->is_locked = backend_module_is_locked(t->mod_name);
		create_text_pane(s, t, count);
		popupmenu = create_pm_text(t);
		gnome_popup_menu_attach(popupmenu, t->html, NULL);
		backend_new_text_display(t->html, t->mod_name, s);
		text_list = g_list_append(text_list, (TEXT_DATA *) t);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(s->notebook_text),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_text_switch_page), text_list);

	modbuf = g_strdup(s->MainWindowModule);
	keybuf = g_strdup(s->currentverse);

	set_page_text(modbuf, text_list, s);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
	//g_list_free(mods);
	return mods;
}

void gui_shutdown_text(void)
{
	text_list = g_list_first(text_list);
	while (text_list != NULL) {
		TEXT_DATA *t = (TEXT_DATA *) text_list->data;
		/* 
		   free any search dialogs created 
		 */
		if (t->find_dialog)	
			g_free(t->find_dialog);
		/* 
		   free each TEXT_DATA item created 
		 */
		g_free((TEXT_DATA *) text_list->data);
		text_list = g_list_next(text_list);
	}
	g_list_free(text_list);
}

/******  end of file  ******/
