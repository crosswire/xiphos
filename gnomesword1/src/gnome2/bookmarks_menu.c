/*
 * GnomeSword Bible Study Tool
 * bookmarks_treeview.c - gui for bookmarks using treeview
 *
 * Copyright (C) 2003 GnomeSword Developer Team
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
#include <libxml/parser.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#include "gui/gnomesword.h"
#include "gui/bookmarks_menu.h"
#include "gui/bookmarks_treeview.h"
#include "gui/utilities.h"
#include "gui/main_window.h"
#include "gui/dialog.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/key.h"
#include "main/xml.h"



BOOKMARK_MENU menu;

gboolean bookmarks_changed;

static void save_bookmarks(GtkMenuItem * menuitem, gpointer user_data);

/******************************************************************************
 * Name
 *  parse_treeview
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void parse_treeview(xmlNodePtr parent, GtkTreeIter * tree_parent)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void parse_treeview(xmlNodePtr parent, GtkTreeIter * tree_parent)
{
	static xmlNodePtr cur_node;
	GtkTreeIter child;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	
	gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &child,
                                             tree_parent);
	
	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &child,
			   2, &caption, 3, &key, 4, &module, -1);
		if(gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), 
							&child)) {
			cur_node = xml_add_folder_to_parent(parent, 
							caption);
			parse_treeview(cur_node, &child);
						     
		}
		else 
			xml_add_bookmark_to_parent(parent, 
						caption,
						key,
						module);
		
		g_free(caption);
		g_free(key);
		g_free(module);	
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &child));
}


/******************************************************************************
 * Name
 *   save_iter_to_xml_removed
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_iter_to_xml_removed(GtkTreeIter * iter)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void save_iter_to_xml_removed(GtkTreeIter * iter)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	xmlAttrPtr root_attr;
	const xmlChar *xml_filename;
	gchar *caption = NULL;
	gchar buf[256];
	GtkTreeIter child;
	
	if (!bookmarks_changed) 
		return;
	
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, -1);
	sprintf(buf, "%s/bookmarks/removed/%s.xml", settings.gSwordDir,caption);
	xml_filename = (const xmlChar *) buf;
	root_doc = xmlNewDoc((const xmlChar *) "1.0");
	
	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)
				       "SwordBookmarks");
		root_attr =
		    xmlNewProp(root_node, "syntaxVersion",
			       (const xmlChar *) "1");
		xmlDocSetRootElement(root_doc, root_node);
	}
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, -1);
		if( gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model),iter)) {
			cur_node = xml_add_folder_to_parent(root_node, caption);
			parse_treeview(cur_node, iter);
		}
	g_free(caption);
	xmlSaveFormatFile(xml_filename, root_doc,1);
	xmlFreeDoc(root_doc);
	
	bookmarks_changed = FALSE;
}

/******************************************************************************
 * Name
 *  save_treeview_to_xml_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_treeview_to_xml_bookmarks(GtkTreeIter * iter, gchar *file_buf)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void save_treeview_to_xml_bookmarks(GtkTreeIter * iter, gchar * filename)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	xmlAttrPtr root_attr;
	const xmlChar *xml_filename;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	
	if (!bookmarks_changed) 
		return;
	
	xml_filename = (const xmlChar *) filename;
	root_doc = xmlNewDoc((const xmlChar *) "1.0");
	
	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)
				       "SwordBookmarks");
		root_attr =
		    xmlNewProp(root_node, "syntaxVersion",
			       (const xmlChar *) "1");
		xmlDocSetRootElement(root_doc, root_node);
	}
	
	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, 3, &key, 4, &module, -1);
		if( gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model),iter)) {
			cur_node = xml_add_folder_to_parent(root_node, caption);
			parse_treeview(cur_node, iter);
		}
		else 		
			xml_add_bookmark_to_parent(root_node,  
						caption,
						key,
						module);
		g_free(caption);
		g_free(key);
		g_free(module);	
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model),iter));
	
	xmlSaveFormatFile(xml_filename, root_doc,1);
	xmlFreeDoc(root_doc);
	g_free(filename);	
	bookmarks_changed = FALSE;
}


					    
/******************************************************************************
 * Name
 *  add_node_to_ctree
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   GtkCTreeNode *add_node_to_ctree(GtkCTree * ctree, 
 *			GtkCTreeNode *node, BOOKMARK_DATA * data)	
 *
 * Description
 *    actually add the GtkCTreeNode to the bookmark ctree
 *
 * Return value
 *   GtkCTreeNode
 */

static void add_item_to_tree(GtkTreeIter *iter,GtkTreeIter *parent, 
							BOOKMARK_DATA * data)
{
	gtk_tree_store_append(GTK_TREE_STORE(model), iter,
			      parent);
	
		gtk_tree_store_set(GTK_TREE_STORE(model), iter, 
			   COL_OPEN_PIXBUF, data->opened,
			   COL_CLOSED_PIXBUF, data->closed,
			   COL_CAPTION, data->caption, 
			   COL_KEY, data->key,
			   COL_MODULE, data->module,
			   -1);
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   
 *
 * Description
 *   called gnome_request_dialog to return information from user
 *
 * Return value
 *   void
 */

static void string_callback(gchar * str, gpointer data)
{
/*	gchar *text[3];
	gchar buf[80], buf2[80];
	gint length, i = 0, j = 0;
	GtkCList *clist;
	GtkCTreeNode *first_child;


	if ((str == NULL) || (strlen(str) == 0)) {
		(gchar *) data = NULL;
	} else {
		GtkCTreeNode *newnode;
	
		switch (GPOINTER_TO_INT(data)) {
		case 0:
			text[0] = str;
			text[1] = "GROUP";
			text[2] = "GROUP";
			first_child
			    = GTK_CTREE_ROW(selected_node)->children;
			newnode = gtk_ctree_insert_node(p_bmtree->ctree,
							selected_node,
							first_child,
							text, 3,
							pixmap1, mask1,
							pixmap2, mask2,
							FALSE, FALSE);
			gtk_ctree_sort_node(bmtree.ctree,
					    selected_node);
			gtk_ctree_select(p_bmtree->ctree, newnode);
			gtk_ctree_expand(p_bmtree->ctree,
					 selected_node);
			break;
		case 1:
			clist = GTK_CLIST(p_bmtree->ctree);
			gtk_clist_select_row(clist, 0, 0);
			sprintf(buf, "%s", str);
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
			    gtk_ctree_insert_node(p_bmtree->ctree,
						  selected_node, NULL,
						  text, 3, pixmap1,
						  mask1, pixmap2, mask2,
						  FALSE, FALSE);
			gtk_ctree_sort_node(bmtree.ctree,
					    selected_node);
			gtk_ctree_select(p_bmtree->ctree, newrootnode);
			break;
		}
	}
	*/

}
/******************************************************************************
 * Name
 *   goto_bookmark
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void goto_bookmark(gchar * mod_name, gchar * key)
 *
 * Description
 *   test for module type and call the appropriate function to
 *   display bookmark
 *
 * Return value
 *   void
 */

static void goto_bookmark(gchar * mod_name, gchar * key)
{
	gint module_type;

	if (use_dialog) {
		module_type = get_mod_type(mod_name);
		switch (module_type) {
		case TEXT_TYPE:
			gui_bibletext_dialog_goto_bookmark(mod_name,
							   key);
			break;
		case COMMENTARY_TYPE:
			gui_commentary_dialog_goto_bookmark(mod_name,
							    key);
			break;
		case DICTIONARY_TYPE:
			gui_dictionary_dialog_goto_bookmark(mod_name,
							    key);
			break;
		case BOOK_TYPE:
			gui_gbs_dialog_goto_bookmark(mod_name, key);
			break;
		}
	} else
		gui_change_module_and_key(mod_name, key);
}

/******************************************************************************
 * Name
 *   on_point_to_here_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
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
/*	GS_DIALOG *yes_no_dialog;
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
	yes_no_dialog->title = N_("Bookmark?");
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
		bookmarks_changed = TRUE;
		save_bookmarks(NULL, NULL);
//		gtk_widget_set_sensitive(menu.save, bookmarks_changed);
	}
	g_free(yes_no_dialog);
	*/
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void bibletime_bookmarks_activate(GtkMenuItem * menuitem,
				  gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeIter parent;
	GString *str;
	const xmlChar *file;
	gchar *text[3];

	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model),&parent))
		return;
	
	str = g_string_new(settings.swbmDir);
	g_string_sprintf(str, "%s/%s", settings.homedir,
			 ".kde/share/apps/bibletime/bookmarks.xml");
	
	file = (const xmlChar *) str->str;
	
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter,
			      &parent);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   COL_CAPTION, "BibleTime", 
			   COL_KEY, NULL,
			   COL_MODULE, NULL,
			   -1);	
	parse_bookmarks(bookmark_tree, file, &iter);
	g_string_free(str, TRUE);
}


/******************************************************************************
 * Name
 *   on_allow_reordering_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
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

static void on_allow_reordering_activate(GtkMenuItem * menuitem,
				  gpointer user_data)
{
	 gtk_tree_view_set_reorderable(bookmark_tree,
                           GTK_CHECK_MENU_ITEM(menuitem)->active);
}

/******************************************************************************
 * Name
 *   on_dialog_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_dialog_activate(GtkMenuItem * menuitem,
 *				  gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_dialog_activate(GtkMenuItem * menuitem, gpointer user_data)
{	
	GtkTreeIter selected;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	
	use_dialog = TRUE; 
	
	if (gtk_tree_selection_get_selected(current_selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption, 3, &key, 4, &module, -1);
		goto_bookmark(module, key);
	}
	use_dialog = FALSE;
}


/******************************************************************************
 * Name
 *   on_edit_item_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_edit_item_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   edit bookmark
 *
 * Return value
 *   void
 */

static void on_edit_item_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GS_DIALOG *info;
	BOOKMARK_DATA * data;	
	gint test;
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gboolean is_leaf;
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;	
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption, 3, &key, 4, &module, -1);
	
	info = gui_new_dialog();
	info->title = N_("Bookmark");
	if(gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)) {
		info->stock_icon = "gtk-open";
		info->label_top = N_("Edit folder name");
		info->label1 = "Folder name: ";
		is_leaf = FALSE;		
	}
	else {
		info->stock_icon = "gnome-stock-book-open";
		info->label_top = N_("Edit bookmark");
		info->label1 = "Bookmark name: ";
		info->text2 = g_strdup(key);
		info->text3 = g_strdup(module);
		info->label2 = "Verse: ";
		info->label3 = "Module: ";
		is_leaf = TRUE;		
	}
		
	info->text1 = g_strdup(caption);
	info->ok = TRUE;
	info->cancel = TRUE;

	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		data = g_new(BOOKMARK_DATA,1);
		if(is_leaf) {
			data->opened = pixbufs->pixbuf_helpdoc;
			data->closed = NULL;
		}
		else {				
			data->opened = pixbufs->pixbuf_opened;
			data->closed = pixbufs->pixbuf_closed;	
		}
			
		data->caption = info->text1;
		data->key = info->text2; 
		data->module = info->text3; 
		data->is_leaf = TRUE;
		gtk_tree_store_set(GTK_TREE_STORE(model), &selected, 
			   COL_OPEN_PIXBUF, data->opened,
			   COL_CLOSED_PIXBUF, data->closed,
			   COL_CAPTION, data->caption, 
			   COL_KEY, data->key,
			   COL_MODULE, data->module,
			   -1);	
		bookmarks_changed = TRUE;
		save_bookmarks(NULL, NULL);
		g_free(data);
	}
	g_free(info->text1);	// we used g_strdup() 
	if(info->text2) g_free(info->text2);
	if(info->text3) g_free(info->text3);
	g_free(info);
	g_free(caption);
	g_free(key);
	g_free(module);
}


/******************************************************************************
 * Name
 *   on_remove_folder_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_remove_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   remove folder - and save it
 *
 * Return value
 *   void
 */

static void on_remove_folder_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	GtkTreeIter iter;
	gchar buf[256];
	gchar *caption = NULL;
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption, -1);

	sprintf(buf, "%s/removed/%s.xml", settings.swbmDir,
		caption);
	g_warning(buf);

	bookmarks_changed = TRUE;
	save_iter_to_xml_removed(&selected);
	gtk_tree_store_remove(GTK_TREE_STORE(model), &selected);
	bookmarks_changed = TRUE;
	save_bookmarks(NULL, NULL);
	g_free(caption);
}


/******************************************************************************
 * Name
 *   restore_ok
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *   
 *   void restore_ok(GtkButton *button, gpointer data);
 *
 * Description
 *   OK button for restore fileselection was clicked.
 *
 * Return value
 *   void
 */

static void restore_ok(GtkButton * button, GtkWidget * filesel)
{
	const xmlChar *file;
	gchar buf[256];
	
	file = (const xmlChar *)
	    gtk_file_selection_get_filename(GTK_FILE_SELECTION
					    (filesel));	
	gui_load_removed(file);
	
	bookmarks_changed = TRUE;
	save_bookmarks(NULL, NULL);
	gtk_widget_destroy(filesel);
}


/******************************************************************************
 * Name
 *   restore_cancel
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *   
 *   void restore_cancel(GtkButton *button, gpointer data);
 *
 * Description
 *   Cancel button for restore fileselection was clicked.
 *
 * Return value
 *   void
 */

static void restore_cancel(GtkButton * button, GtkWidget * dlg)
{
	gtk_widget_destroy(dlg);
}


/******************************************************************************
 * Name
 *   on_restore_folder_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_restore_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   restore a saved bookmark folder
 *
 * Return value
 *   void
 */

static void on_restore_folder_activate(GtkMenuItem * menuitem,
				gpointer user_data)
{
	GtkWidget *fileselection;
	GtkWidget *ok;
	GtkWidget *cancel;
	gchar buf[256];

	fileselection =
	    gtk_file_selection_new(_("Restore Bookmark Folder"));
	gtk_container_set_border_width(GTK_CONTAINER(fileselection),
				       10);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION
					       (fileselection));

	ok = GTK_FILE_SELECTION(fileselection)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselection), "ok", ok);
	gtk_widget_show(ok);
	GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);

	cancel = GTK_FILE_SELECTION(fileselection)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselection), "cancel",
			    cancel);
	gtk_widget_show(cancel);
	GTK_WIDGET_SET_FLAGS(cancel, GTK_CAN_DEFAULT);

	sprintf(buf, "%s/removed/*.xml", settings.swbmDir);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION
					(fileselection), buf);

	gtk_widget_show(fileselection);

	gtk_signal_connect(GTK_OBJECT(ok), "clicked",
			   GTK_SIGNAL_FUNC(restore_ok),
			   (GtkWidget *) fileselection);
	gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
			   GTK_SIGNAL_FUNC(restore_cancel),
			   (GtkWidget *) fileselection);

}


/******************************************************************************
 * Name
 *   on_delete_item_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_delete_item_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   delete bookmark - if a group delete all in the group
 *
 * Return value
 *   void
 */

static void on_delete_item_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gint test;
	GS_DIALOG *yes_no_dialog;
	gchar *name_string;
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	GString *str;
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption, 3, &key, 4, &module, -1);
	name_string = caption;

	str = g_string_new("");
	yes_no_dialog = gui_new_dialog();
	yes_no_dialog->stock_icon = "gtk-dialog-warning";
	yes_no_dialog->title = N_("Bookmark");	
	if(gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)) {
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s %s",
			_("Remove the selected folder"),
			name_string,
			_("(and all its contents)?"));
	}
	else {
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			_("Remove the selected bookmark"),
			name_string);
	}
	yes_no_dialog->label_top = str->str;
	yes_no_dialog->yes = TRUE;
	yes_no_dialog->no = TRUE;

	test = gui_alert_dialog(yes_no_dialog);
	if (test == GS_YES) {
		gtk_tree_store_remove(GTK_TREE_STORE(model), &selected);
		bookmarks_changed = TRUE;
		save_bookmarks(NULL, NULL);
	}
	g_free(yes_no_dialog);
	g_free(caption);
	g_free(key);
	g_free(module);
	g_string_free(str,TRUE);
}


/******************************************************************************
 * Name
 *   save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

void save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
{
	
	GtkTreeIter root;
	GtkTreeIter first_child;
	gchar buf[256];
	
	if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &root))
		return;
	if(!gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &first_child,
                                             &root))
		return;

	sprintf(buf, "%s/bookmarks/%s", settings.gSwordDir,"bookmarks.xml");
	save_treeview_to_xml_bookmarks(&first_child, g_strdup(buf));
}

/******************************************************************************
 * Name
 *   gui_save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void gui_save_bookmarks(void)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

void gui_save_bookmarks_treeview(void)
{
	save_bookmarks(NULL, NULL);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
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
	gtk_tree_view_expand_all(bookmark_tree);
}


/******************************************************************************
 * Name
 *  on_collapse_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
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
 	gtk_tree_view_collapse_all(bookmark_tree);	
}


/******************************************************************************
 * Name
 *  on_add_bookmark_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
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
	GtkTreeIter selected;
	GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *mod_name = NULL;
	gint test;
	BOOKMARK_DATA * data;
	GS_DIALOG *info;
	gchar buf[256];
	
	
	if (gtk_tree_selection_get_selected(current_selection, NULL, &selected)) {
		mod_name = get_module_name();
		key = get_module_key();		
		data = g_new(BOOKMARK_DATA,1);
		info = gui_new_dialog();
		//info->title = N_("Bookmark");
		info->stock_icon = "gtk-add";
		info->label_top = N_("Add Bookmark");
		sprintf(buf, "%s, %s", key, mod_name);
	
		info->text1 = g_strdup(buf);
		info->text2 = g_strdup(key);
		info->text3 = g_strdup(mod_name);
		info->label1 = N_("Label: ");
		info->label2 = N_("Verse: ");
		info->label3 = N_("Module: ");
		info->ok = TRUE;
		info->cancel = TRUE;
	
		test = gui_gs_dialog(info);
		if (test == GS_OK) {
			data->caption = info->text1;
			data->key = info->text2; 
			data->module = info->text3; 
			data->is_leaf = TRUE;
			data->opened = pixbufs->pixbuf_helpdoc;
			data->closed = NULL;			
			add_item_to_tree(&iter,&selected, data);			
			bookmarks_changed = TRUE;
			save_bookmarks(NULL, NULL);
		}
		g_free(info->text1);	/* we used g_strdup() */
		g_free(info->text2);
		g_free(info->text3);
		g_free(info);
	}
}

/******************************************************************************
 * Name
 *  on_insert_bookmark_activate 
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
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
	on_add_bookmark_activate(menuitem,NULL);
}


/******************************************************************************
 * Name
 *   on_new_subgroup_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
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

static void on_new_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	char *t, *buf;
	gint test;
	GS_DIALOG *info;
	BOOKMARK_DATA * data;
	
	if(!gtk_tree_selection_get_selected(current_selection, NULL, &selected)) 
		return;
	
	t = "|";
	info = gui_new_dialog();
	info->stock_icon = "gtk-open";
	info->title = N_("Bookmark");
	info->label_top = N_("Enter Folder Name");
	info->label_middle = N_("(use no \'|\')");
	info->text1 = g_strdup(_("Folder Name"));
	info->label1 = N_("Folder: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	
	data = g_new(BOOKMARK_DATA,1);
	/*** open dialog to get name for new folder ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {	
		buf = g_strdelimit(info->text1, t, ' ');
		data->caption = g_strdup(buf);
		data->key = NULL; 
		data->module = NULL; 
		data->is_leaf = FALSE;
		data->opened = pixbufs->pixbuf_opened;
		data->closed = pixbufs->pixbuf_closed;			
		add_item_to_tree(&iter,&selected, data);
		bookmarks_changed = TRUE;
		save_bookmarks(NULL, NULL);
	}
	if(data->caption) g_free(data->caption);
	g_free(data);
	g_free(info->text1);
	g_free(info);
}




/******************************************************************************
 * Name
 *   pmBookmarkTree_uiinfo
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   GnomeUIInfo pmBookmarkTree_uiinfo[] 
 *
 * Description
 *  gnome menu structure for bookmarks popup menu
 *  !! CHANGING MENU STRUCTURE DON'T FORGET ALSO CHANGE create_bookmark_menu !!
 *
 */

static GnomeUIInfo rr_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Remove Folder"),
	 N_("Remove folder and save it"),
	 on_remove_folder_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_CUT,
	 0, 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Restore Folder"),
	 N_("Restore saved folder"),
	 on_restore_folder_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_PASTE,
	 0, 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo pmBookmarkTree_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Open in a dialog"),
	 N_("Open this bookmark in a dialog"),
	 (gpointer) on_dialog_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_OPEN,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("New Folder"),
	 N_("Add new Folder to selected Folder"),
	 (gpointer) on_new_folder_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_NEW,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Insert Bookmark"),
	 N_("Insert new bookmark here"),
	 (gpointer) on_insert_bookmark_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_NEW,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Add Bookmark at Root"),
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
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Allow Reordering"),
	 N_
	 ("Toggle Reording - allow items to be moved from one folder to another"),
	 (gpointer) on_allow_reordering_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_ITEM, N_("Bibletime Bookmarks"),
	 N_("Load Bibletime Bookmarks"),
	 (gpointer) bibletime_bookmarks_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Remove - Restore"),
	 NULL,
	 rr_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_MENU_PROP,
	 0, 0, NULL},
	GNOMEUIINFO_END
};


/******************************************************************************
 * Name
 *   create_bookmark_menu
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void create_bookmark_menu(void)
 *
 * Description
 *   create bookmark tree popup menu
 *  !! CHANGING MENU STRUCTURE DON'T FORGET ALSO CHANGE create_bookmark_menu !!
 *
 * Return value
 *   void
 */

void gui_create_bookmark_menu(void)
{
	menu.menu = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(menu.menu),
			    "menu.menu", menu.menu);
	gnome_app_fill_menu(GTK_MENU_SHELL(menu.menu),
			    pmBookmarkTree_uiinfo, NULL, FALSE, 0);

	menu.in_dialog = pmBookmarkTree_uiinfo[0].widget;
	menu.new = pmBookmarkTree_uiinfo[1].widget;
	menu.insert = pmBookmarkTree_uiinfo[2].widget;
	menu.add = pmBookmarkTree_uiinfo[3].widget;
	menu.edit = pmBookmarkTree_uiinfo[4].widget;
	menu.point = pmBookmarkTree_uiinfo[5].widget;
	menu.delete = pmBookmarkTree_uiinfo[6].widget;
	
	
	
	menu.reorder = pmBookmarkTree_uiinfo[11].widget;

	menu.bibletime = pmBookmarkTree_uiinfo[13].widget;
	menu.rr_submenu = pmBookmarkTree_uiinfo[14].widget;

	menu.remove = rr_menu_uiinfo[0].widget;
	menu.restore = rr_menu_uiinfo[2].widget;

	gtk_widget_set_sensitive(menu.in_dialog, FALSE);
	gtk_widget_set_sensitive(menu.new, FALSE);
	gtk_widget_set_sensitive(menu.insert, FALSE);
	gtk_widget_set_sensitive(menu.add, FALSE);
	gtk_widget_set_sensitive(menu.edit, FALSE);
	gtk_widget_set_sensitive(menu.point, FALSE);
	gtk_widget_set_sensitive(menu.delete, FALSE);
	gtk_widget_set_sensitive(menu.bibletime,
				 settings.have_bibletime);

	gtk_widget_set_sensitive(menu.rr_submenu, FALSE);
	gtk_widget_set_sensitive(menu.remove, TRUE);
	gtk_widget_set_sensitive(menu.restore, TRUE);


	gnome_app_install_menu_hints(GNOME_APP(widgets.app),
				     pmBookmarkTree_uiinfo);
				    
}
