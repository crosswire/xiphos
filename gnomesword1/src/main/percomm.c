/*
 * GnomeSword Bible Study Tool
 * percomm.c - support for personal commentary modules
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
#  include <config.h>
#endif

#include <gnome.h>

#include "gui/_percomm.h"
#include "gui/_editor.h"
#include "backend/module_options.h"
#include "gui/info_box.h"

#include "main/percomm.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"
#include "main/support.h"

#include "backend/percomm_.h"
#include "backend/sword.h"

/******************************************************************************
 * globals to this file only 
 */ 

static GString *gstr;

/******************************************************************************
 * Name
 *   save_note_receiver
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   	gboolean save_note_receiver(const HTMLEngine * engine,
 *		   const char *data, unsigned int len, void *user_data)
 *
 * Description
 *    
 *
 * Return value
 *   gboolean
 */ 

static gboolean save_note_receiver(const HTMLEngine * engine,
		   const char *data, unsigned int len, void *user_data)
{
	static gboolean startgrabing = FALSE;
	if (!strncmp(data, "</BODY>", 7))
		startgrabing = FALSE;
	if (startgrabing) {
		gstr = g_string_append(gstr, data);
		//g_warning(gstr->str);
	}
	if (strstr(data, "<BODY") != NULL)
		startgrabing = TRUE;

	return TRUE;
}


/******************************************************************************
 * Name
 *   editor_save_note
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void editor_save_note(GtkWidget * html_widget)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void editor_save_note(GtkWidget * html_widget, char * mod_name)
{
	GtkHTML *html;

	html = GTK_HTML(html_widget);
	gtk_html_set_editable(html, FALSE);
	gstr = g_string_new("");
	if (!gtk_html_save
	    (html, (GtkHTMLSaveReceiverFn) save_note_receiver,
	     GINT_TO_POINTER(0))) {
		g_warning("file not writen");
	} else {
		save_percomm_note(gstr->str, mod_name);
		g_print("\nfile writen\n");
	}
	g_string_free(gstr, 1);
	gtk_html_set_editable(html, TRUE);
}

/******************************************************************************
 * Name
 *  save_percomm_note
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void save_percomm_note(gchar *note)	
 *
 * Description
 *    save note to current personal commentary (cur_p)
 *    by calling backend_save_personal_comment()
 *
 * Return value
 *   void
 */ 

void save_percomm_note(gchar *note, char * mod_name)
{
	backend_save_personal_comment(mod_name, note);
}

/******************************************************************************
 * Name
 *  delete_percomm_note
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void delete_percomm_note(void)	
 *
 * Description
 *    opens info box to make sure user want to delete note if so
 *    calls backend_delete_personal_comment() to delete note
 *
 * Return value
 *   void
 */ 

void delete_percomm_note(char * mod_name)
{	
	GtkWidget *label1, *label2, *label3, *msgbox;
	gint answer = -1;
	gchar *key;
	
	key = backend_get_percomm_key(mod_name);
	
	msgbox = gui_create_info_box();
	label1 = lookup_widget(msgbox, "lbInfoBox1");
	label2 = lookup_widget(msgbox, "lbInfoBox2");
	label3 = lookup_widget(msgbox, "lbInfoBox3");
	gtk_label_set_text(GTK_LABEL(label1), _("Are you sure you want"));
	gtk_label_set_text(GTK_LABEL(label2), _("to delete the note for"));
	gtk_label_set_text(GTK_LABEL(label3),key);

	gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
	answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
	switch (answer) {
	case 0:
		backend_delete_personal_comment(mod_name);
		break;
	default:
		break;
	}
	free(key);
}

