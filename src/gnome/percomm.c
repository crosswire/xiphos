/*
 * GnomeSword Bible Study Tool
 * percom.c - personal comments gui
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

#include "gui/gnomesword.h"
#include "gui/gtkhtml_display.h"
#include "gui/percomm.h"
#include "gui/editor.h"
#include "gui/toolbar_style.h"
#include "gui/toolbar_edit.h"
#include "gui/editor_menu.h"
#include "gui/editor_spell.h"
#include "gui/utilities.h"
#include "gui/html.h"
#include "gui/dialog.h"

#include "main/percomm.h"
#include "main/settings.h"
#include "main/commentary.h"
#include "main/xml.h"
 
/******************************************************************************
 * global to this file only
 */
static PC_DATA *cur_p;
static gboolean percomm_display_change;
static GList *percomm_list;
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
				   const char *data, unsigned int len,
				   void *user_data)
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
 *  gui_save_note
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void gui_save_note(GSHTMLEditorControlData * e)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_save_note(GSHTMLEditorControlData * e)
{
	gtk_html_set_editable(e->html, FALSE);
	gstr = g_string_new("");
	if (!gtk_html_save
	    (e->html, (GtkHTMLSaveReceiverFn) save_note_receiver,
	     GINT_TO_POINTER(0))) {
		g_warning("file not writen");
	} else {
		//change_percomm_module(e->filename);
		//set_percomm_key(e->key);
		save_percomm_note(gstr->str);
		/*g_print("\nmod = %s\nkey = %s\nnote = %s\n",e->filename, 
				e->key,gstr->str); */
		g_print("\nfile writen\n");
	}
	g_string_free(gstr, 0);
	gtk_html_set_editable(e->html, TRUE);
}


/******************************************************************************
 * Name
 *  gui_percomm_tabs
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void gui_percomm_tabs(gboolean choice)
 *
 * Description
 *    show/hide percomm notebook tabs
 *
 * Return value
 *   void
 */

void gui_percomm_tabs(gboolean choice)
{
	settings.percomm_tabs = choice;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK
				   (widgets.notebook_percomm),
				   settings.percomm_tabs);
	//set_percomm_frame_label(cur_p->ec->frame, cur_p->mod_name);
}


/******************************************************************************
 * Name
 *  on_notebook_percomm_switch_page
 *
 * Synopsis
 *   #include "_percomm.h"
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

static void on_notebook_percomm_switch_page(GtkNotebook * notebook,
					    GtkNotebookPage * page,
					    gint page_num, GList * pcl)
{
	PC_DATA *p, *p_last;
	gchar *text_str = NULL;
	/*
	 * get data structure for new module 
	 */
	p_last = (PC_DATA *) g_list_nth_data(pcl,
					     settings.
					     percomm_last_page);
	p = (PC_DATA *) g_list_nth_data(pcl, page_num);
	/*
	 * point PC_DATA *cur_p to p - PC_DATA is global
	 */
	cur_p = p;

	change_percomm_module(p->mod_name);
	strcpy(p->ec->key, settings.currentverse);
	set_percomm_key(p->ec->key);

	if (!p->ec->htmlwidget)
		gui_add_new_percomm_pane(p);

	/*
	 * remember new module name
	 */
	settings.personalcommentsmod = p->mod_name;
	xml_set_value("GnomeSword", "modules", "percomm",p->mod_name);
	/*
	 * remember page number
	 */
	settings.percomm_last_page = page_num;
	/*
	 * display new module with current verse
	 */
	if (percomm_display_change) {
		text_str = get_percomm_text(p->ec->key);
		if (text_str) {
			entry_display(p->ec->htmlwidget, p->mod_name,
				      text_str, p->ec->key, FALSE);
			free(text_str);
		}
	}
	/*
	 * set edit mode
	 *//*
	   if(GTK_CHECK_MENU_ITEM(p_last->ec->editnote)->active) {
	   gtk_widget_hide(p_last->ec->frame_toolbar);
	   gtk_widget_hide(p_last->ec->handlebox_toolbar);
	   } */
	if (GTK_CHECK_MENU_ITEM(p->ec->editnote)->active) {
		gtk_widget_show(p->ec->toolbar_edit);
		gtk_widget_show(p->ec->toolbar_style);
	}

	widgets.html_percomm = p->ec->htmlwidget;

	GTK_CHECK_MENU_ITEM(p->ec->show_tabs)->active =
	    settings.percomm_tabs;
	gui_percomm_tabs(settings.percomm_tabs);
	gui_update_statusbar(p->ec);
}

/******************************************************************************
 * Name
 *  release
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   gint release(GtkWidget * widget, GdkEventButton * event,
 *					GSHTMLEditorControlData * cd)	
 *
 * Description
 *    ?????
 *
 * Return value
 *   gint
 */

static gint release(GtkWidget * widget, GdkEventButton * event,
		    GSHTMLEditorControlData * cd)
{

	return FALSE;
}

/******************************************************************************
 * Name
 *  html_key_pressed
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   gint html_key_pressed(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *   a key has been pressed - ecd->changed to true 
 *
 * Return value
 *   gint
 */

static gint html_key_pressed(GtkWidget * html, GdkEventButton * event,
			     GSHTMLEditorControlData * ecd)
{
	ecd->changed = TRUE;
	//file_changed = TRUE;
	gui_update_statusbar(ecd);
	return 1;
}

/******************************************************************************
 * Name
 *  html_load_done
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void html_load_done(GtkWidget * html,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *   ???? 
 *
 * Return value
 *   void
 */

static void html_load_done(GtkWidget * html,
			   GSHTMLEditorControlData * ecd)
{
	gui_update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_submit
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_submit(GtkHTML * html, const gchar * method,
 *		      const gchar * url, const gchar * encoding,
 *		      GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    ?????
 *
 * Return value
 *   void
 */

static void on_submit(GtkHTML * html, const gchar * method,
		      const gchar * url, const gchar * encoding,
		      GSHTMLEditorControlData * ecd)
{
	/*
	   GList *l;

	   l = NULL;

	   l=html->engine->form->elements;
	   while (l != NULL) {  
	   g_warning((gchar *) l->data);
	   l = g_list_next(l);
	   }
	   g_list_free(l);
	 */
	g_warning(method);
	g_warning(url);
	g_warning(encoding);

}

/******************************************************************************
 * Name
 *  html_button_pressed
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    mouse button pressed in editor 
 *
 * Return value
 *   gint
 */

static gint html_button_pressed(GtkWidget * html,
				GdkEventButton * event,
				GSHTMLEditorControlData * ecd)
{
	if (ecd->personal_comments)
		settings.whichwindow = PERCOMM_WINDOW;
	if (ecd->studypad)
		settings.whichwindow = STUDYPAD_WINDOW;
	switch (event->button) {
	case 1:
		if (event->type == GDK_2BUTTON_PRESS && ecd->obj
		    && event->state & GDK_CONTROL_MASK) {
			ecd->releaseId =
			    gtk_signal_connect(GTK_OBJECT(html),
					       "button_release_event",
					       GTK_SIGNAL_FUNC(release),
					       ecd);

		}

		else
			return TRUE;
		break;
	case 2:
		/* 
		 * pass this for pasting 
		 */
		return TRUE;
	case 3:
		gtk_signal_emit_stop_by_name(GTK_OBJECT(html),
					     "button_press_event");
		break;
	default:
		return FALSE;
	}

	return FALSE;
}

/******************************************************************************
 * Name
 *  on_html_enter_notify_event
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   gboolean on_html_enter_notify_event(GtkWidget * widget,
 *			   GdkEventCrossing * event,
 *			   GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    mouse moved into editor - sets studypad html widget to edit mode
 *
 * Return value
 *   gboolean
 */

static gboolean on_html_enter_notify_event(GtkWidget * widget,
					   GdkEventCrossing * event,
					   GSHTMLEditorControlData *
					   ecd)
{
	if (!ecd->personal_comments && !ecd->gbs)
		if (!gtk_html_get_editable(ecd->html))
			gtk_html_set_editable(ecd->html, TRUE);
	return TRUE;
}

/******************************************************************************
 * Name
 *   gui_percomm_control
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   GSHTMLEditorControlData *gui_percomm_control(gchar *mod_name,
 *   		gint page_num)	
 *
 * Description
 *   create personal commentary editor control
 *
 * Return value
 *   GSHTMLEditorControlData *
 */

static void create_percomm_pane(PC_DATA * p)
{
	GtkWidget *vboxPC;
	GtkWidget *vbox;
	GtkWidget *frame34;
	GtkWidget *scrolledwindow17;
	GtkWidget *toolbar;

	p->ec->personal_comments = TRUE;
	
	vboxPC = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vboxPC);
	gtk_box_pack_start(GTK_BOX(p->vbox), vboxPC, TRUE, TRUE, 0);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_box_pack_start(GTK_BOX(vboxPC), vbox, TRUE, TRUE,
			   0);

	frame34 = gtk_frame_new(NULL);
	gtk_widget_show(frame34);
	gtk_box_pack_end(GTK_BOX(vbox), frame34, TRUE, TRUE, 0);

	scrolledwindow17 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow17);
	gtk_container_add(GTK_CONTAINER(frame34), scrolledwindow17);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow17),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	p->ec->htmlwidget = gtk_html_new();
	p->ec->html = GTK_HTML(p->ec->htmlwidget);
	gtk_widget_show(p->ec->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17),
			  p->ec->htmlwidget);
	gtk_html_load_empty(p->ec->html);

	p->ec->statusbar = gtk_statusbar_new();
	gtk_widget_show(p->ec->statusbar);
	gtk_box_pack_start(GTK_BOX(vboxPC), p->ec->statusbar, FALSE,
			   TRUE, 0);

	p->ec->vbox = vboxPC;

	p->ec->pm = gui_create_editor_popup(p->ec);
	gnome_popup_menu_attach(p->ec->pm, p->ec->htmlwidget, NULL);

	p->html = p->ec->htmlwidget;

	gtk_signal_connect(GTK_OBJECT(p->ec->html), "submit",
			   GTK_SIGNAL_FUNC(on_submit), p->ec);
	gtk_signal_connect(GTK_OBJECT
			   (p->ec->htmlwidget),
			   "load_done",
			   GTK_SIGNAL_FUNC(html_load_done), p->ec);
	gtk_signal_connect(GTK_OBJECT
			   (p->ec->htmlwidget),
			   "key_press_event",
			   GTK_SIGNAL_FUNC(html_key_pressed), p->ec);
	gtk_signal_connect(GTK_OBJECT
			   (p->ec->htmlwidget),
			   "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), p->ec);
	gtk_signal_connect(GTK_OBJECT(p->ec->htmlwidget),
			   "enter_notify_event",
			   GTK_SIGNAL_FUNC(on_html_enter_notify_event),
			   p->ec);
	/* gs_html.c */
	gtk_signal_connect(GTK_OBJECT(p->ec->htmlwidget),
			   "link_clicked",
			   GTK_SIGNAL_FUNC(gui_link_clicked), NULL);
	/* gs_html.c */
	gtk_signal_connect(GTK_OBJECT(p->ec->htmlwidget), "on_url",
			   GTK_SIGNAL_FUNC(gui_url), NULL);

	widgets.toolbar_comments = gui_toolbar_style(p->ec);
	gtk_box_pack_start(GTK_BOX(vbox),
			   widgets.toolbar_comments, FALSE, FALSE, 0);
	gtk_widget_hide(widgets.toolbar_comments);
	toolbar = gui_toolbar_edit(p->ec);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE,
			   FALSE, 0);
	gtk_widget_hide(toolbar);

}

/******************************************************************************
 * Name
 *  gui_display_percomm
 *
 * Synopsis
 *   #include "_percomm.h"
 *   
 *   void gui_display_percomm(gchar * key)	
 *
 * Description
 *   call backend_display_percomm and pass module page num and key
 *
 * Return value
 *   void
 */

void gui_display_percomm(gchar * key)
{
	gchar *text_str = NULL;
	settings.percomverse = key;
	strcpy(cur_p->ec->key, key);
	text_str = get_percomm_text(key);
	if (text_str) {
		entry_display(cur_p->html, cur_p->mod_name, text_str,
			      key, FALSE);
		free(text_str);
	}
	gui_update_statusbar(cur_p->ec);
}

/******************************************************************************
 * Name
 *  gui_set_percomm_page_and_key
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *  void gui_set_percomm_page(gchar * mod_name)	
 *
 * Description
 *   change percomm module notebook page
 *
 * Return value
 *   void
 */

void gui_set_percomm_page(gchar * mod_name)
{
	gint page = 0;
	PC_DATA *p = NULL;

	percomm_list = g_list_first(percomm_list);
	while (percomm_list != NULL) {
		p = (PC_DATA *) percomm_list->data;
		if (!strcmp(p->mod_name, mod_name))
			break;
		++page;
		percomm_list = g_list_next(percomm_list);
	}
	
	if (!p->ec->htmlwidget)
		gui_add_new_percomm_pane(p);
	
	GTK_CHECK_MENU_ITEM(p->ec->editnote)->active = TRUE;
	
	if (settings.text_last_page != page) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_percomm),
				      page);
	}
}

/******************************************************************************
 * Name
 *  set_page_percomm
 *
 * Synopsis
 *   #include "_percomm.h"
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
	if (page == 0)
		on_notebook_percomm_switch_page((GtkNotebook *) widgets.
						notebook_percomm, NULL,
						page, percomm_list);
	else
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (widgets.notebook_percomm), page);
	settings.percomm_last_page = page;
}

/******************************************************************************
 * Name
 *  gui_add_new_pane
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void gui_add_new_pane(TEXT_DATA * t)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_add_new_percomm_pane(PC_DATA * p)
{
	create_percomm_pane(p);
}

/******************************************************************************
 * Name
 *  add_vbox_to_notebook
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void add_vbox_to_notebook(TEXT_DATA * t)
 *
 * Description
 *   adds a vbox and label to the text notebook for each text module
 *
 * Return value
 *   void
 */

static void add_vbox_to_notebook(PC_DATA * p)
{
	GtkWidget *label;

	p->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(p->vbox);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_percomm),
			  p->vbox);

	label = gtk_label_new(p->mod_name);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK
				   (widgets.notebook_percomm),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (widgets.notebook_percomm),
				    p->mod_num), label);

	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (widgets.notebook_percomm),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (widgets.notebook_percomm),
					  p->mod_num), p->mod_name);
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

void gui_setup_percomm(GList * mods)
{
	GList *tmp = NULL;
	gchar *modbuf;
	PC_DATA *p;
	gint count = 0;

	percomm_list = NULL;
	percomm_display_change = TRUE;

	widgets.notebook_percomm = gtk_notebook_new();
	gtk_widget_ref(widgets.notebook_percomm);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "widgets.notebook_percomm",
				 widgets.notebook_percomm,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.notebook_percomm);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK
				    (widgets.notebook_percomm), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK
				  (widgets.notebook_percomm));
	GTK_WIDGET_UNSET_FLAGS(widgets.notebook_percomm, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK
				   (widgets.notebook_percomm), TRUE);

	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		p = g_new(PC_DATA, 1);
		p->mod_name = (gchar *) tmp->data;
		p->mod_num = count;
		p->search_string = NULL;
		p->ec = gs_html_editor_control_data_new();
		strcpy(p->ec->key, settings.currentverse);
		p->ec->htmlwidget = NULL;
		p->ec->stylebar = load_percomm_options(p->mod_name, "Style bar");
		p->ec->editbar = load_percomm_options(p->mod_name, "Edit bar");
		strcpy(p->ec->filename, p->mod_name);
		add_vbox_to_notebook(p);
		percomm_list =
		    g_list_append(percomm_list, (PC_DATA *) p);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(widgets.notebook_percomm),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_percomm_switch_page),
			   percomm_list);

	modbuf = g_strdup(settings.personalcommentsmod);

	set_page_percomm(modbuf, percomm_list);

	g_free(modbuf);
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

void gui_shutdown_percomm(void)
{
	percomm_list = g_list_first(percomm_list);
	while (percomm_list != NULL) {
		PC_DATA *p = (PC_DATA *) percomm_list->data;
		/* 
		 * free any search dialogs created 
		 */		
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


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *  	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_percomm_in_workbook(GtkWidget * workbook_lower, gint page_num)
{
	GtkWidget *label;
	
	settings.percomm_page = page_num;
	
	widgets.vbox_percomm = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(widgets.vbox_percomm);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "widgets.vbox_percomm",
				 widgets.vbox_percomm,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.vbox_percomm);
	gtk_container_add(GTK_CONTAINER(workbook_lower), widgets.vbox_percomm);

	/*
	 * personal comments editor goes here
	 */

	label = gtk_label_new(_("Personal Comments"));
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "label",
				 label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(workbook_lower),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK
				    (workbook_lower), page_num),
				   label);

	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
					 (workbook_lower),
					 gtk_notebook_get_nth_page
					 (GTK_NOTEBOOK
					  (workbook_lower),
					  page_num),
					 _("Personal Comments"));
}


/******   end of file   ******/
