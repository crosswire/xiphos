/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_Interlinear_dlg.c
     * -------------------
     * Thu Feb  7 21:55:14 2002 
     * copyright (C) 2002 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include "gs_detach_int.h"
#include "gs_gnomesword.h"
#include "sw_sword.h"

GtkWidget *Interlinear_UnDock_Dialog;
GtkWidget *vboxInt;
GtkWidget *entrycbIntBook;
GtkWidget *sbIntChapter;
GtkWidget *sbIntVerse;
GtkWidget *entryIntLookup;
gboolean ApplyChangeBook;

void undock_interlinear_page(SETTINGS * s)
{
	ApplyChangeBook = FALSE;
	Interlinear_UnDock_Dialog = create_dlgInterlinear(s);
	gtk_widget_reparent(s->frameInt, vboxInt);
	gtk_notebook_remove_page(GTK_NOTEBOOK(s->workbook_lower), 2);
	s->cvInterlinear = intsyncSWORD(entrycbIntBook, 
					sbIntChapter, 
					sbIntVerse, 
					entryIntLookup,
					s->currentverse);
	updateIntDlg(s);
	gtk_widget_show(Interlinear_UnDock_Dialog);
	g_free(s->cvInterlinear);
	ApplyChangeBook = TRUE;
}

void on_btnDockInt_clicked(GtkButton * button, SETTINGS * s)
{
	gtk_widget_destroy(Interlinear_UnDock_Dialog);
}


static void on_dlgInterlinear_destroy(GtkObject * object, SETTINGS * s)
{
	GtkWidget * tab_label, *menu_label, *vbox;
	tab_label = gtk_label_new(_("Interlinear"));
	gtk_widget_show(tab_label);
	menu_label = gtk_label_new(_("Interlinear Page"));
	gtk_widget_show(menu_label);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox",
				 vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_notebook_insert_page_menu(GTK_NOTEBOOK(s->workbook_lower),
				      vbox, tab_label, menu_label, 2);
	gtk_widget_reparent(s->frameInt, vbox);
	gtk_notebook_set_page(GTK_NOTEBOOK(s->workbook_lower), 2); 
	s->dockedInt = TRUE;
	updateinterlinearpage();
}

static void on_buttonIntSync_clicked(GtkButton * button, SETTINGS * s)
{
	ApplyChangeBook = FALSE;
	s->cvInterlinear = intsyncSWORD(entrycbIntBook, 
					sbIntChapter, 
					sbIntVerse, 
					entryIntLookup,
					s->currentverse);
	updateIntDlg(s);
	g_free(s->cvInterlinear);
	ApplyChangeBook = TRUE;
	
}

static void
on_entrycbIntBook_changed              (GtkEditable     *editable,
                                        SETTINGS * s)
{
	if(ApplyChangeBook) {
		gchar *bookname, buf[256];
		bookname = gtk_entry_get_text(GTK_ENTRY(editable));	
		sprintf(buf,"%s %d:%d", bookname, 1, 1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntChapter), 1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), 1);
		gtk_entry_set_text(GTK_ENTRY(entryIntLookup), buf);
		s->cvInterlinear = buf;
		updateIntDlg(s);	
	}
}

static gboolean
on_sbIntChapter_button_release_event(GtkWidget * widget,
				   GdkEventButton * event,
				   SETTINGS * s)
{		
	ApplyChangeBook = FALSE;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sbIntVerse), 1);
	s->cvInterlinear = intchangeverseSWORD(entrycbIntBook, 
					sbIntChapter, 
					sbIntVerse, 
					entryIntLookup);
	updateIntDlg(s);
	ApplyChangeBook = TRUE;
	return TRUE;
}

static gboolean
on_sbIntVerse_button_release_event(GtkWidget * widget,
				 GdkEventButton * event,
				 SETTINGS * s)
{	
	ApplyChangeBook = FALSE;
	s->cvInterlinear = intchangeverseSWORD(entrycbIntBook, 
					sbIntChapter, 
					sbIntVerse, 
					entryIntLookup);
	updateIntDlg(s);
	ApplyChangeBook = TRUE;
	return TRUE;
}



static gboolean
on_entryIntLookup_key_press_event      (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        SETTINGS * s)
{
	gchar *buf;		//-- pointer to entry string
	ApplyChangeBook = FALSE;
	buf = gtk_entry_get_text(GTK_ENTRY(entryIntLookup));	//-- set pointer to entry text
	if (event->keyval == 65293 || event->keyval == 65421) {	//-- if user hit return key continue
		s->cvInterlinear = intsyncSWORD(entrycbIntBook, 
					sbIntChapter, 
					sbIntVerse, 
					entryIntLookup,
					buf);
		updateIntDlg(s);;	//-- change verse to entry text 
		g_free(s->cvInterlinear);
		ApplyChangeBook = TRUE;
		return TRUE; 
	}
	ApplyChangeBook = TRUE;
	return FALSE;
}

static void
on_btnIntGotoVerse_clicked             (GtkButton       *button,
                                        SETTINGS * s)
{
	gchar *buf;		//-- pointer to entry string
		//-- pointer to entry string
	ApplyChangeBook = FALSE;
	buf = gtk_entry_get_text(GTK_ENTRY(entryIntLookup));	//-- set pointer to entry text
	s->cvInterlinear = intsyncSWORD(entrycbIntBook, 
					sbIntChapter, 
					sbIntVerse, 
					entryIntLookup,
					buf);
	updateIntDlg(s);;	//-- change verse to entry text 
	g_free(s->cvInterlinear);
	ApplyChangeBook = TRUE;
}

GtkWidget *create_dlgInterlinear(SETTINGS * s)
{
	GtkWidget *dlgInterlinear;
	GtkWidget *dialog_vbox19;
	GtkWidget *toolbar29;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *buttonIntSync;
	GtkWidget *cbIntBook;
	GList *cbIntBook_items = NULL;
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
	cbIntBook_items = backend_getBibleBooksSWORD();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbIntBook), cbIntBook_items);
	g_list_free(cbIntBook_items);
	
	
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
	   s);
	gtk_signal_connect (GTK_OBJECT (buttonIntSync), "clicked",
		      GTK_SIGNAL_FUNC (on_buttonIntSync_clicked),
		      s); 
	gtk_signal_connect (GTK_OBJECT (entrycbIntBook), "changed",
		      GTK_SIGNAL_FUNC (on_entrycbIntBook_changed),
		      s);
	gtk_signal_connect (GTK_OBJECT (sbIntChapter), "button_release_event",
		      GTK_SIGNAL_FUNC (on_sbIntChapter_button_release_event),
		      s);
	gtk_signal_connect (GTK_OBJECT (sbIntVerse), "button_release_event",
		      GTK_SIGNAL_FUNC (on_sbIntVerse_button_release_event),
		      s);
	gtk_signal_connect (GTK_OBJECT (entryIntLookup), "key_press_event",
		      GTK_SIGNAL_FUNC (on_entryIntLookup_key_press_event),
		      s);
	gtk_signal_connect (GTK_OBJECT (btnIntGotoVerse), "clicked",
		      GTK_SIGNAL_FUNC (on_btnIntGotoVerse_clicked),
		      s);
	gtk_signal_connect (GTK_OBJECT (btnDockInt), "clicked",
		      GTK_SIGNAL_FUNC (on_btnDockInt_clicked),
		      s);
	
	return dlgInterlinear;
}
/******   end of file   ******/
