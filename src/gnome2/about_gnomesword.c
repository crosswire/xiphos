/*
 * GnomeSword Bible Study Tool
 * about_gnomesword.c - GnomeSword about dialog
 *
 * Copyright (C) 2000-2006 GnomeSword Developer Team
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

#include "gui/about_gnomesword.h"
#include "main/settings.h"

static const gchar *authors[] = {
    "Terry Biggs",
    "Moses McKnight",
    "Jan Paul Schmidt",
    "Victor Porton",
    "Stephen Binks",
    "Carl Constantine",
    "Jp Robinson",
    "Andy Piper",
     "Karl Kleinpaste",
    NULL
};
  
static const gchar *documenters[] = { 
    "Pierre Benz",
    "Andy Piper",
    "Peter von Kaehne",
    NULL
};

static gchar *translators = "Jan Paul Schmidt\nJorge Chacon\nDominique Corbex\nZdenko Podobn\nPavlo Bohmat";

/******************************************************************************
 * Name
 *   gui_create_about_gnomesword
 *
 * Synopsis
 *   #include "gui/about_gnomesword.h"
 *
 *   void gui_create_about_gnomesword(void);
 *
 * Description
 *   Create GnomeSword about dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget*
gui_create_about_gnomesword(void)
{
  GdkPixbuf *about1_logo_pixbuf;
  GtkWidget *about1;
  GError *error = NULL;

  about1_logo_pixbuf = gdk_pixbuf_new_from_file (PACKAGE_PIXMAPS_DIR"/about.png", &error);

  about1 = gtk_about_dialog_new ();
  gtk_about_dialog_set_name
          (GTK_ABOUT_DIALOG (about1), PACKAGE);
  gtk_about_dialog_set_version
          (GTK_ABOUT_DIALOG (about1), VERSION);
  gtk_about_dialog_set_copyright
          (GTK_ABOUT_DIALOG (about1),
           _("Copyright 2000-2006 The GnomeSword Development Team"));
  gtk_about_dialog_set_comments
          (GTK_ABOUT_DIALOG (about1),
           _("We would like to thank Troy Griffitts and all the other folks who have given us The SWORD Project."));
  gtk_about_dialog_set_authors
          (GTK_ABOUT_DIALOG (about1), authors);
  gtk_about_dialog_set_documenters
          (GTK_ABOUT_DIALOG (about1), documenters);
  gtk_about_dialog_set_translator_credits
          (GTK_ABOUT_DIALOG (about1), translators);
  gtk_about_dialog_set_logo
          (GTK_ABOUT_DIALOG (about1), about1_logo_pixbuf);

  return about1;
}
