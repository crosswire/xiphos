/*
 * GnomeSword Bible Study Tool
 * commentary_dialog.c - dialog for a commentary module
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
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#ifdef USE_GTKMOZEMBED
#include <gtkmozembed.h>
#else
#include "gui/html.h"
#endif

#include "gecko/gecko-html.h"

#include "gui/commentary_dialog.h"
#include "gui/dialog.h"
#include "gui/gnomesword.h"
#include "gui/main_window.h"
#include "gui/sidebar.h"
#include "gui/widgets.h"

#include "main/lists.h"
#include "main/navbar.h"
#include "main/settings.h"
#include "main/sword.h"


static void create_menu(DIALOG_DATA * d, GdkEventButton * event);

extern gboolean dialog_freed;
extern gboolean do_display;
/****************************************************************************************
 * static - global to this file only
 */

DIALOG_DATA *cur_d;
static gboolean apply_change;
static GtkWidget *sync_button;



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

void gui_display_commentary_with_struct(DIALOG_DATA * d, gchar * key)
{
	strcpy(settings.comm_key, key);
	if (d->key)
		g_free(d->key);
	d->key = g_strdup(key);
	//display(c, key);
}

/******************************************************************************
 * Name
 *   gui_note_can_close
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   void gui_note_can_close(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
/*
void gui_note_can_close(GSHTMLEditorControlData * ecd)
{
	gchar *filename = NULL;
	gchar *buf = NULL;
	gint test;
	GS_DIALOG *info;
	GString *str;

	if (settings.modifiedPC) {
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		g_string_printf(str,
				"<span weight=\"bold\">%s</span>\n\n%s %s",
				_("Note for"),
				ecd->key,
				_
				("has been modified. Do you wish to save it?"));
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
//                      gui_save_note(ecd);
		}
		settings.modifiedPC = FALSE;
		g_free(info);
		g_string_free(str, TRUE);
	}
}
*/
/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void  gui_on_lookup_commentary_dialog_selection
		(GtkMenuItem * menuitem, gchar * dict_mod_description)	
 *
 * Description
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */
/*
void gui_on_lookup_commentary_dialog_selection
    (GtkMenuItem * menuitem, gchar * dict_mod_description) {
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;

	mod_name =
	    main_module_name_from_description(dict_mod_description);
	if (!mod_name)
		return;

	dict_key = gui_get_word_or_selection(cur_d->html, FALSE);
	if (dict_key && mod_name) {
		main_display_dictionary(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}
}
*/

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

static void on_dialog_destroy(GtkObject * object, DIALOG_DATA * d)
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

static gboolean on_dialog_motion_notify_event(GtkWidget * widget,
					      GdkEventMotion * event,
					      DIALOG_DATA * d)
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

static void dialog_url(GtkHTML * html, const gchar * url,
		       DIALOG_DATA * d)
{
	cur_d = d;
}
#endif

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

static gint button_press_event(GtkWidget * html,
			       GdkEventButton * event, DIALOG_DATA * d)
{
	cur_d = d;
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		create_menu(d, event);
		break;
	}
	return FALSE;
}
static gboolean html_key_press_event(GtkWidget * widget,
				     GdkEventKey * event,
				     DIALOG_DATA * d)
{
	cur_d = d;
	return FALSE;
}



/******************************************************************************
 * Name
 *   book_changed
 *
 * Synopsis
 *   #include ".h"
 *
 *   void book_changed(GtkEditable * editable, gpointer user_data)	
 *
 * Description
 *   change book 
 *
 * Return value
 *   void
 */

static void book_changed(GtkEditable * editable, DIALOG_DATA * d)
{
	gchar *url;
	gchar *bookname = gtk_editable_get_chars(editable, 0, -1);

	cur_d = d;
	if (*bookname) {
		url = g_strdup_printf("sword:///%s 1:1", bookname);
		main_dialogs_url_handler(d, url, TRUE);
		g_free(url);
	}
}


/******************************************************************************
 * Name
 *   chapter_button_release_event
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   gboolean chapter_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event,   gpointer user_data)	
 *
 * Description
 *    change chapter 
 *
 * Return value
 *   gboolean
 */

static gboolean chapter_button_release_event(GtkWidget * widget,
					     GdkEventButton * event,
					     DIALOG_DATA * d)
{
	G_CONST_RETURN gchar *bookname;
	gchar *url;
	gchar *val_key;
	gint chapter;
	cur_d = d;
	bookname = (gchar *) gtk_entry_get_text(GTK_ENTRY(d->cbe_book));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (d->spb_chapter));
	url = g_strdup_printf("sword:///%s %d:1", bookname, chapter);
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);

	return FALSE;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   gboolean (GtkWidget * widget, 
 *			GdkEventButton * event,  gpointer user_data)	
 *
 * Description
 *    change verse
 *
 * Return value
 *   gboolean
 */

static gboolean verse_button_release_event(GtkWidget * widget,
					   GdkEventButton * event,
					   DIALOG_DATA * d)
{
	G_CONST_RETURN gchar *bookname;
	gchar *url;
	gchar *val_key;
	gint chapter, verse;
	cur_d = d;
	bookname = (gchar *) gtk_entry_get_text(GTK_ENTRY(d->cbe_book));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (d->spb_chapter));
	verse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (d->spb_verse));
	url =
	    g_strdup_printf("sword:///%s %d:%d", bookname, chapter,
			    verse);

	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);

	return FALSE;
}


/******************************************************************************
 * Name
 *   entry_key_press_event
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   gboolean entry_key_press_event(GtkWidget * widget,
 *				GdkEventKey * event, gpointer user_data)	
 *
 * Description
 *   go to verse in free form entry if user hit <enter>
 *
 * Return value
 *   gboolean
 */
/*
static gboolean entry_key_press_event(GtkWidget * widget,
				      GdkEventKey * event,
				      DIALOG_DATA * d)
{
	// if <enter> key //
	if (evend->keyval == 65293 || evend->keyval == 65421) {
#ifndef USE_GTKHTML38
		GSHTMLEditorControlData *ec
		    = (GSHTMLEditorControlData *) d->editor;
#endif
		const gchar *buf =
		    gtk_entry_get_text(GTK_ENTRY(widget));
		gchar *url = g_strdup_printf("sword:///%s", buf);
		main_dialogs_url_handler(c, url, TRUE);
		g_free(url);
		main_navbar_set(d->navbar, d->navbar.key);

#ifndef USE_GTKHTML38
		if (!ec)
			return;
		if (ec->key)
			g_free(ec->key);
		ec->key = g_strdup_printf("%s", buf);
#endif
	}
	return FALSE;
}
*/

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

static void on_entry_activate(GtkEntry * entry, DIALOG_DATA * d)
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

static void sync_with_main(DIALOG_DATA * d)
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

static void sync_toggled(GtkToggleButton * button, DIALOG_DATA * d)
{
	if (button->active) {
		sync_with_main(d);
		d->sync = TRUE;
	} else
		d->sync = FALSE;
	cur_d = d;
}

static void on_comboboxentry4_changed(GtkComboBox * combobox, DIALOG_DATA * d)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	cur_d = d;
	if (!do_display)
		return;
#ifdef DEBUG
	g_message("on_comboboxentry4_changed");
#endif
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


static void on_comboboxentry5_changed(GtkComboBox * combobox, DIALOG_DATA * d)
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
#ifdef DEBUG
	g_message("on_comboboxentry5_changed");
#endif
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


static void on_comboboxentry6_changed(GtkComboBox * combobox, DIALOG_DATA * d)
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
#ifdef DEBUG
	g_message("on_comboboxentry6_changed");
#endif
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
 *
 * Return value
 *   void
 */

void commentary_prefixable_link(GtkHTML * html,
				const gchar * url,
				gpointer data)
{
	gchar buf[32];

	cur_d = data;
	strcpy(buf, cur_d->key);
	*(strrchr(buf, ' ')) = '\0';
	gui_prefixable_link_clicked(html, url, data, buf);
}

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

static GtkWidget *create_nav_toolbar(DIALOG_DATA * d)
{
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
}


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

void gui_create_commentary_dialog(DIALOG_DATA * d, gboolean do_edit)
{
	GtkWidget *vbox30;
	GtkWidget *vbox_toolbars;
	GtkWidget *toolbar_nav;
	GtkWidget *frame19;
	GtkWidget *scrolledwindow38;
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData *ec =
	    (GSHTMLEditorControlData *) d->editor;
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
/*	d->html = embed_dialogs_new((DIALOG_DATA*) d);
	gtk_container_add(GTK_CONTAINER(frame19), d->html);
	gtk_widget_show(d->html);*/
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


	if (do_edit) {
#ifndef USE_GTKHTML38
		ec->htmlwidget = gtk_html_new();
		ec->html = GTK_HTML(ec->htmlwidget);
		gtk_widget_show(ec->htmlwidget);
		gtk_container_add(GTK_CONTAINER(scrolledwindow38),
				  ec->htmlwidget);

		ec->vbox = vbox30;

		ec->pm = gui_create_editor_popup(ec);
		gnome_popup_menu_attach(ec->pm, ec->htmlwidget, NULL);

		ec->statusbar = gtk_statusbar_new();
		gtk_widget_show(ec->statusbar);
		gtk_box_pack_start(GTK_BOX(vbox30), ec->statusbar,
				   FALSE, TRUE, 0);
		
		/* html.c */
		g_signal_connect(G_OBJECT(ec->htmlwidget),
				 "key_press_event",
				 G_CALLBACK(html_key_press_event), d);
		g_signal_connect(GTK_OBJECT(ec->htmlwidget),
				 "link_clicked",
				 G_CALLBACK(commentary_prefixable_link), NULL);

		g_signal_connect(GTK_OBJECT(ec->htmlwidget),
				 "on_url", G_CALLBACK(gui_url), NULL);

		gui_toolbar_style(ec);
		gtk_box_pack_start(GTK_BOX(vbox_toolbars),
				   ec->toolbar_style, FALSE, FALSE, 0);
		gtk_widget_show(ec->toolbar_style);
		gui_toolbar_edit(ec);
		gtk_box_pack_start(GTK_BOX(vbox_toolbars),
				   ec->toolbar_edit, FALSE, FALSE, 0);
		gtk_widget_show(ec->toolbar_edit);
#endif  /* USE_GTKHTML38 */
	} else {
		d->html = gtk_html_new();
		gtk_widget_show(d->html);
		gtk_container_add(GTK_CONTAINER(scrolledwindow38),
				  d->html);
		gtk_html_load_empty(GTK_HTML(d->html));

		g_signal_connect(GTK_OBJECT(d->html),
				 "link_clicked",
				 G_CALLBACK(commentary_prefixable_link), d);
		g_signal_connect(GTK_OBJECT(d->html), "on_url",
				 G_CALLBACK(dialog_url), d);
		g_signal_connect(GTK_OBJECT(d->html),
				 "button_press_event",
				 G_CALLBACK(button_press_event), d);
	}

#endif  /* USE_GTKMOZEMBED */

	g_signal_connect(GTK_OBJECT(d->dialog), "destroy",
			 G_CALLBACK(on_dialog_destroy), d);
	g_signal_connect(GTK_OBJECT(d->dialog),
			 "motion_notify_event",
			 G_CALLBACK(on_dialog_motion_notify_event), d);
}


/** Commentary menu stuff
 **
 **
 **
 **
 **
 **
 **/

static void on_about_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
	gui_display_about_module_dialog(cur_d->mod_name, FALSE);
}

static void on_print1_activate(GtkMenuItem * menuitem,
			       gpointer user_data)
{
#ifdef USE_GTKMOZEMBED
//	embed_print(TRUE, GTK_MOZ_EMBED(cur_d->html));
#else
	 gui_html_print(cur_d->html, FALSE);
#endif
}


static void on_copy2_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
#ifdef USE_GTKMOZEMBED
	gecko_html_copy_selection(GECKO_HTML(cur_d->html));
	//embed_copy_selection(GTK_MOZ_EMBED(cur_d->html));
#else
	gui_copy_html(cur_d->html);
#endif
}


static void on_find1_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{
#ifdef USE_GTKMOZEMBED
	
#else
	gui_find_dlg(cur_d->html, cur_d->mod_name, FALSE, NULL);
#endif
}


static void
on_set_module_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *url = g_strdup_printf("sword://%s/%s",
				     cur_d->mod_name,
				     settings.currentverse);
	gui_set_module_font(cur_d->mod_name);
	main_dialogs_url_handler(cur_d, url, TRUE);
	g_free(url);
}


static void on_use_current_dictionary_activate(GtkMenuItem * menuitem,
					       gpointer user_data)
{
	gchar *dict_key = NULL;
#ifdef USE_GTKMOZEMBED	
	gecko_html_copy_selection(GECKO_HTML(cur_d->html));
	//embed_copy_selection(GTK_MOZ_EMBED(cur_d->html));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);
#else
	dict_key = gui_get_word_or_selection(cur_d->html, FALSE);
	if (dict_key) {
		main_display_dictionary(settings.
						DictWindowModule,
						dict_key);
		g_free(dict_key);
	}
#endif
}


static void
on_unlock_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	gchar *cipher_key;
	gchar *cipher_old;

	cipher_old = get_cipher_key(settings.MainWindowModule);
	cipher_key = gui_add_cipher_key(settings.MainWindowModule, cipher_old);
	
*/
}


static void
on_all_readings_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_primary_reading_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_secondary_reading_activate(GtkMenuItem * menuitem,
			      gpointer user_data)
{

}


/******************************************************************************
 * Name
 *  gui_lookup_bibletext_selection
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 * void gui_lookup_bibletext_selection(GtkMenuItem * menuitem,
					 gchar * dict_mod_description)
 *
 * Description
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */

static void lookup_commentary_selection(GtkMenuItem * menuitem,
					gchar * dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;

	mod_name =
	    main_module_name_from_description(dict_mod_description);
#ifdef USE_GTKMOZEMBED	
	gecko_html_copy_selection(GECKO_HTML(cur_d->html));
	//embed_copy_selection(GTK_MOZ_EMBED(cur_d->html));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);
	dict_key = 
		g_strdup(gtk_editable_get_chars(
			(GtkEditable *)widgets.entry_dict,0,-1));

#else
	dict_key = gui_get_word_or_selection(cur_d->html, FALSE);
#endif
	if (dict_key && mod_name) {
		main_display_dictionary(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}
}

/******************************************************************************
 * Name
 *  on_view_mod_activate
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   void on_view_mod_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   show a different text module by changing the notebook page
 *
 * Return value
 *   void
 */

static void on_view_mod_activate(GtkMenuItem * menuitem,
				 gpointer user_data)
{
	gchar *module_name =
	    main_module_name_from_description((gchar *) user_data);
	gchar *url = NULL;
	if (module_name) {
		url = g_strdup_printf("sword://%s/%s",
				      module_name, cur_d->key);
		main_dialogs_url_handler(cur_d, url, TRUE);
		g_free(url);
		g_free(module_name);
	}
}

/******************************************************************************
 * Name
 *  global_option_red_words
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_global_option(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_red_words(GtkCheckMenuItem * menuitem,
				    DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->words_in_red = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_strongs
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_strongs(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_strongs(GtkCheckMenuItem * menuitem,
				  DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->strongs = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_morphs
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_morphs(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_morphs(GtkCheckMenuItem * menuitem,
				 DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->morphs = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_footnotes
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_footnotes(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_footnotes(GtkCheckMenuItem * menuitem,
				    DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->footnotes = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_greekaccents
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_greekaccents(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_greekaccents(GtkCheckMenuItem * menuitem,
				       DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->greekaccents = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_lemmas
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_lemmas(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_lemmas(GtkCheckMenuItem * menuitem,
				 DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->lemmas = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_scripturerefs
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_scripturerefs(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_scripturerefs(GtkCheckMenuItem * menuitem,
					DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->scripturerefs = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_hebrewpoints
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_hebrewpoints(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_hebrewpoints(GtkCheckMenuItem * menuitem,
				       DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->hebrewpoints = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_hebrewcant
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_hebrewcant(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_hebrewcant(GtkCheckMenuItem * menuitem,
				     DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->hebrewcant = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  global_option_headings
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_headings(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void global_option_headings(GtkCheckMenuItem * menuitem,
				   DIALOG_DATA * d)
{
	gchar *url =
	    g_strdup_printf("sword://%s/%s", d->mod_name, d->key);
	d->ops->headings = menuitem->active;
	main_dialogs_url_handler(d, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
 *
 *   void (GtkMenuItem * menuitem, DIALOG_DATA * d)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_close_activate(GtkMenuItem * menuitem, DIALOG_DATA * d)
{
	//close_text_dialog(cur_d);
}


/******************************************************************************
 * Name
 *  on_sync_activate
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
 *
 *  void on_sync_activate(GtkMenuItem * menuitem, 
						DIALOG_DATA * vt)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_sync_activate(GtkMenuItem * menuitem, gpointer data)
{
	sync_with_main(cur_d);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
 *
 *  void (GtkMenuItem * menuitem, 
						DIALOG_DATA * vt)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_book_heading_activate(GtkMenuItem * menuitem, gpointer data)
{
	main_dialogs_book_heading(cur_d);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
 *
 *  void (GtkMenuItem * menuitem, 
						DIALOG_DATA * vt)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_chapter_heading_activate(GtkMenuItem * menuitem, gpointer data)
{
	main_dialogs_chapter_heading(cur_d);
}


static void stay_in_sync_activate(GtkCheckMenuItem * menuitem,
				  gpointer data)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sync_button),
				     menuitem->active);
}


static GnomeUIInfo view_text_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("item1"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-book-green",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo file3_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_SUBTREE, N_("Open Module"),
	 NULL,
	 view_text_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Sync with main"),
	 NULL,
	 (gpointer) on_sync_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-refresh",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Stay in sync"),
	 NULL,
	 (gpointer) stay_in_sync_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-refresh",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_PRINT_ITEM(on_print1_activate, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CLOSE_ITEM(on_close_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo note_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("item2"),
	 NULL,
	 NULL, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo edit3_menu_uiinfo[] = {
	GNOMEUIINFO_MENU_COPY_ITEM(on_copy2_activate, NULL),
	GNOMEUIINFO_MENU_FIND_ITEM(on_find1_activate, NULL),
	{
	 GNOME_APP_UI_SUBTREE, N_("Note"),
	 NULL,
	 note_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-dnd",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo all_readings_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("All Readings"),
	 NULL,
	 (gpointer) on_all_readings_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Primary Reading"),
	 NULL,
	 (gpointer) on_primary_reading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Secondary Reading"),
	 NULL,
	 (gpointer) on_secondary_reading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo variants_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_RADIOITEMS, NULL, NULL, all_readings_uiinfo,
	 NULL, NULL, GNOME_APP_PIXMAP_NONE, NULL, 0,
	 (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo module_options_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Set Module Font"),
	 NULL,
	 (gpointer) on_set_module_font_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-select-font",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Words of Christ in Red"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Words of Christ in Red",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM,
	 N_("Strong's Numbers"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Strong's Numbers",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE,
	 NULL,
	 0,
	 (GdkModifierType) 0,
	 NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Morphological Tags"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Morphological Tags",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Footnotes"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Footnotes",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Greek Accents"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Greek Accents",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Lemmas"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Lemmas",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Scripture Cross-references"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Scripture Cross-references",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Vowel Points"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Hebrew Vowel Points",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Cantillation"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Hebrew Cantillation",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Headings"),
	 NULL,
	 NULL,			//(gpointer) on_global_option,
	 (gpointer) "Headings",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Variants"),
	 NULL,
	 variants_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static GnomeUIInfo lookup_selection_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Use Current Dictionary"),
	 NULL,
	 (gpointer) on_use_current_dictionary_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-find",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_END
};

static GnomeUIInfo menu1_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("About"),
	 NULL,
	 (gpointer) on_about_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-about",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	{
	 GNOME_APP_UI_SUBTREE, N_("File"),
	 NULL,
	 file3_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-open",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Edit"),
	 NULL,
	 edit3_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-dnd",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("_Module Options"),
	 NULL,
	 module_options_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-ok",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_SUBTREE, N_("Lookup Selection"),
	 NULL,
	 lookup_selection_menu_uiinfo, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-find",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Unlock This Module"),
	 NULL,
	 (gpointer) on_unlock_module_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-authentication",
	 0, (GdkModifierType) 0, NULL},
        {
	 GNOME_APP_UI_ITEM, N_("Display Book Heading"),
	 NULL,
	 (gpointer) on_book_heading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-open",
	 0, (GdkModifierType) 0, NULL},
        {
	 GNOME_APP_UI_ITEM, N_("Display Chapter Heading"),
	 NULL,
	 (gpointer) on_chapter_heading_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-open",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static void create_menu(DIALOG_DATA * d, GdkEventButton * event)
{
	GtkWidget *menu1;
	GtkWidget *lookup_selection_menu;
	GtkWidget *usecurrent;
	GtkWidget *view_menu;
	GtkWidget *separator;
	GtkWidget *edit_per_menu;
	GnomeUIInfo *menuitem;
	gchar *mod_name = d->mod_name;

	cur_d = d;
	menu1 = gtk_menu_new();
	gnome_app_fill_menu(GTK_MENU_SHELL(menu1), menu1_uiinfo,
			    NULL, FALSE, 0);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (all_readings_uiinfo[0].widget),
				       TRUE);
	gtk_widget_hide(module_options_menu_uiinfo[2].widget);	//"words_in_red"
	gtk_widget_hide(module_options_menu_uiinfo[3].widget);	//"strongs_numbers"
	gtk_widget_hide(module_options_menu_uiinfo[4].widget);	//"/morph_tags"
	gtk_widget_hide(module_options_menu_uiinfo[5].widget);	//"footnotes"
	gtk_widget_hide(module_options_menu_uiinfo[6].widget);	// "greek_accents"
	gtk_widget_hide(module_options_menu_uiinfo[7].widget);	//"lemmas"
	gtk_widget_hide(module_options_menu_uiinfo[8].widget);	//"cross_references"
	gtk_widget_hide(module_options_menu_uiinfo[9].widget);	//"hebrew_vowel_points"
	gtk_widget_hide(module_options_menu_uiinfo[10].widget);	//"hebrew_cantillation"
	gtk_widget_hide(module_options_menu_uiinfo[11].widget);	//"headings"
	gtk_widget_hide(module_options_menu_uiinfo[12].widget);	//"variants"
	gtk_widget_hide(menu1_uiinfo[6].widget);	//"unlock_module"
	gtk_widget_hide(edit3_menu_uiinfo[2].widget);

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (file3_menu_uiinfo[2].widget),
				       d->sync);


	view_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM
				  (file3_menu_uiinfo[0].widget),
				  view_menu);

	gui_add_mods_2_gtk_menu(COMM_DESC_LIST, view_menu,
				(GCallback) on_view_mod_activate);

	edit_per_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM
				  (edit3_menu_uiinfo[2].widget),
				  edit_per_menu);


	lookup_selection_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu1_uiinfo[5].widget),
				  lookup_selection_menu);

	usecurrent =
	    gtk_menu_item_new_with_label(_("Use Current Dictionary"));
	gtk_widget_show(usecurrent);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  usecurrent);

	g_signal_connect(GTK_OBJECT(usecurrent),
			 "activate",
			 G_CALLBACK(on_use_current_dictionary_activate),
			 NULL);

	separator = gtk_menu_item_new();
	gtk_widget_show(separator);
	gtk_container_add(GTK_CONTAINER(lookup_selection_menu),
			  separator);
	gtk_widget_set_sensitive(separator, FALSE);

	gui_add_mods_2_gtk_menu(DICT_DESC_LIST, lookup_selection_menu,
				(GCallback)
				lookup_commentary_selection);


	if ((main_check_for_global_option(mod_name,
					  "GBFRedLetterWords")) ||
	    (main_check_for_global_option(mod_name,
					  "OSISRedLetterWords"))) {
		gtk_widget_show(module_options_menu_uiinfo[2].widget);	//"words_in_red");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[2].
				    widget)->active =
		    d->ops->words_in_red;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[2].widget),
				 "activate",
				 G_CALLBACK(global_option_red_words),
				 (DIALOG_DATA *) d);
	}
	if ((main_check_for_global_option(mod_name, "GBFStrongs"))
	    || (main_check_for_global_option(mod_name, "ThMLStrongs"))
	    || (main_check_for_global_option(mod_name, "OSISStrongs"))) {
		gtk_widget_show(module_options_menu_uiinfo[3].widget);	//"strongs_numbers");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[3].
				    widget)->active = d->ops->strongs;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[3].widget),
				 "activate",
				 G_CALLBACK(global_option_strongs),
				 (DIALOG_DATA *) d);
	}
	if ((main_check_for_global_option(mod_name, "GBFMorph")) ||
	    (main_check_for_global_option(mod_name, "ThMLMorph")) ||
	    (main_check_for_global_option(mod_name, "OSISMorph"))) {
		gtk_widget_show(module_options_menu_uiinfo[4].widget);	//"/morph_tags");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[4].
				    widget)->active = d->ops->morphs;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[4].widget),
				 "activate",
				 G_CALLBACK(global_option_morphs),
				 (DIALOG_DATA *) d);
	}
	if ((main_check_for_global_option(mod_name, "GBFFootnotes")) ||
	    (main_check_for_global_option(mod_name, "ThMLFootnotes")) ||
	    (main_check_for_global_option(mod_name, "OSISFootnotes"))) {
		gtk_widget_show(module_options_menu_uiinfo[5].widget);	//"footnotes");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[5].
				    widget)->active = d->ops->footnotes;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[5].widget),
				 "activate",
				 G_CALLBACK(global_option_footnotes),
				 (DIALOG_DATA *) d);
	}
	if (main_check_for_global_option(mod_name, "UTF8GreekAccents")) {
		gtk_widget_show(module_options_menu_uiinfo[6].widget);	// "greek_accents");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[6].
				    widget)->active =
		    d->ops->greekaccents;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[6].widget),
				 "activate",
				 G_CALLBACK(global_option_greekaccents),
				 (DIALOG_DATA *) d);
	}
	if (main_check_for_global_option(mod_name, "ThMLLemma")) {
		gtk_widget_show(module_options_menu_uiinfo[7].widget);	//"lemmas");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[7].
				    widget)->active = d->ops->lemmas;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[7].widget),
				 "activate",
				 G_CALLBACK(global_option_lemmas),
				 (DIALOG_DATA *) d);
	}
	if (main_check_for_global_option(mod_name, "ThMLScripref") ||
	    (main_check_for_global_option(mod_name, "OSISScripref"))) {
		gtk_widget_show(module_options_menu_uiinfo[8].widget);	//"cross_references");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[8].
				    widget)->active =
		    d->ops->scripturerefs;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[8].widget),
				 "activate",
				 G_CALLBACK
				 (global_option_scripturerefs),
				 (DIALOG_DATA *) d);
	}
	if (main_check_for_global_option(mod_name, "UTF8HebrewPoints")) {
		gtk_widget_show(module_options_menu_uiinfo[9].widget);	//"hebrew_vowel_points");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[9].
				    widget)->active =
		    d->ops->hebrewpoints;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[9].widget),
				 "activate",
				 G_CALLBACK(global_option_hebrewpoints),
				 (DIALOG_DATA *) d);
	}
	if (main_check_for_global_option(mod_name, "UTF8Cantillation")) {
		gtk_widget_show(module_options_menu_uiinfo[10].widget);	//"hebrew_cantillation");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[10].
				    widget)->active =
		    d->ops->hebrewcant;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[10].
				  widget), "activate",
				 G_CALLBACK(global_option_hebrewcant),
				 (DIALOG_DATA *) d);
	}
	if (main_check_for_global_option(mod_name, "ThMLHeadings") ||
	    (main_check_for_global_option(mod_name, "OSISHeadings"))) {
		gtk_widget_show(module_options_menu_uiinfo[11].widget);	//"headings");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[11].
				    widget)->active = d->ops->headings;

		g_signal_connect(GTK_OBJECT
				 (module_options_menu_uiinfo[11].
				  widget), "activate",
				 G_CALLBACK(global_option_headings),
				 (DIALOG_DATA *) d);
	}
	if (main_check_for_global_option(mod_name, "ThMLVariants")) {
		gtk_widget_show(module_options_menu_uiinfo[12].widget);	//"variants");

		gtk_widget_show(all_readings_uiinfo[0].widget);	//"all_readings");

		gtk_widget_show(all_readings_uiinfo[1].widget);	//"primary_reading");

		gtk_widget_show(all_readings_uiinfo[2].widget);	//"secondary_reading");

	}
	if (main_has_cipher_tag(mod_name))
		gtk_widget_show(menu1_uiinfo[6].widget);


	gtk_menu_popup((GtkMenu*)menu1, NULL, NULL, NULL, NULL, 2,
		     			gtk_get_current_event_time());
	/*gnome_popup_menu_do_popup_modal(menu1, NULL,
					NULL, event, NULL, d->html);
	gtk_widget_destroy(menu1);*/
	//g_free(ops);
}

void gui_commentary_dialog_create_menu(DIALOG_DATA * d)
{
	create_menu(d, NULL);
	
}



/******   end of file   ******/
