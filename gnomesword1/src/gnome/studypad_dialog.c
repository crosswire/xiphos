/*
 * GnomeSword Bible Study Tool
 * editor_dialog.c - dialog for detached editors
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
#include <gal/e-paned/e-hpaned.h>

#include "gui/studypad_dialog.h"
#include "gui/utilities.h"
#include "gui/gnomesword.h"

#include "main/settings.h"

/******************************************************************************
 * Name
 *   editor_dialog_destroy
 *
 * Synopsis
 *   #include "editor_dialog.h"
 *   
 *   void editor_dialog_destroy(GtkObject *object, gpointer user_data)
 *
 * Description
 *   
 *   
 *
 * Return value
 *   void
 */

static void studypad_dialog_destroy(GtkObject *object, gpointer user_data)
{
	/* we need the if to prevent a loop */
	/*if(!settings.docked)
		gui_attach_detach_studypad();
	*/
}

/******************************************************************************
 * Name
 *   create_editor_dialog
 *
 * Synopsis
 *   #include "editor_dialog.h"
 *   
 *   GtkWidget*  create_editor_dialog(void)
 *
 * Description
 *   create dialog to hold the editors when detached
 *   
 *
 * Return value
 *   GtkWidget*
 */

static GtkWidget* create_studypad_dialog(void)
{
	
}

/******************************************************************************
 * Name
 *   gui_attach_detach_editor
 *
 * Synopsis
 *   #include "editor_dialog.h"
 *   
 *   void gui_attach_detach_editor(void)
 *
 * Description
 *   dock/undock editors
 *   
 *
 * Return value
 *   
 */

void gui_attach_detach_studypad(void)
{
	
}


