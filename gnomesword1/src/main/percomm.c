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
static GList *percomm_list;
static GString *gstr;

/******************************************************************************
 * globals
 */ 
PC_DATA *cur_p;
gboolean percomm_display_change = TRUE;

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

void editor_save_note(GtkWidget * html_widget)
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
		save_percomm_note(gstr->str);
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

void save_percomm_note(gchar *note)
{
	backend_save_personal_comment(cur_p->mod_num, note);
	cur_p->ec->changed = FALSE;
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

void delete_percomm_note(void)
{	
	GtkWidget *label1, *label2, *label3, *msgbox;
	gint answer = -1;
	const gchar *key;
	
	key = backend_get_percomm_key(cur_p->mod_num);
	
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
		backend_delete_personal_comment(cur_p->mod_num);
		cur_p->ec->changed = FALSE;
		break;
	default:
		break;
	}
}


/******************************************************************************
 * Name
 *  set_percomm_page_and_key
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *  void set_percomm_page_and_key(gint page_num, gchar * key)	
 *
 * Description
 *   change percomm module notebook page and display new key (reference)
 *
 * Return value
 *   void
 */

void set_percomm_page_and_key(gint page_num, gchar * key)
{
	/*
	 * we don't want backend_dispaly_percomm to be
	 * called by on_notebook_percomm_switch_page
	 */
	percomm_display_change = FALSE;
	if (settings.text_last_page != page_num) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (settings.notebook_percomm),
				      page_num);
	}
	backend_display_percomm(page_num, key);
	percomm_display_change = TRUE;
}

/******************************************************************************
 * Name
 *  set_page_percomm
 *
 * Synopsis
 *   #include "percomm.h"
 *   
 *   static void set_page_percomm(gchar * modname, GList * percomm_list)
 *
 * Description
 *   change percomm module by finding page number from module name
 *
 * Return value
 *   void
 */

static void set_page_percomm(gchar * modname, GList * percomm_list)
{
	gint page = 0;
	PC_DATA *p = NULL;

	percomm_list = g_list_first(percomm_list);
	while (percomm_list != NULL) {
		p = (PC_DATA *) percomm_list->data;
		if (!strcmp(p->mod_name, modname))
			break;
		++page;
		percomm_list = g_list_next(percomm_list);
	}
	cur_p = p;
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_percomm), page);
	settings.percomm_last_page = page;
}

/******************************************************************************
 * Name
 *  display_percomm
 *
 * Synopsis
 *   #include "percomm.h"
 *   
 *   void display_percomm(gchar * key)	
 *
 * Description
 *   call backend_display_percomm and pass module page num and key
 *
 * Return value
 *   void
 */

void display_percomm(gchar *key)
{
	backend_display_percomm(settings.percomm_last_page, key);
}

/******************************************************************************
 * Name
 *  setup_percomm
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   GList *setup_percomm(GList *mods)
 *
 * Description
 *   set up gui for sword personal comments modules - 
 *   return list of percomm module names
 *
 * Return value
 *   GList *
 */

void setup_percomm(GList *mods)
{
	GList *tmp = NULL;
	gchar *mod_name;
	gchar *modbuf;
	gchar *keybuf;
	PC_DATA *p;
	gint count = 0;

	percomm_list = NULL;
	
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		mod_name = (gchar *) tmp->data;
		p = g_new(PC_DATA, 1);
		p->mod_name = mod_name;
		p->mod_description =
		    backend_get_module_description(mod_name);
		p->mod_num = count;
		p->search_string = NULL;
		p->key = NULL;
		p->ec = gui_percomm_control(p->mod_name,count);
		p->html = p->ec->htmlwidget;
		backend_new_percomm_display(p->ec->htmlwidget,
					    p->mod_name);
		percomm_list =
		    g_list_append(percomm_list, (PC_DATA *) p);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(settings.notebook_percomm),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_percomm_switch_page),
			   percomm_list);

	modbuf = g_strdup(settings.personalcommentsmod);
	keybuf = g_strdup(settings.currentverse);

	set_page_percomm(modbuf, percomm_list);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *  shutdown_percomm
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *  void shutdown_percomm(void)	
 *
 * Description
 *   shut down text module support clean mem
 *
 * Return value
 *   void
 */

void shutdown_percomm(void)
{
	percomm_list = g_list_first(percomm_list);
	while (percomm_list != NULL) {
		PC_DATA *p = (PC_DATA *) percomm_list->data;
		/* 
		 * free any search dialogs created 
		 */		
		if (p->ec->search_dialog)
			g_free(p->ec->search_dialog);		
		if (p->ec->replace_dialog)
			g_free(p->ec->replace_dialog);		
		/* 
		 * free editor controls 
		 */
		if (p->ec)
			g_free(p->ec);
		/* 
		 * free each TEXT_DATA item created 
		 */
		g_free((PC_DATA *) percomm_list->data);
		percomm_list = g_list_next(percomm_list);
	}
	g_list_free(percomm_list);
}

