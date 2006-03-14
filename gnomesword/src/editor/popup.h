/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* This file is part of the GtkHTML library

   Copyright (C) 2000 Helix Code, Inc.
   Authors:           Radek Doulik (rodo@helixcode.com)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHcANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _POPUP_H_
#define _POPUP_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifndef USE_GTKHTML38
#include "editor/editor.h"

gint  popup_show            (GSHTMLEditorControlData *cd,
			     GdkEventButton     *event);
gint  popup_show_at_cursor  (GSHTMLEditorControlData *cd);
void  property_dialog_show  (GSHTMLEditorControlData *cd);

#endif
#endif
