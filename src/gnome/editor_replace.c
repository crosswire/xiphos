/*
 * GnomeSword Bible Study Tool
 * editor_replace.c - find and replace dialog for editors
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


#include <config.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine.h>

#include "gui/editor_replace.h"
#include "gui/editor.h"

struct _GtkHTMLReplaceAskDialog {
	GtkWidget *dialog;
	HTMLEngine *engine;
};

struct _GtkHTMLReplaceDialog {
	GtkWidget *dialog;
	GtkHTML *html;
	GtkWidget *entry_find;
	GtkWidget *entry_replace;
	GtkWidget *backwards;
	GtkWidget *case_sensitive;

	GtkHTMLReplaceAskDialog *ask_dialog;
};
static GtkHTMLReplaceDialog *dialog;

/******************************************************************************
 * Name
 *   replace_do
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void replace_do (GtkHTMLReplaceAskDialog *d, 
 *				HTMLReplaceQueryAnswer answer)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void replace_do(GtkHTMLReplaceAskDialog * d,
		       HTMLReplaceQueryAnswer answer)
{
	gtk_widget_hide(d->dialog);
	html_engine_replace_do(d->engine, answer);
}

/******************************************************************************
 * Name
 *   replace_cb
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void replace_cb (GtkWidget *button, GtkHTMLReplaceAskDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void replace_cb(GtkWidget * button, GtkHTMLReplaceAskDialog * d)
{
	replace_do(d, RQA_Replace);
}

/******************************************************************************
 * Name
 *   replace_all_cb
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void replace_all_cb (GtkWidget *button, 
 *				GtkHTMLReplaceAskDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void replace_all_cb(GtkWidget * button,
			   GtkHTMLReplaceAskDialog * d)
{
	replace_do(d, RQA_ReplaceAll);
}

/******************************************************************************
 * Name
 *   next_cb
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void next_cb (GtkWidget *button, GtkHTMLReplaceAskDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void next_cb(GtkWidget * button, GtkHTMLReplaceAskDialog * d)
{
	replace_do(d, RQA_Next);
}

/******************************************************************************
 * Name
 *   cancel_cb
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void cancel_cb (GtkWidget *button, GtkHTMLReplaceAskDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void cancel_cb(GtkWidget * button, GtkHTMLReplaceAskDialog * d)
{
	replace_do(d, RQA_Cancel);
}

/******************************************************************************
 * Name
 *   ask_dialog_new
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   GtkHTMLReplaceAskDialog * ask_dialog_new (HTMLEngine *e)
 *
 * Description
 *   
 *
 * Return value
 *   GtkHTMLReplaceAskDialog
 */

static GtkHTMLReplaceAskDialog *ask_dialog_new(HTMLEngine * e)
{
	GtkHTMLReplaceAskDialog *d;

	GtkWidget *dialog_ask;
	GtkWidget *dialog_vbox31;
	GtkWidget *hseparator2;
	GtkWidget *dialog_action_area31;
	GtkWidget *hbuttonbox10;
	GtkWidget *button_replace_ask;
	GtkWidget *button_all;
	GtkWidget *button_next;
	GtkWidget *button_cancel_ask;


	dialog_ask = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog_ask), "dialog_ask",
			    dialog_ask);
	gtk_window_set_title(GTK_WINDOW(dialog_ask),
			     _("Replace confirmation"));
	GTK_WINDOW(dialog_ask)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_policy(GTK_WINDOW(dialog_ask), TRUE, TRUE, TRUE);


	d = g_new(GtkHTMLReplaceAskDialog, 1);
	d->dialog = dialog_ask;
	d->engine = e;

	dialog_vbox31 = GTK_DIALOG(dialog_ask)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_ask), "dialog_vbox31",
			    dialog_vbox31);
	gtk_widget_show(dialog_vbox31);

	hseparator2 = gtk_hseparator_new();
	gtk_widget_ref(hseparator2);
	gtk_object_set_data_full(GTK_OBJECT(dialog_ask), "hseparator2",
				 hseparator2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hseparator2);
	gtk_box_pack_start(GTK_BOX(dialog_vbox31), hseparator2, TRUE,
			   TRUE, 0);

	dialog_action_area31 = GTK_DIALOG(dialog_ask)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog_ask),
			    "dialog_action_area31",
			    dialog_action_area31);
	gtk_widget_show(dialog_action_area31);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area31), 10);

	hbuttonbox10 = gtk_hbutton_box_new();
	gtk_widget_ref(hbuttonbox10);
	gtk_object_set_data_full(GTK_OBJECT(dialog_ask), "hbuttonbox10",
				 hbuttonbox10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbuttonbox10);
	gtk_box_pack_start(GTK_BOX(dialog_action_area31), hbuttonbox10,
			   TRUE, TRUE, 0);

	button_replace_ask = gtk_button_new_with_label(_("Replace"));
	gtk_widget_ref(button_replace_ask);
	gtk_object_set_data_full(GTK_OBJECT(dialog_ask),
				 "button_replace_ask",
				 button_replace_ask,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_replace_ask);
	gtk_container_add(GTK_CONTAINER(hbuttonbox10),
			  button_replace_ask);
	GTK_WIDGET_SET_FLAGS(button_replace_ask, GTK_CAN_DEFAULT);

	button_all = gtk_button_new_with_label(_("Replace all"));
	gtk_widget_ref(button_all);
	gtk_object_set_data_full(GTK_OBJECT(dialog_ask), "button_all",
				 button_all,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_all);
	gtk_container_add(GTK_CONTAINER(hbuttonbox10), button_all);
	GTK_WIDGET_SET_FLAGS(button_all, GTK_CAN_DEFAULT);

	button_next = gtk_button_new_with_label(_("Next"));
	gtk_widget_ref(button_next);
	gtk_object_set_data_full(GTK_OBJECT(dialog_ask), "button_next",
				 button_next,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_next);
	gtk_container_add(GTK_CONTAINER(hbuttonbox10), button_next);
	GTK_WIDGET_SET_FLAGS(button_next, GTK_CAN_DEFAULT);

	button_cancel_ask =
	    gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	gtk_widget_ref(button_cancel_ask);
	gtk_object_set_data_full(GTK_OBJECT(dialog_ask),
				 "button_cancel_ask", button_cancel_ask,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_cancel_ask);
	gtk_container_add(GTK_CONTAINER(hbuttonbox10),
			  button_cancel_ask);
	GTK_WIDGET_SET_FLAGS(button_cancel_ask, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(button_replace_ask), "clicked",
			   GTK_SIGNAL_FUNC(replace_cb), d);
	gtk_signal_connect(GTK_OBJECT(button_all), "clicked",
			   GTK_SIGNAL_FUNC(replace_all_cb), d);
	gtk_signal_connect(GTK_OBJECT(button_next), "clicked",
			   GTK_SIGNAL_FUNC(next_cb), d);
	gtk_signal_connect(GTK_OBJECT(button_cancel_ask), "clicked",
			   GTK_SIGNAL_FUNC(cancel_cb), d);

	gtk_widget_grab_default(button_replace_ask);

	return d;
}

/******************************************************************************
 * Name
 *   ask
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void ask (HTMLEngine *e, gpointer data)
 *
 * Description
 *   open ask dialog (replace?)
 *
 * Return value
 *   void
 */

static void ask(HTMLEngine * e, gpointer data)
{
	GtkHTMLReplaceDialog *d = (GtkHTMLReplaceDialog *) data;

	if (!d->ask_dialog) {
//		g_warning("new ask dialog!");
		d->ask_dialog = ask_dialog_new(e);
		gtk_widget_show(GTK_WIDGET(d->ask_dialog->dialog));
	} else {
		d->ask_dialog->engine = e;
		gtk_widget_show(GTK_WIDGET(d->ask_dialog->dialog));
		gdk_window_raise(GTK_WIDGET(d->ask_dialog->dialog)->
				 window);
	}
}

/******************************************************************************
 * Name
 *   button_replace_cb
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void button_replace_cb (GtkWidget *but, 
 *				GtkHTMLReplaceDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void button_replace_cb(GtkWidget * but, GtkHTMLReplaceDialog * d)
{
	gtk_widget_hide(d->dialog);
	html_engine_replace(d->html->engine,
			    gtk_entry_get_text(GTK_ENTRY
					       (d->entry_find)),
			    gtk_entry_get_text(GTK_ENTRY
					       (d->entry_replace)),
			    GTK_TOGGLE_BUTTON(d->case_sensitive)->
			    active,
			    GTK_TOGGLE_BUTTON(d->backwards)->active ==
			    0, FALSE, ask, d);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void button_cancel_clicked(GtkButton * button,
				  GtkHTMLReplaceDialog * d)
{
	gtk_widget_hide(d->dialog);
}


/******************************************************************************
 * Name
 *   entry_activate
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void entry_activate (GtkWidget *entry, GtkHTMLReplaceDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void entry_activate(GtkWidget * entry, GtkHTMLReplaceDialog * d)
{
	button_replace_cb(NULL, d);
}

/******************************************************************************
 * Name
 *   gs_editor_replace_dialog_new
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   GtkHTMLReplaceDialog *gs_editor_replace_dialog_new (GtkHTML *html)
 *
 * Description
 *   
 *
 * Return value
 *   GtkHTMLReplaceDialog *
 */

GtkHTMLReplaceDialog *gs_editor_replace_dialog_new(GtkHTML * html)
{
	GtkWidget *dialog_vbox30;
	GtkWidget *vbox87;
	GtkWidget *hbox86;
	GtkWidget *label254;
	GtkWidget *hbox87;
	GtkWidget *label255;
	GtkWidget *hbox85;
	GtkWidget *dialog_action_area30;
	GtkWidget *hbuttonbox9;
	GtkWidget *button_replace;
	GtkWidget *button_cancel;

	dialog = g_new(GtkHTMLReplaceDialog, 1);

	dialog->dialog = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog->dialog),
			    "dialog->dialog", dialog->dialog);
	gtk_window_set_title(GTK_WINDOW(dialog->dialog), _("Replace"));
	GTK_WINDOW(dialog->dialog)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_policy(GTK_WINDOW(dialog->dialog), TRUE, TRUE,
			      FALSE);

	dialog->html = html;
	dialog->ask_dialog = NULL;

	dialog_vbox30 = GTK_DIALOG(dialog->dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog->dialog), "dialog_vbox30",
			    dialog_vbox30);
	gtk_widget_show(dialog_vbox30);

	vbox87 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox87);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog), "vbox87",
				 vbox87,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox87);
	gtk_box_pack_start(GTK_BOX(dialog_vbox30), vbox87, TRUE, TRUE,
			   0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox87), 5);

	hbox86 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox86);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog), "hbox86",
				 hbox86,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox86);
	gtk_box_pack_start(GTK_BOX(vbox87), hbox86, FALSE, TRUE, 0);

	label254 = gtk_label_new(_(" Replace "));
	gtk_widget_ref(label254);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog), "label254",
				 label254,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label254);
	gtk_box_pack_start(GTK_BOX(hbox86), label254, FALSE, FALSE, 0);

	dialog->entry_find = gtk_entry_new();
	gtk_widget_ref(dialog->entry_find);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog),
				 "dialog->entry_find",
				 dialog->entry_find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dialog->entry_find);
	gtk_box_pack_start(GTK_BOX(hbox86), dialog->entry_find, TRUE,
			   TRUE, 0);

	hbox87 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox87);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog), "hbox87",
				 hbox87,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox87);
	gtk_box_pack_start(GTK_BOX(vbox87), hbox87, FALSE, TRUE, 0);

	label255 = gtk_label_new(_("       with "));
	gtk_widget_ref(label255);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog), "label255",
				 label255,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label255);
	gtk_box_pack_start(GTK_BOX(hbox87), label255, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(label255), GTK_JUSTIFY_RIGHT);

	dialog->entry_replace = gtk_entry_new();
	gtk_widget_ref(dialog->entry_replace);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog),
				 "dialog->entry_replace",
				 dialog->entry_replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dialog->entry_replace);
	gtk_box_pack_start(GTK_BOX(hbox87), dialog->entry_replace, TRUE,
			   TRUE, 0);

	hbox85 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox85);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog), "hbox85",
				 hbox85,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox85);
	gtk_box_pack_start(GTK_BOX(vbox87), hbox85, FALSE, TRUE, 0);

	dialog->backwards =
	    gtk_check_button_new_with_label(_("search backward"));
	gtk_widget_ref(dialog->backwards);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog),
				 "dialog->backwards", dialog->backwards,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dialog->backwards);
	gtk_box_pack_start(GTK_BOX(hbox85), dialog->backwards, TRUE,
			   TRUE, 4);

	dialog->case_sensitive =
	    gtk_check_button_new_with_label(_("case sensitive"));
	gtk_widget_ref(dialog->case_sensitive);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog),
				 "dialog->case_sensitive",
				 dialog->case_sensitive,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(dialog->case_sensitive);
	gtk_box_pack_start(GTK_BOX(hbox85), dialog->case_sensitive,
			   TRUE, TRUE, 4);

	dialog_action_area30 = GTK_DIALOG(dialog->dialog)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog->dialog),
			    "dialog_action_area30",
			    dialog_action_area30);
	gtk_widget_show(dialog_action_area30);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area30), 10);

	hbuttonbox9 = gtk_hbutton_box_new();
	gtk_widget_ref(hbuttonbox9);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog),
				 "hbuttonbox9", hbuttonbox9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbuttonbox9);
	gtk_box_pack_start(GTK_BOX(dialog_action_area30), hbuttonbox9,
			   TRUE, TRUE, 0);

	button_replace = gtk_button_new_with_label(_("Replace"));
	gtk_widget_ref(button_replace);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog),
				 "button_replace", button_replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_replace);
	gtk_container_add(GTK_CONTAINER(hbuttonbox9), button_replace);
	GTK_WIDGET_SET_FLAGS(button_replace, GTK_CAN_DEFAULT);

	button_cancel = gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	gtk_widget_ref(button_cancel);
	gtk_object_set_data_full(GTK_OBJECT(dialog->dialog),
				 "button_cancel", button_cancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(button_cancel);
	gtk_container_add(GTK_CONTAINER(hbuttonbox9), button_cancel);
	GTK_WIDGET_SET_FLAGS(button_cancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(button_replace), "clicked",
			   GTK_SIGNAL_FUNC(button_replace_cb), dialog);
	gtk_signal_connect(GTK_OBJECT(button_cancel), "clicked",
			   GTK_SIGNAL_FUNC(button_cancel_clicked),
			   dialog);

	return dialog;
}

/******************************************************************************
 * Name
 *   gs_editor_replace_dialog_destroy
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void gs_editor_replace_dialog_destroy (GtkHTMLReplaceDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gs_editor_replace_dialog_destroy(GtkHTMLReplaceDialog * d)
{
	g_free(d);
}

/******************************************************************************
 * Name
 *  run_dialog
 *
 * Synopsis
 *   #include ".h"
 *
 *   void run_dialog(GnomeDialog *** dialog, GtkHTML * html, 
 *				DialogCtor ctor, const gchar * title)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void run_replace_dialog(GtkHTML * html, const gchar * title)
{
	if (dialog) {
		gtk_window_set_title(GTK_WINDOW(dialog->dialog), title);
		dialog->html = html;
		gtk_widget_show(GTK_WIDGET(dialog->dialog));
		gdk_window_raise(GTK_WIDGET(dialog->dialog)->window);
	} else {
		gs_editor_replace_dialog_new(html);
		gtk_window_set_title(GTK_WINDOW(dialog->dialog), title);
		gtk_widget_show(GTK_WIDGET(dialog->dialog));
	}
}

/******************************************************************************
 * Name
 *   replace
 *
 * Synopsis
 *   #include "editor_replace.h"
 *
 *   void replace(GSHTMLEditorControlData *ecd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void replace(GSHTMLEditorControlData * ecd)
{
	gchar title[256];
	
	sprintf(title,"Replace in %s", g_basename(ecd->filename));
	run_replace_dialog(ecd->html, title);

	if (ecd->replace_dialog)
		gtk_widget_grab_focus(ecd->replace_dialog->entry_find);
}
