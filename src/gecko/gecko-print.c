/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2002 Jorn Baayen
 * Copyright (C) 2003, 2004 Christian Persch
 * Copyright (C) 2005 Juerg Billeter
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
 */

#include <unistd.h>
#include <gnome.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include "gecko/gecko-print.h"
#include "gecko/gecko-html.h"
//#include "gs-utils.h"
#include <gtk/gtkprintunixdialog.h>

static GtkPrintSettings * gecko_print_load_config_from_file ( void );
static void               gecko_print_save_config_to_file   (GtkPrintSettings *config);
static void               cancel_print_cb                  (GtkDialog *dialog, 
							    gint arg1, 
							    GeckoPrintInfo *info);
static void               parent_destroyed_cb              (GtkWindow *window, 
							    GeckoPrintInfo *info);
static gboolean           print_jobs_run                   ( void );
void                      print_present_config_dialog      (GeckoPrintInfo *info);
static GeckoPrintInfo    * gecko_print_get_print_info        ( void );
void                      gecko_print_info_free             (GeckoPrintInfo *info);
gboolean                  gecko_print_verify_postscript     (GtkPrinter *printer,
							    GtkWidget *print_dialog);
void                      gecko_print_present_status_dialog (GtkWindow *window,
							    GeckoPrintInfo *info);
gboolean                 gecko_print_preview                (GeckoPrintInfo *info);

gboolean                 print_preview_finished_cb         (GtkWindow     *win,
							    GdkEvent      *ev,
							    GeckoPrintInfo *info);
void                     preview_go_first                  (GtkToolButton *b,
							    GeckoPrintInfo *info);
void                     preview_go_back                   (GtkToolButton *b,
							    GeckoPrintInfo *info);
void                     preview_go_forward                (GtkToolButton *b,
							    GeckoPrintInfo *info);
void                     preview_go_last                   (GtkToolButton *b,
							    GeckoPrintInfo *info);
void                     preview_close                     (GtkToolButton *b,
							    GeckoPrintInfo *info);
gboolean                 print_free_idle_cb                (GeckoPrintInfo *info);
void                     gecko_finish_printing              (GtkPrintJob *job,
							   GeckoPrintInfo *info,
							    GError *error);

static GSList * current_jobs = NULL;

static gboolean currently_running = FALSE;

void
gecko_print_run (GtkWindow *window, gpointer html, gpointer fake_win, 
		gpointer content_box)
{
    GeckoPrintInfo *info;
    
    info = gecko_print_get_print_info ();
    info->owner = window;
    info->html_frame = html;
    info->fake_win = fake_win;
    info->content_box = content_box;
    info->previewed = FALSE;

    print_present_config_dialog (info);
    
}

void
print_present_config_dialog (GeckoPrintInfo *info)
{
    int ret;

    if (!info->print_dialog) {
	info->print_dialog = gtk_print_unix_dialog_new ("Print Dialog", 
							GTK_WINDOW (info->owner));
	gtk_print_unix_dialog_set_settings (
				  GTK_PRINT_UNIX_DIALOG (info->print_dialog), 
				  info->config);
	gtk_print_unix_dialog_set_page_setup (
					GTK_PRINT_UNIX_DIALOG (info->print_dialog),
					info->setup);
	gtk_print_unix_dialog_set_manual_capabilities (
				       GTK_PRINT_UNIX_DIALOG (info->print_dialog), 
				       GTK_PRINT_CAPABILITY_PAGE_SET |
				       GTK_PRINT_CAPABILITY_COPIES   |
				       GTK_PRINT_CAPABILITY_COLLATE  |
				       GTK_PRINT_CAPABILITY_REVERSE  |
				       GTK_PRINT_CAPABILITY_SCALE |
				       GTK_PRINT_CAPABILITY_GENERATE_PS |
				       GTK_PRINT_CAPABILITY_PREVIEW);
    }
    while (1) {
	
	ret = gtk_dialog_run (GTK_DIALOG (info->print_dialog));
	info->config = 
	   gtk_print_unix_dialog_get_settings (
				   GTK_PRINT_UNIX_DIALOG (info->print_dialog));
	info->setup = 
	    gtk_print_unix_dialog_get_page_setup (GTK_PRINT_UNIX_DIALOG 
						  (info->print_dialog));
	info->printer = 
	    gtk_print_unix_dialog_get_selected_printer (GTK_PRINT_UNIX_DIALOG 
							(info->print_dialog));
	g_object_ref (info->printer);
	if (ret != GTK_RESPONSE_OK)
	    break;
	if (gecko_print_verify_postscript (info->printer, info->print_dialog)) 
	    break;

    }

    gtk_widget_hide (info->print_dialog);
    if (ret == GTK_RESPONSE_APPLY) {
	g_idle_add ((GSourceFunc) gecko_print_preview, info);
	return;
    }
    if (ret != GTK_RESPONSE_OK) {
	gecko_print_info_free (info);
	return;
    }
    gecko_print_save_config_to_file (info->config);
    gecko_print_present_status_dialog (info->owner, info);
    info->cancel_print_id = g_signal_connect (info->owner, "destroy",
					      G_CALLBACK (parent_destroyed_cb), 
					      info);


    current_jobs = g_slist_append (current_jobs, info);

    if (!currently_running) {
	g_idle_add ((GSourceFunc) print_jobs_run,
		    NULL);
	currently_running = TRUE;
    }

}

static gboolean 
print_jobs_run ()
{
    GeckoPrintInfo * info = current_jobs->data;
    info->started = TRUE;
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (info->dialog),
    				      _("Preparing to print"));

    gecko_html_print (info->html_frame, info, FALSE, NULL);

    return FALSE;
}

static GtkPrintSettings * 
gecko_print_load_config_from_file ()
{
    GtkPrintSettings *settings;

    settings = gtk_print_settings_new ();
    
    // * TODO: Load settings from a file somehow 
    return settings;
}

static void
gecko_print_save_config_to_file (GtkPrintSettings *config)
{
    /* TODO: Save settings to a file somehow */
    return;
}

void
gecko_print_info_free (GeckoPrintInfo *info)
{
    if (info) {
	g_object_unref (info->config);
	g_object_unref (info->setup);
	g_object_unref (info->printer);
	gtk_widget_destroy (info->print_dialog);
	if (info->tempfile) {
	    g_unlink (info->tempfile);
	    g_free (info->tempfile);
	}

	g_free (info->header_left_string);
	g_free (info->header_center_string);
	g_free (info->header_right_string);
	g_free (info->footer_left_string);
	g_free (info->footer_center_string);
	g_free (info->footer_right_string);
	if (info->dialog != NULL) {
	    gtk_widget_destroy (info->progress);
	    gtk_widget_destroy (info->dialog);
	}
	if (info->fake_win)
	    gtk_widget_destroy (info->fake_win);
	if (info->cancel_print_id) {
	    g_signal_handler_disconnect (info->owner, info->cancel_print_id);
	}
	g_free (info);
    }
    
}

GeckoPrintInfo *
gecko_print_get_print_info ()
{
  GeckoPrintInfo *info;
  
  info = g_new0 (GeckoPrintInfo, 1);

  info->config = gecko_print_load_config_from_file ();

  info->setup = gtk_page_setup_new ();

  info->cancelled = FALSE;
  info->moz_finished = FALSE;

  info->dialog = NULL;
  info->print_dialog = NULL;

  info->header_left_string = g_strdup ("&T");
  info->header_center_string = g_strdup ("");
  info->header_right_string = g_strdup ("");
  info->footer_left_string = g_strdup ("");
  info->footer_center_string = g_strdup ("&PT");
  info->footer_right_string = g_strdup ("");

  return info;
}

gboolean
gecko_print_verify_postscript (GtkPrinter *printer, GtkWidget *print_dialog)
{
    if (!gtk_printer_accepts_ps (printer)) {
	GtkDialog *dialog;
	dialog = GTK_DIALOG(gtk_message_dialog_new ( GTK_WINDOW (print_dialog),
					  GTK_DIALOG_MODAL,
					  GTK_MESSAGE_ERROR,
					  GTK_BUTTONS_OK,
					  _("Printing is not supported on this"
					    " printer")));
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
						  _("Printer %s does not"
						    " support postscript "
						    "printing."), 
						  gtk_printer_get_description (printer));
	  gtk_dialog_run (GTK_DIALOG (dialog));
	  gtk_widget_destroy (GTK_WIDGET (dialog));
	  
	  return FALSE;
    }

    return TRUE;
}

static gboolean
print_idle_cb (GeckoPrintInfo *info)
{
    GtkPrintJob *job;
    
    if (g_file_test (info->tempfile, G_FILE_TEST_EXISTS) == FALSE) 
	return FALSE;
    job = gtk_print_job_new (gtk_window_get_title (GTK_WINDOW (info->owner)), 
			     info->printer,
			     info->config, info->setup);
    gtk_print_job_set_source_file (job, info->tempfile, NULL);
    gtk_print_job_send (job, (GtkPrintJobCompleteFunc) gecko_finish_printing,
			info, NULL);

    return FALSE;

}

static void  
parent_destroyed_cb (GtkWindow *window, GeckoPrintInfo *info)
{
    current_jobs = g_slist_remove (current_jobs, info);
    if (info->started) {
	info->cancelled = TRUE;
	if (info->moz_finished) {
	    if (info->print_idle_id)
		g_source_remove (info->print_idle_id);
	    gecko_print_info_free (info);
	}
    } else {
	gecko_print_info_free (info);
    }
}

static void
cancel_print_cb (GtkDialog *dialog, gint arg1, GeckoPrintInfo *info)
{
    gtk_widget_hide (info->dialog);
    if (info->started) {
	info->cancelled = TRUE;
	if (info->moz_finished) {
	    if (info->print_idle_id)
		g_source_remove (info->print_idle_id);
	    gecko_print_info_free (info);
	}
    } else {
	current_jobs = g_slist_remove (current_jobs, info);
	gecko_print_info_free (info);
    }
}

void 
gecko_print_moz_finished (GeckoPrintInfo *info)
{
    info->moz_finished = TRUE;

    current_jobs = g_slist_remove (current_jobs, info);

    if (!info->cancelled) {
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (info->dialog),
						  _("Printing"));
	
	info->print_idle_id = g_idle_add ((GSourceFunc) print_idle_cb,
				  info);

    } else {
	if (g_slist_length (current_jobs) == 0) {
	    currently_running = FALSE; 
	} else {
	    g_idle_add ((GSourceFunc) print_jobs_run,
			NULL);
	}
	g_idle_add ((GSourceFunc) print_free_idle_cb, info);
    }

}

void
gecko_print_present_status_dialog (GtkWindow *window, GeckoPrintInfo *info)
{
    if (info->dialog != NULL)
	return;
    
    info->dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW (window), 
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_MESSAGE_INFO,
						       GTK_BUTTONS_CANCEL,
						       "<b>%s</b>",
						       _("Printing"));
    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (info->dialog),
					      _("Waiting to print"));
    info->progress = gtk_progress_bar_new ();
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(info->dialog)->vbox),
		       info->progress);

    gtk_widget_show (info->progress);

    g_signal_connect (info->dialog, "response",
		      G_CALLBACK (cancel_print_cb),
		      (gpointer) info);

    gtk_window_present (GTK_WINDOW (info->dialog));
}

void 
gecko_print_cancel (GeckoPrintInfo *info)
{
    cancel_print_cb (NULL, 0, info);
}

void
gecko_print_update_progress (GeckoPrintInfo *info, gdouble percentage)
{
    /* This is horribly inefficient, but since we're waiting for
     * mozilla to do its thing, might as well waste a few cycles
     */
    GeckoPrintInfo * temp;
    GSList *jobs = current_jobs;
    while (g_slist_length (jobs) != 0) {
	temp = jobs->data;
	gtk_progress_bar_pulse (GTK_PROGRESS_BAR (temp->progress));
	jobs = g_slist_next(jobs);
    }
    jobs = g_slist_find (current_jobs, info);
    temp = jobs->data;
    if (percentage > 1.0)
	percentage = 1.0;
    else if (percentage <0.0)
	percentage = 0.0;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (temp->progress), 
				   percentage);

}

gboolean
gecko_print_preview (GeckoPrintInfo *info)
{
    gint width, height;
    /* Add some funky nav widgets to the fake_win since we're gonna
     * have to actually show it now :(
     */
    if (!info->previewed) {
	GtkWidget *box;
	info->previewed = TRUE;
	box = gtk_toolbar_new ();
	
	gtk_box_pack_start (GTK_BOX (info->content_box), box, FALSE, FALSE, 0);
	info->GoFirst = GTK_WIDGET (gtk_tool_button_new_from_stock (GTK_STOCK_GOTO_FIRST));
	gtk_container_add (GTK_CONTAINER (box), info->GoFirst);
	g_signal_connect (info->GoFirst, "clicked",
			  G_CALLBACK (preview_go_first),
			  info);
	gtk_widget_show (info->GoFirst);
	
	info->GoBack = GTK_WIDGET (gtk_tool_button_new_from_stock (GTK_STOCK_GO_BACK));
	gtk_container_add (GTK_CONTAINER (box), info->GoBack);
	g_signal_connect (info->GoBack, "clicked",
			  G_CALLBACK (preview_go_back),
			  info);
	gtk_widget_show (info->GoBack);
	
	info->GoForward = GTK_WIDGET (gtk_tool_button_new_from_stock (GTK_STOCK_GO_FORWARD));
	gtk_container_add (GTK_CONTAINER (box), info->GoForward);
	g_signal_connect (info->GoForward, "clicked",
			  G_CALLBACK (preview_go_forward),
			  info);
	gtk_widget_show (info->GoForward);
	
	info->GoLast = GTK_WIDGET (gtk_tool_button_new_from_stock (GTK_STOCK_GOTO_LAST));
	gtk_container_add (GTK_CONTAINER (box), info->GoLast);
	g_signal_connect (info->GoLast, "clicked",
			  G_CALLBACK (preview_go_last),
			  info);
	gtk_widget_show (info->GoLast);
	
	info->Close = GTK_WIDGET (gtk_tool_button_new_from_stock (GTK_STOCK_CLOSE));
	gtk_container_add (GTK_CONTAINER (box), info->Close);
	g_signal_connect (info->Close, "clicked",
			  G_CALLBACK (preview_close),
			  info);
	gtk_widget_show (info->Close);
	
	gtk_widget_show (box);
    }
    gtk_window_set_modal (GTK_WINDOW (info->fake_win), TRUE);
    gtk_widget_set_sensitive (info->GoFirst, FALSE);
    gtk_widget_set_sensitive (info->GoBack, FALSE);
    info->currentpage = 1;
    gecko_html_print (info->html_frame, info, TRUE, &(info->npages));

    if (info->npages == 1) { /*Desensitise all buttons */
	gtk_widget_set_sensitive (info->GoForward, FALSE);
	gtk_widget_set_sensitive (info->GoLast, FALSE);
    }
    g_signal_connect (info->fake_win, "delete-event",
		      G_CALLBACK (print_preview_finished_cb),
		      info);
    
    /* Set the preview window to the same size as the real window */
    gtk_window_get_size (GTK_WINDOW (info->owner), &width, &height);
    gtk_window_resize (GTK_WINDOW (info->fake_win), width, height);

    gtk_container_set_focus_child (GTK_CONTAINER (info->fake_win),
				   GTK_WIDGET (info->html_frame));

    gtk_widget_show (GTK_WIDGET (info->fake_win));

    return FALSE;
}

gboolean
print_preview_finished_cb (GtkWindow *win, GdkEvent *ev, GeckoPrintInfo *info)
{
    gtk_widget_hide (GTK_WIDGET (info->fake_win));
    gecko_html_preview_end (info->html_frame);
    print_present_config_dialog (info);
    return TRUE;
}

void
preview_go_first (GtkToolButton *b, GeckoPrintInfo *info)
{
    info->currentpage = 1;
    gecko_html_preview_navigate (info->html_frame, info->currentpage);

    /* Reset sensitives */
    gtk_widget_set_sensitive (info->GoBack, FALSE);
    gtk_widget_set_sensitive (info->GoFirst, FALSE);
    gtk_widget_set_sensitive (info->GoForward, TRUE);
    gtk_widget_set_sensitive (info->GoLast, TRUE);
}

void
preview_go_back (GtkToolButton *b, GeckoPrintInfo *info)
{
    info->currentpage--;
    gecko_html_preview_navigate (info->html_frame, info->currentpage);

    /* Reset sensitives */
    gtk_widget_set_sensitive (info->GoForward, TRUE);
    gtk_widget_set_sensitive (info->GoLast, TRUE);

    if (info->currentpage == 1) {
	gtk_widget_set_sensitive (info->GoBack, FALSE);
	gtk_widget_set_sensitive (info->GoFirst, FALSE);
    }
}

void
preview_go_forward (GtkToolButton *b, GeckoPrintInfo *info)
{
    info->currentpage++;
    gecko_html_preview_navigate (info->html_frame, info->currentpage);
    /* Reset sensitives */
    gtk_widget_set_sensitive (info->GoBack, TRUE);
    gtk_widget_set_sensitive (info->GoFirst, TRUE);

    if (info->currentpage == info->npages) {
	gtk_widget_set_sensitive (info->GoForward, FALSE);
	gtk_widget_set_sensitive (info->GoLast, FALSE);
    }
}

void
preview_go_last (GtkToolButton *b, GeckoPrintInfo *info)
{
    info->currentpage=info->npages;
    gecko_html_preview_navigate (info->html_frame, info->currentpage);
    
    /* Reset sensitives */
    gtk_widget_set_sensitive (info->GoBack, TRUE);
    gtk_widget_set_sensitive (info->GoFirst, TRUE);
    gtk_widget_set_sensitive (info->GoForward, FALSE);
    gtk_widget_set_sensitive (info->GoLast, FALSE);

}

void 
preview_close (GtkToolButton *b, GeckoPrintInfo *info)
{
    print_preview_finished_cb (NULL, NULL, info);
}

gboolean
print_free_idle_cb (GeckoPrintInfo *info)
{
    gecko_print_info_free (info);
    return FALSE;
}

void
gecko_finish_printing (GtkPrintJob *job, GeckoPrintInfo *info, GError *error)
{
    if (error) {
	// *There was an error printing.  Panic.
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new          (NULL,
						  GTK_DIALOG_MODAL |
						  GTK_DIALOG_DESTROY_WITH_PARENT,
						  GTK_MESSAGE_WARNING,
						  GTK_BUTTONS_OK,
						  _("An error "
						    "occurred while printing")
						  );
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
						  _("It was not possible to "
						    "print your document: %s"),
						  error->message);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_hide (dialog);
	gtk_widget_destroy (dialog);
    }
    
    gecko_print_info_free (info);
    
    if (g_slist_length (current_jobs) > 0)
	g_idle_add ((GSourceFunc) print_jobs_run, NULL);
    else
	currently_running = FALSE;
    
}

