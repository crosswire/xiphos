/*
 * Xiphos Bible Study Tool
 * export_dialog.c -
 *
 * Copyright (C) 2008-2011 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#ifndef USE_GTKBUILDER
  #include <glade/glade-xml.h>
#endif

#include "gui/export_dialog.h"
#include "gui/dialog.h"
#include "gui/utilities.h"


#include "main/export_passage.h"
#include "main/settings.h"
#include "main/sword.h"

#include "gui/debug_glib_null.h"


static GtkWidget *dialog;
static gchar *filename;


typedef struct _export_dialog EXPORT_DIALOG;
struct _export_dialog {
	GtkWidget *rb_copy;
	GtkWidget *rb_export;
	GtkWidget *rb_book;
	GtkWidget *rb_chapter;
	GtkWidget *rb_verse;
	GtkWidget *rb_multi_verse;
	GtkWidget *cb_versenum;
	GtkWidget *rb_html;
	GtkWidget *rb_plain;
	GtkWidget *lb_version;
	GtkWidget *lb_key;
	GtkWidget *sb_start_verse;
	GtkWidget *sb_end_verse;
	GtkWidget *warning_label;
	gint format;
	gint start_verse;
	gint end_verse;
	gint passage_type;
};

EXPORT_DIALOG d;
EXPORT_DATA edata;

/******************************************************************************
 * Name
 *   on_filechooserdialog_response
 *
 * Synopsis
 *   #include "gui/export_dialog.h"
 *
 *   void on_filechooserdialog_response(GtkDialog * fdialog,
 *					    gint response_id,
 *					    GtkFileChooser * filesel)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static
void on_filechooserdialog_response(GtkDialog * fdialog,
					    gint response_id,
					    GtkFileChooser * filesel)
{

	switch (response_id) {
	case GTK_RESPONSE_ACCEPT:
		edata.filename = g_strdup(gtk_file_chooser_get_filename(filesel));

		if (d.format)
			main_export_html(edata);
		else
			main_export_plain(edata);
		break;

	case GTK_RESPONSE_CANCEL:
		break;

	}
	gtk_widget_destroy(GTK_WIDGET(fdialog));
}


/******************************************************************************
 * Name
 *   _get_export_filename
 *
 * Synopsis
 *   #include "gui/export_dialog.h"
 *
 *   void _get_export_filename(void)
 *
 * Description
 *   open file save dialog using libglade
 *
 * Return value
 *   void
 */

void _get_export_filename(void)
{
#ifndef USE_GTKBUILDER
	gchar *glade_file;
	GladeXML *gxml;
#endif
	GtkWidget *fdialog;
	filename = NULL;

#ifndef USE_GTKBUILDER
	glade_file = gui_general_user_file("export-dialog" UI_SUFFIX, FALSE);
	g_return_if_fail(glade_file != NULL);
	GS_message(("%s",glade_file));
#endif

	/* build the widget */
#ifdef USE_GTKBUILDER
	fdialog = gtk_file_chooser_dialog_new ("Save Export File",
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);	
#else
	gxml = glade_xml_new(glade_file, "filechooserdialog1", NULL);

	fdialog =  glade_xml_get_widget (gxml, "filechooserdialog1");
#endif
	g_signal_connect(fdialog,
			 "response",
			 G_CALLBACK(on_filechooserdialog_response),
			 (GtkFileChooser *)fdialog);

#ifdef USE_GTKBUILDER
	gtk_dialog_run (GTK_DIALOG (fdialog));
#endif
}

/******************************************************************************
 * Name
 *   on_dialog_export_passage_response
 *
 * Synopsis
 *   #include "gui/export_dialog.h"
 *
 *   void on_dialog_export_passage_response(GtkDialog * dialog,
 *						gint response_id,
 *						gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void on_dialog_export_passage_response(GtkDialog * dialog,
			 		gint response_id,
			 		gpointer user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
		gtk_widget_destroy(GTK_WIDGET(dialog));
		break;
	case GTK_RESPONSE_OK:
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_html)))
			d.format = 1;
		else
			d.format = 0;

		edata.passage_type =
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_book)) ? BOOK
			 : gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_chapter)) ? CHAPTER
			 : gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_verse)) ? VERSE
			 : VERSE_RANGE;

		edata.verse_num = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.cb_versenum));

		if (edata.passage_type == VERSE_RANGE) {
			edata.start_verse =
				gtk_spin_button_get_value_as_int (
				    GTK_SPIN_BUTTON (d.sb_start_verse));
			edata.end_verse =
				gtk_spin_button_get_value_as_int (
				    GTK_SPIN_BUTTON (d.sb_end_verse));
			if (edata.end_verse < edata.start_verse) {
				int swap;
				swap = edata.end_verse;
				edata.end_verse = edata.start_verse;
				edata.start_verse = swap;
			}
		} else {
			edata.start_verse = 0 ;
			edata.end_verse = 0;
		}

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_export)))
			_get_export_filename();
		else {
			edata.filename = NULL;
			if (d.format)
				main_export_html(edata);
			else
				main_export_plain(edata);
		}

		gtk_widget_destroy(GTK_WIDGET(dialog));
		break;
	}
}


/******************************************************************************
 * Name
 *   _check_for_distribution_license
 *
 * Synopsis
 *   #include "gui/export_dialog.h"
 *
 *  gint _check_for_distribution_license(gchar * mod_name)
 *
 * Description
 *   checks the module's conf file for DistributionLicense entry - if the
 *   entry does not exist or contains the word 'copyright' a warning
 *   dialog is called - this needs lots of work --FIXME--
 *
 * Return value
 *   gint
 */

#define	OP_NAME	"Warned about License for Export"

gint _check_for_distribution_license(gchar * mod_name)
{
	gchar *distributionlicense;
	gchar *conf_file;

	conf_file = g_strdup_printf("%s/" DOTSWORD "/mods.d/%s.conf",
				    settings.homedir,
				    g_utf8_strdown(mod_name,
                                    -1));

	if (g_file_test(conf_file,G_FILE_TEST_EXISTS))
		distributionlicense = get_conf_file_item(
							 conf_file,
							 mod_name,
							 "DistributionLicense");
	else
		distributionlicense = main_get_mod_config_entry(
						mod_name,
				 		"DistributionLicense");
	g_free(conf_file);
	GS_message(("DistributionLicense: %s",distributionlicense));

	if (!distributionlicense || (distributionlicense &&
				          g_strstr_len(distributionlicense,
					  strlen(distributionlicense),
					  "Copyright"))) {
		if (main_get_one_option(mod_name, OP_NAME) == 0) {
			gui_generic_warning(_("Please check copyright before exporting!"));
			main_save_module_options(mod_name, OP_NAME, 1);
		}
		
		return 1;
	}

	return 0;
}


static
void on_rb_multi_verse_toggled (GtkToggleButton *togglebutton,
                                gpointer user_data)
{
	gint state = gtk_toggle_button_get_active(togglebutton);
	gtk_widget_set_sensitive(d.sb_start_verse, state);
	gtk_widget_set_sensitive(d.sb_end_verse, state);
}


/******************************************************************************
 * Name
 *   gui_export_dialog
 *
 * Synopsis
 *   #include "gui/export_dialog.h"
 *
 *   void gui_export_dialog(void)
 *
 * Description
 *   create a new passage export dialog using libglade
 *
 * Return value
 *   void
 */

void gui_export_dialog(void)
{
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
#else
	GladeXML *gxml;
#endif
	gint dist_license, curVerse;
	gdouble max;
	char *ref;

	gchar *glade_file = gui_general_user_file("export-dialog" UI_SUFFIX, FALSE);
	g_return_if_fail(glade_file != NULL);
	GS_message(("%s",glade_file));

	dist_license = _check_for_distribution_license(settings.MainWindowModule);

	/* build the widget */
#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new ();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, "dialog_export_passage", NULL);
#endif
	
	dialog = UI_GET_ITEM(gxml, "dialog_export_passage");

	d.rb_book = UI_GET_ITEM(gxml, "radiobutton1");
	d.rb_chapter = UI_GET_ITEM(gxml, "radiobutton2");
	d.rb_verse = UI_GET_ITEM(gxml, "radiobutton3");
	d.rb_multi_verse = UI_GET_ITEM(gxml, "rb_multi_verse");
	d.cb_versenum = UI_GET_ITEM(gxml, "check_versenum");
	d.rb_html = UI_GET_ITEM(gxml, "radiobutton4");
	d.rb_plain = UI_GET_ITEM(gxml, "radiobutton5");
	d.rb_copy = UI_GET_ITEM(gxml, "rb_copy");
	d.rb_export = UI_GET_ITEM(gxml, "rb_export");
	d.lb_version = UI_GET_ITEM(gxml, "label3");
	d.lb_key = UI_GET_ITEM(gxml, "label4");
	d.sb_start_verse = UI_GET_ITEM(gxml, "sb_start_verse");
	d.sb_end_verse = UI_GET_ITEM(gxml, "sb_end_verse");
	d.warning_label = UI_GET_ITEM(gxml, "hbox2");

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.rb_copy), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.rb_multi_verse), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.rb_plain), TRUE);

	g_signal_connect(dialog, "response",
			 G_CALLBACK(on_dialog_export_passage_response), NULL);

	max = main_get_max_verses ();
	gtk_spin_button_set_range (GTK_SPIN_BUTTON (d.sb_start_verse),
				   1, max);
	gtk_spin_button_set_range (GTK_SPIN_BUTTON (d.sb_end_verse),
				   1, max);
	curVerse = main_get_current_verse();
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (d.sb_start_verse),
				   curVerse);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (d.sb_end_verse),
				   curVerse);
	gtk_widget_set_sensitive (d.sb_start_verse, FALSE);
	gtk_widget_set_sensitive (d.sb_end_verse, FALSE);

	/* experiment: hide single verse option; subsumed into multi-verse. */
	gtk_widget_hide(d.rb_verse);
	gtk_widget_set_sensitive(d.sb_start_verse, TRUE);
	gtk_widget_set_sensitive(d.sb_end_verse, TRUE);
	/* end experiment. */

	if (dist_license) {
		gtk_widget_show(d.warning_label);
		gtk_widget_hide(d.rb_book);
	} else {
		gtk_widget_hide(d.warning_label);
		gtk_widget_show(d.rb_book);
	}

	gtk_label_set_text (GTK_LABEL(d.lb_version), settings.MainWindowModule);
	ref = (char *)main_getText(settings.currentverse);
	gtk_label_set_text (GTK_LABEL(d.lb_key), ref);
	g_free(ref);

	g_signal_connect ((gpointer) d.rb_multi_verse, "toggled",
		    G_CALLBACK (on_rb_multi_verse_toggled),
		    NULL);
}
