/*
 * GnomeSword Bible Study Tool
 * _commentary.c - gui for commentary modules
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

/*
 * gnome
 */
#include "_commentary.h"
#include "cipher_key_dialog.h"
/*
 * main
 */
#include "commentary.h"
#include "gs_shortcutbar.h"
#include "gs_html.h"
#include "gs_viewcomm_dlg.h"


/******************************************************************************
 * externs
 */
 
extern gboolean isrunningVC;
extern COMM_DATA *cur_c;
extern gboolean comm_display_change;
/******************************************************************************
 * global to this file only 
 */



void on_notebook_comm_switch_page(GtkNotebook * notebook,
				 GtkNotebookPage * page,
				 gint page_num, GList * cl)
{
	COMM_DATA *c;
	
	c = (COMM_DATA *) g_list_nth_data(cl, page_num);
	cur_c = c;
	strcpy(settings->CommWindowModule, c->modName);
	/*
	 * set settings->comm_key to current module key
	 */
	if(c->key)
		strcpy(settings->comm_key,c->key);
	settings->commLastPage = page_num;
	if(comm_display_change) {
		if ((c->key[0] == '\0') && (settings->currentverse != NULL)) {
			set_commentary_page_and_key(c->modnum,
					      settings->currentverse);
			strcpy(settings->comm_key,settings->currentverse);
			strcpy(c->key, settings->comm_key);
		}
	}
	GTK_CHECK_MENU_ITEM(c->showtabs)->active = settings->comm_tabs;
}




static
void on_copy_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	copyGS_HTML(c->html);
}

static
void on_find_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	//searchGS_FIND_DLG(c, FALSE, NULL);
}

static
void on_lookup_selection_activate(GtkMenuItem * menuitem,
				  gchar * dict_mod_description)
{
	gchar *dict_key, *dict_mod;

	dict_mod = get_module_name_from_description(dict_mod_description);
	dict_key = get_word_or_selection(cur_c->html, FALSE);
	if (dict_key) {
		display_dictlex_in_viewer(dict_mod, dict_key, settings);
		g_free(dict_key);
		g_free(dict_mod);
	}
}

static
void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
				       COMM_DATA * c)
{
	gchar *key = get_word_or_selection(c->html, FALSE);
	if (key) {
		display_dictlex_in_viewer(settings->DictWindowModule, 
						key,
						settings);
		g_free(key);
	}
}

static 
void on_view_mod_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gint page;

	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebook_comm),
			      page);
}

static 
void on_comm_showtabs_activate(GtkMenuItem * menuitem,
				      SETTINGS * s)
{
	s->comm_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_comm),
				   s->comm_tabs);
}

static 
void on_view_new_activate(GtkMenuItem * menuitem, SETTINGS * s)
{
	static GtkWidget *dlg;
        GdkCursor *cursor;	
	
	gtk_widget_show(s->app);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(s->app->window,cursor);
	
	if(!isrunningVC) {
		dlg = create_dlgViewComm(s);
		isrunningVC = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(s->app);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(s->app->window,cursor);

}

static 
void on_unlock_key_activate(GtkMenuItem * menuitem, COMM_DATA * c)
{
	GtkWidget *dlg;
	
	dlg = gui_create_cipher_key_dialog(c->modName);
	gtk_widget_show(dlg);
}
GtkWidget *gui_create_pm(COMM_DATA * c)
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
	   if module has cipher key include this item
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
	tmp = mod_lists->dict_descriptions; //backend_get_mod_description_list_SWORD(DICT_MODS);
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
	tmp = mod_lists->comm_descriptions;
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
    /*** for using the current dictionary for lookup ***/
	gtk_signal_connect(GTK_OBJECT(usecurrent), "activate",
			   GTK_SIGNAL_FUNC
			   (on_same_lookup_selection_activate), c);

	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			   GTK_SIGNAL_FUNC(on_copy_activate), c);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			   GTK_SIGNAL_FUNC(on_find_activate), c);
	gtk_signal_connect(GTK_OBJECT(c->showtabs), "activate",
			   GTK_SIGNAL_FUNC
			   (on_comm_showtabs_activate), settings);
	gtk_signal_connect(GTK_OBJECT(view_new), "activate",
			   GTK_SIGNAL_FUNC
			   (on_view_new_activate), settings);
	return pm;
}




static 
void on_btn_sync_clicked(GtkButton * button, COMM_DATA * c)
{
	set_commentary_page_and_key(c->modnum, settings->currentverse);
}

static 
void on_btn_back_clicked(GtkButton * button, COMM_DATA * c)
{
	const gchar *key = navigate_commentary(c->modnum, 0);
	if(key) {
		strcpy(settings->comm_key,key);
		strcpy(cur_c->key,settings->comm_key);
	}
}

static 
void on_btn_forward_clicked(GtkButton * button, COMM_DATA * c)
{
	const gchar *key = navigate_commentary(c->modnum, 1);
	if(key) {
		strcpy(settings->comm_key,key);
		strcpy(cur_c->key,settings->comm_key);
	}
}

static 
void on_btn_print_clicked(GtkButton * button, COMM_DATA * c)
{
	html_print(c->html);
}

static 
gboolean on_button_release_event(GtkWidget * widget,
			GdkEventButton * event, COMM_DATA * c)
{
	extern gboolean in_url;
	gchar *key;
	
	settings->whichwindow = COMMENTARY_WINDOW;
	
	switch (event->button) {
	case 1:
		if (!in_url) {
			key = buttonpresslookupGS_HTML(c->html);
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


void gui_create_commentary_pane(SETTINGS * s, COMM_DATA * c,
							gint count)
{
	GtkWidget *frameCOMM;
	GtkWidget *vbox57;
	GtkWidget *frame;
	GtkWidget *toolbarCOMM;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator19;
	GtkWidget *scrolledwindowCOMMhtml;
	GtkWidget *label;


	frameCOMM = gtk_frame_new(NULL);
	gtk_widget_ref(frameCOMM);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frameCOMM",
				 frameCOMM, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frameCOMM);
	gtk_container_add(GTK_CONTAINER(s->notebook_comm), frameCOMM);

	vbox57 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox57);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox57",
				 vbox57, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox57);
	gtk_container_add(GTK_CONTAINER(frameCOMM), vbox57);

	frame = gtk_frame_new(NULL);
	gtk_widget_ref(frame);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame", frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame);
	gtk_box_pack_start(GTK_BOX(vbox57), frame, FALSE, TRUE, 0);
	
	toolbarCOMM =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarCOMM);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbarCOMM",
				 toolbarCOMM, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(toolbarCOMM);
	gtk_container_add(GTK_CONTAINER(frame), toolbarCOMM);	
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarCOMM),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_REFRESH);
	c->btnCOMMSync =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarCOMM),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"),
				       _("Sync with Bible Text"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(c->btnCOMMSync);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "c->btnCOMMSync", c->btnCOMMSync,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btnCOMMSync);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_BACK);
	c->btnCOMMBack =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarCOMM),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Back"),
				       _("Go to previous comment"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(c->btnCOMMBack);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "c->btnCOMMBack", c->btnCOMMBack,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btnCOMMBack);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_FORWARD);
	c->btnCOMMForward =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarCOMM),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Forward"),
				       _("Go to next comment"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(c->btnCOMMForward);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "c->btnCOMMForward",
				 c->btnCOMMForward, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btnCOMMForward);

	vseparator19 = gtk_vseparator_new();
	gtk_widget_ref(vseparator19);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "vseparator19", vseparator19,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vseparator19);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarCOMM),
				  vseparator19, NULL, NULL);
	gtk_widget_set_usize(vseparator19, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_PRINT);
	c->btnCOMMPrint =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarCOMM),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"),
				       _("Print Comment"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(c->btnCOMMPrint);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "c->btnCOMMPrint", c->btnCOMMPrint,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->btnCOMMPrint);

	scrolledwindowCOMMhtml = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowCOMMhtml);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
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
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "c->html", c->html, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowCOMMhtml),
			  c->html);
	gtk_html_load_empty(GTK_HTML(c->html));

	label = gtk_label_new(c->modName);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label",
				 label, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->notebook_comm),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(s->notebook_comm),
				    count), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (s->notebook_comm),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (s->notebook_comm),
					  count), (gchar *) c->modName);



	gtk_signal_connect(GTK_OBJECT(c->html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(c->html), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) s->app);
	gtk_signal_connect(GTK_OBJECT(c->html),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (on_button_release_event), (COMM_DATA *) c);


	gtk_signal_connect(GTK_OBJECT(c->btnCOMMSync), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_sync_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMBack), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_back_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMForward),
			   "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btn_forward_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMPrint), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btn_print_clicked), (COMM_DATA *) c);

}




//******  end of file  ******/
