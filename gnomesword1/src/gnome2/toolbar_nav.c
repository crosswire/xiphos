/*
 * GnomeSword Bible Study Tool
 * toolbar_nav.c - Bible text navigation toolbar
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
#ifdef USE_GTKHTML30
#include <gal/widgets/e-unicode.h>
#endif

#include "gui/toolbar_nav.h"
#include "gui/shortcutbar_main.h"
#include "gui/history.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"
#include "gui/tabbed_browser.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/navbar.h"
#include "main/url.hh"
#include "main/xml.h"

NAVBAR navbar_main;
NAV_BAR nav_bar;
GtkWidget *cbe_book;
GtkWidget *spb_chapter;
GtkWidget *spb_verse;
GtkWidget *cbe_freeform_lookup;
extern gboolean do_display;
//static gchar *f_message = "toolbar_nav.c line #%d \"%s\" = %s";

/******************************************************************************
 * Name
 *   on_cbeBook_changed
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_cbeBook_changed(GtkEditable * editable, 
 *						gpointer user_data)	
 *
 * Description
 *   change book 
 *
 * Return value
 *   void
 */

static void on_cbeBook_changed(GtkEditable * editable,
			       gpointer user_data)
{
	gchar *url;
	G_CONST_RETURN gchar *bookname;
	if (settings.apply_change) {
		bookname = (gchar*)gtk_entry_get_text(GTK_ENTRY(editable)); // e_utf8_gtk_editable_get_text(editable);//e_utf8_
		if (*bookname) {
			//g_message(f_message,76,"bookname",bookname);
			
			url = g_strdup_printf("sword:///%s 1:1", bookname);
			//g_message(f_message,80,"url",url);
			main_url_handler(url, TRUE);
			g_free(url);
		}
	}
}

/******************************************************************************
 * Name
 *   on_spbChapter_button_release_event
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   gboolean on_spbChapter_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event,   gpointer user_data)	
 *
 * Description
 *    change chapter 
 *
 * Return value
 *   gboolean
 */

static gboolean on_spbChapter_button_release_event(GtkWidget * widget,
						   GdkEventButton *
						   event,
						   gpointer user_data)
{
	if (settings.apply_change) {
		const gchar *bookname = gtk_entry_get_text(GTK_ENTRY(cbe_book));
		gint chapter = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (spb_chapter));
		gchar *url = g_strdup_printf("sword:///%s %d:1", bookname, 
						chapter);
		main_url_handler(url, TRUE);
		g_free(url);
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_spbVerse_button_release_event
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   gboolean on_spbVerse_button_release_event(GtkWidget * widget, 
 *			GdkEventButton * event,  gpointer user_data)	
 *
 * Description
 *    change verse
 *
 * Return value
 *   gboolean
 */

static gboolean on_spbVerse_button_release_event(GtkWidget * widget,
						 GdkEventButton * event,
						 gpointer user_data)
{
	if (settings.apply_change) {
		const gchar *bookname = gtk_entry_get_text(GTK_ENTRY(cbe_book));
		int chapter = gtk_spin_button_get_value(GTK_SPIN_BUTTON
						(spb_chapter));
		int verse = gtk_spin_button_get_value(GTK_SPIN_BUTTON
						(spb_verse));
		gchar *url = g_strdup_printf("sword:///%s %d:%d", bookname, 
						chapter, verse);
		main_url_handler(url, TRUE);
		g_free(url);
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_button_dict_book_clicked
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_button_dict_book_clicked(GtkButton * button,
 *				      gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_button_dict_book_clicked(GtkButton * button, gpointer user_data)
{
/*	if (gtk_notebook_get_current_page
	    (GTK_NOTEBOOK(widgets.workbook_lower)))
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.workbook_lower),
					      0);
	else
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.workbook_lower),
					      1);
*/
}


/******************************************************************************
 * Name
 *   on_togglebutton_parallel_view_toggled
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_togglebutton_parallel_view_toggled(GtkToggleButton *
 *						  togglebutton,
 *						  gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_togglebutton_parallel_view_toggled(GtkToggleButton *
						  togglebutton,
						  gpointer user_data)
{
	if(togglebutton->active) 
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
						      1);
	else
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
						      0);
}


/******************************************************************************
 * Name
 *   on_btnLookup_clicked
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_btnLookup_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *    go to verse in free form entry
 *
 * Return value
 *   void
 */

static void on_btnLookup_clicked(GtkButton * button, gpointer user_data)
{
	const gchar *buf;
	gchar *url;
	
	buf = gtk_entry_get_text(GTK_ENTRY(cbe_freeform_lookup));
	url = g_strdup_printf("sword:///%s",buf);
	main_url_handler(url, TRUE); //-- change verse to entry text
	g_free(url);
}

/******************************************************************************
 * Name
 *   on_cbeFreeformLookup_key_press_event
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   gboolean on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
 *				GdkEventKey * event, gpointer user_data)	
 *
 * Description
 *   go to verse in free form entry if user hit <enter>
 *
 * Return value
 *   gboolean
 */

static gboolean on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
						     GdkEventKey *
						     event,
						     gpointer user_data)
{
	const gchar *buf;
	gchar *url;

	buf = gtk_entry_get_text(GTK_ENTRY(widget));
	/* if <enter> key */
	if (event->keyval == 65293 || event->keyval == 65421) {
		url = g_strdup_printf("sword:///%s",buf);
		main_url_handler(url, TRUE);
		g_free(url);
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_entry_activate
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void on_entry_activate(GtkEntry * entry, DIALOG_DATA * c)	
 *
 * Description
 *   go to verse in free form entry if user hit <enter>
 *
 * Return value
 *   void
 */

static void on_entry_activate(GtkEntry * entry, gpointer data)
{
	const gchar *buf = gtk_entry_get_text(entry);
	if(navbar_main.key)
		g_free(navbar_main.key);
	navbar_main.key = g_strdup(buf);
	gchar *url = g_strdup_printf("sword:///%s", buf);
	main_url_handler(url, TRUE);
	g_free(url);
	main_navbar_set(navbar_main,navbar_main.key);
}


/******************************************************************************
 * Name
 *   on_button_back_clicked
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_nav_bar.button_back_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *    move backward through history list
 *
 * Return value
 *   void
 */

static void on_button_back_clicked(GtkButton * button,
				   gpointer user_data)
{
	gui_navigate_history(widgets.app, 0);
}

/******************************************************************************
 * Name
 *   on_button_forward_clicked
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_button_forward_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   go forward through history list 
 *
 * Return value
 *   void
 */

static void on_button_forward_clicked(GtkButton * button,
				      gpointer user_data)
{
	gui_navigate_history(widgets.app, 1);
}

static void on_comboboxentry4_changed(GtkComboBox * combobox, gpointer data)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel* model = gtk_combo_box_get_model(combobox);
	
	if(!do_display)
		return;
#ifdef DEBUG
	g_message("on_comboboxentry4_changed");
#endif
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   0, &book, -1);
	
	url = g_strdup_printf("sword:///%s 1:1",book);	
	buf = g_strdup_printf("%s 1:1",book);
	
	main_url_handler(url, TRUE);
	main_navbar_set(navbar_main,buf);
	g_free(url);
	g_free(book);
	g_free(buf);
}


static void on_comboboxentry5_changed(GtkComboBox * combobox, gpointer data)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *chapter = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	
	GtkTreeModel* model = gtk_combo_box_get_model(combobox);
	GtkTreeModel* book_model = gtk_combo_box_get_model(GTK_COMBO_BOX(navbar_main.comboboxentry_book));
	
	
	if(!do_display)
		return;
#ifdef DEBUG
	g_message("on_comboboxentry5_changed");
#endif
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(
				navbar_main.comboboxentry_book), &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter,
			   0, &book, -1);
	
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   0, &chapter, -1);
	
	url = g_strdup_printf("sword:///%s %s:1",
				book,
				chapter);
	buf = g_strdup_printf("%s %s:1",
				book,
				chapter);
	main_url_handler(url, TRUE);
	main_navbar_set(navbar_main,buf);
	
	g_free(url);
	g_free(book);
	g_free(chapter);
	g_free(buf);		
}


static void on_comboboxentry6_changed(GtkComboBox * combobox, gpointer data)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *chapter = NULL;
	gchar *verse = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	
	GtkTreeModel* model = gtk_combo_box_get_model(combobox);
	GtkTreeModel* book_model = gtk_combo_box_get_model(GTK_COMBO_BOX(navbar_main.comboboxentry_book));
	GtkTreeModel* chapter_model = gtk_combo_box_get_model(GTK_COMBO_BOX(navbar_main.comboboxentry_chapter));

	
	if(!do_display)
		return;
#ifdef DEBUG
	g_message("on_comboboxentry6_changed");
#endif
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(
				navbar_main.comboboxentry_book), &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter,
			   0, &book, -1);
	
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(
				navbar_main.comboboxentry_chapter), &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(chapter_model), &iter,
			   0, &chapter, -1);
	
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   0, &verse, -1);
	
	url = g_strdup_printf("sword:///%s %s:%s",
				book,
				chapter,
				verse);
	buf = g_strdup_printf("%s %s:%s",
				book,
				chapter,
				verse);
	main_url_handler(url, TRUE);
	main_navbar_set(navbar_main,buf);
	
	g_free(url);
	g_free(book);
	g_free(chapter);
	g_free(verse);
	g_free(buf);
}

/******************************************************************************
 * Name
 *   gui_create_nav_toolbar
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   GtkWidget *gui_create_nav_toolbar(void)	
 *
 * Description
 *    create the Bible text navigation toolbar and return it
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_nav_toolbar(GtkWidget * app)
{
	//GtkWidget *toolbarNav;
/****************************************************************************/
	GtkWidget *hbox3;
	GtkWidget *image;
	GtkWidget *separatortoolitem;
	GtkListStore *store;
	GtkCellRenderer *renderer;
	
	GtkTooltips *tooltips;
	
	tooltips = gtk_tooltips_new ();
	
	navbar_main.is_dialog = FALSE;
	navbar_main.key = g_strdup(settings.currentverse);
	navbar_main.module_name = g_strdup(settings.MainWindowModule);
/*
	toolbarNav = gtk_toolbar_new();
	gtk_widget_show(toolbarNav);
	gtk_box_pack_start(GTK_BOX(app), toolbarNav, FALSE,
			   FALSE, 0);
	
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbarNav),
			      GTK_TOOLBAR_ICONS);

*/	
	hbox3 = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox3);
	gtk_container_set_border_width(GTK_CONTAINER(hbox3), 3);
	
	navbar_main.button_back	 = gtk_button_new();
	gtk_widget_show(navbar_main.button_back);
	gtk_box_pack_start(GTK_BOX(hbox3), navbar_main.button_back, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(navbar_main.button_back), GTK_RELIEF_NONE);
	
	nav_bar.button_back = navbar_main.button_back;
	
	image =
	    gtk_image_new_from_stock("gtk-go-back", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image);
	gtk_container_add(GTK_CONTAINER(navbar_main.button_back), image);

	navbar_main.button_forward = gtk_button_new();
	gtk_widget_show(navbar_main.button_forward);
	gtk_box_pack_start(GTK_BOX(hbox3), navbar_main.button_forward, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(navbar_main.button_forward), GTK_RELIEF_NONE);

	nav_bar.button_forward = navbar_main.button_forward;
	
	image =
	    gtk_image_new_from_stock("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image);
	gtk_container_add(GTK_CONTAINER(navbar_main.button_forward), image);

	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE, TRUE,
			   0);

	store = gtk_list_store_new (1, G_TYPE_STRING);
	navbar_main.comboboxentry_book = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));
	//comboboxentry4 = gtk_combo_box_entry_new_text (); //gtk_combo_box_entry_new();
	gtk_widget_show(navbar_main.comboboxentry_book);
	gtk_box_pack_start(GTK_BOX(hbox3), navbar_main.comboboxentry_book, TRUE, TRUE, 0);
	gtk_widget_set_size_request(navbar_main.comboboxentry_book, -1, 6);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (navbar_main.comboboxentry_book), renderer, TRUE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (navbar_main.comboboxentry_book), renderer,
                                "text", 0,
                                NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM
					 (separatortoolitem), FALSE);
					 
	store = gtk_list_store_new (1, G_TYPE_STRING);
	
	navbar_main.comboboxentry_chapter = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));//gtk_combo_box_entry_new();
	gtk_widget_show(navbar_main.comboboxentry_chapter);
	gtk_box_pack_start(GTK_BOX(hbox3), navbar_main.comboboxentry_chapter, FALSE, TRUE, 0);
	gtk_widget_set_size_request(navbar_main.comboboxentry_chapter, 61, -1);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (navbar_main.comboboxentry_chapter), renderer, TRUE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (navbar_main.comboboxentry_chapter), renderer,
                                "text", 0,
                                NULL);

	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM
					 (separatortoolitem), FALSE);
		 
	store = gtk_list_store_new (1, G_TYPE_STRING);
	navbar_main.comboboxentry_verse = gtk_combo_box_new_with_model (GTK_TREE_MODEL (store));//gtk_combo_box_entry_new();
	gtk_widget_show(navbar_main.comboboxentry_verse);
	gtk_box_pack_start(GTK_BOX(hbox3), navbar_main.comboboxentry_verse, FALSE, TRUE, 0);
	gtk_widget_set_size_request(navbar_main.comboboxentry_verse, 61, -1);
	

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (navbar_main.comboboxentry_verse), renderer, TRUE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (navbar_main.comboboxentry_verse), renderer,
                                "text", 0,
                                NULL);
	

	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE, TRUE,
			   0);

	navbar_main.lookup_entry = gtk_entry_new();
	gtk_widget_show(navbar_main.lookup_entry);
	gtk_box_pack_start(GTK_BOX(hbox3), navbar_main.lookup_entry, TRUE, TRUE, 0);
	
	nav_bar.lookup_entry = navbar_main.lookup_entry;
	
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
	gtk_widget_set_sensitive(nav_bar.button_back, FALSE);
	
	
	g_signal_connect ((gpointer) navbar_main.comboboxentry_book, "changed",
		    G_CALLBACK (on_comboboxentry4_changed),
		    NULL);
	g_signal_connect ((gpointer) navbar_main.comboboxentry_chapter, "changed",
		    G_CALLBACK (on_comboboxentry5_changed),
		    NULL);
	g_signal_connect ((gpointer) navbar_main.comboboxentry_verse, "changed",
		    G_CALLBACK (on_comboboxentry6_changed),
		    NULL);
	g_signal_connect ((gpointer) navbar_main.lookup_entry, "activate",
		    G_CALLBACK (on_entry_activate),
		    NULL);

/*  	GLADE_HOOKUP_OBJECT (vbox_tab, entry1, "entry1");
	
	g_signal_connect((gpointer) entry1,
			 "focus",
			 G_CALLBACK(on_focus), (TAB *) tab);
	g_signal_connect((gpointer) entry1,
			 "focus-in-event",
			 G_CALLBACK(on_focus_in), (TAB *) tab);*/
	return hbox3;
/*
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *cbBook;
	GtkObject *spbChapter_adj;
	GtkObject *spbVerse_adj;
	GtkWidget *btnLookup;
	GtkWidget *button_dict_book;
	GtkWidget *togglebutton_parallel_view;
	GtkWidget *label;
	GtkObject *spb_chapter_adj;
	GtkObject *spb_verse_adj;*/
/*
	gnome_app_add_toolbar(GNOME_APP(app), GTK_TOOLBAR(toolbarNav),
			      "toolbarNav",
			      BONOBO_DOCK_ITEM_BEH_EXCLUSIVE,
			      BONOBO_DOCK_TOP, 1, 0, 0);

	nav_bar.button_back = gtk_toolbar_insert_stock (GTK_TOOLBAR (toolbarNav),
                                "gtk-go-back",
                                _("Move backwards through history"),
                                NULL, NULL, NULL, -1);
	gtk_widget_show(nav_bar.button_back);
	
	nav_bar.button_forward = gtk_toolbar_insert_stock (GTK_TOOLBAR (toolbarNav),
                                "gtk-go-forward",
                                _("Move foward through history"),
                                NULL, NULL, NULL, -1);					   
	gtk_widget_show(nav_bar.button_forward);
	
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
	gtk_widget_set_sensitive(nav_bar.button_back, FALSE);
	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

	widgets.cb_books = gtk_combo_new();
	gtk_widget_show(widgets.cb_books);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), widgets.cb_books, 
				NULL,
				NULL);
	

	cbe_book = GTK_COMBO(widgets.cb_books)->entry;
	gtk_widget_show(cbe_book);
	gtk_tooltips_set_tip(tooltips, cbe_book, _("Select a Book of the Bible"), 
				NULL);
	gtk_entry_set_text(GTK_ENTRY(cbe_book), _("Romans"));

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	spb_chapter_adj = gtk_adjustment_new(8, 0, 151, 1, 10, 10);
	spb_chapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spb_chapter_adj), 1, 0);
	gtk_widget_show(spb_chapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), spb_chapter,
				  _("Change Chapter"), NULL);

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	spb_verse_adj = gtk_adjustment_new(28, 0, 180, 1, 10, 10);
	spb_verse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spb_verse_adj), 1, 0);
	gtk_widget_show(spb_verse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), spb_verse,
				  _("Change Verse"), NULL);

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	cbe_freeform_lookup = gtk_entry_new();
	gtk_widget_show(cbe_freeform_lookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav),
				  cbe_freeform_lookup, 
		_("Enter a Verse reference in Book 1:1 format and press Return or click the Go to Verse button")
		, NULL);
		
	nav_bar.lookup_entry = cbe_freeform_lookup;

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	
	btnLookup = gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbarNav),
					     GTK_STOCK_JUMP_TO,
		_("Go to verse"),
					     NULL, NULL, NULL, -1);
	gtk_widget_show(btnLookup);
	
	g_signal_connect(GTK_OBJECT(cbe_book), "changed",
			   G_CALLBACK(on_cbeBook_changed), NULL);
	g_signal_connect(GTK_OBJECT(spb_chapter),
			   "button_release_event",
			   G_CALLBACK
			   (on_spbChapter_button_release_event), NULL);
	g_signal_connect(GTK_OBJECT(spb_verse),
			   "button_release_event",
			   G_CALLBACK(on_spbVerse_button_release_event),
			   NULL);
	g_signal_connect(GTK_OBJECT(cbe_freeform_lookup),
			   "key_press_event",
			   G_CALLBACK
			   (on_cbeFreeformLookup_key_press_event),
			   NULL);
	g_signal_connect(GTK_OBJECT(btnLookup), "clicked",
			   G_CALLBACK(on_btnLookup_clicked), NULL);
	g_signal_connect(GTK_OBJECT(nav_bar.button_back), "clicked",
			   G_CALLBACK(on_button_back_clicked), NULL);
	g_signal_connect(GTK_OBJECT(nav_bar.button_forward),
			   "clicked",
			   G_CALLBACK(on_button_forward_clicked), NULL);
*/
//	return toolbarNav;
}
