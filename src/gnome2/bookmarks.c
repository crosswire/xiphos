/*
 * GnomeSword Bible Study Tool
 * bookmarks.c - gui for bookmarks
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
#include "gui/bookmarks.h"
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


typedef struct _bmtree BM_TREE;
struct _bmtree {
	GtkWidget *bookmark_tree;
	GtkCTree *ctree;
};

/******************************************************************************
 * structures
 */
struct _bookmark_menu {
	GtkWidget *in_dialog;
	GtkWidget *new;
	GtkWidget *insert;
	GtkWidget *edit;
	GtkWidget *point;
	GtkWidget *delete;
	GtkWidget *save;
	GtkWidget *bibletime;
	GtkWidget *rr_submenu;
	GtkWidget *remove;
	GtkWidget *restore;
};
typedef struct _bookmark_menu BOOKMARK_MENU;

struct _bookmark_data {
	gchar *caption;
	gchar *key;
	gchar *module;
	gboolean is_leaf;
};
typedef struct _bookmark_data BOOKMARK_DATA;


/******************************************************************************
 * externals
 */
extern GdkPixmap *pixmap1;
extern GdkPixmap *pixmap2;
extern GdkPixmap *pixmap3;
extern GdkBitmap *mask1;
extern GdkBitmap *mask2;
extern GdkBitmap *mask3;


/******************************************************************************
 * globals
 */
GtkWidget *pmBookmarkTree;


/******************************************************************************
 * static
 */
static GtkCTreeNode *selected_node;
static BOOKMARK_DATA *es;
BOOKMARK_MENU menu;
static BM_TREE bmtree;
static BM_TREE *p_bmtree;
static GtkCTreeNode *newrootnode;
static gboolean button_three;
/* save bookmarks if true  */
static gboolean bookmarks_changed;
/* open bookmark in dialog  */
static gboolean use_dialog;
static gint selected_row;

/*****************************************************************************
 * static function declrations
 */
static void after_press(GtkCTree * ctree, gpointer data);
static void after_move(GtkCTree * ctree,
		       GtkCTreeNode * child,
		       GtkCTreeNode * parent,
		       GtkCTreeNode * sibling, gpointer data);
static void remove_selection(GtkCTree * ctree);
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
static void bibletime_bookmarks_activate(GtkMenuItem * menuitem,
					 gpointer user_data);
static void add_node(xmlNodePtr cur, GtkCTree * ctree,
		     GtkCTreeNode * parent);

void gui_add_bookmark_to_tree(GtkCTreeNode * node,
		   gchar * modName, gchar * verse);

void on_new_folder_activate(GtkMenuItem * menuitem,
			  gpointer user_data);

void on_edit_item_activate(GtkMenuItem * menuitem,
		       gpointer user_data);
void on_delete_item_activate(GtkMenuItem * menuitem,
			 gpointer user_data);
void on_allow_reordering_activate(GtkMenuItem * menuitem,
			      gpointer user_data);
void gui_create_add_bookmark_menu(GtkWidget * menu,
			  GtkWidget * bookmark_menu_widget);			  
		   

/******************************************************************************
 * Name
 *  get_node_data
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void get_node_data(GNode * node)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void get_node_data(GNode * node)
{
	es = g_new(BOOKMARK_DATA, 1);
	es = (BOOKMARK_DATA *) node->data;
}


/******************************************************************************
 * Name
 *  add_xml_folder_to_parent
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   	xmlNodePtr add_xml_folder_to_parent(xmlNodePtr parent, 
 *						BOOKMARK_DATA * es)
 *
 * Description
 *    
 *
 * Return value
 *   xmlNodePtr
 */

static xmlNodePtr add_xml_folder_to_parent(xmlNodePtr parent,
					   BOOKMARK_DATA * es)
{ 
	xmlNodePtr cur_node;
	xmlAttrPtr xml_attr;

	cur_node = xmlNewChild(parent,
			       NULL, (const xmlChar *) "Folder", NULL);
	xml_attr = xmlNewProp(cur_node,
			      "caption", (const xmlChar *) es->caption);
	return cur_node;
}


/******************************************************************************
 * Name
 *  add_xml_bookmark_to_parent
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void add_xml_bookmark_to_parent(xmlNodePtr parent, 
 *						BOOKMARK_DATA * es)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void add_xml_bookmark_to_parent(xmlNodePtr parent,
				       BOOKMARK_DATA * es)
{
	xmlNodePtr xml_node;
	xmlAttrPtr xml_attr;
	gchar *mod_desc = NULL;

	if (es->module) {
		if (strlen(es->module) > 2)
			mod_desc = get_module_description(es->module);

	} else
		es->module = " ";

	if (mod_desc == NULL)
		mod_desc = " ";

	xml_node = xmlNewChild(parent,
			       NULL,
			       (const xmlChar *) "Bookmark", NULL);
	xml_attr = xmlNewProp(xml_node,
			      "modulename",
			      (const xmlChar *) es->module);
	xml_attr =
	    xmlNewProp(xml_node, "key", (const xmlChar *) es->key);
	xml_attr =
	    xmlNewProp(xml_node, "moduledescription",
		       (const xmlChar *) mod_desc);
	xml_attr =
	    xmlNewProp(xml_node, "description",
		       (const xmlChar *) es->caption);
}

/******************************************************************************
 * Name
 *   gui_xml_new_bookmark_file
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void gui_new_xml_bookmark_file(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_new_xml_bookmark_file(void)
{
	xmlDocPtr xml_doc;
	xmlNodePtr xml_root;
	xmlNodePtr xml_node;
	xmlNodePtr xml_folder;
	xmlAttrPtr xml_attr;
	const xmlChar *xml_filename;
	gchar buf[256];
	BOOKMARK_DATA *es, data;

	es = &data;
	sprintf(buf, "%s/%s/bookmarks.xml", settings.gSwordDir,
		"bookmarks");
	xml_filename = (const xmlChar *) buf;
	xml_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (xml_doc == NULL) {
		fprintf(stderr,
			"Document not created successfully. \n");
		return;
	}

	xml_node = xmlNewNode(NULL, (const xmlChar *) "SwordBookmarks");
	xml_attr = xmlNewProp(xml_node, "syntaxVersion",
			      (const xmlChar *) "1");
	xmlDocSetRootElement(xml_doc, xml_node);

	es->caption = "Personal";
	xml_root = add_xml_folder_to_parent(xml_node, es);

	es->caption = "What must I do to be saved?";
	xml_folder = add_xml_folder_to_parent(xml_root, es);

	es->caption = "Acts 16:31";
	es->module = NULL;
	es->key = "Acts 16:31";
	add_xml_bookmark_to_parent(xml_folder, es);

	es->caption = "Eph 2:8";
	es->module = NULL;
	es->key = "Eph 2:8";
	add_xml_bookmark_to_parent(xml_folder, es);

	es->caption = "Romans 1:16";
	es->module = NULL;
	es->key = "Romans 1:16";
	add_xml_bookmark_to_parent(xml_folder, es);


	es->caption = "What is the Gospel?";
	xml_folder = add_xml_folder_to_parent(xml_root, es);

	es->caption = "1 Cor 15:1-4";
	es->module = NULL;
	es->key = "1 Cor 15:1";
	add_xml_bookmark_to_parent(xml_folder, es);


	xmlSaveFormatFile(xml_filename, xml_doc,1);
	xmlFreeDoc(xml_doc);
}


/******************************************************************************
 * Name
 *  parse_bookmark_tree
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void parse_bookmark_tree(GNode * node, xmlNodePtr parent)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void parse_bookmark_tree(GNode * node, xmlNodePtr parent)
{
	static xmlNodePtr cur_node;
	GNode *work;

	for (work = node->children; work; work = work->next) {
		get_node_data(work);
		if (!es->is_leaf) {
			cur_node = add_xml_folder_to_parent(parent, es);
		} else {
			add_xml_bookmark_to_parent(parent, es);
		}
		g_free(es);

		parse_bookmark_tree(work, cur_node);
	}
}


/******************************************************************************
 * Name
 *  save_gnode_to_xml_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void save_gnode_to_xml_bookmarks(GNode * node, gchar *file_buf)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void save_gnode_to_xml_bookmarks(GNode * gnode, gchar * file_buf)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	xmlAttrPtr root_attr;
	const xmlChar *xml_filename;

	if (bookmarks_changed) {

		if (!gnode)
			return;
		xml_filename = (const xmlChar *) file_buf;
		root_doc = xmlNewDoc((const xmlChar *) "1.0");
		/*xmlNewDocProp(root_doc,"encoding",
					 "UTF-8");*/
		
		if (root_doc != NULL) {
			root_node = xmlNewNode(NULL, (const xmlChar *)
					       "SwordBookmarks");
			root_attr =
			    xmlNewProp(root_node, "syntaxVersion",
				       (const xmlChar *) "1");
			xmlDocSetRootElement(root_doc, root_node);
		}

		es = (BOOKMARK_DATA *) gnode->data;
/*		g_warning("bookmark = %s - %s - %s", es->caption,
			  es->key, es->module);
*/
		if ((!g_strcasecmp(es->module, "ROOT")) ||
		    (!g_strcasecmp(es->module, "GROUP"))) {
			cur_node =
			    add_xml_folder_to_parent(root_node, es);
			parse_bookmark_tree(gnode, cur_node);
		} else {
			add_xml_bookmark_to_parent(root_node, es);
		}

		while ((gnode = g_node_next_sibling(gnode)) != NULL) {
			get_node_data(gnode);
			if (!es->is_leaf) {
				cur_node =
				    add_xml_folder_to_parent(root_node,
							     es);
				parse_bookmark_tree(gnode, cur_node);
			} else {
				if (root_doc != NULL) {
					add_xml_bookmark_to_parent
					    (root_node, es);
				}
			}
		}
		g_print("\nsaving = %s\n", xml_filename);
		xmlSaveFormatFile(xml_filename, root_doc,1);
		xmlFreeDoc(root_doc);
		g_free(file_buf);
	}

	bookmarks_changed = FALSE;
	gtk_widget_set_sensitive(menu.save, bookmarks_changed);
}



/******************************************************************************
 * Name
 *  add_node_to_ctree
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
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

static GtkCTreeNode *add_node_to_ctree(GtkCTree * ctree,
				       GtkCTreeNode * node,
				       BOOKMARK_DATA * data)
{
	gchar *text[3];
	GdkPixmap *pixmap_closed;
	GdkBitmap *mask_closed;
	GdkPixmap *pixmap_opened;
	GdkBitmap *mask_opened;

	text[0] = data->caption;
	text[1] = data->key;
	text[2] = data->module;

	if (data->is_leaf) {
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

	return gtk_ctree_insert_node(ctree,
				     node,
				     NULL,
				     text,
				     3,
				     pixmap_opened,
				     mask_opened,
				     pixmap_closed,
				     mask_closed, data->is_leaf, FALSE);
}

/******************************************************************************
 * Name
 *   get_xml_folder_data
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void get_xml_folder_data(xmlNodePtr cur, BOOKMARK_DATA * data)
 *
 * Description
 *    get date from xml bookmark folder and put into 
 *    BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void get_xml_folder_data(xmlNodePtr cur, BOOKMARK_DATA * data)
{
	xmlChar *folder;

	folder = xmlGetProp(cur, "caption");
	data->caption = g_strdup(folder);
	data->key = g_strdup("GROUP");
	data->module = g_strdup("GROUP");
	data->is_leaf = FALSE;
}

/******************************************************************************
 * Name
 *    get_xml_bookmark_data
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void get_xml_bookmark_data(xmlNodePtr cur, BOOKMARK_DATA * data)	
 *
 * Description
 *    get date from xml bookmark and put into BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void get_xml_bookmark_data(xmlNodePtr cur, BOOKMARK_DATA * data)
{
	xmlChar *mod1;
	xmlChar *key;
	xmlChar *caption;
	gchar buf[500];

	mod1 = xmlGetProp(cur, "modulename");
	key = xmlGetProp(cur, "key");
	caption = xmlGetProp(cur, "caption");	/* needed to load early devel release */
	if (caption) {
		if (strlen(caption) > 0) {
			data->caption = g_strdup(caption);
		} else {
			sprintf(buf, "%s, %s", key, mod1);
			data->caption = g_strdup(buf);
		}
	} else {
		caption = xmlGetProp(cur, "description");
		if (caption) {
			if (strlen(caption) > 0) {
				data->caption = g_strdup(caption);
			} else {
				sprintf(buf, "%s, %s", key, mod1);
				data->caption = g_strdup(buf);
			}
		} else
			data->caption = g_strdup(key);
	}


	data->key = g_strdup(key);
	data->module = g_strdup(mod1);
	data->is_leaf = TRUE;
}


/******************************************************************************
 * Name
 *  free_bookmark_data
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void free_bookmark_data(BOOKMARK_DATA *data)
 *
 * Description
 *    frees the BOOKMARK_DATA structure
 *
 * Return value
 *   void
 */

static void free_bookmark_data(BOOKMARK_DATA * data)
{
	g_free(data->caption);
	g_free(data->key);
	g_free(data->module);
}


/******************************************************************************
 * Name
 *  add_node
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void add_node(xmlDocPtr doc, xmlNodePtr cur, GtkCTree * ctree,
						GtkCTreeNode *node)	
 *
 * Description
 *    parse the xml bookmarks and add to bookmark ctree
 *
 * Return value
 *   void
 */

static void add_node(xmlNodePtr cur, GtkCTree * ctree,
		     GtkCTreeNode * node)
{
	GtkCTreeNode *sibling = NULL;
	static GtkCTreeNode *parent = NULL;
	BOOKMARK_DATA data, *p = NULL;
	xmlNodePtr work = NULL;

	p = &data;
	if (cur == NULL)
		return;

	for (work = cur->xmlChildrenNode; work; work = work->next) {
		if (!xmlStrcmp
		    (work->name, (const xmlChar *) "Bookmark")) {
			get_xml_bookmark_data(work, p);
			sibling = add_node_to_ctree(ctree, node, p);
			free_bookmark_data(p);
			add_node(work, ctree, node);
		} else
		    if (!xmlStrcmp
			(work->name, (const xmlChar *) "Folder")) {
			get_xml_folder_data(work, p);
			parent = add_node_to_ctree(ctree, node, p);
			free_bookmark_data(p);
			add_node(work, ctree, parent);
		}
	}
}


/******************************************************************************
 * Name
 *  parse_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void parse_bookmarks(GtkCTree * ctree)	
 *
 * Description
 *    load a xml bookmark file
 *
 * Return value
 *   void
 */

static void parse_bookmarks(GtkCTree * ctree, const xmlChar * file,
			    GtkCTreeNode * root_node)
{
	xmlDocPtr doc;
	xmlNodePtr cur = NULL;
	BOOKMARK_DATA data, *p = NULL;
	GtkCTreeNode *node;

	p = &data;

	doc = xmlParseFile(file);

	if (doc == NULL) {
		fprintf(stderr, "Document not parsed successfully. \n");
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr, "empty document \n");
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "SwordBookmarks")) {
		fprintf(stderr,
			"wrong type, root node != SwordBookmarks\n");
		xmlFreeDoc(doc);
		return;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *) "Bookmark")) {
			get_xml_bookmark_data(cur, p);
			add_node_to_ctree(ctree, root_node, p);
			free_bookmark_data(p);
		} else {
			get_xml_folder_data(cur, p);
			if(p->caption) 
				node = add_node_to_ctree(ctree, root_node, p);
			free_bookmark_data(p);
			add_node(cur, ctree, node);
		}

		if (cur->next)
			cur = cur->next;
		else
			break;
	}
	xmlFreeDoc(doc);
	gtk_ctree_expand_to_depth(ctree, root_node, 1);
}

/******************************************************************************
 * Name
 *  load_xml_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void load_xml_bookmarks(GtkCTree * ctree)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void load_xml_bookmarks(GtkCTree * ctree, GtkCTreeNode * node)
{
	GString *str;
	const xmlChar *file;

	str = g_string_new(settings.swbmDir);
	g_string_sprintf(str, "%s/bookmarks.xml", settings.swbmDir);
	file = (const xmlChar *) str->str;
	parse_bookmarks(ctree, file, node);
	g_string_free(str, TRUE);
}


/******************************************************************************
 * Name
 *  get_module_key
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
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
 *   #include "gui/bookmarks.h"
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
			return (gchar *) xml_get_value("modules",
						       "bible");
			break;
		case COMMENTARY_WINDOW:
			return (gchar *) xml_get_value("modules",
						       "comm");
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

	item = gtk_menu_item_new_with_caption(str);
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
 *   after_press
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void after_press(GtkCTree * ctree, gpointer data)
 *
 * Description
 *   this does nothing :(
 *
 * Return value
 *   void
 */

static void after_press(GtkCTree * ctree, gpointer data)
{

}


/******************************************************************************
 * Name
 *   button_press_event
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   gboolean button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
 *
 * Description
 *   catch button 3 and select the row the pointer is over
 *   but does not display the bookmark 
 *
 * Return value
 *   void
 */

gboolean button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data)
{
	if (event->button == 3) {
		//gint row;
		gint column;
		button_three = TRUE;
		if (gtk_clist_get_selection_info
		    (GTK_CLIST(p_bmtree->ctree), event->x, event->y,
		     &selected_row, &column)) {
			gtk_clist_select_row(GTK_CLIST(p_bmtree->ctree),
					     selected_row, 0);
		}
		gtk_menu_popup(GTK_MENU(pmBookmarkTree),
			       NULL, NULL, NULL, NULL,
			       event->button, event->time);
		button_three = FALSE;
		return TRUE;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   goto_bookmark
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
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

void goto_bookmark(gchar * mod_name, gchar * key)
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
 *   on_ctree_select_row
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_ctree_select_row(GtkCTree * ctree,
 *		    GList * node, gint column, gpointer user_data)
 *
 * Description
 *   get information from ctree node and sent it to goto_bookmark
 *  also set sensitivity of menu items
 *
 * Return value
 *   void
 */

static void on_ctree_select_row(GtkCTree * ctree,
				GList * node, gint column,
				gpointer user_data)
{
	gchar *mod_name, *key;

	selected_node = (GtkCTreeNode *) node;
	/* if node is leaf we need to change mod and key */
	if (GTK_CTREE_ROW(selected_node)->is_leaf) {
		key =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[1])->text;
		mod_name =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[2])->text;
		if (strlen(mod_name) < 3)
			mod_name = xml_get_value("modules", "bible");
		gtk_widget_set_sensitive(menu.in_dialog, TRUE);
		gtk_widget_set_sensitive(menu.new, FALSE);
		gtk_widget_set_sensitive(menu.insert, FALSE);
		gtk_widget_set_sensitive(menu.point, TRUE);
		gtk_widget_set_sensitive(menu.rr_submenu, FALSE);
		if (!button_three)
			goto_bookmark(mod_name, key);
	} else {
		gtk_widget_set_sensitive(menu.in_dialog, FALSE);
		gtk_widget_set_sensitive(menu.new, TRUE);
		gtk_widget_set_sensitive(menu.insert, TRUE);
		gtk_widget_set_sensitive(menu.point, FALSE);
		gtk_widget_set_sensitive(menu.rr_submenu, TRUE);

		if (strcmp
		    (GTK_CELL_PIXTEXT
		     (GTK_CTREE_ROW(selected_node)->row.cell[0])->text,
		     "Bookmarks"))
			gtk_widget_set_sensitive(menu.remove, TRUE);
		else
			gtk_widget_set_sensitive(menu.remove, FALSE);

		if (!button_three)
			gtk_ctree_toggle_expansion(ctree,
						   selected_node);
	}
	gtk_widget_set_sensitive(menu.edit, TRUE);
	gtk_widget_set_sensitive(menu.delete, TRUE);

}

/******************************************************************************
 * Name
 *   after_move
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void after_move(GtkCTree * ctree, GtkCTreeNode * child,
 *		GtkCTreeNode * parent, GtkCTreeNode * sibling,
 *		gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void after_move(GtkCTree * ctree, GtkCTreeNode * child,
		       GtkCTreeNode * parent, GtkCTreeNode * sibling,
		       gpointer data)
{
//      GtkCTreeNode * node;
	char *source;
	char *target1;
	char *target2;
	gboolean child_is_leaf;
//      gboolean node_is_leaf;
	gboolean sibling_is_leaf;

	gtk_ctree_get_node_info(ctree, child, &source,
				NULL, NULL, NULL, NULL, NULL,
				&child_is_leaf, NULL);
	if (parent)
		gtk_ctree_get_node_info(ctree, parent, &target1,
					NULL, NULL, NULL, NULL, NULL,
					NULL, NULL);
	if (sibling)
		gtk_ctree_get_node_info(ctree, sibling, &target2,
					NULL, NULL, NULL, NULL, NULL,
					&sibling_is_leaf, NULL);
/*
	if(parent && child_is_leaf && !sibling_is_leaf)
		gtk_ctree_move(ctree, child, parent,NULL);
	if(parent && !child_is_leaf) {
		node = GTK_CTREE_NODE_PREV(child);
		gtk_ctree_get_node_info(ctree, node,NULL,
				NULL, NULL, NULL, NULL, NULL,
				&node_is_leaf, NULL);
		if(node_is_leaf)
			gtk_ctree_move(ctree, child, parent,node);
	}
*/
	g_print("Moving \"%s\" to \"%s\" with sibling \"%s\".\n",
		source, (parent) ? target1 : "nil",
		(sibling) ? target2 : "nil");
	bookmarks_changed = TRUE;
	gtk_widget_set_sensitive(menu.save, bookmarks_changed);
	gtk_ctree_sort_node(ctree, parent);
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

static void remove_selection(GtkCTree * ctree)
{
	GtkCList *clist;
	GtkCTreeNode *node;

	clist = GTK_CLIST(ctree);

	gtk_clist_freeze(clist);

	while (clist->selection) {
		node = clist->selection->data;
		gtk_ctree_remove_node(ctree, node);

		if (clist->selection_mode == GTK_SELECTION_BROWSE)
			break;
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
			sprintf(buf, "%s", str);/*** make file name ***/
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
		    gtk_ctree_node_nth(GTK_CTREE(widgets.bookmark_tree),
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

void on_new_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	char *t, *buf;
	gint test;
	GS_DIALOG *info;

	t = "|";
	info = gui_new_dialog();
	info->label_top = N_("Enter Folder Name - use no \'|\'");
	info->text1 = g_strdup(_("Folder Name"));
	info->label1 = N_("Folder: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*** open dialog to get name for root node ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		buf = g_strdelimit(info->text1, t, ' ');
		stringCallback(buf, GINT_TO_POINTER(0));
		bookmarks_changed = TRUE;
		gtk_widget_set_sensitive(menu.save, bookmarks_changed);
	}
	g_free(info->text1);
	g_free(info);
}


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
	BOOKMARK_DATA *es;

	if (!cnode || !gnode)
		return FALSE;

	es = g_new(BOOKMARK_DATA, 1);
	gnode->data = (BOOKMARK_DATA *) es;
	es->is_leaf = GTK_CTREE_ROW(cnode)->is_leaf;
	es->caption =
	    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(cnode)->row.cell[0])->text;
	es->key =
	    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(cnode)->row.cell[1])->text;
	es->module =
	    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(cnode)->row.cell[2])->text;
	return TRUE;
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

void bibletime_bookmarks_activate(GtkMenuItem * menuitem,
				  gpointer user_data)
{
	GString *str;
	const xmlChar *file;
	gchar *text[3];
	GtkCTreeNode *node;

	str = g_string_new(settings.swbmDir);
	g_string_sprintf(str, "%s/%s", settings.homedir,
			 ".kde/share/apps/bibletime/bookmarks.xml");
	text[0] = "BibleTime";
	text[1] = str->str;
	text[2] = "ROOT";
	node =
	    gtk_ctree_insert_node(bmtree.ctree, NULL, NULL,
				  text, 3, pixmap1, mask1,
				  pixmap2, mask2, FALSE, FALSE);
	file = (const xmlChar *) str->str;
	parse_bookmarks(bmtree.ctree, file, node);
	g_string_free(str, TRUE);

}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void (GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

void gui_save_old_bookmarks_to_new(GNode * gnode)
{
	gchar buf[256];

	sprintf(buf, "%s/bookmarks/%s", settings.gSwordDir,
		"bookmarks.xml");
	bookmarks_changed = TRUE;
	save_gnode_to_xml_bookmarks(gnode, g_strdup(buf));
}





/******************************************************************************
 * Name
 *   save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

static void save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
{

	GNode *gnode;
	GtkCTreeNode *node;
	gchar buf[256];

	sprintf(buf, "%s/bookmarks/%s", settings.gSwordDir,
		"bookmarks.xml");

	node = GTK_CTREE_NODE(g_list_nth(GTK_CLIST
					 (bmtree.ctree)->row_list, 0));

	gnode = gtk_ctree_export_to_gnode(bmtree.ctree, NULL,
					  NULL, node, ctree2gnode,
					  NULL);
	save_gnode_to_xml_bookmarks(g_node_first_child(gnode),
				    g_strdup(buf));
}

/******************************************************************************
 * Name
 *   gui_save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void gui_save_bookmarks(void)
 *
 * Description
 *   save bookmark tree 
 *
 * Return value
 *   void
 */

void gui_save_bookmarks(void)
{
	save_bookmarks(NULL, NULL);
	
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
		bookmarks_changed = TRUE;
		gtk_widget_set_sensitive(menu.save, bookmarks_changed);
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
 *   on_remove_folder_activate
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void on_remove_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   remove folder - and save it
 *
 * Return value
 *   void
 */

void on_remove_folder_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
	gchar buf[256];
	GNode *gnode;
	GtkCTreeNode *node;
	BOOKMARK_DATA *bd;

	node = GTK_CTREE_NODE(g_list_nth(GTK_CLIST
					 (bmtree.ctree)->row_list,
					 selected_row));

	gnode = gtk_ctree_export_to_gnode(bmtree.ctree, NULL,
					  NULL, node, ctree2gnode,
					  NULL);
	bd = (BOOKMARK_DATA *) gnode->data;
	sprintf(buf, "%s/removed/%s.xml", settings.swbmDir,
		bd->caption);
	g_warning(buf);

	bookmarks_changed = TRUE;
	save_gnode_to_xml_bookmarks(gnode, g_strdup(buf));
	remove_selection(bmtree.ctree);
	bookmarks_changed = TRUE;
}


/******************************************************************************
 * Name
 *   restore_ok
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
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
	GtkCTreeNode *node;
	const xmlChar *file;

	node = GTK_CTREE_NODE(g_list_nth(GTK_CLIST
					 (bmtree.ctree)->row_list,
					 selected_row));

	file = (const xmlChar *)
	    gtk_file_selection_get_filename(GTK_FILE_SELECTION
					    (filesel));

	parse_bookmarks(bmtree.ctree, file, node);

	bookmarks_changed = TRUE;

	gtk_widget_destroy(filesel);
}


/******************************************************************************
 * Name
 *   restore_cancel
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
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
 *   #include "gui/bookmarks.h"
 *
 *   void on_restore_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   restore a saved bookmark folder
 *
 * Return value
 *   void
 */

void on_restore_folder_activate(GtkMenuItem * menuitem,
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
	if (test == GS_YES) {
		remove_selection(p_bmtree->ctree);
		bookmarks_changed = TRUE;
		gtk_widget_set_sensitive(menu.save, bookmarks_changed);
	}
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
		bookmarks_changed = TRUE;
		gtk_widget_set_sensitive(menu.save, bookmarks_changed);
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
 *   on_dialog_activate
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
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

void on_dialog_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *modName, *key;
	GtkCTreeNode *selected_node;

	selected_node = GTK_CLIST(bmtree.ctree)->selection->data;
	use_dialog = TRUE;
	if (GTK_CTREE_ROW(selected_node)->is_leaf) {
		key =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[1])->text;
		modName =
		    GTK_CELL_PIXTEXT(GTK_CTREE_ROW(selected_node)->row.
				     cell[2])->text;
		goto_bookmark(modName, key);
	}
	use_dialog = FALSE;
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
		newnode = gtk_ctree_insert_node(bmtree.ctree,
						node,
						NULL,
						text,
						3,
						pixmap3,
						mask3,
						NULL,
						NULL, TRUE, FALSE);
		gtk_ctree_sort_node(bmtree.ctree, node);
		gtk_ctree_select(bmtree.ctree, newnode);
		if (node)
			gtk_ctree_expand(bmtree.ctree, node);
		bookmarks_changed = TRUE;
		gtk_widget_set_sensitive(menu.save, bookmarks_changed);
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

void gui_verselist_to_bookmarks(GList * list, gchar * module_name)
{
	char *text[3], *t, *buf;
	GtkCTreeNode *node;
	GtkCTree *ctree;
	GList *tmp;
	gint test;
	const gchar *key_buf = NULL;
	gchar *tmpbuf;
	GString *str;

	GS_DIALOG *info;

	node = NULL;
	tmp = list;
	t = "|";
	ctree = GTK_CTREE(widgets.bookmark_tree);

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
		set_results_position((char) 1);	/* TOP */
		str = g_string_new(" ");
		while ((key_buf = get_next_result_key()) != NULL) {
			tmpbuf = (gchar *) key_buf;
			//g_warning(tmpbuf);
			g_string_sprintf(str, "%s, %s", tmpbuf,
					 module_name);
			text[0] = str->str;
			text[1] = tmpbuf;
			text[2] = module_name;
			node = gtk_ctree_insert_node(ctree,
						     newrootnode,
						     node,
						     text,
						     3,
						     pixmap3,
						     mask3,
						     NULL, NULL, TRUE,
						     FALSE);

		}
		g_string_free(str, TRUE);
		bookmarks_changed = TRUE;
		gtk_widget_set_sensitive(menu.save, bookmarks_changed);
	}
	g_free(info->text1);
	g_free(info);
	g_list_free(tmp);
	g_list_free(list);
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
	GtkCTreeNode *node = NULL;
	gchar *text[3];
	bmtree.ctree = GTK_CTREE(widgets.bookmark_tree);
	bmtree.bookmark_tree = widgets.bookmark_tree;
	p_bmtree = &bmtree;



	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->bookmark_tree),
				 "button_press_event",
				 GTK_SIGNAL_FUNC(button_press_event),
				 NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->bookmark_tree),
				 "button_release_event",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->bookmark_tree),
				 "tree_move",
				 GTK_SIGNAL_FUNC(after_move), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->bookmark_tree),
				 "end_selection",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->bookmark_tree),
				 "toggle_focus_row",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->bookmark_tree),
				 "select_all",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->bookmark_tree),
				 "unselect_all",
				 GTK_SIGNAL_FUNC(after_press), NULL);
	gtk_signal_connect_after(GTK_OBJECT(p_bmtree->bookmark_tree),
				 "scroll_vertical",
				 GTK_SIGNAL_FUNC(after_press), NULL);

	/* add root node to bookmark ctree */
	text[0] = "Bookmarks";
	text[1] = "ROOT";
	text[2] = "ROOT";
	node =
	    gtk_ctree_insert_node(bmtree.ctree, NULL, NULL,
				  text, 3, pixmap1, mask1,
				  pixmap2, mask2, FALSE, FALSE);
	/* add bookmarks to root node */
	load_xml_bookmarks(bmtree.ctree, node);

	gtk_ctree_sort_recursive(bmtree.ctree, node);

	gtk_signal_connect(GTK_OBJECT(widgets.bookmark_tree),
			   "tree_select_row",
			   GTK_SIGNAL_FUNC(on_ctree_select_row),
			   bmtree.ctree);
	gtk_clist_set_row_height(GTK_CLIST(p_bmtree->ctree), 15);
	gtk_ctree_set_spacing(p_bmtree->ctree, 3);
	gtk_ctree_set_indent(p_bmtree->ctree, 8);

	create_bookmark_menu();
	bookmarks_changed = FALSE;
	use_dialog = FALSE;
}


/******************************************************************************
 * Name
 *   pmBookmarkTree_uiinfo
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
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
	 (gpointer) save_bookmarks, NULL, NULL,
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
 *   #include "gui/bookmarks.h"
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

void create_bookmark_menu(void)
{
	pmBookmarkTree = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmBookmarkTree),
			    "pmBookmarkTree", pmBookmarkTree);
	gnome_app_fill_menu(GTK_MENU_SHELL(pmBookmarkTree),
			    pmBookmarkTree_uiinfo, NULL, FALSE, 0);

	menu.in_dialog = pmBookmarkTree_uiinfo[0].widget;
	menu.new = pmBookmarkTree_uiinfo[1].widget;
	menu.insert = pmBookmarkTree_uiinfo[2].widget;
	menu.edit = pmBookmarkTree_uiinfo[4].widget;
	menu.point = pmBookmarkTree_uiinfo[5].widget;
	menu.delete = pmBookmarkTree_uiinfo[6].widget;

	menu.save = pmBookmarkTree_uiinfo[8].widget;

	menu.bibletime = pmBookmarkTree_uiinfo[14].widget;
	menu.rr_submenu = pmBookmarkTree_uiinfo[15].widget;

	menu.remove = rr_menu_uiinfo[0].widget;
	menu.restore = rr_menu_uiinfo[2].widget;

	gtk_widget_set_sensitive(menu.in_dialog, FALSE);
	gtk_widget_set_sensitive(menu.new, FALSE);
	gtk_widget_set_sensitive(menu.insert, FALSE);
	gtk_widget_set_sensitive(menu.edit, FALSE);
	gtk_widget_set_sensitive(menu.point, FALSE);
	gtk_widget_set_sensitive(menu.delete, FALSE);
	gtk_widget_set_sensitive(menu.save, FALSE);
	gtk_widget_set_sensitive(menu.bibletime,
				 settings.have_bibletime);

	gtk_widget_set_sensitive(menu.rr_submenu, FALSE);
	gtk_widget_set_sensitive(menu.remove, TRUE);
	gtk_widget_set_sensitive(menu.restore, TRUE);


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
 *   creates an add bookmark menu list of root nodes
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
	ctree = GTK_CTREE(widgets.bookmark_tree);

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
