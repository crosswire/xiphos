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

#include "gs_gnomesword.h"
#include "gs_editor.h"
#include "gs_html.h"
#include "sword.h"
#include "module_options.h"
#include "gs_info_box.h"
#include "support.h"

/*
 * gnome
 */

/*
 * main
 */
#include "percomm.h"
/*
 * backend
 */
#include "percomm_.h"

/******************************************************************************
 * externs
 */

/******************************************************************************
 * globals to this file only 
 */
 
static GList *percomm_list;
static gboolean display_change = TRUE;

PC_DATA *cur_p;

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
	
	msgbox = create_InfoBox();
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
 *  on_notebook_percomm_switch_page
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void on_notebook_percomm_switch_page(GtkNotebook * notebook,
 *				  GtkNotebookPage * page,
 *				  gint page_num, GList * pcl)	
 *
 * Description
 *   sets gui to new sword module
 *
 * Return value
 *   void
 */
 
void on_notebook_percomm_switch_page(GtkNotebook * notebook,
				     GtkNotebookPage * page,
				     gint page_num, GList * pcl)
{
	PC_DATA *p, *p_last;
	/*
	 * get data structure for new module 
	 */
	p_last = (PC_DATA *) g_list_nth_data(pcl, 
				settings->percomm_last_page);
	p = (PC_DATA *) g_list_nth_data(pcl, page_num);
	/*
	 * do work that's non gui
	 */
	percomm_page_changed(page_num, p);
	/*
	 * keep showtabs menu item current 
	 */
	//GTK_CHECK_MENU_ITEM(p->showtabs)->active = settings->percomm_tabs;

	/*
	 * set edit mode
	 */
	if(GTK_CHECK_MENU_ITEM(p_last->ec->editnote)->active) {
		gtk_widget_hide(p_last->ec->frame_toolbar);
		gtk_widget_hide(p_last->ec->handlebox_toolbar);
	}
	if(GTK_CHECK_MENU_ITEM(p->ec->editnote)->active){
		gtk_widget_show(p->ec->frame_toolbar);
		gtk_widget_show(p->ec->handlebox_toolbar);
	}
		
}

/******************************************************************************
 * Name
 *  percomm_page_changed
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void percomm_page_changed(gint page_num, PC_DATA *p)	
 *
 * Description
 *    take care of non gui stuff on notebook page change
 *
 * Return value
 *   void
 */

void percomm_page_changed(gint page_num, PC_DATA * p)
{
	/*
	 * remember new module name
	 */
	strcpy(settings->personalcommentsmod, p->mod_name);
	/*
	 * point TEXT_DATA *cur_t to t - cur_t is global to this file
	 */
	cur_p = p;
	/*
	 * remember page number
	 */
	settings->percomm_last_page = page_num;
	/*
	 * display new module with current verse
	 */
	if (display_change) {
		set_percomm_page_and_key(page_num,
					 settings->currentverse);
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
	display_change = FALSE;
	if (settings->text_last_page != page_num) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (settings->notebook_percomm),
				      page_num);
	}
	backend_display_percomm(page_num, key);
	display_change = TRUE;
}

/******************************************************************************
 * Name
 *  set_page_percomm
 *
 * Synopsis
 *   #include "percomm.h"
 *   
 *   static void set_page_percomm(gchar * modname, GList * percomm_list,
 *			  SETTINGS * s)	
 *
 * Description
 *   change percomm module by finding page number from module name
 *
 * Return value
 *   void
 */

static void set_page_percomm(gchar * modname, GList * percomm_list,
			     SETTINGS * s)
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
	gtk_notebook_set_page(GTK_NOTEBOOK(s->notebook_percomm), page);
	s->percomm_last_page = page;
	/*
	   gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_percomm),
	   s->percomm_tabs);
	 */
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

void display_percomm(gchar * key)
{
	backend_display_percomm(settings->percomm_last_page, key);
}

/******************************************************************************
 * Name
 *  setup_percomm
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   GList *setup_percomm(SETTINGS * s)
 *
 * Description
 *   set up gui for sword personal comments modules - 
 *   return list of percomm module names
 *
 * Return value
 *   GList *
 */

GList *setup_percomm(SETTINGS * s)
{
	GList *tmp = NULL;
	GList *mods = NULL;
	gchar *mod_name;
	gchar *modbuf;
	gchar *keybuf;
	PC_DATA *p;
	gint count = 0;

	percomm_list = NULL;

	mods = backend_get_list_of_percom_modules();
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
		p->find_dialog = NULL;
		p->ec = gui_percomm_control(s,p->mod_name,count);
		p->html = p->ec->htmlwidget;
		backend_new_percomm_display(p->ec->htmlwidget,
					    p->mod_name, s);
		percomm_list =
		    g_list_append(percomm_list, (PC_DATA *) p);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(s->notebook_percomm),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_percomm_switch_page),
			   percomm_list);

	modbuf = g_strdup(s->personalcommentsmod);
	keybuf = g_strdup(s->currentverse);

	set_page_percomm(modbuf, percomm_list, s);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
	return mods;
}

/******************************************************************************
 * Name
 *  shutdown_text
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *  void shutdown_text(void)	
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
		if (p->find_dialog)
			g_free(p->find_dialog);
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

/******  end of file  ******/
