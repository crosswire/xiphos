/*
 * GnomeSword Bible Study Tool
 * _percom.c - personal comments gui
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
#include <gtkhtml/gtkhtml.h>

#ifdef USE_SPELL
#include "spell.h"
#include "spell_gui.h"
#endif

/* frontend */
#include "_percomm.h"
#include "_editor.h"
#include "editor_toolbar.h"

/* main */
#include "percomm.h"
#include "settings.h"
#include "support.h"


/******************************************************************************
 * Name
 *   gui_percomm_control
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   GSHTMLEditorControlData *gui_percomm_control(SETTINGS * s, 
 *				gchar *mod_name, gint page_num)	
 *
 * Description
 *   create personal commentary editor control
 *
 * Return value
 *   GSHTMLEditorControlData *
 */

GSHTMLEditorControlData *gui_percomm_control(SETTINGS * s, 
				gchar *mod_name, gint page_num)
{
	GtkWidget * vbox;
	GtkWidget *frame12;
	GtkWidget *vboxPC;
	GtkWidget *label85;
	GtkWidget *html;
	
	GSHTMLEditorControlData *pcecd =
	    gs_html_editor_control_data_new(s);
	
	pcecd->personal_comments = TRUE;

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox",
				 vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(s->notebook_percomm), vbox);
	
	frame12 = gtk_frame_new(NULL);
	gtk_widget_ref(frame12);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "frame12",
				 frame12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame12);
	gtk_box_pack_start(GTK_BOX(vbox), frame12, TRUE, TRUE, 0);

	vboxPC = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxPC);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "vboxPC", vboxPC,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxPC);
	gtk_container_add(GTK_CONTAINER(frame12), vboxPC);

							
	html = gtk_html_new();

	gui_create_html_editor(html, vboxPC, s, pcecd);
	
	
	label85 = gtk_label_new(mod_name);
	gtk_widget_ref(label85);
	gtk_object_set_data_full(GTK_OBJECT(s->app), "label85",
				 label85,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label85);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->notebook_percomm),
				gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(s->notebook_percomm),
							page_num), label85);	
							     
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->notebook_percomm),
                                gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(s->notebook_percomm),
							page_num), mod_name);
	
	
	s->toolbarComments = toolbar_style(pcecd);
	gtk_widget_hide(pcecd->handlebox_toolbar);
	return pcecd;
}
