/*
 * GnomeSword Bible Study Tool
 * utilities.c - support functions
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "gui/utilities.h"


/******************************************************************************
 * Name
 *   gui_lookup_widget
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   GtkWidget *gui_lookup_widget(GtkWidget * widget, 
				const gchar * widget_name)
 *
 * Description
 * This function returns a widget in a component.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.   
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_lookup_widget(GtkWidget * widget, 
				const gchar * widget_name)
{
	GtkWidget *parent, *found_widget;

	for (;;) {
		if (GTK_IS_MENU(widget))
			parent =
			    gtk_menu_get_attach_widget(GTK_MENU(widget));
		else
			parent = widget->parent;
		if (parent == NULL)
			break;
		widget = parent;
	}

	found_widget =
	    (GtkWidget *) gtk_object_get_data(GTK_OBJECT(widget),
					      widget_name);
	if (!found_widget)
		g_warning("Widget not found: %s", widget_name);
	return found_widget;
}

/******   end of file   ******/
