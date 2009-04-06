/*
 * Xiphos Bible Study Tool
 * commentary_dialog.c - dialog for a commentary module
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
#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif


#include "gui/commentary_dialog.h"
#include "gui/dialog.h"
#include "gui/xiphos.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/navbar_versekey_dialog.h"
#include "gui/sidebar.h"
#include "gui/widgets.h"

#include "main/lists.h"
#include "main/navbar.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/navbar_versekey.h"
#include "main/display.hh"

//static void create_menu(DIALOG_DATA * d, GdkEventButton * event);

extern gboolean dialog_freed;
extern gboolean do_display;
/****************************************************************************************
 * static - global to this file only
 */

DIALOG_DATA *cur_d;
//static gboolean apply_change;
#ifdef OLD_NAVBAR
static GtkWidget *sync_button;
#endif



/******************************************************************************
 * Name
 *   gui_display_commentary_with_struct
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void gui_display_commentary_with_struct(gchar * key)	
 *
 * Description
 *    display new key in current commentary
 *
 * Return value
 *   void
 */

void gui_display_commentary_with_struct(DIALOG_DATA *d,
					gchar *key)
{
	strcpy(settings.comm_key, key);
	if (d->key)
		g_free(d->key);
	d->key = g_strdup(key);
}

/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_dialog_destroy(GtkObject * object, 
 *						DIALOG_DATA * d)	
 *
 * Description
 *   shut down the View Commentay Dialog
 *
 * Return value
 *   void
 */

static void on_dialog_destroy(GtkObject *object,
			      DIALOG_DATA *d)
{
	if (!dialog_freed)
		main_free_on_destroy(d);
	dialog_freed = FALSE;
	settings.percomm_dialog_exist = FALSE;
}


/******************************************************************************
 * Name
 *   gui_close_comm_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_close_comm_dialog(DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_close_comm_dialog(DIALOG_DATA * d)
{
	if (d->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(d->dialog);
	}
}


/******************************************************************************
 * Name
 *   on_dialog_motion_notify_event
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   gboolean on_dialog_motion_notify_event(GtkWidget *widget,
                      GdkEventMotion  *event, DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean on_dialog_motion_notify_event(GtkWidget *widget,
					      GdkEventMotion *event,
					      DIALOG_DATA *d)
{
	cur_d = d;
	return FALSE;
}



#ifndef USE_GTKMOZEMBED
/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML *html,
		       const gchar *url,
		       DIALOG_DATA *d)
{
	cur_d = d;
}

/******************************************************************************
 * Name
 *  html_button_pressed
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
 *					DIALOG_DATA * d)	
 *
 * Description
 *    mouse button pressed in window - used to set cur_d to the current
 *    commentary dialog structure
 *
 * Return value
 *   gint
 */

static gint button_press_event(GtkWidget *html,
			       GdkEventButton *event,
			       DIALOG_DATA *d)
{
	cur_d = d;
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		gui_menu_popup (NULL, d);
		//create_menu(d, event);
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   commentary_prefixable_link
 *
 * Synopsis
 *   #include ".h"
 *
 *   void commentary_prefixable_link(void)	
 *
 * Description
 *    front-end-ish handler for xref clicks, to supply book name to prefix.
 *o
 * Return value
 *   void
 */

void commentary_prefixable_link(GtkHTML *html,
				const gchar *url,
				gpointer data)
{
	gchar buf[32];

	cur_d = data;
	strcpy(buf, cur_d->key);
	*(strrchr(buf, ' ')) = '\0';
	gui_prefixable_link_clicked(html, url, data, buf);
}
#endif

/******************************************************************************
 * Name
 *   on_entry_activate
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void on_entry_activate(GtkEntry * entry, DIALOG_DATA * d)	
 *
 * Description
 *   go to verse in free form entry if user hit <enter>
 *
 * Return value
 *   void
 */
#ifdef OLD_NAVBAR
static void on_entry_activate(GtkEntry *entry,
			      DIALOG_DATA *d)
{
	const gchar *buf = gtk_entry_get_text(entry);
	cur_d = d;
	if (d->navbar.key)
		g_free(d->navbar.key);
	d->navbar.key = g_strdup(buf);
	gchar *url = g_strdup_printf("sword:///%s", buf);
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
	main_navbar_set(d->navbar, d->navbar.key);
}
#endif


/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_with_main(DIALOG_DATA * d)	
 *
 * Descriptiond->navbar.lookup_entry
 *   bring the the View Commentay Dialog module into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(DIALOG_DATA *d)
{
	gchar *url =
	    g_strdup_printf("sword:///%s", settings.currentverse);
	main_dialogs_url_handler(d, url, TRUE);
	cur_d = d;
	g_free(url);
}


/******************************************************************************
 * Name
 *   sync_toggled
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_toggled(GtkToggleButton * button, DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

#ifdef OLD_NAVBAR
static void sync_toggled(GtkToggleButton *button,
			 DIALOG_DATA *d)
{
	if (button->active) {
		sync_with_main(d);
		d->sync = TRUE;
	} else
		d->sync = FALSE;
	cur_d = d;
}


static void on_comboboxentry4_changed(GtkComboBox *combobox,
				      DIALOG_DATA *d)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	cur_d = d;
	if (!do_display)
		return;
	GS_message(("on_comboboxentry4_changed"));
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &book, -1);

	url = g_strdup_printf("sword:///%s 1:1", book);
	buf = g_strdup_printf("%s 1:1", book);

	main_dialogs_url_handler(d, url, TRUE);
	main_navbar_set(d->navbar, buf);
	g_free(url);
	g_free(book);
	g_free(buf);
}


static void on_comboboxentry5_changed(GtkComboBox *combobox,
				      DIALOG_DATA *d)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *chapter = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;

	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	GtkTreeModel *book_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (d->navbar.comboboxentry_book));


	cur_d = d;
	if (!do_display)
		return;
	GS_message(("on_comboboxentry5_changed"));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (d->navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   0, &chapter, -1);

	url = g_strdup_printf("sword:///%s %s:1", book, chapter);
	buf = g_strdup_printf("%s %s:1", book, chapter);
	main_dialogs_url_handler(d, url, TRUE);
	main_navbar_set(d->navbar, buf);

	g_free(url);
	g_free(book);
	g_free(chapter);
	g_free(buf);
}


static void on_comboboxentry6_changed(GtkComboBox *combobox,
				      DIALOG_DATA *d)
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
				    (d->navbar.comboboxentry_book));
	GtkTreeModel *chapter_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (d->navbar.comboboxentry_chapter));


	cur_d = d;
	if (!do_display)
		return;
	GS_message(("on_comboboxentry6_changed"));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (d->navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (d->navbar.comboboxentry_chapter),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(chapter_model), &iter, 0,
			   &chapter, -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &verse, -1);

	url = g_strdup_printf("sword:///%s %s:%s",
			      book, chapter, verse);
	buf = g_strdup_printf("%s %s:%s", book, chapter, verse);
	main_dialogs_url_handler(d, url, TRUE);
	main_navbar_set(d->navbar, buf);

	g_free(url);
	g_free(book);
	g_free(chapter);
	g_free(verse);
	g_free(buf);
}
#endif


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

static GtkWidget *create_nav_toolbar(DIALOG_DATA *d)
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
	d->navbar.comboboxentry_book =
	    gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	gtk_widget_show(d->navbar.comboboxentry_book);
	gtk_box_pack_start(GTK_BOX(hbox3), d->navbar.comboboxentry_book,
			   TRUE, TRUE, 0);
	gtk_widget_set_size_request(d->navbar.comboboxentry_book, -1,
				    6);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (d->navbar.comboboxentry_book),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (d->navbar.comboboxentry_book),
				       renderer, "text", 0, NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM
					 (separatortoolitem), FALSE);

	store = gtk_list_store_new(1, G_TYPE_STRING);

	d->navbar.comboboxentry_chapter = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));	//gtk_combo_box_entry_new();
	gtk_widget_show(d->navbar.comboboxentry_chapter);
	gtk_box_pack_start(GTK_BOX(hbox3),
			   d->navbar.comboboxentry_chapter, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(d->navbar.comboboxentry_chapter, 61,
				    -1);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (d->navbar.comboboxentry_chapter),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (d->navbar.
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
	d->navbar.comboboxentry_verse = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));	//gtk_combo_box_entry_new();
	gtk_widget_show(d->navbar.comboboxentry_verse);
	gtk_box_pack_start(GTK_BOX(hbox3),
			   d->navbar.comboboxentry_verse, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(d->navbar.comboboxentry_verse, 61,
				    -1);


	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (d->navbar.comboboxentry_verse),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (d->navbar.comboboxentry_verse),
				       renderer, "text", 0, NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);

	d->navbar.lookup_entry = gtk_entry_new();
	gtk_widget_show(d->navbar.lookup_entry);
	gtk_box_pack_start(GTK_BOX(hbox3), d->navbar.lookup_entry, TRUE,
			   TRUE, 0);

	g_signal_connect(GTK_OBJECT(sync_button),
			 "toggled", G_CALLBACK(sync_toggled), d);
	g_signal_connect((gpointer) d->navbar.comboboxentry_book,
			 "changed",
			 G_CALLBACK(on_comboboxentry4_changed), d);
	g_signal_connect((gpointer) d->navbar.comboboxentry_chapter,
			 "changed",
			 G_CALLBACK(on_comboboxentry5_changed), d);
	g_signal_connect((gpointer) d->navbar.comboboxentry_verse,
			 "changed",
			 G_CALLBACK(on_comboboxentry6_changed), d);
	g_signal_connect((gpointer) d->navbar.lookup_entry, "activate",
			 G_CALLBACK(on_entry_activate), d);
	return hbox3;
#else
	d->navbar.type = NB_DIALOG;
	return gui_navbar_versekey_dialog_new(d);
#endif
}

#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (GeckoHtml *html,
			 gchar *uri,
			 DIALOG_DATA *d)
{	
    	gui_menu_popup (NULL, d);
	//gui_commentary_dialog_create_menu(d); 
}
#endif

/******************************************************************************
 * Name
 *   gui_create_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_create_commentary_dialog(void)	
 *
 * Description
 *   create a Commentary Dialog
 *
 * Return value
 *   void
 */

void gui_create_commentary_dialog(DIALOG_DATA *d,
				  gboolean do_edit)
{
	GtkWidget *vbox30;
	GtkWidget *vbox_toolbars;
	GtkWidget *toolbar_nav;
	GtkWidget *frame19;
#ifndef USE_GTKMOZEMBED
	GtkWidget *scrolledwindow38;
#endif
	
	cur_d = d;
	d->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_object_set_data(GTK_OBJECT(d->dialog), "d->dialog",
			    d->dialog);
	gtk_window_set_title(GTK_WINDOW(d->dialog),
			     main_get_module_description(d->mod_name));
	//gtk_window_set_default_size(GTK_WINDOW(d->dialog), 462, 280);
	gtk_window_set_resizable(GTK_WINDOW(d->dialog), TRUE);
	if (do_edit)
		gtk_widget_set_size_request(d->dialog, 590, 380);
	else
		gtk_widget_set_size_request(d->dialog, 460, 280);

	vbox30 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox30);

	gtk_container_add(GTK_CONTAINER(d->dialog), vbox30);

	vbox_toolbars = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_toolbars);
	gtk_box_pack_start(GTK_BOX(vbox30), vbox_toolbars, FALSE, FALSE,
			   0);

	toolbar_nav = create_nav_toolbar(d);
	gtk_widget_show(toolbar_nav);
	gtk_box_pack_start(GTK_BOX(vbox_toolbars), toolbar_nav, FALSE,
			   FALSE, 0);

	frame19 = gtk_frame_new(NULL);
	gtk_widget_show(frame19);
	gtk_box_pack_start(GTK_BOX(vbox30), frame19, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame19), GTK_SHADOW_IN);

#ifdef USE_GTKMOZEMBED	
	d->html = GTK_WIDGET(gecko_html_new(((DIALOG_DATA*) d),TRUE,DIALOG_COMMENTARY_TYPE));
	gtk_container_add(GTK_CONTAINER(frame19), d->html);
	gtk_widget_show(d->html);	
	g_signal_connect((gpointer)d->html,
		      "popupmenu_requested",
		      G_CALLBACK (_popupmenu_requested_cb),
		      (DIALOG_DATA*)d);
#else
	scrolledwindow38 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow38);
	gtk_container_add(GTK_CONTAINER(frame19), scrolledwindow38);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow38),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow38,
					    settings.shadow_type);
	d->html = gtk_html_new();
	gtk_widget_show(d->html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow38),
			  d->html);
	gtk_html_load_empty(GTK_HTML(d->html));

#ifndef USE_GTKMOZEMBED	
	g_signal_connect(GTK_OBJECT(d->html),
			 "link_clicked",
			 G_CALLBACK(commentary_prefixable_link), d);
#endif	
	
	g_signal_connect(GTK_OBJECT(d->html), "on_url",
			 G_CALLBACK(dialog_url), d);
	g_signal_connect(GTK_OBJECT(d->html),
			 "button_press_event",
			 G_CALLBACK(button_press_event), d);

#endif  /* !USE_GTKMOZEMBED */

	g_signal_connect(GTK_OBJECT(d->dialog), "destroy",
			 G_CALLBACK(on_dialog_destroy), d);
	g_signal_connect(GTK_OBJECT(d->dialog),
			 "motion_notify_event",
			 G_CALLBACK(on_dialog_motion_notify_event), d);
}


void gui_commentary_dialog_sync_toggled(GtkToggleButton * button,
					DIALOG_DATA * d)
{
	if (d == NULL)
		d = cur_d;
	if (button->active) {
		sync_with_main(d);
		d->sync = TRUE;
	} else
		d->sync = FALSE;
}


/******   end of file   ******/
