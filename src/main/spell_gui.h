/* 
 * added to gnomesword 2001-01-06
*/

/* Bluefish HTML Editor
 * spell_gui.h - Spell Cheking GUI prototypes
 *
 * Copyright (C) 1998 Olivier Sessink and Chris Mazuc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __SPELL_GUI_H_
#define __SPELL_GUI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>
#include <gtk/gtkwidget.h>
#include "gui/_editor.h"

void spell_check_cb(GtkWidget * w, GSHTMLEditorControlData * ecd);
GtkWidget* create_spc_window (GSHTMLEditorControlData *ecd);

#ifdef __cplusplus
}
#endif

#endif

