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
#include "gui/dialog.h"
#include "gui/html.h"
#include "gui/gnomesword.h"
#include "gui/editor_menu.h"
#include "gui/main_window.h"
#include "gui/sidebar.h"
#include "gui/toolbar_style.h"
#include "gui/toolbar_edit.h"
#include "gui/widgets.h"

#include "main/commentary.h"
#include "main/percomm.h"
#include "main/sword.h"
#include "main/lists.h"
#include "main/settings.h"
#include "main/key.h"


extern gboolean dialog_freed;

/****************************************************************************************
 * static - global to this file only
 */

DIALOG_DATA *cur_vc;
static gboolean apply_change;




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

void gui_display_commentary_with_struct(DIALOG_DATA * c, gchar * key)
{
	strcpy(settings.comm_key, key);
	if(c->key)
		g_free(c->key);
	c->key = g_strdup(key);
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

void gui_note_can_close(GSHTMLEditorControlData *ecd)
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
			_("has been modified. Do you wish to save it?"));
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			gui_save_note(ecd);
		}
		settings.modifiedPC = FALSE;
		g_free(info);
		g_string_free(str,TRUE);
	}
}

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
    (GtkMenuItem * menuitem, gchar * dict_mod_description) 
{
	gchar *dict_key = NULL;
	gchar *mod_name = NULL;

	mod_name = module_name_from_description(dict_mod_description);
	if(!mod_name) 
		return;

	dict_key = gui_get_word_or_selection(cur_vc->html, FALSE);
	if (dict_key && mod_name) {
		if (settings.inViewer)
			gui_display_dictlex_in_sidebar(mod_name,
						      dict_key);
		if (settings.inDictpane)
			main_display_dictionary(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
	}
}

/******************************************************************************
 * Name
 *  display
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void display(DIALOG_DATA *c, gchar * key)	
 *
 * Description
 *   calls entry_display to display a commentary entry 
 *
 * Return value
 *   void
 */

static void display(DIALOG_DATA * vc, gchar * key)
{
	gchar *text_str = NULL;
	gchar *strkey;
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) vc->editor;

	cur_vc = vc;

	strkey = get_valid_key(key);
	if(vc->key)
		g_free(vc->key);
	vc->key = g_strdup(strkey);

	if (vc->book_heading) {
		vc->book_heading = FALSE;
		text_str = get_book_heading(vc->mod_name, strkey);
	}

	else if (vc->chapter_heading) {
		vc->chapter_heading = FALSE;
		text_str = get_chapter_heading(vc->mod_name, strkey);
	}

	else {
		if (ec) {
			settings.percomverse = key;
			change_percomm_module(vc->mod_name);
			strcpy(ec->key, key);
			text_str = get_percomm_text(key);
			if (text_str) {
				entry_display(ec->htmlwidget,
					      vc->mod_name,
					      text_str, key, FALSE);
				free(text_str);
				gui_update_statusbar(ec);
				return;
			}
		} else {
			text_str =
			    get_commentary_text(vc->mod_name,
						strkey);
		}
	}

	if (text_str) {
		entry_display(vc->html, vc->mod_name,
			      text_str, key, TRUE);
		free(text_str);
	}
}



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

static void on_dialog_destroy(GtkObject * object, DIALOG_DATA * vc)
{	
	if (!dialog_freed)
		main_free_on_destroy(vc);
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
 *   void gui_close_comm_dialog(DIALOG_DATA * vc)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_close_comm_dialog(DIALOG_DATA * vc)
{
	if (vc->dialog) {
		dialog_freed = FALSE;
		gtk_widget_destroy(vc->dialog);
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
                      GdkEventMotion  *event, DIALOG_DATA * vc)	
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean on_dialog_motion_notify_event(GtkWidget * widget,
					      GdkEventMotion * event,
					      DIALOG_DATA * vc)
{
	cur_vc = vc;
	return FALSE;
}



/******************************************************************************
 * Name
 *   dialog_url
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void dialog_url(GtkHTML * html, const gchar * url, DIALOG_DATA * vc)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_url(GtkHTML * html, const gchar * url,
		       DIALOG_DATA * vc)
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
 *					DIALOG_DATA * vc)	
 *
 * Description
 *    mouse button pressed in window - used to set cur_vc to the current
 *    commentary dialog structure
 *
 * Return value
 *   gint
 */

static gint button_press_event(GtkWidget * html,
			       GdkEventButton * event, DIALOG_DATA * vc)
{
	cur_vc = vc;
	return FALSE;
}
static gboolean html_key_press_event(GtkWidget * widget,
				      GdkEventKey * event,
				      DIALOG_DATA * vc)
{
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) vc->editor;

	ec->changed = TRUE;
	//g_warning("html_key_press_event");
	gui_update_statusbar(ec);
	return FALSE;
	
}

void html_cursor_move(GtkHTML *html, GtkDirectionType dir_type, 
				     GtkHTMLCursorSkipType skip)
{
	g_warning("html_cursor_move");
}
/******************************************************************************
 * Name
 *   update_controls
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void update_controls(DIALOG_DATA * vc)	
 *
 * Description
 *   update the book, chapter and verse contorls
 *
 * Return value
 *   void
 */

static void update_controls(DIALOG_DATA * vc)
{
	gchar *val_key;
	gint cur_chapter, cur_verse;

	cur_vc = vc;
	apply_change = FALSE;
	val_key = get_valid_key(vc->key);
	//vc->key = vc->key;
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

static void book_changed(GtkEditable * editable, DIALOG_DATA * vc)
{
	gchar buf[256], *val_key;
	gchar *bookname = NULL;
	if (apply_change) {
		bookname = gtk_editable_get_chars(editable, 0, -1);
		if (*bookname) {			
			sprintf(buf, "%s 1:1", bookname);
			val_key = get_valid_key(buf);
			display(vc, val_key);
			if(vc->key)
				g_free(vc->key);
			vc->key = g_strdup(val_key);
			update_controls(vc);
			g_free(val_key);
		}
		
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
					     DIALOG_DATA * vc)
{
	if (apply_change) {
		const gchar *bookname;
		gchar *val_key;
		gchar buf[256];
		gint chapter;

		bookname = gtk_entry_get_text(GTK_ENTRY(vc->cbe_book));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (vc->spb_chapter));
		sprintf(buf, "%s %d:1", bookname, chapter);
		val_key = get_valid_key(buf);
		display(vc, val_key);
		if(vc->key)
			g_free(vc->key);
		vc->key = g_strdup(val_key);
		update_controls(vc);
		g_free(val_key);
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
					   DIALOG_DATA * vc)
{
	if (apply_change) {
		const gchar *bookname;
		gchar buf[256], *val_key;
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
		if(vc->key)
			g_free(vc->key);
		vc->key = g_strdup(val_key);
		update_controls(vc);
		g_free(val_key);
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
				      DIALOG_DATA * vc)
{
	/* if <enter> key */
	if (event->keyval == 65293 || event->keyval == 65421) {
		const gchar *buf;
		gchar *val_key;
		buf = gtk_entry_get_text(GTK_ENTRY(widget));
		val_key = get_valid_key(buf);
		display(vc, val_key);
		if(vc->key)
			g_free(vc->key);
		vc->key = g_strdup(val_key);
		update_controls(vc);
		g_free(val_key);
	}
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

static void sync_with_main(DIALOG_DATA * vc)
{
	if(vc->key)
		g_free(vc->key);
	vc->key = g_strdup(settings.currentverse);
	update_controls(vc);
	display(vc, vc->key);
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

static void sync_toggled(GtkToggleButton * button, DIALOG_DATA * vc)
{
	if(button->active) {
		sync_with_main(vc);
		vc->sync = TRUE;
	}
	else		
		vc->sync = FALSE;
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

static GtkWidget *create_nav_toolbar(DIALOG_DATA * vc)
{
	GtkWidget *toolbar_nav;
	GtkWidget *sync_button;
	GtkWidget *cbBook;
	GtkObject *spbChapter_adj;
	GtkObject *spbVerse_adj;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator;
	GtkWidget *label;

	toolbar_nav =
	    gtk_toolbar_new();
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar_nav), GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbar_nav);
	gtk_widget_set_size_request(toolbar_nav, -1, 34);
	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_REFRESH, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar_nav)));
	sync_button =
		gtk_toolbar_append_element(GTK_TOOLBAR
				       (toolbar_nav),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Sync"),
				       _("Toggle sync with main window"), 
				       NULL,
				       tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(sync_button);


	vseparator = gtk_vseparator_new();
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vseparator, NULL, NULL);
	gtk_widget_set_size_request(vseparator, 6, 7);
	
	
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
	gtk_widget_set_size_request(vc->cbe_book, 100, -1);
	gtk_widget_show(vc->cbe_book);

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar_nav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	spbChapter_adj = gtk_adjustment_new(8, -1, 151, 1, 10, 10);
	vc->spb_chapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbChapter_adj), 1, 0);
	gtk_widget_show(vc->spb_chapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vc->spb_chapter, NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON
				    (vc->spb_chapter), TRUE);
				    
	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar_nav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	spbVerse_adj = gtk_adjustment_new(28, -1, 180, 1, 10, 10);
	vc->spb_verse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVerse_adj), 1, 0);
	gtk_widget_show(vc->spb_verse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vc->spb_verse, NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(vc->spb_verse),
				    TRUE);

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar_nav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	vc->freeform_lookup = gtk_entry_new();
	gtk_widget_show(vc->freeform_lookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar_nav),
				  vc->freeform_lookup, NULL, NULL);
	gtk_entry_set_text(GTK_ENTRY(vc->freeform_lookup),
			   _("Romans 8:28"));

	g_signal_connect(GTK_OBJECT(vc->cbe_book),
			   "changed",
			   G_CALLBACK(book_changed), vc);

	g_signal_connect(GTK_OBJECT(sync_button),
			   "toggled",
			   G_CALLBACK(sync_toggled), vc);
			   
	g_signal_connect(GTK_OBJECT(vc->spb_chapter),
			   "button_release_event",
			   G_CALLBACK
			   (chapter_button_release_event), vc);
	g_signal_connect(GTK_OBJECT(vc->spb_verse),
			   "button_release_event",
			   G_CALLBACK
			   (verse_button_release_event), vc);
	g_signal_connect(GTK_OBJECT(vc->freeform_lookup),
			   "key_press_event",
			   G_CALLBACK(entry_key_press_event), vc);
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

void gui_create_commentary_dialog(DIALOG_DATA * vc, gboolean do_edit)
{
	GtkWidget *vbox30;
	GtkWidget *vbox_toolbars;
	GtkWidget *toolbar_nav;
	GtkWidget *frame19;
	GtkWidget *scrolledwindow38;	
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) vc->editor;

	vc->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_object_set_data(GTK_OBJECT(vc->dialog), "vc->dialog",
			    vc->dialog);
	gtk_window_set_title(GTK_WINDOW(vc->dialog),
			     get_module_description(vc->mod_name));
	//gtk_window_set_default_size(GTK_WINDOW(vc->dialog), 462, 280);
	gtk_window_set_policy(GTK_WINDOW(vc->dialog), TRUE, TRUE,
			      FALSE);
	if (do_edit)
		gtk_widget_set_size_request(vc->dialog, 590, 380);
	else
		gtk_widget_set_size_request(vc->dialog, 460, 280);

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
		ec->htmlwidget = gtk_html_new();
		ec->html = GTK_HTML(ec->htmlwidget);
		gtk_widget_show(ec->htmlwidget);
		gtk_container_add(GTK_CONTAINER(scrolledwindow38),
				  ec->htmlwidget);
	
		//gtk_html_set_editable(ec->html, TRUE);
		ec->vbox = vbox30;

		ec->pm = gui_create_editor_popup(ec);
		gnome_popup_menu_attach(ec->pm, ec->htmlwidget,
					NULL);

		ec->statusbar = gtk_statusbar_new();
		gtk_widget_show(ec->statusbar);
		gtk_box_pack_start(GTK_BOX(vbox30), ec->statusbar,
				   FALSE, TRUE, 0);
		/*
		   g_signal_connect(GTK_OBJECT(vc->ec->html), "submit",
		   G_CALLBACK(on_submit), vc->ec);
		   g_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "load_done",
		   G_CALLBACK(html_load_done), vc->ec);
		   g_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "key_press_event",
		   G_CALLBACK(html_key_pressed), vc->ec);
		   g_signal_connect(GTK_OBJECT
		   (vc->ec->htmlwidget),
		   "button_press_event",
		   G_CALLBACK(html_button_pressed), vc->ec);
		   g_signal_connect(GTK_OBJECT(vc->ec->htmlwidget),
		   "enter_notify_event",
		   G_CALLBACK(on_html_enter_notify_event),
		   vc->ec);
		g_signal_connect(G_OBJECT(vc->ec->htmlwidget),
		   		"cursor_move",
		   		G_CALLBACK(html_cursor_move), vc->ec);
		 */
		/* html.c */
		g_signal_connect(G_OBJECT(ec->htmlwidget),
		   		"key_press_event",
		   		G_CALLBACK(html_key_press_event), vc);
		g_signal_connect(GTK_OBJECT(ec->htmlwidget),
				   "link_clicked",
				   G_CALLBACK(gui_link_clicked),
				   NULL);
		
		g_signal_connect(GTK_OBJECT(ec->htmlwidget),
				   "on_url", G_CALLBACK(gui_url),
				   NULL);

		gui_toolbar_style(ec);
		gtk_box_pack_start(GTK_BOX(vbox_toolbars),
				   ec->toolbar_style, FALSE, FALSE,
				   0);
		gtk_widget_show(ec->toolbar_style);
		gui_toolbar_edit(ec);
		gtk_box_pack_start(GTK_BOX(vbox_toolbars),
				   ec->toolbar_edit, FALSE, FALSE,
				   0);
		gtk_widget_show(ec->toolbar_edit);

	} else {
		vc->html = gtk_html_new();
		gtk_widget_show(vc->html);
		gtk_container_add(GTK_CONTAINER(scrolledwindow38),
				  vc->html);
		gtk_html_load_empty(GTK_HTML(vc->html));

		g_signal_connect(GTK_OBJECT(vc->html),
				   "link_clicked",
				   G_CALLBACK(gui_link_clicked),
				   vc);
		g_signal_connect(GTK_OBJECT(vc->html), "on_url",
				   G_CALLBACK(dialog_url), vc);
		g_signal_connect(GTK_OBJECT(vc->html),
				   "button_press_event",
				   G_CALLBACK(button_press_event),
				   vc);
	}


	g_signal_connect(GTK_OBJECT(vc->dialog), "destroy",
			   G_CALLBACK(on_dialog_destroy), vc);
	g_signal_connect(GTK_OBJECT(vc->dialog),
			   "motion_notify_event",
			   G_CALLBACK
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
	/*strcpy(settings.comm_key, key);
		if(vc->key)
			g_free(vc->key);
		vc->key = g_strdup(val_key);
	display(cur_vc, key);*/
}


/******************************************************************************
 * Name
 *   gui_commentary_dialog_goto_bookmark
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_commentary_dialog_goto_bookmark(gchar * mod_name, gchar * key)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_commentary_dialog_goto_bookmark(gchar * mod_name, gchar * key)
{
/*	GList *tmp = NULL;
	tmp = g_list_first(dialog_list);
	while (tmp != NULL) {
		DIALOG_DATA *vc = (DIALOG_DATA *) tmp->data;
		if(!strcmp(vc->mod_name, mod_name)) {
			if(vc->key)
				g_free(vc->key);
			vc->key = g_strdup(key);
			display(vc, vc->key);
			update_controls(vc);
			gdk_window_raise(vc->dialog->window);
			return;
		}		
		tmp = g_list_next(tmp);
	}
*/
/*	if(is_personal_comment(mod_name))
		gui_open_commentary_editor(mod_name);
	else
		gui_open_commentary_dialog(mod_name);*/
	//strcpy(cur_vc->key, key);
//	display(cur_vc, cur_vc->key);
//	update_controls(cur_vc);
}


/******************************************************************************
 * Name
 *   gui_keep_comm_dialog_in_sync
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_keep_comm_dialog_in_sync(gchar * key)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_keep_comm_dialog_in_sync(gchar * key)
{
/*	GList *tmp = NULL;
	tmp = g_list_first(dialog_list);
	while (tmp != NULL) {
		DIALOG_DATA *vc = (DIALOG_DATA *) tmp->data;
		if(vc->sync) {
			sync_with_main(vc);
		}
		tmp = g_list_next(tmp);
	}
*/
}
