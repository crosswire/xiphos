/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_commentary.c (commentary support)
     * -------------------
     * Fri Apr 12 11:48:29 2002
     * copyright (C) 2002 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>


#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_html.h"
#include "sword.h"
#include "gs_commentary.h"
#include "commentary.h"
#include "shortcutbar.h"
#include "gs_viewcomm_dlg.h"
//#include <gtkhtml/gtkhtml.h>

extern SETTINGS *settings;
extern gboolean isrunningVC;  

GList *comm_list;

static
COMM_DATA *getCOMM(GList * cl)
{
	GList *tmp;
	COMM_DATA *c = NULL;

	tmp = NULL;
	tmp = cl;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		c = (COMM_DATA *) tmp->data;
		if (!strcmp(c->modName, settings->CommWindowModule)) {
			break;
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return c;
}

static
void on_notebookCOMM_switch_page(GtkNotebook * notebook,
				 GtkNotebookPage * page,
				 gint page_num, GList * comm_list)
{
	COMM_DATA *c, *c_old;
	// c_old = (COMM_DATA *) g_list_nth_data(comm_list, settings->gbsLastPage);
	c = (COMM_DATA *) g_list_nth_data(comm_list, page_num);

	sprintf(settings->CommWindowModule, "%s", c->modName);
/*
    if (settings->finddialog) {
	gnome_dialog_close(g_old->find_dialog->dialog);
	searchGS_FIND_DLG(g, FALSE, settings->findText);
    }
    */
	settings->commLastPage = page_num;
	if ((c->key == NULL) && (settings->currentverse != NULL))
		backend_displayinCOMM(c->modnum,
				      settings->currentverse);
	GTK_CHECK_MENU_ITEM(c->showtabs)->active = settings->comm_tabs;
}


static void setPageCOMM(gchar * modname, GList * comm_list,
			SETTINGS * s)
{
	gint page = 0;
	COMM_DATA *c = NULL;

	comm_list = g_list_first(comm_list);
	while (comm_list != NULL) {
		c = (COMM_DATA *) comm_list->data;
		if (!strcmp(c->modName, modname))
			break;
		++page;
		comm_list = g_list_next(comm_list);
	}

	gtk_notebook_set_page(GTK_NOTEBOOK(s->notebookCOMM), page);
	s->commLastPage = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookCOMM),
				   s->comm_tabs);
}/****  popup menu call backs  ****/

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
				  gchar * modDescription)
{
	COMM_DATA *c;
	gchar modName[16];
	gchar *key;

	c = getCOMM(comm_list);
	memset(modName, 0, 16);
	modNameFromDesc(modName, modDescription);
	key = lookupGS_HTML(c->html, FALSE);
	if (key) {
		display_dictlex_in_viewer(modName, key, settings);
		g_free(key);
	}
}

static
void on_same_lookup_selection_activate(GtkMenuItem * menuitem,
				       COMM_DATA * c)
{
	gchar *key = lookupGS_HTML(c->html, FALSE);
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
	gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebookCOMM),
			      page);
}

static void on_comm_showtabs_activate(GtkMenuItem * menuitem,
				      SETTINGS * s)
{
	s->comm_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookCOMM),
				   s->comm_tabs);
}

static void on_view_new_activate(GtkMenuItem * menuitem, SETTINGS * s)
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
GtkWidget *create_pmCOMM(COMM_DATA * c)
{
	GtkWidget *pmCOMM;
	GtkAccelGroup *pmCOMM_accels;
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

	pmCOMM = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmCOMM), "pmCOMM", pmCOMM);
	pmCOMM_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pmCOMM));

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_ref(copy);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM), "copy", copy,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(pmCOMM), copy);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_ref(find);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM), "find", find,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(pmCOMM), find);

	c->showtabs =
	    gtk_check_menu_item_new_with_label(_("Show Tabs"));
	gtk_widget_ref(c->showtabs);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM), "c->showtabs",
				 c->showtabs, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(c->showtabs);
	gtk_container_add(GTK_CONTAINER(pmCOMM), c->showtabs);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pmCOMM), separator);
	gtk_widget_set_sensitive(separator, FALSE);

	view_new = gtk_menu_item_new_with_label("View in new window");
	gtk_widget_ref(view_new);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM), "view_new",
				 view_new, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(view_new);
	gtk_container_add(GTK_CONTAINER(pmCOMM), view_new);

	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pmCOMM), separator);
	gtk_widget_set_sensitive(separator, FALSE);


	lookup_selection =
	    gtk_menu_item_new_with_label(_("Lookup Selection"));
	gtk_widget_ref(lookup_selection);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM),
				 "lookup_selection", lookup_selection,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(lookup_selection);
	gtk_container_add(GTK_CONTAINER(pmCOMM), lookup_selection);

	lookup_selection_menu = gtk_menu_new();
	gtk_widget_ref(lookup_selection_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM),
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
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM), "usecurrent",
				 usecurrent, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);


	separator = gtk_menu_item_new();
	gtk_widget_ref(separator);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM), "separator",
				 separator, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(pmCOMM), separator);
	gtk_widget_set_sensitive(separator, FALSE);


	view_commentary =
	    gtk_menu_item_new_with_label(_("View Commentary"));
	gtk_widget_ref(view_commentary);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM),
				 "view_commentary", view_commentary,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(view_commentary);
	gtk_container_add(GTK_CONTAINER(pmCOMM), view_commentary);

	view_commentary_menu = gtk_menu_new();
	gtk_widget_ref(view_commentary_menu);
	gtk_object_set_data_full(GTK_OBJECT(pmCOMM),
				 "view_commentary_menu",
				 view_commentary_menu,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_commentary),
				  view_commentary_menu);
	view_commentary_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (view_commentary_menu));

	tmp = backend_get_mod_description_list_SWORD(DICT_MODS);
	while (tmp != NULL) {
		item4 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item4);
		gtk_object_set_data_full(GTK_OBJECT(pmCOMM),
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
	tmp = backend_get_mod_description_list_SWORD(COMM_MODS);
	while (tmp != NULL) {
		item3 =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_ref(item3);
		gtk_object_set_data_full(GTK_OBJECT(pmCOMM),
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
	return pmCOMM;
}




static void on_btnCOMMSync_clicked(GtkButton * button, COMM_DATA * c)
{
	backend_displayinCOMM(c->modnum, settings->currentverse);
}

static void on_btnCOMMBack_clicked(GtkButton * button, COMM_DATA * c)
{
	backend_nav_commentary_COMM(c->modnum, 0);
}

static void on_btnCOMMForward_clicked(GtkButton * button, COMM_DATA * c)
{
	backend_nav_commentary_COMM(c->modnum, 1);
}

static void on_btnCOMMPrint_clicked(GtkButton * button, COMM_DATA * c)
{
	html_print(c->html);
}

static gboolean
on_button_release_event(GtkWidget * widget,
			GdkEventButton * event, COMM_DATA * c)
{
	extern gboolean in_url;
	gchar *key;
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
					gotoBookmarkSWORD(dict, key);
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


static GtkWidget *createPaneCOMM(SETTINGS * s, COMM_DATA * c,
				 gint count)
{
	GtkWidget *frameCOMM;
	GtkWidget *vbox57;
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
	gtk_container_add(GTK_CONTAINER(s->notebookCOMM), frameCOMM);

	vbox57 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox57);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox57",
				 vbox57, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox57);
	gtk_container_add(GTK_CONTAINER(frameCOMM), vbox57);

	toolbarCOMM =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarCOMM);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbarCOMM",
				 toolbarCOMM, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(toolbarCOMM);
	gtk_box_pack_start(GTK_BOX(vbox57), toolbarCOMM, FALSE,
			   TRUE, 0);
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
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->notebookCOMM),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(s->notebookCOMM),
				    count), label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (s->notebookCOMM),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (s->notebookCOMM),
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
			   GTK_SIGNAL_FUNC(on_btnCOMMSync_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMBack), "clicked",
			   GTK_SIGNAL_FUNC(on_btnCOMMBack_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMForward),
			   "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnCOMMForward_clicked),
			   (COMM_DATA *) c);
	gtk_signal_connect(GTK_OBJECT(c->btnCOMMPrint), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnCOMMPrint_clicked), (COMM_DATA *) c);

	return frameCOMM;
}

void gui_displayCOMM(gchar * key)
{
	backend_displayinCOMM(settings->commLastPage, key);
}

void gui_setupCOMM(SETTINGS * s)
{
	GtkWidget *popupmenu;
	GList *tmp = NULL;
	GList *mods = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	COMM_DATA *c;
	gint count = 0;

	comm_list = NULL;

	mods = backend_get_list_of_mods_by_type(COMM_MODS);
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		c = g_new(COMM_DATA, 1);
		c->modName = modname;
		c->modDescription =
		    backend_get_module_description(modname);
		c->modnum = count;
		c->searchstring = NULL;
		c->key = NULL;
		c->find_dialog = NULL;
		createPaneCOMM(s, c, count);
		popupmenu = create_pmCOMM(c);
		gnome_popup_menu_attach(popupmenu, c->html, NULL);
		backend_newDisplayCOMM(c->html, c->modName, s);
		comm_list = g_list_append(comm_list, (COMM_DATA *) c);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(s->notebookCOMM),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebookCOMM_switch_page), comm_list);

	modbuf = g_strdup(s->CommWindowModule);
	keybuf = g_strdup(s->currentverse);

	setPageCOMM(modbuf, comm_list, s);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
	g_list_free(mods);
	//s->dlLastPage = 0;
}

void gui_shutdownCOMM(void)
{
	comm_list = g_list_first(comm_list);
	while (comm_list != NULL) {
		COMM_DATA *c = (COMM_DATA *) comm_list->data;
		if (c->find_dialog)	/* free any search dialogs created */
			g_free(c->find_dialog);
		g_free((COMM_DATA *) comm_list->data);
		comm_list = g_list_next(comm_list);
	}
	g_list_free(comm_list);
}

/******  end of file  ******/
