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
#include <gtkhtml/gtkhtmlfontstyle.h>
#include <gtkhtml/htmlform.h>
#include <gtkhtml/htmlenums.h>
#include <gtkhtml/htmlsettings.h>
#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#ifdef USE_GTKHTML30
#include <gtkhtml/htmllinktext.h>
#include <gal/widgets/e-unicode.h>
#endif
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "gui/editor.h"
#include "gui/note_editor.h"
#include "gui/note_toolbar_edit.h"
#include "gui/note_toolbar_style.h"
#include "gui/editor_replace.h"
#include "gui/editor_spell.h"
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


static GtkWidget *create_menu(DIALOG_DATA * d);

extern gboolean dialog_freed;

static GtkWidget *sync_button;

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
					      DIALOG_DATA * d)
{
	//cur_vc = d;
	return FALSE;
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
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) d->editor;
	//cur_vc = vc;
	ec->changed = TRUE;
	//g_warning("html_key_press_event");
	gui_update_statusbar(ec);
	return FALSE;
	
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
		sync_with_main(c);
		c->sync = TRUE;
	}
	else		
		c->sync = FALSE;
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



void gui_create_note_editor(DIALOG_DATA * d)
{
	GtkWidget *vbox30;
	GtkWidget *vbox_toolbars;
	GtkWidget *toolbar_nav;
	GtkWidget *frame19;
	GtkWidget *scrolledwindow38;	
	GSHTMLEditorControlData *ec = (GSHTMLEditorControlData*) d->editor;

	d->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_object_set_data(GTK_OBJECT(d->dialog), "d->dialog",
			    d->dialog);
	gtk_window_set_title(GTK_WINDOW(d->dialog),
			     main_get_module_description(d->mod_name));
	gtk_window_set_policy(GTK_WINDOW(d->dialog), TRUE, TRUE,
			      FALSE);
	gtk_widget_set_size_request(d->dialog, 590, 380);

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

	scrolledwindow38 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow38);
	gtk_container_add(GTK_CONTAINER(frame19), scrolledwindow38);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow38),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow38,
                                             settings.shadow_type);


	
	ec->htmlwidget = gtk_html_new();
	ec->html = GTK_HTML(ec->htmlwidget);
	gtk_widget_show(ec->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow38),
			  ec->htmlwidget);

	ec->vbox = vbox30;

	ec->pm = create_menu(d);
	gnome_popup_menu_attach(ec->pm, ec->htmlwidget,
				NULL);

	ec->statusbar = gtk_statusbar_new();
	gtk_widget_show(ec->statusbar);
	gtk_box_pack_start(GTK_BOX(vbox30), ec->statusbar,
			   FALSE, TRUE, 0);
	/*
	   g_signal_connect(GTK_OBJECT(d->ec->html), "submit",
	   G_CALLBACK(on_submit), d->ec);
	   g_signal_connect(GTK_OBJECT
	   (d->ec->htmlwidget),
	   "load_done",
	   G_CALLBACK(html_load_done), d->ec);
	   g_signal_connect(GTK_OBJECT
	   (d->ec->htmlwidget),
	   "key_press_event",
	   G_CALLBACK(html_key_pressed), d->ec);
	   g_signal_connect(GTK_OBJECT
	   (d->ec->htmlwidget),
	   "button_press_event",
	   G_CALLBACK(html_button_pressed), d->ec);
	   g_signal_connect(GTK_OBJECT(d->ec->htmlwidget),
	   "enter_notify_event",
	   G_CALLBACK(on_html_enter_notify_event),
	   d->ec);
	g_signal_connect(G_OBJECT(d->ec->htmlwidget),
			"cursor_move",
			G_CALLBACK(html_cursor_move), d->ec);
	 */
	/* html.c */
	g_signal_connect(G_OBJECT(ec->htmlwidget),
			"key_press_event",
			G_CALLBACK(html_key_press_event), d);
	g_signal_connect(GTK_OBJECT(ec->htmlwidget),
			   "link_clicked",
			   G_CALLBACK(gui_link_clicked),
			   NULL);
	
	g_signal_connect(GTK_OBJECT(ec->htmlwidget),
			   "on_url", G_CALLBACK(gui_url),
			   NULL);


	gui_note_toolbar_style(d);	
	gtk_box_pack_start(GTK_BOX(vbox_toolbars),
			   ec->toolbar_style, FALSE, FALSE,
			   0);			   
	gtk_widget_show(ec->toolbar_style);


	gui_note_toolbar_edit(d);
	gtk_box_pack_start(GTK_BOX(vbox_toolbars),
			   ec->toolbar_edit, FALSE, FALSE,
			   0);
	gtk_widget_show(ec->toolbar_edit);



	g_signal_connect(GTK_OBJECT(d->dialog), "destroy",
			   G_CALLBACK(on_dialog_destroy), d);
	g_signal_connect(GTK_OBJECT(d->dialog),
			   "motion_notify_event",
			   G_CALLBACK
			   (on_dialog_motion_notify_event), d);	
}

/***** menu stuff *****/
/******************************************************************************
 * Name
 *  gui_new_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void gui_new_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    ask to save old file if needed then create new
 *
 * Return value
 *   void
 */

static void gui_new_activate(GtkMenuItem * menuitem,
		      DIALOG_DATA * d)
{
	gchar *filename = NULL;
	gchar *buf = NULL;
	gint test;
	GS_DIALOG *info;
	GString *str;
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			buf = settings.studypadfilename;
		else
			buf = N_("File");
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			buf,
			"has been modified. Do you wish to save it?");
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			if (settings.studypadfilename) {
				filename = g_strdup(settings.studypadfilename);
				save_file(filename, ecd);
			} else {
				gui_fileselection_save(ecd,TRUE);
			}
		}
		settings.modifiedSP = FALSE;
		g_free(info);
		g_string_free(str,TRUE);
	}
	
	
	settings.studypadfilename = NULL;
	xml_set_value("GnomeSword", "studypad", "lastfile", 
							NULL);
	
	sprintf(ecd->filename, "%s", "");
	ecd->filename[0] = '\0';
	gtk_html_select_all(ecd->html);
	gtk_html_cut(ecd->html);
	gtk_statusbar_push(GTK_STATUSBAR(ecd->statusbar), 1,
			   _("-untitled-"));
	ecd->changed = FALSE;
}

/******************************************************************************
 * Name
 *  on_open_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_open_activate(GtkMenuItem * menuitem,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open a file into notepad
 *
 * Return value
 *   void
 */

static void on_open_activate(GtkMenuItem * menuitem,
			     DIALOG_DATA * d)
{
	GtkWidget *openFile;
	gchar buf[255];
	gchar *tmp_buf = NULL;
	GString *str;
	gchar *filename = NULL;
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		GS_DIALOG *info;
		gint test;

		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			tmp_buf = settings.studypadfilename;
		else
			tmp_buf = N_("File");
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			tmp_buf,
			"has been modified. Do you wish to save it?");
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_gs_dialog(info);
		if (test == GS_YES) {
			if (settings.studypadfilename) {
				filename = g_strdup(settings.studypadfilename);
				save_file(filename, ecd);
			} else {
				gui_fileselection_save(ecd,TRUE);
			}
		}
		g_free(info);
		g_string_free(str,TRUE);
	}
	sprintf(buf, "%s/*.pad", settings.studypaddir);
	openFile = gui_fileselection_open(ecd);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(openFile),
					buf);
	gtk_widget_show(openFile);
}

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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	if (ecd->personal_comments) {
		main_dialog_save_note(d);
		ecd->changed = FALSE;
		gui_update_statusbar(ecd);
	}
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	if (ecd->personal_comments) {
		if(ecd->changed) {
			main_dialog_save_note(d);
			ecd->changed = FALSE;
			gui_update_statusbar(ecd);
		}
		gui_fileselection_save(ecd,TRUE);
	}
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
			main_dialog_goto_bookmark(ecd->filename, ecd->key);
		}
		settings.percomverse = ecd->key;
		ecd->changed = FALSE;
		gui_update_statusbar(ecd);
		g_free(info);
		g_string_free(str,TRUE);
	}
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	if (ecd->filename) {
		save_file(ecd->filename, ecd);
		return;
	} else {
		gui_fileselection_save(ecd,TRUE);
	}
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gui_fileselection_save(ecd,FALSE);
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gui_fileselection_save(ecd,TRUE);
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gui_html_print(ecd->htmlwidget, FALSE);	/* gs_html.c */
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gtk_html_cut(ecd->html);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gtk_html_copy(ecd->html);
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gtk_html_paste(ecd->html,FALSE);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gtk_html_undo(ecd->html);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	gui_find_dlg(ecd->htmlwidget, ecd->filename,
		  FALSE, NULL);
	//search(ecd, FALSE, NULL);
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	replace(ecd);
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

static void set_link_to_module(gchar * linktext, gchar * linkref, 
			       gchar * linkmod, DIALOG_DATA * d)
{
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
	const gchar *url;
	const gchar *text;
	gchar *target;
	gchar *url_copy;
	gchar buf[256];
//	HTMLEngine *e;
	gchar *new_link = NULL;

//	e = ecd->html->engine;
	
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
		new_link = g_strdup_printf("<a href=\"%s\">%s</a>",url_copy,linktext);
		gtk_html_insert_html(ecd->html,new_link);
		g_free(url_copy);
		g_free(new_link);
	}
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
	GSHTMLEditorControlData * ecd = (GSHTMLEditorControlData *) d->editor;
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
	info->label1 = N_("Link text: ");
	info->text2 = g_strdup("");
	info->label2 = N_("Reference: ");
	info->text3 = g_strdup("");
	info->label3 = N_("Module: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*
	 * get selected text  
	 */
	if (html_engine_is_selection_active(ecd->html->engine)) {
		gchar *buf;
		buf =
		    html_engine_get_selection_string(ecd->html->engine);
		info->text1 = g_strdup(buf);
		info->text2 = g_strdup(buf);
	}
	info->text3 = g_strdup(xml_get_value("modules", "bible"));//settings.MainWindowModule);
	/*** open dialog to get name for list ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		if (strlen(info->text1) > 0) {
			set_link_to_module(info->text1, 
					info->text2, 
					info->text3,
					   d);
			ecd->changed = TRUE;
			gui_update_statusbar(ecd);
		}
	}
	g_free(info->text1);
	g_free(info->text2);
	g_free(info->text3);
	g_free(info);
	g_string_free(str,TRUE);
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
	g_signal_connect(GTK_OBJECT(spell), "activate",
			   G_CALLBACK(spell_check_cb), (DIALOG_DATA *) d);
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
}




/******   end of file   ******/
