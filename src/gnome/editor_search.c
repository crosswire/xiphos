/*
 * GnomeSword Bible Study Tool
 * editor_search.c - search dialog for editors
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
 
/* 
 * most of this code is from the GtkHTML library.
 */
#include <config.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <gal/widgets/e-unicode.h>

#include "gui/editor_search.h"
#include "gui/editor.h"

struct _GSHTMLSearchDialog {
	GnomeDialog *dialog;
	GtkHTML     *html;
	GtkWidget   *entry;
	GtkWidget   *backward;
	GtkWidget   *case_sensitive;

	gboolean     regular;
};

/******************************************************************************
 * Name
 *   next_button_cb
 *
 * Synopsis
 *   #include "editor_search.h"
 *
 *   void next_button_cb(GtkWidget *but, GtkHTML *html)
 *
 * Description
 *   find again
 *
 * Return value
 *   void
 */

static void next_button_cb(GtkWidget *but, GtkHTML *html)
{
	if (html->engine->search_info)
		html_engine_search_next (html->engine);	
}

/******************************************************************************
 * Name
 *   search_cb
 *
 * Synopsis
 *   #include "editor_search.h"
 *
 *   void search_cb(GtkWidget *but, GSHTMLSearchDialog *d)
 *
 * Description
 *   find the text in the entry
 *
 * Return value
 *   void
 */

static void search_cb(GtkWidget *but, GSHTMLSearchDialog *d)
{
	char *text;

	text = e_utf8_gtk_entry_get_text(GTK_ENTRY(d->entry));
	html_engine_search(d->html->engine, text,
			GTK_TOGGLE_BUTTON(d->case_sensitive)->active,
			GTK_TOGGLE_BUTTON(d->backward)->active == 0, d->regular);
	g_free (text);
}

/******************************************************************************
 * Name
 *   close_dialog
 *
 * Synopsis
 *   #include "editor_search.h"
 *
 *   void close_dialog(GtkWidget *but, GSHTMLSearchDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void close_dialog(GtkWidget *but, GSHTMLSearchDialog *d)
{
	gnome_dialog_close (d->dialog);	
}

/******************************************************************************
 * Name
 *   entry_activate
 *
 * Synopsis
 *   #include "editor_search.h"
 *
 *   void entry_activate (GtkWidget *entry, GSHTMLSearchDialog *d)
 *
 * Description
 *   someone pressed the <enter> key
 *
 * Return value
 *   void
 */

static void entry_activate (GtkWidget *entry, GSHTMLSearchDialog *d)
{
	search_cb (NULL, d);
}

/******************************************************************************
 * Name
 *   gs_editor_search_dialog_new
 *
 * Synopsis
 *   #include "editor_search.h"
 *
 *   GSHTMLSearchDialog *gs_editor_search_dialog_new (GtkHTML *html)
 *
 * Description
 *   
 *
 * Return value
 *   GSHTMLSearchDialog *
 */

GSHTMLSearchDialog *gs_editor_search_dialog_new (GtkHTML *html)
{
	GSHTMLSearchDialog *dialog = g_new (GSHTMLSearchDialog, 1);
	GtkWidget *hbox;

	dialog->dialog         = GNOME_DIALOG (gnome_dialog_new (NULL, _("Find"), _("Find Next"), GNOME_STOCK_BUTTON_CANCEL, NULL));
	dialog->entry          = gtk_entry_new_with_max_length (20);
	dialog->backward       = gtk_check_button_new_with_label (_("backward"));
	dialog->case_sensitive = gtk_check_button_new_with_label (_("case sensitive"));
	dialog->html           = html;
	dialog->regular        = FALSE;

	hbox = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->backward);
	gtk_box_pack_start_defaults (GTK_BOX (hbox), dialog->case_sensitive);

	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), dialog->entry);
	gtk_box_pack_start_defaults (GTK_BOX (dialog->dialog->vbox), hbox);
	gtk_widget_show (dialog->entry);
	gtk_widget_show_all (hbox);

	gnome_dialog_button_connect (dialog->dialog, 0, search_cb, dialog);
	gnome_dialog_button_connect (dialog->dialog, 1, next_button_cb, (GtkHTML*)html);
	gnome_dialog_button_connect (dialog->dialog, 2, close_dialog, dialog);
	gnome_dialog_close_hides (dialog->dialog, TRUE);
	gnome_dialog_set_close (dialog->dialog, FALSE);

	gnome_dialog_set_default (dialog->dialog, 0);
	gtk_widget_grab_focus (dialog->entry);

	gtk_signal_connect (GTK_OBJECT (dialog->entry), "activate",
			    entry_activate, dialog);

	return dialog;
}

/******************************************************************************
 * Name
 *   gs_editor_search_dialog_destroy
 *
 * Synopsis
 *   #include "editor_search.h"
 *
 *   void gs_editor_search_dialog_destroy (GSHTMLSearchDialog *d)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gs_editor_search_dialog_destroy (GSHTMLSearchDialog *d)
{
	g_free (d);
}

/******************************************************************************
 * Name
 *   search
 *
 * Synopsis
 *   #include "editor_search.h"
 *
 *   void search (GSHTMLEditorControlData *ecd, 
 *				gboolean regular, gchar *text)
 *
 * Description
 *   create of show find dialog
 *
 * Return value
 *   void
 */

void search (GSHTMLEditorControlData *ecd, 
				gboolean regular, gchar *text)
{
	RUN_DIALOG (search, regular ? _("Find Regular Expression") :  _("Find"));
	
	if (ecd->search_dialog)
		ecd->search_dialog->regular = regular;

	if (ecd->search_dialog) {
		if(text)
			gtk_entry_set_text(GTK_ENTRY(ecd->search_dialog->entry),text);
		
		gtk_widget_grab_focus (ecd->search_dialog->entry);		
	}
}

/******************************************************************************
 * Name
 *   search_next
 *
 * Synopsis
 *   #include "editor_search.h"
 *
 *   void search_next (GSHTMLEditorControlData *ecd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void search_next (GSHTMLEditorControlData *ecd)
{
	if (ecd->html->engine->search_info) {
		html_engine_search_next (ecd->html->engine);
	} else {
		search (ecd, FALSE, NULL);
	}
}


