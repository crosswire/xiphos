/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*  This file is part of the GtkHTML library.

    Copyright (C) 2000 Helix Code, Inc.

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

    Author: Radek Doulik <rodo@helixcode.com>
*/

/*
 *   taken from GtkHTML and modified to work with gnomesword
 *   terry
 *
 */

#include <config.h>
#include "gs_control_data.h"
//#include "spell.h"

GtkHTMLControlData *
gtk_html_control_data_new (GtkHTML *html, GtkWidget *vbox)
{
	GtkHTMLControlData * ncd = g_new0 (GtkHTMLControlData, 1);

	ncd->html                    = html;
	ncd->vbox                    = vbox;
	//ncd->properties_dialog       = NULL;
	//ncd->properties_types        = NULL;
	ncd->block_font_style_change = FALSE;
//	ncd->dict_client             = spell_new_dictionary ();
//	ncd->dict                    = ncd->dict_client ? bonobo_object_corba_objref (BONOBO_OBJECT (ncd->dict_client)) : NULL;
	//ncd->gdk_painter             = NULL;
	//ncd->plain_painter           = NULL;
	ncd->format_html             = FALSE;
	ncd->html_modified	= FALSE;
	ncd->note_editor		= FALSE;

	return ncd;
}

void
gtk_html_control_data_destroy (GtkHTMLControlData *cd)
{
	g_assert (cd);

/*	if (cd->search_dialog)
		gtk_html_search_dialog_destroy (cd->search_dialog);

	if (cd->replace_dialog)
		gtk_html_replace_dialog_destroy (cd->replace_dialog);

	if (cd->dict_client)
		bonobo_object_unref (BONOBO_OBJECT (cd->dict_client));
*/
	g_free (cd);
}
