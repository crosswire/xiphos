/*
 * GnomeSword Bible Study Tool
 * interlinear_dialog.c - dialog for detached interlinear
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

#include "gui/interlinear_dialog.h"
#include "gui/interlinear.h"
#include "gui/gnomesword.h"

#include "main/sword.h"
//#include "main/gs_gnomesword.h"
#include "main/lists.h"

/******************************************************************************
 * static
 */
static GtkWidget *Interlinear_UnDock_Dialog;
static GtkWidget *vboxInt;
static GtkWidget *entrycbIntBook;
static GtkWidget *sbIntChapter;
static GtkWidget *sbIntVerse;
static GtkWidget *entryIntLookup;
static gboolean ApplyChangeBook;


/******************************************************************************
 * Name
 *   change_verse_interlinear
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
 *   
 *   gchar *change_verse_interlinear(void)
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

static gchar *change_verse_interlinear(void)
{	
	gchar *retval;
	gchar *bookname, buf[256];
	gint chapter, verse;
	const char *newbook;
	
	bookname = 
	    gtk_entry_get_text(GTK_ENTRY(entrycbIntBook));
	chapter =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sbIntChapter));
	verse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sbIntVerse));

	sprintf(buf, "%s %d:%d", bookname, chapter, verse);
	
	newbook = get_book_from_key(buf);
	chapter = get_chapter_from_key(buf);
	verse = get_verse_from_key(buf);
	
	if (strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(entrycbIntBook), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter), chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), verse);
	sprintf(buf,"%s %d:%d",newbook,chapter,verse);
	gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
	retval = buf;
	return retval;	
}

/******************************************************************************
 * Name
 *   update_controls_interlinear
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
 *   
 *   gchar *update_controls_interlinear(gchar * ref)
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

static gchar *update_controls_interlinear(gchar * ref)
{
	gchar *bookname, buf[256];
	gint chapter, verse;
	const char *newbook;
				
	newbook = get_book_from_key(ref);
	chapter = get_chapter_from_key(ref);
	verse = get_verse_from_key(ref);
	
	bookname = gtk_entry_get_text(GTK_ENTRY(entrycbIntBook));
	if(strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(entrycbIntBook), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter), chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), verse);
	sprintf(buf,"%s %d:%d",newbook,chapter,verse);
	gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
	
	return g_strdup(buf);
}

/******************************************************************************
 * Name
 *   undock_interlinear_page
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
 *   
 *   void undock_interlinear_page(void)
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

void gui_undock_interlinear_page(void)
{
	ApplyChangeBook = FALSE;
	Interlinear_UnDock_Dialog = gui_create_interlinear_dialog();
	gtk_widget_reparent(widgets.frame_interlinear, vboxInt);
	gtk_notebook_remove_page(GTK_NOTEBOOK(widgets.workbook_lower), 2);
	settings.cvInterlinear =
		update_controls_interlinear(settings.currentverse);
	gtk_widget_show(Interlinear_UnDock_Dialog);
	gui_update_interlinear_page_detached();
	g_free(settings.cvInterlinear);
	ApplyChangeBook = TRUE;
}

/******************************************************************************
 * Name
 *   on_btnDockInt_clicked
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
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

void gui_btnDockInt_clicked(GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(Interlinear_UnDock_Dialog);
}

/******************************************************************************
 * Name
 *   on_dlgInterlinear_destroy
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
 *   
 *   void on_dlgInterlinear_destroy(GtkObject *object, gpointer user_data)
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

static void on_dlgInterlinear_destroy(GtkObject *object, gpointer user_data)
{
	GtkWidget * tab_label, *menu_label, *vbox;
	tab_label = gtk_label_new(_("Interlinear"));
	gtk_widget_show(tab_label);
	menu_label = gtk_label_new(_("Interlinear Page"));
	gtk_widget_show(menu_label);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vbox",
				 vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_notebook_insert_page_menu(GTK_NOTEBOOK(widgets.workbook_lower),
				      vbox, tab_label, menu_label, 2);
	gtk_widget_reparent(widgets.frame_interlinear, vbox);
	gtk_notebook_set_page(GTK_NOTEBOOK(widgets.workbook_lower), 2); 
	settings.dockedInt = TRUE;
	gui_update_interlinear_page();
}

/******************************************************************************
 * Name
 *   on_buttonIntSync_clicked
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
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

static void on_buttonIntSync_clicked(GtkButton * button, gpointer user_data)
{
	ApplyChangeBook = FALSE;	
	settings.cvInterlinear =
		update_controls_interlinear(settings.currentverse);
	gui_update_interlinear_page_detached();
	g_free(settings.cvInterlinear);
	ApplyChangeBook = TRUE;
}

/******************************************************************************
 * Name
 *   on_entrycbIntBook_changed
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
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

static void on_entrycbIntBook_changed(GtkEditable *editable,
		gpointer user_data)
{
	if(ApplyChangeBook) {
		gchar *bookname, buf[256];
		bookname = gtk_entry_get_text(GTK_ENTRY(editable));	
		sprintf(buf,"%s %d:%d", bookname, 1, 1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter), 1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), 1);
		gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
		settings.cvInterlinear = buf;
		gui_update_interlinear_page_detached();	
	}
}

/******************************************************************************
 * Name
 *   on_sbIntChapter_button_release_event
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
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

static gboolean on_sbIntChapter_button_release_event(GtkWidget *widget,
		GdkEventButton *event, gpointer user_data)
{		
	ApplyChangeBook = FALSE;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), 1);
	settings.cvInterlinear = change_verse_interlinear();
	gui_update_interlinear_page_detached();
	ApplyChangeBook = TRUE;
	return TRUE;
}

/******************************************************************************
 * Name
 *   on_sbIntVerse_button_release_event
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
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

static gboolean on_sbIntVerse_button_release_event(GtkWidget *widget,
		GdkEventButton *event, gpointer user_data)
{	
	ApplyChangeBook = FALSE;
	settings.cvInterlinear = change_verse_interlinear();
	gui_update_interlinear_page_detached();
	ApplyChangeBook = TRUE;
	return TRUE;
}

/******************************************************************************
 * Name
 *   on_entryIntLookup_key_press_event
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
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

static gboolean on_entryIntLookup_key_press_event(GtkWidget *widget,
		GdkEventKey *event, gpointer user_data)
{
	gchar *buf;		
	ApplyChangeBook = FALSE;
	buf = gtk_entry_get_text(GTK_ENTRY(entryIntLookup));	
	if (event->keyval == 65293 || event->keyval == 65421) {	
		settings.cvInterlinear = update_controls_interlinear(buf);
		gui_update_interlinear_page_detached();
		g_free(settings.cvInterlinear);
		ApplyChangeBook = TRUE;
		return TRUE; 
	}
	ApplyChangeBook = TRUE;
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_btnIntGotoVerse_clicked
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
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

static void on_btnIntGotoVerse_clicked(GtkButton *button, gpointer user_data)
{
	gchar *buf;		//-- pointer to entry string
		//-- pointer to entry string
	ApplyChangeBook = FALSE;
	buf = gtk_entry_get_text(GTK_ENTRY(entryIntLookup));	//-- set pointer to entry text
	
	settings.cvInterlinear = update_controls_interlinear(buf);
	
	gui_update_interlinear_page_detached();	//-- change verse to entry text 
	g_free(settings.cvInterlinear);
	ApplyChangeBook = TRUE;
}

/******************************************************************************
 * Name
 *   create_interlinear_dialog
 *
 * Synopsis
 *   #include "interlinear_dialog.h"
 *   
 *   GtkWidget *create_interlinear_dialog(void)
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

GtkWidget *gui_create_interlinear_dialog(void)
{
	GtkWidget *dlgInterlinear;
	GtkWidget *dialog_vbox19;
	GtkWidget *toolbar29;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *buttonIntSync;
	GtkWidget *cbIntBook;
	GtkObject *sbIntChapter_adj;
	GtkObject *sbIntVerse_adj;
	GtkWidget *btnIntGotoVerse;
	GtkWidget *dialog_action_area19;
	GtkWidget *btnDockInt;
	
	dlgInterlinear = gnome_dialog_new (_("GnomeSWORD - Interlinear Page"), NULL);
	gtk_object_set_data (GTK_OBJECT (dlgInterlinear), "dlgInterlinear", dlgInterlinear);
	gtk_window_set_default_size (GTK_WINDOW (dlgInterlinear), 627, 354);
	gtk_window_set_policy (GTK_WINDOW (dlgInterlinear), TRUE, TRUE, FALSE);
	
	dialog_vbox19 = GNOME_DIALOG (dlgInterlinear)->vbox;
	gtk_object_set_data (GTK_OBJECT (dlgInterlinear), "dialog_vbox19", dialog_vbox19);
	gtk_widget_show (dialog_vbox19);
	
	vboxInt = gtk_vbox_new (FALSE, 0);
	gtk_widget_ref (vboxInt);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "vboxInt", vboxInt,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (vboxInt);
	gtk_box_pack_start (GTK_BOX (dialog_vbox19), vboxInt, TRUE, TRUE, 0);
	
	toolbar29 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref (toolbar29);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "toolbar29", toolbar29,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (toolbar29);
	gtk_box_pack_start (GTK_BOX (vboxInt), toolbar29, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar29), GTK_RELIEF_NONE);
	
	tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgInterlinear, GNOME_STOCK_PIXMAP_REFRESH);
	buttonIntSync = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("button6"),
				NULL, NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref (buttonIntSync);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "buttonIntSync", buttonIntSync,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (buttonIntSync);
	
	cbIntBook = gtk_combo_new ();
	gtk_widget_ref (cbIntBook);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "cbIntBook", cbIntBook,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (cbIntBook);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), cbIntBook, NULL, NULL);
	gtk_widget_set_usize (cbIntBook, 154, -2);
	
	/*** get and load books of the Bible ***/
	gtk_combo_set_popdown_strings(GTK_COMBO(cbIntBook), get_list(BOOKS_LIST));	
	
	entrycbIntBook = GTK_COMBO (cbIntBook)->entry;
	gtk_widget_ref (entrycbIntBook);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "entrycbIntBook", entrycbIntBook,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (entrycbIntBook);
	gtk_entry_set_text (GTK_ENTRY (entrycbIntBook), _("Romans"));
	
	sbIntChapter_adj = gtk_adjustment_new (8, 0, 151, 1, 10, 10);
	sbIntChapter = gtk_spin_button_new (GTK_ADJUSTMENT (sbIntChapter_adj), 1, 0);
	gtk_widget_ref (sbIntChapter);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "sbIntChapter", sbIntChapter,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (sbIntChapter);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), sbIntChapter, NULL, NULL);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (sbIntChapter), TRUE);
	
	sbIntVerse_adj = gtk_adjustment_new (28, 0, 180, 1, 10, 10);
	sbIntVerse = gtk_spin_button_new (GTK_ADJUSTMENT (sbIntVerse_adj), 1, 0);
	gtk_widget_ref (sbIntVerse);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "sbIntVerse", sbIntVerse,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (sbIntVerse);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), sbIntVerse, NULL, NULL);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (sbIntVerse), TRUE);
	
	entryIntLookup = gtk_entry_new ();
	gtk_widget_ref (entryIntLookup);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "entryIntLookup", entryIntLookup,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (entryIntLookup);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar29), entryIntLookup, NULL, NULL);
	gtk_widget_set_usize (entryIntLookup, 190, -2);
	gtk_entry_set_text (GTK_ENTRY (entryIntLookup), _("Romans 8:28"));
	
	tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgInterlinear, GNOME_STOCK_PIXMAP_JUMP_TO);
	btnIntGotoVerse = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
				GTK_TOOLBAR_CHILD_BUTTON,
				NULL,
				_("Goto verse"),
				_("Go to verse in free form lookup and add verse to history"), NULL,
				tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref (btnIntGotoVerse);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "btnIntGotoVerse", btnIntGotoVerse,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btnIntGotoVerse);
	
	dialog_action_area19 = GNOME_DIALOG (dlgInterlinear)->action_area;
	gtk_object_set_data (GTK_OBJECT (dlgInterlinear), "dialog_action_area19", dialog_action_area19);
	gtk_widget_show (dialog_action_area19);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area19), GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area19), 8);
	
	gnome_dialog_append_button_with_pixmap (GNOME_DIALOG (dlgInterlinear),
					  _("Dock"), GNOME_STOCK_PIXMAP_HOME);
	btnDockInt = GTK_WIDGET (g_list_last (GNOME_DIALOG (dlgInterlinear)->buttons)->data);
	gtk_widget_ref (btnDockInt);
	gtk_object_set_data_full (GTK_OBJECT (dlgInterlinear), "btnDockInt", btnDockInt,
			    (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btnDockInt);
	GTK_WIDGET_SET_FLAGS (btnDockInt, GTK_CAN_DEFAULT);
	
	gtk_signal_connect (GTK_OBJECT (dlgInterlinear), "destroy",
	   GTK_SIGNAL_FUNC (on_dlgInterlinear_destroy),
	   NULL);
	gtk_signal_connect (GTK_OBJECT (buttonIntSync), "clicked",
		      GTK_SIGNAL_FUNC (on_buttonIntSync_clicked),
		      NULL); 
	gtk_signal_connect (GTK_OBJECT (entrycbIntBook), "changed",
		      GTK_SIGNAL_FUNC (on_entrycbIntBook_changed),
		      NULL);
	gtk_signal_connect (GTK_OBJECT (sbIntChapter), "button_release_event",
		      GTK_SIGNAL_FUNC (on_sbIntChapter_button_release_event),
		      NULL);
	gtk_signal_connect (GTK_OBJECT (sbIntVerse), "button_release_event",
		      GTK_SIGNAL_FUNC (on_sbIntVerse_button_release_event),
		      NULL);
	gtk_signal_connect (GTK_OBJECT (entryIntLookup), "key_press_event",
		      GTK_SIGNAL_FUNC (on_entryIntLookup_key_press_event),
		      NULL);
	gtk_signal_connect (GTK_OBJECT (btnIntGotoVerse), "clicked",
		      GTK_SIGNAL_FUNC (on_btnIntGotoVerse_clicked),
		      NULL);
	gtk_signal_connect (GTK_OBJECT (btnDockInt), "clicked",
		      GTK_SIGNAL_FUNC (gui_btnDockInt_clicked),
		      NULL);
	
	return dlgInterlinear;
}
/******   end of file   ******/
