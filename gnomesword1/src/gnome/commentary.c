/*
 * GnomeSword Bible Study Tool
 * commentary.c - gui for commentary modules
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
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>

#include "gui/gtkhtml_display.h"
#include "gui/commentary.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/commentary_find.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"

#include "main/commentary.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"


static void on_notebook_comm_switch_page(GtkNotebook * notebook,
				 GtkNotebookPage * page,
				 gint page_num, GList * cl);
/******************************************************************************
 * externs
 */ 
extern gboolean comm_find_running;

/******************************************************************************
 * global to this file only 
 */
static GList *comm_list;
static COMM_DATA *cur_c;
static gboolean comm_display_change;


/******************************************************************************
 * Name
 *  display
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void display(COMM_DATA *c, gchar * key)	
 *
 * Description
 *   calls entry_display to display a commentary entry 
 *
 * Return value
 *   void
 */
 
static void display(COMM_DATA *c, gchar * key)
{
	gchar *text_str = NULL;
	gchar *strkey;
	
	strkey = get_valid_key(key);
	if(c->book_heading){
		c->book_heading = FALSE;
		text_str = get_book_heading(c->mod_name, strkey);
	}
	
	else if(c->chapter_heading){
		c->chapter_heading = FALSE;
		text_str = get_chapter_heading(c->mod_name, strkey);
	}
	
	else {
		text_str = get_commentary_text(c->mod_name, strkey);
	}
	entry_display(c->html, c->mod_name, text_str, key, TRUE);
	free(text_str);
	
}

/******************************************************************************
 * Name
 *  set_commentary_page
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void set_commentary_page(gchar * modname, GList * comm_list)	
 *
 * Description
 *    change commentary page without changing key
 *
 * Return value
 *   void
 */
 
static void set_commentary_page(gchar * modname, GList * comm_list)
{
	gint page = 0;
	COMM_DATA *c = NULL;

	comm_list = g_list_first(comm_list);
	while (comm_list != NULL) {
		c = (COMM_DATA *) comm_list->data;
		if (!strcmp(c->mod_name, modname))
			break;
		++page;
		comm_list = g_list_next(comm_list);
	}
	if(page)
		gtk_notebook_set_page(GTK_NOTEBOOK(
				  widgets.notebook_comm), page);
	else
		on_notebook_comm_switch_page(GTK_NOTEBOOK(
				  widgets.notebook_comm),
				  NULL,
				  page, 
				  comm_list);
	settings.comm_last_page = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_comm),
			settings.comm_tabs);
}



/******************************************************************************
 * Name
 *  set_comm_frame_label
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void set_comm_frame_label(COMM_DATA *c)	
 *
 * Description
 *   sets frame label to module name or null
 *
 * Return value
 *   void
 */

static void set_comm_frame_label(COMM_DATA *c)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */	
	if (settings.comm_tabs)
		gtk_frame_set_label(GTK_FRAME(c->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(c->frame), c->mod_name);
	
}

/******************************************************************************
 * Name
 *  on_notebook_comm_switch_page
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_notebook_comm_switch_page(GtkNotebook * notebook,
 *				  GtkNotebookPage * page,
 *				  gint page_num, GList * cl)	
 *
 * Description
 *   sets commentary gui to new sword module
 *
 * Return value
 *   void
 */

void on_notebook_comm_switch_page(GtkNotebook * notebook,
				 GtkNotebookPage * page,
				 gint page_num, GList * cl)
{
	COMM_DATA *c, *c_old;
	c_old = (COMM_DATA *) g_list_nth_data(cl,
					 settings.comm_last_page);
	c = (COMM_DATA *) g_list_nth_data(cl, page_num);
	cur_c = c;
	
	strcpy(settings.CommWindowModule, c->mod_name);
	
	if(!c->frame)
		gui_add_new_comm_pane(c);
	
	set_comm_frame_label(c);
	
	if(comm_find_running) {
		gnome_dialog_close(c_old->find_dialog->dialog);
		search_comm_find_dlg(c, FALSE, settings.findText);
	}
	gui_change_window_title(c->mod_name);
	/*
	 * set search module to current text module 
	 */
	strcpy(settings.sb_search_mod, c->mod_name);
	/*
	 * set search frame label to current commentary module 
	 */	
	gui_set_search_label(c->mod_name);
	
	settings.comm_last_page = page_num;
	/*
	 * set settings.comm_key to current module key
	 */
	if(c->key)
		strcpy(settings.comm_key,c->key);
	
	if(comm_display_change) {
		if ((c->key[0] == '\0') && (settings.currentverse != NULL)) {
			display(c, settings.currentverse);
			strcpy(settings.comm_key,settings.currentverse);
			strcpy(c->key, settings.comm_key);
		}
	}
	
	if(settings.comm_tool)
		gtk_widget_show(c->frame_toolbar);
	else
		gtk_widget_hide(c->frame_toolbar);
	GTK_CHECK_MENU_ITEM(c->showtoolbar)->active = settings.comm_tool;
	
	GTK_CHECK_MENU_ITEM(c->showtabs)->active = settings.comm_tabs;
	widgets.html_comm = c->html;
}

/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, COMM_DATA * c)
 *
 * Description
 *   copy selection to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gui_copy_html(c->html);
}

/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_find_activate(GtkMenuItem * menuitem, COMM_DATA * c)
 *
 * Description
 *   open find dialog
 *
 * Return value
 *   void
 */

static void on_find_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	search_comm_find_dlg(c, FALSE, NULL);
}

/******************************************************************************
 * Name
 *   on_lookup_selection_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_lookup_selection_activate(GtkMenuItem * menuitem,
 *					gchar * dict_mod_description)
 *
 * Description
 *   lookup selection in new dict/lex
 *
 * Return value
 *   void
 */

static void on_lookup_selection_activate(GtkMenuItem * menuitem,
					gchar * dict_mod_description)
{
	gchar *dict_key, dict_mod[16];

	memset(dict_mod, 0, 16);
	module_name_from_description(dict_mod, dict_mod_description);
	dict_key = gui_get_word_or_selection(cur_c->html, FALSE);
	if (dict_key) {
		gui_display_dictlex_in_viewer(dict_mod, dict_key);
		g_free(dict_key);
	}
}

/******************************************************************************
 * Name
 *   on_same_lookup_selection_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
 *							COMM_DATA * c)
 *
 * Description
 *   lookup selection in current dict/lex
 *
 * Return value
 *   void
 */

static void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
							COMM_DATA * c)
{
	gchar *key = gui_get_word_or_selection(c->html, FALSE);
	if (key) {
		gui_display_dictlex_in_viewer(settings.DictWindowModule, 
						key);
		g_free(key);
	}
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_view_mod_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)
 *
 * Description
 *   select new mode by changing the notebook page
 *
 * Return value
 *   void
 */

static void on_view_mod_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{
	gint page;

	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_comm),
			      page);
}

/******************************************************************************
 * Name
 *   on_comm_showtabs_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_comm_showtabs_activate(GtkMenuItem * menuitem, COMM_DATA *c)
 *
 * Description
 *   display/hide commentary notebook tabs
 *
 * Return value
 *   void
 */

static void on_comm_showtabs_activate(GtkMenuItem * menuitem,
						COMM_DATA *c)
{
	settings.comm_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_comm),
				   settings.comm_tabs);
	set_comm_frame_label(c);
}

/******************************************************************************
 * Name
 *   on_comm_showtoolbar_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_comm_showtoolbar_activate(GtkMenuItem *menuitem, COMM_DATA *)
 *
 * Description
 *   display/hide commentary toolbar
 *
 * Return value
 *   void
 */

static void on_comm_showtoolbar_activate(GtkMenuItem * menuitem,
						COMM_DATA * c)
{
	settings.comm_tool = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if(settings.comm_tool)
		gtk_widget_show(c->frame_toolbar);
	else
		gtk_widget_hide(c->frame_toolbar);
}

/******************************************************************************
 * Name
 *   on_view_new_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_view_new_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   open new commentary dialog
 *
 * Return value
 *   void
 */

static void on_view_new_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	gui_open_commentary_dialog(c->mod_name);
}

/******************************************************************************
 * Name
 *   on_unlock_key_activate
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_unlock_key_activate(GtkMenuItem *menuitem, COMM_DATA *c)
 *
 * Description
 *   open cipher dialog to add cipher key to unlock module
 *
 * Return value
 *   void
 */

static void on_unlock_key_activate(GtkMenuItem *menuitem, COMM_DATA *c)
{
	GtkWidget *dlg;
	
	dlg = gui_create_cipher_key_dialog(c->mod_name);
	gtk_widget_show(dlg);
}

/******************************************************************************
 * Name
 *   gui_create_pm
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   GtkWidget *gui_create_pm(COMM_DATA * c)
 *
 * Description
 *   create popup menu for a commentary pane
 *
 * Return value
 *   GtkWidget*
 */

static GtkWidget *create_pm(COMM_DATA * c)
{
	GtkWidget *pm;
	GtkAccelGroup *pm_accels;
	GtkWidget *copy;
	GtkWidget *separator;
	GtkWidget *lookup_selection;
	GtkWidget *lookup_selection_menu;
	GtkAccelGroup *lookup_selection_menu_accels;
	GtkWidget *usecurrent;
	GtkWidget *item3;
	GtkWidget *item4;
	GtkWidget *view_new;
	GtkWidget *view_commentary;
	GtkWidget *view_commentary_menu;
	GtkAccelGroup *view_commentary_menu_accels;
	GtkWidget *find;
	GList *tmp;
	gint i;

	tmp = NULL;

	pm = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pm), "pm", pm);
	pm_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pm));

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_ref(copy);
	gtk_object_set_data_full(GTK_OBJECT(pm), "copy", copy,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(pm), copy);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_ref(find);
	gtk_object_set_data_full(GTK_OBJECT(pm), "find", find,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(pm), find);

	c->showtabs =
	    gtk_check_menu_item_new_with_label(_("Show Tabs"));
	gtk_widget_ref(c->showtabs);
	gtk_object_set_data_full(GTK_OBJECT(pm), "c->showtabs",
				 c->showtabs, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->showtabs);
	gtk_container_add(GTK_CONTAINER(pm), c->showtabs);

	c->showtoolbar =
	    gtk_check_menu_item_new_with_label(_("Show Toolbar"));
	gtk_widget_ref(c->showtoolbar);
	gtk_object_set_data_full(GTK_OBJECT(pm), "c->showtoolbar",
				 c->showtoolbar, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->showtoolbar);
	gtk_container_add(GTK_CONTAINER(pm), c->showtoolbar);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	view_new = gtk_menu_item_new_with_label("View in new window");
	gtk_widget_ref(view_new);
	gtk_object_set_data_full(GTK_OBJECT(pm), "view_new",
				 view_new, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(view_new);
	gtk_container_add(GTK_CONTAINER(pm), view_new);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm), separator);
	gtk_widget_set_sensitive(separator, FALSE);


	lookup_selection =
	    gtk_menu_item_new_with_label(_("Lookup Selection"));
	gtk_widget_ref(lookup_selection);
	gtk_object_set_data_full(GTK_OBJECT(pm),
				 "lookup_selection", lookup_selection,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(lookup_selection);
	gtk_container_add(GTK_CONTAINER(pm), lookup_selection);

	lookup_selection_menu = gtk_menu_new();
	gtk_widget_ref(lookup_selection_menu);
	gtk_object_set_data_full(GTK_OBJECT(pm),
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
	gtk_object_set_data_full(GTK_OBJECT(pm), "usecurrent",
				 usecurrent, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);


	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm), separator);
	gtk_widget_set_sensitive(separator, FALSE);


	view_commentary =
	    gtk_menu_item_new_with_label(_("View Commentary"));
	gtk_widget_ref(view_commentary);
	gtk_object_set_data_full(GTK_OBJECT(pm),
				 "view_commentary", view_commentary,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(view_commentary);
	gtk_container_add(GTK_CONTAINER(pm), view_commentary);

	view_commentary_menu = gtk_menu_new();
	gtk_widget_ref(view_commentary_menu);
	gtk_object_set_data_full(GTK_OBJECT(pm),
				 "view_commentary_menu",
				 view_commentary_menu,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_commentary),
				  view_commentary_menu);
	view_commentary_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (view_commentary_menu));
	/*
	 * if module has cipher key include this item
	 */
	if(c->has_key) {
		GtkWidget *add_module_key;
		separator = gtk_menu_item_new();
		gtk_widget_ref(separator);
		gtk_object_set_data_full(GTK_OBJECT(pm), "separator",
					 separator, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(separator);	
		gtk_container_add(GTK_CONTAINER(pm), separator);
		gtk_widget_set_sensitive(separator, FALSE);
			
		add_module_key = gtk_menu_item_new_with_label (_("Unlock This Module"));
		gtk_widget_ref (add_module_key);
		gtk_object_set_data_full (GTK_OBJECT (pm), "add_module_key",add_module_key ,
				    (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show (add_module_key);
		gtk_container_add (GTK_CONTAINER (pm), add_module_key);
			
		gtk_signal_connect (GTK_OBJECT (add_module_key), "activate",
                      	GTK_SIGNAL_FUNC (on_unlock_key_activate),
                      	c);
	}
	tmp = get_list(DICT_DESC_LIST);
	while (tmp != NULL) {
		item4 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item4);
		gtk_object_set_data_full(GTK_OBJECT(pm),
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
	tmp = get_list(COMM_DESC_LIST);
	while (tmp != NULL) {
		item3 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pm),
					 "item3", item3,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_view_mod_activate),
				   GINT_TO_POINTER(i));
		gtk_container_add(GTK_CONTAINER
				  (view_commentary_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	/*
	 * for using the current dictionary for lookup 
	 */
	gtk_signal_connect(GTK_OBJECT(usecurrent), "activate",
			GTK_SIGNAL_FUNC
			(on_same_lookup_selection_activate), c);

	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			GTK_SIGNAL_FUNC(on_copy_activate), c);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			GTK_SIGNAL_FUNC(on_find_activate), c);
	gtk_signal_connect(GTK_OBJECT(c->showtabs), "activate",
			GTK_SIGNAL_FUNC(on_comm_showtabs_activate), 
			c);
	gtk_signal_connect(GTK_OBJECT(c->showtoolbar), "activate",
			GTK_SIGNAL_FUNC(on_comm_showtoolbar_activate), 
			c);
	gtk_signal_connect(GTK_OBJECT(view_new), "activate",
			GTK_SIGNAL_FUNC(on_view_new_activate), 
			c);
	return pm;
}

/******************************************************************************
 * Name
 *   on_btn_sync_clicked
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_btn_sync_clicked(GtkButton * button, COMM_DATA * c)
 *
 * Description
 *   bring commentary in sync with Bible text
 *
 * Return value
 *   void
 */

static void on_btn_sync_clicked(GtkButton * button, COMM_DATA * c)
{
	gui_set_commentary_page_and_key(c->modnum, settings.currentverse);
}

/******************************************************************************
 * Name
 *   on_btn_back_clicked
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_btn_back_clicked(GtkButton * button, COMM_DATA * c)
 *
 * Description
 *   move backward through the commentary keys
 *
 * Return value
 *   void
 */

static void on_btn_back_clicked(GtkButton * button, COMM_DATA * c)
{
	gchar *key;
	
	set_commentary_key(c->mod_name, c->key);	
	key = navigate_commentary(c->mod_name, 0);
	if(key) {
		strcpy(settings.comm_key,key);
		strcpy(cur_c->key, settings.comm_key);
		display(c,key);
		free(key);
	}
}

/******************************************************************************
 * Name
 *   on_btn_forward_clicked
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_btn_forward_clicked(GtkButton * button, COMM_DATA * c)
 *
 * Description
 *   move foward through the commentary keys
 *
 * Return value
 *   void
 */

static void on_btn_forward_clicked(GtkButton * button, COMM_DATA * c)
{
	gchar *key;
	
	set_commentary_key(c->mod_name, c->key);	
	key = navigate_commentary(c->mod_name, 1);
	if(key) {
		strcpy(settings.comm_key,key);
		strcpy(cur_c->key, settings.comm_key);
		display(c,key);
		free(key);
	}
}

/******************************************************************************
 * Name
 *   on_btn_print_clicked
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_btn_print_clicked(GtkButton * button, COMM_DATA * c)
 *
 * Description
 *   send comment to the printer
 *
 * Return value
 *   void
 */

static void on_btn_print_clicked(GtkButton * button, COMM_DATA * c)
{
	gui_html_print(c->html);
}

/******************************************************************************
 * Name
 *   on_btn_book_heading_clicked
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_btn_book_heading_clicked(GtkButton * button, COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_btn_book_heading_clicked(GtkButton *button, COMM_DATA *c)
{
	c->book_heading = TRUE;
	display(c,settings.currentverse);
}

/******************************************************************************
 * Name
 *   on_btn_chap_heading_clicked
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void on_btn_chap_heading_clicked(GtkButton * button, COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_btn_chap_heading_clicked(GtkButton *button, COMM_DATA *c)
{
	c->chapter_heading = TRUE;
	display(c,settings.currentverse);
}

/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   gboolean on_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event, COMM_DATA * c)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static gboolean on_button_release_event(GtkWidget * widget,
			GdkEventButton * event, COMM_DATA * c)
{
	extern gboolean in_url;
	gchar *key;
	
	settings.whichwindow = COMMENTARY_WINDOW;
	
	gui_change_window_title(c->mod_name);
	/*
	 * set search module to current commentary module 
	 */
	strcpy(settings.sb_search_mod, c->mod_name);
	/*
	 * set search frame label to current commentary module 
	 */	
	gui_set_search_label(c->mod_name);
	
	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(c->html);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict = g_strdup(settings.DefaultDict);
				else
					dict = g_strdup(settings.DictWindowModule);
				if (settings.inViewer)
					gui_display_dictlex_in_viewer(dict, key);
				if (settings.inDictpane)
					gui_change_module_and_key(dict, key);
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
 *    gui_create_commentary_pane
 *
 * Synopsis
 *   #include "_commentary.h"
 *
 *   void gui_create_commentary_pane(COMM_DATA * c)
 *
 * Description
 *   create a commentary pane (window) for a sword commentary module
 *
 * Return value
 *   void
 */

static void create_commentary_pane(COMM_DATA *c)
{
	GtkWidget *vbox57;
	GtkWidget *toolbar;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator19;
	GtkWidget *scrolledwindowCOMMhtml;


	c->frame = gtk_frame_new(NULL);
	gtk_widget_ref(c->frame);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "c->frame",
				 c->frame, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->frame);
	gtk_container_add(GTK_CONTAINER(c->vbox), c->frame);

	vbox57 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox57);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vbox57",
				 vbox57, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox57);
	gtk_container_add(GTK_CONTAINER(c->frame), vbox57);

	c->frame_toolbar = gtk_frame_new(NULL);
	gtk_widget_ref(c->frame_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), 
				"c->frame_toolbar", c->frame_toolbar,
				(GtkDestroyNotify) gtk_widget_unref);
	//gtk_widget_show(c->frame_toolbar);
	gtk_box_pack_start(GTK_BOX(vbox57), 
				c->frame_toolbar, FALSE, TRUE, 0);
	
	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "toolbar",
				 toolbar, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_container_add(GTK_CONTAINER(c->frame_toolbar), toolbar);	
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_REFRESH);
	c->btnCOMMSync =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"),
				       _("Sync with Bible Text"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(c->btnCOMMSync);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "c->btnCOMMSync", c->btnCOMMSync,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btnCOMMSync);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_BACK);
	c->btnCOMMBack =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Back"),
				       _("Go to previous comment"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(c->btnCOMMBack);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "c->btnCOMMBack", c->btnCOMMBack,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btnCOMMBack);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_FORWARD);
	c->btnCOMMForward =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Forward"),
				       _("Go to next comment"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(c->btnCOMMForward);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "c->btnCOMMForward",
				 c->btnCOMMForward, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btnCOMMForward);

	vseparator19 = gtk_vseparator_new();
	gtk_widget_ref(vseparator19);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "vseparator19", vseparator19,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vseparator19);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar),
				  vseparator19, NULL, NULL);
	gtk_widget_set_usize(vseparator19, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_PRINT);
	c->btnCOMMPrint =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"),
				       _("Print Comment"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(c->btnCOMMPrint);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "c->btnCOMMPrint", c->btnCOMMPrint,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btnCOMMPrint);

	vseparator19 = gtk_vseparator_new();
	gtk_widget_ref(vseparator19);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "vseparator19", vseparator19,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vseparator19);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar),
				  vseparator19, NULL, NULL);
	gtk_widget_set_usize(vseparator19, 5, 7);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_TOP);
	c->btn_book_heading =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Book"),
				       _("Display Book Heading"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(c->btn_book_heading);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "c->btn_book_heading", c->btn_book_heading,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btn_book_heading);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_UP);
	c->btn_chap_heading =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Chapter"),
				       _("Display Chapter Heading"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(c->btn_chap_heading);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "c->btn_chap_heading", c->btn_chap_heading,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btn_chap_heading);
	
	scrolledwindowCOMMhtml = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowCOMMhtml);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "scrolledwindowCOMMhtml",
				 scrolledwindowCOMMhtml,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindowCOMMhtml);
	gtk_box_pack_start(GTK_BOX(vbox57), scrolledwindowCOMMhtml,
			   TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowCOMMhtml),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);


	c->html = gtk_html_new();
	gtk_widget_ref(c->html);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "c->html", c->html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowCOMMhtml),
			  c->html);
	gtk_html_load_empty(GTK_HTML(c->html));


	gtk_signal_connect(GTK_OBJECT(c->html), "link_clicked",
			   GTK_SIGNAL_FUNC(gui_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(c->html), "on_url",
			   GTK_SIGNAL_FUNC(gui_url), 
			   (gpointer) widgets.app);
	gtk_signal_connect(GTK_OBJECT(c->html), "button_release_event",
			   GTK_SIGNAL_FUNC(on_button_release_event), 
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMSync), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_sync_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMBack), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_back_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMForward),"clicked",
			   GTK_SIGNAL_FUNC(on_btn_forward_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMPrint), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_print_clicked), 
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btn_chap_heading), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_chap_heading_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btn_book_heading), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_book_heading_clicked),
			   (COMM_DATA *) c);
}

/******************************************************************************
 * Name
 *  gui_set_commentary_page_and_key
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_set_commentary_page_and_key(gint page_num, gchar *key)	
 *
 * Description
 *   change commentary page and key 
 *
 * Return value
 *   void
 */
 
void gui_set_commentary_page_and_key(gint page_num, gchar *key)
{
	comm_display_change = FALSE;
	strcpy(settings.comm_key,key);
	strcpy(cur_c->key,key);
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.notebook_comm), page_num);
	display(cur_c,key);
	comm_display_change = TRUE;
}

/******************************************************************************
 * Name
 *   gui_display_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_display_commentary(gchar * key)	
 *
 * Description
 *    display new key in current commentary
 *
 * Return value
 *   void
 */
 
void gui_display_commentary(gchar * key)
{
        if(!cur_c) return;
	strcpy(settings.comm_key,key);
	strcpy(cur_c->key, key);
	display(cur_c, key);
} 

/******************************************************************************
 * Name
 *  gui_add_new_comm_pane
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_add_new_comm_pane(COMM_DATA *c)
 *
 * Description
 *   creates a commentary pane when user selects a new commentary module
 *
 * Return value
 *   void
 */

void gui_add_new_comm_pane(COMM_DATA *c)
{	
	GtkWidget *popupmenu;
	
	create_commentary_pane(c);
	popupmenu = create_pm(c);
	gnome_popup_menu_attach(popupmenu, c->html, NULL);
}

/******************************************************************************
 * Name
 *  add_vbox_to_notebook
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void add_vbox_to_notebook(COMM_DATA *c)
 *
 * Description
 *   adds a vbox and label to the commentary notebook for each comm module
 *
 * Return value
 *   void
 */

static void add_vbox_to_notebook(COMM_DATA *c)
{	
	GtkWidget *label;
	
	c->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(c->vbox);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), 
			"c->vbox", c->vbox,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(c->vbox);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_comm), c->vbox);
	
	label = gtk_label_new(c->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label",
				 label, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_comm),
				gtk_notebook_get_nth_page
				(GTK_NOTEBOOK(widgets.notebook_comm),
				c->modnum), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
				(widgets.notebook_comm),
				gtk_notebook_get_nth_page
				(GTK_NOTEBOOK(widgets.notebook_comm),
				c->modnum), (gchar *) c->mod_name);
}


/******************************************************************************
 * Name
 *  gui_setup_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_setup_commentary(GList *mods)	
 *
 * Description
 *    setup gui commentary support and return list on commentary names
 *
 * Return value
 *   void
 */
 
void gui_setup_commentary(GList *mods)
{
	GList *tmp = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	COMM_DATA *c;
	gint count = 0;
	extern gboolean comm_find_running;
	
	comm_list = NULL;
	comm_find_running = FALSE;
	
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		c = g_new(COMM_DATA, 1);
		c->frame = NULL;
		c->mod_name = modname;
		c->modnum = count;
		c->search_string = NULL;
		c->key[0] = '\0';
		c->book_heading = FALSE;
		c->chapter_heading = FALSE;
		c->find_dialog = NULL;		
		c->has_key = module_is_locked(c->mod_name);
		add_vbox_to_notebook(c);
		comm_list = g_list_append(comm_list, (COMM_DATA *) c);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(widgets.notebook_comm),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_comm_switch_page), comm_list);

	modbuf = g_strdup(settings.CommWindowModule);
	keybuf = g_strdup(settings.currentverse);

	set_commentary_page(modbuf, comm_list);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *   shutdown_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void shutdown_commentary(void)	
 *
 * Description
 *   shut down and cleanup 
 *
 * Return value
 *   void
 */
 
void gui_shutdown_commentary(void)
{
	comm_list = g_list_first(comm_list);
	while (comm_list != NULL) {
		COMM_DATA *c = (COMM_DATA *) comm_list->data;
		/* 
		 * free any find dialogs created 
		 */
		if (c->find_dialog)	
			g_free(c->find_dialog);
		
		g_free((COMM_DATA *) comm_list->data);
		comm_list = g_list_next(comm_list);
	}
	g_list_free(comm_list);
}


//******  end of file  ******/
