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
#include <gal/widgets/e-unicode.h>

#include "gui/gtkhtml_display.h"
#include "gui/commentary_dialog.h"
#include "gui/commentary_menu.h"
#include "gui/html.h"
#include "gui/gnomesword.h"
#include "gui/editor_menu.h"
#include "gui/main_window.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/toolbar_style.h"
#include "gui/toolbar_edit.h"

#include "main/commentary.h"
#include "main/percomm.h"
#include "main/sword.h"
#include "main/lists.h"
#include "main/settings.h"


/****************************************************************************************
 * static - global to this file only
 */
static GList *dialog_list;
static VIEW_COMM *cur_vc;
static gboolean dialog_freed;
static gboolean apply_change;



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

void gui_on_lookup_commentary_dialog_selection
    (GtkMenuItem * menuitem, gchar * dict_mod_description) {
	gchar *dict_key, mod_name[16];

	memset(mod_name, 0, 16);
	module_name_from_description(mod_name, dict_mod_description);

	dict_key = gui_get_word_or_selection(cur_vc->c->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			gui_display_dictlex_in_viewer(mod_name,
						      dict_key);
		if (settings.inDictpane)
			gui_change_module_and_key(mod_name, dict_key);
		g_free(dict_key);
	}
}

/******************************************************************************
 * Name
 *  display
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void display(VIEW_COMM *c, gchar * key)	
 *
 * Description
 *   calls entry_display to display a commentary entry 
 *
 * Return value
 *   void
 */

static void display(VIEW_COMM * vc, gchar * key)
{
	gchar *text_str = NULL;
	gchar *strkey;

	cur_vc = vc;

	strkey = get_valid_key(key);
	strcpy(vc->c->key, strkey);

	if (vc->c->book_heading) {
		vc->c->book_heading = FALSE;
		text_str = get_book_heading(vc->c->mod_name, strkey);
	}

	else if (vc->c->chapter_heading) {
		vc->c->chapter_heading = FALSE;
		text_str = get_chapter_heading(vc->c->mod_name, strkey);
	}

	else {
		if (vc->ec) {
			settings.percomverse = key;
			change_percomm_module(vc->c->mod_name);
			strcpy(vc->ec->key, key);
			text_str = get_percomm_text(key);
			if (text_str) {
				entry_display(vc->ec->htmlwidget,
					      vc->c->mod_name,
					      text_str, key, FALSE);
				free(text_str);
				gui_update_statusbar(vc->ec);
				return;
			}
		} else {
			text_str =
			    get_commentary_text(vc->c->mod_name,
						strkey);
		}
	}

	if (text_str) {
		entry_display(vc->c->html, vc->c->mod_name,
			      text_str, key, TRUE);
		free(text_str);
	}
}


/******************************************************************************
 * Name
 *   free_on_destroy
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void free_on_destroy(VIEW_COMM * vc)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

static void free_on_destroy(VIEW_COMM * vc)
{
	dialog_list = g_list_remove(dialog_list, (VIEW_COMM *) vc);
	g_free(vc);
}


/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void on_dialog_destroy(GtkObject * object, 
 *						VIEW_COMM * vc)	
 *
 * Description
 *   shut down the View Commentay Dialog
 *
 * Return value
 *   void
 */

static void on_dialog_destroy(GtkObject * object, VIEW_COMM * vc)
{
	if (!dialog_freed) {
		if (vc->ec)
			gui_html_editor_control_data_destroy(NULL,
							     vc->ec);
		free_on_destroy(vc);
	}
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
 *   void gui_close_comm_dialog(void)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_close_comm_dialog(void)
{
	if (cur_vc->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(cur_vc->dialog);
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
                      GdkEventMotion  *event, VIEW_COMM * vc)	
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean on_dialog_motion_notify_event(GtkWidget * widget,
					      GdkEventMotion * event,
					      VIEW_COMM * vc)
{
	cur_vc = vc;
	return TRUE;
}



/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, VIEW_COMM * vc)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML * html, const gchar * url,
		       VIEW_COMM * vc)
{
	cur_vc = vc;
}


/******************************************************************************
 * Name
 *  html_button_pressed
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
 *					VIEW_COMM * vc)	
 *
 * Description
 *    mouse button pressed in window - used to set cur_vc to the current
 *    commentary dialog structure
 *
 * Return value
 *   gint
 */

static gint button_press_event(GtkWidget * html,
			       GdkEventButton * event, VIEW_COMM * vc)
{
	cur_vc = vc;
	return FALSE;
}


/******************************************************************************
 * Name
 *   update_controls
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void update_controls(VIEW_COMM * vc)	
 *
 * Description
 *   update the book, chapter and verse contorls
 *
 * Return value
 *   void
 */

static void update_controls(VIEW_COMM * vc)
{
	gchar *val_key;
	gint cur_chapter, cur_verse;

	cur_vc = vc;
	apply_change = FALSE;
	val_key = get_valid_key(vc->c->key);
	//vc->c->key = vc->key;
	cur_chapter = get_chapter_from_key(val_key);
	cur_verse = get_verse_from_key(val_key);
	/* 
	 *  set book, chapter,verse and freeform lookup entries
	 *  to new verse - apply_change is set to 
	 *  false so we don't start a loop
	 */
	gtk_entry_set_text(GTK_ENTRY(vc->cbe_book),
			   get_book_from_key(val_key));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (vc->spb_chapter), cur_chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (vc->spb_verse), cur_verse);
	gtk_entry_set_text(GTK_ENTRY(vc->freeform_lookup), val_key);
	g_free(val_key);

	apply_change = TRUE;
}


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

static void book_changed(GtkEditable * editable, VIEW_COMM * vc)
{
	if (apply_change) {
		gchar buf[256], *val_key;
		gchar *bookname =
		    gtk_entry_get_text(GTK_ENTRY(vc->cbe_book));
		sprintf(buf, "%s 1:1", bookname);
		val_key = get_valid_key(buf);
		display(vc, val_key);
		strcpy(vc->c->key, val_key);
		update_controls(vc);
		g_free(val_key);
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
					     VIEW_COMM * vc)
{
	if (apply_change) {
		gchar *bookname, *val_key;
		gchar buf[256];
		gint chapter;

		bookname = gtk_entry_get_text(GTK_ENTRY(vc->cbe_book));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (vc->spb_chapter));
		sprintf(buf, "%s %d:1", bookname, chapter);
		val_key = get_valid_key(buf);
		display(vc, val_key);
		strcpy(vc->c->key, val_key);
		update_controls(vc);
		g_free(val_key);
		return TRUE;
	}
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
					   VIEW_COMM * vc)
{
	if (apply_change) {
		gchar *bookname, buf[256], *val_key;
		gint chapter, verse;

		bookname = gtk_entry_get_text(GTK_ENTRY(vc->cbe_book));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (vc->spb_chapter));
		verse =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (vc->spb_verse));
		sprintf(buf, "%s %d:%d", bookname, chapter, verse);

		val_key = get_valid_key(buf);
		display(vc, val_key);
		strcpy(vc->c->key, val_key);
		update_controls(vc);
		g_free(val_key);
		return TRUE;
	}
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
				      VIEW_COMM * vc)
{
	/* if <enter> key */
	if (event->keyval == 65293 || event->keyval == 65421) {
		gchar *buf, *val_key;
		buf = gtk_entry_get_text(GTK_ENTRY(widget));
		val_key = get_valid_key(buf);
		display(vc, val_key);
		strcpy(vc->c->key, val_key);
		update_controls(vc);
		g_free(val_key);
	}
	return TRUE;
}



/******************************************************************************
 * Name
 *   sync_with_main
 *
 * Synopsis
 *   #include "gui/commentary_dialog.h"
 *
 *   void sync_with_main(VIEW_COMM * vc)	
 *
 * Description
 *   bring the the View Commentay Dialog module into sync with main window
 *
 * Return value
 *   void
 */

static void sync_with_main(VIEW_COMM * vc)
{
	strcpy(vc->c->key, settings.currentverse);
	update_controls(vc);
	display(vc, vc->c->key);
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

static GtkWidget *create_nav_toolbar(VIEW_COMM * vc)
{
	GtkWidget *toolbar_nav;
	GtkWidget *cbBook;
	GtkObject *spbChapter_adj;
	GtkObject *spbVerse_adj;

	toolbar_nav =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbar_nav);
	gtk_widget_set_usize(toolbar_nav, -2, 34);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar_nav),
				      GTK_RELIEF_NONE);

	cbBook = gtk_combo_new();
	gtk_widget_show(cbBook);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav), cbBook,
				  NULL, NULL);

	/*
	 * get and load books of the Bible 
	 */
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook),
				      get_list(BOOKS_LIST));

	vc->cbe_book = GTK_COMBO(cbBook)->entry;
	gtk_widget_set_usize(vc->cbe_book, 100, -2);
	gtk_widget_show(vc->cbe_book);

	spbChapter_adj = gtk_adjustment_new(8, -1, 151, 1, 10, 10);
	vc->spb_chapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbChapter_adj), 1, 0);
	gtk_widget_show(vc->spb_chapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vc->spb_chapter, NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON
				    (vc->spb_chapter), TRUE);

	spbVerse_adj = gtk_adjustment_new(28, -1, 180, 1, 10, 10);
	vc->spb_verse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVerse_adj), 1, 0);
	gtk_widget_show(vc->spb_verse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vc->spb_verse, NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(vc->spb_verse),
				    TRUE);

	vc->freeform_lookup = gtk_entry_new();
	gtk_widget_show(vc->freeform_lookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vc->freeform_lookup, NULL, NULL);
	gtk_entry_set_text(GTK_ENTRY(vc->freeform_lookup),
			   _("Romans 8:28"));

	gtk_signal_connect(GTK_OBJECT(vc->cbe_book),
			   "changed",
			   GTK_SIGNAL_FUNC(book_changed), vc);
	gtk_signal_connect(GTK_OBJECT(vc->spb_chapter),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (chapter_button_release_event), vc);
	gtk_signal_connect(GTK_OBJECT(vc->spb_verse),
			   "button_release_event",
			   GTK_SIGNAL_FUNC
			   (verse_button_release_event), vc);
	gtk_signal_connect(GTK_OBJECT(vc->freeform_lookup),
			   "key_press_event",
			   GTK_SIGNAL_FUNC(entry_key_press_event), vc);
	return toolbar_nav;
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

static void create_commentary_dialog(VIEW_COMM * vc, gboolean do_edit)
{
	GtkWidget *vbox30;
	GtkWidget *vbox_toolbars;
	GtkWidget *toolbar_nav;
	GtkWidget *frame19;
	GtkWidget *scrolledwindow38;

	vc->dialog = gtk_window_new(GTK_WINDOW_DIALOG);

	gtk_object_set_data(GTK_OBJECT(vc->dialog), "vc->dialog",
			    vc->dialog);
	gtk_window_set_title(GTK_WINDOW(vc->dialog),
			     get_module_description(vc->c->mod_name));
	gtk_window_set_default_size(GTK_WINDOW(vc->dialog), 462, 280);
	gtk_window_set_policy(GTK_WINDOW(vc->dialog), TRUE, TRUE,
			      FALSE);

	vbox30 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox30);

	gtk_container_add(GTK_CONTAINER(vc->dialog), vbox30);

	vbox_toolbars = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox_toolbars);
	gtk_box_pack_start(GTK_BOX(vbox30), vbox_toolbars, FALSE, FALSE,
			   0);

	toolbar_nav = create_nav_toolbar(vc);
	gtk_widget_show(toolbar_nav);
	gtk_box_pack_start(GTK_BOX(vbox_toolbars), toolbar_nav, FALSE,
			   FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar_nav),
				      GTK_RELIEF_NONE);

	frame19 = gtk_frame_new(NULL);
	gtk_widget_show(frame19);
	gtk_box_pack_start(GTK_BOX(vbox30), frame19, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame19), GTK_SHADOW_IN);

	scrolledwindow38 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow38);
	gtk_container_add(GTK_CONTAINER(frame19), scrolledwindow38);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow38),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);


	if (do_edit) {
		vc->ec->htmlwidget = gtk_html_new();
		vc->ec->html = GTK_HTML(vc->ec->htmlwidget);
		gtk_widget_show(vc->ec->htmlwidget);
		gtk_container_add(GTK_CONTAINER(scrolledwindow38),
				  vc->ec->htmlwidget);
		gtk_html_load_empty(vc->ec->html);

		vc->ec->vbox = vbox30;

		vc->ec->pm = gui_create_editor_popup(vc->ec);
		gnome_popup_menu_attach(vc->ec->pm, vc->ec->htmlwidget,
					NULL);

		vc->ec->statusbar = gtk_statusbar_new();
		gtk_widget_show(vc->ec->statusbar);
		gtk_box_pack_start(GTK_BOX(vbox30), vc->ec->statusbar,
				   FALSE, TRUE, 0);
		/*
		   gtk_signal_connect(GTK_OBJECT(vc->ec->html), "submit",
		   GTK_SIGNAL_FUNC(on_submit), vc->ec);
		   gtk_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "load_done",
		   GTK_SIGNAL_FUNC(html_load_done), vc->ec);
		   gtk_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "key_press_event",
		   GTK_SIGNAL_FUNC(html_key_pressed), vc->ec);
		   gtk_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "button_press_event",
		   GTK_SIGNAL_FUNC(html_button_pressed), vc->ec);
		   gtk_signal_connect(GTK_OBJECT(vc->ec->htmlwidget),
		   "enter_notify_event",
		   GTK_SIGNAL_FUNC(on_html_enter_notify_event),
		   vc->ec);
		 */
		/* html.c */
		gtk_signal_connect(GTK_OBJECT(vc->ec->htmlwidget),
				   "link_clicked",
				   GTK_SIGNAL_FUNC(gui_link_clicked),
				   NULL);
		/* html.c */
		gtk_signal_connect(GTK_OBJECT(vc->ec->htmlwidget),
				   "on_url", GTK_SIGNAL_FUNC(gui_url),
				   NULL);

		gui_toolbar_style(vc->ec);
		gtk_box_pack_start(GTK_BOX(vbox_toolbars),
				   vc->ec->toolbar_style, FALSE, FALSE,
				   0);
		gtk_widget_hide(vc->ec->toolbar_style);
		gui_toolbar_edit(vc->ec);
		gtk_box_pack_start(GTK_BOX(vbox_toolbars),
				   vc->ec->toolbar_edit, FALSE, FALSE,
				   0);
		gtk_widget_hide(vc->ec->toolbar_edit);

	} else {
		vc->c->html = gtk_html_new();
		gtk_widget_show(vc->c->html);
		gtk_container_add(GTK_CONTAINER(scrolledwindow38),
				  vc->c->html);
		gtk_html_load_empty(GTK_HTML(vc->c->html));

		gtk_signal_connect(GTK_OBJECT(vc->c->html),
				   "link_clicked",
				   GTK_SIGNAL_FUNC(gui_link_clicked),
				   vc);
		gtk_signal_connect(GTK_OBJECT(vc->c->html), "on_url",
				   GTK_SIGNAL_FUNC(dialog_url), vc);
		gtk_signal_connect(GTK_OBJECT(vc->c->html),
				   "button_press_event",
				   GTK_SIGNAL_FUNC(button_press_event),
				   vc);
	}


	gtk_signal_connect(GTK_OBJECT(vc->dialog), "destroy",
			   GTK_SIGNAL_FUNC(on_dialog_destroy), vc);
	gtk_signal_connect(GTK_OBJECT(vc->dialog),
			   "motion_notify_event",
			   GTK_SIGNAL_FUNC
			   (on_dialog_motion_notify_event), vc);
}


/******************************************************************************
 * Name
 *   gui_display_commentary_in_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_display_commentary_in_dialog(gchar * key)	
 *
 * Description
 *    display new key in current commentary
 *
 * Return value
 *   void
 */

void gui_display_commentary_in_dialog(gchar * key)
{
	strcpy(settings.comm_key, key);
	strcpy(cur_vc->c->key, key);
	display(cur_vc, key);
}

/******************************************************************************
 * Name
 *   gui_open_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_open_commentary_dialog(gint mod_num)	
 *
 * Description
 *   create a commentary dialog by calling create_commentary_dialog()
 *   or bring an already existing dialog to the top
 *
 * Return value
 *   void
 */

void gui_open_commentary_dialog(gchar * mod_name)
{
	VIEW_COMM *vc;
	GtkWidget *popupmenu;

	vc = g_new(VIEW_COMM, 1);

	vc->c = g_new(COMM_DATA, 1);
	vc->c->cgs = g_new(COMM_GLOBALS, 1);
	vc->ec = NULL;
	vc->dialog = NULL;
	vc->c->mod_name = g_strdup(mod_name);
	vc->c->book_heading = FALSE;
	vc->c->chapter_heading = FALSE;
	vc->c->is_dialog = TRUE;
	vc->c->is_percomm = is_personal_comment(vc->c->mod_name);
	dialog_list = g_list_append(dialog_list, (VIEW_COMM *) vc);
	create_commentary_dialog(vc, FALSE);

	if (has_cipher_tag(vc->c->mod_name)) {
		vc->c->is_locked = module_is_locked(vc->c->mod_name);
		vc->c->cipher_old = get_cipher_key(vc->c->mod_name);
	} else {
		vc->c->is_locked = 0;
		vc->c->cipher_old = NULL;
	}

	popupmenu = gui_create_pm_comm(vc->c);
	gnome_popup_menu_attach(popupmenu, vc->c->html, NULL);
	sync_with_main(vc);
	gtk_widget_show(vc->dialog);
}


/******************************************************************************
 * Name
 *   gui_open_commentary_editor
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_open_commentary_editor(gint mod_num)	
 *
 * Description
 *   create a commentary editor by calling create_commentary_dialog(vc,true)
 *   or bring an already existing editor to the top
 *
 * Return value
 *   void
 */

void gui_open_commentary_editor(gchar * mod_name)
{
	VIEW_COMM *vc;

	GList *tmp = g_list_first(dialog_list);
	while (tmp != NULL) {
		vc = (VIEW_COMM *) tmp->data;
		if (!strcmp(vc->c->mod_name, mod_name)) {
			gtk_entry_set_text(GTK_ENTRY(vc->entry_key),
					   settings.currentverse);
			display(vc, settings.currentverse);
			gdk_window_raise(GTK_WIDGET(vc->dialog)->
					 window);
			return;
		}
		tmp = g_list_next(tmp);
	}

	vc = g_new(VIEW_COMM, 1);
	vc->ec = gs_html_editor_control_data_new();
	vc->dialog = NULL;
	vc->c->is_dialog = TRUE;
	vc->c->mod_name = g_strdup(mod_name);

	vc->ec->stylebar =
	    load_percomm_options(vc->c->mod_name, "Style bar");
	vc->ec->editbar =
	    load_percomm_options(vc->c->mod_name, "Edit bar");
	vc->ec->personal_comments = TRUE;
	strcpy(vc->ec->filename, vc->c->mod_name);
	vc->c->book_heading = FALSE;
	vc->c->chapter_heading = FALSE;
	dialog_list = g_list_append(dialog_list, (VIEW_COMM *) vc);
	create_commentary_dialog(vc, TRUE);
	sync_with_main(vc);

	if (vc->ec->stylebar)
		gtk_widget_show(vc->ec->toolbar_style);
	if (vc->ec->editbar)
		gtk_widget_show(vc->ec->toolbar_edit);
	gtk_widget_show(vc->dialog);
	gtk_widget_set_sensitive(vc->ec->toolbars, TRUE);
	gtk_html_set_editable(GTK_HTML(vc->ec->html), TRUE);
	settings.percomm_dialog_exist = TRUE;
}


/******************************************************************************
 * Name
 *   gui_setup_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_setup_commentary_dialog(GList *mods)	
 *
 * Description
 *   setup support for commentary dialogs.
 *   create a list of structures that will allow user to display a
 *   dialog for each commentary installed.
 *
 * Return value
 *   void
 */

void gui_setup_commentary_dialog(GList * mods)
{
	dialog_list = NULL;
	dialog_freed = FALSE;
	settings.percomm_dialog_exist = FALSE;
	cur_vc = g_new(VIEW_COMM, 1);
}


/******************************************************************************
 * Name
 *  gui_shutdown_commentary_dialog
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *  void gui_shutdown_commentary_dialog(void)	
 *
 * Description
 *   shut down commentary dialog support and clean mem
 *
 * Return value
 *   void
 */

void gui_shutdown_commentary_dialog(void)
{

	dialog_list = g_list_first(dialog_list);
	while (dialog_list != NULL) {
		VIEW_COMM *vc = (VIEW_COMM *) dialog_list->data;
		dialog_freed = TRUE;
//              g_warning("shuting down %s dialog",vc->mod_name);
		if (vc->ec)
			gui_html_editor_control_data_destroy(NULL,
							     vc->ec);
		/* 
		 *  destroy any dialogs created 
		 */
		if (vc->dialog)
			gtk_widget_destroy(vc->dialog);
		/* 
		 * free each COMM_DATA item created 
		 */
		g_free(vc->c->mod_name);
		g_free(vc->c->cgs);
		g_free(vc->c);
		g_free((VIEW_COMM *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	}
	g_list_free(dialog_list);
	g_free(cur_vc);
}
