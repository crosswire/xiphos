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

    Author: Ettore Perazzoli <ettore@helixcode.com>
*/

/*
 * added to gnomesword Mon Dec 17 12:51:37 2001
 *
 */

#ifndef _GS_EDITOR_TOOLBAR_H
#define _GS_EDITOR_TOOLBAR_H

#include <gtkhtml/gtkhtml.h>
#include "gs_html_editor.h"

GtkWidget *toolbar_style         (GSHTMLEditorControlData * ecd);
void       toolbar_update_format (GSHTMLEditorControlData * ecd);
#endif /* _GS_EDITOR_TOOLBAR_H */
