/*
 * GnomeSword Bible Study Tool
 * gbs_dialog.c - dialog for displaying a gbs module
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
#include "gui/gbs_dialog.h"
#include "gui/html.h"

#include "main/gbs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"


/******************************************************************************
 *  externs  
 */
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;

/******************************************************************************
 * static - global to this file only
 */
static GList *dialog_list;
static GBS_DIALOG *cur_dlg;
static gboolean dialog_freed;



/******************************************************************************
 * Name
 *  add_book_to_ctree
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void add_book_to_ctree(GtkWidget * ctree, gchar * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
static void add_book_to_ctree(GtkWidget * ctree, gchar * mod_name)
{

	GtkCTreeNode *rootnode;
	gchar *buf[3];

	buf[0] = mod_name;
	buf[1] = mod_name;
	buf[2] = "0";
	rootnode = gtk_ctree_insert_node(GTK_CTREE(ctree),
					 NULL, NULL, buf, 3, pixmap1,
					 mask1, pixmap2, mask2, FALSE,
					 FALSE);

}


/******************************************************************************
 * Name
 *  add_node_gbs
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   GtkCTreeNode *add_node_gbs(GBS_DIALOG * dlg, NODEDATA *data)	
 *
 * Description
 *    
 *
 * Return value
 *   GtkCTreeNode*
 */ 
 
static GtkCTreeNode *add_node_gbs(GBS_DIALOG * dlg, NODEDATA * data)
{
	GtkCTreeNode *retval;

	retval = gtk_ctree_insert_node(
			GTK_CTREE(dlg->g->ctree),
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
		
/******************************************************************************
 * Name
 *  add_node_children
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void add_node_children(GBS_DIALOG * dlg, GtkCTreeNode *node, 
 *			gchar *bookname, unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
static void add_node_children(GBS_DIALOG * dlg, GtkCTreeNode *node, 
				gchar *bookname, unsigned long offset)
{
	gchar buf[256];
	gchar *tmpbuf;
	GtkCTreeNode *tmp_parent_node = node;
	NODEDATA nodedata, *p_nodedata;
	
	p_nodedata = &nodedata;
	p_nodedata->sibling = NULL;
	p_nodedata->buf[1] = bookname;

	if (gbs_treekey_first_child(offset)) {
		offset = gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_nodedata->parent = node;
		p_nodedata->buf[2] = buf;
		tmpbuf =
		    gbs_get_treekey_local_name(offset);
		p_nodedata->buf[0] = (gchar*)tmpbuf;
		if (gbs_treekey_has_children(offset)) {
			p_nodedata->pixmap1 = pixmap1;
			p_nodedata->mask1 = mask1;
			p_nodedata->pixmap2 = pixmap2;
			p_nodedata->mask2 = mask2;
			p_nodedata->is_leaf = FALSE;
			p_nodedata->expanded = FALSE;
		} else {
			p_nodedata->pixmap1 = pixmap3;
			p_nodedata->mask1 = mask3;
			p_nodedata->pixmap2 = NULL;
			p_nodedata->mask2 = NULL;
			p_nodedata->is_leaf = TRUE;
			p_nodedata->expanded = FALSE;
		}
		node = add_node_gbs(dlg, p_nodedata);
		free(tmpbuf);
	}

	while (treekey_next_sibling(offset)) {
		offset = gbs_get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_nodedata->parent = tmp_parent_node;
		p_nodedata->buf[2] = buf;
		tmpbuf =
		    gbs_get_treekey_local_name(offset);
		p_nodedata->buf[0] = (gchar*)tmpbuf;
		if (gbs_treekey_has_children(offset)) {
			p_nodedata->pixmap1 = pixmap1;
			p_nodedata->mask1 = mask1;
			p_nodedata->pixmap2 = pixmap2;
			p_nodedata->mask2 = mask2;
			p_nodedata->is_leaf = FALSE;
			p_nodedata->expanded = FALSE;
		} else {
			p_nodedata->pixmap1 = pixmap3;
			p_nodedata->mask1 = mask3;
			p_nodedata->pixmap2 = NULL;
			p_nodedata->mask2 = NULL;
			p_nodedata->is_leaf = TRUE;
			p_nodedata->expanded = FALSE;
		}
		node = add_node_gbs(dlg, p_nodedata);
		free(tmpbuf);
	}
}

/******************************************************************************
 * Name
 *  on_ctreeGBS_select_row
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   void on_ctreeGBS_select_row(GtkCTree *ctree, GList *node, gint column,
 *						GBS_DIALOG * dlg)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

static void ctree_select_row(GtkCTree *ctree, GList *node, gint column,
						GBS_DIALOG * dlg)
{
	gchar *bookname, *nodename, *offset, *key, *text = NULL;
	GtkCTreeNode *treeNode;
	
	treeNode = GTK_CTREE_NODE(node);
	
	nodename = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
					cell[0])->text;
	bookname = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
					cell[1])->text;
	offset = GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
					cell[2])->text;
	dlg->g->offset = strtoul(offset, NULL, 0);
	
	g_warning("bookname = %s mod_name = %s",bookname,dlg->g->mod_name);
	
	change_book(bookname, dlg->g->offset);
	
	text = display_row_gbs(dlg->g->mod_name, offset);
	if (text) {
	/** fill ctree node with children **/
		if ((GTK_CTREE_ROW(node)->children == NULL)
		    && (!GTK_CTREE_ROW(node)->is_leaf)) {
			add_node_children(dlg, treeNode, bookname, 
				    strtoul(offset, NULL, 0));     
			gtk_ctree_expand(GTK_CTREE(dlg->g->ctree),
					 treeNode);
		}
		key = get_book_key(dlg->g->mod_name);
		
		entry_display(dlg->g->html, dlg->g->mod_name,
		   text, key, TRUE);
		if(key) 
			free(key);
		free(text);
	} 
}


/******************************************************************************
 * Name
 *   free_on_destroy
 *
 * Synopsis
 *   #include "dictlex_dialog.h"
 *
 *   void free_on_destroy(DL_DIALOG * dlg)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

static void free_on_destroy(GBS_DIALOG * dlg)
{
	g_free(dlg->g);
	dialog_list = g_list_remove(dialog_list, (GBS_DIALOG *) dlg);
	g_warning("shuting down %s dialog", dlg->g->mod_name);
	g_free(dlg);
}


/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "dictlex_dialog.h"
 *
 *   void dialog_destroy(GtkObject *object, DL_DIALOG * dlg)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void dialog_destroy(GtkObject * object, GBS_DIALOG * dlg)
{
	if (!dialog_freed)
		free_on_destroy(dlg);
	dialog_freed = FALSE;
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

static void close_clicked(GtkButton * button, GBS_DIALOG * dlg)
{
	if (dlg->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(dlg->dialog);
	}
}


/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "dictlex_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, DL_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML * html, const gchar * url,
		       GBS_DIALOG * dlg)
{
	//gchar buf[255];
	//gint context_id2;

	cur_dlg = dlg;
	/*
	   context_id2 =
	   gtk_statusbar_get_context_id(GTK_STATUSBAR(vt->statusbar),
	   "GnomeSword");
	   gtk_statusbar_pop(GTK_STATUSBAR(vt->statusbar), context_id2);


	   if (url == NULL) {           
	   gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar), context_id2,
	   "");
	   }

	   else {               
	   if (*url == '@') {
	   ++url;
	   sprintf(buf, _("Show %s in main window"), url);
	   }

	   else if (*url == '[') {
	   ++url;
	   while (*url != ']') {
	   ++url;
	   }
	   ++url;
	   sprintf(buf, "%s", url);
	   }

	   else if (*url == '*') {
	   ++url;
	   sprintf(buf, "%s", url);
	   }

	   else
	   sprintf(buf, _("Go to %s"), url);

	   gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar), context_id2,
	   buf);
	   }
	 */
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_thml_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}




/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_gbf_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}
void
modops_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
edit_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}

void
paste_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}









/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void
on_plain_text_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}




/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void
on_button_new_clicked(GtkButton * button, gpointer user_data)
{

}




/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void
on_button_save_clicked(GtkButton * button, gpointer user_data)
{

}




/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void cut_clicked(GtkButton * button, gpointer user_data)
{

}




/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void copy_clicked(GtkButton * button, gpointer user_data)
{

}




/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void undo_clicked(GtkButton * button, gpointer user_data)
{

}




/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void link_clicked(GtkButton * button, gpointer user_data)
{

}


static GnomeUIInfo thml_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("ThML"),
	 NULL,
	 (gpointer) on_thml_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("GBF"),
	 NULL,
	 (gpointer) on_gbf_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Plain Text"),
	 NULL,
	 (gpointer) on_plain_text_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo format_type_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_RADIOITEMS, NULL, NULL, thml_uiinfo,
	 NULL, NULL, GNOME_APP_PIXMAP_NONE, NULL, 0,
	 (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo pixmapmenuitem1_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Format Type"),
	 NULL,
	 format_type_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("ModOps"),
	 N_("my tips"),
	 (gpointer) modops_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo menubar_options_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Module Options"),
	 NULL,
	 pixmapmenuitem1_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_INDEX,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gbs_dialog.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void create_gbs_dialog(GBS_DIALOG * dlg)
{
	GtkWidget *gbs_dialog;
	GtkWidget *vbox_dialog;
	GtkWidget *toolbar;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *tb_edit;
	GtkWidget *button_new;
	GtkWidget *button_save;
	GtkWidget *vseparator29;
	GtkWidget *button_cut;
	GtkWidget *button_copy;
	GtkWidget *button_past;
	GtkWidget *button_undo;
	GtkWidget *vseparator30;
	GtkWidget *button_link;
	GtkWidget *vseparator32;
	GtkWidget *button_find;
	GtkWidget *button_replace;
	GtkWidget *vseparator31;
	GtkWidget *button26;
	GtkWidget *menubar_options;
	GtkWidget *vseparator28;
	GtkWidget *button_close;
	GtkWidget *frame_gbs;
	GtkWidget *hpaned;
	GtkWidget *scrolledwindow_ctree;
	GtkWidget *ctree_gbs;
	GtkWidget *label241;
	GtkWidget *label242;
	GtkWidget *label243;
	GtkWidget *scrolledwindow_html;
	GtkWidget *statusbar5;

	gbs_dialog = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_object_set_data(GTK_OBJECT(gbs_dialog), "gbs_dialog",
			    gbs_dialog);
	gtk_window_set_title(GTK_WINDOW(gbs_dialog),
			     _("GnomeSword - GBS"));
	gtk_window_set_default_size(GTK_WINDOW(gbs_dialog), 525, 306);

	vbox_dialog = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox_dialog);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "vbox_dialog",
				 vbox_dialog,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox_dialog);
	gtk_container_add(GTK_CONTAINER(gbs_dialog), vbox_dialog);

	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "toolbar",
				 toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), toolbar, FALSE, FALSE,
			   0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_PROPERTIES);
	tb_edit =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Edit"),
				       _("Edit this Book"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(tb_edit);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "tb_edit",
				 tb_edit,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tb_edit);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_NEW);
	button_new =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("New"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_new);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_new",
				 button_new,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_new);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_SAVE);
	button_save =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_save);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_save",
				 button_save,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_save);

	vseparator29 = gtk_vseparator_new();
	gtk_widget_ref(vseparator29);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "vseparator29",
				 vseparator29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator29);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator29,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator29, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_CUT);
	button_cut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Cut"), _("Cut to clipboard"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_cut);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_cut",
				 button_cut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_cut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_COPY);
	button_copy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Copy"),
				       _("Copy to clipboard"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_copy);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_copy",
				 button_copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_copy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_PASTE);
	button_past =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Paste"),
				       _("Paste from clipborad"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_past);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_past",
				 button_past,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_past);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_UNDO);
	button_undo =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Undo"), _("Undo change"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_undo);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_undo",
				 button_undo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_undo);

	vseparator30 = gtk_vseparator_new();
	gtk_widget_ref(vseparator30);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "vseparator30",
				 vseparator30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator30);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator30,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator30, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	button_link =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Link"), _("Add a link"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_link);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_link",
				 button_link,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_link);

	vseparator32 = gtk_vseparator_new();
	gtk_widget_ref(vseparator32);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "vseparator32",
				 vseparator32,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator32);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator32,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator32, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_SEARCH);
	button_find =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Find"), _("Open Find Dialog"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_find);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_find",
				 button_find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_find);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_SRCHRPL);
	button_replace =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Replace"),
				       _
				       ("Open Find and Replace Dialog"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(button_replace);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog),
				 "button_replace", button_replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_replace);

	vseparator31 = gtk_vseparator_new();
	gtk_widget_ref(vseparator31);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "vseparator31",
				 vseparator31,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator31);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator31,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator31, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	button26 =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button8"),
				       _("Start Spell Check"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button26);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button26",
				 button26,
				 (GtkDestroyNotify) gtk_widget_unref);

	menubar_options = gtk_menu_bar_new();
	gtk_widget_ref(menubar_options);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog),
				 "menubar_options", menubar_options,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar_options);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), menubar_options,
				  NULL, NULL);
	gnome_app_fill_menu(GTK_MENU_SHELL(menubar_options),
			    menubar_options_uiinfo, NULL, FALSE, 0);

	gtk_widget_ref(menubar_options_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog),
				 "pixmapmenuitem1",
				 menubar_options_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pixmapmenuitem1_menu_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "format_type",
				 pixmapmenuitem1_menu_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(thml_uiinfo[0].widget);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "thml",
				 thml_uiinfo[0].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (thml_uiinfo[0].widget), TRUE);

	gtk_widget_ref(thml_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "gbf",
				 thml_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(thml_uiinfo[2].widget);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "plain_text",
				 thml_uiinfo[2].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pixmapmenuitem1_menu_uiinfo[1].widget);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog),
				 "checkmenuitem1",
				 pixmapmenuitem1_menu_uiinfo[1].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	vseparator28 = gtk_vseparator_new();
	gtk_widget_ref(vseparator28);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "vseparator28",
				 vseparator28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator28);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator28,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator28, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(gbs_dialog,
				      GNOME_STOCK_PIXMAP_EXIT);
	button_close =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Close"),
				       _("Close this dialog"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(button_close);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "button_close",
				 button_close,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_close);

	frame_gbs = gtk_frame_new(NULL);
	gtk_widget_ref(frame_gbs);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "frame_gbs",
				 frame_gbs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame_gbs);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), frame_gbs, TRUE, TRUE,
			   0);

	hpaned = gtk_hpaned_new();
	gtk_widget_ref(hpaned);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "hpaned",
				 hpaned,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hpaned);
	gtk_container_add(GTK_CONTAINER(frame_gbs), hpaned);
	gtk_paned_set_position(GTK_PANED(hpaned), 179);

	scrolledwindow_ctree = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow_ctree);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog),
				 "scrolledwindow_ctree",
				 scrolledwindow_ctree,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow_ctree);
	gtk_paned_pack1(GTK_PANED(hpaned), scrolledwindow_ctree, FALSE,
			TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_ctree),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	ctree_gbs = gtk_ctree_new(3, 0);
	gtk_widget_ref(ctree_gbs);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "ctree_gbs",
				 ctree_gbs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ctree_gbs);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_ctree),
			  ctree_gbs);
	gtk_clist_set_column_width(GTK_CLIST(ctree_gbs), 0, 600);
	gtk_clist_set_column_width(GTK_CLIST(ctree_gbs), 1, 40);
	gtk_clist_set_column_width(GTK_CLIST(ctree_gbs), 2, 40);
	gtk_clist_column_titles_hide(GTK_CLIST(ctree_gbs));

	label241 = gtk_label_new(_("label241"));
	gtk_widget_ref(label241);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "label241",
				 label241,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label241);
	gtk_clist_set_column_widget(GTK_CLIST(ctree_gbs), 0, label241);

	label242 = gtk_label_new(_("label242"));
	gtk_widget_ref(label242);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "label242",
				 label242,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label242);
	gtk_clist_set_column_widget(GTK_CLIST(ctree_gbs), 1, label242);

	label243 = gtk_label_new(_("label243"));
	gtk_widget_ref(label243);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "label243",
				 label243,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label243);
	gtk_clist_set_column_widget(GTK_CLIST(ctree_gbs), 2, label243);

	scrolledwindow_html = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow_html);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog),
				 "scrolledwindow_html",
				 scrolledwindow_html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow_html);
	gtk_paned_pack2(GTK_PANED(hpaned), scrolledwindow_html, TRUE,
			TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_html),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	statusbar5 = gtk_statusbar_new();
	gtk_widget_ref(statusbar5);
	gtk_object_set_data_full(GTK_OBJECT(gbs_dialog), "statusbar5",
				 statusbar5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar5);
	gtk_box_pack_start(GTK_BOX(vbox_dialog), statusbar5, FALSE,
			   FALSE, 0);

	gtk_signal_connect(GTK_OBJECT(gbs_dialog), "destroy",
			   GTK_SIGNAL_FUNC(dialog_destroy),
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(tb_edit), "toggled",
			   GTK_SIGNAL_FUNC(edit_toggled),
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(button_new), "clicked",
			   GTK_SIGNAL_FUNC(on_button_new_clicked),
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(button_save), "clicked",
			   GTK_SIGNAL_FUNC(on_button_save_clicked),
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(button_cut), "clicked",
			   GTK_SIGNAL_FUNC(cut_clicked),
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(button_copy), "clicked",
			   GTK_SIGNAL_FUNC(copy_clicked),
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(button_past), "clicked",
			   GTK_SIGNAL_FUNC(paste_clicked),
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(button_undo), "clicked",
			   GTK_SIGNAL_FUNC(undo_clicked),
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(button_link), "clicked",
			   GTK_SIGNAL_FUNC(link_clicked),
			   (GBS_DIALOG *) dlg);
			   /*
	gtk_signal_connect(GTK_OBJECT(button_find), "clicked",
			   GTK_SIGNAL_FUNC(on_button_find_clicked),
			   (GBS_DATA *) dlg);
	gtk_signal_connect(GTK_OBJECT(button_replace), "clicked",
			   GTK_SIGNAL_FUNC(on_button_replace_clicked),
			   (GBS_DATA *) dlg);
	gtk_signal_connect(GTK_OBJECT(button26), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSpell_clicked),
			   (GBS_DATA *) dlg);
			   */
	gtk_signal_connect(GTK_OBJECT(button_close), "clicked",
			   GTK_SIGNAL_FUNC(close_clicked),
			   (GBS_DIALOG *) dlg);



	/*  */

	dlg->dialog = gbs_dialog;
	dlg->btn_close = button_close;
	dlg->g->ctree = ctree_gbs;

	dlg->g->html = gtk_html_new();

	gtk_widget_ref(dlg->g->html);
	gtk_object_set_data_full(GTK_OBJECT(dlg->dialog),
				 "dlg->g->->html", dlg->g->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dlg->g->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_html),
			  dlg->g->html);
	gtk_html_load_empty(GTK_HTML(dlg->g->html));
	
	gtk_signal_connect(GTK_OBJECT(dlg->g->ctree), "tree_select_row",
			   GTK_SIGNAL_FUNC(ctree_select_row), 
			   (GBS_DIALOG *) dlg);
	gtk_signal_connect(GTK_OBJECT(dlg->g->html), "on_url",
			   GTK_SIGNAL_FUNC(dialog_url), 
			   (GBS_DATA *) dlg);
/*
	gtk_signal_connect(GTK_OBJECT(dlg->g->html), "link_clicked",
			   GTK_SIGNAL_FUNC(link_clicked), 
			   (GBS_DATA *) dlg);
	gtk_signal_connect(GTK_OBJECT(dlg->g->html),
			   "button_release_event",
			   GTK_SIGNAL_FUNC(button_release),
			   (GBS_DATA *) dlg);
*/

	/*  */
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

void gui_open_gbs_dialog(gchar * mod_name)
{
	GBS_DIALOG *dlg;

	dlg = g_new(GBS_DIALOG, 1);
	dlg->g = g_new(GBS_DATA, 1);
	dlg->g->find_dialog = NULL;
	//dlg->g->mod_num = get_module_number(mod_name, GBS_MODS);
	dlg->g->search_string = NULL;
	dlg->dialog = NULL;
	dlg->g->mod_name = mod_name;
	create_gbs_dialog(dlg);
	gtk_widget_show(dlg->dialog); 
	dialog_list = g_list_append(dialog_list, (GBS_DIALOG *) dlg);
	add_book_to_ctree(dlg->g->ctree, dlg->g->mod_name); 
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

void gui_setup_gbs_dialog(GList * mods)
{
	dialog_list = NULL;
	dialog_freed = FALSE;
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

void gui_shutdown_gbs_dialog(void)
{
	
	dialog_list = g_list_first(dialog_list);
	while (dialog_list != NULL) {
		GBS_DIALOG *dlg = (GBS_DIALOG *) dialog_list->data;
		dialog_freed = TRUE;

		g_warning("shuting down %s dialog", dlg->g->mod_name);
		/* 
		 *  destroy any dialogs created 
		 */
		if (dlg->dialog)
			gtk_widget_destroy(dlg->dialog);
		/* 
		 * free any search dialogs created 
		 */
		if (dlg->g->find_dialog)
			g_free(dlg->g->find_dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		g_free(dlg->g);
		g_free((GBS_DIALOG *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	}
	g_list_free(dialog_list);
}





//******  end of file  ******/
