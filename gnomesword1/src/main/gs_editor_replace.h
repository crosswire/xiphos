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

/* 
 * added to gnomesword Fri Dec 14 21:53:23 2001
*/

#ifndef _GS_EDITOR_REPLACE_H_
#define _GS_EDITOR_REPLACE_H_

#include <gnome.h>
#include "gs_editor.h"


typedef struct _GtkHTMLReplaceAskDialog GtkHTMLReplaceAskDialog;

typedef struct _GtkHTMLReplaceDialog GtkHTMLReplaceDialog;


	

GtkHTMLReplaceDialog * gs_editor_replace_dialog_new     (GtkHTML *html);
void                   gs_editor_replace_dialog_destroy (GtkHTMLReplaceDialog *d);


#endif /* _GS_EDITOR_REPLACE_H_ */
