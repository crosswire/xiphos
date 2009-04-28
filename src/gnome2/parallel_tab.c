/*
 * Xiphos Bible Study Tool
 * parallel_dialog.c - dialog for detached parallel
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <glade/glade-xml.h>
#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif  /* USE_GTKMOZEMBED */

#include "gui/parallel_tab.h"
#include "gui/parallel_view.h"
#include "gui/xiphos.h"
#include "gui/navbar_versekey_parallel.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/lists.h"
#include "main/navbar.h"
#include "main/navbar_versekey.h"
#include "main/parallel_view.h"
#include "main/settings.h"
#include "main/url.hh"
#include "main/xml.h"



extern gboolean do_display;
extern gboolean shift_key_pressed;

GtkWidget *entrycbIntBook;
GtkWidget *sbIntChapter;
GtkWidget *sbIntVerse;
GtkWidget *entryIntLookup;
    
PARA_LABELS plabels;

/******************************************************************************
 * static
 */
static GtkWidget *parallel_vbox;
NAVBAR_VERSEKEY navbar_parallel;
#ifdef OLD_NAVBAR
static NAVBAR navbar;
static GtkWidget *sync_button;
#endif

static void sync_with_main(const gchar * key);


#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb(GeckoHtml *html,
			gchar *uri,
			gpointer user_data)
{
	gui_popup_menu_parallel();
}
#else
static gboolean
_popupmenu_requested_cb(GtkHTML *html,
			GdkEventButton * event,
			gpointer date)
{
	if (event->button == 3) {
		gui_popup_menu_parallel();
		return 1;
	}
	return 0;
}
#endif


/******************************************************************************
 * Name
 *   on_btnDockInt_clicked
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   void on_btnDockInt_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 *
 *   
 *
 * Return value
 *   void
 */

void gui_parallel_close_tab(GtkButton * button, gpointer user_data)
{
	//gtk_widget_destroy(parallel_UnDock_Dialog);
}


/******************************************************************************
 * Name
 *   on_dlgparallel_destroy
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   void on_dlgparallel_destroy(GtkObject *object, gpointer user_data)
 *
 * Description
 *   
 *
 *
 *   
 *
 * Return value
 *   void
 */

static void on_parallel_tab_destroy(GtkObject * object,
				      gpointer user_data)
{
	
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                             TRUE);
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_bible_parallel), 
					1);
	settings.dockedInt = TRUE;
	main_update_parallel_page();
}


/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/parallel_dialog.h"
 *
 *   void sync_with_main(void)	
 *
 * Description navbar.lookup_entry
 *   bring the the Parallel Dialog into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(const gchar * key)
{	
	gchar *buf = NULL;
	gchar *url = NULL;
	
	if(key)
		buf = (gchar*)main_url_encode(key);
	else
		buf = (gchar*)main_url_encode(xml_get_value("keys", "verse"));
	GS_message(("%s",buf));
	if(buf && (strlen(buf) > 3)) {
		url =
		    g_strdup_printf("xiphos.url?action=showParallel&"
					"type=verse&value=%s",
					buf);
		
		main_url_handler(url, TRUE);
		g_free(url);
	}
}


/******************************************************************************
 * Name
 *   gui_keep_parallel_dialog_in_sync
 *
 * Synopsis
 *   #include "gui/parallel_dialog.h"
 *
 *   void gui_keep_parallel_dialog_in_sync(const char * key)
 *
 * 
 *
 * Return value
 *   void
 */

void gui_keep_parallel_tab_in_sync(void)
{
#ifdef OLD_NAVBAR
	if(GTK_TOGGLE_BUTTON(sync_button)->active)
#else
	if(GTK_TOGGLE_BUTTON(navbar_parallel.button_sync)->active)
#endif
		sync_with_main(NULL);
}

void gui_parallel_tab_sync(const gchar * key)
{
	sync_with_main(key);	
}


/******************************************************************************
 * Name
 *   gui_set_parallel_navbar
 *
 * Synopsis
 *   #include "gui/parallel_dialog.h"
 *
 *   void gui_set_parallel_navbar(const char key)	
 *
 * 
 *
 * Return value
 *   void
 */
/*
void gui_set_parallel_navbar(const char * key)
{
	main_navbar_set(navbar, key);
}
*/


#ifdef OLD_NAVBAR
/******************************************************************************
 * Name
 *   sync_toggled
 *
 * Synopsis
 *   #include "gui/parallel_dialog.h"
 *
 *   void sync_toggled(GtkToggleButton * button, DIALOG_DATA * c)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void sync_toggled(GtkToggleButton * button, gpointer data)
{
	if (button->active)
		sync_with_main(NULL);
}

/******************************************************************************
 * Name
 *   on_entry_activate
 *
 * Synopsis
 *   #include "parallel_dialog.h"
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
	if (navbar.key)
		g_free(navbar.key);
	navbar.key = g_strdup(buf);
	gchar *url =
	    g_strdup_printf("xiphos.url?action=showParallel&"
				"type=verse&value=%s",
				main_url_encode(buf));
	main_url_handler(url, TRUE);
	g_free(url);
	//main_navbar_set(navbar, navbar.key);
}

static void on_comboboxentry4_changed(GtkComboBox * combobox, gpointer data)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	if (!do_display)
		return;
	GS_message(("on_comboboxentry4_changed"));
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &book, -1);

	//url = g_strdup_printf("sword:///%s 1:1", book);
	buf = g_strdup_printf("%s 1:1", book);
	url = g_strdup_printf("xiphos.url?action=showParallel&"
				"type=verse&value=%s",
				main_url_encode(buf));
	main_url_handler(url, TRUE);
	//main_navbar_set(navbar, buf);
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

	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	GtkTreeModel *book_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (navbar.comboboxentry_book));


	if (!do_display)
		return;
	GS_message(("on_comboboxentry5_changed"));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   0, &chapter, -1);

	buf = g_strdup_printf("%s %s:1", book, chapter);
	url = g_strdup_printf("xiphos.url?action=showParallel&"
				"type=verse&value=%s",
				main_url_encode(buf));
	main_url_handler(url, TRUE);
	//main_dialogs_url_handler(c, url, TRUE);
	//main_navbar_set(navbar, buf);

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

	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	GtkTreeModel *book_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (navbar.comboboxentry_book));
	GtkTreeModel *chapter_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (navbar.comboboxentry_chapter));


	if (!do_display)
		return;
	GS_message(("on_comboboxentry6_changed"));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (navbar.comboboxentry_chapter),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(chapter_model), &iter, 0,
			   &chapter, -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &verse, -1);

	buf = g_strdup_printf("%s %s:%s", book, chapter, verse);
	url = g_strdup_printf("xiphos.url?action=showParallel&"
				"type=verse&value=%s",
				main_url_encode(buf));
	main_url_handler(url, TRUE);
	//main_dialogs_url_handler(c, url, TRUE);
	//main_navbar_set(navbar, buf);

	g_free(url);
	g_free(book);
	g_free(chapter);
	g_free(verse);
	g_free(buf);
}
#endif /* OLD_NAVBAR */

/******************************************************************************
 * Name
 *   create_nav_toolbar
 *
 * Synopsis
 *   #include ".h"
 *
 *   GtkWidget *create_nav_toolbar(void)	
 *
 * Description
 *    create navigation toolbar and 
 *
 * Return value
 *   void
 */

static GtkWidget *create_nav_toolbar(void)
{
#ifdef OLD_NAVBAR
	GtkWidget *hbox3;
	GtkWidget *image;
	GtkWidget *separatortoolitem;
	GtkListStore *store;
	GtkCellRenderer *renderer;

	hbox3 = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox3);
	gtk_container_set_border_width(GTK_CONTAINER(hbox3), 3);

	sync_button = gtk_toggle_button_new();
	gtk_widget_show(sync_button);
	gtk_box_pack_start(GTK_BOX(hbox3), sync_button, FALSE, FALSE,
			   0);
	gtk_button_set_relief(GTK_BUTTON(sync_button), GTK_RELIEF_NONE);

	image =
	    gtk_image_new_from_stock("gtk-refresh",
				     GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image);
	gtk_container_add(GTK_CONTAINER(sync_button), image);

	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);

	store = gtk_list_store_new(1, G_TYPE_STRING);
	navbar.comboboxentry_book =
	    gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	gtk_widget_show(navbar.comboboxentry_book);
	gtk_box_pack_start(GTK_BOX(hbox3), navbar.comboboxentry_book,
			   TRUE, TRUE, 0);
	gtk_widget_set_size_request(navbar.comboboxentry_book, -1,
				    6);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (navbar.comboboxentry_book),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (navbar.comboboxentry_book),
				       renderer, "text", 0, NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM
					 (separatortoolitem), FALSE);

	store = gtk_list_store_new(1, G_TYPE_STRING);

	navbar.comboboxentry_chapter = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));	//gtk_combo_box_entry_new();
	gtk_widget_show(navbar.comboboxentry_chapter);
	gtk_box_pack_start(GTK_BOX(hbox3),
			   navbar.comboboxentry_chapter, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(navbar.comboboxentry_chapter, 61,
				    -1);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (navbar.comboboxentry_chapter),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (navbar.
					comboboxentry_chapter),
				       renderer, "text", 0, NULL);

	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM
					 (separatortoolitem), FALSE);

	store = gtk_list_store_new(1, G_TYPE_STRING);
	navbar.comboboxentry_verse = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));	//gtk_combo_box_entry_new();
	gtk_widget_show(navbar.comboboxentry_verse);
	gtk_box_pack_start(GTK_BOX(hbox3),
			   navbar.comboboxentry_verse, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(navbar.comboboxentry_verse, 61,
				    -1);


	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (navbar.comboboxentry_verse),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (navbar.comboboxentry_verse),
				       renderer, "text", 0, NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);

	navbar.lookup_entry = gtk_entry_new();
	gtk_widget_show(navbar.lookup_entry);
	gtk_box_pack_start(GTK_BOX(hbox3), navbar.lookup_entry, TRUE,
			   TRUE, 0);

	g_signal_connect(GTK_OBJECT(sync_button),
			 "toggled", G_CALLBACK(sync_toggled), NULL);
	g_signal_connect((gpointer) navbar.comboboxentry_book,
			 "changed",
			 G_CALLBACK(on_comboboxentry4_changed), NULL);
	g_signal_connect((gpointer) navbar.comboboxentry_chapter,
			 "changed",
			 G_CALLBACK(on_comboboxentry5_changed), NULL);
	g_signal_connect((gpointer) navbar.comboboxentry_verse,
			 "changed",
			 G_CALLBACK(on_comboboxentry6_changed), NULL);
	g_signal_connect((gpointer) navbar.lookup_entry, "activate",
			 G_CALLBACK(on_entry_activate), NULL);
	return hbox3;
#else
	return gui_navbar_versekey_parallel_new();
#endif
}

#ifndef USE_GTKMOZEMBED
/******************************************************************************
 * Name
 *  on_text_button_press_event
 *
 * Synopsis
 *   #include ".h"
 *
 *  gboolean on_text_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */
static gboolean on_text_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		shift_key_pressed = TRUE;
		break;
	case 3:
		//gui_popup_pm_text();
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_text_button_release_event
 *
 * Synopsis
 *   #include "_bibletext.h"
 *
 *  gboolean on_text_button_release_event(GtkWidget * widget,
 *					  GdkEventButton * event,
 *					  DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_text_button_release_event(GtkWidget * widget,
					     GdkEventButton * event,
					     gpointer date)
{
	switch (event->button) {
	case 1:
		
		break;
	case 2:
		if (shift_key_pressed) {
			shift_key_pressed = FALSE;
			break;
		}
		break;
	case 3:
		break;
	}
	return FALSE;
}
#endif /* !USE_GTKMOZEMBED */

/******************************************************************************
 * Name
 *   create_parallel_tab
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   GtkWidget *create_parallel_tab(void)
 *
 * Description
 *   
 *   
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *_create_parallel_tab(void)
{
	GtkWidget *toolbar29;
  	GtkWidget *box_parallel_labels;
#ifdef USE_GTKMOZEMBED
	GtkWidget *eventbox;
	GtkWidget *frame;	
#else
	GtkWidget *scrolled_window;
#endif
    
    	//plabels = g_new0(PARA_LABELS,1);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                             FALSE);
	gtk_notebook_set_current_page (GTK_NOTEBOOK(widgets.notebook_bible_parallel), 
					0);
	gtk_notebook_set_show_border (GTK_NOTEBOOK(widgets.notebook_bible_parallel),
                                        TRUE);
    
	parallel_vbox = gtk_vbox_new(FALSE, 0);
	g_signal_connect(GTK_OBJECT(parallel_vbox), "destroy",
			   G_CALLBACK(on_parallel_tab_destroy),
			   NULL);
	gtk_widget_show(parallel_vbox);
	gtk_box_pack_start(GTK_BOX(widgets.page), parallel_vbox, TRUE, TRUE,
			   0);
	toolbar29 = create_nav_toolbar();
	gtk_widget_show(toolbar29);
	gtk_box_pack_start(GTK_BOX(parallel_vbox), toolbar29, FALSE, FALSE, 0);
    	
	box_parallel_labels = gtk_hbox_new (TRUE, 2);
	gtk_widget_show (box_parallel_labels);
	gtk_box_pack_start (GTK_BOX (parallel_vbox), box_parallel_labels, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box_parallel_labels), 2);

	plabels.label_1 = gtk_label_new (" ");
	gtk_widget_show (plabels.label_1);
	gtk_box_pack_start (GTK_BOX (box_parallel_labels), plabels.label_1, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (plabels.label_1), TRUE);

	plabels.label_2 = gtk_label_new (" ");
	gtk_widget_show (plabels.label_2);
	gtk_box_pack_start (GTK_BOX (box_parallel_labels), plabels.label_2, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (plabels.label_2), TRUE);

	plabels.label_3 = gtk_label_new (" ");
	gtk_widget_show (plabels.label_3);
	gtk_box_pack_start (GTK_BOX (box_parallel_labels), plabels.label_3, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (plabels.label_3), TRUE);

	plabels.label_4 = gtk_label_new (" ");
	gtk_widget_show (plabels.label_4);
	gtk_box_pack_start (GTK_BOX (box_parallel_labels), plabels.label_4, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (plabels.label_4), TRUE);

	plabels.label_5 = gtk_label_new (" ");
	gtk_widget_show (plabels.label_5);
	gtk_box_pack_start (GTK_BOX (box_parallel_labels), plabels.label_5, FALSE, FALSE, 0);
	gtk_label_set_use_markup (GTK_LABEL (plabels.label_5), TRUE);


    
	
#ifdef OLD_NAVBAR
	navbar.key = g_strdup(settings.currentverse);
	navbar.module_name = g_strdup(settings.parallel1Module);	   
	main_navbar_fill_book_combo(navbar);
#endif
	
#ifdef USE_GTKMOZEMBED
	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);	
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);	
	gtk_box_pack_start(GTK_BOX(parallel_vbox), frame, TRUE, TRUE, 0);
	
	eventbox = gtk_event_box_new ();
	gtk_widget_show (eventbox);
	gtk_container_add(GTK_CONTAINER(frame), eventbox);
	
	widgets.html_parallel_dialog = GTK_WIDGET(gecko_html_new(NULL, FALSE, PARALLEL_TYPE)); //embed_new(PARALLEL_TYPE); //gtk_moz_embed_new();
	gtk_widget_show(widgets.html_parallel_dialog);					   
	gtk_container_add(GTK_CONTAINER(eventbox),
			  widgets.html_parallel_dialog);
	g_signal_connect((gpointer)widgets.html_parallel_dialog,
			 "popupmenu_requested",
			 G_CALLBACK (_popupmenu_requested_cb),
			 NULL);
#else
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolled_window);	
	gtk_box_pack_start(GTK_BOX(parallel_vbox), scrolled_window, TRUE, TRUE,0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolled_window,
                                             settings.shadow_type);
		
	widgets.html_parallel_dialog = gtk_html_new();
	gtk_widget_show(widgets.html_parallel_dialog);
	gtk_html_load_empty(GTK_HTML(widgets.html_parallel_dialog));
	gtk_container_add(GTK_CONTAINER(scrolled_window),
			  widgets.html_parallel_dialog);
			  
	g_signal_connect(GTK_OBJECT(widgets.html_parallel_dialog), "link_clicked",
				G_CALLBACK(gui_link_clicked),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_parallel_dialog), "on_url",
				G_CALLBACK(gui_url),
				GINT_TO_POINTER(TEXT_TYPE));		    
	g_signal_connect(GTK_OBJECT(widgets.html_parallel_dialog),"button_release_event",
				G_CALLBACK(on_text_button_release_event),
				NULL);
	g_signal_connect(GTK_OBJECT(widgets.html_parallel_dialog), "button_press_event",
				G_CALLBACK(on_text_button_press_event),
				NULL);
	
	g_signal_connect(GTK_OBJECT(widgets.html_parallel_dialog),
			 "button_release_event",
			 G_CALLBACK (_popupmenu_requested_cb),
			 NULL);
#endif	
	gtk_widget_hide(widgets.hpaned);
	return parallel_vbox;
	
}


GtkWidget *gui_create_parallel_tab(void)
{
	settings.dockedInt = FALSE;	
	return _create_parallel_tab();
}


void gui_destroy_parallel_tab(void)
{
	settings.dockedInt = TRUE;
	gtk_widget_destroy(parallel_vbox);
	gtk_widget_show(widgets.hpaned);
}

/******   end of file   ******/
