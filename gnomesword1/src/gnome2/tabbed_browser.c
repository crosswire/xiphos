/*
 * GnomeSword Bible Study Tool
 * tabbed_browser.c - functions to facilitate tabbed browsing of different passages at once
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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


#include "gui/gnomesword.h"
//#include "gui/gtkhtml_display.h"
#include "gui/tabbed_browser.h"
#include "gui/bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary.h"
#include "gui/shortcutbar_main.h"
#include "gui/sidebar.h"
#include "gui/cipher_key_dialog.h"
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
					gint page_num, GList **tl);
static GtkWidget* tab_widget_new(PASSAGE_TAB_INFO *tbinf,
					const gchar *label_text);




/******************************************************************************
 * externs
 */

//extern gboolean gsI_isrunning;
PASSAGE_TAB_INFO *cur_passage_tab;

/******************************************************************************
 * globals to this file only 
 */
static GList *passage_list;
static gboolean page_change = FALSE;
//static gboolean display_change = TRUE;
static gint removed_page;


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   
 */
/* 
static void save_tabs(void) //const gchar * filename)
{
	xmlDocPtr xml_doc;
	xmlNodePtr root_node;
	xmlNodePtr cur_node;
	xmlNodePtr section_node;
	xmlAttrPtr xml_attr;
	const xmlChar *xml_filename;
	gchar *tabs_dir;
	gchar *file;
	GList *tmp = NULL;
	PASSAGE_TAB_INFO *pt;
	gchar * filename = "mytabs.xml";
	
	
	tabs_dir = g_strdup_printf("%s/tabs/",settings.gSwordDir);

	
	if (access(tabs_dir, F_OK) == -1) {
		if ((mkdir(tabs_dir, S_IRWXU)) == -1) {
			printf("can't create tabs dir");
			return;
		}
	}	
	file = g_strdup_printf("%s/%s",tabs_dir,filename);
	xml_filename = (const xmlChar *) file;
	
	xml_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (xml_doc == NULL) {
		fprintf(stderr,
			"Document not created successfully. \n");
		return;
	}

	root_node = xmlNewNode(NULL, (const xmlChar *) "GnomeSword");
	xml_attr = xmlNewProp(root_node, "Version", VERSION);
	xmlDocSetRootElement(xml_doc, root_node);
	
	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *) "tabs", NULL);
	tmp = passage_list;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		pt = (PASSAGE_TAB_INFO*) tmp->data;
		cur_node = xmlNewChild(section_node,
				NULL, (const xmlChar *) "tab", NULL);
		xmlNewProp(cur_node,"text_mod",
				(const xmlChar *)pt->text_mod);		
		xmlNewProp(cur_node, "commentary_mod", 
				(const xmlChar *)pt->commentary_mod);
		xmlNewProp(cur_node, "dictlex_mod",
				(const xmlChar *)pt->dictlex_mod);		
		xmlNewProp(cur_node, "book_mod", 
				(const xmlChar *)pt->book_mod);
		xmlNewProp(cur_node, "text_commentary_key",
				(const xmlChar *)pt->text_commentary_key);		
		xmlNewProp(cur_node, "dictlex_key", 
				(const xmlChar *)pt->dictlex_key);		
		xmlNewProp(cur_node, "book_key", 
				(const xmlChar *)pt->book_key);
		tmp = g_list_next(tmp);
	}
	xmlSaveFormatFile(xml_filename, xml_doc,1);
	xmlFreeDoc(xml_doc);
}
*/


/******************************************************************************
 * Name
 *  on_notebook_main_close_page
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void on_notebook_main_close_page(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
 
static void on_notebook_main_close_page(GtkButton * button, gpointer user_data)
{
	PASSAGE_TAB_INFO *pt = (PASSAGE_TAB_INFO*)user_data;
	gui_close_passage_tab(gtk_notebook_page_num
				(GTK_NOTEBOOK(widgets.notebook_main),
				pt->page_widget));
}


/******************************************************************************
 * Name
 *  set_current_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void set_current_tab (PASSAGE_TAB_INFO *pt)	
 *
 * Description
 *   point cur_passage_tab to the current tab and turns the close button on 
 *   and off
 *
 * Return value
 *   void
 */
 
static void set_current_tab (PASSAGE_TAB_INFO *pt)
{
	PASSAGE_TAB_INFO *ot = cur_passage_tab;
	
	if (ot != NULL && ot->button_close != NULL) {
		gtk_widget_hide (ot->button_close);
		gtk_widget_show (ot->close_pixmap);
	}
	cur_passage_tab = pt;
	if (pt != NULL && pt->button_close != NULL) {
		gtk_widget_show (pt->button_close);
		gtk_widget_hide (pt->close_pixmap);
	}
}


/******************************************************************************
 * Name
 *  tab_widget_new
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   GtkWidget* tab_widget_new(PASSAGE_TAB_INFO *tbinf,const gchar *label_text)	
 *
 * Description
 *   creates a tab widget that contains a label and a close button
 *
 * Return value
 *   GtkWidget*
 */
 
static GtkWidget* tab_widget_new(PASSAGE_TAB_INFO *tbinf, const gchar *label_text)
{
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *box;
	GtkRequisition r;
	GdkColor color;
	
	g_return_val_if_fail(label_text != NULL, NULL);
	tmp_toolbar_icon = gtk_image_new_from_stock(GTK_STOCK_CLOSE, 
					GTK_ICON_SIZE_MENU); 
	gtk_widget_show(tmp_toolbar_icon); 
	
	tbinf->button_close = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(tbinf->button_close), tmp_toolbar_icon);
	gtk_button_set_relief(GTK_BUTTON(tbinf->button_close), GTK_RELIEF_NONE);
	gtk_widget_set_usize (tbinf->button_close, 16, 16);
	
	tbinf->close_pixmap = gtk_image_new_from_stock(GTK_STOCK_CLOSE, 
					GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_size_request (tbinf->button_close, &r);
	gtk_widget_set_usize (tbinf->close_pixmap, r.width, r.height);
	gtk_widget_set_sensitive(tbinf->close_pixmap, FALSE);
	
	tbinf->tab_label = GTK_LABEL(gtk_label_new (label_text));
	gtk_widget_show (GTK_WIDGET(tbinf->tab_label));
	
	color.red = 0;
	color.green = 0; 
	color.blue = 0;
	
	gtk_widget_modify_fg (tbinf->button_close, GTK_STATE_NORMAL, &color);
	gtk_widget_modify_fg (tbinf->button_close, GTK_STATE_INSENSITIVE, &color);
	gtk_widget_modify_fg (tbinf->button_close, GTK_STATE_ACTIVE, &color);
	gtk_widget_modify_fg (tbinf->button_close, GTK_STATE_PRELIGHT, &color);
	gtk_widget_modify_fg (tbinf->button_close, GTK_STATE_SELECTED, &color);
	gtk_widget_show(tbinf->button_close);
	
	box = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(tbinf->tab_label), TRUE, 
					TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), tbinf->button_close, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), tbinf->close_pixmap, FALSE, FALSE, 0);
	
	gtk_widget_show(box);
	
	g_signal_connect (GTK_OBJECT (tbinf->button_close), "clicked",
				G_CALLBACK(on_notebook_main_close_page),
				tbinf);
	
	return box;
}

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
					 gint page_num, GList **tl)
{ 
	gint number_of_pages = gtk_notebook_get_n_pages(notebook);
	PASSAGE_TAB_INFO *pt;
	
	page_change = TRUE;
	/* get data structure for new passage */
	/*
	 * this is needed to stop seg fault if the left tab is closed when
	 * there are only two tabs - because number_of_pages equals 2 even
	 * thought there is only 1. 
	 */
	if(number_of_pages == 2 && removed_page == 0)
		pt = (PASSAGE_TAB_INFO*)g_list_nth_data(*tl, 0);
	else
		pt = (PASSAGE_TAB_INFO*)g_list_nth_data(*tl, page_num);
	removed_page = 1;
	/* point PASSAGE_TAB_INFO *cur_passage_tab to pt - cur_passage_tab is global to this file */
	set_current_tab (pt);
	
	//sets the text mod and key
	main_display_bible(pt->text_mod, pt->text_commentary_key);
	
	//sets the commentary mod and key
	if(pt->comm_showing)
		main_display_commentary(pt->commentary_mod, 
				pt->text_commentary_key);
	else
		main_display_book(pt->book_mod, pt->book_offset);
//	set_commentary_key(pt->commentary_mod, pt->text_commentary_key);
	
	//sets the dictionary mod and key
	main_display_dictionary(pt->dictlex_mod, pt->dictlex_key);
//	gui_set_dictlex_mod_and_key(pt->dictlex_mod, pt->dictlex_key);
	page_change = FALSE;
	//gtk_notebook_reorder_child(notebook,GTK_WIDGET(pt->page_widget),0);
	//sets the book mod and key
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
	GtkWidget *tab_widget;
	GtkWidget *menu_label;
	
	tbinf->page_widget = gtk_vbox_new (FALSE, 0);
	gtk_widget_show(tbinf->page_widget);

	tab_widget = tab_widget_new(tbinf, (gchar*)tbinf->text_commentary_key);

	gtk_notebook_append_page(GTK_NOTEBOOK(widgets.notebook_main),
				 tbinf->page_widget, tab_widget);
	
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(widgets.notebook_main),
					tbinf->page_widget,
					(gchar*)tbinf->text_commentary_key);
	
	menu_label = gtk_label_new((gchar*)tbinf->text_commentary_key);	
	gtk_notebook_set_menu_label(GTK_NOTEBOOK(widgets.notebook_main),
                                             tbinf->page_widget,
                                             menu_label);
}


/******************************************************************************
 * Name
 *  gui_set_tab_label
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_set_tab_label(TABED_PAGE *p)	
 *
 * Description
 *   sets current tab label to current verse
 *
 * Return value
 *   void
 */
 
void gui_set_tab_label(const gchar * key)
{
	GString *str = g_string_new(NULL);
	
	if(cur_passage_tab->text_commentary_key)
		g_free(cur_passage_tab->text_commentary_key);
	cur_passage_tab->text_commentary_key = g_strdup(key);
		
	g_string_printf(str,"%s: %s",
				cur_passage_tab->text_mod,
				cur_passage_tab->text_commentary_key); 
	gtk_label_set_text (cur_passage_tab->tab_label,str->str);
	gtk_notebook_set_menu_label_text(
					GTK_NOTEBOOK(widgets.notebook_main),
                                        cur_passage_tab->page_widget,
                                            str->str );
	g_string_free(str,TRUE);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
 
void gui_update_tab_struct(const gchar * text_mod, 
			   const gchar * commentary_mod, 
			   const gchar * dictlex_mod, 
			   const gchar * book_mod, 
			   const gchar * dictlex_key,
			   const gchar * book_offset,
			   gboolean comm_showing)
{	
	if(page_change) 
		return;
	
	
	if(text_mod) {
		if(cur_passage_tab->text_mod)
			g_free(cur_passage_tab->text_mod);
		cur_passage_tab->text_mod = g_strdup(text_mod);	
				
	}
	if(commentary_mod) {
		cur_passage_tab->comm_showing = comm_showing;
		if(cur_passage_tab->commentary_mod)
			g_free(cur_passage_tab->commentary_mod);
		cur_passage_tab->commentary_mod = g_strdup(commentary_mod);		
	} 
	if(dictlex_mod) {
		if(cur_passage_tab->dictlex_mod)
			g_free(cur_passage_tab->dictlex_mod);
		cur_passage_tab->dictlex_mod = g_strdup(dictlex_mod);		
	} 
	if(book_mod) {
		cur_passage_tab->comm_showing = comm_showing;
		if(cur_passage_tab->book_mod)
			g_free(cur_passage_tab->book_mod);
		cur_passage_tab->book_mod = g_strdup(book_mod);		
	}
	if(book_offset) {
		if(cur_passage_tab->book_offset)
			g_free(cur_passage_tab->book_offset);
		cur_passage_tab->book_offset = g_strdup(book_mod);		
	}
	/*if(text_commentary_key) {
		if(cur_passage_tab->text_commentary_key)
			g_free(cur_passage_tab->text_commentary_key);
		cur_passage_tab->text_commentary_key = g_strdup(text_commentary_key);		
	}*/
	if(dictlex_key) {
		if(cur_passage_tab->dictlex_key)
			g_free(cur_passage_tab->dictlex_key);
		cur_passage_tab->dictlex_key = g_strdup(dictlex_key);		
	}
}

/******************************************************************************
 * Name
 *  gui_open_passage_in_new_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_open_passage_in_new_tab(gchar *verse_key)
 *
 * Description
 *   opens the given verse in a new passage tab
 *
 * Return value
 *   void
 */
void gui_open_passage_in_new_tab(gchar *verse_key)
{
	PASSAGE_TAB_INFO *pt;
	
	if(!settings.browsing)
		return;
	pt = g_new0(PASSAGE_TAB_INFO, 1);
	pt->text_mod = g_strdup(settings.MainWindowModule);
	pt->commentary_mod = g_strdup(settings.CommWindowModule);
	pt->dictlex_mod = g_strdup(settings.DictWindowModule);
	pt->book_mod = g_strdup(settings.book_mod);
	pt->text_commentary_key = g_strdup(verse_key);
	pt->dictlex_key = g_strdup(settings.dictkey);
	pt->book_offset = NULL;
	pt->comm_showing = settings.comm_showing;
	
	passage_list = g_list_append(passage_list, (PASSAGE_TAB_INFO*)pt);
	set_current_tab(pt);
	notebook_main_add_page(pt);
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_main),
				gtk_notebook_page_num
				(GTK_NOTEBOOK(widgets.notebook_main),
				pt->page_widget));
}


/******************************************************************************
 * Name
 *  gui_open_module_in_new_tab
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_open_module_in_new_tab(gchar *verse_key)
 *
 * Description
 *   opens the given module in a new passage tab
 *
 * Return value
 *   void
 */

void gui_open_module_in_new_tab(gchar *module)
{
	PASSAGE_TAB_INFO *pt;
	gint module_type;
	
	if(!settings.browsing)
		return;
	
	pt = g_new0(PASSAGE_TAB_INFO, 1);
	module_type = main_get_mod_type(module);
	
	switch (module_type) {
	case -1:
		return;
		break;
	case TEXT_TYPE:
		pt->text_mod = g_strdup(module);
		pt->commentary_mod = g_strdup(settings.CommWindowModule);
		pt->dictlex_mod = g_strdup(settings.DictWindowModule);
		pt->book_mod = g_strdup(settings.book_mod);
		break;
	case COMMENTARY_TYPE:
		pt->text_mod = g_strdup(settings.MainWindowModule);
		pt->commentary_mod = g_strdup(module);
		pt->dictlex_mod = g_strdup(settings.DictWindowModule);
		pt->book_mod = g_strdup(settings.book_mod);
		break;
	case DICTIONARY_TYPE:
		pt->text_mod = g_strdup(settings.MainWindowModule);
		pt->commentary_mod = g_strdup(settings.CommWindowModule);
		pt->dictlex_mod = g_strdup(module);
		pt->book_mod = g_strdup(settings.book_mod);
		break;
	case BOOK_TYPE:
		pt->text_mod = g_strdup(settings.MainWindowModule);
		pt->commentary_mod = g_strdup(settings.CommWindowModule);
		pt->dictlex_mod = g_strdup(settings.DictWindowModule);
		pt->book_mod = g_strdup(module);
		break;
	}
		
	pt->text_commentary_key = g_strdup(settings.currentverse);
	pt->dictlex_key = g_strdup(settings.dictkey);	
	pt->book_offset = NULL;
	pt->comm_showing = settings.comm_showing;
	
	passage_list = g_list_append(passage_list, (PASSAGE_TAB_INFO*)pt);
	set_current_tab(pt);
	notebook_main_add_page(pt);
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_main),
				gtk_notebook_page_num
				(GTK_NOTEBOOK(widgets.notebook_main),
				pt->page_widget));
}


/******************************************************************************
 * Name
 *  gui_close_al_tabs
 *
 * Synopsis
 *   #include "tabbed_browser.h"
 *
 *   void gui_close_all_tabs(void)
 *
 * Description
 *   called from preferences dialog when disable browsing is choosen
 *
 * Return value
 *   void
 */

void gui_close_all_tabs(void)
{
	gint i;
	gint number_of_pages = 
		gtk_notebook_get_n_pages(GTK_NOTEBOOK(widgets.notebook_main));
	for(i = number_of_pages-1;i > -1;i--) {
		PASSAGE_TAB_INFO *pt = 
			(PASSAGE_TAB_INFO*)g_list_nth_data(passage_list, (guint)i);
		passage_list = g_list_remove(passage_list, pt);
		g_free(pt->text_mod);
		g_free(pt->commentary_mod);
		g_free(pt->dictlex_mod);
		g_free(pt->book_mod);
		g_free(pt->text_commentary_key);
		g_free(pt->dictlex_key);
		g_free(pt->book_offset);
		g_free(pt);
		gtk_notebook_remove_page(GTK_NOTEBOOK(widgets.notebook_main), i);
	}
	
	g_list_free(passage_list);
	passage_list = NULL;
	cur_passage_tab = NULL;
//	gui_set_text_frame_label(cur_t);
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
	if (1 == gtk_notebook_get_n_pages(GTK_NOTEBOOK(widgets.notebook_main)))
		return;
	PASSAGE_TAB_INFO *pt = (PASSAGE_TAB_INFO*)g_list_nth_data(passage_list, (guint)pagenum);
	passage_list = g_list_remove(passage_list, pt);
	if(pt->text_mod) g_free(pt->text_mod);
	if(pt->commentary_mod) g_free(pt->commentary_mod);
	if(pt->dictlex_mod) g_free(pt->dictlex_mod);
	if(pt->book_mod) g_free(pt->book_mod);
	if(pt->text_commentary_key) g_free(pt->text_commentary_key);
	if(pt->dictlex_key) g_free(pt->dictlex_key);
	if(pt->book_offset) g_free(pt->book_offset);
	g_free(pt);
	cur_passage_tab = NULL;
	removed_page = pagenum;
	gtk_notebook_remove_page(GTK_NOTEBOOK(widgets.notebook_main), pagenum);
}

static void on_notebook_main_new_tab_clicked(GtkButton *button, gpointer user_data)
{
	gui_open_passage_in_new_tab(settings.currentverse);
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
	PASSAGE_TAB_INFO *pt = NULL;
	static gboolean connected = FALSE;
	
	removed_page = 1;
	cur_passage_tab = NULL;
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
		pt->book_offset = NULL;

		passage_list = g_list_append(passage_list, (PASSAGE_TAB_INFO*)pt);
		notebook_main_add_page(pt);
		tmp = g_list_next(tmp);
	}
	if (NULL == pt) {
		pt = g_new0(PASSAGE_TAB_INFO, 1);
		pt->text_mod = g_strdup(settings.MainWindowModule);
		pt->commentary_mod = g_strdup(settings.CommWindowModule);
		pt->dictlex_mod = g_strdup(settings.DictWindowModule);
		pt->book_mod = NULL;
		pt->text_commentary_key = g_strdup(settings.currentverse);
		pt->dictlex_key = g_strdup(settings.dictkey);
		pt->book_offset = NULL;
		passage_list = g_list_append(passage_list, (PASSAGE_TAB_INFO*)pt);
		notebook_main_add_page(pt);
	}
	set_current_tab(pt);
	/*
	 * this is ugly :(
	 * but when browsing is truned off and then back on in preferences
	 * we don't need to connect or bad things happen since we are already
	 * connected.
	 *
	 */
	if(!connected) {
		g_signal_connect(GTK_OBJECT(widgets.notebook_main),
				   "switch_page",
				   G_CALLBACK
				   (on_notebook_main_switch_page), &passage_list);
		g_signal_connect(GTK_OBJECT(widgets.button_new_tab), "clicked",
				   G_CALLBACK(on_notebook_main_new_tab_clicked), NULL);
		connected = TRUE;
	}
		
	//show the new tab button here instead of in main_window.c so it
	//doesn't get shown if !settings.browsing
	//gui_set_text_frame_label(cur_t);
	gtk_widget_show(widgets.button_new_tab);
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
		PASSAGE_TAB_INFO *pt = (PASSAGE_TAB_INFO*)passage_list->data;
		g_free(pt->text_mod);
		g_free(pt->commentary_mod);
		g_free(pt->dictlex_mod);
		g_free(pt->book_mod);
		g_free(pt->text_commentary_key);
		g_free(pt->dictlex_key);
		g_free(pt->book_offset);
		g_free((PASSAGE_TAB_INFO*)passage_list->data);
		passage_list = g_list_next(passage_list);
	}
	g_list_free(passage_list);
	cur_passage_tab = NULL;
}
