/*
 * Xiphos Bible Study Tool
 * shortcutbar_search.c - create search group gui
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <regex.h>

#include "gui/sidebar.h"
#include "gui/search_sidebar.h"
#include "gui/xiphos.h"
#include "gui/widgets.h"
#include "gui/utilities.h"

#include "main/search_dialog.h"
#include "main/search_sidebar.h"
#include "main/sword.h"
#include "main/settings.h"

#include "gui/debug_glib_null.h"

SIDESEARCH ss;

GtkWidget *remember_search; /* needed to change button in search stop */

/******************************************************************************
 * Name
 *    on_search_button_clicked
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *   void on_search_button_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   prepare to begin search
 *
 * Return value
 *   void
 */

static void on_search_button_clicked(GtkButton *button,
				     gpointer user_data)
{
	if (search_active) {
		terminate_search = TRUE;
#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_button_set_image((GtkButton *)remember_search,
				     gtk_image_new_from_icon_name("edit-find-symbolic", GTK_ICON_SIZE_BUTTON));
#else
		gtk_button_set_label((GtkButton *)remember_search,
				     "gtk-find");
		gtk_button_set_use_stock((GtkButton *)remember_search,
					 TRUE);

#endif
		sync_windows();
	} else {
#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_button_set_image((GtkButton *)remember_search,
				     gtk_image_new_from_icon_name("process-stop-symbolic",
								  GTK_ICON_SIZE_BUTTON));
#else
		gtk_button_set_label((GtkButton *)remember_search,
				     "gtk-stop");
		gtk_button_set_use_stock((GtkButton *)remember_search,
					 TRUE);
#endif

		// do the search
		main_do_sidebar_search(user_data);

#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_button_set_image((GtkButton *)remember_search,
				     gtk_image_new_from_icon_name("edit-find-symbolic", GTK_ICON_SIZE_BUTTON));
#else
		gtk_button_set_label((GtkButton *)remember_search,
				     "gtk-find");
		gtk_button_set_use_stock((GtkButton *)remember_search,
					 TRUE);
#endif
	}
}

/******************************************************************************
 * Name
 *   on_rrbUseBounds_toggled
 *
 * Synopsis
 *   #include "gui/shortcutbar_search.h"
 *
 *   void on_rrbUseBounds_toggled(GtkToggleButton * togglebutton,
 *						gpointer user_data)
 *
 * Description
 *   a toggle button has changed
 *   set ok - apply button sensitive
 *
 * Return value
 *   void
 */

static void on_rrbUseBounds_toggled(GtkToggleButton *togglebutton,
				    gpointer user_data)
{
	main_init_sidebar_search_backend();
	if (gtk_toggle_button_get_active(togglebutton)) {
		gtk_widget_show(ss.frame5);
	} else {
		gtk_widget_hide(ss.frame5);
	}
}

/******************************************************************************
 * Name
 *   sidebar_optimized_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void sidebar_optimized_toggled(GtkToggleButton *togglebutton,
 *			    gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void sidebar_optimized_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		gtk_widget_set_sensitive(ss.ckbCaseSensitive, FALSE);
	} else {
		gtk_widget_set_sensitive(ss.ckbCaseSensitive, TRUE);
	}
}

/******************************************************************************
 * Name
 *   gui_create_shortcutbar_search
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *   void gui_create_shortcutbar_search(GtkWidget * vp)
 *
 * Description
 *   create search group of shortcut bar
 *
 * Return value
 *   void
 */

void gui_create_search_sidebar(void)
{
	GtkWidget *vbox1;
	GtkWidget *vbox90;
	GtkWidget *vbox5;
	GtkWidget *frame2;
	GtkWidget *vbox2;
	GtkWidget *frame3;
	GtkWidget *vbox3;
	GtkWidget *frame4;
	GtkWidget *vbox4;
	GtkWidget *table1;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *scrolledwindow_search;
	GtkWidget *viewport_search;
	gchar *header;
	//GtkListStore *store;

	//ss = &sss;

	scrolledwindow_search = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_search);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  scrolledwindow_search);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow_search),
					    settings.shadow_type);
	gtk_container_set_border_width(GTK_CONTAINER(scrolledwindow_search), 2);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow_search),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	viewport_search = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport_search);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_search),
			  viewport_search);

	UI_VBOX(vbox1, FALSE, 4);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(viewport_search), vbox1);

	UI_VBOX(vbox5, FALSE, 0);
	gtk_widget_show(vbox5);
	gtk_box_pack_start(GTK_BOX(vbox1), vbox5, FALSE, TRUE, 0);

	/* text entry */
	ss.entrySearch = gtk_entry_new();
	gtk_widget_show(ss.entrySearch);
	gtk_box_pack_start(GTK_BOX(vbox5), ss.entrySearch, TRUE, TRUE, 0);
	gtk_widget_set_size_request(ss.entrySearch, 130, -1);
	;

/* find button */
#if GTK_CHECK_VERSION(3, 10, 0)
	remember_search =
	    gtk_button_new_from_icon_name("edit-find-symbolic",
					  GTK_ICON_SIZE_BUTTON);
#else
	remember_search = gtk_button_new_from_stock(GTK_STOCK_FIND);
#endif
	gtk_widget_show(remember_search);
	gtk_box_pack_start(GTK_BOX(vbox5), remember_search, TRUE, FALSE,
			   0);
	gtk_widget_set_tooltip_text(remember_search,
				    _("This is an inclusive (\"AND\") search:\nFind matches showing all words."));
	gtk_button_set_relief(GTK_BUTTON(remember_search),
			      GTK_RELIEF_HALF);

	/* progress bar */
	ss.progressbar_search = gtk_progress_bar_new();
	gtk_widget_show(ss.progressbar_search);
	gtk_box_pack_start(GTK_BOX(vbox5), ss.progressbar_search, FALSE,
			   TRUE, 0);

	/* button to open advanced search */
	ss.advanced_search =
	    gtk_button_new_with_mnemonic(_("_Open Advanced Search"));
	gtk_widget_show(ss.advanced_search);
	gtk_box_pack_start(GTK_BOX(vbox5), ss.advanced_search, TRUE, FALSE,
			   0);
	gtk_widget_set_tooltip_text(ss.advanced_search,
				    _("Open the separate Advanced Search dialog."));
	gtk_button_set_relief(GTK_BUTTON(ss.advanced_search),
			      GTK_RELIEF_HALF);

	ss.frame_module = gtk_frame_new(NULL);
	gtk_widget_show(ss.frame_module);
	gtk_box_pack_start(GTK_BOX(vbox1), ss.frame_module, FALSE, FALSE,
			   0);
	gtk_container_set_border_width(GTK_CONTAINER(ss.frame_module), 2);
	gtk_frame_set_shadow_type(GTK_FRAME(ss.frame_module),
				  GTK_SHADOW_NONE);

	label1 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Search Module"));
	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);

	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(ss.frame_module), label1);

	UI_VBOX(vbox90, FALSE, 0);
	gtk_widget_show(vbox90);
	gtk_container_add(GTK_CONTAINER(ss.frame_module), vbox90);
	gtk_container_set_border_width(GTK_CONTAINER(vbox90), 4);

	ss.radiobutton_search_text =
	    gtk_radio_button_new_with_label(NULL, _("Bible"));
	gtk_widget_show(ss.radiobutton_search_text);
	gtk_widget_set_size_request(ss.radiobutton_search_text, -1, 20);
	gtk_box_pack_start(GTK_BOX(vbox90), ss.radiobutton_search_text,
			   FALSE, FALSE, 0);

	ss.radiobutton_search_comm =
	    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(ss.radiobutton_search_text),
							_("Commentary"));
	gtk_widget_show(ss.radiobutton_search_comm);
	gtk_widget_set_size_request(ss.radiobutton_search_comm, -1, 20);
	gtk_box_pack_start(GTK_BOX(vbox90), ss.radiobutton_search_comm,
			   FALSE, FALSE, 0);

	frame2 = gtk_frame_new(NULL);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox1), frame2, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame2), 2);
	gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_NONE);

	label1 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Search Type"));
	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);

	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(frame2), label1);

	UI_VBOX(vbox2, TRUE, 0);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(frame2), vbox2);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 4);

	ss.rbMultiword =
	    gtk_radio_button_new_with_label(NULL, _("Optimized (\"Lucene\")"));
	gtk_widget_show(ss.rbMultiword);
	gtk_box_pack_start(GTK_BOX(vbox2), ss.rbMultiword, FALSE,
			   FALSE, 0);
	gtk_widget_set_size_request(ss.rbMultiword, -1, 20);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ss.rbMultiword), TRUE);

	ss.rbRegExp =
	    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(ss.rbMultiword),
							_("Regular expression"));
	gtk_widget_show(ss.rbRegExp);
	gtk_box_pack_start(GTK_BOX(vbox2), ss.rbRegExp, FALSE, FALSE, 0);
	gtk_widget_set_size_request(ss.rbRegExp, -1, 20);

	ss.rbPhraseSearch =
	    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(ss.rbMultiword),
							_("Exact phrase"));
	gtk_widget_show(ss.rbPhraseSearch);
	gtk_box_pack_start(GTK_BOX(vbox2), ss.rbPhraseSearch,
			   FALSE, FALSE, 0);
	gtk_widget_set_size_request(ss.rbPhraseSearch, -1, 20);

	frame3 = gtk_frame_new(NULL);
	gtk_widget_show(frame3);
	gtk_box_pack_start(GTK_BOX(vbox1), frame3, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame3), 2);
	gtk_frame_set_shadow_type(GTK_FRAME(frame3), GTK_SHADOW_NONE);

	label1 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Search Options"));
	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);

	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(frame3), label1);

	UI_VBOX(vbox3, FALSE, 0);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(frame3), vbox3);
	gtk_container_set_border_width(GTK_CONTAINER(vbox3), 4);

	ss.ckbCaseSensitive =
	    gtk_check_button_new_with_label(_("Match case"));
	gtk_widget_show(ss.ckbCaseSensitive);
	gtk_box_pack_start(GTK_BOX(vbox3), ss.ckbCaseSensitive, FALSE,
			   FALSE, 0);
	gtk_widget_set_size_request(ss.ckbCaseSensitive, -1, 20);

	frame4 = gtk_frame_new(NULL);
	gtk_widget_show(frame4);
	gtk_box_pack_start(GTK_BOX(vbox1), frame4, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame4), 2);
	gtk_frame_set_shadow_type(GTK_FRAME(frame4), GTK_SHADOW_NONE);

	label1 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Search Scope"));
	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);

	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(frame4), label1);

	UI_VBOX(vbox4, TRUE, 0);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(frame4), vbox4);
	gtk_container_set_border_width(GTK_CONTAINER(vbox4), 4);

	ss.rbNoScope =
	    gtk_radio_button_new_with_label(NULL, _("No scope"));
	gtk_widget_show(ss.rbNoScope);
	gtk_box_pack_start(GTK_BOX(vbox4), ss.rbNoScope, FALSE, FALSE, 0);
	gtk_widget_set_size_request(ss.rbNoScope, -1, 20);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ss.rbNoScope),
				     TRUE);

	ss.rrbUseBounds =
	    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(ss.rbNoScope),
							_("Use bounds"));
	gtk_widget_show(ss.rrbUseBounds);
	gtk_box_pack_start(GTK_BOX(vbox4), ss.rrbUseBounds, FALSE,
			   FALSE, 0);
	gtk_widget_set_size_request(ss.rrbUseBounds, -1, 20);

	ss.rbLastSearch =
	    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(ss.rbNoScope),
							_("Last search"));
	gtk_widget_show(ss.rbLastSearch);
	gtk_box_pack_start(GTK_BOX(vbox4), ss.rbLastSearch, FALSE,
			   FALSE, 0);
	gtk_widget_set_size_request(ss.rbLastSearch, -1, 20);

	ss.frame5 = gtk_frame_new(NULL);
	gtk_widget_show(ss.frame5);
	gtk_box_pack_start(GTK_BOX(vbox1), ss.frame5, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(ss.frame5), 2);
	gtk_widget_hide(ss.frame5);
	gtk_frame_set_shadow_type(GTK_FRAME(ss.frame5), GTK_SHADOW_NONE);

	label1 = gtk_label_new(NULL);

	header =
	    g_strdup_printf("<span weight=\"bold\">%s</span>",
			    _("Bounds"));
	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);

	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(ss.frame5), label1);

#if GTK_CHECK_VERSION(3, 4, 0)
	table1 = gtk_grid_new();
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(ss.frame5), table1);
	gtk_grid_set_row_spacing(GTK_GRID(table1), 3);
	gtk_grid_set_column_spacing(GTK_GRID(table1), 3);
	gtk_container_set_border_width(GTK_CONTAINER(table1), 8);

	label1 = gtk_label_new(_("Lower"));
	gtk_widget_show(label1);
	gtk_grid_attach(GTK_GRID(table1), label1, 0, 0, 1, 1);
#if !GTK_CHECK_VERSION(3, 10, 0)
	gtk_misc_set_alignment(GTK_MISC(label1), 1.0, 0.5);
#endif

	ss.entryLower = gtk_combo_box_text_new_with_entry();
	gtk_widget_show(ss.entryLower);
	gtk_grid_attach(GTK_GRID(table1), ss.entryLower, 1, 0, 1, 1);
	gtk_widget_set_size_request(ss.entryLower, 114, 22);

	label2 = gtk_label_new(_("Upper"));
	gtk_widget_show(label2);
	gtk_grid_attach(GTK_GRID(table1), label2, 0, 1, 1, 1);
#if !GTK_CHECK_VERSION(3, 10, 0)
	gtk_misc_set_alignment(GTK_MISC(label2), 1.0, 0.5);
#endif

	ss.entryUpper = gtk_combo_box_text_new_with_entry();
	gtk_widget_show(ss.entryUpper);
	gtk_grid_attach(GTK_GRID(table1), ss.entryUpper, 1, 1, 1, 1);
	gtk_widget_set_size_request(ss.entryUpper, 114, 22);

#else
	table1 = gtk_table_new(2, 2, FALSE);
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(ss.frame5), table1);
	gtk_table_set_row_spacings(GTK_TABLE(table1), 3);
	gtk_table_set_col_spacings(GTK_TABLE(table1), 3);
	gtk_container_set_border_width(GTK_CONTAINER(table1), 8);

	label1 = gtk_label_new(_("Lower"));
	gtk_widget_show(label1);
	gtk_table_attach(GTK_TABLE(table1), label1, 0, 1, 0, 1,
			 (GtkAttachOptions)(GTK_FILL),
			 (GtkAttachOptions)(0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label1), 1.0, 0.5);

	label2 = gtk_label_new(_("Upper"));
	gtk_widget_show(label2);
	gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 1, 2,
			 (GtkAttachOptions)(GTK_FILL),
			 (GtkAttachOptions)(0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label2), 1.0, 0.5);

#if GTK_CHECK_VERSION(2, 24, 0)
	ss.entryLower = gtk_combo_box_text_new_with_entry();
#else
	ss.entryLower = gtk_combo_box_entry_new_text();
#endif
	gtk_widget_show(ss.entryLower);
	gtk_table_attach(GTK_TABLE(table1), ss.entryLower, 1, 2, 0, 1,
			 (GtkAttachOptions)(GTK_FILL),
			 (GtkAttachOptions)(0), 0, 0);
	gtk_widget_set_size_request(ss.entryLower, 114, 22);

#if GTK_CHECK_VERSION(2, 24, 0)
	ss.entryUpper = gtk_combo_box_text_new_with_entry();
#else
	ss.entryUpper = gtk_combo_box_entry_new_text();
#endif
	gtk_widget_show(ss.entryUpper);
	gtk_table_attach(GTK_TABLE(table1), ss.entryUpper, 1, 2, 1, 2,
			 (GtkAttachOptions)(GTK_FILL),
			 (GtkAttachOptions)(0), 0, 0);
	gtk_widget_set_size_request(ss.entryUpper, 114, 22);
#endif

	g_signal_connect(G_OBJECT(ss.rrbUseBounds),
			 "toggled",
			 G_CALLBACK(on_rrbUseBounds_toggled), NULL);
	g_signal_connect(G_OBJECT(remember_search), "clicked",
			 G_CALLBACK(on_search_button_clicked), NULL);

	g_signal_connect(G_OBJECT(ss.entrySearch), "activate",
			 G_CALLBACK(on_search_button_clicked), NULL);

	g_signal_connect(G_OBJECT(ss.advanced_search), "clicked",
			 G_CALLBACK(main_open_search_dialog), NULL);

	/* prep for toggle case sensitive availability per optimized */
	g_signal_connect(ss.rbMultiword, "toggled",
			 G_CALLBACK(sidebar_optimized_toggled), NULL);
	/* initialize it off */
	gtk_widget_set_sensitive(ss.ckbCaseSensitive, FALSE);
}
