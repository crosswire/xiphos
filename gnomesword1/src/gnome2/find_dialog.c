/*
 * GnomeSword Bible Study Tool
 * find_dialog.c 
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
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <gal/widgets/e-unicode.h>

#include "gui/find_dialog.h"
#include "main/settings.h"


typedef struct _find_dialog FIND_DIALOG;

struct _find_dialog {
	GtkWidget *dialog;
	GtkWidget *htmlwidget;
	GtkWidget *entry;
	GtkWidget *find;
	GtkWidget *next;
	GtkWidget *close;
	GtkWidget *regex;
	GtkWidget *backward;
	GtkWidget *case_sensitive;
	gboolean regular;
};

static FIND_DIALOG *dialog;


/******************************************************************************
 * Name
 *   dialog_destroy
 *
 * Synopsis
 *   #include ".h"
 *
 *   void dialog_destroy(GtkObject *object, gpointer data)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void dialog_destroy(GtkObject * object, gpointer data)
{
	g_free(dialog);
	dialog = NULL;
	g_print("\nall done\n");
}


/******************************************************************************
 * Name
 *  find_clicked
 *
 * Synopsis
 *   #include ".h"
 *
 *   void find_clicked(GtkButton * button, FIND_DIALOG * d)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void find_clicked(GtkButton * button, FIND_DIALOG * d)
{
	gchar *text;
	GtkHTML *html;

	html = GTK_HTML(d->htmlwidget);
	text = e_utf8_gtk_entry_get_text(GTK_ENTRY(d->entry));
	html_engine_search(html->engine, text,
			   GTK_TOGGLE_BUTTON(d->case_sensitive)->active,
			   GTK_TOGGLE_BUTTON(d->backward)->active == 0,
			   d->regular);
	sprintf(settings.findText, "%s", text);
	g_free(text);
}


/******************************************************************************
 * Name
 *  next_clicked
 *
 * Synopsis
 *   #include ".h"
 *
 *   void next_clicked(GtkButton * button, FIND_DIALOG * d)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void next_clicked(GtkButton * button, FIND_DIALOG * d)
{
	GtkHTML *html;

	html = GTK_HTML(d->htmlwidget);
	if (html->engine->search_info)
		html_engine_search_next(html->engine);
}


/******************************************************************************
 * Name
 *  close_clicked
 *
 * Synopsis
 *   #include ".h"
 *
 *   void close_clicked(GtkButton * button, FIND_DIALOG * d)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void close_clicked(GtkButton * button, FIND_DIALOG * d)
{
	gtk_widget_destroy(d->dialog);
}


/******************************************************************************
 * Name
 *  create_find_dialog
 *
 * Synopsis
 *   #include ".h"
 *
 *   void create_find_dialog(GtkWidget * htmlwidget)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void create_find_dialog(GtkWidget * htmlwidget)
{
	GtkWidget *dialog_vbox29;
	GtkWidget *vbox45;
	GtkWidget *label180;
	GtkWidget *hbox66;
	GtkWidget *dialog_action_area29;
	GtkWidget *hbuttonbox8;
	GtkWidget *alignment;
	GtkWidget *hbox5;
	GtkWidget *image;
	GtkWidget *label12;
	//gchar title[80];

	//sprintf(title, "%s - %s", settings.program_title, _("Find"));

	dialog = g_new(FIND_DIALOG, 1);	/* must be freed */
	dialog->regular = FALSE;
	dialog->htmlwidget = htmlwidget;


	dialog->dialog = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog->dialog),
			    "dialog->dialog", dialog->dialog);
	gtk_window_set_title(GTK_WINDOW(dialog->dialog), _("Find"));
	GTK_WINDOW(dialog->dialog)->type = GTK_WINDOW_TOPLEVEL;
	/*gtk_window_set_policy(GTK_WINDOW(dialog->dialog), TRUE, TRUE,
			      FALSE);*/
  	gtk_container_set_border_width (GTK_CONTAINER (dialog->dialog), 6);
  	gtk_window_set_resizable (GTK_WINDOW (dialog->dialog), FALSE);
  	gtk_dialog_set_has_separator (GTK_DIALOG (dialog->dialog), FALSE);

	dialog_vbox29 = GTK_DIALOG(dialog->dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog->dialog), "dialog_vbox29",
			    dialog_vbox29);
	gtk_widget_show(dialog_vbox29);

	vbox45 = gtk_vbox_new(FALSE, 12);
	gtk_widget_show(vbox45);
	gtk_box_pack_start(GTK_BOX(dialog_vbox29), vbox45, TRUE, TRUE,
			   0);

	label180 = gtk_label_new(_("Enter Word or Phrase"));
	gtk_widget_show(label180);
	gtk_box_pack_start(GTK_BOX(vbox45), label180, FALSE, FALSE, 0);

	dialog->entry = gtk_entry_new();
	gtk_widget_show(dialog->entry);
	gtk_box_pack_start(GTK_BOX(vbox45), dialog->entry, FALSE, FALSE,
			   0);
	gtk_widget_set_size_request(dialog->entry, 291, -1);

	hbox66 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox66);
	gtk_box_pack_start(GTK_BOX(vbox45), hbox66, TRUE, TRUE, 0);

	dialog->case_sensitive =
	    gtk_check_button_new_with_label(_("Match case"));
	gtk_widget_show(dialog->case_sensitive);
	gtk_box_pack_start(GTK_BOX(hbox66), dialog->case_sensitive,
			   FALSE, FALSE, 0);

	dialog->backward =
	    gtk_check_button_new_with_label(_("Search backwards"));
	gtk_widget_show(dialog->backward);
	gtk_box_pack_start(GTK_BOX(hbox66), dialog->backward, FALSE,
			   FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (dialog->backward), FALSE);

	dialog->regex =
	    gtk_check_button_new_with_label(_("Regular expression"));
	gtk_widget_show(dialog->regex);
	gtk_box_pack_start(GTK_BOX(hbox66), dialog->regex, FALSE, FALSE,
			   0);

	dialog_action_area29 = GTK_DIALOG(dialog->dialog)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog->dialog),
			    "dialog_action_area29",
			    dialog_action_area29);
	gtk_widget_show(dialog_action_area29);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area29), 10);

	hbuttonbox8 = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox8);
	gtk_box_pack_start(GTK_BOX(dialog_action_area29), hbuttonbox8,
			   TRUE, TRUE, 0);

	dialog->close = gtk_button_new_from_stock(GTK_STOCK_CLOSE);	//gtk_button_new_with_label(_("Close"));
	gtk_widget_show(dialog->close);
	gtk_container_add(GTK_CONTAINER(hbuttonbox8), dialog->close);
	//GTK_WIDGET_SET_FLAGS(dialog->close, GTK_CAN_DEFAULT);

	dialog->next = gtk_button_new();
	gtk_widget_show(dialog->next);
	gtk_container_add(GTK_CONTAINER(hbuttonbox8), dialog->next);
	//GTK_WIDGET_SET_FLAGS(dialog->next, GTK_CAN_DEFAULT);

	alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_widget_show(alignment);
	gtk_container_add(GTK_CONTAINER(dialog->next), alignment);

	hbox5 = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox5);
	gtk_container_add(GTK_CONTAINER(alignment), hbox5);

	image =
	    gtk_image_new_from_stock(GTK_STOCK_FIND, GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image);
	gtk_box_pack_start(GTK_BOX(hbox5), image, FALSE, FALSE, 0);

	label12 = gtk_label_new_with_mnemonic(_("Find Next"));
	gtk_widget_show(label12);
	gtk_box_pack_start(GTK_BOX(hbox5), label12, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(label12), GTK_JUSTIFY_LEFT);

	dialog->find = gtk_button_new_from_stock(GTK_STOCK_FIND);	//gtk_button_new_with_label(_("Find"));
	gtk_widget_show(dialog->find);
	gtk_container_add(GTK_CONTAINER(hbuttonbox8), dialog->find);
	GTK_WIDGET_SET_FLAGS(dialog->find, GTK_CAN_DEFAULT);




	g_signal_connect(GTK_OBJECT(dialog->dialog), "destroy",
			   G_CALLBACK(dialog_destroy), dialog);
	g_signal_connect(GTK_OBJECT(dialog->find), "clicked",
			   G_CALLBACK(find_clicked), dialog);
	g_signal_connect(GTK_OBJECT(dialog->next), "clicked",
			   G_CALLBACK(next_clicked), dialog);
	g_signal_connect(GTK_OBJECT(dialog->close), "clicked",
			   G_CALLBACK(close_clicked), dialog);
}


/******************************************************************************
 * Name
 *  find_dialog
 *
 * Synopsis
 *   #include ".h"
 *
 *   void find_dialog(GtkWidget * htmlwidget, const gchar * title)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void find_dialog(GtkWidget * htmlwidget, const gchar * title)
{
	if (dialog) {
		//gtk_window_set_title(GTK_WINDOW(dialog->dialog), title);
		dialog->htmlwidget = htmlwidget;
		gtk_widget_show(GTK_WIDGET(dialog->dialog));
		gdk_window_raise(GTK_WIDGET(dialog->dialog)->window);
	} else {
		create_find_dialog(htmlwidget);
		//gtk_window_set_title(GTK_WINDOW(dialog->dialog), title);
		gtk_widget_show(GTK_WIDGET(dialog->dialog));
	}
}


/******************************************************************************
 * Name
 *  gui_find_dlg
 *
 * Synopsis
 *   #include ".h"
 *
 *   void gui_find_dlg(GtkWidget * htmlwidget, gchar * mod_name,
 *		  gboolean regular, gchar * text)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_find_dlg(GtkWidget * htmlwidget, gchar * mod_name,
		  gboolean regular, gchar * text)
{
	gchar buf[256];

	sprintf(buf, "%s in %s", _("Find"), mod_name);

	find_dialog(htmlwidget, buf);
}

/*** end of file ***/
