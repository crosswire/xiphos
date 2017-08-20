/*
 * Xiphos Bible Study Tool
 * gtk_compat.h -
 *
 * Copyright (C) 2011-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef __GTK_COMPAT_H__
#define __GTK_COMPAT_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>

#if !GTK_CHECK_VERSION(2, 20, 0)
inline gboolean xi_gtk_widget_get_realized(GtkWidget *widget)
{
	return GTK_WIDGET_REALIZED(widget);
}

#define gtk_widget_get_realized xi_gtk_widget_get_realized
#endif
#endif
