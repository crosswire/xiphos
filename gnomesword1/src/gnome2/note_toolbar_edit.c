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

#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlsettings.h>

#include "gui/note_toolbar_edit.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"
#include "gui/dialog.h"
#include "gui/editor_spell.h"
#include "gui/studypad.h"
#include "gui/html.h"
//#include "gui/percomm.h"
#include "gui/commentary_dialog.h"
#include "gui/fileselection.h"
#include "gui/editor_menu.h"
#include "gui/find_dialog.h"

#include "main/settings.h"
#include "main/percomm.h"
#include "main/module_dialogs.h"

static void on_btn_save_clicked(GtkButton * button,
				DIALOG_DATA * d);

/******************************************************************************
 * Name
 *  new_clicked
 *
 * Synopsis
 *   #include "toolbar_edit.h"
 *
 *   void new_clicked(GtkButton * button,
					DIALOG_DATA * d)	
 *
 * Description
 *     
 *
 * Return value
 *   void
 */

static void new_clicked(GtkButton * button,
				DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
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
					DIALOG_DATA * d)	
 *
 * Description
 *    open file dialog 
 *
 * Return value
 *   void
 */

static void on_btn_open_clicked(GtkButton * button,
				DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
	GtkWidget *openFile;
	gchar buf[255];
	gchar *tmp_buf = NULL;
	GString *str;

	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		gint test;
		GS_DIALOG *info;
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
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
			on_btn_save_clicked(NULL, d);
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
					DIALOG_DATA * d)	
 *
 * Description
 *    save contents of editor
 *
 * Return value
 *   void
 */

static void on_btn_save_clicked(GtkButton * button,
				DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
	gchar *filename = NULL;
	if (ecd->personal_comments) {
		main_dialog_save_note(d);
		ecd->changed = FALSE;
		gui_update_statusbar(ecd);
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
					DIALOG_DATA * d)	
 *
 * Description
 *    delete personal commnet
 *
 * Return value
 *   void
 */

static void on_btn_delete_clicked(GtkButton * button,
				  DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
	if (ecd->personal_comments) {
		GS_DIALOG *info;
		gint test;
		GString *str;
		gchar *url = NULL;
		
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		g_string_printf(str,"<span weight=\"bold\">%s</span>\n\n%s %s",
						_("Delete Note?"), 
			_("Are you sure you want to delete the note for\n"), ecd->key);
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			main_dialog_delete_note(d);
			url = g_strdup_printf(	"sword://%s/%s",
						ecd->filename,
						ecd->key);
			main_dialog_goto_bookmark(url);
			g_free(url);
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
 *					DIALOG_DATA * d)	
 *
 * Description
 *    print the editor text
 *
 * Return value
 *   void
 */

static void on_btn_print_clicked(GtkButton * button,
				 DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
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
 *					DIALOG_DATA * d)	
 *
 * Description
 *    cut selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_btn_cut_clicked(GtkButton * button,
			       DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
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
 *					DIALOG_DATA * d)	
 *
 * Description
 *    copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_btn_copy_clicked(GtkButton * button,
				DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
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
 *					DIALOG_DATA * d)	
 *
 * Description
 *    paste contents of clipboard into editor
 *
 * Return value
 *   
 */

static void on_btn_paste_clicked(GtkButton * button,
				 DIALOG_DATA * d)
{	
	GSHTMLEditorControlData *ecd = d->editor;
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
 *					DIALOG_DATA * d)	
 *
 * Description
 *    undo changes in the editor
 *
 * Return value
 *   void
 */

static void on_btn_undo_clicked(GtkButton * button,
				DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
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
				DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
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
		       DIALOG_DATA * d)	
 *
 * Description
 *    open find and replace dialog
 *
 * Return value
 *   void
 */

static void on_btn_replace_clicked(GtkButton * button,
				   DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
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

static GtkWidget *create_toolbar_edit(DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator;


	/****************************************************** toolbar_edit */

	ecd->toolbar_edit =
	    gtk_toolbar_new();
	gtk_widget_show(ecd->toolbar_edit);
	gtk_toolbar_set_style (GTK_TOOLBAR (ecd->toolbar_edit), 
				GTK_TOOLBAR_ICONS);
/*	gtk_toolbar_set_button_relief(GTK_TOOLBAR(ecd->toolbar_edit),
				      GTK_RELIEF_NONE);*/

	if (ecd->studypad) {
		tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_NEW, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR 
						(ecd->toolbar_edit)));
		ecd->btn_new =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("New"),
					       _("Create a new file"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_show(ecd->btn_new);
		
		g_signal_connect(GTK_OBJECT(ecd->btn_new), "clicked",
				   G_CALLBACK(new_clicked),
				   ecd);
		
		tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_OPEN, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR 
						(ecd->toolbar_edit)));
		ecd->btn_open =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Open"),
					       _("Open a file"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_show(ecd->btn_open);

		g_signal_connect(GTK_OBJECT(ecd->btn_open), "clicked",
				   G_CALLBACK(on_btn_open_clicked),
				   ecd);
		tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_SAVE, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR 
						(ecd->toolbar_edit)));
		ecd->btn_save =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Save"),
					       _("Save the current file"),
					       NULL, tmp_toolbar_icon,
					       NULL, NULL);
		gtk_widget_show(ecd->btn_save);
	} else {
		tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_SAVE, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
		ecd->btn_save =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Save"),
					       _("Save note"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_show(ecd->btn_save);

		tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_DELETE, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
		ecd->btn_delete =
		    gtk_toolbar_append_element(GTK_TOOLBAR
					       (ecd->toolbar_edit),
					       GTK_TOOLBAR_CHILD_BUTTON,
					       NULL, _("Delete"),
					       _("Delete note"), NULL,
					       tmp_toolbar_icon, NULL,
					       NULL);
		gtk_widget_show(ecd->btn_delete);

		g_signal_connect(GTK_OBJECT(ecd->btn_delete),
				   "clicked",
				   G_CALLBACK
				   (on_btn_delete_clicked), 
				   (DIALOG_DATA *) d);

	}
	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_PRINT, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
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

	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_CUT, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
	ecd->btn_cut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Cut"), _("Cut"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_cut);

	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_COPY, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
	ecd->btn_copy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Copy"), _("Copy"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_copy);

	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_PASTE, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
	ecd->btn_paste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Paste"), _("Paste"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_paste);

	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_UNDO, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
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

	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_FIND, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
	ecd->btn_Find =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Find"),
				       _("Find in this note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_Find);
	
	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_FIND_AND_REPLACE, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
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

	tmp_toolbar_icon = gtk_image_new_from_stock (
			GTK_STOCK_SPELL_CHECK, 
			gtk_toolbar_get_icon_size (GTK_TOOLBAR (ecd->toolbar_edit)));
	ecd->btn_spell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(ecd->toolbar_edit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Spell"),
				       _("Check spelling in this note"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(ecd->btn_spell);

#ifdef USE_SPELL
	gtk_widget_set_sensitive(ecd->btn_spell, 1);
#else
	gtk_widget_set_sensitive(ecd->btn_spell, 0);
#endif

	g_signal_connect(GTK_OBJECT(ecd->btn_save), "clicked",
			   G_CALLBACK(on_btn_save_clicked), (DIALOG_DATA *) d);

	g_signal_connect(GTK_OBJECT(ecd->btn_print), "clicked",
			   G_CALLBACK(on_btn_print_clicked), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(ecd->btn_cut), "clicked",
			   G_CALLBACK(on_btn_cut_clicked), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(ecd->btn_copy), "clicked",
			   G_CALLBACK(on_btn_copy_clicked), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(ecd->btn_paste), "clicked",
			   G_CALLBACK(on_btn_paste_clicked), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(ecd->btn_undo), "clicked",
			   G_CALLBACK(on_btn_undo_clicked), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(ecd->btn_Find), "clicked",
			   G_CALLBACK(on_btn_Find_clicked), (DIALOG_DATA *) d);
	g_signal_connect(GTK_OBJECT(ecd->btn_replace), "clicked",
			   G_CALLBACK(on_btn_replace_clicked),
			   ecd);
#ifdef USE_SPELL
	g_signal_connect(GTK_OBJECT(ecd->btn_spell), "clicked",
			   G_CALLBACK(spell_check_cb), ecd);
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

GtkWidget *gui_note_toolbar_edit(DIALOG_DATA * d)
{
	GSHTMLEditorControlData *ecd = d->editor;
	g_return_val_if_fail(ecd->html != NULL, NULL);
	g_return_val_if_fail(GTK_IS_HTML(ecd->html), NULL);

	return create_toolbar_edit(d);
}
