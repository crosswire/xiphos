/*
 * GnomeSword Bible Study Tool
 * bookmarks.c - gui for bookmarks
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
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gui/gnomesword.h"
#include "gui/bookmarks.h"
#include "gui/utilities.h"
#include "gui/main_window.h"
#include "gui/dialog.h"

#include "main/bookmarks.h"
#include "main/settings.h"

/******************************************************************************
 * externals
 *****************************************************************************/

extern GtkCTreeNode *personal_node;
extern gchar *fnquickmarks;
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;


/******************************************************************************
 * globals
 *****************************************************************************/

GtkWidget *pmBookmarkTree;
GtkStyle *style;
BM_TREE bmtree;
BM_TREE *p_bmtree;
GtkCTreeNode *newrootnode;

/******************************************************************************
 * static
 */
static GtkCTreeNode *selected_node;

/*****************************************************************************
 * static function declrations
 */
static void after_press(GtkCTree * ctree, gpointer data);
static void after_move(GtkCTree * ctree,
		       GtkCTreeNode * child,
		       GtkCTreeNode * parent,
		       GtkCTreeNode * sibling, gpointer data);
static void remove_selection(GtkWidget * widget, GtkCTree * ctree);
static void stringCallback(gchar * string, gpointer data);
static void on_add_bookmark_activate(GtkMenuItem * menuitem,
				     gpointer user_data);
static void on_insert_bookmark_activate(GtkMenuItem * menuitem,
					gpointer user_data);
static void on_point_to_here_activate(GtkMenuItem * menuitem,
				      gpointer user_data);
static void on_expand_activate(GtkMenuItem * menuitem,
			       gpointer user_data);
static void on_collapse_activate(GtkMenuItem * menuitem,
				 gpointer user_data);


/*** bookmark tree popup menu items  ***/
/* !! CHANGING THIS DON'T FORGET ALSO CHANGE create_bookmark_menu !! */
static GnomeUIInfo pmBookmarkTree_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("New Group"),
	 N_("Add a new root group and file"),
	 (gpointer) on_add_new_group1_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_NEW,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("New SubGroup"),
	 N_("Add new SubGroup to selected group"),
	 (gpointer) on_new_subgroup_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_NEW,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Insert Item"),
	 N_("Insert new bookmark here"),
	 (gpointer) on_insert_bookmark_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_NEW,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Add Item at Root"),
	 N_("Add new bookmark item"),
	 (gpointer) on_add_bookmark_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_NEW,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("_Edit Item"),
	 N_("Edit bookmark item"),
	 (gpointer) on_edit_item_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_PROP,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Make _Point to New "),
	 N_
	 ("Change the bookmark to point to the current reading point"),
	 (gpointer) on_point_to_here_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_PROP,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Delete Item(s)"),
	 N_("Delete item and it's siblings"),
	 (gpointer) on_delete_item_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_TRASH,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Save Bookmarks"),
	 N_("Save all bookmark files"),
	 (gpointer) gui_save_bookmarks, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_SAVE,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Expand All"),
	 N_("Expand all Bookmarks groups"),
	 (gpointer) on_expand_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_BOTTOM,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Collapse All"),
	 N_("Collapse all Bookmarks groups"),
	 (gpointer) on_collapse_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_TOP,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Allow Reordering"),
	 N_("Toggle Reording - allow items to be moved by draging"),
	 (gpointer) on_allow_reordering_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};


/******************************************************************************
 * Name
 *  get_module_key
 *
 * Synopsis
 *   #include ".h"
 *
 *   gchar *get_module_key(void)	
 *
 * Description
 *    returns module key
 *
 * Return value
 *   gchar *
 */

static gchar *get_module_key(void)
{
	if (settings.havebible) {
		switch (settings.whichwindow) {
		case MAIN_TEXT_WINDOW:
			return (gchar *) settings.currentverse;
			break;
		case COMMENTARY_WINDOW:
			return (gchar *) settings.comm_key;
			break;
		case DICTIONARY_WINDOW:
			return (gchar *) settings.dictkey;
			break;
		case INTERLINEAR_WINDOW:
			return (gchar *) settings.cvInterlinear;
			break;
		case BOOK_WINDOW:
			return (gchar *) settings.book_key;
			break;
		}
	}
	return NULL;
}


/******************************************************************************
 * Name
 *  get_module_name
 *
 * Synopsis
 *   #include ".h"
 *
 *   gchar *get_module_name(void)	
 *
 * Description
 *    returns module name
 *
 * Return value
 *   gchar *
 */

static gchar *get_module_name(void)
{
	if (settings.havebible) {
		switch (settings.whichwindow) {
		case MAIN_TEXT_WINDOW:
			return (gchar *) settings.MainWindowModule;
			break;
		case COMMENTARY_WINDOW:
			return (gchar *) settings.CommWindowModule;
			break;
		case DICTIONARY_WINDOW:
			return (gchar *) settings.DictWindowModule;
			break;
		case BOOK_WINDOW:
			return (gchar *) settings.BookWindowModule;
			break;
		}
	}
	return NULL;
}




/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
/*
static void addnewgrouptoMenus(GtkWidget * pmMenu,
			       GtkCTreeNode * node, gchar * str)
{
	GtkWidget *menu;
	GtkWidget *item;

	menu = gui_lookup_widget(pmMenu, "add_bookmark_menu");
	if (!node)
		return;

	item = gtk_menu_item_new_with_label(str);
	gtk_widget_ref(item);
	gtk_object_set_data_full(GTK_OBJECT(pmMenu), "item",
				 item, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(item);
	gtk_signal_connect(GTK_OBJECT(item), "activate",
			   GTK_SIGNAL_FUNC
			   (on_add_bookmark_activate),
			   (GtkCTreeNode *) node);
	gtk_container_add(GTK_CONTAINER(menu), item);
}
*/
/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void after_press(GtkCTree * ctree, gpointer data)
{

}

gboolean button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
{
	if (event->button == 3) {
		gint row;
		gint column;
		if (gtk_clist_get_selection_info
		    (GTK_CLIST(p_bmtree->ctree), event->x, event->y,
		     &row, &column)) {
			gtk_clist_select_row(GTK_CLIST(p_bmtree->ctree),
					     row, 0);
		}
		gtk_menu_popup(GTK_MENU(pmBookmarkTree),
			       NULL, NULL, NULL, NULL,
			       event->button, event->time);
		return TRUE;
	}

	return FALSE;
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
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
on_ctree_select_row(GtkCTree * ctree,
		    GList * node, gint column, gpointer user_data)
{
	gchar *label, *modName, *key;
	GtkWidget *new_widget, *insert_item_widget;


	selected_node = (GtkCTreeNode *) node;

	new_widget =
	    gtk_object_get_data(GTK_OBJECT(pmBookmarkTree), "new");
	insert_item_widget =
	    gtk_object_get_data(GTK_OBJECT(pmBookmarkTree),
				"insert_item");
	/* if node is leaf we need to change mod and key */
	if (GTK_CTREE_ROW(selected_node)->is_leaf) {
		label =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[0])->text;
		key =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[1])->text;
		modName =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[2])->text;
		gtk_widget_set_sensitive(new_widget, FALSE);
		gtk_widget_set_sensitive(insert_item_widget, FALSE);
		gui_change_module_and_key(modName, key);
	} else {
		gtk_widget_set_sensitive(new_widget, TRUE);
		gtk_widget_set_sensitive(insert_item_widget, TRUE);
	}

}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static
void after_move(GtkCTree * ctree, GtkCTreeNode * child,
		GtkCTreeNode * parent, GtkCTreeNode * sibling,
		gpointer data)
{
	char *source;
	char *target1;
	char *target2;

	gtk_ctree_get_node_info(ctree, child, &source,
				NULL, NULL, NULL, NULL, NULL, NULL,
				NULL);
	if (parent)
		gtk_ctree_get_node_info(ctree, parent, &target1,
					NULL, NULL, NULL, NULL, NULL,
					NULL, NULL);
	if (sibling)
		gtk_ctree_get_node_info(ctree, sibling, &target2,
					NULL, NULL, NULL, NULL, NULL,
					NULL, NULL);

	g_print("Moving \"%s\" to \"%s\" with sibling \"%s\".\n",
		source, (parent) ? target1 : "nil",
		(sibling) ? target2 : "nil");
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void remove_selection(GtkWidget * widget, GtkCTree * ctree)
{
	GtkCList *clist;
	GtkCTreeNode *node;

	clist = GTK_CLIST(ctree);

	gtk_clist_freeze(clist);

	while (clist->selection) {
		node = clist->selection->data;
		/*
		if (GTK_CTREE_ROW(node)->is_leaf) {
			//do nothing
		} else
			gtk_ctree_post_recursive(ctree, node, // FIXME: do we need this
						 (GtkCTreeFunc)
						 count_items, NULL);
		*/
		gtk_ctree_remove_node(ctree, node);

		if (clist->selection_mode == GTK_SELECTION_BROWSE)
			break;
	}

	if (clist->selection_mode == GTK_SELECTION_EXTENDED
	    && !clist->selection && clist->focus_row >= 0) {
		node = gtk_ctree_node_nth(ctree, clist->focus_row);

		if (node)
			gtk_ctree_select(ctree, node);
	}

	gtk_clist_thaw(clist);
	after_press(ctree, NULL);
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   
 *
 * Description
 *   called gnome_request_dialog to return information from user
 *
 * Return value
 *   void
 */

static void stringCallback(gchar * str, gpointer data)
{
	gchar *text[3];
	gchar buf[80], buf2[80];
	gint length, i = 0, j = 0;
	GtkCList *clist;
	
	

	if ((str == NULL) || (strlen(str) == 0)) {
		(gchar *) data = NULL;
	} else {
		GtkCTreeNode *newnode;
		switch (GPOINTER_TO_INT(data)) {
		case 0:
			text[0] = str;
			text[1] = "GROUP";
			text[2] = "GROUP";
			newnode = gtk_ctree_insert_node(p_bmtree->ctree,
							selected_node,
							NULL, text, 3,
							pixmap1, mask1,
							pixmap2, mask2,
							FALSE, FALSE);
			gtk_ctree_select(p_bmtree->ctree, newnode);
			gtk_ctree_expand(p_bmtree->ctree,
					 selected_node);
			break;
		case 1:
			clist = GTK_CLIST(p_bmtree->ctree);
			gtk_clist_select_row(clist, 0, 0);
			sprintf(buf, "%s", str);	  /*** make file name ***/
			text[0] = str;
			length = strlen(buf);
			if (length > 29)
				length = 29;
			while (i < length) {
				if (isalpha(buf[i])) {
					buf2[j] = buf[i];
					++j;
				}
				buf2[j] = '\0';
				++i;
			}
			sprintf(buf, "%s%s", buf2, ".conf");
			text[1] = buf;
			text[2] = "ROOT";
			newrootnode =
			    gtk_ctree_insert_node(p_bmtree->ctree, selected_node,
						  NULL, text, 3,
						  pixmap1, mask1,
						  pixmap2, mask2, FALSE,
						  FALSE);
			gtk_ctree_select(p_bmtree->ctree, newrootnode);
			break;
		}
	}

}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   
 *
 * Description
 *   expand the bookmark tree
 *
 * Return value
 *   void
 */

static void on_expand_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gtk_ctree_expand_recursive(p_bmtree->ctree, NULL);
	after_press(p_bmtree->ctree, NULL);
}


/******************************************************************************
 * Name
 *  on_collapse_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_collapse_activate(GtkMenuItem * menuitem,
 *				 gpointer user_data)
 *
 * Description
 *   collapse the bookmark tree
 *
 * Return value
 *   void
 */

static void on_collapse_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
	gtk_ctree_collapse_recursive(p_bmtree->ctree, NULL);
	after_press(p_bmtree->ctree, NULL);
}


/******************************************************************************
 * Name
 *  on_add_bookmark_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_add_bookmark_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)
 *
 * Description
 *   add bookmark (current mod and key of active window (text, comm, or dict)
 *   to root node chosen by user
 *
 * Return value
 *   void
 */

static void on_add_bookmark_activate(GtkMenuItem * menuitem,
				     gpointer user_data)
{
	GtkCTreeNode *node;
	gchar *mod_name, *key;

	if (user_data)
		node = (GtkCTreeNode *) user_data;
	else
		node =
		    gtk_ctree_node_nth(GTK_CTREE(widgets.ctree_widget),
				       0);
	mod_name = get_module_name();
	key = get_module_key();
	gui_add_bookmark_to_tree(node, mod_name, key);
}

/******************************************************************************
 * Name
 *  on_insert_bookmark_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_insert_bookmark_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_insert_bookmark_activate(GtkMenuItem * menuitem,
					gpointer user_data)
{
	on_add_bookmark_activate(menuitem, selected_node);
}


/******************************************************************************
 * Name
 *   on_new_subgroup_activate
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_new_subgroup_activate(GtkMenuItem * menuitem,
 *			      gpointer user_data)
 *
 * Description
 *   add new sub group to selected group
 *
 * Return value
 *   void
 */

void on_new_subgroup_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
	char *t, *buf;
	gint test;
	GS_DIALOG *info;

	t = "|";
	info = gui_new_dialog();
	info->label_top = N_("Enter SubGroup Name - use no \'|\'");
	info->text1 = g_strdup(_("Group Name"));
	info->label1 = N_("Group: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*** open dialog to get name for root node ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		buf = g_strdelimit(info->text1, t, ' ');
		stringCallback(buf, GINT_TO_POINTER(0));
	}
	g_free(info->text1);
	g_free(info);
}


/******************************************************************************
 * Name
 *   on_add_new_group1_activate
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_add_new_group1_activate(GtkMenuItem * menuitem,
 *				gpointer user_data)
 *
 * Description
 *   add new root group to tree
 *
 * Return value
 *   void
 */

void on_add_new_group1_activate(GtkMenuItem * menuitem,
				gpointer user_data)
{
	char *t, *buf;
	gint test;
	GS_DIALOG *info;

	t = "|";
	info = gui_new_dialog();
	info->label_top = N_("Enter Group Name - use no \'|\'");
	info->text1 = g_strdup(_("Group Name"));
	info->label1 = N_("Group: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*** open dialog to get name for root node ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		buf = g_strdelimit(info->text1, t, ' ');
		stringCallback(buf, GINT_TO_POINTER(1));
	}
	g_free(info->text1);
	g_free(info);
}


struct _ExportStruct {
	gchar *label;
	gchar *key;
	gchar *module;
	gboolean is_leaf;
};

typedef struct _ExportStruct ExportStruct;


/******************************************************************************
 * Name
 *   ctree2gnode
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   gboolean ctree2gnode(GtkCTree * ctree, guint depth,
 *	    GNode * gnode, GtkCTreeNode * cnode, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean ctree2gnode(GtkCTree * ctree, guint depth,
			    GNode * gnode, GtkCTreeNode * cnode,
			    gpointer data)
{
	ExportStruct *es;

	if (!cnode || !gnode)
		return FALSE;

	es = g_new(ExportStruct, 1);
	gnode->data = (ExportStruct *) es;
	es->is_leaf = GTK_CTREE_ROW(cnode)->is_leaf;
	es->label =
	    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(cnode)->row.cell[0])->text;
	es->key =
	    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(cnode)->row.cell[1])->text;
	es->module =
	    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(cnode)->row.cell[2])->text;
	return TRUE;
}


/******************************************************************************
 * Name
 *   gui_save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void gui_save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

void gui_save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
{

	GNode *gnode;
	GtkCTreeNode *node;

	node = GTK_CTREE_NODE(g_list_nth(GTK_CLIST
					 (bmtree.ctree)->row_list, 0));

	gnode = gtk_ctree_export_to_gnode(bmtree.ctree, NULL,
					  NULL, node, ctree2gnode,
					  NULL);
	save_bookmarks(gnode);
}


/******************************************************************************
 * Name
 *   on_edit_item_activate
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_edit_item_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   edit bookmark
 *
 * Return value
 *   void
 */

void on_edit_item_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkCList *clist;
	GtkCTreeNode *node;
	gint test;
	GS_DIALOG *info;

	clist = GTK_CLIST(p_bmtree->ctree);
	gtk_clist_freeze(clist);

	node = clist->selection->data;

	info = gui_new_dialog();
	info->label_top = N_("Edit Bookmark");

	info->text1 =
	    g_strdup(GTK_CELL_PIXTEXT
		     (GTK_CTREE_ROW(node)->row.cell[0])->text);
	info->text2 =
	    g_strdup(GTK_CELL_PIXTEXT
		     (GTK_CTREE_ROW(node)->row.cell[1])->text);
	info->text3 =
	    g_strdup(GTK_CELL_PIXTEXT
		     (GTK_CTREE_ROW(node)->row.cell[2])->text);
	info->label1 = "Label: ";
	info->label2 = "Verse: ";
	info->label3 = "Module: ";
	info->ok = TRUE;
	info->cancel = TRUE;

	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
				 cell[0])->text = g_strdup(info->text1);
		GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
				 cell[1])->text = g_strdup(info->text2);
		GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.
				 cell[2])->text = g_strdup(info->text3);
	}

	gtk_clist_thaw(clist);
	g_free(info->text1);	/* we used g_strdup() */
	g_free(info->text2);
	g_free(info->text3);
	g_free(info);
	after_press(p_bmtree->ctree, NULL);
}


/******************************************************************************
 * Name
 *   on_delete_item_activate
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_delete_item_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   delete bookmark - if a group delete all in the group
 *
 * Return value
 *   void
 */

void on_delete_item_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gint test;
	GS_DIALOG *yes_no_dialog;
	GtkCList *clist;
	gchar *name_string;
	GtkCTreeNode *node;

	clist = GTK_CLIST(p_bmtree->ctree);
	node = clist->selection->data;
	name_string =
	    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.cell[0])->text;

	yes_no_dialog = gui_new_dialog();
	yes_no_dialog->label_top =
	    N_
	    ("Really REMOVE the selected item (and all its subitems)?");
	yes_no_dialog->label_bottom = name_string;
	yes_no_dialog->yes = TRUE;
	yes_no_dialog->no = TRUE;

	test = gui_gs_dialog(yes_no_dialog);
	if (test == GS_YES)
		remove_selection(p_bmtree->ctree_widget,
				 p_bmtree->ctree);
	g_free(yes_no_dialog);
}


/******************************************************************************
 * Name
 *   on_point_to_here_activate
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_point_to_here_activate(GtkMenuItem * menuitem,
				       gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_point_to_here_activate(GtkMenuItem * menuitem,
				      gpointer user_data)
{
	GS_DIALOG *yes_no_dialog;
	gint test;
	gchar *modName, *key;
	GtkCellPixText *cell1, *cell2;
	GtkCList *clist;
	gchar *name_string;
	GtkCTreeNode *node;

	clist = GTK_CLIST(p_bmtree->ctree);
	node = clist->selection->data;
	name_string =
	    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(node)->row.cell[0])->text;

	yes_no_dialog = gui_new_dialog();
	yes_no_dialog->label_top =
	    N_
	    ("Make the selected bookmark point to the current Module and Key?");
	yes_no_dialog->label_bottom = name_string;
	yes_no_dialog->yes = TRUE;
	yes_no_dialog->no = TRUE;

	test = gui_gs_dialog(yes_no_dialog);
	if (test == GS_YES) {
		modName = get_module_name();
		key = get_module_key();
		cell1 =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[1]);
		cell2 =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[2]);
		g_free(cell1->text);
		g_free(cell2->text);
		cell1->text = g_strdup(key);
		cell2->text = g_strdup(modName);
	}
	g_free(yes_no_dialog);
}


/******************************************************************************
 * Name
 *   on_allow_reordering_activate
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_allow_reordering_activate(GtkMenuItem * menuitem,
 *				  gpointer user_data)
 *
 * Description
 *   allow reordering of bookmarks
 *
 * Return value
 *   void
 */

void on_allow_reordering_activate(GtkMenuItem * menuitem,
				  gpointer user_data)
{
	gtk_clist_set_reorderable(GTK_CLIST(p_bmtree->ctree),
				  GTK_CHECK_MENU_ITEM(menuitem)->
				  active);
}


/******************************************************************************
 * Name
 *   gui_add_bookmark_to_tree
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void gui_add_bookmark_to_tree(GtkCTreeNode * node, gchar * modName,
 *			  gchar * verse)
 *
 * Description
 *   add leaf node to a root node
 *
 * Return value
 *   void
 */

void gui_add_bookmark_to_tree(GtkCTreeNode * node, gchar * modName,
			      gchar * verse)
{
	gint test;
	GS_DIALOG *info;
	gchar *text[3], buf[256];
	sprintf(buf, "%s, %s", verse, modName);

	info = gui_new_dialog();
	info->label_top = N_("Add Bookmark");

	info->text1 = g_strdup(buf);
	info->text2 = g_strdup(verse);
	info->text3 = g_strdup(modName);
	info->label1 = N_("Label: ");
	info->label2 = N_("Verse: ");
	info->label3 = N_("Module: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		GtkCTreeNode *newnode;
		text[0] = info->text1;
		text[1] = info->text2;
		text[2] = info->text3;
		newnode = gtk_ctree_insert_node(p_bmtree->ctree,
						node,
						NULL,
						text,
						3,
						pixmap3,
						mask3,
						NULL,
						NULL, TRUE, FALSE);
		gtk_ctree_select(p_bmtree->ctree, newnode);
		if (node)
			gtk_ctree_expand(p_bmtree->ctree, node);
	}
	g_free(info->text1);	/* we used g_strdup() */
	g_free(info->text2);
	g_free(info->text3);
	g_free(info);
}


/******************************************************************************
 * Name
 *   gui_verselist_to_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void gui_verselist_to_bookmarks(GList * list)
 *
 * Description
 *   add results of search to tree as a root node with children
 *
 * Return value
 *   void
 */

void gui_verselist_to_bookmarks(GList * list)
{
	char *token, *text[3], *t, *buf;
	GtkCTreeNode *node;
	GtkCTree *ctree;
	GList *tmp;
	gint test;
	GS_DIALOG *info;

	node = NULL;
	tmp = list;
	t = "|";
	ctree = GTK_CTREE(widgets.ctree_widget);

	info = gui_new_dialog();
	info->label_top = N_("Enter Root Group Name - use no \'|\'");
	info->text1 = g_strdup(_("Group Name"));
	info->label1 = N_("Group: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*** open dialog to get name for root node ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		buf = g_strdelimit(info->text1, t, ' ');
		stringCallback(buf, GINT_TO_POINTER(1));
		while (tmp != NULL) {
			token = strtok((gchar *) tmp->data, t);
			text[0] = token;
			token = strtok(NULL, t);
			text[1] = token;
			token = strtok(NULL, t);
			text[2] = token;
			node = gtk_ctree_insert_node(ctree,
						     newrootnode,
						     node,
						     text,
						     3,
						     pixmap3,
						     mask3,
						     NULL, NULL, TRUE,
						     FALSE);
			tmp = g_list_next(tmp);
		}
	}
	g_free(info->text1);
	g_free(info);
	g_list_free(tmp);
	g_list_free(list);
}

/******************************************************************************
 * Name
 *   gnode2ctree
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   gboolean gnode2ctree(GtkCTree * ctree, guint depth,
 *	    GNode * gnode, GtkCTreeNode * cnode, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean gnode2ctree(GtkCTree * ctree, guint depth,
			    GNode * gnode, GtkCTreeNode * cnode,
			    gpointer data)
{
	ExportStruct *es;
	GdkPixmap *pixmap_closed;
	GdkBitmap *mask_closed;
	GdkPixmap *pixmap_opened;
	GdkBitmap *mask_opened;

	if (!cnode || !gnode || (!(es = gnode->data)))
		return FALSE;

	/*g_warning("bookmark = %s, %s ,%s", es->label, es->key,
	   es->module); */

	if (es->is_leaf) {
		pixmap_closed = NULL;
		mask_closed = NULL;
		pixmap_opened = pixmap3;
		mask_opened = mask3;
	} else {
		pixmap_opened = pixmap1;
		mask_opened = mask1;
		pixmap_closed = pixmap2;
		mask_closed = mask2;
	}

	gtk_ctree_set_node_info(ctree, cnode, es->label, 2,
				pixmap_opened, mask_opened,
				pixmap_closed, mask_closed,
				es->is_leaf, (depth < 2));

	gtk_ctree_node_set_text(ctree, cnode, 1, es->key);
	gtk_ctree_node_set_text(ctree, cnode, 2, es->module);
	g_free(es);
	gnode->data = NULL;

	return TRUE;
}


/******************************************************************************
 * Name
 *   gui_load_bookmark_tree
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void gui_load_bookmark_tree(void)
 *
 * Description
 *   fill bookmark tree
 *
 * Return value
 *   void
 */

void gui_load_bookmark_tree(void)
{
	GtkWidget *new_widget, *insert_item_widget;
	GNode *gnode = NULL;

	bmtree.ctree = GTK_CTREE(widgets.ctree_widget);
	bmtree.ctree_widget = widgets.ctree_widget;
	p_bmtree = &bmtree;

	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->ctree_widget),
				 "button_press_event",
				 GTK_SIGNAL_FUNC(button_press_event),
				 NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->ctree_widget),
				 "button_release_event",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->ctree_widget),
				 "tree_move",
				 GTK_SIGNAL_FUNC(after_move), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->ctree_widget),
				 "end_selection",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->ctree_widget),
				 "toggle_focus_row",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->ctree_widget),
				 "select_all",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->ctree_widget),
				 "unselect_all",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->ctree_widget),
				 "scroll_vertical",
				 GTK_SIGNAL_FUNC(after_press), NULL);

	gnode = load_bookmarks();
	if (gnode) {
		gtk_ctree_insert_gnode(bmtree.ctree, NULL, NULL, gnode,
				       gnode2ctree, NULL);
		g_node_destroy(gnode);
	}


	gtk_signal_connect(GTK_OBJECT(widgets.ctree_widget),
			   "tree_select_row",
			   GTK_SIGNAL_FUNC(on_ctree_select_row),
			   bmtree.ctree);
	gtk_clist_set_row_height(GTK_CLIST(p_bmtree->ctree), 15);
	gtk_ctree_set_spacing(p_bmtree->ctree, 3);
	gtk_ctree_set_indent(p_bmtree->ctree, 8);

	create_bookmark_menu();
	/*
	   gnome_popup_menu_attach(menu, p_bmtree->ctree_widget,
	   (gchar *) "1");
	 */
	new_widget =
	    gtk_object_get_data(GTK_OBJECT(pmBookmarkTree), "new");
	insert_item_widget =
	    gtk_object_get_data(GTK_OBJECT(pmBookmarkTree),
				"insert_item");
	gtk_widget_set_sensitive(new_widget, FALSE);
	gtk_widget_set_sensitive(insert_item_widget, FALSE);
}


/******************************************************************************
 * Name
 *   create_bookmark_menu
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void create_bookmark_menu(void)
 *
 * Description
 *   create bookmark tree popup menu
 *
 * Return value
 *   void
 */

void create_bookmark_menu(void)
{
	int i = 0;

	pmBookmarkTree = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmBookmarkTree),
			    "pmBookmarkTree", pmBookmarkTree);
	gnome_app_fill_menu(GTK_MENU_SHELL(pmBookmarkTree),
			    pmBookmarkTree_uiinfo, NULL, FALSE, 0);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "add_new_group1",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree), "new",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "insert_item",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree), "add_item",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "edit_item",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "point_to_here",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "delete_item",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "save_bookmarks1",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "expand_all",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "collapse_all",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_ref(pmBookmarkTree_uiinfo[i].widget);
	gtk_object_set_data_full(GTK_OBJECT(pmBookmarkTree),
				 "allow_reordering",
				 pmBookmarkTree_uiinfo[i++].widget,
				 (GtkDestroyNotify) gtk_widget_unref);
				 
	gnome_app_install_menu_hints(GNOME_APP(widgets.app), 
				pmBookmarkTree_uiinfo);
}


/******************************************************************************
 * Name
 *   gui_create_add_bookmark_menu
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void gui_create_add_bookmark_menu(GtkWidget * menu,
 *			 GtkWidget * bookmark_menu_widget)
 *
 * Description
 *   
 *   creats an add bookmark menu list of root nodes
 *   so user can chose where to add bookmark
 *   called from 
 *
 * Return value
 *   void
 */

void gui_create_add_bookmark_menu(GtkWidget * menu,
			      GtkWidget * bookmark_menu_widget)
{
	GtkWidget *item;
	GtkCTree *ctree;
	GtkCTreeNode *node;
	gboolean is_leaf;
	gint i;
	ctree = GTK_CTREE(widgets.ctree_widget);

	/* collapse tree so we only iterate through the roots */
	gtk_ctree_collapse_recursive(ctree, NULL);
	for (i = 0; i < GTK_CLIST(ctree)->rows; i++) {
		node = gtk_ctree_node_nth(ctree, i);
		if (!node)
			return;
		gtk_ctree_get_node_info(ctree,
					node,
					NULL,
					NULL,
					NULL,
					NULL,
					NULL, NULL, &is_leaf, NULL);
		if (is_leaf)
			continue;
		item =
		    gtk_menu_item_new_with_label(GTK_CELL_PIXTEXT
						 (GTK_CTREE_ROW(node)->
						  row.cell[0])->text);
		gtk_widget_show(item);
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC
				   (on_add_bookmark_activate),
				   (GtkCTreeNode *) node);
		gtk_container_add(GTK_CONTAINER(bookmark_menu_widget),
				  item);
	}
}

/*
bookmarks from Bibletime
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE DOC>
<SwordBookmarks syntaxVersion="1" >
 <Folder caption="Jesus" >
  <Bookmark modulename="WEB" key="John 1:1" moduledescription="World English Bible" description="" />
  <Folder caption="Son of God" >
   <Bookmark modulename="WEB" key="John 3:16" moduledescription="World English Bible" description="" />
  </Folder>
 </Folder>
 <Bookmark modulename="WEB" key="Revelation of John 1:1" moduledescription="World English Bible" description="" />
</SwordBookmarks>
*/
