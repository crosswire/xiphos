/* 
 * added to gnomesword 2001-01-06  for spell checking
*/

/* Bluefish HTML Editor
 * spell_gui.c - Check Spelling GUI
 *
 * Copyright (C) 2000 Pablo De Napoli
 *
 * The code in this file was writen with the aid of glade.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef WITH_SPC

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

//#include "bluefish.h"
//#include "intl.h"
#include "debug.h"
#include "spell.h"
#include "spell_gui.h"
//#include "configure.h"
#include "interface.h"
#include "support.h"

extern GtkWidget *text_widget;


static gboolean spc_is_running;

static void
spc_start_button_clicked_lcb(GtkButton * button, gpointer user_data)
{
	gtk_widget_set_sensitive(spc_gui.start_button, 0);
	gtk_widget_set_sensitive(spc_gui.options_button, 0);

	gtk_widget_set_sensitive(spc_gui.accept_button, 1);
	gtk_widget_set_sensitive(spc_gui.replace_button, 1);
	gtk_widget_set_sensitive(spc_gui.ignore_button, 1);
	gtk_widget_set_sensitive(spc_gui.insert_button, 1);

	spc_is_running = TRUE;
	run_spell_checker();
}

static void
spc_close_button_clicked_lcb(GtkButton * button, gpointer user_data)
{
	DEBUG_MSG("Close button clicked\n");
	spc_message = SPC_CLOSE;
	if (!spc_is_running)
		gtk_widget_destroy(spc_gui.window);
}
static
void on_spc_options_button_clicked(GtkButton * button, gpointer user_data)
{
	//configure_cb(NULL,CONFIGURE_EXTERNAL_PAGE);
}
static
void on_spc_accept_button_clicked(GtkButton * button, gpointer user_data)
{
	spc_message = SPC_ACCEPT;	/* accept word */
}

static
void on_spc_insert_button_clicked(GtkButton * button, gpointer user_data)
{
	spc_message = SPC_INSERT;
}

static
void on_spc_ignore_button_clicked(GtkButton * button, gpointer user_data)
{
	spc_message = SPC_IGNORE;	/* ignore word */
}

static
void on_spc_replace_button_clicked(GtkButton * button, gpointer user_data)
{
	spc_message = SPC_REPLACE;	/* replace word */
}
static
gboolean delete_event_lcb(GtkButton * button, gpointer user_data)
{
	DEBUG_MSG("Delete event\n");
	spc_message = SPC_CLOSE;
	return (spc_is_running);	/* do not call the destroy event */
}

static void
on_near_misses_select_row_lcb(GtkWidget * clist,
			      gint row,
			      gint column,
			      GdkEventButton * event, gpointer data)
{
	gchar *text;
	gtk_clist_get_text(GTK_CLIST(clist), row, column, &text);
	gtk_entry_set_text(GTK_ENTRY(spc_gui.replace_entry), text);
}

Tspc_gui spc_gui;

GtkWidget *create_spc_window(void)
{
	spc_is_running = FALSE;
	spc_gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(spc_gui.window), "spc_window",
			    spc_gui.window);
	gtk_window_set_title(GTK_WINDOW(spc_gui.window),
			     ("Spell Checker"));

	spc_gui.vbox = gtk_vbox_new(FALSE, 0);
	/*  gtk_widget_ref (spc_gui.vbox);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_vbox", spc_gui.vbox,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.vbox);
	gtk_container_add(GTK_CONTAINER(spc_gui.window), spc_gui.vbox);

	spc_gui.hbuttonbox_top = gtk_hbutton_box_new();
	/* gtk_widget_ref (spc_gui.hbuttonbox_top);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "sp_hbuttonbox_top", spc_gui.hbuttonbox_top,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.hbuttonbox_top);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.hbuttonbox_top,
			   TRUE, TRUE, 0);

	spc_gui.start_button = gtk_button_new_with_label(("Start"));
	/* gtk_widget_ref (spc_gui . start_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_start_button", spc_gui.start_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.start_button);
	gtk_container_add(GTK_CONTAINER(spc_gui.hbuttonbox_top),
			  spc_gui.start_button);
	GTK_WIDGET_SET_FLAGS(spc_gui.start_button, GTK_CAN_DEFAULT);

	spc_gui.close_button = gtk_button_new_with_label(("Close"));
	/* gtk_widget_ref (spc_gui.close_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_close_button", spc_gui.close_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.close_button);
	gtk_container_add(GTK_CONTAINER(spc_gui.hbuttonbox_top),
			  spc_gui.close_button);
	GTK_WIDGET_SET_FLAGS(spc_gui.close_button, GTK_CAN_DEFAULT);

	spc_gui.options_button = gtk_button_new_with_label(("Options"));
	/* gtk_widget_ref (spc_gui.options_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui. window), "spc_options_button", spc_gui.options_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.options_button);
	gtk_container_add(GTK_CONTAINER(spc_gui.hbuttonbox_top),
			  spc_gui.options_button);
	GTK_WIDGET_SET_FLAGS(spc_gui.options_button, GTK_CAN_DEFAULT);

	spc_gui.word_frame = gtk_frame_new(("Word"));
	/* gtk_widget_ref (spc_gui.word_frame);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "sp_word_frame", spc_gui.word_frame,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.word_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.word_frame, TRUE,
			   TRUE, 0);

	spc_gui.word_entry = gtk_entry_new();
	/* gtk_widget_ref (spc_gui.word_entry);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_word_entry", spc_gui.word_entry,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_entry_set_editable(GTK_ENTRY(spc_gui.word_entry), 0);
	gtk_widget_show(spc_gui.word_entry);
	gtk_container_add(GTK_CONTAINER(spc_gui.word_frame),
			  spc_gui.word_entry);

	spc_gui.replace_frame = gtk_frame_new(("Replace"));
	/* gtk_widget_ref (spc_gui.replace_frame);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_replace_frame", spc_gui.replace_frame,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.replace_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.replace_frame,
			   FALSE, FALSE, 0);

	spc_gui.replace_entry = gtk_entry_new();
	/* gtk_widget_ref (spc_gui.replace_entry);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "replace_combo", spc_gui.replace_entry,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.replace_entry);
	gtk_container_add(GTK_CONTAINER(spc_gui.replace_frame),
			  spc_gui.replace_entry);

	spc_gui.near_misses_scrolled_window =
	    gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (spc_gui.
					near_misses_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(spc_gui.near_misses_scrolled_window, 20, 100);
	/* gtk_widget_ref (spc_gui.near_misses_scrolled_window);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.near_misses_scrolled_window),
	   "spc_near_misses_scrolled_window", spc_gui.near_misses_scrolled_window,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.near_misses_scrolled_window);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox),
			   spc_gui.near_misses_scrolled_window, TRUE, TRUE,
			   0);

	spc_gui.near_misses_clist = gtk_clist_new(1);
	gtk_clist_set_selection_mode(GTK_CLIST(spc_gui.near_misses_clist),
				     GTK_SELECTION_SINGLE);
	/* gtk_widget_ref (spc_gui.near_misses_clist);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.near_misses_clist), "near_misses_clist", spc_gui.near_misses_clist,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.near_misses_clist);
	gtk_container_add(GTK_CONTAINER
			  (spc_gui.near_misses_scrolled_window),
			  spc_gui.near_misses_clist);

	spc_gui.progress_frame = gtk_frame_new(("Progress"));
	/* gtk_widget_ref (spc_gui.progress_frame);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_progress_frame", spc_gui.progress_frame,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.progress_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.progress_frame,
			   TRUE, TRUE, 0);

	spc_gui.progress_bar = gtk_progress_bar_new();
	/* gtk_widget_ref (spc_gui.progress_bar);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_progress_bar", spc_gui.progress_bar,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.progress_bar);
	gtk_container_add(GTK_CONTAINER(spc_gui.progress_frame),
			  spc_gui.progress_bar);

	spc_gui.accept_button =
	    gtk_button_new_with_label(("Accept word\nin this section"));
	/* gtk_widget_ref (spc_gui.accept_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_accept_button", spc_gui.accept_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	GTK_WIDGET_SET_FLAGS(spc_gui.accept_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.accept_button, 0);

	spc_gui.insert_button =
	    gtk_button_new_with_label(("Insert in Personal\nDictionary"));
	/* gtk_widget_ref (spc_gui.insert_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_insert_button", spc_gui.insert_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	GTK_WIDGET_SET_FLAGS(spc_gui.insert_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.insert_button, 0);

	spc_gui.ignore_button =
	    gtk_button_new_with_label(("Ignore\nWord"));
	/* gtk_widget_ref (spc_gui.ignore_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_ignore_button", spc_gui.ignore_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	GTK_WIDGET_SET_FLAGS(spc_gui.ignore_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.ignore_button, 0);

	spc_gui.replace_button =
	    gtk_button_new_with_label(("Replace\nWord"));
	/* gtk_widget_ref (spc_gui.replace_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_replace_button", spc_gui.replace_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	GTK_WIDGET_SET_FLAGS(spc_gui.replace_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.replace_button, 0);

	spc_gui.button_table = gtk_table_new(2, 2, TRUE);
	gtk_widget_show(spc_gui.button_table);
	gtk_table_attach_defaults(GTK_TABLE(spc_gui.button_table),
				  spc_gui.accept_button, 0, 1, 0, 1);
	gtk_widget_show(spc_gui.accept_button);
	gtk_table_attach_defaults(GTK_TABLE(spc_gui.button_table),
				  spc_gui.replace_button, 1, 2, 0, 1);
	gtk_widget_show(spc_gui.replace_button);
	gtk_table_attach_defaults(GTK_TABLE(spc_gui.button_table),
				  spc_gui.insert_button, 0, 1, 1, 2);
	gtk_widget_show(spc_gui.insert_button);
	gtk_table_attach_defaults(GTK_TABLE(spc_gui.button_table),
				  spc_gui.ignore_button, 1, 2, 1, 2);
	gtk_widget_show(spc_gui.ignore_button);
	gtk_widget_show(spc_gui.button_table);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.button_table,
			   TRUE, TRUE, 0);

	gtk_signal_connect(GTK_OBJECT(spc_gui.window), "delete_event",
			   GTK_SIGNAL_FUNC(delete_event_lcb), NULL);

	gtk_signal_connect(GTK_OBJECT(spc_gui.start_button), "clicked",
			   GTK_SIGNAL_FUNC(spc_start_button_clicked_lcb),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spc_gui.close_button), "clicked",
			   GTK_SIGNAL_FUNC(spc_close_button_clicked_lcb),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spc_gui.options_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_options_button_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spc_gui.accept_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_accept_button_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spc_gui.insert_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_insert_button_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spc_gui.ignore_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_ignore_button_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spc_gui.replace_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_replace_button_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(spc_gui.near_misses_clist),
			   "select_row",
			   GTK_SIGNAL_FUNC(on_near_misses_select_row_lcb),
			   NULL);
	return spc_gui.window;
}

void spell_check_cb(GtkWidget * w, gpointer data)
{
	/* checks if the text is empty */
	text_widget = lookup_widget(gtk_widget_get_toplevel(w), (gchar *)data);
	g_warning((gchar *)data);
	if (gtk_text_get_length(GTK_TEXT(text_widget)) != 0) {
		spc_gui.window = create_spc_window();
		//spc_gui.status_bar_count = bf_statusbar_message(("checking spelling"));
		gtk_widget_show(spc_gui.window);
	}
}

#endif

