/*
 * Xiphos Bible Study Tool
 * link_dialog.c - dialog for inserting a link 
 * 
 * Copyright (C) 2005-2009 Xiphos Developer Team
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

 
#include <config.h>
 
#ifdef USE_GTKHTML3_14_23

#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <editor/gtkhtml-editor.h>
#include <gtkhtml/gtkhtml-stream.h>

 

#include "editor/slib-editor.h"
#include "editor/link_dialog.h"

#include "main/url.hh"
#include "main/sword.h"

#include "gui/utilities.h"

/************* begin link dialog ****************/
static GtkWidget *window;
static GtkWidget *entry_module;
static GtkWidget *entry_verse;
static GtkWidget *entry_text;


G_MODULE_EXPORT
void entry_verse_changed_cb(GtkObject *object, EDITOR *e)
{

	const gchar *verse_str = NULL;

	verse_str = gtk_entry_get_text (GTK_ENTRY (object));
	gtk_entry_set_text(GTK_ENTRY (entry_text),verse_str);

}

G_MODULE_EXPORT
void button_ok_clicked_cb(GtkObject *object, EDITOR *e)
{
	const gchar *mod_str = NULL;
	const gchar *verse_str = NULL;
	const gchar *text_str = NULL;
	const gchar *encoded_mod = NULL;
	const gchar *encoded_verse = NULL;
	GString *str = g_string_new (NULL);
	gint type = 0;

	mod_str = gtk_entry_get_text (GTK_ENTRY (entry_module));
	verse_str = gtk_entry_get_text (GTK_ENTRY (entry_verse));
	text_str = gtk_entry_get_text (GTK_ENTRY (entry_text));

	type = main_get_mod_type((gchar*)mod_str);
	
	if (mod_str)
		encoded_mod = main_url_encode(mod_str);
	encoded_verse = main_url_encode(verse_str);

	switch (type) {
		case -1:
		case TEXT_TYPE:
			g_string_printf(str,"<a href=\"xiphos.url?action=showRef&type=scripRef&value=%s&module=%s\">%s</a>", encoded_verse, encoded_mod, text_str);
			break;
		case COMMENTARY_TYPE:
		case DICTIONARY_TYPE:
		case BOOK_TYPE:
		case PRAYERLIST_TYPE:
			g_string_printf(str,"<a href=\"sword://%s/%s\">%s</a>", encoded_mod, encoded_verse, text_str);
			break;
	}
	
	GS_message (("link: %s", str->str));
	gtkhtml_editor_insert_html (GTKHTML_EDITOR (e->window), str->str);
	g_string_free (str, TRUE);
	g_free((gchar*)encoded_mod);
	g_free((gchar*)encoded_verse);

	gtk_widget_destroy(GTK_WIDGET(window));
}

G_MODULE_EXPORT
void button_test_clicked_cb(GtkObject *object, gpointer user_data)
{
 	const gchar *mod_str = NULL;
	const gchar *verse_str = NULL;
	const gchar *encoded_mod = NULL;
	const gchar *encoded_verse = NULL;
	GString *str = g_string_new (NULL);

	mod_str = gtk_entry_get_text (GTK_ENTRY (entry_module));
	verse_str = gtk_entry_get_text (GTK_ENTRY (entry_verse));
	
	encoded_mod = main_url_encode(mod_str);
	encoded_verse = main_url_encode(verse_str);
	 
	g_string_printf(str,"passagestudy.jsp?action=showRef&type=scripRef&value=%s&module=%s", encoded_verse, encoded_mod);
	GS_message (("link: %s", str->str));
	main_url_handler(str->str, TRUE);
	g_string_free (str, TRUE);
	g_free((gchar*)encoded_mod);
	g_free((gchar*)encoded_verse);       
}

G_MODULE_EXPORT
void button_cancel_clicked_cb(GtkObject *object, gpointer user_data)
{
        gtk_widget_destroy(GTK_WIDGET(window));
}


void
editor_link_dialog (EDITOR *e)
{
	
        GtkBuilder *builder;
	gchar *gbuilder_file;
	
	GtkHTML  *html = gtkhtml_editor_get_html (GTKHTML_EDITOR(e->window));
	if (html->pointer_url)  /* are we in a link */
		return;		/* if so don't do anything */
        
        builder = gtk_builder_new ();
	gbuilder_file = gui_general_user_file ("editor_link_dialog.xml", FALSE);
        gtk_builder_add_from_file (builder, gbuilder_file, NULL);
 
        window = GTK_WIDGET (gtk_builder_get_object (builder, "dialog1"));
	set_window_icon (GTK_WINDOW(window));
        gtk_builder_connect_signals (builder,(EDITOR*) e); 

	entry_module = GTK_WIDGET (gtk_builder_get_object (builder, "entry_module"));
	entry_verse = GTK_WIDGET (gtk_builder_get_object (builder, "entry_verse"));
	entry_text = GTK_WIDGET (gtk_builder_get_object (builder, "entry_text"));
	
        g_object_unref (G_OBJECT (builder));        
        gtk_widget_show (window); 
}
/************* end link dialog ****************/

#endif /* USE_GTKHTML3_14_23 */
