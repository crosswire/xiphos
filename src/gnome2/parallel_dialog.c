/*
 * GnomeSword Bible Study Tool
 * parallel_dialog.c - dialog for detached parallel
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
#  include <config.h>
#endif

#include <gnome.h>

#include "gui/parallel_dialog.h"
#include "gui/parallel_view.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/lists.h"
#include "main/key.h"
#include "main/settings.h"

/******************************************************************************
 * static
 */
static GtkWidget *parallel_UnDock_Dialog;
static GtkWidget *vboxInt;
static GtkWidget *entrycbIntBook;
static GtkWidget *sbIntChapter;
static GtkWidget *sbIntVerse;
static GtkWidget *entryIntLookup;
static gboolean ApplyChangeBook;


/******************************************************************************
 * Name
 *   change_verse_parallel
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   gchar *change_verse_parallel(void)
 *
 * Description
 *   
 *
 *
 *   
 *
 * Return value
 *   gchar *
 */

static gchar *change_verse_parallel(void)
{
	gchar *retval;
	const gchar *bookname;
	gchar buf[256];
	gint chapter, verse;
	char *newbook;

	bookname = gtk_entry_get_text(GTK_ENTRY(entrycbIntBook));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (sbIntChapter));
	verse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (sbIntVerse));

	sprintf(buf, "%s %d:%d", bookname, chapter, verse);

	newbook = get_book_from_key(buf);
	chapter = get_chapter_from_key(buf);
	verse = get_verse_from_key(buf);

	if (strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(entrycbIntBook), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter),
				  chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), verse);
	sprintf(buf, "%s %d:%d", newbook, chapter, verse);
	gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
	retval = buf;
	g_free(newbook);
	return retval;
}

/******************************************************************************
 * Name
 *   update_controls_parallel
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   gchar *update_controls_parallel(gchar * ref)
 *
 * Description
 *   
 *
 *
 *   
 *
 * Return value
 *   gchar*
 */

static gchar *update_controls_parallel(const gchar * ref)
{
	const gchar *bookname;
	gchar buf[256];
	gint chapter, verse;
	char *newbook;

	newbook = get_book_from_key(ref); 
	chapter = get_chapter_from_key(ref);
	verse = get_verse_from_key(ref);

	bookname = gtk_entry_get_text(GTK_ENTRY(entrycbIntBook));
	if (strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(entrycbIntBook), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter),
				  chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), verse);
	sprintf(buf, "%s %d:%d", newbook, chapter, verse);
	gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
	g_free(newbook);
	return g_strdup(buf);
}

/******************************************************************************
 * Name
 *   undock_parallel_page
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   void undock_parallel_page(void)
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

void gui_undock_parallel_page(void)
{
	ApplyChangeBook = FALSE;
	parallel_UnDock_Dialog = gui_create_parallel_dialog();
	gtk_widget_reparent(widgets.frame_parallel, vboxInt);
	gtk_notebook_remove_page(GTK_NOTEBOOK(widgets.notebook_parallel_text),
				 1);
	settings.cvparallel =
	    update_controls_parallel(settings.currentverse);
	gtk_widget_show(parallel_UnDock_Dialog);
	gui_update_parallel_page_detached();
	g_free(settings.cvparallel);
	ApplyChangeBook = TRUE;
}

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

void gui_btnDockInt_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(parallel_UnDock_Dialog);
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

static void on_dlgparallel_destroy(GtkObject * object,
				      gpointer user_data)
{
	GtkWidget *tab_label, *menu_label, *vbox;
	tab_label = gtk_label_new(_("Parallel View"));
	gtk_widget_show(tab_label);
	menu_label = gtk_label_new(_("Parallel View"));
	gtk_widget_show(menu_label);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_notebook_insert_page_menu(GTK_NOTEBOOK
				      (widgets.notebook_parallel_text), vbox,
				      tab_label, menu_label, 
					1);
	gtk_widget_reparent(widgets.frame_parallel, vbox);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_parallel_text), 
					1);
	settings.dockedInt = TRUE;
	gui_update_parallel_page();
}

/******************************************************************************
 * Name
 *   on_buttonIntSync_clicked
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   void on_buttonIntSync_clicked(GtkButton * button, gpointer user_data)
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

static void on_buttonIntSync_clicked(GtkButton * button,
				     gpointer user_data)
{
	ApplyChangeBook = FALSE;
	settings.cvparallel =
	    update_controls_parallel(settings.currentverse);
	gui_update_parallel_page_detached();
	g_free(settings.cvparallel);
	ApplyChangeBook = TRUE;
}

/******************************************************************************
 * Name
 *   on_entrycbIntBook_changed
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   void on_entrycbIntBook_changed(GtkEditable *editable, gpointer user_data)
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

static void on_entrycbIntBook_changed(GtkEditable * editable,
				      gpointer user_data)
{
	if (ApplyChangeBook) {
		const gchar *bookname;
		char buf[256];
		bookname = gtk_entry_get_text(GTK_ENTRY(editable));
		sprintf(buf, "%s %d:%d", bookname, 1, 1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter),
					  1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse),
					  1);
		gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
		settings.cvparallel = buf;
		gui_update_parallel_page_detached();
	}
}

/******************************************************************************
 * Name
 *   on_sbIntChapter_button_release_event
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   gboolean on_sbIntChapter_button_release_event(GtkWidget *widget,
 *   			GdkEventButton *event, gpointer user_data)
 *
 * Description
 *   
 *
 *
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean on_sbIntChapter_button_release_event(GtkWidget * widget,
						     GdkEventButton *
						     event,
						     gpointer user_data)
{
	ApplyChangeBook = FALSE;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), 1);
	settings.cvparallel = change_verse_parallel();
	gui_update_parallel_page_detached();
	ApplyChangeBook = TRUE;
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_sbIntVerse_button_release_event
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   gboolean on_sbIntVerse_button_release_event(GtkWidget *widget,
 *			GdkEventButton *event, gpointer user_data)
 *
 * Description
 *
 *
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean on_sbIntVerse_button_release_event(GtkWidget * widget,
						   GdkEventButton *
						   event,
						   gpointer user_data)
{
	ApplyChangeBook = FALSE;
	settings.cvparallel = change_verse_parallel();
	gui_update_parallel_page_detached();
	ApplyChangeBook = TRUE;
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_entryIntLookup_key_press_event
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   gboolean on_entryIntLookup_key_press_event(GtkWidget *widget,
 *                                      GdkEventKey *event, gpointer user_data)
 *
 * Description
 *
 *
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean on_entryIntLookup_key_press_event(GtkWidget * widget,
						  GdkEventKey * event,
						  gpointer user_data)
{
	const gchar *buf;
	ApplyChangeBook = FALSE;
	buf = gtk_entry_get_text(GTK_ENTRY(entryIntLookup));
	if (event->keyval == 65293 || event->keyval == 65421) {
		settings.cvparallel =
		    update_controls_parallel(buf); 
		gui_update_parallel_page_detached();
		g_free(settings.cvparallel);
		ApplyChangeBook = TRUE;
	}
	ApplyChangeBook = TRUE;
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_btnIntGotoVerse_clicked
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   void on_btnIntGotoVerse_clicked(GtkButton *button, gpointer user_data)
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

static void on_btnIntGotoVerse_clicked(GtkButton * button,
				       gpointer user_data)
{
	const gchar *buf;		//-- pointer to entry string
	//-- pointer to entry string
	ApplyChangeBook = FALSE;
	buf = gtk_entry_get_text(GTK_ENTRY(entryIntLookup));	//-- set pointer to entry text

	settings.cvparallel = update_controls_parallel(buf);

	gui_update_parallel_page_detached();	//-- change verse to entry text 
	g_free(settings.cvparallel);
	ApplyChangeBook = TRUE;
}

/******************************************************************************
 * Name
 *   create_parallel_dialog
 *
 * Synopsis
 *   #include "parallel_dialog.h"
 *   
 *   GtkWidget *create_parallel_dialog(void)
 *
 * Description
 *   
 *
 *
 *   
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_parallel_dialog(void)
{
	GtkWidget *dialog_parallel;
	GtkWidget *dialog_vbox25;
	GtkWidget *toolbar29;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *buttonIntSync;
	GtkWidget *cbIntBook;
	GtkObject *sbIntChapter_adj;
	GtkObject *sbIntVerse_adj;
	GtkWidget *btnIntGotoVerse;
	GtkWidget *dialog_action_area25;
	GtkWidget *hbuttonbox4;
	GtkWidget *btnDockInt;
	gchar title[256];
	
	sprintf(title,"%s - %s", settings.program_title, _("Parallel"));

	dialog_parallel = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog_parallel),
			    "dialog_parallel", dialog_parallel);
	gtk_window_set_title(GTK_WINDOW(dialog_parallel),
			     title);
	gtk_window_set_default_size(GTK_WINDOW(dialog_parallel), 657,
				    361);
	gtk_window_set_policy(GTK_WINDOW(dialog_parallel), TRUE,
			      TRUE, FALSE);

	dialog_vbox25 = GTK_DIALOG(dialog_parallel)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_parallel),
			    "dialog_vbox25", dialog_vbox25);
	gtk_widget_show(dialog_vbox25);

	vboxInt = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vboxInt);
	gtk_box_pack_start(GTK_BOX(dialog_vbox25), vboxInt, TRUE, TRUE,
			   0);

	toolbar29 =
	    gtk_toolbar_new();
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar29), GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbar29);
	gtk_box_pack_start(GTK_BOX(vboxInt), toolbar29, FALSE, FALSE,
			   0);
/*	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar29),
				      GTK_RELIEF_NONE);*/

	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_REFRESH, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar29)));
	buttonIntSync =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar29),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(buttonIntSync);

	cbIntBook = gtk_combo_new();
	gtk_widget_show(cbIntBook);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), cbIntBook,
				  NULL, NULL);
	gtk_widget_set_size_request(cbIntBook, 154, -1);

	/*** get and load books of the Bible ***/
	gtk_combo_set_popdown_strings(GTK_COMBO(cbIntBook),
				      get_list(BOOKS_LIST));

	entrycbIntBook = GTK_COMBO(cbIntBook)->entry;
	gtk_widget_show(entrycbIntBook);
	gtk_entry_set_text(GTK_ENTRY(entrycbIntBook), _("Romans"));

	sbIntChapter_adj = gtk_adjustment_new(8, 0, 151, 1, 10, 10);
	sbIntChapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(sbIntChapter_adj), 1, 0);
	gtk_widget_show(sbIntChapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), sbIntChapter,
				  NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(sbIntChapter),
				    TRUE);

	sbIntVerse_adj = gtk_adjustment_new(28, 0, 180, 1, 10, 10);
	sbIntVerse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(sbIntVerse_adj), 1, 0);
	gtk_widget_show(sbIntVerse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), sbIntVerse,
				  NULL, NULL);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(sbIntVerse), TRUE);

	entryIntLookup = gtk_entry_new();
	gtk_widget_show(entryIntLookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29),
				  entryIntLookup, NULL, NULL);
	gtk_widget_set_size_request(entryIntLookup, 190, -1);
	gtk_entry_set_text(GTK_ENTRY(entryIntLookup), _("Romans 8:28"));
	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_JUMP_TO, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar29)));
	btnIntGotoVerse =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar29),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Goto verse"),
				       _
				       ("Go to verse in free form lookup and add verse to history"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(btnIntGotoVerse);

	dialog_action_area25 =
	    GTK_DIALOG(dialog_parallel)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog_parallel),
			    "dialog_action_area25",
			    dialog_action_area25);
	gtk_widget_show(dialog_action_area25);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area25), 10);

	hbuttonbox4 = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox4);
	gtk_box_pack_start(GTK_BOX(dialog_action_area25), hbuttonbox4,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox4),
				  GTK_BUTTONBOX_END);

	btnDockInt = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	gtk_widget_show(btnDockInt);
	gtk_container_add(GTK_CONTAINER(hbuttonbox4), btnDockInt);
	GTK_WIDGET_SET_FLAGS(btnDockInt, GTK_CAN_DEFAULT);


	g_signal_connect(GTK_OBJECT(dialog_parallel), "destroy",
			   G_CALLBACK(on_dlgparallel_destroy),
			   NULL);
	g_signal_connect(GTK_OBJECT(buttonIntSync), "clicked",
			   G_CALLBACK(on_buttonIntSync_clicked),
			   NULL);
	g_signal_connect(GTK_OBJECT(entrycbIntBook), "changed",
			   G_CALLBACK(on_entrycbIntBook_changed),
			   NULL);
	g_signal_connect(GTK_OBJECT(sbIntChapter),
			   "button_release_event",
			   G_CALLBACK
			   (on_sbIntChapter_button_release_event),
			   NULL);
	g_signal_connect(GTK_OBJECT(sbIntVerse),
			   "button_release_event",
			   G_CALLBACK
			   (on_sbIntVerse_button_release_event), NULL);
	g_signal_connect(GTK_OBJECT(entryIntLookup),
			   "key_press_event",
			   G_CALLBACK
			   (on_entryIntLookup_key_press_event), NULL);
	g_signal_connect(GTK_OBJECT(btnIntGotoVerse), "clicked",
			   G_CALLBACK(on_btnIntGotoVerse_clicked),
			   NULL);
	g_signal_connect(GTK_OBJECT(btnDockInt), "clicked",
			   G_CALLBACK(gui_btnDockInt_clicked),
			   NULL);
	return dialog_parallel;
}

/******   end of file   ******/
