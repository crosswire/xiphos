/*
 * Xiphos Bible Study Tool
 * about_xiphos.c - Xiphos about dialog
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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

#include "gui/about_xiphos.h"
#include "gui/utilities.h"
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
    "Karl Kleinpaste",
    NULL
};

static gchar *translators = "Jan Paul Schmidt\nJorge Chacon\nDominique Corbex\nZdenko Podobn\nPavlo Bohmat\nMartin Zibricky\nAri Constancio\nGertjan Francke\nEicke Godehardt";

/******************************************************************************
 * Name
 *   on_dialog_response
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_dialog_response(GtkDialog * dialog, gint response_id, 
					gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void
on_dialog_response(GtkDialog * dialog, gint response_id, 
		   gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(dialog));
}


/******************************************************************************
 * Name
 *   gui_create_about_xiphos
 *
 * Synopsis
 *   #include "gui/about_xiphos.h"
 *
 *   void gui_create_about_xiphos(void);
 *
 * Description
 *   Create Xiphos about dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget*
gui_create_about_xiphos(void)
{
	GdkPixbuf *about1_logo_pixbuf;
	GtkWidget *about1;
	gchar versionbuild[128];

	snprintf
	    (versionbuild, 120, "%s-%s", VERSION,
#ifdef USE_GTKMOZEMBED
	     "mozembed"
#else
	     "gtkhtml3"
#endif
		);

	about1_logo_pixbuf = pixbuf_finder("about.png", NULL);

	about1 = gtk_about_dialog_new();
  
	g_signal_connect(about1, "response",
			 G_CALLBACK(on_dialog_response), NULL);
  
	gtk_about_dialog_set_name
	    (GTK_ABOUT_DIALOG (about1), PACKAGE);
	gtk_about_dialog_set_version
	    (GTK_ABOUT_DIALOG (about1), versionbuild);
	gtk_about_dialog_set_copyright
	    (GTK_ABOUT_DIALOG (about1),
	     _("Copyright 2000-2009 The Xiphos Development Team"));
	gtk_about_dialog_set_comments
	    (GTK_ABOUT_DIALOG (about1),
	     _("(formerly known as GnomeSword)\nWe would like to thank Troy Griffitts and all the other folks who have given us The SWORD Project."));
	gtk_about_dialog_set_website
	    (GTK_ABOUT_DIALOG (about1), "http://gnomesword.sourceforge.net/");
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
