/*
 * GnomeSword Bible Study Tool
 * module_dialogs.cc - view module in a dialog
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
#include <swmgr.h>
#include <swmodule.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>
#ifdef __cplusplus
}
#endif	

#include <string.h>

#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/display_info.h"
#include "gui/editor.h"
#include "gui/note_editor.h"
#include "gui/font_dialog.h"
#include "gui/sidebar.h"
#include "gui/hints.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/bibletext.h"
#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/sidebar.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/key.h"
#include "main/xml.h"
#include "main/display.hh"
#include "main/global_ops.hh"

#include "backend/sword_main.hh"


typedef struct _treeitem TreeItem;
struct _treeitem {
	gchar *module_name;
	gchar *item_name;
	gchar *offset;
	gboolean is_leaf;
	gboolean expanded;
        GdkPixbuf *pixbuf_opened;
        GdkPixbuf *pixbuf_closed;
};

enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_TITLE,
	COL_BOOK,
	COL_OFFSET,
	N_COLUMNS
}; 

enum {
	TYPE_URI,
	BLANK,
	MODULE,
	KEY,
	TYPE_NOTE,
	NOTE_NUM
};

gboolean bible_freed;

gboolean dialog_freed;
gboolean bible_apply_change;

/******************************************************************************
 * static - global to this file only
 */
static DIALOG_DATA *dlg_bible;
static DIALOG_DATA *dlg_percom;
static GList *list_dialogs;
static gboolean bible_in_url;

/******************************************************************************
 * externs
 */
extern gboolean gsI_isrunning;	/* information dialog */



/******************************************************************************
 * Name
 *  add_tree_item
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   void add_tree_item()	
 *
 * Description
 *    
 *
 * Return value
 *   GtkCTreeNode*
 */ 
 
static void add_tree_item(GtkTreeModel * model, TreeItem *item, GtkTreeIter parent)
{
	
	GtkTreeIter iter;
	
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, &parent);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 
				COL_OPEN_PIXBUF,item->pixbuf_opened,
				COL_CLOSED_PIXBUF,item->pixbuf_closed,
				COL_TITLE, item->item_name, 
				COL_BOOK, item->module_name, 
				COL_OFFSET, item->offset, 
				-1);
	
}

/******************************************************************************
 * Name
 *  add_children_to_root
 *
 * Synopsis
 *   #include "gui/gbs.h"
 *
 *   void add_children_to_root(gchar *bookname,
 *   				unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void main_dialogs_add_children_to_tree(GtkTreeModel * model, GtkTreeIter iter,
	  unsigned long offset, gboolean is_dialog, DIALOG_DATA * d)
{
	gchar buf[256];
	gchar *tmpbuf;
	gchar *mod_name;
	TreeItem treeitem, *p_treeitem;
	GdkPixbuf *open;
	GdkPixbuf *closed;
	BackEnd *be = (BackEnd *)d->backend;
	
	p_treeitem = &treeitem;
	p_treeitem->module_name = d->mod_name;
	
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			   COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
			   COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			   -1);
	
	if (be->treekey_first_child(offset)) {
		offset = be->get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_treeitem->offset = buf;
		tmpbuf = be->treekey_get_local_name(offset);
		p_treeitem->item_name = (gchar*)tmpbuf;
		if (be->treekey_has_children(offset)) {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_closed;
			p_treeitem->pixbuf_closed = pixbufs->pixbuf_opened;
			p_treeitem->is_leaf = FALSE;
			p_treeitem->expanded = FALSE;
		} else {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_helpdoc;
			p_treeitem->pixbuf_closed = NULL;
			p_treeitem->is_leaf = TRUE;
			p_treeitem->expanded = FALSE;
		}
		add_tree_item(model, p_treeitem, iter);
		free(tmpbuf);
	}	
	
	while (be->treekey_next_sibling(offset)) {
		offset = be->get_treekey_offset();
		sprintf(buf, "%lu", offset);
		p_treeitem->offset = buf;
		tmpbuf = be->treekey_get_local_name(offset);
		p_treeitem->item_name = (gchar*)tmpbuf;
		if (be->treekey_has_children(offset)) {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_closed;
			p_treeitem->pixbuf_closed = pixbufs->pixbuf_opened;
			p_treeitem->is_leaf = FALSE;
			p_treeitem->expanded = FALSE;
		} else {
			p_treeitem->pixbuf_opened = pixbufs->pixbuf_helpdoc;
			p_treeitem->pixbuf_closed = NULL;
			p_treeitem->is_leaf = TRUE;
			p_treeitem->expanded = FALSE;
		}
		add_tree_item(model, p_treeitem, iter);
		free(tmpbuf);
	}
}


/******************************************************************************
 * Name
 *  add_book_to_tree
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void add_book_to_tree(GtkWidget * tree, gchar * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void main_dialogs_add_book_to_tree(GtkWidget * tree, gchar * mod_name, 
			     gboolean is_dialog, DIALOG_DATA * d)
{
	GtkTreeIter iter;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(tree));	
	BackEnd *be = (BackEnd *) d->backend;	
	
	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 
				    COL_OPEN_PIXBUF, pixbufs->pixbuf_opened,
				    COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
				    COL_TITLE,d->mod_name,
				    COL_BOOK,d->mod_name,
				    COL_OFFSET,NULL,
				    -1);
	be->set_module(d->mod_name);
	be->set_treekey(0);
	main_dialogs_add_children_to_tree(model, iter, be->get_treekey_offset(),
				is_dialog, d);
	
}


/******************************************************************************
 * Name
 *   tree_selection_changed
 *
 * Synopsis
 *   #include "main/gbs_main.h"
 *
 *   void tree_selection_changed(GtkTreeModel * model, 
 *        GtkTreeSelection * selection, gboolean is_dialog, DIALOG_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_dialogs_tree_selection_changed(GtkTreeModel * model, 
	    GtkTreeSelection * selection, gboolean is_dialog, DIALOG_DATA * g)
{
	GtkTreeIter selected;
	GtkTreePath *path;
	gchar *name = NULL;
	gchar *book = NULL;
	gchar *offset = NULL;	
	unsigned long l_offset;
	BackEnd *be = (BackEnd *) g->backend;
	
	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		path = gtk_tree_model_get_path(model, &selected);
		//tree_level = gtk_tree_path_get_depth(path);
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 
					2,&name,
					3,&book, 
					4,&offset, 
					-1);
		gtk_tree_path_free(path);
		
		if (offset) {
			l_offset = strtoul(offset, NULL, 0);
			g->offset = l_offset;			
			be->set_module(book);
			be->set_treekey(l_offset);
			settings.book_key = be->treekey_get_local_name(l_offset);
			if( !gtk_tree_model_iter_has_child(model, &selected) &&
				be->treekey_has_children(l_offset)) {
				main_dialogs_add_children_to_tree(model, selected, 
						l_offset, is_dialog, g);
			}
			//is_leaf = gtk_tree_model_iter_has_child(model, &selected);
			be->display_mod->Display();
			g_free(name);
			g_free(book);
			g_free(offset);
		}
	}
}


/******************************************************************************
 * Name
 *   main_dialogs_dictionary_entery_changed
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_dialogs_dictionary_entery_changed(DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_dialogs_dictionary_entery_changed(DIALOG_DATA * d)
{
	gint count = 7, i;
	gchar *new_key, *text = NULL;
	gchar *key = NULL;
	static gboolean firsttime = TRUE;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	gint height;
	gchar * mod_name = d->mod_name;
	BackEnd *be = (BackEnd*)d->backend;
	
	//g_warning(be->mod->Name());
	
	if(!backend->is_module(mod_name))
		return;
	
	//key = (gchar*)gtk_entry_get_text(GTK_ENTRY(d->entry));
	be->set_module_key(mod_name, d->key);
	key = be->get_module_key();
	
			
//	xml_set_value("GnomeSword", "keys", "dictionary", key);
//	settings.dictkey = xml_get_value("keys", "dictionary");
	
	be->set_module_key(mod_name, key);
	be->display_mod->Display();
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(d->listview));
	list_store = GTK_LIST_STORE(model);
	
	if (!firsttime) {
		gdk_drawable_get_size ((GdkDrawable *)d->listview->window,
                                             NULL,
                                             &height);
		count = height / settings.cell_height;
	}	 

	if (count) {
		gtk_list_store_clear(list_store);
		new_key = be->navigate_module(-1);

		for (i = 0; i < (count / 2); i++) {
			free(new_key);
			new_key = be->navigate_module(0);
		}

		for (i = 0; i < count; i++) {
			free(new_key);
			new_key = be->navigate_module(1);
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
 *   save_note_receiver
 *
 * Synopsis
 *   #include ".h"
 *
 *   	gboolean save_note_receiver(const HTMLEngine * engine,
 *		   const char *data, unsigned int len, void *user_data)
 *
 * Description
 *    
 *
 * Return value
 *   gboolean
 */

static GString *note_str;
static gboolean save_note_receiver(const HTMLEngine * engine,
				   const char *data, unsigned int len,
				   void *user_data)
{
	static gboolean startgrabing = FALSE;
	if (!strncmp(data, "</BODY>", 7))
		startgrabing = FALSE;
	if (startgrabing) {
		note_str = g_string_append(note_str, data);
		//g_warning(gstr->str);
	}
	if (strstr(data, "<BODY") != NULL)
		startgrabing = TRUE;

	return TRUE;
}


/******************************************************************************
 * Name
 *   main_dialog_save_note
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_dialog_save_note(DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_dialog_save_note(DIALOG_DATA * d)
{
	BackEnd *be = (BackEnd*)d->backend;
	GSHTMLEditorControlData *e = (GSHTMLEditorControlData*)d->editor;
	
	if(!be)
		return;	
	
	gtk_html_set_editable(e->html, FALSE);
	note_str = g_string_new("");
	
	if (!gtk_html_export (e->html, "text/html",
		 (GtkHTMLSaveReceiverFn) save_note_receiver,
		 GINT_TO_POINTER(0)) ){
		g_warning("file not writen");
	} else {
		be->save_entry(note_str->str);	
		g_print("\nnote saved\n");
	}
	g_string_free(note_str, 0);
	gtk_html_set_editable(e->html, TRUE);
}

/******************************************************************************
 * Name
 *   main_dialog_delete_note
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_dialog_delete_note(DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_dialog_delete_note(DIALOG_DATA * d)
{
	BackEnd *be = (BackEnd*)d->backend;
	
	if(!be)
		return;	
	be->delete_entry();	
}


/******************************************************************************
 * Name
 *   update_controls
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void update_controls(DIALOG_DATA * vt)	
 *
 * Description
 *   update the book, chapter and verse contorls
 *
 * Return value
 *   void
 */

void main_dialog_update_controls(DIALOG_DATA * t)
{
	gchar *val_key;
	gint cur_chapter, cur_verse;
	BackEnd *be = (BackEnd*)t->backend;

	dlg_bible = t;
	bible_apply_change = FALSE;
	val_key = be->get_valid_key(t->key);
	cur_chapter = be->key_get_chapter(val_key);
	cur_verse = be->key_get_verse(val_key);
	/* 
	 *  set book, chapter,verse and freeform lookup entries
	 *  to new verse - settings.bible_apply_change is set to false so we don't
	 *  start a loop
	 */
	gtk_entry_set_text(GTK_ENTRY(t->cbe_book),
			   be->key_get_book(val_key));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (t->spb_chapter), cur_chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (t->spb_verse), cur_verse);
	gtk_entry_set_text(GTK_ENTRY(t->freeform_lookup), val_key);
	if(t->key)
		g_free(t->key);
	t->key = g_strdup(val_key);
	g_free(val_key);

	bible_apply_change = TRUE;
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void (DIALOG_DATA * vt)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_bible_dialog_passage_changed(DIALOG_DATA * t, gchar * passage)
{	
	BackEnd *be = (BackEnd*)t->backend;
	gchar buf[256];
	gchar *val_key = NULL;
	if (bible_apply_change) {
		if (*passage) {
			val_key = be->get_valid_key(passage);
			if(t->key)
				g_free(t->key);
			t->key = g_strdup(val_key );
			main_dialog_update_controls(t);	
			be->set_key(t->key);
			be->display_mod->Display();
			g_free(val_key);
		}
	}
}



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void (DIALOG_DATA * vt)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_bible_dialog_display(DIALOG_DATA * t)
{	
	BackEnd *be = (BackEnd*)t->backend;
	be->set_module_key(t->mod_name,t->key);
	be->display_mod->Display();	
}

/******************************************************************************
 * Name
 *   free_on_destroy
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void free_on_destroy(DIALOG_DATA * vt)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

void main_free_on_destroy(DIALOG_DATA * t)
{
	GList *tmp = NULL;
	list_dialogs = g_list_remove(list_dialogs, (DIALOG_DATA*) t);
	g_free(t->ops); 
	g_free(t->key);
	g_free(t->mod_name); 
	if((GSHTMLEditorControlData*)t->editor) {
		gui_html_editor_control_data_destroy(NULL, 
				(GSHTMLEditorControlData*)t->editor);		
		dlg_percom = NULL;
	}
	if((BackEnd*)t->backend) {
		BackEnd* be = (BackEnd*)t->backend;
		delete be;
	}
	g_free(t);
}


/******************************************************************************
 * Name
 *   gui_bibletext_dialog_goto_bookmark
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_bibletext_dialog_goto_bookmark(gchar * mod_name, gchar * key)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_dialog_goto_bookmark(gchar * url)
{
	GList *tmp = NULL;
	gchar **work_buf = NULL;
	
	work_buf = g_strsplit (url,"/",4);
	
	if(!work_buf[MODULE] && !work_buf[KEY]) {
		g_strfreev(work_buf);
		return;
	}
	
	tmp = g_list_first(list_dialogs);
	while (tmp != NULL) {
		DIALOG_DATA *t = (DIALOG_DATA *) tmp->data;
		if(!strcmp(t->mod_name, work_buf[MODULE])) {
			BackEnd* be = (BackEnd*)t->backend;
			if(t->key)
				g_free(t->key);
			t->key = g_strdup(work_buf[KEY]);
			main_dialog_update_controls(t);
			be->set_key(t->key);
			be->display_mod->Display();
			gdk_window_raise(t->dialog->window);	
			g_strfreev(work_buf);
			return;
		}		
		tmp = g_list_next(tmp);
	}
	
	main_dialogs_open(work_buf[2]);
	BackEnd* be = (BackEnd*)dlg_bible->backend;
	if(dlg_bible->key)
		g_free(dlg_bible->key);
	dlg_bible->key = g_strdup(work_buf[3]);
	main_dialog_update_controls(dlg_bible);
	be->set_key(dlg_bible->key);
	be->display_mod->Display();	
	g_strfreev(work_buf);
}


/******************************************************************************
 * Name
 *   main_dialogs_setup
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void main_dialogs_setup (GList *mods)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_dialogs_setup(void)
{
	list_dialogs = NULL;
	bible_freed = FALSE;
	dialog_freed = FALSE;
	bible_apply_change = FALSE;
	dlg_percom = NULL;
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

void main_keep_bibletext_dialog_in_sync(gchar * key)
{
	GList *tmp = NULL;
	gchar * url;
	tmp = g_list_first(list_dialogs);
	while (tmp != NULL) {
		DIALOG_DATA * t = (DIALOG_DATA*) tmp->data;
		if(t->sync) {
			url = g_strdup_printf("sword://%s/%s",t->mod_name, key);
			main_dialogs_url_handler(t, url, TRUE);
			g_free(url);	
		}
		tmp = g_list_next(tmp);
	}
}


/******************************************************************************
 * Name
 *  gui_shutdown_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void gui_shutdown_bibletext_dialog(void)	
 *
 * Description
 *   shut down bibletext dialog support clean mem
 *
 * Return value
 *   void
 */

void main_dialogs_shutdown(void)
{
	GList *tmp = list_dialogs;
	//g_warning("items = %d",g_list_length(tmp));
	while (tmp != NULL) {
		DIALOG_DATA *t = (DIALOG_DATA*) tmp->data;
		bible_freed = TRUE;
		dialog_freed = TRUE;
		/* 
		 *  destroy any dialogs created 
		 */
		if (t->dialog)
			gtk_widget_destroy(t->dialog);
		/* 
		 * free each DIALOG_DATA item created 
		 */
		if((GSHTMLEditorControlData*)t->editor) 
			gui_html_editor_control_data_destroy(NULL, 
					(GSHTMLEditorControlData*)t->editor);
		
		if((BackEnd*)t->backend) {
			BackEnd* be = (BackEnd*)t->backend;
			delete be;
		}
		g_free(t->mod_name);
		g_free(t->ops);
		g_free(t->key);
		g_free(t);
		tmp = g_list_next(tmp);
	}
	g_list_free(list_dialogs);
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

void main_dialog_set_global_opt(gboolean choice)
{	
	char *on_off;
	if (choice)
		on_off = "On";
	else
		on_off = "Off";
	
	g_warning("main_dialog_set_global_opt");
	
}

 
/******************************************************************************
 * Name
 *   note_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint note_uri(const gchar * url)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
static gint note_uri(DIALOG_DATA * t, const gchar * url, gboolean clicked)
{	
	gchar **work_buf = NULL;
	gchar *module = NULL;
	gchar *tmpbuf = NULL;
	
	BackEnd* be = (BackEnd*)t->backend;
	
	work_buf = g_strsplit (url,"/",6);
	
	if (hint.in_popup) {
		gtk_widget_destroy(hint.hint_window);
		hint.in_popup = FALSE;
	}
	
	if(be->is_module(work_buf[2])) 
		module = work_buf[2];
	else
		module = t->mod_name;
	
	//if(strstr(work_buf[4],"x") && clicked) {
	be->set_module_key(module, work_buf[3]);
		tmpbuf = be->get_entry_attribute("Footnote",
							work_buf[5],
							"body");
		//tmpbuf = get_crossref(module,work_buf[3],work_buf[5]);
		if (tmpbuf) {
			if (!gsI_isrunning) {
				gui_create_display_informtion_dialog();
			}
			
			gui_display_text_information(tmpbuf);
			gtk_window_set_title(GTK_WINDOW(dialog_display_info),
			     _("Note"));
			g_free(tmpbuf);
		}
		/*
	} else if(!clicked) {
		
		tmpbuf = get_footnote_body(module,work_buf[3],work_buf[5]);
		if (tmpbuf) {
			void gui_display_text_information(gchar * information)
			gui_display_in_hint_window(tmpbuf);
			g_free(tmpbuf);	
		}			
	}
	*/
	g_strfreev(work_buf);
	return 1;
	
}
 

/******************************************************************************
 * Name
 *   morph_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint morph_uri(const gchar * url)
 *
 * Description
 *   handle a sword uri in the form 'sword://KJV/1John5:8'
 *                                   'sword://MHC/Genesis1:1'
 *                                   'sword:///Romans8:28'
 *   and display in the appropriate pane
 *
 * Return value
 *   gint
 */

static gint morph_uri(DIALOG_DATA * t, const gchar * url, gboolean clicked)
{
	gchar *module = NULL;
	gchar *key = NULL;
	gchar *modbuf = NULL;
	gchar **work_buf = NULL;
	static GtkWidget *dlg;
	
	BackEnd* be = (BackEnd*)t->backend;
	
	work_buf = g_strsplit (url,"/",4);
	if(!work_buf[MODULE] && !work_buf[KEY]) {
		g_strfreev(work_buf);
		return 0;
	}
	if(!strcmp(work_buf[2],"Greek") || strstr(work_buf[2],"x-Robinson")) {
		if(be->is_module("Robinson")) 
			modbuf = "Robinson";
	} else
		return 0;
	
	if (!gsI_isrunning) {
		dlg =
		    gui_create_display_informtion_dialog();
	}
	
	gui_display_mod_and_key(modbuf, work_buf[KEY]);
	gtk_window_set_title(GTK_WINDOW(dialog_display_info),
	     modbuf);
	g_strfreev(work_buf);	
	return 1;
}



/******************************************************************************
 * Name
 *   strongs_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint sword_uri(const gchar * url)
 *
 * Description
 *   handle a sword uri in the form 'sword://KJV/1John5:8'
 *                                   'sword://MHC/Genesis1:1'
 *                                   'sword:///Romans8:28'
 *   and display in the appropriate pane
 *
 * Return value
 *   gint
 */

static gint strongs_uri(DIALOG_DATA * t, const gchar * url, gboolean clicked)
{
	gchar *module = NULL;
	gchar *key = NULL;
	gchar *modbuf = NULL;
	gchar **work_buf = NULL;
	static GtkWidget *dlg;
	
	BackEnd* be = (BackEnd*)t->backend;
	
	work_buf = g_strsplit (url,"/",4);
	if(!work_buf[MODULE] && !work_buf[KEY]) {
		g_strfreev(work_buf);
		return 0;
	}
	
	if(!strcmp(work_buf[2],"Greek")) {
		if(be->is_module(settings.lex_greek)) 
			modbuf = settings.lex_greek;
		else
			return 0;
	} else if(!strcmp(work_buf[2],"Hebrew")) {
		if(be->is_module(settings.lex_hebrew)) 
			modbuf = settings.lex_hebrew;
		else
			return 0;
	}
	
	if (!gsI_isrunning) {
		dlg =
		    gui_create_display_informtion_dialog();
	}
	
	gui_display_mod_and_key(modbuf, work_buf[KEY]);
	gtk_window_set_title(GTK_WINDOW(dialog_display_info),
	     modbuf);
	g_strfreev(work_buf);	
	return 1;
}



/******************************************************************************
 * Name
 *   sword_uri
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint sword_uri(const gchar * url)
 *
 * Description
 *   handle a sword uri in the form 'sword://KJV/1John5:8'
 *                                   'sword://MHC/Genesis1:1'
 *                                   'sword:///Romans8:28'
 *   and display in the appropriate pane
 *
 * Return value
 *   gint
 */

static gint sword_uri(DIALOG_DATA * t, const gchar * url, gboolean clicked)
{
	gchar *module = NULL;
	gchar *key = NULL;
	gchar **work_buf = NULL;
	
	BackEnd* be = (BackEnd*)t->backend;
	
	if(!clicked)
		return 0;
	
	
	work_buf = g_strsplit (url,"/",4);
	if(!work_buf[MODULE] && !work_buf[KEY]) {
		g_strfreev(work_buf);
		return 0;
	}
	
	if(strlen(work_buf[MODULE]) < 2)
		module = g_strdup(t->mod_name);
	else
		module = g_strdup(work_buf[MODULE]);
	
	if(strlen(work_buf[KEY]) < 5)
		key = g_strdup(t->key);
	else
		key =g_strdup( work_buf[KEY]);
	
	if(t->key)
		g_free(t->key);
	t->key = be->get_valid_key(key);
	
	if(t->mod_name)
		g_free(t->mod_name);
	t->mod_name = g_strdup(module);
	
	be->set_module_key(t->mod_name, t->key);
	main_dialog_set_global_options(t);
	be->display_mod->Display();
		
	//main_dialog_update_controls(t);
	
	g_free(module);
	g_free(key);
	g_strfreev(work_buf);	
	return 1;
	
}

/******************************************************************************
 * Name
 *   gui_url_handler
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gint gui_url_handler(const gchar * url)
 *
 * Description
 *   
 *
 * Return value
 *   gint
 */

gint main_dialogs_url_handler(DIALOG_DATA * t, const gchar * url, gboolean clicked)
{		
	//g_warning(url);
	if(strstr(url,"sword://"))
		return sword_uri(t, url, clicked);
	if(strstr(url,"strongs://"))
		return strongs_uri(t, url, clicked);
	if(strstr(url,"morph://"))
		return morph_uri(t, url, clicked);
	if(strstr(url,"noteID://"))
		return note_uri(t, url, clicked);
/*	if(strstr(url,"bookmark://"))
		return bookmark_uri(url,clicked);
	if(strstr(url,"reference://"))
		return reference_uri(url,clicked);
	if(strstr(url,"about://"))
		return about_uri(url,clicked);
*/
	return 0;
}


/******************************************************************************
 * Name
 *   main_dialogs_open
 *
 * Synopsis
 *   #include "modules_dialogs.h"
 *
 *   void main_dialogs_open(gchar * mod_name)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_dialogs_open(gchar * mod_name)
{	
	GtkWidget *popupmenu;
	BackEnd *be;
	DIALOG_DATA *t = NULL;
	gint type;
	gchar *direction = NULL;
	gchar *url;
	GSHTMLEditorControlData *ec;
	
	if(!backend->is_module(mod_name))
		return;
	type = backend->module_type(mod_name);
	
	t = g_new0(DIALOG_DATA, 1);
	t->backend = (BackEnd*) new BackEnd();
	be = (BackEnd*)t->backend;
	
	t->ops = main_new_globals(mod_name);
	t->ops->words_in_red = FALSE;
	t->ops->strongs = FALSE;
	t->ops->morphs = FALSE;
	t->ops->footnotes = FALSE;
	t->ops->greekaccents = FALSE;
	t->ops->lemmas = FALSE;
	t->ops->scripturerefs = FALSE;
	t->ops->hebrewpoints = FALSE;
	t->ops->hebrewcant = FALSE;
	t->ops->headings = FALSE;
	t->ops->variants_all = FALSE;
	t->ops->variants_primary = FALSE;
	t->ops->variants_secondary = FALSE;
	
	t->editor = NULL;
	t->search_string = NULL;
	t->dialog = NULL;
	t->key = NULL;
	t->is_dialog = TRUE;
	t->mod_name = g_strdup(mod_name);
	t->is_rtol = FALSE;
	if((direction = be->get_config_entry(t->mod_name, "Direction"))
					!= NULL) {
		if (!strcmp(direction, "RtoL"))			
			t->is_rtol = TRUE;
	}
	t->sync = FALSE;
	
	switch(type) {
		case TEXT_TYPE:
			gui_create_bibletext_dialog(t);
			if(t->is_rtol)
				be->dialogRTOLDisplay 
				      = new DialogTextviewChapDisp(t->text, be);
			else	
				be->chapDisplay 
				      = new DialogChapDisp(t->html, be); 
			be->init_SWORD(1);			
			t->key = g_strdup(settings.currentverse);
			main_dialog_update_controls(t);
			dlg_bible = t;
		break;
		case COMMENTARY_TYPE:
			gui_create_commentary_dialog(t, FALSE);
			be->entryDisplay = new DialogEntryDisp(t->html, be); 
			be->init_SWORD(1);
			t->key = g_strdup(settings.currentverse);
			main_dialog_update_controls(t);
		break;
		case PERCOM_TYPE:
			t->editor = (GSHTMLEditorControlData *) 
					gs_html_editor_control_data_new();
			ec = (GSHTMLEditorControlData *) t->editor;
			ec->stylebar = TRUE;
			ec->editbar = TRUE;
			ec->personal_comments = TRUE;
			ec->key = g_strdup(settings.currentverse);
			strcpy(ec->filename, t->mod_name);
			t->is_percomm = TRUE;
			gui_create_note_editor(t);
			be->entryDisplay 
				    = new DialogEntryDisp(ec->htmlwidget, be); 
			be->init_SWORD(1);
			t->key = g_strdup(settings.currentverse);
			main_dialog_update_controls(t);
			settings.percomm_dialog_exist = TRUE;
			dlg_percom = t;
		break;
		case DICTIONARY_TYPE:
			gui_create_dictlex_dialog(t);
			be->entryDisplay = new DialogEntryDisp(t->html, be); 
			be->init_SWORD(1);
			t->key = g_strdup(settings.dictkey);
		break;
		case BOOK_TYPE:
			gui_create_gbs_dialog(t);
			be->entryDisplay = new DialogEntryDisp(t->html, be); 
			be->init_SWORD(1);
			t->key = g_strdup(settings.book_key);
			if(settings.book_offset)
				t->offset = settings.book_offset;
			else
				t->offset = 0;
		break;
	}
		
	gtk_widget_show(t->dialog);
	list_dialogs = g_list_append(list_dialogs, (DIALOG_DATA *) t);
	be->set_module(t->mod_name);
	be->set_key(t->key);
	
	if (be->get_config_entry(t->mod_name, "CipherKey")) {
		t->cipher_old = be->get_config_entry(t->mod_name, "CipherKey");
		t->is_locked = strlen(t->cipher_old);
	} else {
		t->is_locked = 0;
		t->cipher_old = NULL;
	}
	
	if(type == BOOK_TYPE)
		be->set_treekey(t->offset);
	
	be->display_mod->Display();
	bible_apply_change = TRUE;
	
	if(type == PERCOM_TYPE){
		gtk_html_set_editable(ec->html, TRUE);
	}
	if(type == DICTIONARY_TYPE)
		gtk_entry_set_text(GTK_ENTRY(t->entry),t->key);	
	if(type == BOOK_TYPE)
		main_dialogs_add_book_to_tree(t->tree, t->mod_name, 
			     TRUE, t);
}


/******   end of file   ******/
