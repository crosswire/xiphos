/*
 * GnomeSword Bible Study Tool
 * toolbar_edit.c - style toolbar for editors
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
/* 
 * most of this code is from the GtkHTML library.
 */
#include <config.h>
#include <gnome.h>
#include <gal/widgets/widget-color-combo.h>

#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlsettings.h>

#include "gui/toolbar_edit.h"
#include "gui/gnomesword.h"
#include "gui/dialog.h"
#include "gui/editor_spell.h"
#include "gui/studypad.h"
#include "gui/html.h"

#include "main/settings.h"
#include "main/percomm.h"

/******************************************************************************
 * Name
 *  on_btn_open_clicked
 *
 * Synopsis
 *   #include "studypad.h"
 *
 *   void on_btn_open_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open file dialog 
 *
 * Return value
 *   void
 */
 
static void on_btn_open_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)
{
	GtkWidget *openFile;
	gchar buf[255];
	
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		gint test;
		GS_DIALOG *info;
	
		info = gui_new_dialog();
		if (strlen(settings.studypadfilename) > 0 )
			info->label_top = settings.studypadfilename;
		else
			info->label_top = N_("File");
		info->label_middle = N_("has been modified. ");
		info->label_bottom = N_("Do you wish to save it?");
		info->yes = TRUE;
		info->no = TRUE;
	
		test = gui_gs_dialog(info);
		if (test == GS_YES){
			on_btn_save_clicked(NULL,
					ecd);
		}		
		g_free(info);
	}
	sprintf(buf, "%s/*.pad", settings.homedir);
	openFile = gui_fileselection_open(ecd);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(openFile),
					buf);
	gtk_widget_show(openFile);
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
		gui_save_note(ecd);
		ecd->changed = FALSE;
		update_statusbar(ecd);
	}
	else if(ecd->studypad) {
		if(strlen(settings.studypadfilename) > 0)  {
			save_file(ecd->filename, ecd);
		} else {
			gui_fileselection_save(ecd);
		}
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
		GS_DIALOG *info;
		gint test;
		gchar *key;
		
		key = get_percomm_key();		
		info = gui_new_dialog();
		info->label_top = N_("Are you sure you want");
		info->label_middle = N_("to delete the note for");
		info->label_bottom = key;
		info->yes = TRUE;
		info->no = TRUE;
		
		test = gui_gs_dialog(info);
		if (test == GS_YES) {	
			delete_percomm_note();
			gui_display_percomm(key);
		}
		settings.percomverse = key;
		ecd->changed = FALSE;
		update_statusbar(ecd);
		g_free(info);
		g_free(key);
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
	gui_html_print(ecd->htmlwidget);
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
 *   create_toolbar_edit
 *
 * Synopsis
 *   #include "toolbar_edit.h"
 *
 *   GtkWidget *create_toolbar_edit (GSHTMLEditorControlData *cd)
 *
 * Description
 *   create the edit toolbar
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_toolbar_edit (GSHTMLEditorControlData *ecd)
{	
	GtkWidget *toolbar;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator;	
	
	
	/****************************************************** toolbar_edit */
	
	ecd->toolbar_edit =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(ecd->toolbar_edit);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->toolbar_edit", ecd->toolbar_edit,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->toolbar_edit);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(ecd->toolbar_edit),
				      GTK_RELIEF_NONE);
		
	if(ecd->studypad) {
		tmp_toolbar_icon =
		    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_OPEN);
		ecd->btn_open =
		    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON, NULL,
					       _("Open"), _("Open File"), NULL,
					       tmp_toolbar_icon, NULL, NULL);
		gtk_widget_ref(ecd->btn_open);
		gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_open",
					 ecd->btn_open,
					 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(ecd->btn_open);
	
		gtk_signal_connect(GTK_OBJECT(ecd->btn_open), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_open_clicked),
			   ecd);	
	}
		   
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_SAVE);
	ecd->btn_save =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save"), _("Save Note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(ecd->btn_save);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_save",
				 ecd->btn_save,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_save);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_TRASH);
	ecd->btn_delete =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Delete"), _("Delete Note"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(ecd->btn_delete);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_delete",
				 ecd->btn_delete,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_delete);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_PRINT);
	ecd->btn_print =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"), _("Print Note"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(ecd->btn_print);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_print",
				 ecd->btn_print,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_print);

	vseparator = gtk_vseparator_new();
	gtk_widget_ref(vseparator);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vseparator",
				 vseparator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(ecd->toolbar_edit), vseparator,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_CUT);
	ecd->btn_cut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Cut"), _("Cut "), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(ecd->btn_cut);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_cut",
				 ecd->btn_cut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_cut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_COPY);
	ecd->btn_copy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Copy"), _("Copy"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(ecd->btn_copy);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_copy",
				 ecd->btn_copy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_copy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_PASTE);
	ecd->btn_paste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Paste"), _("Paste"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(ecd->btn_paste);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_paste",
				 ecd->btn_paste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_paste);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_UNDO);
	ecd->btn_undo =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Undo"), _("Undo"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(ecd->btn_undo);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_undo",
				 ecd->btn_undo,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_undo);

	vseparator = gtk_vseparator_new();
	gtk_widget_ref(vseparator);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vseparator",
				 vseparator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(ecd->toolbar_edit), vseparator,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_SEARCH);
	ecd->btn_Find =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Find"),
				       _("Find in this note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(ecd->btn_Find);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_Find",
				 ecd->btn_Find,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_Find);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_SRCHRPL);
	ecd->btn_replace =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Replace"),
				       _("Find and Replace"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(ecd->btn_replace);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app),
				 "ecd->btn_replace", ecd->btn_replace,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_replace);

	vseparator = gtk_vseparator_new();
	gtk_widget_ref(vseparator);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "vseparator",
				 vseparator,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(ecd->toolbar_edit), vseparator,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	ecd->btn_spell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Spell"),
				       _("Spell check note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(ecd->btn_spell);
	gtk_object_set_data_full(GTK_OBJECT(widgets.app), "ecd->btn_spell",
				 ecd->btn_spell,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ecd->btn_spell);
	
#ifdef USE_SPELL	
	gtk_widget_set_sensitive(ecd->btn_spell, 1);
#else
	gtk_widget_set_sensitive(ecd->btn_spell, 0);
#endif
	
	gtk_signal_connect(GTK_OBJECT(ecd->btn_save), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_save_clicked), ecd);
	
	gtk_signal_connect(GTK_OBJECT(ecd->btn_delete), "clicked",
		   GTK_SIGNAL_FUNC(on_btn_delete_clicked),
		   ecd);		   		   
	gtk_signal_connect(GTK_OBJECT(ecd->btn_print), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_print_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(ecd->btn_cut), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_cut_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(ecd->btn_copy), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_copy_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(ecd->btn_paste), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_paste_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(ecd->btn_undo), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_undo_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(ecd->btn_Find), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_Find_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(ecd->btn_replace), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_replace_clicked),
			   ecd);
#ifdef USE_SPELL			   
	gtk_signal_connect(GTK_OBJECT(ecd->btn_spell), "clicked",
			   GTK_SIGNAL_FUNC(spell_check_cb), ecd);
#endif	
	/************************************************** end toolbar_edit */
	
	return ecd->toolbar_edit;
}



/******************************************************************************
 * Name
 *   toolbar_edit
 *
 * Synopsis
 *   #include "toolbar_edit.h"
 *
 *   GtkWidget *toolbar_edit (GSHTMLEditorControlData *cd)
 *
 * Description
 *   calls create_toolbar_edit() and returns created toolbar
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_toolbar_edit (GSHTMLEditorControlData *ecd)
{
	g_return_val_if_fail (ecd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (ecd->html), NULL);

	return create_toolbar_edit (ecd);
}
