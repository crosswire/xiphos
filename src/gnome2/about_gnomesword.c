/*
 * GnomeSword Bible Study Tool
 * about_gnomesword.c - GnomeSword about dialog
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

#include <bonobo.h>
#include <gnome.h>

#include "gui/about_gnomesword.h"
#include "main/settings.h"

/******************************************************************************
 * public
 */

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
        GtkWidget *about;
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	
        const gchar *authors[] = {
                "Terry Biggs",
                "Moses McKnight",
                "Jan Paul Schmidt",
                "Victor Porton",
                "Stephen Binks",
                "Carl Constantine",
		"Jp Robinson",
                NULL
        };	
	const gchar *documenters[] = { NULL };
	/* TRANSLATORS: Replace this string with your names, one name per line. */
	gchar *translators = "Jan Paul Schmidt\nJorge Chacon\nDominique Corbex\nZdenko Podobný";
	pixbuf = gdk_pixbuf_new_from_file (PACKAGE_PIXMAPS_DIR"/about.png", &error);
	if(!pixbuf){
		fprintf (stderr, "pixmap file error: %s\n", error->message);
		g_error_free (error);
		error = NULL;
	}
        about = gnome_about_new(
                settings.program_title, VERSION,
                "Copyright 2000-2003 The GnomeSword Development Team", 
                _("We would like to thank Troy Griffitts and all the other "
		"folks who have given us The SWORD Project."),
			authors,
                        documenters,
                        translators,		
                	pixbuf
        ); 	
        return about;
}
