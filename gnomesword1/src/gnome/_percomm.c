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
#include "main/spell.h"
#include "main/spell_gui.h"
#endif

#include "gui/_percomm.h"
#include "gui/_editor.h"
#include "gui/editor_toolbar.h"
#include "gui/editor_menu.h"

#include "main/gs_gnomesword.h"
#include "main/gs_html.h"
#include "main/percomm.h"
#include "main/settings.h"
#include "main/support.h"

/******************************************************************************
 * externs
 */
extern PC_DATA *cur_p;
extern gboolean percomm_display_change;

/******************************************************************************
 * Name
 *  gui_set_percomm_frame_label
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void gui_set_percomm_frame_label(void)	
 *
 * Description
 *   sets percomm frame label to module name or null
 *
 * Return value
 *   void
 */

void gui_set_percomm_frame_label(GtkWidget * frame, gchar *mod_name)
{
	/*
	 * set frame label to NULL if tabs are showing
	 * else set frame label to module name
	 */	
	if (settings.percomm_tabs)
		gtk_frame_set_label(GTK_FRAME(frame), NULL);
	else
		gtk_frame_set_label(GTK_FRAME(frame), mod_name);
	
}

/******************************************************************************
 * Name
 *  tabs
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void tabs(gboolean choice)
 *
 * Description
 *    show/hide percomm notebook tabs
 *
 * Return value
 *   void
 */ 
 
void tabs(gboolean choice)
{
	settings.percomm_tabs = choice;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebook_percomm),
				   settings.percomm_tabs);	
	gui_set_percomm_frame_label(cur_p->ec->frame, cur_p->mod_name);
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
 
void on_notebook_percomm_switch_page(GtkNotebook * notebook,
		GtkNotebookPage * page, gint page_num, GList * pcl)
{
	PC_DATA *p, *p_last;
	/*
	 * get data structure for new module 
	 */
	p_last = (PC_DATA *) g_list_nth_data(pcl, 
				settings.percomm_last_page);
	p = (PC_DATA *) g_list_nth_data(pcl, page_num);
	/*
	 * remember new module name
	 */
	strcpy(settings.personalcommentsmod, p->mod_name);
	/*
	 * point PC_DATA *cur_p to p - PC_DATA is global
	 */
	cur_p = p;
	/*
	 * remember page number
	 */
	settings.percomm_last_page = page_num;
	/*
	 * display new module with current verse
	 */
	if (percomm_display_change) {
		set_percomm_page_and_key(page_num,
					 settings.currentverse);
	}
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
	settings.html_percomm = p->ec->htmlwidget;
	
	GTK_CHECK_MENU_ITEM(p->ec->show_tabs)->active = settings.percomm_tabs;
	tabs(settings.percomm_tabs);
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
	update_statusbar(ecd);
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
	update_statusbar(ecd);
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

static gint html_button_pressed(GtkWidget * html, GdkEventButton * event,
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
			   GSHTMLEditorControlData * ecd)
{
	if (!ecd->personal_comments && !ecd->gbs)
		if (!gtk_html_get_editable(ecd->html))
			gtk_html_set_editable(ecd->html, TRUE);
	return TRUE;
}

/******************************************************************************
 * Name
 *  on_btn_save_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_save_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    save contents of editor
 *
 * Return value
 *   void
 */
 
static void on_btn_save_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)
{
	if(ecd->personal_comments) {
		editor_save_note(ecd->htmlwidget);
		update_statusbar(ecd);
	}
		
}


/******************************************************************************
 * Name
 *  on_btn_delete_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_delete_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    delete personal commnet
 *
 * Return value
 *   void
 */
 
static void on_btn_delete_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)
{
	if(ecd->personal_comments) {
		delete_percomm_note();
		ecd->changed = FALSE;
		update_statusbar(ecd);
	}
}

/******************************************************************************
 * Name
 *  on_btn_print_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_print_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    print the editor text
 *
 * Return value
 *   void
 */
 
static void on_btn_print_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	html_print(ecd->htmlwidget);
}

/******************************************************************************
 * Name
 *  on_btn_cut_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_cut_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    cut selected text to clipboard
 *
 * Return value
 *   void
 */
 
static void on_btn_cut_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_html_cut(ecd->html);
	ecd->changed = TRUE;
	update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_btn_copy_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_copy_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    copy selected text to clipboard
 *
 * Return value
 *   void
 */
 
static void on_btn_copy_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_html_copy(ecd->html);
}

/******************************************************************************
 * Name
 *  on_btn_paste_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_paste_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    paste contents of clipboard into editor
 *
 * Return value
 *   
 */
 
static void on_btn_paste_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_html_paste(ecd->html);
	ecd->changed = TRUE;
	update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_btn_undo_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_undo_clicked(GtkButton * button, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    undo changes in the editor
 *
 * Return value
 *   void
 */ 

static void on_btn_undo_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_html_undo(ecd->html);
	ecd->changed = TRUE;
	update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_btn_Find_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_Find_clicked(GtkButton *button, 
 *					GSHTMLEditorControlData *ecd)	
 *
 * Description
 *    open find dialog
 *
 * Return value
 *   void
 */ 

static void on_btn_Find_clicked(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	search(ecd, FALSE, NULL);
}


/******************************************************************************
 * Name
 *  on_btn_replace_clicked
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void on_btn_replace_clicked(GtkButton * button,
		       GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open find and replace dialog
 *
 * Return value
 *   void
 */
 
static void on_btn_replace_clicked(GtkButton * button,
		       GSHTMLEditorControlData * ecd)
{
	replace(ecd);
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

GSHTMLEditorControlData *gui_percomm_control(gchar *mod_name, gint page_num)
{
	GtkWidget * vbox;
	GtkWidget *vboxPC;
	GtkWidget *label85;
	GtkWidget *frame34;
	GtkWidget *scrolledwindow17;
	GtkWidget *toolbar;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator;

	
	GSHTMLEditorControlData *pcecd =
	    gs_html_editor_control_data_new();
	
	pcecd->personal_comments = TRUE;

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox",
				 vbox,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(settings.notebook_percomm), vbox);
	
	pcecd->frame = gtk_frame_new(NULL);
	gtk_widget_ref(pcecd->frame);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->frame",
				 pcecd->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->frame);
	gtk_box_pack_start(GTK_BOX(vbox), pcecd->frame, TRUE, TRUE, 0);

	vboxPC = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxPC);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vboxPC", vboxPC,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxPC);
	gtk_container_add(GTK_CONTAINER(pcecd->frame), vboxPC);

		
	
	pcecd->frame_toolbar = gtk_frame_new(NULL);
	gtk_widget_ref(pcecd->frame_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "pcecd->frame_toolbar",
				 pcecd->frame_toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(vboxPC), pcecd->frame_toolbar, FALSE,
			   TRUE, 0);
	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "toolbar", toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_container_add(GTK_CONTAINER(pcecd->frame_toolbar), toolbar);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);
		
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_SAVE);
	pcecd->btn_save =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save"), _("Save Note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(pcecd->btn_save);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_save",
				 pcecd->btn_save,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_save);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_TRASH);
	pcecd->btn_delete =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"), _("Delete Note"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(pcecd->btn_delete);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_delete",
				 pcecd->btn_delete,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_delete);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_PRINT);
	pcecd->btn_print =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"), _("Print Note"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(pcecd->btn_print);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_print",
				 pcecd->btn_print,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_print);

	vseparator = gtk_vseparator_new();
	gtk_widget_ref(vseparator);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vseparator",
				 vseparator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_CUT);
	pcecd->btn_cut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Cut"), _("Cut "), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(pcecd->btn_cut);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_cut",
				 pcecd->btn_cut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_cut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_COPY);
	pcecd->btn_copy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Copy"), _("Copy"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(pcecd->btn_copy);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_copy",
				 pcecd->btn_copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_copy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_PASTE);
	pcecd->btn_paste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Paste"), _("Paste"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(pcecd->btn_paste);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_paste",
				 pcecd->btn_paste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_paste);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_UNDO);
	pcecd->btn_undo =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Undo"), _("Undo"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(pcecd->btn_undo);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_undo",
				 pcecd->btn_undo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_undo);

	vseparator = gtk_vseparator_new();
	gtk_widget_ref(vseparator);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vseparator",
				 vseparator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_SEARCH);
	pcecd->btn_Find =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Find"),
				       _("Find in this note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(pcecd->btn_Find);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_Find",
				 pcecd->btn_Find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_Find);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_SRCHRPL);
	pcecd->btn_replace =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Replace"),
				       _("Find and Replace"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(pcecd->btn_replace);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "pcecd->btn_replace", pcecd->btn_replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_replace);

	vseparator = gtk_vseparator_new();
	gtk_widget_ref(vseparator);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vseparator",
				 vseparator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar), vseparator,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	pcecd->btn_spell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Spell"),
				       _("Spell check note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(pcecd->btn_spell);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->btn_spell",
				 pcecd->btn_spell,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->btn_spell);

	frame34 = gtk_frame_new(NULL);
	gtk_widget_ref(frame34);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame34", frame34,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame34);
	gtk_box_pack_start(GTK_BOX(vboxPC), frame34, TRUE, TRUE, 0);

	scrolledwindow17 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow17);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindow17", scrolledwindow17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow17);
	gtk_container_add(GTK_CONTAINER(frame34), scrolledwindow17);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow17),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	pcecd->htmlwidget = gtk_html_new();
	pcecd->html = GTK_HTML(pcecd->htmlwidget);
	gtk_widget_ref(pcecd->htmlwidget);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "pcecd->htmlwidget", pcecd->htmlwidget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17),
			  pcecd->htmlwidget);
	gtk_html_load_empty(pcecd->html);

	pcecd->statusbar = gtk_statusbar_new();
	gtk_widget_ref(pcecd->statusbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "pcecd->statusbar",
				 pcecd->statusbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pcecd->statusbar);
	gtk_box_pack_start(GTK_BOX(vboxPC), pcecd->statusbar, FALSE, TRUE,
			   0);
			   
	pcecd->vbox = vboxPC;
	pcecd->pm = gui_create_editor_popup(pcecd);
	gnome_popup_menu_attach(pcecd->pm, pcecd->htmlwidget, NULL);
	
	gtk_signal_connect(GTK_OBJECT(pcecd->html), "submit",
			   GTK_SIGNAL_FUNC(on_submit), pcecd);
	gtk_signal_connect(GTK_OBJECT
			   (pcecd->htmlwidget),
			   "load_done",
			   GTK_SIGNAL_FUNC(html_load_done), pcecd);
	gtk_signal_connect(GTK_OBJECT
			   (pcecd->htmlwidget),
			   "key_press_event",
			   GTK_SIGNAL_FUNC(html_key_pressed), pcecd);
	gtk_signal_connect(GTK_OBJECT
			   (pcecd->htmlwidget),
			   "button_press_event",
			   GTK_SIGNAL_FUNC(html_button_pressed), pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->htmlwidget),
			   "enter_notify_event",
			   GTK_SIGNAL_FUNC(on_html_enter_notify_event),
			   pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->htmlwidget), "link_clicked", 
			GTK_SIGNAL_FUNC(on_link_clicked),	/* gs_html.c */
			   NULL);
	gtk_signal_connect(GTK_OBJECT(pcecd->htmlwidget), "on_url", 
			GTK_SIGNAL_FUNC(on_url),	/* gs_html.c */
			   NULL);

	gtk_signal_connect(GTK_OBJECT(pcecd->btn_save), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_save_clicked), pcecd);
	
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_delete), "clicked",
		   GTK_SIGNAL_FUNC(on_btn_delete_clicked),
		   pcecd);
		   		   
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_print), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_print_clicked), pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_cut), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_cut_clicked), pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_copy), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_copy_clicked), pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_paste), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_paste_clicked), pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_undo), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_undo_clicked), pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_Find), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_Find_clicked), pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_replace), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_replace_clicked),
			   pcecd);
	gtk_signal_connect(GTK_OBJECT(pcecd->btn_spell), "clicked",
			   GTK_SIGNAL_FUNC(spell_check_cb), pcecd);

	
	label85 = gtk_label_new(mod_name);
	gtk_widget_ref(label85);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label85",
				 label85,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label85);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.notebook_percomm),
				gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(settings.notebook_percomm),
							page_num), label85);	
							     
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.notebook_percomm),
                                gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(settings.notebook_percomm),
							page_num), mod_name);
	
	
	settings.toolbarComments = toolbar_style(pcecd);
	gtk_widget_hide(pcecd->handlebox_toolbar);
	return pcecd;
}

/******   end of file   ******/
