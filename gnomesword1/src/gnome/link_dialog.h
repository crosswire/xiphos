/*
 * GnomeSword Bible Study Tool
 * link_dialog.h - dialog for creating links in editors
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

#ifndef __LINK_DIALOG_H_
#define __LINK_DIALOG_H_

#include <gnome.h>
#include "gs_gnomesword.h"
#include "gs_editor.h"

void set_link_to_module(gchar * linkref, gchar * linkmod,
			GSHTMLEditorControlData * ecd);
GtkWidget *create_link_dialog(GSHTMLEditorControlData * ecd);

#endif	/* __LINK_DIALOG_H_ */
