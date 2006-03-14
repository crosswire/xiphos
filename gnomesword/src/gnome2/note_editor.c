/*
 * GnomeSword Bible Study Tool
 * editor_menu.c - popup menu for the html editor
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
#include <gtkhtml/gtkhtml.h>
/*#include <gtkhtml/gtkhtmlfontstyle.h>
#include <gtkhtml/htmlform.h>
#include <gtkhtml/htmlenums.h>
#include <gtkhtml/htmlsettings.h>
#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>*/
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "editor/editor-control.h"


#include "gui/note_editor.h"
#include "gui/fileselection.h"
#include "gui/studypad.h"
#include "gui/html.h"
#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "gui/dialog.h"
#include "gui/find_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/lists.h"
#include "main/xml.h"
#include "main/module_dialogs.h"



extern gboolean dialog_freed;
extern gboolean do_display;

static GtkWidget *sync_button;
static GtkWidget *create_menu(DIALOG_DATA * d);



/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_dialog_destroy(GtkObject * object, 
 *						DIALOG_DATA * vc)	
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




static gboolean html_key_press_event(GtkWidget * widget,
				      GdkEventKey * event,
				      DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) d->editor;
	//cur_vc = vc;
	ec->changed = TRUE;
	//g_warning("html_key_press_event");
	gui_update_statusbar(ec);
#endif
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

static gboolean entry_key_press_event(GtkWidget * widget,
				      GdkEventKey * event,
				      DIALOG_DATA * c)
{
	/* if <enter> key */
#ifndef USE_GTKHTML38
	if (event->keyval == 65293 || event->keyval == 65421) {
		GSHTMLEditorControlData *ec 
				= (GSHTMLEditorControlData*) c->editor;
		const gchar *buf = gtk_entry_get_text(GTK_ENTRY(widget));
		gchar *url = g_strdup_printf("sword:///%s", buf);
		main_dialogs_url_handler(c, url, TRUE);
		g_free(url);
		
		if(!ec)
			return;
		if(ec->key)
			g_free(ec->key);
		ec->key = g_strdup_printf("%s", buf); 
	}
#endif

	return FALSE;
}


/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_with_main(DIALOG_DATA * vc)	
 *
 * Description
 *   bring the the View Commentay Dialog module into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(DIALOG_DATA * c)
{
	gchar *url = g_strdup_printf("sword:///%s", settings.currentverse);
	main_dialogs_url_handler(c, url, TRUE);
	g_free(url);
}



/******************************************************************************
 * Name
 *   sync_toggled
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_toggled(GtkToggleButton * button, DIALOG_DATA * vc)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void sync_toggled(GtkToggleButton * button, DIALOG_DATA * d)
{
	if(button->active) {
		sync_with_main(d);
		d->sync = TRUE;
	}
	else		
		d->sync = FALSE;
}



static void on_comboboxentry4_changed(GtkComboBox * combobox, DIALOG_DATA * c)
{
#ifndef USE_GTKHTML38
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
	GSHTMLEditorControlData *ec
	    = (GSHTMLEditorControlData *) c->editor;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);

	if (!do_display)
		return;
#ifdef DEBUG
	g_message("on_comboboxentry4_changed");
#endif
	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &book, -1);

	url = g_strdup_printf("sword:///%s 1:1", book);
	buf = g_strdup_printf("%s 1:1", book);

	main_dialogs_url_handler(c, url, TRUE);
	main_navbar_set(c->navbar, buf);
	g_free(url);
	g_free(book);
	if (ec) {
		if (ec->key)
			g_free(ec->key);
		ec->key = g_strdup_printf("%s", buf);
	}
	g_free(buf);
#endif
}


static void on_comboboxentry5_changed(GtkComboBox * combobox, DIALOG_DATA * c)
{
#ifndef USE_GTKHTML38
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *chapter = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;

	GSHTMLEditorControlData *ec
	    = (GSHTMLEditorControlData *) c->editor;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	GtkTreeModel *book_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (c->navbar.comboboxentry_book));


	if (!do_display)
		return;
#ifdef DEBUG
	g_message("on_comboboxentry5_changed");
#endif
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (c->navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   0, &chapter, -1);

	url = g_strdup_printf("sword:///%s %s:1",
			      settings.MainWindowModule, book, chapter);
	buf = g_strdup_printf("%s %s:1", book, chapter);
	main_dialogs_url_handler(c, url, TRUE);
	main_navbar_set(c->navbar, buf);

	g_free(url);
	g_free(book);
	g_free(chapter);
	if (ec) {
		if (ec->key)
			g_free(ec->key);
		ec->key = g_strdup_printf("%s", buf);
	}
	g_free(buf);
#endif
}


static void on_comboboxentry6_changed(GtkComboBox * combobox, DIALOG_DATA * c)
{
#ifndef USE_GTKHTML38
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *chapter = NULL;
	gchar *verse = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;

	GSHTMLEditorControlData *ec
	    = (GSHTMLEditorControlData *) c->editor;
	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	GtkTreeModel *book_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (c->navbar.comboboxentry_book));
	GtkTreeModel *chapter_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (c->navbar.comboboxentry_chapter));


	if (!do_display)
		return;
#ifdef DEBUG
	g_message("on_comboboxentry6_changed");
#endif
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (c->navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (c->navbar.comboboxentry_chapter),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(chapter_model), &iter, 0,
			   &chapter, -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &verse, -1);

	url = g_strdup_printf("sword:///%s %s:%s",
			      book, chapter, verse);
	buf = g_strdup_printf("%s %s:%s", book, chapter, verse);
	main_dialogs_url_handler(c, url, TRUE);
	main_navbar_set(c->navbar, buf);

	g_free(url);
	g_free(book);
	g_free(chapter);
	g_free(verse);
	if (ec) {
		if (ec->key)
			g_free(ec->key);
		ec->key = g_strdup_printf("%s", buf);
	}
	g_free(buf);
#endif
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

static void on_entry_activate(GtkEntry * entry, DIALOG_DATA * c)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData *ec
	    = (GSHTMLEditorControlData *) c->editor;
	const gchar *buf = gtk_entry_get_text(entry);
	if (c->navbar.key)
		g_free(c->navbar.key);
	c->navbar.key = g_strdup(buf);
	gchar *url = g_strdup_printf("sword:///%s", buf);
	main_dialogs_url_handler(c, url, TRUE);
	g_free(url);
	main_navbar_set(c->navbar, c->navbar.key);
	
	if (!ec)
		return;
	if (ec->key)
		g_free(ec->key);
	ec->key = g_strdup_printf("%s", buf);
#endif
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

static GtkWidget *create_nav_toolbar(DIALOG_DATA * c)
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
	c->navbar.comboboxentry_book =
	    gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	gtk_widget_show(c->navbar.comboboxentry_book);
	gtk_box_pack_start(GTK_BOX(hbox3), c->navbar.comboboxentry_book,
			   TRUE, TRUE, 0);
	gtk_widget_set_size_request(c->navbar.comboboxentry_book, -1,
				    6);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (c->navbar.comboboxentry_book),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (c->navbar.comboboxentry_book),
				       renderer, "text", 0, NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);
	gtk_widget_set_size_request(separatortoolitem, 6, -1);
	gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM
					 (separatortoolitem), FALSE);

	store = gtk_list_store_new(1, G_TYPE_STRING);

	c->navbar.comboboxentry_chapter = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));	//gtk_combo_box_entry_new();
	gtk_widget_show(c->navbar.comboboxentry_chapter);
	gtk_box_pack_start(GTK_BOX(hbox3),
			   c->navbar.comboboxentry_chapter, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(c->navbar.comboboxentry_chapter, 61,
				    -1);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (c->navbar.comboboxentry_chapter),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (c->navbar.
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
	c->navbar.comboboxentry_verse = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));	//gtk_combo_box_entry_new();
	gtk_widget_show(c->navbar.comboboxentry_verse);
	gtk_box_pack_start(GTK_BOX(hbox3),
			   c->navbar.comboboxentry_verse, FALSE, TRUE,
			   0);
	gtk_widget_set_size_request(c->navbar.comboboxentry_verse, 61,
				    -1);


	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT
				   (c->navbar.comboboxentry_verse),
				   renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT
				       (c->navbar.comboboxentry_verse),
				       renderer, "text", 0, NULL);


	separatortoolitem = (GtkWidget *) gtk_separator_tool_item_new();
	gtk_widget_show(separatortoolitem);
	gtk_box_pack_start(GTK_BOX(hbox3), separatortoolitem, FALSE,
			   TRUE, 0);

	c->navbar.lookup_entry = gtk_entry_new();
	gtk_widget_show(c->navbar.lookup_entry);
	gtk_box_pack_start(GTK_BOX(hbox3), c->navbar.lookup_entry, TRUE,
			   TRUE, 0);

	g_signal_connect(GTK_OBJECT(sync_button),
			 "toggled", G_CALLBACK(sync_toggled), c);
	g_signal_connect((gpointer) c->navbar.comboboxentry_book,
			 "changed",
			 G_CALLBACK(on_comboboxentry4_changed), c);
	g_signal_connect((gpointer) c->navbar.comboboxentry_chapter,
			 "changed",
			 G_CALLBACK(on_comboboxentry5_changed), c);
	g_signal_connect((gpointer) c->navbar.comboboxentry_verse,
			 "changed",
			 G_CALLBACK(on_comboboxentry6_changed), c);
	g_signal_connect((gpointer) c->navbar.lookup_entry, "activate",
			 G_CALLBACK(on_entry_activate), c);
	return hbox3;
}



void gui_create_note_editor(DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GtkWidget *vbox;
	GtkWidget *vbox_toolbars;
	GtkWidget *toolbar_nav;
	
	d->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(d->dialog), "d->dialog",
			    d->dialog);
	gtk_window_set_title(GTK_WINDOW(d->dialog),
			     main_get_module_description(d->mod_name));
	gtk_window_set_resizable(GTK_WINDOW(d->dialog), TRUE);
	gtk_widget_set_size_request(d->dialog, 600, 380);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(d->dialog), vbox);
	
	vbox_toolbars = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_toolbars);
	gtk_box_pack_start(GTK_BOX(vbox), vbox_toolbars, FALSE, FALSE,
			   0);

	toolbar_nav = create_nav_toolbar(d);
	gtk_widget_show(toolbar_nav);
	gtk_box_pack_start(GTK_BOX(vbox_toolbars), toolbar_nav, FALSE,
			   FALSE, 0);
	
	d->editor = (GSHTMLEditorControlData *) 
				editor_new(vbox,PERCOM,NULL);


	g_signal_connect(GTK_OBJECT(d->dialog), "destroy",
			   G_CALLBACK(on_dialog_destroy), d);
#endif
}

/***** menu stuff *****/

/******************************************************************************
 * Name
 *  on_savenote_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_savenote_activate(GtkMenuItem * menuitem,
 *				     GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    save personal commentary note by calling editor_save_note()
 *    or gbs entry by calling savebookEDITOR()
 *
 * Return value
 *   void
 */

static void on_savenote_activate(GtkMenuItem * menuitem,
				 DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	if (ecd->personal_comments) {
		main_dialog_save_note(d);
		ecd->changed = FALSE;
		gui_update_statusbar(ecd);
	}
#endif
}

/******************************************************************************
 * Name
 *  on_exportnote_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_exportnote_activate(GtkMenuItem * menuitem,
 *				     GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    export personal commentary note by calling ()
 *    
 *
 * Return value
 *   void
 */

static void on_exportnote_activate(GtkMenuItem * menuitem,
				 DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	if (ecd->personal_comments) {
		if(ecd->changed) {
			main_dialog_save_note(d);
			ecd->changed = FALSE;
			gui_update_statusbar(ecd);
		}
		gui_fileselection_save(ecd,TRUE);
	}
#endif
}


/******************************************************************************
 * Name
 *  on_deletenote_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_deletenote_activate(GtkMenuItem * menuitem,
 *				       GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    delete personal commentary note by calling delete_percomm_note()
 *
 * Return value
 *   void
 */

static void on_deletenote_activate(GtkMenuItem * menuitem,
				   DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	if (ecd->personal_comments) {
		GS_DIALOG *info;
		gint test;
		GString *str;
		gchar *url = NULL;
		
		str = g_string_new("");

		g_warning(ecd->filename);
		g_warning(ecd->key);
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		g_string_printf(str,"<span weight=\"bold\">%s</span>\n\n%s %s",
			_("Delete Note?"), 
			_("Are you sure you want to delete the note for\n"), 
			ecd->key);
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_gs_dialog(info);
		if (test == GS_YES) { 
			main_dialog_delete_note(d);
			url = g_strdup_printf(	"sword://%s/%s",
						ecd->filename,
						ecd->key);
			//main_dialog_goto_bookmark(url);
			g_free(url);
		}
		settings.percomverse = ecd->key;
		ecd->changed = FALSE;
		gui_update_statusbar(ecd);
		g_free(info);
		g_string_free(str,TRUE);
	}
#endif
}


/******************************************************************************
 * Name
 *  on_save_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_save_activate(GtkMenuItem * menuitem,
				      GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    if filename call save_file() else open save file dialog
 *
 * Return value
 *   void
 */

static void on_save_activate(GtkMenuItem * menuitem,
		      DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	if (ecd->filename) {
		save_file(ecd->filename, ecd);
		return;
	} else {
		gui_fileselection_save(ecd,TRUE);
	}
#endif
}


/******************************************************************************
 * Name
 *  on_export_plain_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_export_plain_activate(GtkMenuItem * menuitem,
				      GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    if filename call save_file_plain_text() else open save file dialog
 *
 * Return value
 *   void
 */

static void on_export_plain_activate(GtkMenuItem * menuitem,
		      DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gui_fileselection_save(ecd,FALSE);
#endif
}


/******************************************************************************
 * Name
 *  on_save_as_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_save_as_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    call create_fileselectionSave() to save file as
 *
 * Return value
 *   void
 */

static void on_save_as_activate(GtkMenuItem * menuitem,
				DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gui_fileselection_save(ecd,TRUE);
#endif
}

/******************************************************************************
 * Name
 *  on_print_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_print_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    print content of editor to printer
 *
 * Return value
 *   void
 */

static void on_print_activate(GtkMenuItem * menuitem,
			      DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gui_html_print(ecd->htmlwidget, FALSE);	/* gs_html.c */
#endif
}

/******************************************************************************
 * Name
 *  on_cut_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_cut_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    cut selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_cut_activate(GtkMenuItem * menuitem,
			    DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gtk_html_cut(ecd->html);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
#endif
}

/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem,
			     DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gtk_html_copy(ecd->html);
#endif
}
 
/******************************************************************************
 * Name
 *  on_paste_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_paste_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    paste contents of clipboard into editor
 *
 * Return value
 *   void
 */

static void on_paste_activate(GtkMenuItem * menuitem,
			      DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gtk_html_paste(ecd->html,FALSE);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
#endif
}

/******************************************************************************
 * Name
 *  on_undo_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_undo_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    undo changes to editor
 *
 * Return value
 *   void
 */

static void on_undo_activate(GtkMenuItem * menuitem,
			     DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gtk_html_undo(ecd->html);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
#endif
}

/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_find_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open find dialog
 *
 * Return value
 *   void
 */

static void on_find_activate(GtkMenuItem * menuitem,
			     DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gui_find_dlg(ecd->htmlwidget, ecd->filename,
		  FALSE, NULL);
	//search(ecd, FALSE, NULL);
#endif
}


/******************************************************************************
 * Name
 *  on_replace_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_replace_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    opens find and replace dialog
 *
 * Return value
 *   void
 */

static void on_replace_activate(GtkMenuItem * menuitem,
				DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	replace(ecd);
#endif
}

/******************************************************************************
 * Name
 *  set_link_to_module
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void set_link_to_module(gchar * linkref, gchar * linkmod,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    set link to module and key
 *
 * Return value
 *   void
 */

static void set_link_to_module(gchar * linkref, gchar * linkmod,
			       DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	const gchar *url;
	const gchar *text;
	gchar *target;
	gchar *url_copy;
	gchar buf[256];
	HTMLEngine *e;
	HTMLObject *new_link;

	e = ecd->html->engine;
	
	if (strlen(linkmod))
		sprintf(buf, "sword://%s/%s", linkmod, linkref);
	else
		sprintf(buf, "sword:///%s", linkref);

	url = buf;
	text = linkref;
	if (url && text && *url && *text) {
		target = strchr(url, '#');
		url_copy =
		    target ? g_strndup(url,
				       target - url) : g_strdup(url);
/*		new_link =
		    html_link_text_new(text,
				       GTK_HTML_FONT_STYLE_DEFAULT,
				       html_colorset_get_color(e->
							settings->
							color_set,
							HTMLLinkColor),
				       url_copy, target);
		html_engine_paste_object(e, new_link,
					 g_utf8_strlen(text, -1));*/
		g_free(url_copy);
	}
#endif
}

/******************************************************************************
 * Name
 *  on_link_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_link_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    opens link dialog
 *
 * Return value
 *   void
 */

static void on_link_activate(GtkMenuItem * menuitem,
			     DIALOG_DATA * d)
{
/*	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gint test;
	GS_DIALOG *info;
	GString *str;
	
	str = g_string_new(NULL);
	g_string_printf(str,"<span weight=\"bold\">%s</span>",
			_("Add reference Link"));

	info = gui_new_dialog();
	//info->stock_icon = GTK_STOCK_ADD;
	info->label_top = str->str;
	info->text1 = g_strdup("");
	info->label1 = N_("Reference: ");
	info->text2 = g_strdup("");
	info->label2 = N_("Module: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	
	// * get selected text *
	
	if (html_engine_is_selection_active(ecd->html->engine)) {
		gchar *buf;
		buf =
		    html_engine_get_selection_string(ecd->html->engine);
		info->text1 = g_strdup(buf);
	}
	info->text2 = g_strdup(xml_get_value("modules", "bible"));//settings.MainWindowModule);
	// *** open dialog to get name for list ***
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		if (strlen(info->text1) > 0) {
			set_link_to_module(info->text1, info->text2,
					   d);
			ecd->changed = TRUE;
			gui_update_statusbar(ecd);
		}
	}
	g_free(info->text1);
	g_free(info->text2);
	g_free(info);
	g_string_free(str,TRUE);
*/
}

/******************************************************************************
 * Name
 *  on_autoscroll_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_autoscroll_activate(GtkMenuItem * menuitem,
				       GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    set personal commentary to follow the Bible text
 *
 * Return value
 *   void
 */
 /*
    static void on_autoscroll_activate(GtkMenuItem * menuitem,
    GSHTMLEditorControlData * ecd)
    {
    settings.notefollow = GTK_CHECK_MENU_ITEM(menuitem)->active;
    }
  */



/******************************************************************************
 * Name
 *  gui_create_editor_popup
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   GtkWidget *gui_create_editor_popup(GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    create popup menu for editor
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_menu(DIALOG_DATA * d)
{
#ifndef USE_GTKHTML38
	GtkWidget *pmEditor;
	guint tmp_key;
	GtkWidget *separator;
	GtkWidget *file_menu;
	GtkWidget *save_note = NULL;
	GtkWidget *export_note = NULL;
	GtkWidget *delete_note = NULL;
	GtkWidget *new = NULL;
	GtkWidget *open = NULL;
	GtkWidget *save = NULL;
	GtkWidget *saveas = NULL;
	GtkWidget *export_plain = NULL;
	GtkWidget *print;
	GtkWidget *edit2_menu;
	GtkWidget *cut;
	GtkWidget *copy;
	GtkWidget *paste;
	GtkWidget *spell;
	GtkWidget *undo;
	GtkWidget *find;
	GtkWidget *replace;
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	
	ecd->editnote = NULL;

	pmEditor = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmEditor), "pmEditor", pmEditor);
	
	
	ecd->file = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->file)->child),
				  _("File"));
	gtk_widget_show(ecd->file);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->file);

	file_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->file), file_menu);

	save_note =
	    gtk_menu_item_new_with_label(_("Save Note"));
	gtk_widget_show(save_note);
	gtk_container_add(GTK_CONTAINER(file_menu), save_note);
	
	export_note =
	    gtk_menu_item_new_with_label(_("Export Note"));
	gtk_widget_show(export_note);
	gtk_container_add(GTK_CONTAINER(file_menu), export_note);

	delete_note =
	    gtk_menu_item_new_with_label(_("Delete Note"));
	gtk_widget_show(delete_note);
	gtk_container_add(GTK_CONTAINER(file_menu),
			  delete_note);

	
	
	new = gtk_menu_item_new_with_label(_("New"));
	gtk_widget_show(new);
	gtk_container_add(GTK_CONTAINER(file_menu), new);

	open = gtk_menu_item_new_with_label(_("Open"));
	gtk_widget_show(open);
	gtk_container_add(GTK_CONTAINER(file_menu), open);

	save = gtk_menu_item_new_with_label(_("Save"));
	gtk_widget_show(save);
	gtk_container_add(GTK_CONTAINER(file_menu), save);


	saveas = gtk_menu_item_new_with_label(_("Save As ..."));
	gtk_widget_show(saveas);
	gtk_container_add(GTK_CONTAINER(file_menu), saveas);
	
	export_plain =
	    gtk_menu_item_new_with_label(_("Export"));
	gtk_widget_show(export_plain);
	gtk_container_add(GTK_CONTAINER(file_menu), export_plain);

	

	print = gtk_menu_item_new_with_label(_("Print"));
	gtk_widget_show(print);
	gtk_container_add(GTK_CONTAINER(file_menu), print);


	ecd->edit2 = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->edit2)->child),
				  _("_Edit"));
	gtk_widget_show(ecd->edit2);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->edit2);

	edit2_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->edit2),
				  edit2_menu);

	cut = gtk_menu_item_new_with_label(_("Cut"));
	gtk_widget_show(cut);
	gtk_container_add(GTK_CONTAINER(edit2_menu), cut);

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(edit2_menu), copy);

	paste = gtk_menu_item_new_with_label(_("Paste"));
	gtk_widget_show(paste);
	gtk_container_add(GTK_CONTAINER(edit2_menu), paste);

	spell = gtk_menu_item_new_with_label(_("Spell Check"));
	gtk_widget_show(spell);
	gtk_container_add(GTK_CONTAINER(edit2_menu), spell);

#ifdef USE_SPELL
	gtk_widget_set_sensitive(spell, 1);
#else
	gtk_widget_set_sensitive(spell, 0);
#endif


	undo = gtk_menu_item_new_with_label(_("Undo"));
	gtk_widget_show(undo);
	gtk_container_add(GTK_CONTAINER(edit2_menu), undo);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(edit2_menu), find);


	replace = gtk_menu_item_new_with_label(_("Replace"));
	gtk_widget_show(replace);
	gtk_container_add(GTK_CONTAINER(edit2_menu), replace);

	ecd->link = gtk_menu_item_new_with_label(_("Link..."));
	gtk_widget_show(ecd->link);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->link);

	
	g_signal_connect(GTK_OBJECT(save_note), "activate",
			   G_CALLBACK
			   (on_savenote_activate), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(export_note), "activate",
			   G_CALLBACK
			   (on_exportnote_activate), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(delete_note), "activate",
			   G_CALLBACK
			   (on_deletenote_activate), (DIALOG_DATA *) d);
			   
			   
	g_signal_connect(GTK_OBJECT(print), "activate",
			   G_CALLBACK(on_print_activate), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(cut), "activate",
			   G_CALLBACK(on_cut_activate), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(copy), "activate",
			   G_CALLBACK(on_copy_activate), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(paste), "activate",
			   G_CALLBACK(on_paste_activate), (DIALOG_DATA *) d);

#ifdef USE_SPELL
/*	g_signal_connect(GTK_OBJECT(spell), "activate",
			   G_CALLBACK(spell_check_cb), (DIALOG_DATA *) d);*/
#endif	/* USE_SPELL */
	g_signal_connect(GTK_OBJECT(undo), "activate",
			   G_CALLBACK(on_undo_activate), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(find), "activate",
			   G_CALLBACK(on_find_activate), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(replace), "activate",
			   G_CALLBACK(on_replace_activate), (DIALOG_DATA *) d);

	g_signal_connect(GTK_OBJECT(ecd->link), "activate",
			   G_CALLBACK(on_link_activate), (DIALOG_DATA *) d);
	return pmEditor;
#endif
}




/******   end of file   ******/
