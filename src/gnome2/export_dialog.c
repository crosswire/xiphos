/*
 * Xiphos Bible Study Tool
 * export_dialog.c -
 *
 * Copyright (C) 2008-2017 Xiphos Developer Team
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
#include <glib/gstdio.h>

#include "gui/export_dialog.h"
#include "gui/dialog.h"
#include "gui/utilities.h"

#include "main/export_passage.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "gui/debug_glib_null.h"

gchar *datafile;

static GtkWidget *dialog;
static gchar *filename;

typedef struct _export_dialog EXPORT_DIALOG;
struct _export_dialog
{
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
	GtkWidget *cb_reference_last;
	GtkWidget *cb_version;
	gint format;
	gint start_verse;
	gint end_verse;
	gint passage_type;
};

EXPORT_DIALOG d;
EXPORT_DATA edata = {
    NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL,
    -1, -1, -1, -1, -1, -1};

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

static void on_filechooserdialog_response(GtkDialog *fdialog,
					  gint response_id,
					  GtkFileChooser *filesel)
{

	switch (response_id) {
	case GTK_RESPONSE_ACCEPT:
		edata.filename =
		    g_strdup(gtk_file_chooser_get_filename(filesel));

		main_export_content(edata, d.format);
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
	glade_file =
	    gui_general_user_file("export-dialog" UI_SUFFIX, FALSE);
	g_return_if_fail(glade_file != NULL);
	XI_message(("%s", glade_file));
#endif

/* build the widget */
#ifdef USE_GTKBUILDER
	fdialog = gtk_file_chooser_dialog_new("Save Export File",
					      NULL,
					      GTK_FILE_CHOOSER_ACTION_SAVE,
#if GTK_CHECK_VERSION(3, 10, 0)
					      "_Cancel",
					      GTK_RESPONSE_CANCEL, "_Save",
					      GTK_RESPONSE_ACCEPT,
#else
					      GTK_STOCK_CANCEL,
					      GTK_RESPONSE_CANCEL,
					      GTK_STOCK_SAVE,
					      GTK_RESPONSE_ACCEPT,
#endif
					      NULL);
#else
	gxml = glade_xml_new(glade_file, "filechooserdialog1", NULL);

	fdialog = glade_xml_get_widget(gxml, "filechooserdialog1");
#endif
	g_signal_connect(fdialog,
			 "response",
			 G_CALLBACK(on_filechooserdialog_response),
			 (GtkFileChooser *)fdialog);

#ifdef USE_GTKBUILDER
	gtk_dialog_run(GTK_DIALOG(fdialog));
#endif
}

static void _save_state_buttons(void)
{
	gchar value[10]; /* set export-copy.xml */
	gchar *file =
	    g_strdup_printf("%s/export-copy.xml", settings.gSwordDir);

	if (xml_load_copy_export_file((const xmlChar *)file)) {
		sprintf(value, "%d",
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_plain)));
		xml_export_set_value("Copy_Export", "dialog", "plaintext",
				     value);
		sprintf(value, "%d",
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.cb_versenum)));
		xml_export_set_value("Copy_Export", "dialog",
				     "verse_numbers", value);
		sprintf(value, "%d",
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.cb_reference_last)));
		xml_export_set_value("Copy_Export", "dialog",
				     "reference_last", value);
		//sprintf(value, "%d", gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.)));
		//xml_export_set_value("Copy_Export", "dialog", "",value );
		sprintf(value, "%d",
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.cb_version)));
		xml_export_set_value("Copy_Export", "dialog", "version",
				     value);
		xml_save_export_doc(file);
		xml_free_export_doc();
	}
	if (file)
		g_free(file);
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

void on_dialog_export_passage_response(GtkDialog *dialog,
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
		    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_book)) ? BOOK : gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_chapter)) ? CHAPTER
																		      : gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_verse)) ? VERSE : VERSE_RANGE;

		edata.verse_num =
		    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.cb_versenum));
		edata.reference_last =
		    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.cb_reference_last));

		if (edata.passage_type == VERSE_RANGE) {
			edata.start_verse =
			    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(d.sb_start_verse));
			edata.end_verse =
			    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(d.sb_end_verse));
			if (edata.end_verse < edata.start_verse) {
				int swap;
				swap = edata.end_verse;
				edata.end_verse = edata.start_verse;
				edata.start_verse = swap;
			}
		} else {
			edata.start_verse = 0;
			edata.end_verse = 0;
		}
		_save_state_buttons();
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(d.rb_export)))
			_get_export_filename();
		else {
			edata.filename = NULL;
			main_export_content(edata, d.format);
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

#define OP_NAME "Warned about License for Export"

gint _check_for_distribution_license(gchar *mod_name)
{
	gchar *distributionlicense;
	gchar *conf_file;

	conf_file = g_strdup_printf("%s/" DOTSWORD "/mods.d/%s.conf",
				    settings.homedir,
				    g_utf8_strdown(mod_name, -1));

	if (g_file_test(conf_file, G_FILE_TEST_EXISTS))
		distributionlicense = get_conf_file_item(conf_file,
							 mod_name,
							 "DistributionLicense");
	else
		distributionlicense = main_get_mod_config_entry(mod_name,
								"DistributionLicense");
	g_free(conf_file);
	XI_message(("DistributionLicense: %s", distributionlicense));

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

static void on_rb_multi_verse_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data)
{
	gint state = gtk_toggle_button_get_active(togglebutton);
	gtk_widget_set_sensitive(d.sb_start_verse, state);
	gtk_widget_set_sensitive(d.sb_end_verse, state);
}

static void on_cb_version_toggled(GtkToggleButton *togglebutton,
				  gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton))
		edata.version = TRUE;
	else
		edata.version = FALSE;
}

static void on_reference_last_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton))
		edata.reference_last = TRUE;
	else
		edata.reference_last = FALSE;
}

/******************************************************************************
 * Name
 *   _load_data
 *
 * Synopsis
 *   #include "gui/export_dialog.h"
 *
 *   void  _load_data(gchar * filename)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void _load_data(gchar *filename)
{
	gchar *file =
	    g_strdup_printf("%s/export-copy.xml", settings.gSwordDir);

	if (xml_load_copy_export_file((const xmlChar *)filename)) {
		gchar *buf;

		/* html */

		edata.bookheader =
		    xml_get_copy_export_value("book", "header");
		edata.chapterheader_book =
		    xml_get_copy_export_value("book", "chapterheader");
		edata.versenumber =
		    xml_get_copy_export_value("versenumber", "format");
		edata.chapterheader_chapter =
		    xml_get_copy_export_value("chapter", "header");
		edata.verselayout_single_verse_ref_last =
		    xml_get_copy_export_value("singleverse", "last");
		edata.verselayout_single_verse_ref_first =
		    xml_get_copy_export_value("singleverse", "first");
		edata.verse_range_ref_first =
		    xml_get_copy_export_value("verserange", "first");
		edata.verse_range_ref_last =
		    xml_get_copy_export_value("verserange", "last");
		edata.verse_range_verse =
		    xml_get_copy_export_value("verserange", "verse");

		if (edata.verse_range_verse == NULL) {
			xml_add_new_item_to_export_doc_section("verserange", "verse", " %s%s");
			edata.verse_range_verse =
			    xml_get_copy_export_value("verserange",
						      "verse");
			xml_save_export_doc(file);
		}

		/* plain text */

		edata.plain_bookheader =
		    xml_get_copy_export_value("book", "plain_header");
		edata.plain_chapterheader_book =
		    xml_get_copy_export_value("book",
					      "plain_chapterheader");
		edata.plain_versenumber =
		    xml_get_copy_export_value("versenumber",
					      "plain_format");
		edata.plain_chapterheader_chapter =
		    xml_get_copy_export_value("chapter", "plain_header");
		edata.plain_verselayout_single_verse_ref_last =
		    xml_get_copy_export_value("singleverse", "plain_last");
		edata.plain_verselayout_single_verse_ref_first =
		    xml_get_copy_export_value("singleverse",
					      "plain_first");
		edata.plain_verse_range_ref_last =
		    xml_get_copy_export_value("verserange", "plain_last");
		edata.plain_verse_range_ref_first =
		    xml_get_copy_export_value("verserange", "plain_first");

		if ((buf =
			 xml_get_copy_export_value("dialog", "plaintext"))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.rb_plain),
						     atoi(buf));
			g_free(buf);
		} else {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.rb_plain), TRUE);
		}

		if ((buf =
			 xml_get_copy_export_value("dialog",
						   "verse_numbers"))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.cb_versenum),
						     atoi(buf));
			g_free(buf);
		} else {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.cb_versenum),
						     TRUE);
		}

		if ((buf =
			 xml_get_copy_export_value("dialog",
						   "reference_last"))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.cb_reference_last),
						     atoi(buf));
			g_free(buf);
		} else {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.cb_reference_last),
						     TRUE);
		}

		if ((buf = xml_get_copy_export_value("dialog", "version"))) {
			edata.version = atoi(buf);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.cb_version),
						     edata.version);
			g_free(buf);
		} else {
			edata.version = 0;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.cb_version), TRUE);
		}

		xml_free_export_doc();
		if (file)
			g_free(file);
	}
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

	gchar *glade_file =
	    gui_general_user_file("export-dialog" UI_SUFFIX, FALSE);
	g_return_if_fail(glade_file != NULL);
	XI_message(("%s", glade_file));

	dist_license =
	    _check_for_distribution_license(settings.MainWindowModule);

/* build the widget */
#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
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
	d.cb_reference_last = UI_GET_ITEM(gxml, "check_reference_last");
	d.cb_version = UI_GET_ITEM(gxml, "check_include_version");

	/* set export-copy.xml */
	datafile =
	    g_strdup_printf("%s/export-copy.xml", settings.gSwordDir);
/* check for datafile */
#if GTK_CHECK_VERSION(2, 24, 0)
	if (g_access(datafile, F_OK) == -1) {
#else
	if (g_access(datafile, 0) == -1) {
#endif
		xml_create_copy_export_file(datafile);
	}
	_load_data(datafile);
	if (datafile)
		g_free(datafile);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.rb_copy), TRUE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(d.rb_multi_verse),
				     TRUE);

	g_signal_connect(dialog, "response",
			 G_CALLBACK(on_dialog_export_passage_response),
			 NULL);

	max = main_get_max_verses(settings.MainWindowModule);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(d.sb_start_verse),
				  1, max);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(d.sb_end_verse), 1, max);
	curVerse = main_get_current_verse(settings.MainWindowModule);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(d.sb_start_verse),
				  curVerse);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(d.sb_end_verse),
				  curVerse);
	gtk_widget_set_sensitive(d.sb_start_verse, FALSE);
	gtk_widget_set_sensitive(d.sb_end_verse, FALSE);

	/* experiment: hide single verse option; subsumed into multi-verse. */
	gtk_widget_hide(d.rb_verse);
	gtk_widget_set_sensitive(d.sb_start_verse, TRUE);
	gtk_widget_set_sensitive(d.sb_end_verse, TRUE);
	/* end experiment. */

	if (dist_license) {
		gtk_widget_show(d.warning_label);
		gtk_widget_show(d.rb_book);	/* formerly hide */
	} else {
		gtk_widget_hide(d.warning_label);
		gtk_widget_show(d.rb_book);
	}

	gtk_label_set_text(GTK_LABEL(d.lb_version),
			   settings.MainWindowModule);
	ref = (char *)main_getText(settings.currentverse);
	gtk_label_set_text(GTK_LABEL(d.lb_key), ref);
	g_free(ref);

	g_signal_connect((gpointer)d.rb_multi_verse, "toggled",
			 G_CALLBACK(on_rb_multi_verse_toggled), NULL);
	g_signal_connect((gpointer)d.cb_version, "toggled",
			 G_CALLBACK(on_cb_version_toggled), NULL);
	g_signal_connect((gpointer)d.cb_reference_last, "toggled",
			 G_CALLBACK(on_reference_last_toggled), NULL);
}
