/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
     insertbar.c  20001-04-24
     
    Copyright (C) 2001 Terry Biggs

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
    
*/

/* most of the code (that does anything) in this file comes form gtkhtml */
#include <config.h>
#include <gnome.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <gal/widgets/widget-color-combo.h>
#include <htmlcolor.h>
#include <htmlcolorset.h>
#include <htmlengine-edit-fontstyle.h>
#include <htmlsettings.h>
#include <htmlengine-edit-cut-and-paste.h>

#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>
#include "filebar.h"

#include "insertbar.h"
#include "toolbar.h"
#include "utils.h"
#include "properties.h"
#include "text.h"
//#include "gs_sword.h" 
#include "gs_editor.h" 
#include "control-data.h"
static void
insert_toolbar_insert_image_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);
	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, TRUE, _("Insert"));
	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_IMAGE, _("Image"),
						   image_insertion,
						   image_insert_cb,
						   image_close_cb);
	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_LINK, _("Link"),
						   link_properties,
						   link_apply_cb,
						   link_close_cb);
	gtk_html_edit_properties_dialog_show (cd->properties_dialog);	
	cd->html_modified = TRUE;
	updatestatusbar(cd);
}

static void
insert_toolbar_insert_link_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);

	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, FALSE, _("Insert"));

	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_TEXT, _("Text"),
						   text_properties,
						   text_apply_cb,
						   text_close_cb);
	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_LINK, _("Link"),
						   link_properties,
						   link_apply_cb,
						   link_close_cb);

	gtk_html_edit_properties_dialog_show (cd->properties_dialog);
	gtk_html_edit_properties_dialog_set_page (cd->properties_dialog, GTK_HTML_EDIT_PROPERTY_LINK);
	cd->html_modified = TRUE;
	updatestatusbar(cd);
}


static void
insert_toolbar_insert_rule_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);

	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, TRUE, _("Insert"));

	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_RULE, _("Rule"),
						   rule_insert,
						   rule_insert_cb,
						   rule_close_cb);

	gtk_html_edit_properties_dialog_show (cd->properties_dialog);
	cd->html_modified = TRUE;
	updatestatusbar(cd);
}


static GnomeUIInfo insert_toolbar_insert_uiinfo[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("Image"), N_("Insert an Image"),
				insert_toolbar_insert_image_cb, GNOME_STOCK_PIXMAP_ADD),
	GNOMEUIINFO_ITEM_STOCK (N_("Link"), N_("Insert a Link"),
				insert_toolbar_insert_link_cb, GNOME_STOCK_PIXMAP_ADD),
	GNOMEUIINFO_ITEM_STOCK (N_("Rule"), N_("Insert a Rule"),
				insert_toolbar_insert_rule_cb, GNOME_STOCK_PIXMAP_ADD),
	
	GNOMEUIINFO_END
};

static GtkWidget *
create_insert_toolbar (GtkHTMLControlData *cd)
{
	GtkWidget *frame, *hbox;

	hbox = gtk_hbox_new (FALSE, 0);
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	cd->toolbar_insert = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_container_add (GTK_CONTAINER (frame), cd->toolbar_insert);
	gtk_box_pack_start (GTK_BOX (hbox), frame, FALSE, FALSE, 0);

	gtk_widget_show_all (hbox);
	gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_insert), insert_toolbar_insert_uiinfo, NULL, cd);
	
	return hbox;
}


GtkWidget *
toolbar_insert(GtkHTMLControlData *cd)
{
	g_return_val_if_fail (cd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (cd->html), NULL);
	
	return create_insert_toolbar (cd);
}
