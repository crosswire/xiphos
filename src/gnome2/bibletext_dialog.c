/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.c - view Bible text module in a dialog
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

#ifdef USE_GTKMOZEMBED
#include <gtkmozembed.h>
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

#include "gui/bibletext_dialog.h"
#include "gui/display_info.h"
#include "gui/font_dialog.h"
#include "gui/sidebar.h"
#include "gui/main_window.h"
#include "gui/navbar_versekey.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/embed-dialogs.h"
#include "main/navbar.h"
#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
#include "main/display.hh"

static void create_menu(DIALOG_DATA * t ,GdkEventButton * event);

/******************************************************************************
 * static - global to this file only
 */
static DIALOG_DATA *cur_vt;
//static gboolean in_url;
static GtkWidget *sync_button;
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
 *  
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   void  gui_on_lookup_bibletext_dialog_selection
		(GtkMenuItem * menuitem, gchar * dict_mod_description)	
 *
 * Description
 *   lookup seledtion in a dict/lex module
 *
 * Return value
 *   void
 */
/*
void gui_on_lookup_bibletext_dialog_selection
		(GtkMenuItem * menuitem, gchar * dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description(dict_mod_description);
	if(!mod_name) 
		return;

	dict_key = gui_get_word_or_selection(cur_vt->html, FALSE);
	if (dict_key && mod_name) {
		main_display_dictionary(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}
}
*/
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

static void close_text_dialog(DIALOG_DATA * t)
{
	if (t->dialog) {
		bible_freed = FALSE;
		gtk_widget_destroy(t->dialog);
	}
}


/******************************************************************************
 * Name
 *   show_in_statusbar
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   void show_in_statusbar(GtkWidget * statusbar, gchar * key, 
 *							gchar * mod)
 *
 * Description
 *   display information (morph or strongs) in statusbar
 *
 * Return value
 *   void
 */

static void show_in_statusbar(GtkWidget * statusbar, gchar * key,
			      gchar * mod)
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



#ifndef USE_GTKMOZEMBED
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

static void link_clicked(GtkHTML * html, const gchar * url,
			 DIALOG_DATA * vt)
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

static void book_changed(GtkEditable * editable, DIALOG_DATA * vt)
{
	gchar buf[256];
	gchar *url;
	gchar *bookname = gtk_editable_get_chars(editable, 0, -1);
	cur_vt = vt;
	if (*bookname) {
		url = g_strdup_printf("sword:///%s 1:1", bookname);
		main_dialogs_url_handler(vt, url, TRUE);
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
					     DIALOG_DATA * vt)
{
	G_CONST_RETURN gchar *bookname;
	gchar *val_key;
	gchar *url;
	gchar buf[256];
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
					   DIALOG_DATA * vt)
{
	const gchar *bookname;
	gchar buf[256], *val_key;
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
				      DIALOG_DATA * vt)
{
	/* if <enter> key */
	cur_vt = vt;
	if (event->keyval == 65293 || event->keyval == 65421) {
		const gchar *buf = gtk_entry_get_text(GTK_ENTRY(widget));
		gchar *url = g_strdup_printf("sword:///%s", buf);
		main_dialogs_url_handler(vt, url, TRUE);
		g_free(url);
	}
	return FALSE;
}


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

static void dialog_destroy(GtkObject * object, DIALOG_DATA * vt)
{
	if (!bible_freed)
		main_free_on_destroy(vt);
	bible_freed = FALSE;
}


static gboolean on_dialog_motion_notify_event(GtkWidget * widget,
					      GdkEventMotion * event,
					      DIALOG_DATA * vt)
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

static void dialog_url(GtkHTML * html, const gchar * url,
		       DIALOG_DATA * vt)
{
	gchar buf[255], *buf1;
	gchar *url_buf = NULL;
	gint context_id2;

	cur_vt = vt;
	
	
	if(url) url_buf = g_strdup(url);
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
		
		if(main_dialogs_url_handler(vt, url, FALSE))
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
/*
			mybuf = strstr(url_buf, "class=");
			if (mybuf) {
				gint i;
				modbuf = strchr(mybuf, '=');
				++modbuf;
				for (i = 0; i < strlen(modbuf); i++) {
					if (modbuf[i] == ' ') {
						modbuf[i] = '\0';
						break;
					}
				}
			}
*/
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
	if(url_buf) 
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

static void sync_with_main(DIALOG_DATA * c)
{
	gchar *url =
	    g_strdup_printf("sword:///%s", settings.currentverse);
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

void gui_bible_dialog_sync_toggled(GtkToggleButton * button, DIALOG_DATA * c)
{
	if (button->active) {
		sync_with_main(c);
		c->sync = TRUE;
	} else
		c->sync = FALSE;
}

#ifdef OLD_NAVBAR
void on_comboboxentry4_changed(GtkComboBox * combobox, DIALOG_DATA * c)
{
	gchar *url = NULL;
	gchar *book = NULL;
	gchar *buf = NULL;
	GtkTreeIter iter;
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
	g_free(buf);
}


void on_comboboxentry5_changed(GtkComboBox * combobox, DIALOG_DATA * c)
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

	url = g_strdup_printf("sword:///%s %s:1", book, chapter);
	buf = g_strdup_printf("%s %s:1", book, chapter);
	main_dialogs_url_handler(c, url, TRUE);
	main_navbar_set(c->navbar, buf);

	g_free(url);
	g_free(book);
	g_free(chapter);
	g_free(buf);
}


void on_comboboxentry6_changed(GtkComboBox * combobox, DIALOG_DATA * c)
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

static void on_entry_activate(GtkEntry * entry, DIALOG_DATA * c)
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

static GtkWidget *create_nav_toolbar(DIALOG_DATA * c)
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
static gboolean on_text_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					DIALOG_DATA * t)
{
	cur_vt = t;
	
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		create_menu(t, event);
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

static gboolean on_button_release_event(GtkWidget * widget,
					GdkEventButton * event,
					DIALOG_DATA * t)
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

static gboolean textview_button_press_event(GtkWidget * widget,
					GdkEventButton * event,
					DIALOG_DATA * t)
{
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		create_menu(t, event);
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



static gint tag_event_handler (GtkTextTag *tag, GtkWidget *widget,
	GdkEvent *event, const GtkTextIter *iter, gpointer user_data)
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
    case GDK_DRAG_ENTER:
    case GDK_DRAG_LEAVE:
    case GDK_DRAG_MOTION:
    case GDK_DRAG_STATUS:
    case GDK_DROP_START:
    case GDK_DROP_FINISHED:
    	return FALSE;
    default:
    	return FALSE;
      break;
    }    
    return FALSE;
}

static void setup_tag (GtkTextTag *tag, gpointer user_data)
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
	GdkColor color2;
	GdkColor color_red;
	GdkColor colorLink;
	PangoFontDescription *font_desc;
	
		
		
	/* verse number tag verse style*/
	tag = gtk_text_buffer_create_tag (buffer, "verseNumber", NULL);
	setup_tag (tag, buffer);  
	color.red = color.green = 0;
	color.blue = 0xffff;
		"scale", PANGO_SCALE_XX_SMALL,
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);	
		
	/* current verse color tag */	
	tag = gtk_text_buffer_create_tag (buffer, "fg_currentverse", NULL);
	color.blue = 0;
	color.green = 0xbbbb;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);
		
	/*  verse color tag */	
	tag = gtk_text_buffer_create_tag (buffer, "fg_verse", NULL);
	color.blue = 0;
	color.green = 0;
	color.red = 0;
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
	GtkWidget *swVText; 
	//gchar *gdk_font = NULL;
	gchar file[250];


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
	vt->html = embed_dialogs_new((DIALOG_DATA*) vt);
	gtk_widget_show(vt->html);
	gtk_container_add(GTK_CONTAINER(frame), vt->html);
	   
	frame = gtk_frame_new(NULL);
	gtk_widget_show(frame);
	gtk_paned_add2((GtkPaned *)paned,frame);
	gtk_widget_set_size_request(frame, -1, 100);	
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
	
	vt->previewer = embed_dialogs_new((DIALOG_DATA*) vt);
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

/** BiblicalText menu stuff
 **
 **
 **
 **
 **
 **
 **/

static void on_about_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_display_about_module_dialog(cur_vt->mod_name, FALSE);
}


static void on_item1_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void on_print1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_html_print(cur_vt->html, FALSE);
}


static void on_copy2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_copy_html(cur_vt->html);
}


static void on_find1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_find_dlg(cur_vt->html, cur_vt->mod_name, FALSE, NULL);
}


static void on_item2_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


static void
on_set_module_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *url = g_strdup_printf(	"sword://%s/%s",
					cur_vt->mod_name,
					settings.currentverse);
	gui_set_module_font(cur_vt->mod_name);
	/* show the change */
	main_dialogs_url_handler(cur_vt, url, TRUE);
	g_free(url);
}

static void on_read_selection_aloud(GtkMenuItem * menuitem,
				    gpointer user_data)
{
	GtkHTML *html = GTK_HTML(cur_vt->html);
	int len;
	gchar *dict_key = gtk_html_get_selection_html(html, &len);

	if (dict_key && len && (*dict_key != '\0')) {
		ReadAloud(0, dict_key);
		g_free(dict_key);
	} else
		gui_generic_warning("No selection made");
}

static void on_use_current_dictionary_activate(GtkMenuItem * menuitem,
				   		gpointer user_data)
{
#ifdef USE_GTKMOZEMBED	
	embed_copy_selection(GTK_MOZ_EMBED(cur_vt->html));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);
#else
	gchar *dict_key = gui_get_word_or_selection(cur_vt->html, FALSE);
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


static void on_show_tabs_activate(GtkMenuItem * menuitem, gpointer user_data)
{

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
 *   lookup selection in a dict/lex module
 *
 * Return value
 *   void
 */

static void lookup_bibletext_selection(GtkMenuItem * menuitem,
				    gchar * dict_mod_description)
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;

	mod_name = main_module_name_from_description(dict_mod_description);
#ifdef USE_GTKMOZEMBED	
	embed_copy_selection(GTK_MOZ_EMBED(cur_vt->html));
	gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);
	dict_key = 
		g_strdup(gtk_editable_get_chars(
			(GtkEditable *)widgets.entry_dict,0,-1));

#else
	dict_key = gui_get_word_or_selection(cur_vt->html, FALSE);
#endif
	if (dict_key && mod_name) {
		main_display_dictionary(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/bibletext.h"
 *
 *   void (GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void edit_percomm(GtkMenuItem * menuitem, gpointer user_data)
{
//	if (settings.use_percomm_dialog)
//		gui_open_commentary_editor((gchar *) user_data);
/*	else {
		gtk_notebook_set_current_page(GTK_NOTEBOOK
				      (widgets.workbook_lower),
				      settings.percomm_page);
		gui_set_percomm_page((gchar *) user_data);
	}
*/
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
	gchar *module_name = main_module_name_from_description((gchar *)user_data);
	gchar *url = NULL;
	if(module_name) {
		url = g_strdup_printf("sword://%s/%s",
					module_name,
					cur_vt->key);
		main_dialogs_url_handler(cur_vt, url, TRUE);
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

static void global_option_red_words(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->words_in_red = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_strongs(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->strongs = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_morphs(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->morphs = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_footnotes(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->footnotes = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_greekaccents(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->greekaccents = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_lemmas(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->lemmas = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_scripturerefs(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->scripturerefs = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_hebrewpoints(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->hebrewpoints = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_hebrewcant(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->hebrewcant = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
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

static void global_option_headings(GtkCheckMenuItem * menuitem, DIALOG_DATA * t)
{
	gchar *url = g_strdup_printf("sword://%s/%s", t->mod_name, t->key);
	t->ops->headings = menuitem->active;
	main_dialogs_url_handler(t, url, TRUE);
	g_free(url);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gui/bibletext_menu.h"
 *
 *   void (GtkMenuItem * menuitem, DIALOG_DATA * t)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_close_activate(GtkMenuItem * menuitem, DIALOG_DATA * t)
{
	close_text_dialog(cur_vt);
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
	gchar *url = g_strdup_printf("sword:///%s", settings.currentverse);
	main_dialogs_url_handler(cur_vt, url, TRUE);
	g_free(url);
}


static GnomeUIInfo view_text_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("item1"),
	 NULL,
	 (gpointer) on_item1_activate, NULL, NULL,
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
	 0, (GdkModifierType) 0, NULL },
	 {
	 GNOME_APP_UI_ITEM, N_("Sync with main"),
	 NULL,
	 (gpointer) on_sync_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-refresh",
	 0, (GdkModifierType) 0, NULL},
	 {
	 GNOME_APP_UI_TOGGLEITEM, N_("Stay in sync"),
	 NULL,
	 (gpointer) gui_bible_dialog_sync_toggled, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-refresh",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_PRINT_ITEM(on_print1_activate, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_CLOSE_ITEM (on_close_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo note_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("item2"),
	 NULL,
	 (gpointer) on_item2_activate, NULL, NULL,
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
	 NULL, //(gpointer) on_global_option,
	 (gpointer) "Words of Christ in Red",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM,
	 N_("Strong's Numbers"),
	 NULL,
	 NULL, //(gpointer) on_global_option,
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
	 NULL, //(gpointer) on_global_option,
	 (gpointer) "Morphological Tags",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Footnotes"),
	 NULL,
	 NULL, //(gpointer) on_global_option,
	 (gpointer) "Footnotes",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Greek Accents"),
	 NULL,
	 NULL, //(gpointer) on_global_option,
	 (gpointer) "Greek Accents",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Lemmas"),
	 NULL,
	 NULL, //(gpointer) on_global_option,
	 (gpointer) "Lemmas",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Scripture Cross-references"),
	 NULL,
	 NULL, //(gpointer) on_global_option,
	 (gpointer) "Scripture Cross-references",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Vowel Points"),
	 NULL,
	 NULL, //(gpointer) on_global_option,
	 (gpointer) "Hebrew Vowel Points",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Hebrew Cantillation"),
	 NULL,
	 NULL, //(gpointer) on_global_option,
	 (gpointer) "Hebrew Cantillation",	/* not seen by user */
	 NULL,
	 GNOME_APP_PIXMAP_NONE, NULL,
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_TOGGLEITEM, N_("Headings"),
	 NULL,
	 NULL, //(gpointer) on_global_option,
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
	 GNOME_APP_UI_ITEM, N_("Read Selection Aloud"),
	 NULL,
	 (gpointer) on_read_selection_aloud, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gnome-stock-mic",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

static void create_menu(DIALOG_DATA * t ,GdkEventButton * event)
{
	GtkWidget *menu1;
	GtkWidget *lookup_selection_menu;
	GtkWidget *usecurrent;
	GtkWidget *view_menu;
	GtkWidget *separator;
	GtkWidget *edit_per_menu;
	GnomeUIInfo *menuitem;
	gchar *mod_name = t->mod_name;
	
	cur_vt = t;
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
	gtk_widget_hide(file3_menu_uiinfo[0].widget);	
	gtk_widget_hide(edit3_menu_uiinfo[2].widget);	

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (file3_menu_uiinfo[2].widget),
				       t->sync);

/*
	view_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file3_menu_uiinfo[0].widget),
				  view_menu);

	gui_add_mods_2_gtk_menu(TEXT_DESC_LIST, view_menu,
				(GCallback) on_view_mod_activate);

	edit_per_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit3_menu_uiinfo[2].widget),
				  edit_per_menu);

	gui_add_mods_2_gtk_menu(PERCOMM_LIST, edit_per_menu,
				(GCallback) edit_percomm);
*/


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
				(GCallback)lookup_bibletext_selection);


	if ((main_check_for_global_option(mod_name,
				     "GBFRedLetterWords")) ||
	    (main_check_for_global_option(mod_name,
				     "OSISRedLetterWords"))) {
		gtk_widget_show(module_options_menu_uiinfo[2].widget);	//"words_in_red");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[2].
				    widget)->active = t->ops->words_in_red;					     
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[2].widget),
				"activate",
				G_CALLBACK(global_option_red_words), 
				(DIALOG_DATA *)t);
	}
	if ((main_check_for_global_option
	     (mod_name, "GBFStrongs"))
	    ||
	    (main_check_for_global_option
	     (mod_name, "ThMLStrongs"))
	    ||
	    (main_check_for_global_option
	     (mod_name, "OSISStrongs"))) {
		gtk_widget_show(module_options_menu_uiinfo[3].widget);	//"strongs_numbers");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[3].
				    widget)->active = t->ops->strongs;					     
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[3].widget),
				"activate",
				G_CALLBACK(global_option_strongs), 
				(DIALOG_DATA *)t);
	}
	if ((main_check_for_global_option(mod_name, "GBFMorph")) ||
	    (main_check_for_global_option(mod_name, "ThMLMorph")) ||
	    (main_check_for_global_option(mod_name, "OSISMorph"))) {
		gtk_widget_show(module_options_menu_uiinfo[4].widget);	//"/morph_tags");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[4].
				    widget)->active = t->ops->morphs;
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[4].widget),
				"activate",
				G_CALLBACK(global_option_morphs), 
				(DIALOG_DATA *)t);
	}
	if ((main_check_for_global_option(mod_name, "GBFFootnotes")) ||
	    (main_check_for_global_option(mod_name, "ThMLFootnotes")) ||
	    (main_check_for_global_option(mod_name, "OSISFootnotes"))) {
		gtk_widget_show(module_options_menu_uiinfo[5].widget);	//"footnotes");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[5].
				    widget)->active = t->ops->footnotes;
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[5].widget),
				"activate",
				G_CALLBACK(global_option_footnotes), 
				(DIALOG_DATA *)t);
	}
	if (main_check_for_global_option(mod_name, "UTF8GreekAccents")) {
		gtk_widget_show(module_options_menu_uiinfo[6].widget);	// "greek_accents");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[6].
				    widget)->active = t->ops->greekaccents;
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[6].widget),
				"activate",
				G_CALLBACK(global_option_greekaccents), 
				(DIALOG_DATA *)t);
	}
	if (main_check_for_global_option(mod_name, "ThMLLemma")) {
		gtk_widget_show(module_options_menu_uiinfo[7].widget);	//"lemmas");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[7].
				    widget)->active = t->ops->lemmas;
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[7].widget),
				"activate",
				G_CALLBACK(global_option_lemmas), 
				(DIALOG_DATA *)t);
	}
	if (main_check_for_global_option(mod_name, "ThMLScripref") ||
	    (main_check_for_global_option(mod_name, "OSISScripref"))) {
		gtk_widget_show(module_options_menu_uiinfo[8].widget);	//"cross_references");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[8].
				    widget)->active = t->ops->scripturerefs;
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[8].widget),
				"activate",
				G_CALLBACK(global_option_scripturerefs), 
				(DIALOG_DATA *)t);
	}
	if (main_check_for_global_option(mod_name, "UTF8HebrewPoints")) {
		gtk_widget_show(module_options_menu_uiinfo[9].widget);	//"hebrew_vowel_points");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[9].
				    widget)->active = t->ops->hebrewpoints;
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[9].widget),
				"activate",
				G_CALLBACK(global_option_hebrewpoints), 
				(DIALOG_DATA *)t);
	}
	if (main_check_for_global_option(mod_name, "UTF8Cantillation")) {
		gtk_widget_show(module_options_menu_uiinfo[10].widget);	//"hebrew_cantillation");
		GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[10].
				    widget)->active = t->ops->hebrewcant;
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[10].widget),
				"activate",
				G_CALLBACK(global_option_hebrewcant), 
				(DIALOG_DATA *)t);
	}
	if (main_check_for_global_option(mod_name, "ThMLHeadings") ||
	    (main_check_for_global_option(mod_name, "OSISHeadings"))) {
		gtk_widget_show(module_options_menu_uiinfo[11].widget);	//"headings");
		 GTK_CHECK_MENU_ITEM(module_options_menu_uiinfo[11].
				    widget)->active = t->ops->headings;
		
		g_signal_connect(GTK_OBJECT(module_options_menu_uiinfo[11].widget),
				"activate",
				G_CALLBACK(global_option_headings), 
				(DIALOG_DATA *)t);
	}
	if (main_check_for_global_option(mod_name, "ThMLVariants")) {
		gtk_widget_show(module_options_menu_uiinfo[12].widget);	//"variants");

		gtk_widget_show(all_readings_uiinfo[0].widget);	//"all_readings");

		gtk_widget_show(all_readings_uiinfo[1].widget);	//"primary_reading");

		gtk_widget_show(all_readings_uiinfo[2].widget);	//"secondary_reading");

	}
	if(t->is_locked)
		gtk_widget_show(menu1_uiinfo[6].widget);
	 
	gtk_menu_popup((GtkMenu*)menu1, NULL, NULL, NULL, NULL, 2,
		     			gtk_get_current_event_time());
	/*gnome_popup_menu_do_popup_modal(menu1, NULL,
					NULL, event, NULL,
					t->text);
	gtk_widget_destroy(menu1);*/
}

void gui_text_dialog_create_menu(DIALOG_DATA * d)
{
	create_menu(d ,NULL);
}




/******   end of file   ******/
