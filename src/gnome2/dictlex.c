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
#include "gui/dictlex_menu.h"
#include "gui/gnomesword.h"
#include "gui/cipher_key_dialog.h"
#include "gui/shortcutbar_main.h"
#include "gui/dictlex_dialog.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/sidebar.h"
#include "gui/find_dialog.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/dictlex.h"
#include "main/xml.h"

/******************************************************************************
 * externs
 */
extern gboolean dict_display_change;
extern gboolean isrunningSD;	/* is the view dictionary dialog runing */


/******************************************************************************
 * global to this file only 
 */
static void on_notebook_dictlex_switch_page(GtkNotebook * notebook,
					    GtkNotebookPage * page,
					    gint page_num,
					    GList * dl_list);
static GList *dl_list;
static DL_DATA *cur_d;
static gint number_of_dictionaries;
static gint cell_height;
static gint dict_last_page;


/******************************************************************************
 * Name
 *  on_lookup_selection_activate
 *
 * Synopsis
 *   #include "gui/dictlex.h"
 *   void gui_lookup_dict_selection_activate(GtkMenuItem * menuitem,
 *				  gchar * modDescription)   	
 *
 * Description
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */

void gui_lookup_dictlex_selection(GtkMenuItem * menuitem,
				  gchar * dict_mod_description)
{
	gchar *dict_key;
	gchar *mod_name = NULL;

	mod_name = module_name_from_description(dict_mod_description);
	if(!mod_name) 
		return;

	dict_key = gui_get_word_or_selection(cur_d->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(mod_name,
						       dict_key);
		if (settings.inDictpane)
			gui_change_module_and_key(mod_name, dict_key);
		g_free(dict_key);
	}
	if (mod_name)
		g_free(mod_name);
}

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
	gui_set_dictionary_page_and_key(dict_last_page, key);
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
	DL_DATA *d = NULL;

	d = (DL_DATA *) g_list_nth_data(dl_list, page_num);
	if(d) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(
				widgets.notebook_dict), page_num);
		gtk_entry_set_text(GTK_ENTRY(d->entry), key);
	}
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
	if(modname) {
		while (dl_list != NULL) {
			d = (DL_DATA *) dl_list->data;
			if (!strcmp(d->mod_name, modname))
				break;
			++page;
			dl_list = g_list_next(dl_list);
		}
	}
	else {
		d = (DL_DATA *) dl_list->data;
		page = 0;
	}
	
	cur_d = d;
	
	if (page)
		gtk_notebook_set_current_page(GTK_NOTEBOOK
				      (widgets.notebook_dict), page);
	else
		on_notebook_dictlex_switch_page(GTK_NOTEBOOK
						(widgets.notebook_dict),
						NULL, page, dl_list);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_dict),
			      page);
	g_print("dictkey = %s\n", settings.dictkey);

	if (d)
		gtk_entry_set_text(GTK_ENTRY(d->entry),
				   settings.dictkey);

	dict_last_page = page;
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

void gui_set_dict_frame_label()
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module namecur_
	 */
	if (settings.dict_tabs)
		gtk_frame_set_label(GTK_FRAME(cur_d->frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(cur_d->frame),
				    cur_d->mod_name);

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

void on_notebook_dictlex_switch_page(GtkNotebook * notebook,
				     GtkNotebookPage * page,
				     gint page_num, GList * dl_list)
{
	DL_DATA *d;		//, *d_old;

	d = (DL_DATA *) g_list_nth_data(dl_list, page_num);

	if (!d->frame)
		gui_add_new_dict_pane(d);


	//-- change tab label to current book name
	cur_d = d;
	gui_change_window_title(d->mod_name);
	/*
	 * set search module to current dict/lex module 
	 */
//      strcpy(settings.sb_search_mod, d->mod_name);
	/*
	 * set search frame label to current dict/lex module 
	 */
//      gui_set_search_label();

	gui_set_dict_frame_label();

	settings.DictWindowModule = d->mod_name;
	xml_set_value("GnomeSword", "modules", "dict", d->mod_name);

	GTK_CHECK_MENU_ITEM(d->showtabs)->active = settings.dict_tabs;
	dict_last_page = page_num;
	widgets.html_dict = d->html;
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

void on_entryDictLookup_changed(GtkEditable * editable, DL_DATA * d)
{
	gint count = 7, i;
	const gchar *key;
	gchar *new_key, *text = NULL;
	static gboolean firsttime = TRUE;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	gint height;
	
	key = gtk_entry_get_text(GTK_ENTRY(d->entry));
	d->key = g_strdup(key);
	//xml_set_value("GnomeSword", "key", "dictionary", d->key);
	
	text = get_dictlex_text(d->mod_name, d->key);
	d->key = get_key_from_module(2, d->mod_name);
	settings.dictkey = d->key;
	xml_set_value("GnomeSword", "key", "dictionary", d->key);
	if (text) {
		entry_display(d->html, d->mod_name, text, d->key, TRUE);
		free(text);
	}
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(d->listview));
	list_store = GTK_LIST_STORE(model);
	
	if (!firsttime) {
		gdk_drawable_get_size ((GdkDrawable *)d->listview->window,
                                             NULL,
                                             &height);
		count = height / cell_height;
	}	 

	if (count) {
		gtk_list_store_clear(list_store);
		new_key = get_dictlex_key(2, d->mod_name, -1);

		for (i = 0; i < (count / 2); i++) {
			free(new_key);
			new_key = get_dictlex_key(2, d->mod_name, 0);
		}

		for (i = 0; i < count; i++) {
			free(new_key);
			new_key = get_dictlex_key(2, d->mod_name, 1);
			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, 0,
					   new_key, -1);
		}
		free(new_key);
	}
	firsttime = FALSE;
}


/******************************************************************************
 * Name
 *  on_btnSyncDL_clicked
 *
 * Synopsis
 *   #include "gui/dictlex.h"
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
 *  html_button_pressed
 *
 * Synopsis
 *   #include "gui/dictlex.h"
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

static gint html_button_pressed(GtkWidget * html,
				GdkEventButton * event, DL_DATA * dl)
{
	settings.whichwindow = DICTIONARY_WINDOW;

	gui_change_window_title(dl->mod_name);

	switch (event->button) {
	case 1:

		break;
	case 2:
		/* 
		 * pass this for pasting 
		 */
		break;
	case 3:
		/*gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
		   "button_press_event"); */
		break;
	default:
		break;
	}

	return FALSE;
}

/******************************************************************************
 * Name
 *  list_button_released
 *
 * Synopsis
 *   #include "gui/dictlex.h"
 *
 *   gint list_button_released(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * d)	
 *
 * Description
 *    mouse button released in key list
 *
 * Return value
 *   gint
 */

static gint list_button_released(GtkWidget * html,
				GdkEventButton * event, DL_DATA * d)
{
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	gchar *buf = NULL;
	GtkTreeModel *model ;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(d->listview));
                        
	if (!gtk_tree_selection_get_selected(selection, &model, &selected))
		return;
	
	switch (event->button) {
	case 1:
		gtk_tree_model_get(model, &selected, 0, &buf, -1);
		if (buf) {
			gtk_entry_set_text(GTK_ENTRY(d->entry), buf);
			g_free(buf);
		}	 	
		break;
	case 2:
	case 3:
	default:
		break;
	}
	
	return FALSE;
}


static void add_columns(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);

	/* column for fixed toggles */
	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Keys",
							  renderer,
							  "text", 0,
							  NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);

	gtk_tree_view_append_column(treeview, column);
	/* get cell (row) height */
	gtk_cell_renderer_get_size(renderer,
                                   GTK_WIDGET(treeview),
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &cell_height);
}

/******************************************************************************
 * Name
 *  gui_create_dictlex_pane
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   GtkWidget *gui_create_dictlex_pane(DL_DATA *dl, gint count)	
 *
 * Description
 *    create a pane for displaying a dictionary or lexicom
 *
 * Return value
 *   void
 */

static void create_dictlex_pane(DL_DATA * dl)
{

	GtkWidget *hpaned7;
	GtkWidget *vbox56;
	GtkWidget *hbox;
	GtkWidget *toolbarDLKey;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSyncDL;
	GtkWidget *label205;
	GtkWidget *frameDictHTML;
	GtkWidget *scrolledwindowDictHTML;
	GtkListStore *model;
	
	dl->frame = gtk_frame_new(NULL);
	gtk_widget_show(dl->frame);
	gtk_container_add(GTK_CONTAINER(dl->vbox), dl->frame);
	gtk_frame_set_shadow_type(GTK_FRAME(dl->frame),GTK_SHADOW_NONE);

	hpaned7 = gtk_hpaned_new();
	gtk_widget_show(hpaned7);
	gtk_container_add(GTK_CONTAINER(dl->frame), hpaned7);
	gtk_paned_set_position(GTK_PANED(hpaned7), 195);

	vbox56 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox56);
	gtk_paned_pack1(GTK_PANED(hpaned7), vbox56, TRUE, TRUE);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox56), hbox, FALSE, TRUE, 0);

	btnSyncDL = gtk_button_new ();
	gtk_widget_show (btnSyncDL);
	gtk_box_pack_start (GTK_BOX (hbox), btnSyncDL, FALSE, TRUE, 0);
	
	tmp_toolbar_icon = gtk_image_new_from_stock (GTK_STOCK_REFRESH, 
						GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (tmp_toolbar_icon);
	gtk_container_add (GTK_CONTAINER (btnSyncDL), tmp_toolbar_icon);
	
	dl->entry = gtk_entry_new();
	gtk_widget_show(dl->entry);
	gtk_box_pack_start (GTK_BOX (hbox), dl->entry, TRUE, TRUE, 0);
	
	/* create tree model */
	model = gtk_list_store_new(1, G_TYPE_STRING);

	/* create tree view */
	dl->listview =
	    gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	gtk_widget_show(dl->listview);
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(dl->listview), TRUE);
	gtk_box_pack_start(GTK_BOX(vbox56), dl->listview, TRUE, TRUE,
			   0);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(dl->listview),
					  FALSE);
	add_columns(GTK_TREE_VIEW(dl->listview));
		     
	frameDictHTML = gtk_frame_new(NULL);
	gtk_widget_show(frameDictHTML);
	gtk_paned_pack2(GTK_PANED(hpaned7), frameDictHTML, TRUE, TRUE);

	scrolledwindowDictHTML = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindowDictHTML);
	gtk_container_add(GTK_CONTAINER(frameDictHTML),
			  scrolledwindowDictHTML);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowDictHTML),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	dl->html = gtk_html_new();
	gtk_widget_show(dl->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowDictHTML),
			  dl->html);
	gtk_html_load_empty(GTK_HTML(dl->html));

	g_signal_connect(GTK_OBJECT(dl->html),
			   "button_press_event",
			   G_CALLBACK(html_button_pressed), dl);
	g_signal_connect(GTK_OBJECT(dl->html),
			   "url_requested",
			   G_CALLBACK(url_requested), NULL);
	g_signal_connect(GTK_OBJECT(dl->html), "on_url",
			   G_CALLBACK(gui_url), 
			   GINT_TO_POINTER(DICTIONARY_TYPE));
	g_signal_connect(GTK_OBJECT(dl->html), "link_clicked",
			   G_CALLBACK(gui_link_clicked), NULL);


	g_signal_connect(GTK_OBJECT(btnSyncDL), "clicked",
			   G_CALLBACK(on_btnSyncDL_clicked), dl);
	g_signal_connect(GTK_OBJECT(dl->entry), "changed",
			   G_CALLBACK(on_entryDictLookup_changed), dl);
	g_signal_connect(G_OBJECT(dl->listview),
			   "button_release_event",
			   G_CALLBACK(list_button_released), dl);
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

void gui_add_new_dict_pane(DL_DATA * dl)
{
	GtkWidget *popup;

	create_dictlex_pane(dl);
	popup = gui_create_pm_dict(dl);	//create_dictlex_pm(dl);
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

static void add_vbox_to_notebook(DL_DATA * dl)
{
	GtkWidget *label;

	dl->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(dl->vbox);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_dict),
			  dl->vbox);

	label = gtk_label_new(dl->mod_name);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(widgets.notebook_dict),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(widgets.notebook_dict),
				    dl->mod_num), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.notebook_dict),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.notebook_dict),
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

gint gui_setup_dictlex(GList * mods)
{
	GList *tmp = NULL;
	gchar *modname;
	gchar *modbuf;
	DL_DATA *dl;
	gint count = 0;

	dl_list = NULL;

	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		dl = g_new0(DL_DATA, 1);
		dl->frame = NULL;
		dl->mod_num = count;
		dl->mod_name = modname;	
		dl->search_string = NULL;
		dl->key = NULL;
		dl->cipher_key = NULL;
		dl->is_dialog = FALSE;
		dl->has_key = module_is_locked(dl->mod_name);
		if (has_cipher_tag(dl->mod_name)) {
			dl->is_locked = module_is_locked(dl->mod_name);
			dl->cipher_old = get_cipher_key(dl->mod_name);
		}

		else {

			dl->is_locked = 0;
			dl->cipher_old = NULL;
		}
		dl->is_rtol = is_module_rtl(dl->mod_name);
		add_vbox_to_notebook(dl);
		dl_list = g_list_append(dl_list, (DL_DATA *) dl);
		++count;
		tmp = g_list_next(tmp);
	}


	g_signal_connect(GTK_OBJECT(widgets.notebook_dict),
			   "switch_page",
			   G_CALLBACK
			   (on_notebook_dictlex_switch_page), dl_list);

	modbuf = g_strdup(settings.DictWindowModule);
	if(modbuf) {
		if(check_for_module(modbuf))
			set_page_dictlex(modbuf, dl_list);
		else if(check_for_module(dl->mod_name))
			set_page_dictlex(dl->mod_name, dl_list);
	}
	else 
		if(check_for_module(dl->mod_name))
			set_page_dictlex(dl->mod_name, dl_list);
	g_free(modbuf);
	g_list_free(tmp);
	number_of_dictionaries = count;
	return count;
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
		if (d->key)
			g_free(d->key);
		//if(d->mod_name) g_free(d->mod_name);
		g_free(d);
		dl_list = g_list_next(dl_list);
	}
	g_list_free(dl_list);

}

//******  end of file  ******/
