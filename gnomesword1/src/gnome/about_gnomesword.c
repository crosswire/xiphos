/*
 * GnomeSword Bible Study Tool
 * abouts_gnomesword.c - GnomeSword about dialog
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
#include <config.h>
#endif

#include <gnome.h>

#include "about_gnomesword.h"

/******************************************************************************
 * public
 *****************************************************************************/

/******************************************************************************
 * Name
 *   gui_create_about_gnomesword
 *
 * Synopsis
 *   #include "about_gnomesword.h"
 *
 *   void gui_create_about_gnomesword(void);
 *
 * Description
 *   Create GnomeSword about dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_about_gnomesword(void)
{
        const gchar *authors[] = {
                "Terry Biggs  tbiggs@users.sf.net",
                "Moses McKnight  m_mcknight@users.sf.net",
                "Jan Paul Schmidt japas@users.sf.net",
                "Victor Porton  porton@users.sf.net",
                "Stephen Binks steveb@icenet.com.au",
                "Carl Constantine duckwing@users.sf.net",
		"Jorge Chacon  jgchacon@itelgua.com",
		"Jp Robinson  robinson@knightmail.com",
                NULL
        };
        GtkWidget *about;

        about = gnome_about_new(
                "gnomesword", VERSION,
                "Copyright 2000-2002", authors,
                _(
                "We would like to thank Troy Griffitts and all the other "
                "folks who have given us The SWORD Project."
		),
                "gnomesword/about.png"
        );
        gtk_object_set_data(GTK_OBJECT(about), "about", about);
        gtk_container_set_border_width(GTK_CONTAINER(about), 4);
        gtk_window_set_modal(GTK_WINDOW(about), TRUE);

        return about;
}
