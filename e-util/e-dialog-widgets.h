/* Evolution internal utilities - Glade dialog widget utilities
 *
 * Copyright (C) 2000 Helix Code, Inc.
 *
 * Author: Federico Mena-Quintero <federico@gimp.org>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef E_DIALOG_WIDGETS_H
#define E_DIALOG_WIDGETS_H

#include <time.h>
#include <glade/glade.h>
#include <libgnomeui/gnome-propertybox.h>



void e_dialog_editable_set (GtkWidget *widget, char *value);
char *e_dialog_editable_get (GtkWidget *widget);

void e_dialog_radio_set (GtkWidget *widget, int value, const int *value_map);
int e_dialog_radio_get (GtkWidget *widget, const int *value_map);

void e_dialog_toggle_set (GtkWidget *widget, gboolean value);
gboolean e_dialog_toggle_get (GtkWidget *widget);

void e_dialog_spin_set (GtkWidget *widget, double value);
double e_dialog_spin_get_double (GtkWidget *widget);
int e_dialog_spin_get_int (GtkWidget *widget);

void e_dialog_option_menu_set (GtkWidget *widget, int value, const int *value_map);
int e_dialog_option_menu_get (GtkWidget *widget, const int *value_map);

void e_dialog_dateedit_set (GtkWidget *widget, time_t t);
time_t e_dialog_dateedit_get (GtkWidget *widget);

gboolean e_dialog_widget_hook_value (GtkWidget *dialog, GtkWidget *widget,
				     gpointer value_var, gpointer info);

void e_dialog_get_values (GtkWidget *dialog);

gboolean e_dialog_xml_widget_hook_value (GladeXML *xml, GtkWidget *dialog, const char *widget_name,
					 gpointer value_var, gpointer info);



#endif
