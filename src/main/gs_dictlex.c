/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_dictlex.c (dictionary / lexicon support)
     * -------------------
     * Wed Apr  3 16:12:45 2002
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
#include <gtkhtml/gtkhtml.h>


#include "gs_gnomesword.h"
#include "gs_dictlex.h"
#include "gs_html.h"
#include "sword.h"
#include "dictlex.h"
#include "gs_viewdict_dlg.h"
#include "cipher_key_dialog.h"


extern SETTINGS *settings;
extern gboolean isrunningSD;    /* is the view dictionary dialog runing */

GList *dl_list;
//gboolean show_tabs_dl;

/******  start code  ******/
void gui_set_dictionary_page_and_key(gint page_num, gchar * key)
{
	DL_DATA *d;

	d = (DL_DATA *) g_list_nth_data(dl_list, page_num);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebookDL),
			      page_num);
	gtk_entry_set_text(GTK_ENTRY(d->entry), key);

}

static void setPageDL(gchar * modname, GList * dl_list)
{
	gint page = 0;
	DL_DATA *d = NULL;

	dl_list = g_list_first(dl_list);
	while (dl_list != NULL) {
		d = (DL_DATA *) dl_list->data;
		if (!strcmp(d->modName, modname))
			break;
		++page;
		dl_list = g_list_next(dl_list);
	}

	gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebookDL), page);
	gtk_entry_set_text(GTK_ENTRY(d->entry), settings->dictkey);

	settings->dlLastPage = page;
}

static
void on_notebookDL_switch_page(GtkNotebook * notebook,
			       GtkNotebookPage * page,
			       gint page_num, GList * dl_list)
{
	DL_DATA *d; //, *d_old;

//      d_old = (DL_DATA*)g_list_nth_data(tmp, settings->dlLastPage);
	d = (DL_DATA *) g_list_nth_data(dl_list, page_num);
	//-- change tab label to current book name

	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK
					(settings->workbook_lower),
					gtk_notebook_get_nth_page
					(GTK_NOTEBOOK
					 (settings->workbook_lower), 0),
					d->modName);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (settings->workbook_lower),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (settings->workbook_lower),
					  0), d->modName);


	sprintf(settings->DictWindowModule, "%s", d->modName);
	/*
	   if(settings->finddialog) {
	   gnome_dialog_close(g_old->find_dialog->dialog);
	   searchGS_FIND_DLG(g, FALSE, settings->findText);
	   } */
	GTK_CHECK_MENU_ITEM(d->showtabs)->active = settings->dict_tabs;
	settings->dlLastPage = page_num;
	//gtk_entry_set_text(GTK_ENTRY(d->entry),settings->dictkey);
}

static void on_entryDictLookup_changed(GtkEditable * editable,
				       DL_DATA * d)
{
	gint count;
	gchar *key, *text;
	GList *dictkeys = NULL;
	static gboolean firsttime = TRUE;

	key = gtk_entry_get_text(GTK_ENTRY(d->entry));
	strcpy(settings->dictkey, key);
	backend_displayinDL(d->modName, key);
	if (firsttime)
		count = 11;
	else
		count =
		    GTK_CLIST(d->clist)->clist_window_height /
		    GTK_CLIST(d->clist)->row_height;
	if (count) {
		dictkeys = backend_fillDictKeysDL(d->modName, count);
		if (dictkeys) {
			dictkeys = g_list_first(dictkeys);
			gtk_clist_clear(GTK_CLIST(d->clist));
			while (dictkeys != NULL) {
				text = (gchar *) dictkeys->data;
				gtk_clist_append(GTK_CLIST(d->clist),
						 &text);
				dictkeys = g_list_next(dictkeys);
			}
			g_list_free(dictkeys);
		}
	}
	firsttime = FALSE;
}

static void
on_clistDictLex_select_row(GtkCList * clist,
			   gint row,
			   gint column, GdkEvent * event, DL_DATA * d)
{
	gchar *text;

	gtk_clist_get_text(GTK_CLIST(GTK_WIDGET(clist)), row, column,
			   &text);
	gtk_entry_set_text(GTK_ENTRY(d->entry), text);
}

static
void on_copy_activate(GtkMenuItem * menuitem, DL_DATA * d)
{
	//copyGS_HTML(d->html);
}

static
void on_find_activate(GtkMenuItem * menuitem, DL_DATA * d)
{
	//searchGS_FIND_DLG(dl, FALSE, NULL);
}

static void on_view_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gint page;

	page = GPOINTER_TO_INT(user_data);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebookDL), page);
}

static
void on_showtabs_activate(GtkMenuItem * menuitem, SETTINGS * s)
{
	s->dict_tabs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebookDL),
				   s->dict_tabs);
}

static 
void on_view_new_activate(GtkMenuItem * menuitem, SETTINGS * s)
{
	extern GtkWidget *frameShowDict;
	static GtkWidget *dlg;
	gchar *modName;
        GdkCursor *cursor;	
	gtk_widget_show(s->app);
	cursor = gdk_cursor_new(GDK_WATCH);
	gdk_window_set_cursor(s->app->window,cursor);
	
	if(!isrunningSD) {
		dlg = create_dlgViewDict(s->app);
		modName = s->DictWindowModule;
		/* set frame label to current Module name  */
		gtk_frame_set_label(GTK_FRAME(frameShowDict),modName);  				
		//initSD(modName);
		isrunningSD = TRUE;
	}
	gtk_widget_show(dlg);
	gtk_widget_show(s->app);
	cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	gdk_window_set_cursor(s->app->window,cursor);
}

static
void on_btnSyncDL_clicked(GtkButton * button, DL_DATA * d)
{
	gtk_entry_set_text(GTK_ENTRY(d->entry), settings->dictkey);
}

static void on_unlock_key_activate(GtkMenuItem * menuitem, DL_DATA * d)
{
	GtkWidget *dlg;
	
	dlg = gui_create_cipher_key_dialog(d->modName);
	gtk_widget_show(dlg);
}
static
GtkWidget *create_pmDL(DL_DATA * dl, GList * mods)
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

	/*
	   if module has cipher key include this item
	 */
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
	tmp = mod_lists->dict_descriptions;
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
			   settings);
	gtk_signal_connect(GTK_OBJECT(view_new), "activate",
			   GTK_SIGNAL_FUNC(on_view_new_activate),
			   settings);
	return pm;
}


static GtkWidget *create_DictLexPane(SETTINGS * s, GList * mods,
				     DL_DATA * dl, gint count)
{

	GtkWidget *frameDict;
	GtkWidget *hpaned7;
	GtkWidget *vbox56;
	GtkWidget *toolbarDLKey;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSyncDL;
	GtkWidget *label205;
	GtkWidget *frameDictHTML;
	GtkWidget *scrolledwindowDictHTML;
	GtkWidget *popupmenu;
	GtkWidget *label;

	frameDict = gtk_frame_new(NULL);
	gtk_widget_ref(frameDict);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frameDict",
				 frameDict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameDict);
	gtk_container_add(GTK_CONTAINER(s->notebookDL), frameDict);

	hpaned7 = gtk_hpaned_new();
	gtk_widget_ref(hpaned7);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "hpaned7",
				 hpaned7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned7);
	gtk_container_add(GTK_CONTAINER(frameDict), hpaned7);
	gtk_paned_set_position(GTK_PANED(hpaned7), 190);

	vbox56 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox56);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox56",
				 vbox56,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox56);
	gtk_paned_pack1(GTK_PANED(hpaned7), vbox56, FALSE, TRUE);


	toolbarDLKey =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarDLKey);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbarDLKey",
				 toolbarDLKey,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarDLKey);
	gtk_box_pack_start(GTK_BOX(vbox56), toolbarDLKey, FALSE, TRUE,
			   0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarDLKey),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(s->app,
				      GNOME_STOCK_PIXMAP_REFRESH);
	btnSyncDL =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarDLKey),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"), _("Load current key"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnSyncDL);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "btnSyncDL",
				 btnSyncDL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSyncDL);




	dl->entry = gtk_entry_new();
	gtk_widget_ref(dl->entry);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "dl->entry",
				 dl->entry,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->entry);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarDLKey), dl->entry,
				  NULL, NULL);

	dl->clist = gtk_clist_new(1);
	gtk_widget_ref(dl->clist);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "dl->clist",
				 dl->clist,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->clist);
	gtk_box_pack_start(GTK_BOX(vbox56), dl->clist, TRUE, TRUE, 0);
	gtk_clist_set_column_width(GTK_CLIST(dl->clist), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(dl->clist));

	label205 = gtk_label_new(_("label205"));
	gtk_widget_ref(label205);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label205",
				 label205,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label205);
	gtk_clist_set_column_widget(GTK_CLIST(dl->clist), 0, label205);

	frameDictHTML = gtk_frame_new(NULL);
	gtk_widget_ref(frameDictHTML);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "frameDictHTML", frameDictHTML,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameDictHTML);
	gtk_paned_pack2(GTK_PANED(hpaned7), frameDictHTML, TRUE, TRUE);

	scrolledwindowDictHTML = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowDictHTML);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
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
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "dl->html", dl->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dl->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowDictHTML),
			  dl->html);
	gtk_html_load_empty(GTK_HTML(dl->html));

	label = gtk_label_new(dl->modName);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->notebookDL),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(s->notebookDL), count),
				   label);
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->notebookDL),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK(s->notebookDL),
					  count),
					 (gchar *) dl->modName);

	gtk_signal_connect(GTK_OBJECT(btnSyncDL), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSyncDL_clicked), dl);
	gtk_signal_connect(GTK_OBJECT(dl->html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(dl->html), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) s->app);
	gtk_signal_connect(GTK_OBJECT(dl->entry), "changed",
			   GTK_SIGNAL_FUNC(on_entryDictLookup_changed),
			   dl);
	gtk_signal_connect(GTK_OBJECT(dl->clist), "select_row",
			   GTK_SIGNAL_FUNC(on_clistDictLex_select_row),
			   dl);

	popupmenu = create_pmDL(dl, mods);
	gnome_popup_menu_attach(popupmenu, dl->html, NULL);
	dl->find_dialog = NULL;

	return frameDict;
}

GList* gui_setup_dict(SETTINGS * s)
{
	GList *tmp = NULL;
	GList *mods = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	DL_DATA *dl;
	gint count = 0;

	dl_list = NULL;

	mods = backend_get_list_of_mods_by_type(DICT_MODS);
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		dl = g_new(DL_DATA, 1);
		dl->modName = modname;
		dl->modDescription =
		    backend_get_module_description(modname);
		dl->searchstring = NULL;	
		dl->has_key = backend_module_is_locked(dl->modName);
		create_DictLexPane(s, mods, dl, count);
		backend_newDisplayDL(dl->html, dl->modName, s);
		dl_list = g_list_append(dl_list, (DL_DATA *) dl);
		++count;
		tmp = g_list_next(tmp);
	}


	gtk_signal_connect(GTK_OBJECT(s->notebookDL), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebookDL_switch_page),
			   dl_list);

	modbuf = g_strdup(s->DictWindowModule);
	keybuf = g_strdup(s->dictkey);

	setPageDL(modbuf, dl_list);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
	s->dlLastPage = 0;
	return mods;
}

void gui_shutdownDL(void)
{
	dl_list = g_list_first(dl_list);
	while (dl_list != NULL) {
		DL_DATA *d = (DL_DATA *) dl_list->data;
		/*if (d->modName)
   g_free(d->modName); *//* free modNmae */
		if (d->find_dialog)
			g_free(d->find_dialog);	/* free any search dialogs created */
		g_free((DL_DATA *) dl_list->data);
		dl_list = g_list_next(dl_list);
	}
	g_list_free(dl_list);

}

/******   end of file   ******/
