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

#include "gui/gtkhtml_display.h"
#include "gui/percomm.h"
#include "gui/_editor.h"
#include "gui/editor_toolbar.h"
#include "gui/editor_menu.h"
#include "gui/info_box.h"

#include "main/gs_gnomesword.h"
#include "main/gs_html.h"
#include "main/percomm.h"
#include "main/settings.h"
#include "main/support.h"
#include "main/commentary.h"

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
		save_percomm_note(gstr->str);
		g_print("\nfile writen\n");
	}
	g_string_free(gstr, 0);
	gtk_html_set_editable(e->html, TRUE);
}

/******************************************************************************
 * Name
 *  set_percomm_frame_label
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *   void set_percomm_frame_label(void)	
 *
 * Description
 *   sets percomm frame label to module name or null
 *
 * Return value
 *   void
 */

static void set_percomm_frame_label(GtkWidget * frame, gchar *mod_name)
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
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(settings.notebook_percomm),
				   settings.percomm_tabs);	
	set_percomm_frame_label(cur_p->ec->frame, cur_p->mod_name);
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
		GtkNotebookPage * page, gint page_num, GList * pcl)
{
	PC_DATA *p, *p_last;
	gchar *text_str = NULL;
	/*
	 * get data structure for new module 
	 */
	p_last = (PC_DATA *) g_list_nth_data(pcl, 
				settings.percomm_last_page);
	p = (PC_DATA *) g_list_nth_data(pcl, page_num);
	
	change_percomm_module(p->mod_name);
	strcpy(p->ec->key,settings.currentverse);
	set_percomm_key(p->ec->key);
	
	if(!p->ec->frame)
		gui_add_new_percomm_pane(p);
	
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
	if(percomm_display_change) {
		text_str = get_percomm_text(p->ec->key);
		if(text_str) {
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
	}*/
	if(GTK_CHECK_MENU_ITEM(p->ec->editnote)->active){
		gtk_widget_show(p->ec->frame_toolbar);
		gtk_widget_show(p->ec->handlebox_toolbar);
	}
	
	settings.html_percomm = p->ec->htmlwidget;
	
	GTK_CHECK_MENU_ITEM(p->ec->show_tabs)->active = settings.percomm_tabs;
	gui_percomm_tabs(settings.percomm_tabs);
	update_statusbar(p->ec);
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
					PC_DATA *p)
{
	if(p->ec->personal_comments) {
		gui_save_note(p->ec);
		p->ec->changed = FALSE;
		update_statusbar(p->ec);
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
					PC_DATA *p)
{
	GtkWidget *label1, *label2, *label3, *msgbox;
	gint answer = -1;
	gchar *key;
	
	if(p->ec->personal_comments) {
		key = get_percomm_key();
		
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
			delete_percomm_note();
			break;
		default:
			break;
		}
		settings.percomverse = key;
		p->ec->changed = FALSE;
		update_statusbar(p->ec);
		free(key);
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

void create_percomm_pane(PC_DATA *p)
{
	GtkWidget *vboxPC;
	GtkWidget *frame34;
	GtkWidget *scrolledwindow17;
	GtkWidget *toolbar;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator;	
	
	
	p->ec->personal_comments = TRUE;
	
	p->ec->frame = gtk_frame_new(NULL);
	gtk_widget_ref(p->ec->frame);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->frame",
				 p->ec->frame,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->frame);
	gtk_box_pack_start(GTK_BOX(p->vbox), p->ec->frame, TRUE, TRUE, 0);

	vboxPC = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxPC);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vboxPC", vboxPC,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vboxPC);
	gtk_container_add(GTK_CONTAINER(p->ec->frame), vboxPC);

		
	
	p->ec->frame_toolbar = gtk_frame_new(NULL);
	gtk_widget_ref(p->ec->frame_toolbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "p->ec->frame_toolbar",
				 p->ec->frame_toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_box_pack_start(GTK_BOX(vboxPC), p->ec->frame_toolbar, FALSE,
			   TRUE, 0);
	toolbar =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "toolbar", toolbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar);
	gtk_container_add(GTK_CONTAINER(p->ec->frame_toolbar), toolbar);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar),
				      GTK_RELIEF_NONE);
		
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_SAVE);
	p->ec->btn_save =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save"), _("Save Note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(p->ec->btn_save);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_save",
				 p->ec->btn_save,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_save);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_TRASH);
	p->ec->btn_delete =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"), _("Delete Note"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(p->ec->btn_delete);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_delete",
				 p->ec->btn_delete,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_delete);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_PRINT);
	p->ec->btn_print =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"), _("Print Note"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(p->ec->btn_print);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_print",
				 p->ec->btn_print,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_print);

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
	p->ec->btn_cut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Cut"), _("Cut "), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(p->ec->btn_cut);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_cut",
				 p->ec->btn_cut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_cut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_COPY);
	p->ec->btn_copy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Copy"), _("Copy"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(p->ec->btn_copy);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_copy",
				 p->ec->btn_copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_copy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_PASTE);
	p->ec->btn_paste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Paste"), _("Paste"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(p->ec->btn_paste);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_paste",
				 p->ec->btn_paste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_paste);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_UNDO);
	p->ec->btn_undo =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Undo"), _("Undo"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(p->ec->btn_undo);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_undo",
				 p->ec->btn_undo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_undo);

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
	p->ec->btn_Find =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Find"),
				       _("Find in this note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(p->ec->btn_Find);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_Find",
				 p->ec->btn_Find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_Find);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_SRCHRPL);
	p->ec->btn_replace =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Replace"),
				       _("Find and Replace"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(p->ec->btn_replace);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "p->ec->btn_replace", p->ec->btn_replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_replace);

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
	p->ec->btn_spell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Spell"),
				       _("Spell check note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(p->ec->btn_spell);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->btn_spell",
				 p->ec->btn_spell,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->btn_spell);

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

	p->ec->htmlwidget = gtk_html_new();
	p->ec->html = GTK_HTML(p->ec->htmlwidget);
	gtk_widget_ref(p->ec->htmlwidget);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "p->ec->htmlwidget", p->ec->htmlwidget,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->htmlwidget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow17),
			  p->ec->htmlwidget);
	gtk_html_load_empty(p->ec->html);

	p->ec->statusbar = gtk_statusbar_new();
	gtk_widget_ref(p->ec->statusbar);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "p->ec->statusbar",
				 p->ec->statusbar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->ec->statusbar);
	gtk_box_pack_start(GTK_BOX(vboxPC), p->ec->statusbar, FALSE, TRUE,
			   0);
			   
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
	gtk_signal_connect(GTK_OBJECT(p->ec->htmlwidget), "link_clicked", 
			GTK_SIGNAL_FUNC(on_link_clicked),	/* gs_html.c */
			   NULL);
	gtk_signal_connect(GTK_OBJECT(p->ec->htmlwidget), "on_url", 
			GTK_SIGNAL_FUNC(on_url),	/* gs_html.c */
			   NULL);

	gtk_signal_connect(GTK_OBJECT(p->ec->btn_save), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_save_clicked), p);
	
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_delete), "clicked",
		   GTK_SIGNAL_FUNC(on_btn_delete_clicked),
		   p);
		   		   
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_print), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_print_clicked), p->ec);
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_cut), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_cut_clicked), p->ec);
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_copy), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_copy_clicked), p->ec);
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_paste), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_paste_clicked), p->ec);
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_undo), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_undo_clicked), p->ec);
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_Find), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_Find_clicked), p->ec);
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_replace), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_replace_clicked),
			   p->ec);
	gtk_signal_connect(GTK_OBJECT(p->ec->btn_spell), "clicked",
			   GTK_SIGNAL_FUNC(spell_check_cb), p->ec);
	
	settings.toolbarComments = toolbar_style(p->ec);
	gtk_widget_hide(p->ec->handlebox_toolbar);
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

void gui_display_percomm(gchar *key)
{	
	gchar *text_str = NULL;
	settings.percomverse = key;
	strcpy(cur_p->ec->key, key);
	text_str = get_percomm_text(key);
	if(text_str) {
		entry_display(cur_p->html, cur_p->mod_name, text_str, key, FALSE);
		free(text_str);
	}
	update_statusbar(cur_p->ec);
}

/******************************************************************************
 * Name
 *  gui_set_percomm_page_and_key
 *
 * Synopsis
 *   #include "_percomm.h"
 *
 *  void gui_set_percomm_page_and_key(gint page_num, gchar * key)	
 *
 * Description
 *   change percomm module notebook page and display new key (reference)
 *
 * Return value
 *   void
 */

void gui_set_percomm_page_and_key(gint page_num, gchar * key)
{
	gchar *text_str = NULL;
	/*
	 * we don't want backend_dispaly_percomm to be
	 * called by on_notebook_percomm_switch_page
	 */
	percomm_display_change = FALSE;
	strcpy(cur_p->ec->key, key);
	settings.percomverse = key;
	if (settings.text_last_page != page_num) {
		gtk_notebook_set_page(GTK_NOTEBOOK
				      (settings.notebook_percomm),
				      page_num);
	}
	
	text_str = get_percomm_text(key);
	if(text_str) {
		entry_display(cur_p->html, cur_p->mod_name, text_str, 
							key, FALSE);
		free(text_str);
	}
	update_statusbar(cur_p->ec);
	percomm_display_change = TRUE;
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
	if(page == 0)
		on_notebook_percomm_switch_page(
			(GtkNotebook *)settings.notebook_percomm,
			NULL, page, percomm_list);
	else
		gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_percomm), page);
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
 *   creates a text pane when user selects a new text module
 *
 * Return value
 *   void
 */

void gui_add_new_percomm_pane(PC_DATA *p)
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

static void add_vbox_to_notebook(PC_DATA *p)
{	
	GtkWidget *label;
	
	p->vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(p->vbox);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), 
			"p->vbox", p->vbox,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(p->vbox);
	gtk_container_add(GTK_CONTAINER(settings.notebook_percomm), p->vbox);
	
	label = gtk_label_new(p->mod_name);
	gtk_widget_ref(label);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label",
				 label,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(settings.notebook_percomm),
				gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(settings.notebook_percomm),
							p->mod_num), label);	
							     
	gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(settings.notebook_percomm),
                                gtk_notebook_get_nth_page(GTK_NOTEBOOK
							(settings.notebook_percomm),
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

void gui_setup_percomm(GList *mods)
{
	GList *tmp = NULL;
	gchar *modbuf;
	PC_DATA *p;
	gint count = 0;

	percomm_list = NULL;
	percomm_display_change = TRUE;
	
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		p = g_new(PC_DATA, 1);
		p->mod_name = (gchar *) tmp->data;
		p->mod_num = count;
		p->search_string = NULL;
		p->ec = gs_html_editor_control_data_new();
		strcpy(p->ec->key, settings.currentverse);
		p->ec->frame = NULL;
		strcpy(p->ec->filename,p->mod_name);
		add_vbox_to_notebook(p);
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


/******   end of file   ******/
