/*
 * GnomeSword Bible Study Tool
 * editor_menu.c - popup menu for the html editor
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
#include <gtkhtml/gtkhtmlfontstyle.h>
#include <gtkhtml/htmlform.h>
#include <gtkhtml/htmlenums.h>
#include <gtkhtml/htmlsettings.h>
#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmllinktext.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlengine-search.h>
#include <gal/widgets/e-unicode.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "gui/editor.h"
#include "gui/toolbar_style.h"
#include "gui/editor_menu.h"
#include "gui/editor_replace.h"
#include "gui/editor_spell.h"
#include "gui/percomm.h"
#include "gui/fileselection.h"
#include "gui/studypad.h"
#include "gui/html.h"
#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "gui/dialog.h"
#include "gui/find_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/widgets.h"

#include "main/percomm.h"
#include "main/settings.h"
#include "main/xml.h"


/******************************************************************************
 * Name
 *  gui_new_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void gui_new_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    ask to save old file if needed then create new
 *
 * Return value
 *   void
 */

void gui_new_activate(GtkMenuItem * menuitem,
		      GSHTMLEditorControlData * ecd)
{
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		GS_DIALOG *info;
		gint test;

		info = gui_new_dialog();
		info->title = N_("Save File?");
		info->label_top = ecd->filename;
		info->label_middle = N_("has been modified.");
		info->label_bottom = N_("Do you wish to save it?");
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_gs_dialog(info);
		if (test == GS_YES) {
			save_file(ecd->filename, ecd);
		}
		g_free(info);
	}
	settings.studypadfilename = NULL;
	xml_set_value("GnomeSword", "studypad", "lastfile", 
							NULL);
	
	sprintf(ecd->filename, "%s", "");
	ecd->filename[0] = '\0';
	gtk_html_select_all(ecd->html);
	gtk_html_cut(ecd->html);
	gtk_statusbar_push(GTK_STATUSBAR(ecd->statusbar), 1,
			   _("-untitled-"));
	ecd->changed = FALSE;
}

/******************************************************************************
 * Name
 *  on_open_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_open_activate(GtkMenuItem * menuitem,
					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open a file into notepad
 *
 * Return value
 *   void
 */

static void on_open_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	GtkWidget *openFile;
	gchar buf[255];
	/* 
	 * if study pad file has changed let's ask about saving it 
	 */
	if (ecd->changed) {
		GS_DIALOG *info;
		gint test;

		info = gui_new_dialog();
		info->title = N_("Save File?");
		info->label_top = ecd->filename;
		info->label_middle = N_("has been modified.");
		info->label_bottom = N_("Do you wish to save it?");
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_gs_dialog(info);
		if (test == GS_YES) {
			save_file(ecd->filename, ecd);
		}
		g_free(info);
	}
	sprintf(buf, "%s/*.pad", settings.studypaddir);
	openFile = gui_fileselection_open(ecd);
	gtk_file_selection_set_filename(GTK_FILE_SELECTION(openFile),
					buf);
	gtk_widget_show(openFile);
}

/******************************************************************************
 * Name
 *  on_savenote_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_savenote_activate(GtkMenuItem * menuitem,
 *				     GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    save personal commentary note by calling editor_save_note()
 *    or gbs entry by calling savebookEDITOR()
 *
 * Return value
 *   void
 */

static void on_savenote_activate(GtkMenuItem * menuitem,
				 GSHTMLEditorControlData * ecd)
{
	if (ecd->personal_comments) {
		gui_save_note(ecd);
		ecd->changed = FALSE;
		gui_update_statusbar(ecd);
	}
}

/******************************************************************************
 * Name
 *  on_exportnote_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_exportnote_activate(GtkMenuItem * menuitem,
 *				     GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    export personal commentary note by calling ()
 *    
 *
 * Return value
 *   void
 */

static void on_exportnote_activate(GtkMenuItem * menuitem,
				 GSHTMLEditorControlData * ecd)
{
	if (ecd->personal_comments) {
		if(ecd->changed) {
			gui_save_note(ecd);
			ecd->changed = FALSE;
			gui_update_statusbar(ecd);
		}
		gui_fileselection_save(ecd);
	}
}
/******************************************************************************
 * Name
 *  on_deletenote_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_deletenote_activate(GtkMenuItem * menuitem,
 *				       GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    delete personal commentary note by calling delete_percomm_note()
 *
 * Return value
 *   void
 */

static void on_deletenote_activate(GtkMenuItem * menuitem,
				   GSHTMLEditorControlData * ecd)
{
	if (ecd->personal_comments) {
		GS_DIALOG *info;
		gint test;
		//gchar *key;

		//key = get_percomm_key(ecd->filename);
		info = gui_new_dialog();
		info->title = N_("Delete Note?");
		info->label_top = N_("Are you sure you want");
		info->label_middle = N_("to delete the note for");
		info->label_bottom = ecd->key;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_gs_dialog(info);
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
		//g_free(key);
	}
}


/******************************************************************************
 * Name
 *  on_save_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_save_activate(GtkMenuItem * menuitem,
				      GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    if filename call save_file() else open save file dialog
 *
 * Return value
 *   void
 */

void on_save_activate(GtkMenuItem * menuitem,
		      GSHTMLEditorControlData * ecd)
{
	if (ecd->filename) {
		save_file(ecd->filename, ecd);
		return;
	} else {
		gui_fileselection_save(ecd);
	}
}


/******************************************************************************
 * Name
 *  on_save_as_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_save_as_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    call create_fileselectionSave() to save file as
 *
 * Return value
 *   void
 */

static void on_save_as_activate(GtkMenuItem * menuitem,
				GSHTMLEditorControlData * ecd)
{
	gui_fileselection_save(ecd);
}

/******************************************************************************
 * Name
 *  on_print_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_print_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    print content of editor to printer
 *
 * Return value
 *   void
 */

static void on_print_activate(GtkMenuItem * menuitem,
			      GSHTMLEditorControlData * ecd)
{
	gui_html_print(ecd->htmlwidget);	/* gs_html.c */
}

/******************************************************************************
 * Name
 *  on_cut_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_cut_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    cut selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_cut_activate(GtkMenuItem * menuitem,
			    GSHTMLEditorControlData * ecd)
{
	gtk_html_cut(ecd->html);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_copy_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_copy_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    copy selected text to clipboard
 *
 * Return value
 *   void
 */

static void on_copy_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	gtk_html_copy(ecd->html);
}
 
/******************************************************************************
 * Name
 *  on_paste_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_paste_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    paste contents of clipboard into editor
 *
 * Return value
 *   void
 */

static void on_paste_activate(GtkMenuItem * menuitem,
			      GSHTMLEditorControlData * ecd)
{
	gtk_html_paste(ecd->html,FALSE);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_undo_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_undo_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    undo changes to editor
 *
 * Return value
 *   void
 */

static void on_undo_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	gtk_html_undo(ecd->html);
	ecd->changed = TRUE;
	gui_update_statusbar(ecd);
}

/******************************************************************************
 * Name
 *  on_find_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_find_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    open find dialog
 *
 * Return value
 *   void
 */

static void on_find_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	gui_find_dlg(ecd->htmlwidget, ecd->filename,
		  FALSE, NULL);
	//search(ecd, FALSE, NULL);
}


/******************************************************************************
 * Name
 *  on_replace_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_replace_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    opens find and replace dialog
 *
 * Return value
 *   void
 */

static void on_replace_activate(GtkMenuItem * menuitem,
				GSHTMLEditorControlData * ecd)
{
	replace(ecd);
}

/******************************************************************************
 * Name
 *  set_link_to_module
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void set_link_to_module(gchar * linkref, gchar * linkmod,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    set link to module and key
 *
 * Return value
 *   void
 */

static void set_link_to_module(gchar * linkref, gchar * linkmod,
			       GSHTMLEditorControlData * ecd)
{
	const gchar *url;
	const gchar *text;
	gchar *target;
	gchar *url_copy;
	gchar buf[256];
	HTMLEngine *e;
	HTMLObject *new_link;

	e = ecd->html->engine;
	
	if (strlen(linkmod))
		sprintf(buf, "version=%s passage=%s", linkmod, linkref);
	else
		sprintf(buf, "passage=%s", linkref);

	url = buf;
	text = linkref;
	if (url && text && *url && *text) {
		target = strchr(url, '#');
		url_copy =
		    target ? g_strndup(url,
				       target - url) : g_strdup(url);
		new_link =
		    html_link_text_new(text,
				       GTK_HTML_FONT_STYLE_DEFAULT,
				       html_colorset_get_color(e->
							settings->
							color_set,
							HTMLLinkColor),
				       url_copy, target);
		html_engine_paste_object(e, new_link,
					 g_utf8_strlen(text, -1));
		g_free(url_copy);
	}
}

/******************************************************************************
 * Name
 *  on_link_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_link_activate(GtkMenuItem * menuitem, 
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    opens link dialog
 *
 * Return value
 *   void
 */

static void on_link_activate(GtkMenuItem * menuitem,
			     GSHTMLEditorControlData * ecd)
{
	gint test;
	GS_DIALOG *info;

	info = gui_new_dialog();
	info->title = N_("Link?");
	info->label_top = N_("Add Reference Link");
	info->text1 = g_strdup("");
	info->label1 = N_("Reference: ");
	info->text2 = g_strdup("");
	info->label2 = N_("Module: ");
	info->ok = TRUE;
	info->cancel = TRUE;
	/*
	 * get selected text  
	 */
	if (html_engine_is_selection_active(ecd->html->engine)) {
		gchar *buf;
		buf =
		    html_engine_get_selection_string(ecd->html->engine);
		info->text1 = g_strdup(buf);
	}
	info->text2 = g_strdup(xml_get_value("modules", "bible"));//settings.MainWindowModule);
	/*** open dialog to get name for list ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		if (strlen(info->text1) > 0) {
			set_link_to_module(info->text1, info->text2,
					   ecd);
			ecd->changed = TRUE;
			gui_update_statusbar(ecd);
		}
	}
	g_free(info->text1);
	g_free(info->text2);
	g_free(info);
}

/******************************************************************************
 * Name
 *  on_autoscroll_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_autoscroll_activate(GtkMenuItem * menuitem,
				       GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    set personal commentary to follow the Bible text
 *
 * Return value
 *   void
 */
 /*
    static void on_autoscroll_activate(GtkMenuItem * menuitem,
    GSHTMLEditorControlData * ecd)
    {
    settings.notefollow = GTK_CHECK_MENU_ITEM(menuitem)->active;
    }
  */
/******************************************************************************
 * Name
 *  on_editnote_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void on_editnote_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    set personal commentary to edit mode
 *
 * Return value
 *   void
 */

static void on_editnote_activate(GtkMenuItem * menuitem,
				 GSHTMLEditorControlData * ecd)
{
	if (ecd->personal_comments) {
		settings.editnote =
		    GTK_CHECK_MENU_ITEM(menuitem)->active;

		if (GTK_CHECK_MENU_ITEM(menuitem)->active) {
			if (ecd->stylebar)
				gtk_widget_show(ecd->toolbar_style);
			if (ecd->editbar)
				gtk_widget_show(ecd->toolbar_edit);
		}

		else {
			gtk_widget_hide(ecd->toolbar_style);
			gtk_widget_hide(ecd->toolbar_edit);
		}
	}

	if (ecd->gbs) {
		settings.editgbs =
		    GTK_CHECK_MENU_ITEM(menuitem)->active;

		if (GTK_CHECK_MENU_ITEM(menuitem)->active) {
			gtk_widget_show(ecd->toolbar_style);
			gtk_widget_show(ecd->toolbar_edit);
		}

		else {
			gtk_widget_hide(ecd->toolbar_style);
			gtk_widget_hide(ecd->toolbar_edit);
		}
	}

	gtk_widget_set_sensitive(ecd->toolbars, settings.editnote);
	gtk_html_set_editable(GTK_HTML(ecd->html),
			      GTK_CHECK_MENU_ITEM(menuitem)->active);
}


/******************************************************************************
 * Name
 *  show_tabs_activate
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void show_tabs_activate(GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    show/hide percomm notebook tabs
 *
 * Return value
 *   void
 */

static void show_tabs_activate(GtkMenuItem * menuitem,
			       GSHTMLEditorControlData * ecd)
{
	gui_percomm_tabs(GTK_CHECK_MENU_ITEM(menuitem)->active);
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void (GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    show/hide 
 *
 * Return value
 *   void
 */

static void edit_bar_activate(GtkMenuItem * menuitem,
			      GSHTMLEditorControlData * ecd)
{
	ecd->editbar = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (ecd->studypad)
		settings.show_edit_bar_sp = ecd->editbar;
	else {
		save_percomm_options(ecd->filename, "Edit bar",
				     ecd->editbar);
	}
	if (ecd->editbar)
		gtk_widget_show(ecd->toolbar_edit);
	else
		gtk_widget_hide(ecd->toolbar_edit);

}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   void (GtkMenuItem * menuitem,
 *					GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    show/hide 
 *
 * Return value
 *   void
 */

static void style_bar_activate(GtkMenuItem * menuitem,
			       GSHTMLEditorControlData * ecd)
{
	ecd->stylebar = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (ecd->studypad)
		settings.show_style_bar_sp = ecd->stylebar;
	else {
		save_percomm_options(ecd->filename, "Style bar",
				     ecd->stylebar);
	}
	if (ecd->stylebar)
		gtk_widget_show(ecd->toolbar_style);
	else
		gtk_widget_hide(ecd->toolbar_style);

}

/******************************************************************************
 * Name
 *  gui_create_editor_popup
 *
 * Synopsis
 *   #include "editor_menu.h"
 *
 *   GtkWidget *gui_create_editor_popup(GSHTMLEditorControlData * ecd)	
 *
 * Description
 *    create popup menu for editor
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_editor_popup(GSHTMLEditorControlData * ecd)
{
	GtkWidget *pmEditor;
	//GtkAccelGroup *pmEditor_accels;
	guint tmp_key;
	GtkWidget *separator;
	GtkWidget *toolbars_menu;
	//GtkAccelGroup *toolbars_menu_accels;
	GtkWidget *style_toolbar;
	GtkWidget *edit_toolbar;
	GtkWidget *file_menu;
	//GtkAccelGroup *file_menu_accels;
	GtkWidget *save_note = NULL;
	GtkWidget *export_note = NULL;
	GtkWidget *delete_note = NULL;
	GtkWidget *new = NULL;
	GtkWidget *open = NULL;
	GtkWidget *save = NULL;
	GtkWidget *saveas = NULL;
	GtkWidget *print;
	GtkWidget *edit2_menu;
	//GtkAccelGroup *edit2_menu_accels;
	GtkWidget *cut;
	GtkWidget *copy;
	GtkWidget *paste;
	GtkWidget *spell;
	GtkWidget *undo;
	GtkWidget *find;
	GtkWidget *replace;
	//GtkAccelGroup *accel_group;

	//accel_group = gtk_accel_group_new();

	ecd->editnote = NULL;

	pmEditor = gtk_menu_new();
	gtk_object_set_data(GTK_OBJECT(pmEditor), "pmEditor", pmEditor);
/*	pmEditor_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(pmEditor));*/

	ecd->toolbars = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->toolbars)->child),
				  _("Toolbars"));
/*	gtk_widget_add_accelerator(ecd->toolbars, "activate_item",
				   pmEditor_accels, tmp_key, 0, 0);*/
	gtk_widget_show(ecd->toolbars);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->toolbars);
	toolbars_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->toolbars),
				  toolbars_menu);
/*	toolbars_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(toolbars_menu));*/

	style_toolbar =
	    gtk_check_menu_item_new_with_label(_("Style Bar"));
	gtk_widget_show(style_toolbar);
	gtk_container_add(GTK_CONTAINER(toolbars_menu), style_toolbar);

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (style_toolbar), ecd->stylebar);

	edit_toolbar =
	    gtk_check_menu_item_new_with_label(_("Edit Bar"));
	gtk_widget_show(edit_toolbar);
	gtk_container_add(GTK_CONTAINER(toolbars_menu), edit_toolbar);

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (edit_toolbar), ecd->editbar);

	if (ecd->personal_comments) {
		gtk_widget_set_sensitive(ecd->toolbars, FALSE);
		if(!settings.use_percomm_dialog) {
			ecd->editnote =
			    gtk_check_menu_item_new_with_label("Edit Note");
			gtk_widget_show(ecd->editnote);
			gtk_container_add(GTK_CONTAINER(pmEditor),
					  ecd->editnote);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
						       (ecd->editnote), FALSE);
			
			gtk_signal_connect(GTK_OBJECT(ecd->editnote),
				   "activate",
				   G_CALLBACK
				   (on_editnote_activate), ecd);		

			separator = gtk_menu_item_new();
			gtk_widget_show(separator);
			gtk_container_add(GTK_CONTAINER(pmEditor), separator);
			gtk_widget_set_sensitive(separator, FALSE);
	
			
			ecd->show_tabs =
			    gtk_check_menu_item_new_with_label("Show Tabs");
			gtk_widget_show(ecd->show_tabs);
			gtk_container_add(GTK_CONTAINER(pmEditor),
					  ecd->show_tabs);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
						       (ecd->show_tabs), FALSE);
		}

		separator = gtk_menu_item_new();
		gtk_widget_show(separator);
		gtk_container_add(GTK_CONTAINER(pmEditor), separator);
		gtk_widget_set_sensitive(separator, FALSE);
	}

	if (ecd->gbs) {

		ecd->editnote =
		    gtk_check_menu_item_new_with_label("Edit");
		gtk_widget_show(ecd->editnote);
		gtk_container_add(GTK_CONTAINER(pmEditor),
				  ecd->editnote);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
					       (ecd->editnote), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(ecd->editnote),
					 FALSE);

		separator = gtk_menu_item_new();
		gtk_widget_show(separator);
		gtk_container_add(GTK_CONTAINER(pmEditor), separator);
		gtk_widget_set_sensitive(separator, FALSE);
	}


	if (ecd->gbs)
		gtk_widget_set_sensitive(GTK_WIDGET(ecd->file), FALSE);


	ecd->file = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->file)->child),
				  _("File"));
	/*gtk_widget_add_accelerator(ecd->file, "activate_item",
				   pmEditor_accels, tmp_key, 0, 0);*/
	gtk_widget_show(ecd->file);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->file);

	file_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->file), file_menu);

/*	file_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(file_menu));*/


	if (ecd->personal_comments) {
		save_note =
		    gtk_menu_item_new_with_label(_("Save Note"));
		gtk_widget_show(save_note);
		gtk_container_add(GTK_CONTAINER(file_menu), save_note);
		
		export_note =
		    gtk_menu_item_new_with_label(_("Export Note"));
		gtk_widget_show(export_note);
		gtk_container_add(GTK_CONTAINER(file_menu), export_note);

		delete_note =
		    gtk_menu_item_new_with_label(_("Delete Note"));
		gtk_widget_show(delete_note);
		gtk_container_add(GTK_CONTAINER(file_menu),
				  delete_note);
	}

	else if (ecd->gbs) {

		open = gtk_menu_item_new_with_label(_("Open File"));
		gtk_widget_show(open);
		gtk_container_add(GTK_CONTAINER(file_menu), open);

		save_note =
		    gtk_menu_item_new_with_label(_("Save Entry"));
		gtk_widget_show(save_note);
		gtk_container_add(GTK_CONTAINER(file_menu), save_note);

		delete_note =
		    gtk_menu_item_new_with_label(_("Delete Entry"));
		gtk_widget_show(delete_note);
		gtk_container_add(GTK_CONTAINER(file_menu),
				  delete_note);
	}

	else {
		new = gtk_menu_item_new_with_label(_("New"));
		gtk_widget_show(new);
		gtk_container_add(GTK_CONTAINER(file_menu), new);

		open = gtk_menu_item_new_with_label(_("Open"));
		gtk_widget_show(open);
		gtk_container_add(GTK_CONTAINER(file_menu), open);

		save = gtk_menu_item_new_with_label(_("Save"));
		gtk_widget_show(save);
		gtk_container_add(GTK_CONTAINER(file_menu), save);


		saveas = gtk_menu_item_new_with_label(_("Save AS"));
		gtk_widget_show(saveas);
		gtk_container_add(GTK_CONTAINER(file_menu), saveas);
	}

	print = gtk_menu_item_new_with_label(_("Print"));
	gtk_widget_show(print);
	gtk_container_add(GTK_CONTAINER(file_menu), print);


	ecd->edit2 = gtk_menu_item_new_with_label("");
	tmp_key =
	    gtk_label_parse_uline(GTK_LABEL(GTK_BIN(ecd->edit2)->child),
				  _("_Edit"));
	/*gtk_widget_add_accelerator(ecd->edit2, "activate_item",
				   pmEditor_accels, tmp_key, 0, 0);*/
	gtk_widget_show(ecd->edit2);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->edit2);

	if (ecd->gbs)
		gtk_widget_set_sensitive(GTK_WIDGET(ecd->edit2), FALSE);

	edit2_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ecd->edit2),
				  edit2_menu);
/*	edit2_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU(edit2_menu));*/

	cut = gtk_menu_item_new_with_label(_("Cut"));
	gtk_widget_show(cut);
	gtk_container_add(GTK_CONTAINER(edit2_menu), cut);

	copy = gtk_menu_item_new_with_label(_("Copy"));
	gtk_widget_show(copy);
	gtk_container_add(GTK_CONTAINER(edit2_menu), copy);

	paste = gtk_menu_item_new_with_label(_("Paste"));
	gtk_widget_show(paste);
	gtk_container_add(GTK_CONTAINER(edit2_menu), paste);

	spell = gtk_menu_item_new_with_label(_("Spell Check"));
	gtk_widget_show(spell);
	gtk_container_add(GTK_CONTAINER(edit2_menu), spell);

#ifdef USE_SPELL
	gtk_widget_set_sensitive(spell, 1);
#else
	gtk_widget_set_sensitive(spell, 0);
#endif


	undo = gtk_menu_item_new_with_label(_("Undo"));
	gtk_widget_show(undo);
	gtk_container_add(GTK_CONTAINER(edit2_menu), undo);

	find = gtk_menu_item_new_with_label(_("Find"));
	gtk_widget_show(find);
	gtk_container_add(GTK_CONTAINER(edit2_menu), find);


	replace = gtk_menu_item_new_with_label(_("Replace"));
	gtk_widget_show(replace);
	gtk_container_add(GTK_CONTAINER(edit2_menu), replace);

	ecd->link = gtk_menu_item_new_with_label(_("Link..."));
	gtk_widget_show(ecd->link);
	gtk_container_add(GTK_CONTAINER(pmEditor), ecd->link);

	if (ecd->gbs)
		gtk_widget_set_sensitive(GTK_WIDGET(ecd->link), FALSE);

	if (ecd->personal_comments) {
		gtk_signal_connect(GTK_OBJECT(save_note), "activate",
				   G_CALLBACK
				   (on_savenote_activate), ecd);
		gtk_signal_connect(GTK_OBJECT(export_note), "activate",
				   G_CALLBACK
				   (on_exportnote_activate), ecd);
		gtk_signal_connect(GTK_OBJECT(delete_note), "activate",
				   G_CALLBACK
				   (on_deletenote_activate), ecd);
		if(!settings.use_percomm_dialog)
			gtk_signal_connect(GTK_OBJECT(ecd->show_tabs),
				   "activate",
				   G_CALLBACK
				   (show_tabs_activate), ecd);
	} else if (ecd->gbs) {
		gtk_signal_connect(GTK_OBJECT(save_note), "activate",
				   G_CALLBACK
				   (on_savenote_activate), ecd);
		gtk_signal_connect(GTK_OBJECT(delete_note), "activate",
				   G_CALLBACK
				   (on_deletenote_activate), ecd);
		gtk_signal_connect(GTK_OBJECT(ecd->editnote),
				   "activate",
				   G_CALLBACK
				   (on_editnote_activate), ecd);
		gtk_signal_connect(GTK_OBJECT(open), "activate",
				   G_CALLBACK(on_open_activate),
				   ecd);
	} else {
		gtk_signal_connect(GTK_OBJECT(new), "activate",
				   G_CALLBACK(gui_new_activate),
				   ecd);
		gtk_signal_connect(GTK_OBJECT(open), "activate",
				   G_CALLBACK(on_open_activate),
				   ecd);
		gtk_signal_connect(GTK_OBJECT(save), "activate",
				   G_CALLBACK(on_save_activate),
				   ecd);
		gtk_signal_connect(GTK_OBJECT(saveas), "activate",
				   G_CALLBACK(on_save_as_activate),
				   ecd);
	}
	gtk_signal_connect(GTK_OBJECT(print), "activate",
			   G_CALLBACK(on_print_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(cut), "activate",
			   G_CALLBACK(on_cut_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(copy), "activate",
			   G_CALLBACK(on_copy_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(paste), "activate",
			   G_CALLBACK(on_paste_activate), ecd);

#ifdef USE_SPELL
	gtk_signal_connect(GTK_OBJECT(spell), "activate",
			   G_CALLBACK(spell_check_cb), ecd);
#endif				/* USE_SPELL */

	gtk_signal_connect(GTK_OBJECT(style_toolbar),
			   "activate",
			   G_CALLBACK(style_bar_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(edit_toolbar),
			   "activate",
			   G_CALLBACK(edit_bar_activate), ecd);

	gtk_signal_connect(GTK_OBJECT(undo), "activate",
			   G_CALLBACK(on_undo_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(find), "activate",
			   G_CALLBACK(on_find_activate), ecd);
	gtk_signal_connect(GTK_OBJECT(replace), "activate",
			   G_CALLBACK(on_replace_activate), ecd);

	gtk_signal_connect(GTK_OBJECT(ecd->link), "activate",
			   G_CALLBACK(on_link_activate), ecd);
	//gtk_menu_set_accel_group(GTK_MENU(pmEditor), accel_group);
	return pmEditor;
}



/******   end of file   ******/
