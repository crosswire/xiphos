/*
 * GnomeSword Bible Study Tool
 * link_dialog.c - dialog for creating links in editors
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
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/gtkhtmlfontstyle.h>
#include <gtkhtml/htmlform.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <gal/widgets/e-unicode.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


/*
 * gnome
 */
#include "link_dialog.h"
/*
 * main
 */
#include "gs_editor.h"

/******************************************************************************
 * globals to this file only 
 */

static GtkWidget *entryLinkRef;
static GtkWidget *entryLinkMod;


/******************************************************************************
 * Name
 *  set_link_to_module
 *
 * Synopsis
 *   #include "link_dialog.h"
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
 
void set_link_to_module(gchar * linkref, gchar * linkmod,
					GSHTMLEditorControlData * ecd)
{
	gchar buf[256], *target = "";
	HTMLEngine *e = ecd->html->engine;

	if (strlen(linkmod))
		sprintf(buf, "version=%s passage=%s", linkmod, linkref);
	else
		sprintf(buf, "passage=%s", linkref);
	html_engine_selection_push(e);
	html_engine_insert_link(e, buf, target);
	html_engine_selection_pop(e);
}

/******************************************************************************
 * Name
 *   on_btn_link_ok_clicked
 *
 * Synopsis
 *   #include "link_dialog.h"
 *
 *   void on_btn_link_ok_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    ok button on link dialog clicked - get ref(key) and module
 *    and send them to set_link_to_module()
 *
 * Return value
 *   void
 */ 
 
static void on_btn_link_ok_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gchar *linkreference, *linkmodule;
	GtkWidget *dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));

	linkreference =
	    gtk_editable_get_chars(GTK_EDITABLE(entryLinkRef), 0, -1);
	linkmodule =
	    gtk_editable_get_chars(GTK_EDITABLE(entryLinkMod), 0, -1);
	set_link_to_module(linkreference, linkmodule, ecd);

	gtk_widget_destroy(dlg);
	ecd->changed = TRUE;
	update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_cancel_clicked
 *
 * Synopsis
 *   #include "link_dialog.h"
 *
 *   void on_cancel_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    cancel clicked close link dialog and do nothing else
 *
 * Return value
 *   void
 */ 
 
static void on_cancel_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	GtkWidget *dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));

	gtk_widget_destroy(dlg);
}

/******************************************************************************
 * Name
 *  create_link_dialog
 *
 * Synopsis
 *   #include "link_dialog.h"
 *
 *   GtkWidget *create_link_dialog(GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    create dialog for setting up links in text
 *
 * Return value
 *   GtkWidget *
 */ 
 
GtkWidget *create_link_dialog(GSHTMLEditorControlData * ecd)
{
	GtkWidget *dlgLink;
	GtkWidget *dialog_vbox17;
	GtkWidget *vbox46;
	GtkWidget *label182;
	GtkWidget *label183;
	GtkWidget *dialog_action_area17;
	GtkWidget *btnLinkOK;
	GtkWidget *btnLinkCancel;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();

	dlgLink =
	    gnome_dialog_new(_("GnomeSWORD Add Reference Link"), NULL);
	gtk_object_set_data(GTK_OBJECT(dlgLink), "dlgLink", dlgLink);
	gtk_window_set_default_size(GTK_WINDOW(dlgLink), 465, -1);

	dialog_vbox17 = GNOME_DIALOG(dlgLink)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgLink), "dialog_vbox17",
			    dialog_vbox17);
	gtk_widget_show(dialog_vbox17);

	vbox46 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox46);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "vbox46", vbox46,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox46);
	gtk_box_pack_start(GTK_BOX(dialog_vbox17), vbox46, TRUE, TRUE,
			   0);

	label182 = gtk_label_new(_("Reference"));
	gtk_widget_ref(label182);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "label182",
				 label182,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label182);
	gtk_box_pack_start(GTK_BOX(vbox46), label182, FALSE, FALSE, 0);

	entryLinkRef = gtk_entry_new();
	gtk_widget_ref(entryLinkRef);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "entryLinkRef",
				 entryLinkRef,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryLinkRef);
	gtk_box_pack_start(GTK_BOX(vbox46), entryLinkRef, FALSE, FALSE,
			   0);
	gtk_tooltips_set_tip(tooltips, entryLinkRef,
			     _
			     ("Bible Reference or Module key to display when link is clicked"),
			     NULL);

	/*
	 * put selected text in label entry 
	 */
	gtk_html_copy(ecd->html);
	gtk_editable_paste_clipboard(GTK_EDITABLE
				     (GTK_ENTRY(entryLinkRef)));

	label183 = gtk_label_new(_("Module"));
	gtk_widget_ref(label183);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "label183",
				 label183,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label183);
	gtk_box_pack_start(GTK_BOX(vbox46), label183, FALSE, FALSE, 0);

	entryLinkMod = gtk_entry_new();
	gtk_widget_ref(entryLinkMod);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "entryLinkMod",
				 entryLinkMod,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryLinkMod);
	gtk_box_pack_start(GTK_BOX(vbox46), entryLinkMod, FALSE, FALSE,
			   0);
	gtk_tooltips_set_tip(tooltips, entryLinkMod,
			     _("Name of module to show when link is clicked"),
			     NULL);

	dialog_action_area17 = GNOME_DIALOG(dlgLink)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgLink), "dialog_action_area17",
			    dialog_action_area17);
	gtk_widget_show(dialog_action_area17);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area17),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area17),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgLink),
				   GNOME_STOCK_BUTTON_OK);
	btnLinkOK =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgLink)->buttons)->
		       data);
	gtk_widget_ref(btnLinkOK);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "btnLinkOK",
				 btnLinkOK,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLinkOK);
	GTK_WIDGET_SET_FLAGS(btnLinkOK, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(dlgLink),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnLinkCancel =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgLink)->buttons)->
		       data);
	gtk_widget_ref(btnLinkCancel);
	gtk_object_set_data_full(GTK_OBJECT(dlgLink), "btnLinkCancel",
				 btnLinkCancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLinkCancel);
	GTK_WIDGET_SET_FLAGS(btnLinkCancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btnLinkOK), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_link_ok_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(btnLinkCancel), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_clicked), ecd);

	gtk_object_set_data(GTK_OBJECT(dlgLink), "tooltips", tooltips);

	return dlgLink;
}

/******   end of file   ******/
