/*
 * GnomeSword Bible Study Tool
 * toolbar_edit.c - style toolbar for editors
 *               
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
 * this code originally came from the GtkHTML library.
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
#include "gui/percomm.h"
#include "gui/commentary_dialog.h"
#include "gui/fileselection.h"
#include "gui/editor_menu.h"
#include "gui/find_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/percomm.h"

static void on_btn_save_clicked(GtkButton * button,
				GSHTMLEditorControlData * ecd);

/******************************************************************************
 * Name
 *  new_clicked
 *
 * Synopsis
 *   #include "toolbar_edit.h"
 *
 *   void new_clicked(GtkButton * button,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *     
 *
 * Return value
 *   void
 */

static void new_clicked(GtkButton * button,
				GSHTMLEditorControlData * ecd)
{
	gui_new_activate(NULL, ecd);
}


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
	gchar *tmp_buf = NULL;
	GString *str;
	gchar *filename = NULL;

	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		gint test;
		GS_DIALOG *info;

		info = gui_new_dialog();
		info->stock_icon = "gtk-dialog-warning";
		str = g_string_new("");
		if (settings.studypadfilename)
			tmp_buf = settings.studypadfilename;
		else
			tmp_buf = N_("File");
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			tmp_buf,
			_("has been modified. Do you wish to save it?"));
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			on_btn_save_clicked(NULL, ecd);
		}
		g_free(info);
		g_string_free(str,TRUE);
	}
	sprintf(buf, "%s/*.pad", settings.studypaddir);
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
	gchar *filename = NULL;
	if (ecd->personal_comments) {
		gui_save_note(ecd);
		ecd->changed = FALSE;
		gui_update_statusbar(ecd);
	} else if (ecd->studypad) {
		if (strlen(settings.studypadfilename) > 0) {
			filename = g_strdup(settings.studypadfilename);
			save_file(filename, ecd);
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
	if (ecd->personal_comments) {
		GS_DIALOG *info;
		gint test;
		GString *str;
		
		str = g_string_new("");

		info = gui_new_dialog();
		info->stock_icon = "gtk-dialog-warning";
		g_string_printf(str,"<span weight=\"bold\">%s</span>\n\n%s %s",
			_("Delete Note?"), 
			_("Are you sure you want to delete the note for\n"), 
			ecd->key);
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			delete_percomm_note();
			if(settings.use_percomm_dialog)
				gui_display_commentary_in_dialog(ecd->key);
			else
				gui_display_percomm(ecd->key);
		}
		settings.percomverse = ecd->key;
		ecd->changed = FALSE;
		gui_update_statusbar(ecd);
		g_free(info);
		g_string_free(str,TRUE);
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
	gui_html_print(ecd->htmlwidget, FALSE);
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
	gui_update_statusbar(ecd);
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
	gtk_html_paste(ecd->html,FALSE);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
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
	gui_update_statusbar(ecd);
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
	gui_find_dlg(ecd->htmlwidget, ecd->filename,
		  FALSE, NULL);
	//search(ecd, FALSE, NULL);
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

static GtkWidget *create_toolbar_edit(GSHTMLEditorControlData * ecd)
{
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator;


	/****************************************************** toolbar_edit */

	ecd->toolbar_edit =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_show(ecd->toolbar_edit);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(ecd->toolbar_edit),
				      GTK_RELIEF_NONE);

	if (ecd->studypad) {
		tmp_toolbar_icon =
		    gnome_stock_pixmap_widget(widgets.app,
					      GNOME_STOCK_PIXMAP_NEW);
		ecd->btn_new =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("New"),
					       _("New File"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_show(ecd->btn_new);
		
		gtk_signal_connect(GTK_OBJECT(ecd->btn_new), "clicked",
				   GTK_SIGNAL_FUNC(new_clicked),
				   ecd);
		
		tmp_toolbar_icon =
		    gnome_stock_pixmap_widget(widgets.app,
					      GNOME_STOCK_PIXMAP_OPEN);
		ecd->btn_open =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Open"),
					       _("Open File"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_show(ecd->btn_open);

		gtk_signal_connect(GTK_OBJECT(ecd->btn_open), "clicked",
				   GTK_SIGNAL_FUNC(on_btn_open_clicked),
				   ecd);
		tmp_toolbar_icon =
		    gnome_stock_pixmap_widget(widgets.app,
					      GNOME_STOCK_PIXMAP_SAVE);
		ecd->btn_save =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Save"),
					       _("Save Curent File"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_show(ecd->btn_save);
	} else {
		tmp_toolbar_icon =
		    gnome_stock_pixmap_widget(widgets.app,
					      GNOME_STOCK_PIXMAP_SAVE);
		ecd->btn_save =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Save"),
					       _("Save Note"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_show(ecd->btn_save);

		tmp_toolbar_icon =
		    gnome_stock_pixmap_widget(widgets.app,
					      GNOME_STOCK_PIXMAP_TRASH);
		ecd->btn_delete =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Delete"),
					       _("Delete Note"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_show(ecd->btn_delete);

		gtk_signal_connect(GTK_OBJECT(ecd->btn_delete),
				   "clicked",
				   GTK_SIGNAL_FUNC
				   (on_btn_delete_clicked), ecd);

	}
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_PRINT);
	ecd->btn_print =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Print"),
				       _("Print window contents"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_print);

	vseparator = gtk_vseparator_new();
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(ecd->toolbar_edit),
				  vseparator, NULL, NULL);
	gtk_widget_set_usize(vseparator, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_CUT);
	ecd->btn_cut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Cut"), _("Cut "), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_cut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_COPY);
	ecd->btn_copy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Copy"), _("Copy"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_copy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_PASTE);
	ecd->btn_paste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Paste"), _("Paste"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_paste);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_UNDO);
	ecd->btn_undo =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Undo"), _("Undo"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_undo);

	vseparator = gtk_vseparator_new();
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(ecd->toolbar_edit),
				  vseparator, NULL, NULL);
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
	gtk_widget_show(ecd->btn_replace);

	vseparator = gtk_vseparator_new();
	gtk_widget_show(vseparator);
	gtk_toolbar_append_widget(GTK_TOOLBAR(ecd->toolbar_edit),
				  vseparator, NULL, NULL);
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
	gtk_widget_show(ecd->btn_spell);

#ifdef USE_SPELL
	gtk_widget_set_sensitive(ecd->btn_spell, 1);
#else
	gtk_widget_set_sensitive(ecd->btn_spell, 0);
#endif

	gtk_signal_connect(GTK_OBJECT(ecd->btn_save), "clicked",
			   GTK_SIGNAL_FUNC(on_btn_save_clicked), ecd);

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

GtkWidget *gui_toolbar_edit(GSHTMLEditorControlData * ecd)
{
	g_return_val_if_fail(ecd->html != NULL, NULL);
	g_return_val_if_fail(GTK_IS_HTML(ecd->html), NULL);

	return create_toolbar_edit(ecd);
}
