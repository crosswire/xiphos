/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*  This file is part of the GtkHTML library.

    Copyright (C) 2000 Helix Code, Inc.
    Authors:           Radek Doulik (rodo@helixcode.com)

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

#include <config.h>
#include "dialog.h"

/*
  somewhat ugly (those *** :) so I feel like I may comment
  as first argument it takes poiter to pointer on Dialog struct
  which MUST contains GnomeDialog * as first field

  it is bassicaly the same trick as used in GtkObject's
*/

void
run_dialog (GnomeDialog ***dialog, GtkHTML *html, DialogCtor ctor)
{
	if (*dialog) {
		gtk_widget_show (GTK_WIDGET (**dialog));
		gdk_window_raise (GTK_WIDGET (**dialog)->window);
	} else {
		*dialog = ctor (html);
		gtk_widget_show (GTK_WIDGET (**dialog));
	}
}
