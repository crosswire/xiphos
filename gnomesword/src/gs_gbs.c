/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_gbs.c
     * -------------------
     * Wed Feb 20 11:24:16 2002
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
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#include  <gal/shortcut-bar/e-vscrolled-bar.h>


#include "gs_gbs.h"
#include "sw_gbs.h"
#include "gs_getinfo_dlg.h"
#include "gs_editor.h"
#include "gs_info_box.h"
#include  "support.h"

/****  externs  ****/
extern SETTINGS *settings;
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;

GtkWidget *scrolledwindow;
GtkCTreeNode *current_node;

void addbooktoCTreeGS_GBS(SETTINGS * s, GList * books)
{
	GList * tmp = NULL;
	gchar * buf[3];
	GtkCTreeNode * node;
	gint i, count;	
	
	gtk_ctree_collapse_recursive (GTK_CTREE(s->ctree_widget_books), NULL); 
	count = GTK_CLIST(GTK_CTREE(s->ctree_widget_books))->rows;
	for(i=0; i< count; i++){
		node = gtk_ctree_node_nth(GTK_CTREE(s->ctree_widget_books), i);
		gtk_ctree_remove_node (GTK_CTREE(s->ctree_widget_books), node);
	}
		
	tmp = books;
	while (tmp != NULL) {
		buf[0] = (gchar *) tmp->data;
		buf[1] = (gchar *) tmp->data;
		buf[2] = "0";

		node =
		    gtk_ctree_insert_node(GTK_CTREE(s->ctree_widget_books),
					  NULL, NULL, buf, 3, pixmap1,
					  mask1, pixmap2, mask2, FALSE,
					  FALSE);

		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

/****  setup up GBS ctree for loading into a shortcut bar group  ****/
GtkWidget *setupGBS(SETTINGS * s, GList * books)
{
	GtkWidget *pmGbs;
	
	scrolledwindow = e_vscrolled_bar_new(NULL);
	gtk_widget_ref(scrolledwindow);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "scrolledwindow",
				 scrolledwindow,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow);
	
	
	s->ctree_widget_books = gtk_ctree_new(3, 0);
	gtk_widget_ref(s->ctree_widget_books);
	gtk_object_set_data_full(GTK_OBJECT(s->app),
				 "s->ctree_widget_books",
				 s->ctree_widget_books,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(s->ctree_widget_books);
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  s->ctree_widget_books);
	gtk_clist_set_column_width(GTK_CLIST(s->ctree_widget_books), 0,
				   280);
	gtk_clist_set_column_width(GTK_CLIST(s->ctree_widget_books), 1,
				   80);
	gtk_clist_set_column_width(GTK_CLIST(s->ctree_widget_books), 2,
				   280);
	/* load books */
	loadBookListSW_GBS(s);
	/* create and attach popup menu */
	pmGbs = create_pmGBS();
	gnome_popup_menu_attach(pmGbs, s->ctree_widget_books, s);

	gtk_signal_connect(GTK_OBJECT(s->ctree_widget_books), "select_row",
			   GTK_SIGNAL_FUNC(on_ctreeBooks_select_row), s);
	return scrolledwindow;
}

/****  start popup menu for GBS tree ****/

/** popup menu callbacks **/

static void
on_create_new_book_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GETINFNO *p_info;
	GETINFNO info;
	GtkWidget *dlg;

	/* get book name */

	p_info = &info;
	p_info->text[0] = "New Book                           ";
	p_info->text[1] = "File Name                          ";
	p_info->text[2] = "0";
	p_info->howmany = 2;

	p_info->labelText[0] = "New GBS Book";
	p_info->labelText[1] = "Enter Title";
	p_info->labelText[2] = "Enter File Name";
	p_info->labelText[3] = "";

	dlg = create_dlgGetTextInfo(p_info);
	gnome_dialog_set_default(GNOME_DIALOG(dlg), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(dlg));
	/* add root node to ctree */

	p_info->text[2] = g_strdup("0");
	current_node =
	    gtk_ctree_insert_node(GTK_CTREE(settings->ctree_widget_books),
				  NULL, NULL, p_info->text, 3, pixmap1,
				  mask1, pixmap2, mask2, FALSE, FALSE);
	/* create a new gbs book */
	addnewbookSW_GBS(settings, p_info->text[0], p_info->text[1]);
	g_free(p_info->text[0]);
	g_free(p_info->text[1]);
	g_free(p_info->text[2]);
}

static void
on_add_child_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GETINFNO *p_info;
	GETINFNO info;
	GtkWidget *dlg;


	p_info = &info;
	p_info->text[0] = "New child                           ";
	p_info->text[1] = "";
	p_info->text[2] = "";
	p_info->howmany = 1;

	p_info->labelText[0] = "Add Child to Current Node";
	p_info->labelText[1] = "Child Name";
	p_info->labelText[2] = "";
	p_info->labelText[3] = "";

	dlg = create_dlgGetTextInfo(p_info);
	gnome_dialog_set_default(GNOME_DIALOG(dlg), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(dlg));

	addchildSW_GBS(settings, p_info->text[0]);

	g_free(p_info->text[0]);
	g_free(p_info->text[1]);
	g_free(p_info->text[2]);
}


static void
on_add_sibling_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GETINFNO *p_info;
	GETINFNO info;
	GtkWidget *dlg;
	//GtkCTreeNode *node;
	/* get book name */

	p_info = &info;
	p_info->text[0] = "New sibling                           ";
	p_info->text[1] = "File Name                          ";
	p_info->text[2] = "0";
	p_info->howmany = 1;

	p_info->labelText[0] = "Add New Sibling";
	p_info->labelText[1] = "Sibling Title";
	p_info->labelText[2] = "";
	p_info->labelText[3] = "";

	dlg = create_dlgGetTextInfo(p_info);
	gnome_dialog_set_default(GNOME_DIALOG(dlg), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(dlg));
	/*

	   node = gtk_ctree_insert_node(GTK_CTREE(settings->ctree_widget_books),
	   NULL, NULL, p_info->text, 3,
	   pixmap1, mask1, pixmap2,
	   mask2, FALSE, FALSE);
	 */
	addSiblingSW_GBS(settings, p_info->text[0]);
	g_free(p_info->text[0]);
	g_free(p_info->text[1]);
	g_free(p_info->text[2]);
}

/*    */
static void
on_edit_current_node_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GETINFNO *p_info;
	GETINFNO info;
	GtkWidget *dlg;
	//GtkCTreeNode *node;
	/* get book name */

	p_info = &info;
	p_info->text[0] = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[0])->text;
	p_info->text[1] = "";
	p_info->text[2] = "0";
	p_info->howmany = 1;

	p_info->labelText[0] = "Edit Note Title";
	p_info->labelText[1] = "Node Title";
	p_info->labelText[2] = "";
	p_info->labelText[3] = "";

	dlg = create_dlgGetTextInfo(p_info);
	gnome_dialog_set_default(GNOME_DIALOG(dlg), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(dlg));
	
	changeNodeNameSW_GBS(settings, p_info->text[0]);
	
	g_free(p_info->text[0]);
	g_free(p_info->text[1]);
	g_free(p_info->text[2]);
}


static void
on_delete_current_node_activate(GtkMenuItem * menuitem, gpointer user_data)
{	
	GtkWidget 
		*label1,
		*label2,
		*label3,
		*msgbox;
	gint 
		answer;
	
	msgbox = create_InfoBox();
	label1 = lookup_widget(msgbox,"lbInfoBox1");	
	label2 = lookup_widget(msgbox,"lbInfoBox2");	
	label3 = lookup_widget(msgbox,"lbInfoBox3");
	gtk_label_set_text(GTK_LABEL(label1), "Are you sure you want");
	gtk_label_set_text(GTK_LABEL(label2), "to delete this node");
	gtk_label_set_text(GTK_LABEL(label3), GTK_CELL_PIXTEXT(GTK_CTREE_ROW(current_node)->row.
				     cell[0])->text);
	
	gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
	answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
	switch (answer) {
		case 0:	if(deleteNodeSW_GBS(settings))
				gtk_ctree_remove_node(GTK_CTREE(settings->ctree_widget_books),
                                             current_node);
			break;
		default:
			break;
	}	
}



/** popup menu items **/
static GnomeUIInfo pmGBS_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Create New Book"),
	 N_("Create a new empty book"),
	 (gpointer) on_create_new_book_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOOK_RED,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Add Child"),
	 N_("add a child to current node"),
	 (gpointer) on_add_child_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_DOWN,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Add Sibling"),
	 NULL,
	 (gpointer) on_add_sibling_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_FORWARD,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Edit Current Node"),
	 N_("edit the name of the current node"),
	 (gpointer) on_edit_current_node_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_PROP,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Delete Current Node"),
	 NULL,
	 (gpointer) on_delete_current_node_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CUT,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

/***  set sensitivity of edit items  ***/
void setitemssensitivityGS_GBS(GSHTMLEditorControlData *gbsecd, gint iswritable)
{
	gboolean
		sensitive = FALSE,
		addchild = FALSE;
	switch(iswritable) {
		case 0: sensitive = FALSE;
			addchild = FALSE;
			break;
		case 1: sensitive = TRUE;
			addchild = TRUE;
			break;
		case 2: sensitive = FALSE;
			addchild = TRUE;
			break;
	}
	
	gtk_widget_set_sensitive(GTK_WIDGET(pmGBS_uiinfo[2].widget), /* Add Child */
			addchild);
	gtk_widget_set_sensitive(GTK_WIDGET(pmGBS_uiinfo[3].widget), /* Add Sibling */
			sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(pmGBS_uiinfo[5].widget), /* Edit Current Node */
			sensitive);
	//gtk_widget_set_sensitive(GTK_WIDGET(pmGBS_uiinfo[6].widget), /* Delete Current Node */
	//		sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(gbsecd->editnote), /* edit book */
			sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(gbsecd->file), /* file menu */
			sensitive);		
	gtk_widget_set_sensitive(GTK_WIDGET(gbsecd->edit2), /* edit menu */
			sensitive);		
	gtk_widget_set_sensitive(GTK_WIDGET(gbsecd->link), /* link item */
			sensitive);	
}

/***  create popup menu  ***/
GtkWidget *create_pmGBS(void)
{
	GtkWidget *pmGBS;

	pmGBS = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmGBS), "pmGBS", pmGBS);
	gnome_app_fill_menu(GTK_MENU_SHELL(pmGBS), pmGBS_uiinfo,
			    NULL, FALSE, 0);

	gtk_widget_ref(pmGBS_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "create_new_book",
				 pmGBS_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_ref(pmGBS_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "separator23",
				 pmGBS_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmGBS_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "add_child",
				 pmGBS_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_sensitive(GTK_WIDGET(pmGBS_uiinfo[2].widget),
				 FALSE);
				 
	gtk_widget_ref(pmGBS_uiinfo[3].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "add_sibling",
				 pmGBS_uiinfo[3].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_sensitive(GTK_WIDGET(pmGBS_uiinfo[3].widget),
				 FALSE);

	gtk_widget_ref(pmGBS_uiinfo[4].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "separator22",
				 pmGBS_uiinfo[4].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmGBS_uiinfo[5].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "edit_current_node",
				 pmGBS_uiinfo[5].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_sensitive(GTK_WIDGET(pmGBS_uiinfo[5].widget),
				 FALSE);

	gtk_widget_ref(pmGBS_uiinfo[6].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmGBS), "delete_current_node",
				 pmGBS_uiinfo[6].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_sensitive(GTK_WIDGET(pmGBS_uiinfo[6].widget),
				 FALSE);

	return pmGBS;
}

/****  end popup menu for GBS tree ****/

void setnodeinfoGS_GBS(SETTINGS *s, NODEDATA *data)
{
	gtk_ctree_set_node_info(GTK_CTREE(s->ctree_widget_books),
		data->parent,
		*data->buf,
		3,
		data->pixmap1,
		data->mask1,
		data->pixmap2, 
		data->mask2, 
		data->is_leaf,
		data->expanded);	
}

GtkCTreeNode *addnodeGS_GBS(SETTINGS *s, NODEDATA *data)
{
	GtkCTreeNode *retval;
	
	retval = gtk_ctree_insert_node(GTK_CTREE(s->ctree_widget_books),
			data->parent,
			data->sibling,  
			data->buf, 
			3, 
			data->pixmap1,
			data->mask1,
			data->pixmap2, 
			data->mask2, 
			data->is_leaf, 
			data->expanded);
	return retval;
}

