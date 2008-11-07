/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2005 Don Scorgie <DonScorgie@Blueyonder.co.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Don Scorgie <DonScorgie@Blueyonder.co.uk>
 */

#ifndef GECKO_PRINT_H
#define GECKO_PRINT_H

#include <glib.h>
#include <gtk/gtk.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef USE_GTKUPRINT
/* Needed to fill the struct */
#include <gtk/gtkprinter.h>
#include <gtk/gtkprintsettings.h>


G_BEGIN_DECLS


typedef struct _GeckoPrintInfo
{
    GtkPrintSettings *config;
    GtkPageSetup     *setup;
    GtkPrinter       *printer;
    GtkWidget        *print_dialog;

    char *tempfile;
    guint print_idle_id;
    guint cancel_print_id;

    gboolean cancelled;
    gboolean moz_finished;
    gboolean started;
    gboolean previewed;
    
    char *header_left_string;
    char *header_center_string;
    char *header_right_string;
    char *footer_left_string;
    char *footer_center_string;
    char *footer_right_string;
    GtkWindow *owner;
    GtkWidget *dialog;
    gpointer html_frame;
    GtkWidget *progress;
    GtkWidget *fake_win;
    GtkWidget *content_box;

    /* Preview buttons */
    GtkWidget *GoBack;
    GtkWidget *GoForward;
    GtkWidget *GoFirst;
    GtkWidget *GoLast;
    GtkWidget *Close;
    gint npages;
    gint currentpage;


} GeckoPrintInfo;

void          gecko_print_run               (GtkWindow *window, 
					    gpointer html,
					    gpointer fake_win,
					    gpointer content_box);
void          gecko_print_moz_finished      (GeckoPrintInfo *info);
void          gecko_print_cancel            (GeckoPrintInfo *info);
void          gecko_print_info_free         (GeckoPrintInfo *info);
void          gecko_print_update_progress   (GeckoPrintInfo *info,
					    gdouble percentage);
G_END_DECLS

#endif
#endif
