/*
 * GnomeSword Bible Study Tool
 * dictlex_dialog.c - dialog for displaying a dictlex module
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
#include <gtkhtml/gtkhtml.h>

#include "gui/gtkhtml_display.h"
#include "gui/dictlex_dialog.h"

#include "main/dictlex.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"
#include "main/settings.h"
#include "main/lists.h"


/******************************************************************************
 * static - global to this file only
 */
static GList * dialog_list;
static DL_DIALOG *cur_dlg;

/******************************************************************************
 * Name
 *   on_entrySDLookup_changed
 *
 * Synopsis
 *   #include "dictlex_dialog.h"
 *
 *   void on_entrySDLookup_changed(GtkEditable * editable,
 *						gpointer user_data)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_entry_lookup_changed(GtkEditable * editable,
						DL_DATA * d)
{
	gint count, i;
	gchar *key, *new_key, *text;
	static gboolean firsttime = TRUE;
	
	key = gtk_entry_get_text(GTK_ENTRY(d->entry));
	
	text = get_dictlex_text(d->mod_name, key);	
	entry_display(d->html, d->mod_name,
		   text, key, TRUE);
	free(text);
	
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
 *   on_btn_close_clicked
 *
 * Synopsis
 *   #include "dictlex_dialog.h"
 *
 *   void on_btn_close_clicked(GtkButton *button, DL_DIALOG *dlg)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_btn_close_clicked(GtkButton *button, DL_DIALOG *dlg)
{
	gtk_widget_destroy(dlg->dialog);
	dlg->dialog = NULL;
}

/******************************************************************************
 * Name
 *   gui_create_dictlex_dialog
 *
 * Synopsis
 *   #include "dictlex_dialog.h"
 *
 *   GtkWidget *gui_create_dictlex_dialog(GtkWidget * app)
 *
 * Description
 *    create the interface
 *
 * Return value
 *   GtkWidget *
 */

static void create_dictlex_dialog(DL_DIALOG *dlg)
{
	GtkWidget *hpaned7;
	GtkWidget *vbox;
	GtkWidget *vbox56;
	GtkWidget *toolbarDLKey;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSyncDL;
	GtkWidget *label205;
	GtkWidget *frameDictHTML;
	GtkWidget *scrolledwindowDictHTML;
	GtkWidget *label;
	GtkWidget *hbuttonbox1;
	GtkWidget *btn_close;
	
	gchar title[256];
	
	sprintf(title,"%s - %s", dlg->d->mod_name, _("GnomeSword"));
	dlg->dialog = gtk_window_new (GTK_WINDOW_DIALOG);
	    
	gtk_object_set_data(GTK_OBJECT(dlg->dialog), "dlg->dialog",
			    dlg->dialog);
	gtk_window_set_title (GTK_WINDOW (dlg->dialog), title);
	gtk_window_set_default_size(GTK_WINDOW(dlg->dialog), 465, 275);
	gtk_window_set_policy(GTK_WINDOW(dlg->dialog), TRUE, TRUE,
			      FALSE);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_ref (vbox);
	gtk_object_set_data_full (GTK_OBJECT (dlg->dialog), "vbox", 
			vbox,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (dlg->dialog), vbox);
  
  
	dlg->d->frame = gtk_frame_new(NULL);
	gtk_widget_ref(dlg->d->frame);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "dlg->d->frame",
				 dlg->d->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dlg->d->frame);	
	gtk_box_pack_start(GTK_BOX(vbox), dlg->d->frame, TRUE, TRUE,
			   0);

	hpaned7 = gtk_hpaned_new();
	gtk_widget_ref(hpaned7);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "hpaned7",
				 hpaned7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned7);
	gtk_container_add(GTK_CONTAINER(dlg->d->frame), hpaned7);
	gtk_paned_set_position(GTK_PANED(hpaned7), 190);

	vbox56 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox56);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "vbox56",
				 vbox56,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox56);
	gtk_paned_pack1(GTK_PANED(hpaned7), vbox56, FALSE, TRUE);


	toolbarDLKey =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarDLKey);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "toolbarDLKey",
				 toolbarDLKey,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarDLKey);
	gtk_box_pack_start(GTK_BOX(vbox56), toolbarDLKey, FALSE, TRUE,
			   0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarDLKey),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(dlg->dialog,
				      GNOME_STOCK_PIXMAP_REFRESH);
	btnSyncDL =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarDLKey),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"), _("Load current key"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnSyncDL);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "btnSyncDL",
				 btnSyncDL,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSyncDL);




	dlg->d->entry = gtk_entry_new();
	gtk_widget_ref(dlg->d->entry);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "dlg->d->entry",
				 dlg->d->entry,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dlg->d->entry);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarDLKey), dlg->d->entry,
				  NULL, NULL);

	dlg->d->clist = gtk_clist_new(1);
	gtk_widget_ref(dlg->d->clist);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "dlg->d->clist",
				 dlg->d->clist,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dlg->d->clist);
	gtk_box_pack_start(GTK_BOX(vbox56), dlg->d->clist, TRUE, TRUE, 0);
	gtk_clist_set_column_width(GTK_CLIST(dlg->d->clist), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(dlg->d->clist));

	label205 = gtk_label_new(_("label205"));
	gtk_widget_ref(label205);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "label205",
				 label205,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label205);
	gtk_clist_set_column_widget(GTK_CLIST(dlg->d->clist), 0, label205);

	frameDictHTML = gtk_frame_new(NULL);
	gtk_widget_ref(frameDictHTML);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog),
				 "frameDictHTML", frameDictHTML,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frameDictHTML);
	gtk_paned_pack2(GTK_PANED(hpaned7), frameDictHTML, TRUE, TRUE);

	scrolledwindowDictHTML = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowDictHTML);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog),
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


	dlg->d->html = gtk_html_new();
	gtk_widget_ref(dlg->d->html);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog),
				 "dlg->d->html", dlg->d->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dlg->d->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindowDictHTML),
			  dlg->d->html);
	gtk_html_load_empty(GTK_HTML(dlg->d->html));

	label = gtk_label_new(dlg->d->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog), "label", label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	
	hbuttonbox1 = gtk_hbutton_box_new ();
	gtk_widget_ref (hbuttonbox1);
	gtk_object_set_data_full (GTK_OBJECT(dlg->dialog), 
			"hbuttonbox1", hbuttonbox1,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (hbuttonbox1);
	gtk_box_pack_end (GTK_BOX (vbox), hbuttonbox1, FALSE, TRUE, 0);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);
	
	btn_close = gnome_stock_button (GNOME_STOCK_BUTTON_CLOSE);
	gtk_widget_ref (btn_close);
	gtk_object_set_data_full (GTK_OBJECT (dlg->dialog),
			"btn_close", btn_close,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btn_close);
	gtk_container_add (GTK_CONTAINER (hbuttonbox1), btn_close);
	GTK_WIDGET_SET_FLAGS (btn_close, GTK_CAN_DEFAULT);
	gtk_button_set_relief (GTK_BUTTON (btn_close), GTK_RELIEF_NONE);

	gtk_signal_connect (GTK_OBJECT (btn_close), "clicked",
                      GTK_SIGNAL_FUNC (on_btn_close_clicked),
                      dlg);
/*
	gtk_signal_connect(GTK_OBJECT(dlg->d->html),
			"button_press_event",
			GTK_SIGNAL_FUNC(html_button_pressed), 
			dlg);
	gtk_signal_connect(GTK_OBJECT(dlg->d->html), "link_clicked",
			   GTK_SIGNAL_FUNC(on_link_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(dlg->d->html), "on_url",
			   GTK_SIGNAL_FUNC(on_url), (gpointer) settings.app);
	gtk_signal_connect(GTK_OBJECT(dlg->d->clist), "select_row",
			   GTK_SIGNAL_FUNC(on_clistDictLex_select_row),
			   dlg);
*/
	gtk_signal_connect(GTK_OBJECT(btnSyncDL), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSyncDL_clicked), dlg->d);
	gtk_signal_connect(GTK_OBJECT(dlg->d->entry), "changed",
			   GTK_SIGNAL_FUNC(on_entry_lookup_changed),
			   dlg->d);
	gtk_signal_connect(GTK_OBJECT(dlg->d->clist), "select_row",
			   GTK_SIGNAL_FUNC(on_clistDictLex_select_row),
			   dlg->d);
}


/******************************************************************************
 * Name
 *   gui_open_dictlex_dialog
 *
 * Synopsis
 *   #include "dictlex_dialog.h"
 *
 *   void gui_open_dictlex_dialog(gint mod_num)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_open_dictlex_dialog(gint mod_num)
{		
	DL_DIALOG *dlg;
	
	dlg = (DL_DIALOG *) g_list_nth_data(dialog_list, mod_num);	
	
//	g_warning(dlg->d->mod_name);
	if(dlg->dialog == NULL){
		create_dictlex_dialog(dlg);
		gtk_widget_show(dlg->dialog);
		on_btnSyncDL_clicked(NULL, dlg->d);
	}
	else {
		gtk_widget_show(dlg->dialog);
		gdk_window_raise(GTK_WIDGET(dlg->dialog)->window);
	}
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

void gui_setup_dictlex_dialog(GList *mods)
{
	DL_DIALOG *dlg;
	GList *tmp = NULL;
	gint count = 0;
	
	dialog_list = NULL;
	
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		dlg = g_new(DL_DIALOG, 1);
		dlg->d = g_new(DL_DATA, 1);
		dlg->d->find_dialog = NULL;
		dlg->d->mod_num = count;
		dlg->d->search_string = NULL;
		dlg->dialog = NULL;
		dlg->d->mod_name = (gchar *) tmp->data;
		//gui_get_module_global_options(vt->t);
		++count;
		dialog_list = g_list_append(dialog_list, (DL_DIALOG*) dlg);
		tmp = g_list_next(tmp);
	}
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

void gui_shutdown_dictlex_dialog(void) 
{
	dialog_list = g_list_first(dialog_list);
	while (dialog_list != NULL) {
		DL_DIALOG *dlg = (DL_DIALOG *) dialog_list->data;
		/* 
		 *  destroy any dialogs created 
		 */
		if (dlg->dialog)
			 gtk_widget_destroy(dlg->dialog);
		/* 
		 * free any search dialogs created 
		 */
		if (dlg->d->find_dialog)
			 g_free(dlg->d->find_dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		g_free(dlg->d);
		g_free((DL_DIALOG *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	} g_list_free(dialog_list);
}





//******  end of file  ******/

