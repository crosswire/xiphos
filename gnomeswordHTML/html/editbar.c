/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
     editbar.c  20001-04-24
     
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

#include "editbar.h"
#include "toolbar.h"
#include "utils.h"
#include "properties.h"
#include "text.h"
#include "gs_sword.h" 
#include "gs_editor.h" 

static void
edit_toolbar_copy_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	html_engine_copy (cd->html->engine);
}
static void
edit_toolbar_cut_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	html_engine_cut(cd->html->engine);
}
static void
edit_toolbar_paste_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	html_engine_paste(cd->html->engine);
}


static GnomeUIInfo edit_toolbar_edit_uiinfo[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("Copy"), N_("Copy selected text"),
				edit_toolbar_copy_cb, GNOME_STOCK_PIXMAP_COPY),
	GNOMEUIINFO_ITEM_STOCK (N_("Cut"), N_("Cut selected text"),
				edit_toolbar_cut_cb, GNOME_STOCK_PIXMAP_CUT),
	GNOMEUIINFO_ITEM_STOCK (N_("Paste"), N_("Paste from clipboard"),
				edit_toolbar_paste_cb, GNOME_STOCK_PIXMAP_PASTE),
	GNOMEUIINFO_END
};



static GtkWidget *
create_edit_toolbar (GtkHTMLControlData *cd)
{
	GtkWidget *frame, *hbox;

	hbox = gtk_hbox_new (FALSE, 0);
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	cd->toolbar_edit = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_container_add (GTK_CONTAINER (frame), cd->toolbar_edit);
	gtk_box_pack_start (GTK_BOX (hbox), frame, FALSE, FALSE, 0);

	gtk_widget_show_all (hbox);
	gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_edit), edit_toolbar_edit_uiinfo, NULL, cd);
	
	return hbox;
}


GtkWidget *
toolbar_edit (GtkHTMLControlData *cd)
{
	g_return_val_if_fail (cd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (cd->html), NULL);
	
	return create_edit_toolbar (cd);
}

