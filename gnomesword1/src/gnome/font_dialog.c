/*
 * GnomeSword Bible Study Tool
 * font_dialog.c - dialog to set module font
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

#include "gui/font_dialog.h"

#include "main/sword.h"

static GtkWidget *dlg;
static GtkWidget *combo_entry_size;
static GtkWidget *fontpicker;
static GtkWidget *checkbutton_no_font;
static GtkWidget *label_mod;
static GtkWidget *label_current_font;
static GtkWidget *button_ok;
static gboolean new_font_set = FALSE;


/******************************************************************************
 * Name
 *   get_html_font_name
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   gchar *get_html_font_name(gchar *fontname)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static gchar *get_html_font_name(gchar *fontname)
{
	gchar *token, *retval;
	++fontname;
	token = strtok(fontname,"-");
	token = strtok(NULL,"-");
	retval = token;
	return retval;
}

/******************************************************************************
 * Name
 *   ok_clicked
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   void ok_clicked(GtkButton * button, MOD_FONT * mf)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void ok_clicked(GtkButton * button, MOD_FONT * mf)
{
	if (!mf->no_font && new_font_set) {
		gchar *new_font = g_strdup(mf->new_gdk_font);
		mf->new_font = g_strdup(get_html_font_name(new_font));
		g_free(new_font);
	} else {
		mf->new_font = g_strdup("none");
		mf->new_gdk_font = "none";
	}
	mf->new_font_size =
	    gtk_entry_get_text(GTK_ENTRY(combo_entry_size));
	save_font_info(mf);
	
	gtk_widget_destroy(dlg);
	g_free(mf->new_font);
}


/******************************************************************************
 * Name
 *   cancel_clicked
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   oid cancel_clicked(GtkButton * button, MOD_FONT * mf)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void cancel_clicked(GtkButton * button, MOD_FONT * mf)
{
	gtk_widget_destroy(dlg);
}


/******************************************************************************
 * Name
 *   dialog_de"gui/stroy
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   void dialog_destroy(GtkObject * object, MOD_FONT * mf)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void dialog_destroy(GtkObject * object, MOD_FONT * mf)
{
	g_free(mf);
	new_font_set = 0;
	gtk_main_quit();
	
}


/******************************************************************************
 * Name
 *   font_set
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   void font_set(GnomeFontPicker * gnomefontpicker,
 *		     gchar * arg1, MOD_FONT * mf)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void font_set(GnomeFontPicker * gnomefontpicker,
		     gchar * arg1, MOD_FONT * mf)
{
	mf->new_gdk_font =
	    gnome_font_picker_get_font_name((GnomeFontPicker *)
					    fontpicker);
	new_font_set = 1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (checkbutton_no_font), FALSE);
	gtk_widget_set_sensitive(button_ok, TRUE);
}


/******************************************************************************
 * Name
 *   no_font_toggled
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   void no_font_toggled(GtkToggleButton * togglebutton,
 *			    MOD_FONT * mf)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void no_font_toggled(GtkToggleButton * togglebutton,
			    MOD_FONT * mf)
{
	mf->no_font = togglebutton->active;
	gtk_widget_set_sensitive(button_ok, TRUE);
}


/******************************************************************************
 * Name
 *   size_changed
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   void size_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void size_changed(GtkEditable * editable, gpointer user_data)
{
	gtk_widget_set_sensitive(button_ok, TRUE);
}


/******************************************************************************
 * Name
 *   create_dialog_mod_font
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   GtkWidget *create_dialog_mod_font(MOD_FONT * mf)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_dialog_mod_font(MOD_FONT * mf)
{
	GtkWidget *dialog_mod_font;
	GtkWidget *dialog_vbox21;
	GtkWidget *hbox_picker;
	GtkWidget *combo_size;
	GtkWidget *frame44;
	GtkWidget *vbox56;
	GtkWidget *hbox67;
	GtkWidget *pixmap6;
	GtkWidget *vbox57;
	GtkWidget *label206;
	GtkWidget *dialog_action_area21;
	GtkWidget *hbuttonbox1;
	GtkWidget *button_cancel;
	GList *combo_size_items = NULL;

	dialog_mod_font = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog_mod_font),
			    "dialog_mod_font", dialog_mod_font);
	gtk_window_set_title(GTK_WINDOW(dialog_mod_font),
			     _("Set Module Font - GnomeSword"));
	GTK_WINDOW(dialog_mod_font)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_default_size(GTK_WINDOW(dialog_mod_font), 301,
				    164);
	gtk_window_set_policy(GTK_WINDOW(dialog_mod_font), TRUE, TRUE,
			      TRUE);

	dialog_vbox21 = GTK_DIALOG(dialog_mod_font)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_mod_font),
			    "dialog_vbox21", dialog_vbox21);
	gtk_widget_show(dialog_vbox21);

	frame44 = gtk_frame_new(NULL);
	gtk_widget_show(frame44);
	gtk_box_pack_start(GTK_BOX(dialog_vbox21), frame44, TRUE, TRUE,
			   0);
	gtk_widget_set_usize(frame44, -2, 53);

	vbox56 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox56);
	gtk_container_add(GTK_CONTAINER(frame44), vbox56);

	hbox67 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox67);
	gtk_box_pack_start(GTK_BOX(vbox56), hbox67, TRUE, TRUE, 0);

	pixmap6 =
	    gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
				       "/gnome-fontsel.xpm");
	gtk_widget_show(pixmap6);
	gtk_box_pack_start(GTK_BOX(hbox67), pixmap6, TRUE, TRUE, 0);

	vbox57 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox57);
	gtk_box_pack_start(GTK_BOX(hbox67), vbox57, TRUE, TRUE, 0);

	label206 = gtk_label_new(_("Change font for"));
	gtk_widget_show(label206);
	gtk_box_pack_start(GTK_BOX(vbox57), label206, FALSE, FALSE, 0);

	label_mod = gtk_label_new(_("Module"));
	gtk_widget_show(label_mod);
	gtk_box_pack_start(GTK_BOX(vbox57), label_mod, FALSE, FALSE, 0);

	label_current_font = gtk_label_new(_("Current font: "));
	gtk_widget_show(label_current_font);
	gtk_box_pack_start(GTK_BOX(vbox57), label_current_font, FALSE,
			   FALSE, 0);


	hbox_picker = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox_picker);
	gtk_box_pack_start(GTK_BOX(vbox56), hbox_picker, FALSE, FALSE,
			   0);

	fontpicker = gnome_font_picker_new();
	gtk_widget_show(fontpicker);
	gtk_box_pack_start(GTK_BOX(hbox_picker), fontpicker, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(fontpicker, 254, -2);
	gnome_font_picker_set_mode(GNOME_FONT_PICKER(fontpicker),
				   GNOME_FONT_PICKER_MODE_FONT_INFO);
	gnome_font_picker_fi_set_show_size(GNOME_FONT_PICKER
					   (fontpicker), FALSE);
	gnome_font_picker_fi_set_use_font_in_label(GNOME_FONT_PICKER
						   (fontpicker), TRUE,
						   14);

	combo_size = gtk_combo_new();
	gtk_widget_show(combo_size);
	gtk_box_pack_start(GTK_BOX(hbox_picker), combo_size, TRUE, TRUE,
			   0);
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("+5"));
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("+4"));
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("+3"));
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("+2"));
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("+1"));
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("+0"));
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("-1"));
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("-2"));
	combo_size_items =
	    g_list_append(combo_size_items, (gpointer) _("-3"));
	gtk_combo_set_popdown_strings(GTK_COMBO(combo_size),
				      combo_size_items);
	g_list_free(combo_size_items);

	combo_entry_size = GTK_COMBO(combo_size)->entry;
	gtk_widget_show(combo_entry_size);
	gtk_entry_set_text(GTK_ENTRY(combo_entry_size), _("+0"));

	checkbutton_no_font =
	    gtk_check_button_new_with_label(
			_("Do not set a font for this module - use default font"));
	gtk_widget_show(checkbutton_no_font);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (checkbutton_no_font),
				     mf->no_font);
	gtk_box_pack_start(GTK_BOX(vbox56), checkbutton_no_font, FALSE,
			   FALSE, 0);

	dialog_action_area21 = GTK_DIALOG(dialog_mod_font)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog_mod_font),
			    "dialog_action_area21",
			    dialog_action_area21);
	gtk_widget_show(dialog_action_area21);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area21), 10);

	hbuttonbox1 = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox1);
	gtk_box_pack_start(GTK_BOX(dialog_action_area21), hbuttonbox1,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox1),
				  GTK_BUTTONBOX_EDGE);

	button_ok = gnome_stock_button(GNOME_STOCK_BUTTON_OK);
	gtk_widget_show(button_ok);
	gtk_container_add(GTK_CONTAINER(hbuttonbox1), button_ok);
	gtk_widget_set_sensitive(button_ok, FALSE);
	GTK_WIDGET_SET_FLAGS(button_ok, GTK_CAN_DEFAULT);

	button_cancel = gnome_stock_button(GNOME_STOCK_BUTTON_CANCEL);
	gtk_widget_show(button_cancel);
	gtk_container_add(GTK_CONTAINER(hbuttonbox1), button_cancel);
	GTK_WIDGET_SET_FLAGS(button_cancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(dialog_mod_font), "destroy",
			   GTK_SIGNAL_FUNC(dialog_destroy), mf);
	gtk_signal_connect(GTK_OBJECT(fontpicker), "font_set",
			   GTK_SIGNAL_FUNC(font_set), mf);
	gtk_signal_connect(GTK_OBJECT(checkbutton_no_font), "toggled",
			   GTK_SIGNAL_FUNC(no_font_toggled), mf);
	gtk_signal_connect(GTK_OBJECT(button_ok), "clicked",
			   GTK_SIGNAL_FUNC(ok_clicked), mf);
	gtk_signal_connect(GTK_OBJECT(button_cancel), "clicked",
			   GTK_SIGNAL_FUNC(cancel_clicked), mf);
	gtk_signal_connect(GTK_OBJECT(combo_entry_size), "changed",
			   GTK_SIGNAL_FUNC(size_changed), NULL);

	return dialog_mod_font;

}


/******************************************************************************
 * Name
 *   gui_set_module_font
 *
 * Synopsis
 *   #include "gui/font_dialog.h"
 *
 *   void gui_set_module_font(gchar * mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_set_module_font(gchar * mod_name)
{
	gchar buf[256];
	MOD_FONT *mf;

	mf = g_new(MOD_FONT, 1);
	mf->mod_name = mod_name;
	mf->old_font = NULL;
	mf->old_gdk_font = NULL;
	mf->old_font_size = NULL;
	mf->new_font = NULL;
	mf->new_gdk_font = NULL;
	mf->new_font_size = NULL;
	mf->no_font = 0;
	get_font_info(mf);

	new_font_set = 0;

	if (mf->old_font)
		sprintf(buf, "Current Font: %s", mf->old_font);
	else
		sprintf(buf, "Current Font: %s", "none");

	if (!strncmp(mf->old_font, "none", 4))
		mf->no_font = 1;

	dlg = create_dialog_mod_font(mf);
	gtk_widget_show(dlg);

	gtk_label_set_text(GTK_LABEL(label_mod), mf->mod_name);
	gtk_label_set_text(GTK_LABEL(label_current_font), buf);
	if(mf->old_gdk_font[0] == '-'){
		gnome_font_picker_set_font_name ((GnomeFontPicker *)fontpicker,
                                             mf->old_gdk_font);
	}
	/* this is here so the program will wait on the font information
	   so it can display the module with the new font - if there is a
	   better way please fix it :) */
	gtk_main ();
}
