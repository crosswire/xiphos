/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_dock_dlg.c
     * -------------------
     * Sat Feb  2 15:32:56 2002 
     * copyright (C) 2002 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include "gs_undock_sb.h"
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"


static void
on_dlgDock_destroy(GtkObject       *object,
                                        SETTINGS *s)
{
	if(!s->docked)
		on_btnSBDock_clicked(NULL, s);
}

GtkWidget*
create_dlgDock (SETTINGS *s)
{
  GtkWidget *dlgDock; 

  dlgDock = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dlgDock), "dlgDock", dlgDock);
  gtk_window_set_title (GTK_WINDOW (dlgDock), _("GnomeSWORD"));
  gtk_window_set_policy (GTK_WINDOW (dlgDock), TRUE, TRUE, FALSE);
  gtk_widget_set_usize(dlgDock, s->shortcutbar_width, -2);
	
  s->vboxDock = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (s->vboxDock);
  gtk_object_set_data_full (GTK_OBJECT (dlgDock), "s->vboxDock", s->vboxDock,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (s->vboxDock);
  gtk_container_add (GTK_CONTAINER (dlgDock), s->vboxDock);
	
  gtk_signal_connect (GTK_OBJECT (dlgDock), "destroy",
                      GTK_SIGNAL_FUNC (on_dlgDock_destroy),
                      s);

  return dlgDock;
}

