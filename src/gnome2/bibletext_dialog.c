/*
 * Xiphos Bible Study Tool
 * bibletext_dialog.c - view Bible text module in a dialog
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
#include <config.h>
#endif

#include <gnome.h>
#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#include "gui/bibletext_dialog.h"
#include "gui/display_info.h"
#include "gui/font_dialog.h"
#include "gui/sidebar.h"
#include "gui/main_window.h"
#include "gui/menu_popup.h"
#include "gui/navbar_versekey_dialog.h"
#include "gui/xiphos.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/navbar.h"
#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
#include "main/display.hh"


/******************************************************************************
 * static - global to this file only
 */
static DIALOG_DATA *cur_vt;

GtkTextBuffer *text_buffer;
/******************************************************************************
 * externs
 */
extern gboolean gsI_isrunning;	/* information dialog */
extern gboolean bible_freed;
extern gboolean bible_apply_change;
extern gboolean do_display;


/******************************************************************************
 * Name
 *   gui_close_text_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void btn_close_clicked(GtkButton * button,DIALOG_DATA * vt)
 *
 * Description
 *   call gtk_widget_destroy to destroy the bibletext dialog
 *   set vt->dialog to NULL
 *
 * Return value
 *   void
 */
/*
static void close_text_dialog(DIALOG_DATA * t)
{
	if (t->dialog) {
		bible_freed = FALSE;
		gtk_widget_destroy(t->dialog);
	}
}
*/

#ifndef USE_GTKMOZEMBED
/******************************************************************************
 * Name
 *   show_in_statusbar
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   void show_in_statusbar(GtkWidget * statusbar, gchar * key, gchar * mod)
 *
 * Description
 *   display information (morph or strongs) in statusbar
 *
 * Return value
 *   void
 */

static void show_in_statusbar(GtkWidget *statusbar,
			      gchar *key,
			      gchar *mod)
{
	gchar *str;
	gchar *text;
	gint context_id2;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
					 settings.program_title);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);

	text = main_get_striptext(mod, key);
	str = remove_linefeeds(text);
	if (str) {
		gtk_statusbar_push(GTK_STATUSBAR(statusbar),
				   context_id2, str);
		g_free(str);
	}
	g_free(text);
}
#endif


/******************************************************************************
 * Name
 *   link_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void link_clicked(GtkHTML * html, const gchar * url,
 *							gpointer data)
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

#ifndef USE_GTKMOZEMBED
static void link_clicked(GtkHTML *html,
			 const gchar *url,
			 DIALOG_DATA *vt)
{
	cur_vt = vt;
	main_dialogs_url_handler(vt, url, TRUE);
}
#endif

/******************************************************************************
 * Name
 *   book_changed
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void book_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   change book
 *
 * Return value
 *   void
 */
/*
static void book_changed(GtkEditable *editable,
			 DIALOG_DATA *vt)
{
//	gchar buf[256];
	gchar *url;
	gchar *bookname = gtk_editable_get_chars(editable, 0, -1);
	cur_vt = vt;
	if (*bookname) {
		url = g_strdup_printf("sword:///%s 1:1", bookname);
		main_dialogs_url_handler(vt, url, TRUE);
		g_free(url);
	}
}

*/
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
/*
static gboolean chapter_button_release_event(GtkWidget *widget,
					     GdkEventButton *event,
					     DIALOG_DATA *vt)
{
	G_CONST_RETURN gchar *bookname;
	gchar *url;
	gint chapter;
	cur_vt = vt;

	bookname = (gchar*)gtk_entry_get_text(GTK_ENTRY(vt->cbe_book));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (vt->spb_chapter));
	url = g_strdup_printf("sword:///%s %d:1", bookname, chapter);
	main_dialogs_url_handler(vt, url, TRUE);
	g_free(url);
	return FALSE;
}
*/

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   gboolean (GtkWidget * widget, GdkEventButton * event,  gpointer user_data)
 *
 * Description
 *    change verse
 *
 * Return value
 *   gboolean
 */
/*
static gboolean verse_button_release_event(GtkWidget *widget,
					   GdkEventButton *event,
					   DIALOG_DATA *vt)
{
	const gchar *bookname;
//	gchar buf[256], *val_key;
	gint chapter, verse;
	gchar *url;
	cur_vt = vt;

	bookname = gtk_entry_get_text(GTK_ENTRY(vt->cbe_book));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (vt->spb_chapter));
	verse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (vt->spb_verse));

	url = g_strdup_printf("sword:///%s %d:%d", bookname, chapter, verse);
	main_dialogs_url_handler(vt, url, TRUE);
	g_free(url);
	return FALSE;
}
*/

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
				      DIALOG_DATA * vt)
{
	// if <enter> key 
	cur_vt = vt;
	if (event->keyval == 65293 || event->keyval == 65421) {
		const gchar *buf = gtk_entry_get_text(GTK_ENTRY(widget));
		gchar *url = g_strdup_printf("sword:///%s", buf);
		main_dialogs_url_handler(vt, url, TRUE);
		g_free(url);
	}
	return FALSE;
}
*/
 
/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void dialog_destroy(GtkObject *object, DL_DIALOG * dlg)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void dialog_destroy(GtkObject *object,
			   DIALOG_DATA *vt)
{
	if (!bible_freed)
		main_free_on_destroy(vt);
	bible_freed = FALSE;
}


static gboolean on_dialog_motion_notify_event(GtkWidget *widget,
					      GdkEventMotion *event,
					      DIALOG_DATA *vt)
{
	cur_vt = vt;
	return FALSE;
}


#ifndef USE_GTKMOZEMBED
/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, DIALOG_DATA * vt)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML *html,
		       const gchar *url,
		       DIALOG_DATA *vt)
{
	gchar buf[255], *buf1;
	gchar *url_buf = NULL;
	gint context_id2;

	cur_vt = vt;


	if (url) url_buf = g_strdup(url);
	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(vt->statusbar),
					 settings.program_title);
	gtk_statusbar_pop(GTK_STATUSBAR(vt->statusbar), context_id2);

	/***  moved out of url - clear appbar  ***/
	if (url == NULL) {
		gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar),
				   context_id2, "");
		in_url = FALSE;
	}
	/***  we are in an url  ***/
	else {
		in_url = TRUE;

		if (main_dialogs_url_handler(vt, url, FALSE))
			return;
		if (*url_buf == '#') {
			++url_buf;	/* remove # */
			if (*url_buf == 'T') {
				++url_buf;	/* remove T */
				if (*url_buf == 'G') {
					++url_buf;	/* remove G */
					if (settings.havethayer) {
						buf1 = g_strdup(url);
						show_in_statusbar(vt->
								  statusbar,
								  buf1,
								  "Thayer");
						g_free(buf1);
						return;
					}

					else
						return;
				}

				if (*url_buf == 'H') {
					++url_buf;	/* remove H */
					if (settings.havebdb) {
						buf1 = g_strdup(url);
						show_in_statusbar(vt->
								  statusbar,
								  buf1,
								  "BDB");
						g_free(buf1);
						return;
					}

					else
						return;
				}
			}

			if (*url_buf == 'G') {
				++url_buf;	/* remove G */
				buf1 = g_strdup(url_buf);
				if (atoi(buf1) > 5624) {
					if (settings.havethayer) {
						show_in_statusbar(vt->
								  statusbar,
								  buf1,
								  "Thayer");
						g_free(buf1);
						return;
					} else
						return;

				}

				else {
					show_in_statusbar(vt->statusbar,
							  buf1,
							  settings.
							  lex_greek);
					g_free(buf1);
					return;
				}
			}

			if (*url_buf == 'H') {
				++url_buf;	/* remove H */
				buf1 = g_strdup(url);
				if (atoi(buf1) > 8674) {
					if (settings.havebdb) {
						show_in_statusbar(vt->
								  statusbar,
								  buf1,
								  "BDB");
						g_free(buf1);
						return;
					}

					else
						return;
				}

				else {
					show_in_statusbar(vt->statusbar,
							  buf1,
							  settings.
							  lex_hebrew);
					g_free(buf1);
					return;
				}
			}
		}
		/***  thml morph tag  ***/
		else if (!strncmp(url, "type=morph", 10)) {
			gchar *modbuf = NULL;
			gchar *mybuf = NULL;
			buf1 = g_strdup(url_buf);
			modbuf = "Robinson";
			mybuf = NULL;
			mybuf = strstr(buf1, "value=");
			if (mybuf) {
				mybuf = strchr(mybuf, '=');
				++mybuf;
			}
			buf1 = g_strdup(mybuf);
			show_in_statusbar(vt->statusbar, buf1, modbuf);
			g_free(buf1);
			return;
		}
		/*** thml strongs ***/
		else if (!strncmp(url_buf, "type=Strongs", 12)) {
			gchar *modbuf = NULL;
			gchar *mybuf = NULL;
			gchar newref[80];
			gint type = 0;
			//buf = g_strdup(url);
			mybuf = NULL;
			mybuf = strstr(url, "value=");
			//i = 0;
			if (mybuf) {
				mybuf = strchr(mybuf, '=');
				++mybuf;
				if (mybuf[0] == 'H')
					type = 0;
				if (mybuf[0] == 'G')
					type = 1;
				++mybuf;
				sprintf(newref, "%5.5d", atoi(mybuf));
			}
			if (type)
				modbuf = settings.lex_greek;
			else
				modbuf = settings.lex_hebrew;

			buf1 = g_strdup(newref);
			show_in_statusbar(vt->statusbar, buf1, modbuf);
			g_free(buf1);
			return;
		}
		/***  any other link  ***/
		else
			sprintf(buf, "%s %s", _("Go to "), url);
		gtk_statusbar_push(GTK_STATUSBAR(vt->statusbar),
				   context_id2, buf);
	}
	if (url_buf)
		g_free(url_buf);
}
#endif


/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_with_main(DIALOG_DATA * c)
 *
 * Descriptionc->navbar.lookup_entry
 *   bring the the View Commentay Dialog module into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(DIALOG_DATA *c)
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
 *   void sync_toggled(GtkToggleButton * button, DIALOG_DATA * c)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void gui_bible_dialog_sync_toggled(GtkToggleButton *button,
				   DIALOG_DATA *c)
{
	/*if (c && (c->mod_type == COMMENTARY_TYPE)) {
		gui_commentary_dialog_sync_toggled(button, c);
	} else {*/
		if (c == NULL)
			c = cur_vt;
		if (button->active) {
			sync_with_main(c);
			c->sync = TRUE;
		} else
			c->sync = FALSE;
	//}
}

#ifdef OLD_NAVBAR
void on_comboboxentry4_changed(GtkComboBox *combobox,
			       DIALOG_DATA *c)
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

	url = g_strdup_printf("sword:///%s 1:1", book);
	buf = g_strdup_printf("%s 1:1", book);

	main_dialogs_url_handler(c, url, TRUE);
	main_navbar_set(c->navbar, buf);
	g_free(url);
	g_free(book);
	g_free(buf);
}


void on_comboboxentry5_changed(GtkComboBox *combobox,
			       DIALOG_DATA *c)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *chapter = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;

	GtkTreeModel *model = gtk_combo_box_get_model(combobox);
	GtkTreeModel *book_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (c->navbar.comboboxentry_book));


	if (!do_display)
		return;
	GS_message(("on_comboboxentry5_changed"));
	gtk_combo_box_get_active_iter(GTK_COMBO_BOX
				      (c->navbar.comboboxentry_book),
				      &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(book_model), &iter, 0, &book,
			   -1);

	gtk_combo_box_get_active_iter(combobox, &iter);
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   0, &chapter, -1);

	url = g_strdup_printf("sword:///%s %s:1", book, chapter);
	buf = g_strdup_printf("%s %s:1", book, chapter);
	main_dialogs_url_handler(c, url, TRUE);
	main_navbar_set(c->navbar, buf);

	g_free(url);
	g_free(book);
	g_free(chapter);
	g_free(buf);
}


void on_comboboxentry6_changed(GtkComboBox *combobox,
			       DIALOG_DATA *c)
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
				    (c->navbar.comboboxentry_book));
	GtkTreeModel *chapter_model =
	    gtk_combo_box_get_model(GTK_COMBO_BOX
				    (c->navbar.comboboxentry_chapter));


	if (!do_display)
		return;
	GS_message(("on_comboboxentry6_changed"));
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
	g_free(buf);
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

static void on_entry_activate(GtkEntry *entry,
			      DIALOG_DATA *c)
{
	const gchar *buf = gtk_entry_get_text(entry);
	if (c->navbar.key)
		g_free(c->navbar.key);
	c->navbar.key = g_strdup(buf);
	gchar *url = g_strdup_printf("sword:///%s", buf);
	main_dialogs_url_handler(c, url, TRUE);
	g_free(url);
	main_navbar_set(c->navbar, c->navbar.key);

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

static GtkWidget *create_nav_toolbar(DIALOG_DATA *c)
{
#ifdef OLD_NAVBAR
	GtkWidget *hbox3;
	GtkWidget *image;
	GtkWidget *separatortoolitem;
	GtkListStore *store;
	GtkCellRenderer *renderer;
	GtkWidget *sync_button;

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
			 "toggled", G_CALLBACK(gui_bible_dialog_sync_toggled), c);
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
#else
	c->navbar.type = NB_DIALOG;
	return gui_navbar_versekey_dialog_new(c);
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
static gboolean on_text_button_press_event(GtkWidget *widget,
					   GdkEventButton *event,
					   DIALOG_DATA *t)
{
	cur_vt = t;

	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		gui_menu_popup (NULL, t);
		//create_menu(t, event);
		break;
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *  on_button_release_event
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *  gboolean on_button_release_event(GtkWidget * widget,
			    GdkEventButton * event, DIALOG_DATA * t)
 *
 * Description
 *   called when mouse button is clicked in html widget
 *
 * Return value
 *   gboolean
 */

static gboolean on_button_release_event(GtkWidget *widget,
					GdkEventButton *event,
					DIALOG_DATA *t)
{

	gchar *key;
	cur_vt = t;

	//settings.whichwindow = MAIN_TEXT_WINDOW;
	/*
	 * set program title to current text module name
	 */
	//gui_change_window_title(t->mod_name);

	switch (event->button) {
	case 1:
		if (!in_url) {
			key = gui_button_press_lookup(t->html);
			if (key) {
				gchar *dict = NULL;
				if (settings.useDefaultDict)
					dict =
					    g_strdup(settings.
						     DefaultDict);
				else
					dict =
					    g_strdup(settings.
						     DictWindowModule);
				main_display_dictionary(dict,
								  key);
				g_free(key);
				if (dict)
					g_free(dict);
			}
		}
		break;
	case 2:
		break;
	case 3:
		break;
	}
	return FALSE;
}

static gboolean textview_button_press_event(GtkWidget *widget,
					    GdkEventButton *event,
					    DIALOG_DATA *t)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		//create_menu(t, event);
		gui_menu_popup (NULL, t);
		return TRUE;
		break;
	}
	return FALSE;
}

static gboolean textview_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					gpointer data)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
	return FALSE;
}



static gint tag_event_handler(GtkTextTag *tag,
			      GtkWidget *widget,
			      GdkEvent *event,
			      const GtkTextIter *iter,
			      gpointer user_data)
{
	gint char_index;

	char_index = gtk_text_iter_get_offset (iter);
	//printf ("offset = %d", char_index);
	switch (event->type)
	{
	case GDK_MOTION_NOTIFY:
		printf ("Motion event at char %d tag `%s'\n",
			char_index, tag->name);
		return TRUE;
		break;

	case GDK_BUTTON_PRESS:
		printf ("Button press at char %d tag `%s'\n",
			char_index, tag->name);
		switch(event->button.button){
		case 1:
			//return do_something_with_tag(tag, iter, user_data);
			break;
		case 2:
		case 3:
			break;
		}

		return TRUE;
		break;

	case GDK_2BUTTON_PRESS:
		printf ("Double click at char %d tag `%s'\n",
			char_index, tag->name);
		return TRUE;
		break;

	case GDK_3BUTTON_PRESS:
		printf ("Triple click at char %d tag `%s'\n",
			char_index, tag->name);
		return TRUE;
		break;

	case GDK_BUTTON_RELEASE:
		printf ("Button release at char %d tag `%s'\n",
			char_index, tag->name);
		return TRUE;
		break;

	case GDK_KEY_PRESS:
	case GDK_KEY_RELEASE:
		printf ("Key event at char %d tag `%s'\n",
			char_index, tag->name);
		return TRUE;
		break;

	case GDK_ENTER_NOTIFY:
		printf ("enter event at char %d tag `%s'\n",
			char_index, tag->name);
		return TRUE;
		break;

	case GDK_LEAVE_NOTIFY:
		printf ("leave event at char %d tag `%s'\n",
			char_index, tag->name);
		return TRUE;
		break;

	case GDK_PROPERTY_NOTIFY:
	case GDK_SELECTION_CLEAR:
	case GDK_SELECTION_REQUEST:
	case GDK_SELECTION_NOTIFY:
	case GDK_PROXIMITY_IN:
	case GDK_PROXIMITY_OUT:
#if 0
	case GDK_DRAG_ENTER:
	case GDK_DRAG_LEAVE:
	case GDK_DRAG_MOTION:
	case GDK_DRAG_STATUS:
	case GDK_DROP_START:
	case GDK_DROP_FINISHED:
#endif /* 0 */
		return FALSE;

	default:
		return FALSE;
		break;
	}
	return FALSE;
}

static void setup_tag (GtkTextTag *tag,
		       gpointer user_data)
{
	g_signal_connect (G_OBJECT (tag),
			  "event",
			  G_CALLBACK (tag_event_handler),
			  user_data);
}

static void create_text_tags(GtkTextBuffer *buffer)
{
	GtkTextTag *tag;
	GdkColor color;

	/* verse number tag verse style*/
	tag = gtk_text_buffer_create_tag (buffer, "verseNumber", NULL);
	setup_tag (tag, buffer);
	color.red = 0;
	color.green = 0;
	color.blue = 0xffff;
	g_object_set (G_OBJECT (tag),
		      "foreground_gdk", &color,
		      NULL);

	/* current verse color tag */
	tag = gtk_text_buffer_create_tag (buffer, "fg_currentverse", NULL);
	color.red = 0;
	color.green = 0xbbbb;
	color.blue = 0;
	g_object_set (G_OBJECT (tag),
		      "foreground_gdk", &color,
		      NULL);

	/*  verse color tag */
	tag = gtk_text_buffer_create_tag (buffer, "fg_verse", NULL);
	color.red = 0;
	color.green = 0;
	color.blue = 0;
	g_object_set (G_OBJECT (tag),
		      "foreground_gdk", &color,
		      NULL);

	/* right to left tag */
	tag = gtk_text_buffer_create_tag (buffer, "rtl_text", NULL);
        g_object_set (G_OBJECT (tag),
		      "wrap_mode", GTK_WRAP_WORD,
		      "direction", GTK_TEXT_DIR_RTL,
		      "indent", 0,
		      "left_margin", 0,
		      "right_margin", 0,
		      NULL);

	/* large tag */
	tag = gtk_text_buffer_create_tag (buffer, "large", NULL);
        g_object_set (G_OBJECT (tag),
		      "scale", PANGO_SCALE_XX_LARGE,
		      NULL);
}
#endif

#ifdef USE_GTKMOZEMBED
static void
_popupmenu_requested_cb (GeckoHtml *html,
			 gchar *uri,
			 gpointer user_data)
{
	DIALOG_DATA * d = (DIALOG_DATA *)user_data;
	gui_text_dialog_create_menu(d);
}
#endif

/******************************************************************************
 * Name
 *   create_bibletext_dialog
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   void create_bibletext_dialog(DIALOG_DATA * vt)
 *
 * Description
 *   create a text dialog
 *
 * Return value
 *   void
 */

void gui_create_bibletext_dialog(DIALOG_DATA * vt)
{
	GtkWidget *vbox33;
	GtkWidget *paned;
	GtkWidget *frame;
#ifndef USE_GTKMOZEMBED
	GtkWidget *swVText;
	//gchar *gdk_font = NULL;
	gchar file[250];
#endif

	vt->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_object_set_data(GTK_OBJECT(vt->dialog), "dlg->dialog",
			    vt->dialog);
	gtk_window_set_title(GTK_WINDOW(vt->dialog),
			     main_get_module_description(vt->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(vt->dialog), 400, 400);
	gtk_window_set_resizable(GTK_WINDOW(vt->dialog), TRUE);

	vbox33 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox33);
	gtk_container_add(GTK_CONTAINER(vt->dialog), vbox33);

	vt->toolbar_nav = create_nav_toolbar(vt);
	gtk_widget_show(vt->toolbar_nav);
	gtk_box_pack_start(GTK_BOX(vbox33), vt->toolbar_nav, FALSE, FALSE,
			   0);

	paned = gtk_vpaned_new();
	gtk_box_pack_start(GTK_BOX(vbox33), paned, TRUE, TRUE, 0);
	gtk_widget_show(paned);

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_paned_add1((GtkPaned *)paned,frame);
	gtk_widget_set_size_request(frame, -1, 400);

#ifdef USE_GTKMOZEMBED
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
	vt->html = GTK_WIDGET(gecko_html_new(vt,TRUE,DIALOG_TEXT_TYPE));
	gtk_widget_show(vt->html);
	gtk_container_add(GTK_CONTAINER(frame), vt->html);
	g_signal_connect((gpointer)vt->html,
		      "popupmenu_requested",
		      G_CALLBACK(_popupmenu_requested_cb),
		      vt);

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_paned_add2((GtkPaned *)paned,frame);
	gtk_widget_set_size_request(frame, -1, 100);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);

	vt->previewer = GTK_WIDGET(gecko_html_new(vt,FALSE,VIEWER_TYPE));
	gtk_widget_show(vt->previewer);
	gtk_container_add(GTK_CONTAINER(frame), vt->previewer);
#else
	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(swVText);
	gtk_container_add(GTK_CONTAINER(frame), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)swVText,
					    settings.shadow_type);

	if (!vt->is_rtol) {
		vt->html = gtk_html_new();
		gtk_widget_show(vt->html);
		gtk_container_add(GTK_CONTAINER(swVText), vt->html);
		gtk_html_load_empty(GTK_HTML(vt->html));
		g_signal_connect(G_OBJECT(vt->html), "on_url",
				   G_CALLBACK(dialog_url), (gpointer) vt);
		g_signal_connect(GTK_OBJECT(vt->html), "link_clicked",
				   G_CALLBACK(link_clicked), vt);
		g_signal_connect(GTK_OBJECT(vt->html),
				   "motion_notify_event",
				   G_CALLBACK
				   (on_dialog_motion_notify_event), vt);
		g_signal_connect(GTK_OBJECT(vt->html),
				   "button_release_event",
				   G_CALLBACK
				   (on_button_release_event),
				   (DIALOG_DATA *) vt);
		g_signal_connect(GTK_OBJECT(vt->html),
				   "button_press_event",
				   G_CALLBACK
				   (on_text_button_press_event),
				   (DIALOG_DATA *) vt);
		g_signal_connect(GTK_OBJECT(vt->html),
				   "url_requested",
				   G_CALLBACK
				   (url_requested),
				   (DIALOG_DATA *) vt);
	} else {
		// * use gtktextview for right to left text * //
		sprintf(file, "%s/fonts.conf", settings.gSwordDir);
		vt->text = gtk_text_view_new ();
		gtk_widget_show (vt->text);
		gtk_container_add (GTK_CONTAINER (swVText), vt->text);
		gtk_text_view_set_editable (GTK_TEXT_VIEW (vt->text), FALSE);
		text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (vt->text));
		create_text_tags(text_buffer);
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW (vt->text), GTK_WRAP_WORD);

		g_signal_connect(GTK_OBJECT(vt->text),
				   "button_press_event",
				   G_CALLBACK
				   (textview_button_press_event),
				   (DIALOG_DATA *) vt);
		g_signal_connect(GTK_OBJECT(vt->text),
				   "button_release_event",
				   G_CALLBACK
				   (textview_button_release_event),
				   (DIALOG_DATA *) vt);
		g_signal_connect(GTK_OBJECT(vt->text),
				   "url_requested",
				   G_CALLBACK
				   (url_requested),
				   (DIALOG_DATA *) vt);
	}

	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_paned_add2((GtkPaned *)paned,frame);

	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(swVText);
	gtk_container_add(GTK_CONTAINER(frame), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)swVText,
                                             settings.shadow_type);

	vt->previewer = gtk_html_new();
	gtk_widget_show(vt->previewer);
	gtk_container_add(GTK_CONTAINER(swVText), vt->previewer);
	gtk_html_load_empty(GTK_HTML(vt->previewer));

	g_signal_connect(GTK_OBJECT(vt->previewer), "link_clicked",
				   G_CALLBACK(link_clicked), vt);
#endif
	vt->statusbar = gtk_statusbar_new();
	gtk_widget_show(vt->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox33), vt->statusbar, FALSE, FALSE,
			   0);

	g_signal_connect(GTK_OBJECT(vt->dialog),
			   "destroy",
			   G_CALLBACK(dialog_destroy), vt);


	g_signal_connect(GTK_OBJECT(vt->dialog),
			   "motion_notify_event",
			   G_CALLBACK
			   (on_dialog_motion_notify_event), vt);
}


void gui_text_dialog_create_menu(DIALOG_DATA * d)
{
	gui_menu_popup (NULL, d);
}

/******   end of file   ******/
