/*
 * GnomeSword Bible Study Tool
 * tabbed_browser.c - functions to facilitate tabbed browsing of different passages at once
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


#include "gui/gnomesword.h"
#include "gui/gtkhtml_display.h"
#include "gui/tabbed_browser.h"
#include "gui/bibletext.h"
#include "gui/bibletext_menu.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary.h"
//#include "gui/shortcutbar_main.h"
//#include "gui/sidebar.h"
//#include "gui/cipher_key_dialog.h"
#include "gui/html.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_search.h"
#include "gui/dialog.h"
#include "gui/font_dialog.h"
#include "gui/dictlex.h"
#include "gui/widgets.h"

//#include "main/bibletext.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/sword.h"
#include "main/xml.h"

static void on_notebook_main_switch_page(GtkNotebook * notebook,
					 GtkNotebookPage * page,
					 gint page_num, GList * tl);
static void notebook_main_add_page(PASSAGE_TAB_INFO *tbinf);

/******************************************************************************
 * externs
 */

//extern gboolean gsI_isrunning;
PASSAGE_TAB_INFO *cur_passage_tab;

/******************************************************************************
 * globals to this file only 
 */
static GList *passage_list;
//static gboolean display_change = TRUE;
//static gint text_last_page;



/******************************************************************************
 * Name
 *  on_notebook_main_switch_page
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void on_notebook_main_switch_page(GtkNotebook * notebook,
 *				  GtkNotebookPage * page,
 *				  gint page_num, GList * tl)	
 *
 * Description
 *   sets gui to new passage
 *
 * Return value
 *   void
 */
static void on_notebook_main_switch_page(GtkNotebook * notebook,
					 GtkNotebookPage * page,
					 gint page_num, GList * tl)
{
	PASSAGE_TAB_INFO *pt;
	
	/* get data structure for new passage */
	pt = (PASSAGE_TAB_INFO*)g_list_nth_data(tl, page_num);
	
	/* point PASSAGE_TAB_INFO *cur_passage_tab to pt - cur_passage_tab is global to this file */
	cur_passage_tab = pt;

	//sets the text mod and key
	gui_set_text_mod_and_key(pt->text_mod, pt->text_commentary_key);
	
	//sets the commentary mod and key
	set_commentary_key(pt->commentary_mod, pt->text_commentary_key);
	
	//sets the dictionary mod and key
	gui_set_dictlex_mod_and_key(pt->dictlex_mod, pt->dictlex_key);
	
	//sets the book mod and key
}

/******************************************************************************
 * Name
 *  gui_open_verse_in_new_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_open_verse_in_new_tab(gchar *verse_key)
 *
 * Description
 *   opens the given verse in a new passage tab
 *
 * Return value
 *   void
 */
void gui_open_verse_in_new_tab(gchar *verse_key)
{
	PASSAGE_TAB_INFO *pt;
	pt = g_new0(PASSAGE_TAB_INFO, 1);
	pt->text_mod = cur_t->mod_name;
	pt->commentary_mod = cur_c->mod_name;
	pt->dictlex_mod = cur_d->mod_name;
	pt->book_mod = NULL;
	pt->text_commentary_key = verse_key;
	pt->dictlex_key = cur_d->key;
	pt->book_key = NULL;
	
	notebook_main_add_page(pt);
	
	passage_list = g_list_append(passage_list, (PASSAGE_TAB_INFO*)pt);
	cur_passage_tab = pt;
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_main),
				gtk_notebook_page_num
				(GTK_NOTEBOOK(widgets.notebook_main),
				pt->page_widget));
}

/******************************************************************************
 * Name
 *  gui_close_passage_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_close_passage_tab(gint pagenum)
 *
 * Description
 *   closes the given passage tab or the current one if pagenum = -1
 *
 * Return value
 *   void
 */
void gui_close_passage_tab(gint pagenum)
{
	if (-1 == pagenum)
		pagenum = gtk_notebook_get_current_page(GTK_NOTEBOOK(widgets.notebook_main));
	
	PASSAGE_TAB_INFO *pt = (PASSAGE_TAB_INFO*)g_list_nth_data(passage_list, (guint)pagenum);
	passage_list = g_list_remove(passage_list, pt);
	
	g_free(pt);
	gtk_notebook_remove_page(GTK_NOTEBOOK(widgets.notebook_main), pagenum);
}

/******************************************************************************
 * Name
 *  notebook_main_add_page
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void notebook_main_add_page(PASSAGE_TAB_INFO *tbinf)
 *
 * Description
 *   adds a new page and label to the main notebook for a new scripture passage
 *
 * Return value
 *   void
 */
static void notebook_main_add_page(PASSAGE_TAB_INFO *tbinf)
{
	GtkWidget *label;
	int i;

	tbinf->page_widget = gtk_vbox_new (FALSE, 0);
	gtk_widget_show(tbinf->page_widget);

	label = gtk_label_new(tbinf->text_mod);
	gtk_widget_show(label);

	gtk_notebook_append_page(GTK_NOTEBOOK(widgets.notebook_main),
				 tbinf->page_widget, label);
	
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.notebook_main),
					tbinf->page_widget,
					(gchar*)tbinf->text_mod);
}

/******************************************************************************
 * Name
 *  gui_notebook_main_setup
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_notebook_main_setup(GList *ptlist)
 *
 * Description
 *   set up notebook for browsing multiple passages
 *
 * Return value
 *   void
 */
void gui_notebook_main_setup(GList *ptlist)
{
	GList *tmp = NULL;
	PASSAGE_TAB_INFO *pt;
	
	passage_list = NULL;
	tmp = ptlist;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		pt = g_new0(PASSAGE_TAB_INFO, 1);
		pt->text_mod = NULL;
		pt->commentary_mod = NULL;
		pt->dictlex_mod = NULL;
		pt->book_mod = NULL;
		pt->text_commentary_key = NULL;
		pt->dictlex_key = NULL;
		pt->book_key = NULL;

		notebook_main_add_page(pt);
		passage_list = g_list_append(passage_list, (PASSAGE_TAB_INFO*)pt);
		tmp = g_list_next(tmp);
	}

	cur_passage_tab = pt;
	g_signal_connect(GTK_OBJECT(widgets.notebook_main),
			   "switch_page",
			   G_CALLBACK
			   (on_notebook_main_switch_page), passage_list);

	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  gui_notebook_main_shutdown
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *  void gui_notebook_main_shutdown(void)	
 *
 * Description
 *   shut down main notebook and clean mem
 *
 * Return value
 *   void
 */
void gui_notebook_main_shutdown(void)
{
	passage_list = g_list_first(passage_list);
	while (passage_list != NULL) {
		PASSAGE_TAB_INFO *t = (PASSAGE_TAB_INFO*)passage_list->data;
		g_free((PASSAGE_TAB_INFO*)passage_list->data);
		passage_list = g_list_next(passage_list);
	}
	g_list_free(passage_list);
}
