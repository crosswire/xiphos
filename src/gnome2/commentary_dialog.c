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

//#include "gui/gtkhtml_display.h"
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


static void create_menu(DIALOG_DATA * t ,GdkEventButton * event);

extern gboolean dialog_freed;

/****************************************************************************************
 * static - global to this file only
 */

DIALOG_DATA *cur_vc;
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
//			gui_save_note(ecd);
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
			main_sidebar_display_dictlex(mod_name,
						      dict_key);
		if (settings.inDictpane)
			main_display_dictionary(mod_name, dict_key);
		g_free(dict_key);
		g_free(mod_name);
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
			       GdkEventButton * event, DIALOG_DATA * c)
{
	cur_vc = c;
	switch (event->button) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		create_menu(c, event);
		break;
	}
	return FALSE;
}
static gboolean html_key_press_event(GtkWidget * widget,
				      GdkEventKey * event,
				      DIALOG_DATA * vc)
{
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) vc->editor;
	cur_vc = vc;
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

static void book_changed(GtkEditable * editable, DIALOG_DATA * c)
{
	gchar *url;
	gchar *bookname = gtk_editable_get_chars(editable, 0, -1);
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) c->editor;
	if (*bookname) {
		url = g_strdup_printf("sword:///%s 1:1", bookname);
		main_dialogs_url_handler(c, url, TRUE);
		g_free(url);
	}
	if(!ec)
		return;
	if(ec->key)
		g_free(ec->key);
	ec->key = g_strdup_printf("%s 1:1", bookname);
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
					     DIALOG_DATA * c)
{
	G_CONST_RETURN gchar *bookname;
	gchar *url;
	gchar *val_key;
	gint chapter;
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) c->editor;

	bookname = (gchar*)gtk_entry_get_text(GTK_ENTRY(c->cbe_book));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (c->spb_chapter));
	url = g_strdup_printf("sword:///%s %d:1", bookname, chapter);
	main_dialogs_url_handler(c, url, TRUE);
	g_free(url);	
	
	if(!ec)
		return;
	if(ec->key)
		g_free(ec->key);
	ec->key = g_strdup_printf("%s %d:1", bookname, chapter);
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
					   DIALOG_DATA * c)
{
	G_CONST_RETURN gchar *bookname;
	gchar *url;
	gchar *val_key;
	gint chapter, verse;
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) c->editor;

	bookname = (gchar*)gtk_entry_get_text(GTK_ENTRY(c->cbe_book));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (c->spb_chapter));
	verse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (c->spb_verse));
	url = g_strdup_printf("sword:///%s %d:%d", bookname, chapter, verse);
	main_dialogs_url_handler(c, url, TRUE);
	g_free(url);
	
	if(!ec)
		return;
	if(ec->key)
		g_free(ec->key);
	ec->key = g_strdup_printf("%s %d:%d", bookname, chapter, verse);
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

static void sync_toggled(GtkToggleButton * button, DIALOG_DATA * c)
{
	if(button->active) {
		sync_with_main(cur_vc);
		cur_vc->sync = TRUE;
	}
	else		
		cur_vc->sync = FALSE;
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
			     main_get_module_description(vc->mod_name));
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


/** Commentary menu stuff
 **
 **
 **
 **
 **
 **
 **/

static void on_about_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_display_about_module_dialog(cur_vc->mod_name, FALSE);
}

static void on_print1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_html_print(cur_vc->html, FALSE);
}


static void on_copy2_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_copy_html(cur_vc->html);
}


static void on_find1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gui_find_dlg(cur_vc->html, cur_vc->mod_name, FALSE, NULL);
}


static void
on_set_module_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gchar *url = g_strdup_printf(	"sword://%s/%s",
					cur_vc->mod_name,
					settings.currentverse);
	gui_set_module_font(cur_vc->mod_name);
	main_dialogs_url_handler(cur_vc, url, TRUE);
	g_free(url);
}


static void on_use_current_dictionary_activate(GtkMenuItem * menuitem,
				   		gpointer user_data)
{
	gchar *dict_key = gui_get_word_or_selection(cur_vc->html, FALSE);
	if (dict_key) {
		if (settings.inViewer)
			main_sidebar_display_dictlex(settings.
						      DictWindowModule,
						      dict_key);
		if (settings.inDictpane)
			main_display_dictionary(settings.
						  DictWindowModule,
						  dict_key);
		g_free(dict_key);
	}
}


static void
on_unlock_module_activate(GtkMenuItem * menuitem, gpointer user_data)
{
/*	gchar *cipher_key;
	gchar *cipher_old;

	cipher_old = get_cipher_key(settings.MainWindowModule);
	cipher_key = gui_add_cipher_key(settings.MainWindowModule, cipher_old);
	if (cipher_key) {
		main_locked_module_display(widgets.html_text,
					     settings.MainWindowModule,
					     cipher_key);
	}
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

	mod_name = module_name_from_description(dict_mod_description);
	dict_key = gui_get_word_or_selection(cur_vc->html, FALSE);
	if (dict_key && mod_name) {
		if (settings.inViewer)
			main_sidebar_display_dictlex(mod_name,
						      dict_key);
		if (settings.inDictpane)
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
	gchar *module_name = module_name_from_description((gchar *)user_data);
	gchar *url = NULL;
	if(module_name) {
		url = g_strdup_printf("sword://%s/%s",
					module_name,
					cur_vc->key);
		main_dialogs_url_handler(cur_vc, url, TRUE);
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
	//close_text_dialog(cur_vc);
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
	sync_with_main(cur_vc);
}


static void stay_in_sync_activate(GtkCheckMenuItem * menuitem, gpointer data)
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
	 (gpointer) stay_in_sync_activate, NULL, NULL,
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
	
	cur_vc = t;
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
				       t->sync);


	view_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file3_menu_uiinfo[0].widget),
				  view_menu);

	gui_add_mods_2_gtk_menu(COMM_DESC_LIST, view_menu,
				(GCallback) on_view_mod_activate);

	edit_per_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit3_menu_uiinfo[2].widget),
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
				(GCallback)lookup_commentary_selection);


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
	if(main_has_cipher_tag(mod_name))
		gtk_widget_show(menu1_uiinfo[6].widget);


	/*
	 * menu1_uiinfo[0].widget, "about");
	 * menu1_uiinfo[1].widget, "separator4");
	 * menu1_uiinfo[2].widget, "file3");
	 * file3_menu_uiinfo[0].widget, "view_text");
	 * view_text_menu_uiinfo[0].widget, "item1");
	 * file3_menu_uiinfo[1].widget, "separator8");
	 * file3_menu_uiinfo[2].widget, "print1");
	 * menu1_uiinfo[3].widget, "edit3");
	 * edit3_menu_uiinfo[0].widget, "copy2");
	 * edit3_menu_uiinfo[1].widget, "find1");
	 * edit3_menu_uiinfo[2].widget, "note");
	 * note_menu_uiinfo[0].widget, "item2");
	 * menu1_uiinfo[4].widget, "module_options");
	 * module_options_menu_uiinfo[0].widget, "set_module_font");
	 * module_options_menu_uiinfo[1].widget, "separator5");
	 * menu1_uiinfo[5].widget, "lookup_selection");
	 * lookup_selection_menu_uiinfo[0].widget, "use_current_dictionary");
	 * lookup_selection_menu_uiinfo[1].widget, "separator6");
	 * menu1_uiinfo[7].widget, "separator7");
	 * menu1_uiinfo[8].widget, "show_tabs");
	 */
	gnome_popup_menu_do_popup_modal(menu1, NULL,
					NULL, event, NULL,
					t->html);
	gtk_widget_destroy(menu1);
	//g_free(ops);
}





/******   end of file   ******/
