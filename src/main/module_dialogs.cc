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
#include <url.h>

#ifdef USE_GTKMOZEMBED
#include <gtkmozembed.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>
#ifdef __cplusplus
}
#endif	

#include <string.h>


#ifndef USE_GTKHTML38
#include "editor/editor.h"
#include "editor/editor-control.h"
//#include "editor/note_editor.h"
#endif

#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/display_info.h"
#include "gui/font_dialog.h"
#include "gui/sidebar.h"
#include "gui/hints.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/note_editor.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/sidebar.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
#include "main/display.hh"
#include "main/global_ops.hh"

#include "backend/sword_main.hh"
	
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"


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
extern gboolean do_display;


/******************************************************************************
 * Name
 *   main_dialogs_clear_viewer
 *
 * Synopsis
 *   #include "main/module_dialogs.h"
 *
 *   void main_dialogs_clear_viewer(DIALOG_DATA *d)
 *
 * Description
 *   clear the information viewer
 *
 * Return value
 *   void
 */

void main_dialogs_clear_viewer(DIALOG_DATA *d)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	GString *search_str;
	gboolean was_editable = FALSE;
	gchar *buf;

#ifdef USE_GTKMOZEMBED
	GtkMozEmbed *new_browser = GTK_MOZ_EMBED(d->previewer);
#else	
	/* setup gtkhtml widget */
	GtkHTML *html = GTK_HTML(d->previewer);
	was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
#endif	
	g_string_printf(tmp_str,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);

	str = g_string_new(tmp_str->str);
	buf = N_("Previewer");
	g_string_printf(tmp_str,
	"<b>%s</b><br><font color=\"grey\">" "<HR></font><br>", buf);
	str = g_string_append(str, tmp_str->str);
		
	g_string_printf(tmp_str, " %s", "</font></body></html>");
	str = g_string_append(str, tmp_str->str);

#ifdef USE_GTKMOZEMBED
	if (str->len)
		gtk_moz_embed_render_data(new_browser, str->str, str->len,
					"file:///sword", 
					"text/html");
#else	
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
	gtk_html_set_editable(html, was_editable);
#endif	
	//free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}


/******************************************************************************
 * Name
 *   main_dialogs_information_viewer
 *
 * Synopsis
 *   #include "main/previewer.h"
 *
 *   void main_dialogs_information_viewer(GtkWidget * html_widget, gchar * mod_name, 
 *		    gchar * text, gchar *key, gchar * type)
 *
 * Description
 *   display information in the information previewer
 *
 * Return value
 *   void
 */

void main_dialogs_information_viewer(DIALOG_DATA * d, gchar * mod_name, 
				     gchar * text, gchar * key,
		             	     gchar * action ,gchar * type ,
			     	     gchar * morph_text, gchar * morph)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	GString *search_str;
	MOD_FONT *mf = get_font(mod_name);
#ifdef USE_GTKMOZEMBED
	GtkMozEmbed *new_browser = GTK_MOZ_EMBED(sidebar.html_viewer_widget);
#else	
	GtkHTML *html = GTK_HTML(sidebar.html_viewer_widget);
#endif

	g_string_printf(tmp_str,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);

	str = g_string_new(tmp_str->str);
	if(type) {
		if(!strcmp(type,"n")) {
			g_string_printf(tmp_str,
				"<font color=\"grey\">%s<HR></font><br>",
					_("Footnote"));
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(type,"x")) {
			g_string_printf(tmp_str,
				"<font color=\"grey\">%s<HR></font><br>",
					_("Cross Reference"));
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(action ,"showStrongs")) {  //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str,
				"<font color=\"grey\">%s: %s<HR></font><br>",
					_("Strongs"),key);
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(action ,"showMorph")) {  //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str,
				"<font color=\"grey\">%s: %s<HR></font><br>",
					_("Morphology"),key);
			str = g_string_append(str, tmp_str->str);
		}
	}
	
	if(!strcmp(action ,"showStrongsMorph")) {  //&& !strcmp(type,"Greek")
		g_string_printf(tmp_str,
			"<font color=\"grey\">%s: %s<HR></font><br>",
				_("Strongs"),key);
		str = g_string_append(str, tmp_str->str);
		g_string_printf(tmp_str, 
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font)?mf->old_font:"none", 
				(mf->old_font_size)?mf->old_font_size:"+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);
		
		g_string_printf(tmp_str,
			"<font color=\"grey\"><br><br>%s: %s<HR></font><br>",
					_("Morphology"),morph);
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, morph_text);
		g_string_printf(tmp_str, " %s<br>", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);
		
		
	} else {
		g_string_printf(tmp_str, 
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font)?mf->old_font:"none", 
				(mf->old_font_size)?mf->old_font_size:"+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);
	
		g_string_printf(tmp_str, " %s", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);
	
	}
	
#ifdef USE_GTKMOZEMBED
	if (str->len)
		gtk_moz_embed_render_data(new_browser, str->str, str->len,
					"file:///sword", 
					"text/html");
#else	
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
	//gtk_html_set_editable(html, was_editable);
#endif	
/*	if (str->len) {
		gtk_html_load_from_string(html,str->str,str->len);
	}
*/	
	free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}






/******************************************************************************
 * Name
 *   main_dialog_information_viewer
 *
 * Synopsis
 *   #include "main/sword.h.h"
 *
 *   void main_dialog_information_viewer(GtkWidget * html_widget, gchar * mod_name, 
 *		    gchar * text, gchar *key, gchar * type, gboolean show_key)
 *
 * Description
 *   display information in the information viewer
 *
 * Return value
 *   void
 */

void main_dialog_information_viewer(gchar * mod_name, gchar * text, gchar * key,
		             gchar * action ,gchar * type ,gchar * morph_text,
			     gchar * morph, DIALOG_DATA * d)
{
	GString *tmp_str = g_string_new(NULL);
	GString *str;
	GString *search_str;
	MOD_FONT *mf = get_font(mod_name);
#ifdef USE_GTKMOZEMBED
	GtkMozEmbed *new_browser;
#else	
	GtkHTML *html;	
#endif	
	if(!d->previewer)
		return;
#ifdef USE_GTKMOZEMBED
	new_browser = GTK_MOZ_EMBED(d->previewer);
#else	
	html = GTK_HTML(d->previewer);
#endif	
		
	g_string_printf(tmp_str,
		HTML_START
		"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color,
		settings.link_color);

	str = g_string_new(tmp_str->str);
	if(type) {
		if(!strcmp(type,"n")) {
			g_string_printf(tmp_str,"<font color=\"grey\">%s<HR></font><br>",
					_("Footnote"));
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(type,"x")) {
			g_string_printf(tmp_str,"<font color=\"grey\">%s<HR></font><br>",
					_("Cross Reference"));
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(action ,"showStrongs")) {  //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str,"<font color=\"grey\">%s: %s<HR></font><br>",
					_("Strongs"),key);
			str = g_string_append(str, tmp_str->str);
		}
		if(!strcmp(action ,"showMorph")) {  //&& !strcmp(type,"Greek")
			g_string_printf(tmp_str,"<font color=\"grey\">%s: %s<HR></font><br>",
					_("Morphology"),key);
			str = g_string_append(str, tmp_str->str);
		}
	}
	
	if(!strcmp(action ,"showStrongsMorph")) {  //&& !strcmp(type,"Greek")
		g_string_printf(tmp_str,"<font color=\"grey\">%s: %s<HR></font><br>",
				_("Strongs"),key);
		str = g_string_append(str, tmp_str->str);
		g_string_printf(tmp_str, 
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font)?mf->old_font:"none", 
				(mf->old_font_size)?mf->old_font_size:"+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);
		
		g_string_printf(tmp_str,"<font color=\"grey\"><br><br>%s: %s<HR></font><br>",
					_("Morphology"),morph);
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, morph_text);
		g_string_printf(tmp_str, " %s<br>", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);
		
		
	} else {
		g_string_printf(tmp_str, 
				"<font face=\"%s\" size=\"%s\">",
				(mf->old_font)?mf->old_font:"none", 
				(mf->old_font_size)?mf->old_font_size:"+0");
		str = g_string_append(str, tmp_str->str);
		str = g_string_append(str, text);
	
		g_string_printf(tmp_str, " %s", "</font></body></html>");
		str = g_string_append(str, tmp_str->str);
	
	}
	
#ifdef USE_GTKMOZEMBED
	if (str->len)
		gtk_moz_embed_render_data(new_browser, str->str, str->len,
					"file:///sword", 
					"text/html");
#else	
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
#endif	
	
	
	free_font(mf);
	g_string_free(str, TRUE);
	g_string_free(tmp_str, TRUE);
}

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
	gint count = 10, i;
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

void main_dialog_save_note(gpointer data)
{
	//gchar *buf = NULL;                                       
//	gsize bytes_read;
//	gsize bytes_written;
//	GError **error = NULL;
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData *e = (GSHTMLEditorControlData*) data;
	BackEnd *be = (BackEnd*)e->be;
	
	
	if(!be)
		return;	
	
	gtk_html_set_editable(e->html, FALSE);
	note_str = g_string_new("");
	
	if (!gtk_html_export (e->html, "text/html",
		 (GtkHTMLSaveReceiverFn) save_note_receiver,
		 GINT_TO_POINTER(0)) ){
		g_warning("file not writen");
	} else {
		/*buf = g_convert(note_str->str,
                             -1,
                             OLD_CODESET,
                             UTF_8,
                             &bytes_read,
                             &bytes_written,
                             error);*/
		//g_message(note_str->str);
		be->save_entry(note_str->str);	
		g_print("\nnote saved\n");
		//g_free(buf);
	}
	g_string_free(note_str, 0);
	gtk_html_set_editable(e->html, TRUE);
#endif
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

void main_dialog_delete_note(gpointer data)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData *e = (GSHTMLEditorControlData*) data;
	BackEnd *be = (BackEnd*)e->be;
	
	if(!be)
		return;	
	be->delete_entry();
#endif	
}


/******************************************************************************
 * Name
 *   main_free_on_destroy
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_free_on_destroy(DIALOG_DATA * vt)
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
#ifndef USE_GTKHTML38 
	if((GSHTMLEditorControlData*)t->editor) {
		editor_control_data_destroy(NULL, 
				(GSHTMLEditorControlData*)t->editor);		
		dlg_percom = NULL;
	}
#endif
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

void main_dialog_goto_bookmark(const gchar * module, const gchar * key)
{
	DIALOG_DATA *t = NULL;
	GList *tmp = NULL;
	
	tmp = g_list_first(list_dialogs);
	while (tmp != NULL) {
		t = (DIALOG_DATA *) tmp->data;
		if(!strcmp(t->mod_name, module)) {
			BackEnd* be = (BackEnd*)t->backend;			
			if(t->mod_type == BOOK_TYPE) {
				t->offset = atoi(key);			
				be->set_treekey(t->offset);
			} else {
				if(t->key)
					g_free(t->key);
				t->key = g_strdup(key);
				be->set_key(t->key);
			}
			if(t->mod_type == TEXT_TYPE || 
					t->mod_type == COMMENTARY_TYPE)
				main_navbar_set(t->navbar, key);
			be->display_mod->Display();
			gdk_window_raise(t->dialog->window);
			return;
		}		
		tmp = g_list_next(tmp);
	}
	
	t = main_dialogs_open((gchar*)module,key);
	BackEnd* be = (BackEnd*)t->backend;	
	if(t->mod_type == BOOK_TYPE) {	
		t->offset = atoi(key);		
		be->set_treekey(t->offset);
	} else {
		if(t->key)
			g_free(t->key);
		t->key = g_strdup(key);
		be->set_key(t->key);
	}
	be->display_mod->Display();
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
 *   main_keep_bibletext_dialog_in_sync
 *
 * Synopsis
 *   #include ".h"
 *
 *   void main_keep_bibletext_dialog_in_sync(gchar * key)	
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
 *  main_dialogs_shutdown
 *
 * Synopsis
 *   #include ".h"
 *
 *  void main_dialogs_shutdown(void)	
 *
 * Description
 *   shut down dialog support clean mem
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
#ifndef USE_GTKHTML38
		if((GSHTMLEditorControlData*)t->editor) 
			editor_control_data_destroy(NULL, 
					(GSHTMLEditorControlData*)t->editor);
#endif		
		if((BackEnd*)t->backend) {
			BackEnd* be = (BackEnd*)t->backend;
			delete be;
		}
		if(t->navbar.key)
			g_free(t->navbar.key);
		if(t->navbar.module_name)
			g_free(t->navbar.module_name);
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
 *   main_dialog_set_global_opt
 *
 * Synopsis
 *   #include ".h"
 *
 *   main_dialog_set_global_opt(gboolean choice)	
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
 *   show_note
 *
 * Synopsis
 *   #include ".h"
 *
 *   gint show_note(DIALOG_DATA * d,const gchar * module, const gchar * passage, 
		const gchar * type, const gchar * value, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
static gint show_note(DIALOG_DATA * d,const gchar * module, const gchar * passage, 
		const gchar * type, const gchar * value, gboolean clicked)
{	
	gchar *tmpbuf = NULL;
	gchar *buf = NULL;
	GString *str = g_string_new(NULL);
	GList *tmp = NULL;
	gint i = 0;	
	BackEnd* be = (BackEnd*)d->backend;
	RESULTS *list_item;
	
	
	if(!be->is_module((gchar*)module)) 
		module = settings.MainWindowModule;
	
	if(passage && (strlen(passage) < 5))
		passage = settings.currentverse;
	
	if(strstr(type,"x") && clicked) {
		be->set_module_key((gchar*)module, (gchar*)passage);
		tmpbuf = be->get_entry_attribute("Footnote",
							 (gchar*)value,
							"refList");
		if (tmpbuf) {
			main_display_verse_list_in_sidebar(settings.
					  currentverse,
					  (gchar*)module,
					  tmpbuf);
			g_free(tmpbuf);
		}
	} else if(strstr(type,"n") && !clicked) {
		be->set_module_key((gchar*)module, (gchar*)passage);		
		tmpbuf = be->get_entry_attribute("Footnote",
						(gchar*)value,
						"body");		
		buf = be->render_this_text((gchar*)module,(gchar*)tmpbuf);
		if(tmpbuf) g_free(tmpbuf);	
		if (buf) {
			main_dialog_information_viewer((gchar*)module, 
					buf, 
					(gchar*)value,
					"showNote",			
					(gchar*)type,
					NULL,
					NULL,
					d);
			if(buf) g_free(buf);	
		}			
	} else if(strstr(type,"x") && !clicked) {
		be->set_module_key((gchar*)module, (gchar*)passage);		
		tmpbuf = be->get_entry_attribute("Footnote",
						(gchar*)value,
						"refList");
		
		
		list_of_verses = g_list_first(list_of_verses);
		if(list_of_verses) {
			while(list_of_verses) {
				list_item = (RESULTS*)list_of_verses->data;
				g_free(list_item->module);
				g_free(list_item->key);
				g_free(list_item);
				list_of_verses = g_list_next(list_of_verses);
			}
			g_list_free(list_of_verses);
		}
		list_of_verses = NULL;
		
		tmp = be->parse_verse_list(tmpbuf, settings.currentverse);  
		while (tmp != NULL) {
			buf = g_strdup_printf(
				"<a href=\"sword://%s/%s\">"
				"<font color=\"%s\">%s,</font></a><br>",
				(gchar*)module,
				(const char *) tmp->data,
				settings.bible_text_color,
				(const char *) tmp->data);
			str = g_string_append(str,buf);
			if(buf) g_free(buf);
			buf = NULL;
			++i;
			g_free((char *) tmp->data);
			tmp = g_list_next(tmp);
		}
		g_list_free(tmp);
		
		buf = g_strdup_printf(
				"<a href=\"sword://%s/%s\">"
				"<font color=\"%s\">%s%s</font></a><br>",
				(gchar*)module,
				settings.currentverse,
				settings.bible_text_color,
				_("Back to "),
				settings.currentverse);
		str = g_string_append(str,buf);
		if(buf) g_free(buf);		
		
		if(tmpbuf) g_free(tmpbuf);	
		if (str) {
			main_dialog_information_viewer((gchar*)module, 
					str->str, 
					(gchar*)value,
					"showNote",			
					(gchar*)type,
					NULL,
					NULL,
					d);
		}			
	}
	g_string_free(str, 1);
	return 1;
}


/******************************************************************************
 * Name
 *   show_morph
 *
 * Synopsis
 *   #include ".hh"
 *
 *   gint show_morph(DIALOG_DATA * d, const gchar * type, 
			const gchar * value, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */
 
 static gint show_morph(DIALOG_DATA * d, const gchar * type, 
			const gchar * value, gboolean clicked)
{	
	gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	static GtkWidget *dlg;	
		
	
	BackEnd* be = (BackEnd*)d->backend;
	
	if(!strcmp(type,"Greek") || strstr(type,"x-Robinson")) {
		if(be->is_module("Robinson")) 
			modbuf = "Robinson";
	} 		
	if (clicked) {
		if (!gsI_isrunning) 
			dlg = gui_create_display_informtion_dialog();
		else
			gtk_widget_show(dlg);
		
		gui_display_mod_and_key(modbuf, (gchar*)value);
		gtk_window_set_title(GTK_WINDOW(dialog_display_info),
		     modbuf);			
	} else {
		mybuf = main_get_rendered_text(modbuf, (gchar*)value);
		if (mybuf) {
			main_dialog_information_viewer(modbuf, 
					mybuf, 
					(gchar*)value, 
					"showMorph",
					(gchar*)type,
					NULL,
					NULL,
					d);
			g_free(mybuf);
		}
	}
	return 1;
	
}


 /******************************************************************************
 * Name
 *   show_strongs
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_strongs(DIALOG_DATA * t, const gchar * type, 
			const gchar * value, gboolean clicked)
 *
 * Description
 *  
 *
 * Return value
 *   gint
 */ 
 
static gint show_strongs(DIALOG_DATA * t, const gchar * type, 
			const gchar * value, gboolean clicked)
{	
	gchar *modbuf_viewer = NULL;
	gchar *modbuf = NULL;
	gchar *mybuf = NULL;
	guint delay;	
	guint i;
	static GtkWidget *dlg;	
	
	BackEnd* be = (BackEnd*)t->backend;
	
	if(!strcmp(t->mod_name,"NASB")) {
		if(!strcmp(type,"Greek")) 
			modbuf = "NasbGreek";
		else 
			modbuf = "NasbHebrew";
			
	} else {
		if(!strcmp(type,"Greek")) 
			modbuf = settings.lex_greek;
		else if(!strcmp(type,"Hebrew")) 
			modbuf = settings.lex_hebrew;
	}
	
	if (clicked) {
		if (!gsI_isrunning) 
			dlg = gui_create_display_informtion_dialog();
		else
			gtk_widget_show(dlg);
		
		gui_display_mod_and_key(modbuf, (gchar*)value);
		gtk_window_set_title(GTK_WINDOW(dialog_display_info),
		     modbuf);		
	} else {
		mybuf =
		    main_get_rendered_text(modbuf, (gchar*)value);
		if (mybuf) {
			main_dialog_information_viewer(  
					modbuf, 
					mybuf, 
					(gchar*)value, 
					"showStrongs",
					(gchar*)type,
					NULL,
					NULL,
					t);
			g_free(mybuf);
		}
	}
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
	if(t->navbar.module_name)
		main_navbar_set(t->navbar, t->key);
	
	g_free(module);
	g_free(key);
	g_strfreev(work_buf);	
	return 1;
	
}

static gint show_strongs_morph(DIALOG_DATA * d,const gchar * type, const gchar * value, 
			 const gchar * morph, gboolean clicked)
{	
	gchar *modbuf_viewer = NULL;
	gchar *modbuf = NULL;
	gchar *morph_mod = NULL;
	gchar *strongs_buf = NULL;
	gchar *morph_buf = NULL;
	guint delay;	
	guint i;
	static GtkWidget *dlg;	
	
	if(!strcmp(d->mod_name,"NASB")) {
		if(!strcmp(type,"Greek")) 
			modbuf = "NasbGreek";
		else 
			modbuf = "NasbHebrew";
	} else {
		if(!strcmp(type,"Greek")) {
			modbuf = settings.lex_greek;
			if(backend->is_module("Robinson")) 
				morph_mod = "Robinson";
		} else
			modbuf = settings.lex_hebrew;
	}
	
	if (clicked) {
		if (!gsI_isrunning) 
			dlg = gui_create_display_informtion_dialog();
		else
			gtk_widget_show(dlg);
		
		gui_display_mod_and_key(modbuf, (gchar*)value);
		gtk_window_set_title(GTK_WINDOW(dialog_display_info),
		     modbuf);		
	} else {
		strongs_buf =
		    main_get_rendered_text(modbuf, (gchar*)value);
		morph_buf = 
		    main_get_rendered_text(morph_mod, (gchar*)morph);
		if (strongs_buf) {
			main_dialog_information_viewer(  
					modbuf, 
					strongs_buf, 
					(gchar*)value, 
					"showStrongs",
					(gchar*)type,
					(gchar*)morph_buf,
					(gchar*)morph,
					d);
			g_free(strongs_buf);
			if(morph_buf) g_free(morph_buf);
		}
	}
	return 1;
}
 


static gint new_url_handler(DIALOG_DATA * t, const gchar * url, gboolean clicked)
{

	gchar* action = NULL;
	gchar* type = NULL;
	gchar* value = NULL;
	gchar* module = NULL;
	gchar* passage = NULL;
	gchar* strongs = NULL;
	gchar* morph = NULL;
	gchar *buf = NULL;
	URL* m_url;
	
#ifdef DEBUG	
	g_message("new_url_handler url = %s",url);
#endif
	m_url = new URL((const char*)url);	
	action = g_strdup(m_url->getParameterValue("action"));
	type = g_strdup((gchar*)m_url->getParameterValue("type"));
	value = g_strdup((gchar*)m_url->getParameterValue("value"));
	morph = g_strdup((gchar*)m_url->getParameterValue("morph"));
	strongs = g_strdup((gchar*)m_url->getParameterValue("lemma"));
	
#ifdef DEBUG
	g_message("action = %s",action);
	g_message("type = %s",type);  
	g_message("value = %s",value);
#endif	

	
	if(strlen(strongs) >= 1 && strlen(morph) >= 1 ) {
		show_strongs_morph(t, type,strongs,morph,clicked);
	} else if(strlen(strongs) >= 1 && strlen(morph) < 1) {
		show_strongs(t, type,strongs,clicked);			
	}
 
	if(!strcmp(action,"showStrongs")) {
		show_strongs(t, type, value, clicked);
	}
	
	
	if(!strcmp(action,"showMorph")) {
		show_morph(t, type, value, clicked);
	}
	
	if(!strcmp(action,"showNote")) {	
		module = g_strdup(m_url->getParameterValue("module"));
		passage = g_strdup((gchar*)m_url->getParameterValue("passage"));
		show_note(t, module, passage, type, value, clicked);
		if(module) g_free(module);
		if(passage) g_free(passage);
	}
	
	if(!strcmp(action,"showRef")) {	
		module = g_strdup(m_url->getParameterValue("module"));
		//new_reference_uri(module,value,clicked);
		if(module) g_free(module);
	}
	
	if(action) g_free(action);
	if(type) g_free(type);
	if(value) g_free(value);
	return 1;
}

/******************************************************************************
 * Name
 *   main_dialogs_url_handler
 *
 * Synopsis
 *   #include "main/module_dialogs.h"
 *
 *   gint main_dialogs_url_handler(DIALOG_DATA * t, const gchar * url, gboolean clicked)
 *
 * Description
 *   
 *
 * Return value
 *   gint
 */

gint main_dialogs_url_handler(DIALOG_DATA * t, const gchar * url, gboolean clicked)
{	
	g_message("main_dialogs_url_handler url = %s",url);
	if(strstr(url,"passagestudy.jsp") || strstr(url,"gnomesword.url"))
		return new_url_handler(t,url,clicked);
	if(strstr(url,"sword://"))
		return sword_uri(t, url, clicked);
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

DIALOG_DATA *main_dialogs_open(const gchar * mod_name ,  const gchar * key)
{	
	GtkWidget *popupmenu;
	BackEnd *be;
	DIALOG_DATA *t = NULL;
	gint type;
	gchar *direction = NULL;
	gchar *url;

#ifndef USE_GTKHTML38
	GSHTMLEditorControlData *ec;
#endif
	do_display = TRUE;
	if(!backend->is_module(mod_name))
		return NULL;
	type = backend->module_type(mod_name);
	
	t = g_new0(DIALOG_DATA, 1);
	t->backend = (BackEnd*) new BackEnd();
	be = (BackEnd*)t->backend;
	
	t->ops = main_new_globals((gchar*)mod_name);
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
	t->navbar.module_name = NULL;
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
			t->mod_type = TEXT_TYPE;
			gui_create_bibletext_dialog(t);
#ifdef USE_GTKMOZEMBED
			be->chapDisplay = new DialogChapDisp(t->html, be);
#else
			if(t->is_rtol)
				be->dialogRTOLDisplay 
				   = new DialogTextviewChapDisp(t->text, be);
			else	
				be->chapDisplay 
				      = new DialogChapDisp(t->html, be); 
#endif
			be->init_SWORD(1);
			if(key)
				t->key = g_strdup(key);
			else			
				t->key = g_strdup(settings.currentverse);
			dlg_bible = t;
			t->navbar.is_dialog = TRUE;
			t->navbar.key = g_strdup(settings.currentverse);
			t->navbar.module_name = g_strdup(mod_name);
			main_navbar_fill_book_combo(t->navbar);
		break;
		case COMMENTARY_TYPE:
			t->mod_type = COMMENTARY_TYPE;
			gui_create_commentary_dialog(t, FALSE);
			be->entryDisplay = new DialogEntryDisp(t->html, be); 
			be->init_SWORD(1);
			if(key)
				t->key = g_strdup(key);
			else			
				t->key = g_strdup(settings.currentverse);
			t->navbar.is_dialog = TRUE;
			t->navbar.key = g_strdup(settings.currentverse);
			t->navbar.module_name = g_strdup(mod_name);
			main_navbar_fill_book_combo(t->navbar);
		break;
#ifndef USE_GTKHTML38
		case PERCOM_TYPE:
			t->mod_type = PERCOM_TYPE;
			gui_create_note_editor(t);
			ec = (GSHTMLEditorControlData *) t->editor;
			ec->key = g_strdup(settings.currentverse);
			strcpy(ec->filename, t->mod_name);
			t->is_percomm = TRUE;
			be->entryDisplay 
				    = new DialogEntryDisp(ec->htmlwidget, be); 
			be->init_SWORD(1);
			ec->be = (BackEnd*)be;
			if(key)
				t->key = g_strdup(key);
			else			
				t->key = g_strdup(settings.currentverse);
			settings.percomm_dialog_exist = TRUE;
			dlg_percom = t;
			t->navbar.is_dialog = TRUE;
			t->navbar.key = g_strdup(t->key);
			t->navbar.module_name = g_strdup(mod_name);
			main_navbar_fill_book_combo(t->navbar);
		break;
#endif
		case DICTIONARY_TYPE:
			t->mod_type = DICTIONARY_TYPE;
			gui_create_dictlex_dialog(t);
			be->entryDisplay = new DialogEntryDisp(t->html, be); 
			be->init_SWORD(1);
			if(key)
				t->key = g_strdup(key);
			else			
				t->key = g_strdup(settings.dictkey);
		break;
		case BOOK_TYPE:
			t->mod_type = BOOK_TYPE;
			gui_create_gbs_dialog(t);
			be->entryDisplay = new DialogEntryDisp(t->html, be); 
			be->init_SWORD(1);
			t->key = NULL; 
			if(key)
				t->offset = atoi(key);
			else
				t->offset = 4;
		break;
	}
		
	gtk_widget_show(t->dialog);
	list_dialogs = g_list_append(list_dialogs, (DIALOG_DATA *) t);
	be->set_module(t->mod_name);
	if(type == TEXT_TYPE) 
		main_dialogs_clear_viewer(t);
	if(type == BOOK_TYPE) {
		main_dialogs_add_book_to_tree(t->tree, t->mod_name, 
			     TRUE, t);
		be->set_treekey(t->offset);
		g_message("offset = %d",t->offset);
	} else
		be->set_key(t->key);
	
	if (be->get_config_entry(t->mod_name, "CipherKey")) {
		t->cipher_old = be->get_config_entry(t->mod_name, "CipherKey");
		t->is_locked = strlen(t->cipher_old);
	} else {
		t->is_locked = 0;
		t->cipher_old = NULL;
	}	
	
	be->display_mod->Display();
	bible_apply_change = TRUE;
	
#ifndef USE_GTKHTML38
	if(type == PERCOM_TYPE)
		gtk_html_set_editable(ec->html, TRUE);
#endif
	if(type == DICTIONARY_TYPE)
		gtk_entry_set_text(GTK_ENTRY(t->entry),t->key);	
	return t;
}

/******   end of file   ******/
