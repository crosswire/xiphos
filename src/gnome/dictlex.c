/*
 * GnomeSword Bible Study Tool
 * _dictlex.c - gui for commentary modules
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

#include "gui/gtkhtml_display.h"
#include "gui/dictlex.h"
#include "gui/cipher_key_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/dictlex_dialog.h"
#include "gui/html.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/dictlex.h"

/******************************************************************************
 * externs
 */
extern gboolean dict_display_change;
extern gboolean isrunningSD;    /* is the view dictionary dialog runing */


/******************************************************************************
 * global to this file only 
 */
static GList *dl_list;
static DL_DATA *cur_d;



/******************************************************************************
 * Name
 *   gui_display_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void gui_display_dictlex(gchar * key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_display_dictlex(gchar * key)
{
	gui_set_dictionary_page_and_key(settings.dict_last_page, key);
}

/******************************************************************************
 * Name
 *   gui_set_dictionary_page_and_key
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void gui_set_dictionary_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *   change notebook page and set new key in entry widget
 *
 * Return value
 *   void
 */

void gui_set_dictionary_page_and_key(gint page_num, gchar * key)
{
	DL_DATA *d;

	d = (DL_DATA *) g_list_nth_data(dl_list, page_num);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebookDL),
			      page_num);
	gtk_entry_set_text(GTK_ENTRY(d->entry), key);

}

/******************************************************************************
 * Name
 *   set_page_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void set_page_dictlex(gchar * modname, GList * dl_list)
 *
 * Description
 *    set dictlex notebook page on startup
 *
 * Return value
 *   void
 */

static void set_page_dictlex(gchar * modname, GList * dl_list)
{
	gint page = 0;
	DL_DATA *d = NULL;

	dl_list = g_list_first(dl_list);
	while (dl_list != NULL) {
		d = (DL_DATA *) dl_list->data;
		if (!strcmp(d->mod_name, modname))
			break;
		++page;
		dl_list = g_list_next(dl_list);
	}

	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebookDL), page);
	gtk_entry_set_text(GTK_ENTRY(d->entry), settings.dictkey);

	settings.dict_last_page = page;
}
/******************************************************************************
 * Name
 *  gui_set_dict_frame_label
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void gui_set_dict_frame_label(void)	
 *
 * Description
 *   sets dict/lex frame label to module name or null
 *
 * Return value
 *   void
 */

static void gui_set_dict_frame_label(DL_DATA *d)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */	
	if (settings.dict_tabs)
		gtk_frame_set_label(GTK_FRAME(d->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(d->frame), d->mod_name);
	
}

/******************************************************************************
 * Name
 *   on_notebook_dictlex_switch_page
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_notebook_dictlex_switch_page(GtkNotebook * notebook,
 *		GtkNotebookPage * page,	gint page_num, GList * dl_list)	
 *
 * Description
 *    change dictionary module
 *
 * Return value
 *   void
 */

static void on_notebook_dictlex_switch_page(GtkNotebook * notebook,
		GtkNotebookPage * page,	gint page_num, GList * dl_list)
{
	DL_DATA *d; //, *d_old;

	d = (DL_DATA *) g_list_nth_data(dl_list, page_num);
	
	if(!d->frame)
		gui_add_new_dict_pane(d);
	
	//-- change tab label to current book name
	cur_d = d;
	
	gui_set_dict_frame_label(d);
	
	sprintf(settings.DictWindowModule, "%s", d->mod_name);
	GTK_CHECK_MENU_ITEM(d->showtabs)->active = settings.dict_tabs;
	settings.dict_last_page = page_num;
	settings.html_dict = d->html;
}

/******************************************************************************
 * Name
 *   on_entryDictLookup_changed
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_entryDictLookup_changed(GtkEditable * editable,
						       DL_DATA * d)	
 *
 * Description
 *    look up text in dictionary entry widget
 *
 * Return value
 *   void
 */

void on_entryDictLookup_changed(GtkEditable * editable,
						       DL_DATA * d)
{
	gint count, i;
	gchar *key, *new_key, *text;
	static gboolean firsttime = TRUE;
	
	key = gtk_entry_get_text(GTK_ENTRY(d->entry));
	strcpy(settings.dictkey, key);	
	
	text = get_dictlex_text(d->mod_name, key);	
	entry_display(d->html, d->mod_name,
		   text, key, TRUE);
	free(text);
		
	if (firsttime)
		count = 7;
	else
		count =
		    GTK_CLIST(d->clist)->clist_window_height /
		    GTK_CLIST(d->clist)->row_height;
	
	if (count) {
		gtk_clist_clear(GTK_CLIST(d->clist));
		set_dictlex_module(d->mod_name);
		set_dictlex_key(key);
		new_key = get_dictlex_key(-1);
		
		for (i = 0; i < (count / 2); i++) {
			free(new_key);
			new_key = get_dictlex_key(0);
		}
		
		for (i = 0; i < count; i++) {
			free(new_key);			
			new_key = get_dictlex_key(1);
			gtk_clist_append(GTK_CLIST(d->clist),
						 &new_key);
		}
		free(new_key);		
	}
	
	firsttime = FALSE;
}

/******************************************************************************
 * Name
 *   on_clistDictLex_select_row
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_clistDictLex_select_row(GtkCList * clist, gint row,
 *			   gint column, GdkEvent * event, DL_DATA * d)
 *
 * Description
 *    display selected key
 *
 * Return value
 *   void
 */

void on_clistDictLex_select_row(GtkCList * clist, gint row,
			   gint column, GdkEvent * event, DL_DATA * d)
{
	gchar *text;

	gtk_clist_get_text(GTK_CLIST(GTK_WIDGET(clist)), row, column,
			   &text);
	gtk_entry_set_text(GTK_ENTRY(d->entry), text);

}

/******************************************************************************
 * Name
 *   on_copy_activate
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, DL_DATA * d)	
 *
 * Description
 *    copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem, DL_DATA * d)
{
	//gui_copy_html(d->html);
}

/******************************************************************************
 * Name
 *   on_find_activate
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_find_activate(GtkMenuItem * menuitem, DL_DATA * d)	
 *
 * Description
 *    open find dialog
 *
 * Return value
 *   void
 */

static void on_find_activate(GtkMenuItem * menuitem, DL_DATA * d)
{
	//searchGS_FIND_DLG(dl, FALSE, NULL);
}

/******************************************************************************
 * Name
 *  on_view_activate
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_view_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    display a different dictionary
 *
 * Return value
 *   void
 */

static void on_view_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gint page;

	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebookDL), page);
}

/******************************************************************************
 * Name
 *  on_showtabs_activate
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_showtabs_activate(GtkMenuItem *menuitem, gpointer user_data)
 *
 * Description
 *    hide/show dictlex notebook tabs
 *
 * Return value
 *   void
 */

static void on_showtabs_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	settings.dict_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebookDL),
			settings.dict_tabs);
}

/******************************************************************************
 * Name
 *  on_view_new_activate
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_view_new_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *    open view dictionary dialog
 *
 * Return value
 *   void
 */

static void on_view_new_activate(GtkMenuItem *menuitem, DL_DATA * d)
{
	gui_open_dictlex_dialog(d->mod_name);
}

/******************************************************************************
 * Name
 *  on_btnSyncDL_clicked
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_btnSyncDL_clicked(GtkButton * button, DL_DATA * d)	
 *
 * Description
 *    sync current module with current key
 *
 * Return value
 *   void
 */

void on_btnSyncDL_clicked(GtkButton * button, DL_DATA * d)
{
	gtk_entry_set_text(GTK_ENTRY(d->entry), settings.dictkey);
}

/******************************************************************************
 * Name
 *  on_unlock_key_activate
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   void on_unlock_key_activate(GtkMenuItem * menuitem, DL_DATA * d)	
 *
 * Description
 *    open cipher key dialog
 *
 * Return value
 *   void
 */

static void on_unlock_key_activate(GtkMenuItem * menuitem, DL_DATA * d)
{
	GtkWidget *dlg;
	
	dlg = gui_create_cipher_key_dialog(d->mod_name);
	gtk_widget_show(dlg);
}

/******************************************************************************
 * Name
 *   gui_create_dictlex_pm
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   GtkWidget *gui_create_dictlex_pm(DL_DATA * dl)	
 *
 * Description
 *   create a popup menu for the dict/lex pane 
 *
 * Return value
 *   GtkWidget*
 */

static GtkWidget *create_dictlex_pm(DL_DATA * dl)
{
	GtkWidget *pm;
	GtkAccelGroup *pm_accels;
	GtkWidget *copy;
	GtkWidget *separator;
	GtkWidget *item3;
	GtkWidget *view_new;
	GtkWidget *view;
	GtkWidget *view_menu;
	GtkAccelGroup *view_menu_accels;
	GtkWidget *find;
	GList *tmp;
	gint i;

	tmp = NULL;

	pm = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pm), "pm", pm);
	pm_accels = gtk_menu_ensure_uline_accel_group(GTK_MENU(pm));

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_ref(copy);
	gtk_object_set_data_full(GTK_OBJECT(pm), "copy", copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(pm), copy);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_ref(find);
	gtk_object_set_data_full(GTK_OBJECT(pm), "find", find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(pm), find);

	dl->showtabs =
	    gtk_check_menu_item_new_with_label(_("Show Tabs"));
	gtk_widget_ref(dl->showtabs);
	gtk_object_set_data_full(GTK_OBJECT(pm), "dl->showtabs",
				 dl->showtabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->showtabs);
	gtk_container_add(GTK_CONTAINER(pm), dl->showtabs);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm), "separator", separator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	view_new =
	    gtk_menu_item_new_with_label(_("View in new window"));
	gtk_widget_ref(view_new);
	gtk_object_set_data_full(GTK_OBJECT(pm), "view_new",
				 view_new,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(view_new);
	gtk_container_add(GTK_CONTAINER(pm), view_new);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pm), "separator", separator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pm), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	view = gtk_menu_item_new_with_label(_("View Dict/Lex"));
	gtk_widget_ref(view);
	gtk_object_set_data_full(GTK_OBJECT(pm), "view", view,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(view);
	gtk_container_add(GTK_CONTAINER(pm), view);

	view_menu = gtk_menu_new();
	gtk_widget_ref(view_menu);
	gtk_object_set_data_full(GTK_OBJECT(pm), "view_menu", view_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view), view_menu);
	view_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(view_menu));

	/* if module has cipher key include this item */
	if(dl->has_key) {
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
                      	dl);
	}

	i = 0;
	tmp = get_list(DICT_DESC_LIST);
	while (tmp != NULL) {
		item3 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pm), "item3",
					 item3, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item3);
		gtk_signal_connect(GTK_OBJECT(item3), "activate",
				   GTK_SIGNAL_FUNC
				   (on_view_activate),
				   GINT_TO_POINTER(i));
		gtk_container_add(GTK_CONTAINER(view_menu), item3);
		++i;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			   GTK_SIGNAL_FUNC(on_copy_activate), dl);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			   GTK_SIGNAL_FUNC(on_find_activate), dl);
	gtk_signal_connect(GTK_OBJECT(dl->showtabs), "activate",
			   GTK_SIGNAL_FUNC(on_showtabs_activate),
			   &settings);
	gtk_signal_connect(GTK_OBJECT(view_new), "activate",
			   GTK_SIGNAL_FUNC(on_view_new_activate),
			   dl);
	return pm;
}

/******************************************************************************
 * Name
 *  html_button_pressed
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * d)	
 *
 * Description
 *    mouse button pressed in dictionary / lexicon 
 *
 * Return value
 *   gint
 */ 

static gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
					DL_DATA * dl)
{	
	settings.whichwindow = DICTIONARY_WINDOW;
	
	
	switch (event->button) {
	case 1:
		return TRUE;
		break;
	case 2:
		/* 
		 * pass this for pasting 
		 */
		return TRUE;
		break;
	case 3:
		/*gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
					     "button_press_event");*/
		return TRUE;
		break;
	default:
	}

	return FALSE;
}

/******************************************************************************
 * Name
 *  gui_create_dictlex_pane
 *
 * Synopsis
 *   #include "_dictlex.h"
 *
 *   GtkWidget *gui_create_dictlex_pane(DL_DATA *dl, gint count)	
 *
 * Description
 *    create a pane for displaying a dictionary of lexicom
 *
 * Return value
 *   void
 */

static void create_dictlex_pane(DL_DATA *dl)
{

	GtkWidget *hpaned7;
	GtkWidget *vbox56;
	GtkWidget *toolbarDLKey;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSyncDL;
	GtkWidget *label205;
	GtkWidget *frameDictHTML;
	GtkWidget *scrolledwindowDictHTML;

	dl->frame = gtk_frame_new(NULL);
	gtk_widget_ref(dl->frame);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "dl->frame",
				 dl->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->frame);
	gtk_container_add(GTK_CONTAINER(dl->vbox), dl->frame);

	hpaned7 = gtk_hpaned_new();
	gtk_widget_ref(hpaned7);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "hpaned7",
				 hpaned7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned7);
	gtk_container_add(GTK_CONTAINER(dl->frame), hpaned7);
	gtk_paned_set_position(GTK_PANED(hpaned7), 190);

	vbox56 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox56);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox56",
				 vbox56,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox56);
	gtk_paned_pack1(GTK_PANED(hpaned7), vbox56, FALSE, TRUE);


	toolbarDLKey =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarDLKey);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "toolbarDLKey",
				 toolbarDLKey,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarDLKey);
	gtk_box_pack_start(GTK_BOX(vbox56), toolbarDLKey, FALSE, TRUE,
			   0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarDLKey),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_REFRESH);
	btnSyncDL =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarDLKey),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"), _("Load current key"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnSyncDL);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnSyncDL",
				 btnSyncDL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSyncDL);




	dl->entry = gtk_entry_new();
	gtk_widget_ref(dl->entry);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "dl->entry",
				 dl->entry,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->entry);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarDLKey), dl->entry,
				  NULL, NULL);

	dl->clist = gtk_clist_new(1);
	gtk_widget_ref(dl->clist);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "dl->clist",
				 dl->clist,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->clist);
	gtk_box_pack_start(GTK_BOX(vbox56), dl->clist, TRUE, TRUE, 0);
	gtk_clist_set_column_width(GTK_CLIST(dl->clist), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(dl->clist));

	label205 = gtk_label_new(_("label205"));
	gtk_widget_ref(label205);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label205",
				 label205,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label205);
	gtk_clist_set_column_widget(GTK_CLIST(dl->clist), 0, label205);

	frameDictHTML = gtk_frame_new(NULL);
	gtk_widget_ref(frameDictHTML);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "frameDictHTML", frameDictHTML,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameDictHTML);
	gtk_paned_pack2(GTK_PANED(hpaned7), frameDictHTML, TRUE, TRUE);

	scrolledwindowDictHTML = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowDictHTML);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindowDictHTML",
				 scrolledwindowDictHTML,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindowDictHTML);
	gtk_container_add(GTK_CONTAINER(frameDictHTML),
			  scrolledwindowDictHTML);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowDictHTML),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);


	dl->html = gtk_html_new();
	gtk_widget_ref(dl->html);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "dl->html", dl->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowDictHTML),
			  dl->html);
	gtk_html_load_empty(GTK_HTML(dl->html));

	gtk_signal_connect(GTK_OBJECT(dl->html),
			"button_press_event",
			GTK_SIGNAL_FUNC(html_button_pressed), 
			dl);
	gtk_signal_connect(GTK_OBJECT(btnSyncDL), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSyncDL_clicked), dl);
	gtk_signal_connect(GTK_OBJECT(dl->html), "link_clicked",
			   GTK_SIGNAL_FUNC(gui_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(dl->html), "on_url",
			   GTK_SIGNAL_FUNC(gui_url), (gpointer) settings.app);
	gtk_signal_connect(GTK_OBJECT(dl->entry), "changed",
			   GTK_SIGNAL_FUNC(on_entryDictLookup_changed),
			   dl);
	gtk_signal_connect(GTK_OBJECT(dl->clist), "select_row",
			   GTK_SIGNAL_FUNC(on_clistDictLex_select_row),
			   dl);
}
/******************************************************************************
 * Name
 *  gui_add_new_dict_pane
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void gui_add_new_dict_pane(DL_DATA *dl)
 *
 * Description
 *   creates a dictlex pane when user selects a new dictlex module
 *
 * Return value
 *   void
 */

void gui_add_new_dict_pane(DL_DATA *dl)
{	
	GtkWidget *popup;
	
	create_dictlex_pane(dl);
	popup = create_dictlex_pm(dl);
	gnome_popup_menu_attach(popup, dl->html, NULL);
}

/******************************************************************************
 * Name
 *  add_vbox_to_notebook
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void add_vbox_to_notebook(TEXT_DATA * t)
 *
 * Description
 *   adds a vbox and label to the dictlex notebook for each dictlex module
 *
 * Return value
 *   void
 */

static void add_vbox_to_notebook(DL_DATA *dl)
{	
	GtkWidget *label;
	
	dl->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(dl->vbox);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), 
			"dl->vbox", dl->vbox,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->vbox);
	gtk_container_add(GTK_CONTAINER(settings.notebookDL), dl->vbox);	
	
	label = gtk_label_new(dl->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.notebookDL),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(settings.notebookDL), dl->mod_num),
				   label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.notebookDL),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK(settings.notebookDL),
					  dl->mod_num),
					 (gchar *) dl->mod_name);

	
}

/******************************************************************************
 * Name
 *   gui_setup_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void gui_setup_dictlex(GList *mods)
 *
 * Description
 *   setup dictlex support 
 *
 * Return value
 *  void
 */

void gui_setup_dictlex(GList *mods)
{
	GList *tmp = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	DL_DATA *dl;
	gint count = 0;

	dl_list = NULL;

	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		dl = g_new(DL_DATA, 1);
		dl->frame = NULL;
		dl->mod_num = count;
		dl->mod_name = modname;
		dl->search_string = NULL;
		dl->find_dialog = NULL;	
		dl->has_key = module_is_locked(dl->mod_name);
		add_vbox_to_notebook(dl);
		dl_list = g_list_append(dl_list, (DL_DATA *) dl);
		++count;
		tmp = g_list_next(tmp);
	}


	gtk_signal_connect(GTK_OBJECT(settings.notebookDL), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook_dictlex_switch_page),
			   dl_list);

	modbuf = g_strdup(settings.DictWindowModule);
	keybuf = g_strdup(settings.dictkey);

	set_page_dictlex(modbuf, dl_list);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
	settings.dict_last_page = 0;
}

/******************************************************************************
 * Name
 *   gui_shutdown_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void gui_shutdown_dictlex(void)
 *
 * Description
 *    shutdown and cleanup dictlex support
 *
 * Return value
 *   void
 */

void gui_shutdown_dictlex(void)
{
	dl_list = g_list_first(dl_list);
	while (dl_list != NULL) {
		DL_DATA *d = (DL_DATA *) dl_list->data;
		/* free any search dialogs created */
		if (d->find_dialog)
			g_free(d->find_dialog);
		g_free((DL_DATA *) dl_list->data);
		dl_list = g_list_next(dl_list);
	}
	g_list_free(dl_list);

}

//******  end of file  ******/
