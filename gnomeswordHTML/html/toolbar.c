/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*  This file is part of the GtkHTML library.

    Copyright (C) 2000 Helix Code, Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    Author: Ettore Perazzoli <ettore@helixcode.com>
*/

/* FIXME: Should use BonoboUIHandler.  */

#include <config.h>
#include <gnome.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <gal/widgets/widget-color-combo.h>
#include <htmlcolor.h>
#include <htmlcolorset.h>
#include <htmlengine-edit-fontstyle.h>
#include <htmlsettings.h>
#include <htmlengine-edit-cut-and-paste.h>

#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprint/gnome-print-master-preview.h>

#include "toolbar.h"
#include "utils.h"
#include "properties.h"
#include "text.h"
#include "gs_sword.h" /*  add by tb 2001-04-18  */
#include "gs_editor.h" /*  add by tb 2001-04-18  */
#include "tt.xpm"



#define EDITOR_TOOLBAR_PATH "/HTMLEditor"
#define BUFFER_SIZE 4096


gchar filename[240];


/* Paragraph style option menu.  */

static struct {
	GtkHTMLParagraphStyle style;
	const gchar *description;
} paragraph_style_items[] = {
	{ GTK_HTML_PARAGRAPH_STYLE_NORMAL, N_("Normal") },
	{ GTK_HTML_PARAGRAPH_STYLE_H1, N_("Header 1") },
	{ GTK_HTML_PARAGRAPH_STYLE_H2, N_("Header 2") },
	{ GTK_HTML_PARAGRAPH_STYLE_H3, N_("Header 3") },
	{ GTK_HTML_PARAGRAPH_STYLE_H4, N_("Header 4") },
	{ GTK_HTML_PARAGRAPH_STYLE_H5, N_("Header 5") },
	{ GTK_HTML_PARAGRAPH_STYLE_H6, N_("Header 6") },
	{ GTK_HTML_PARAGRAPH_STYLE_ADDRESS, N_("Address") },
	{ GTK_HTML_PARAGRAPH_STYLE_PRE, N_("Pre") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMDIGIT, N_("List item (digit)") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMDOTTED, N_("List item (unnumbered)") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMROMAN, N_("List item (roman)") },
	{ GTK_HTML_PARAGRAPH_STYLE_NORMAL, NULL },
};

static void
paragraph_style_changed_cb (GtkHTML *html,
			    GtkHTMLParagraphStyle style,
			    gpointer data)
{
	GtkOptionMenu *option_menu;
	guint i;

	option_menu = GTK_OPTION_MENU (data);

	for (i = 0; paragraph_style_items[i].description != NULL; i++) {
		if (paragraph_style_items[i].style == style) {
			gtk_option_menu_set_history (option_menu, i);
			return;
		}
	}

	g_warning ("Editor component toolbar: unknown paragraph style %d", style);
}

static void
paragraph_style_menu_item_activated_cb (GtkWidget *widget,
					gpointer data)
{
	GtkHTMLParagraphStyle style;
	GtkHTML *html;

	html = GTK_HTML (data);
	style = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (widget), "paragraph_style_value"));

	/* g_warning ("Setting paragraph style to %d.", style); */

	gtk_html_set_paragraph_style (html, style);
}

static GtkWidget *
setup_paragraph_style_option_menu (GtkHTML *html)
{
	GtkWidget *option_menu;
	GtkWidget *menu;
	guint i;

	option_menu = gtk_option_menu_new ();
	menu = gtk_menu_new ();

	for (i = 0; paragraph_style_items[i].description != NULL; i++) {
		GtkWidget *menu_item;

		menu_item = gtk_menu_item_new_with_label (_(paragraph_style_items[i].description));
		gtk_widget_show (menu_item);

		gtk_menu_append (GTK_MENU (menu), menu_item);

		gtk_object_set_data (GTK_OBJECT (menu_item), "paragraph_style_value",
				     GINT_TO_POINTER (paragraph_style_items[i].style));
		gtk_signal_connect (GTK_OBJECT (menu_item), "activate",
				    GTK_SIGNAL_FUNC (paragraph_style_menu_item_activated_cb),
				    html);
	}

	gtk_option_menu_set_menu (GTK_OPTION_MENU (option_menu), menu);

	gtk_signal_connect (GTK_OBJECT (html), "current_paragraph_style_changed",
			    GTK_SIGNAL_FUNC (paragraph_style_changed_cb), option_menu);

	gtk_widget_show (option_menu);

	return option_menu;
}

static void
set_font_size (GtkWidget *w, GtkHTMLControlData *cd)
{
	GtkHTMLFontStyle style = GTK_HTML_FONT_STYLE_SIZE_1 + GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (w),
												    "size"));

	if (!cd->block_font_style_change)
		gtk_html_set_font_style (cd->html, GTK_HTML_FONT_STYLE_MAX & ~GTK_HTML_FONT_STYLE_SIZE_MASK, style);
}

static void
font_size_changed (GtkWidget *w, GtkHTMLParagraphStyle style, GtkHTMLControlData *cd)
{
	if (style == GTK_HTML_FONT_STYLE_DEFAULT)
		style = GTK_HTML_FONT_STYLE_SIZE_3;
	cd->block_font_style_change = TRUE;
	gtk_option_menu_set_history (GTK_OPTION_MENU (cd->font_size_menu),
				     (style & GTK_HTML_FONT_STYLE_SIZE_MASK) - GTK_HTML_FONT_STYLE_SIZE_1);
	cd->block_font_style_change = FALSE;
}

static GtkWidget *
setup_font_size_option_menu (GtkHTMLControlData *cd)
{
	GtkWidget *option_menu;
	GtkWidget *menu;
	guint i;
	gchar size [3];

	cd->font_size_menu = option_menu = gtk_option_menu_new ();

	menu = gtk_menu_new ();
	size [2] = 0;

	for (i = 0; i < GTK_HTML_FONT_STYLE_SIZE_MAX; i++) {
		GtkWidget *menu_item;

		size [0] = (i>1) ? '+' : '-';
		size [1] = '0' + ((i>1) ? i - 2 : 2 - i);

		menu_item = gtk_menu_item_new_with_label (size);
		gtk_widget_show (menu_item);

		gtk_menu_append (GTK_MENU (menu), menu_item);

		gtk_object_set_data (GTK_OBJECT (menu_item), "size",
				     GINT_TO_POINTER (i));
		gtk_signal_connect (GTK_OBJECT (menu_item), "activate",
				    GTK_SIGNAL_FUNC (set_font_size), cd);
	}

	gtk_option_menu_set_menu (GTK_OPTION_MENU (option_menu), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (option_menu), 2);

	gtk_signal_connect (GTK_OBJECT (cd->html), "insertion_font_style_changed",
			    GTK_SIGNAL_FUNC (font_size_changed), cd);

	gtk_widget_show (option_menu);

	return option_menu;
}

static void
color_changed (GtkWidget *w, GdkColor *gdk_color, GtkHTMLControlData *cd)
{
	HTMLColor *color = gdk_color
		&& gdk_color != &html_colorset_get_color (cd->html->engine->settings->color_set, HTMLTextColor)->color
		? html_color_new_from_gdk_color (gdk_color) : NULL;

	gtk_html_set_color (cd->html, color);
	if (color)
		html_color_unref (color);
}

static void
unset_focus (GtkWidget *w, gpointer data)
{
	GTK_WIDGET_UNSET_FLAGS (w, GTK_CAN_FOCUS);
}

inline static void
set_color_combo (GtkHTML *html, GtkHTMLControlData *cd)
{
	color_combo_set_color (COLOR_COMBO (cd->combo),
			       &html_colorset_get_color_allocated (html->engine->painter, HTMLTextColor)->color);
}

static void
realize_engine (GtkHTML *html, GtkHTMLControlData *cd)
{
	set_color_combo (html, cd);
	gtk_signal_disconnect_by_func (GTK_OBJECT (html), realize_engine, cd);
}

static void
load_done (GtkHTML *html, GtkHTMLControlData *cd)
{
	set_color_combo (html, cd);
}

static GtkWidget *
setup_color_combo (GtkHTMLControlData *cd)
{
	HTMLColor *color;

	color = html_colorset_get_color (cd->html->engine->settings->color_set, HTMLTextColor);
	if (GTK_WIDGET_REALIZED (cd->html))
		html_color_alloc (color, cd->html->engine->painter);
	else
		gtk_signal_connect (GTK_OBJECT (cd->html), "realize", realize_engine, cd);
        gtk_signal_connect (GTK_OBJECT (cd->html), "load_done", GTK_SIGNAL_FUNC (load_done), cd);

	cd->combo = color_combo_new (NULL, _("Automatic"), &color->color, "toolbar_text");
	GTK_WIDGET_UNSET_FLAGS (cd->combo, GTK_CAN_FOCUS);
	gtk_container_forall (GTK_CONTAINER (cd->combo), unset_focus, NULL);
        gtk_signal_connect (GTK_OBJECT (cd->combo), "changed", GTK_SIGNAL_FUNC (color_changed), cd);

	gtk_widget_show_all (cd->combo);
	return cd->combo;
}


/* Font style group.  */

static void
editor_toolbar_tt_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (cd->html),
						 GTK_HTML_FONT_STYLE_MAX,
						 GTK_HTML_FONT_STYLE_FIXED);
		else
			gtk_html_set_font_style (GTK_HTML (cd->html), ~GTK_HTML_FONT_STYLE_FIXED, 0);
	}
}

static void
editor_toolbar_bold_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (cd->html),
						 GTK_HTML_FONT_STYLE_MAX,
						 GTK_HTML_FONT_STYLE_BOLD);
		else
			gtk_html_set_font_style (GTK_HTML (cd->html), ~GTK_HTML_FONT_STYLE_BOLD, 0);
	}
}

static void
editor_toolbar_italic_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (cd->html),
						 GTK_HTML_FONT_STYLE_MAX,
						 GTK_HTML_FONT_STYLE_ITALIC);
		else
			gtk_html_set_font_style (GTK_HTML (cd->html), ~GTK_HTML_FONT_STYLE_ITALIC, 0);
	}
}

static void
editor_toolbar_underline_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (cd->html),
						 GTK_HTML_FONT_STYLE_MAX,
						 GTK_HTML_FONT_STYLE_UNDERLINE);
		else
			gtk_html_set_font_style (GTK_HTML (cd->html), ~GTK_HTML_FONT_STYLE_UNDERLINE, 0);
	}
}

static void
editor_toolbar_strikeout_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			gtk_html_set_font_style (GTK_HTML (cd->html),
						 GTK_HTML_FONT_STYLE_MAX,
						 GTK_HTML_FONT_STYLE_STRIKEOUT);
		else
			gtk_html_set_font_style (GTK_HTML (cd->html), ~GTK_HTML_FONT_STYLE_STRIKEOUT, 0);
	}
}

static void
insertion_font_style_changed_cb (GtkHTML *widget, GtkHTMLFontStyle font_style, GtkHTMLControlData *cd)
{
	cd->block_font_style_change = TRUE;

	if (font_style & GTK_HTML_FONT_STYLE_FIXED)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->tt_button), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->tt_button), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_BOLD)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->bold_button), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->bold_button), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_ITALIC)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->italic_button), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->italic_button), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_UNDERLINE)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->underline_button), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->underline_button), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_STRIKEOUT)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->strikeout_button), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cd->strikeout_button), FALSE);

	cd->block_font_style_change = FALSE;
}


/* Alignment group.  */

static void
editor_toolbar_left_align_cb (GtkWidget *widget,
			      GtkHTMLControlData *cd)
{
	/* If the button is not active at this point, it means that the user clicked on
           some other button in the radio group.  */
	if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
		return;

	gtk_html_set_paragraph_alignment (GTK_HTML (cd->html),
					  GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT);
}

static void
editor_toolbar_center_cb (GtkWidget *widget,
			  GtkHTMLControlData *cd)
{
	/* If the button is not active at this point, it means that the user clicked on
           some other button in the radio group.  */
	if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
		return;

	gtk_html_set_paragraph_alignment (GTK_HTML (cd->html),
					  GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER);
}

static void
editor_toolbar_right_align_cb (GtkWidget *widget,
			       GtkHTMLControlData *cd)
{
	/* If the button is not active at this point, it means that the user clicked on
           some other button in the radio group.  */
	if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
		return;

	gtk_html_set_paragraph_alignment (GTK_HTML (cd->html),
					  GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT);
}

static void
safe_set_active (GtkWidget *widget,
		 gpointer data)
{
	GtkObject *object;
	GtkToggleButton *toggle_button;

	object = GTK_OBJECT (widget);
	toggle_button = GTK_TOGGLE_BUTTON (widget);

	gtk_signal_handler_block_by_data (object, data);
	gtk_toggle_button_set_active (toggle_button, TRUE);
	gtk_signal_handler_unblock_by_data (object, data);
}

static void
paragraph_alignment_changed_cb (GtkHTML *widget,
				GtkHTMLParagraphAlignment alignment,
				GtkHTMLControlData *cd)
{
	switch (alignment) {
	case GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT:
		safe_set_active (cd->left_align_button, cd);
		break;
	case GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER:
		safe_set_active (cd->center_button, cd);
		break;
	case GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT:
		safe_set_active (cd->right_align_button, cd);
		break;
	default:
		g_warning ("Unknown GtkHTMLParagraphAlignment %d.", alignment);
	}
}


/* Indentation group.  */

static void
editor_toolbar_indent_cb (GtkWidget *widget,
			  GtkHTMLControlData *cd)
{
	gtk_html_modify_indent_by_delta (GTK_HTML (cd->html), +1);
}

static void
editor_toolbar_unindent_cb (GtkWidget *widget,
			    GtkHTMLControlData *cd)
{
	gtk_html_modify_indent_by_delta (GTK_HTML (cd->html), -1);
}

/*  add by tb 2001-04-18  */
static void
editor_toolbar_code_cb(GtkWidget *widget,
			    GtkHTMLControlData *cd)
{
	
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
		gtk_notebook_set_page(GTK_NOTEBOOK(cd->notebook),1);
	else
		gtk_notebook_set_page(GTK_NOTEBOOK(cd->notebook),0);	
}


/* Editor toolbar.  */

static GnomeUIInfo editor_toolbar_alignment_group[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("Left align"), N_("Left justifies the paragraphs"),
				editor_toolbar_left_align_cb, GNOME_STOCK_PIXMAP_ALIGN_LEFT),
	GNOMEUIINFO_ITEM_STOCK (N_("Center"), N_("Centers justifies the paragraphs"),
				editor_toolbar_center_cb, GNOME_STOCK_PIXMAP_ALIGN_CENTER),
	GNOMEUIINFO_ITEM_STOCK (N_("Right align"), N_("Right justifies the paragraphs"),
				editor_toolbar_right_align_cb, GNOME_STOCK_PIXMAP_ALIGN_RIGHT),
	GNOMEUIINFO_END
};

static GnomeUIInfo editor_toolbar_style_uiinfo[] = {
	{ GNOME_APP_UI_TOGGLEITEM, N_("Typewriter"), N_("Toggle typewriter font style"),
	  editor_toolbar_tt_cb, NULL, NULL, GNOME_APP_PIXMAP_DATA, tt_xpm },
	{ GNOME_APP_UI_TOGGLEITEM, N_("Bold"), N_("Makes the text bold"),
	  editor_toolbar_bold_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_TEXT_BOLD },
	{ GNOME_APP_UI_TOGGLEITEM, N_("Italic"), N_("Makes the text italic"),
	  editor_toolbar_italic_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_TEXT_ITALIC },
	{ GNOME_APP_UI_TOGGLEITEM, N_("Underline"), N_("Underlines the text"),
	  editor_toolbar_underline_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_TEXT_UNDERLINE },
	{ GNOME_APP_UI_TOGGLEITEM, N_("Strikeout"), N_("Strikes out the text"),
	  editor_toolbar_strikeout_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_TEXT_STRIKEOUT },
	
	/*
	{ GNOME_APP_UI_TOGGLEITEM, N_("Code"), N_("Toggle code view"),
	  editor_toolbar_code_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_PREFERENCES},
	*/
	GNOMEUIINFO_SEPARATOR,

	GNOMEUIINFO_RADIOLIST (editor_toolbar_alignment_group),

	GNOMEUIINFO_SEPARATOR,

	GNOMEUIINFO_ITEM_STOCK (N_("Unindent"), N_("Indents the paragraphs less"),
				editor_toolbar_unindent_cb, GNOME_STOCK_PIXMAP_TEXT_UNINDENT),
	GNOMEUIINFO_ITEM_STOCK (N_("Indent"), N_("Indents the paragraphs more"),
				editor_toolbar_indent_cb, GNOME_STOCK_PIXMAP_TEXT_INDENT),

	GNOMEUIINFO_END
};

/* static void
toolbar_destroy_cb (GtkObject *object,
		    GtkHTMLControlData *cd)
{
	if (cd->html)
		gtk_signal_disconnect (GTK_OBJECT (cd->html),
				       cd->font_style_changed_connection_id);
}

static void
html_destroy_cb (GtkObject *object,
		 GtkHTMLControlData *cd)
{
	cd->html = NULL;
} */
static void
editor_toolbar_copy_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	html_engine_copy (cd->html->engine);
}
static void
editor_toolbar_cut_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	html_engine_cut(cd->html->engine);
}
static void
editor_toolbar_paste_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	html_engine_paste(cd->html->engine);
}


static GnomeUIInfo editor_toolbar_edit_uiinfo[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("Copy"), N_("Copy selected text"),
				editor_toolbar_copy_cb, GNOME_STOCK_PIXMAP_COPY),
	GNOMEUIINFO_ITEM_STOCK (N_("Cut"), N_("Cut selected text"),
				editor_toolbar_cut_cb, GNOME_STOCK_PIXMAP_CUT),
	GNOMEUIINFO_ITEM_STOCK (N_("Paste"), N_("Paste from clipboard"),
				editor_toolbar_paste_cb, GNOME_STOCK_PIXMAP_PASTE),
	GNOMEUIINFO_SEPARATOR,
	{ GNOME_APP_UI_TOGGLEITEM, N_("Code"), N_("Toggle code view"),
	  editor_toolbar_code_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_PREFERENCES },
	
	GNOMEUIINFO_END
};

static void
editor_toolbar_new_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	//gtk_html_load_empty(cd->html);
}

static void 
loadFILE(GtkWidget *filesel, GtkHTMLControlData *cd)
{
	GtkHTMLStream *stream;
	char buffer[BUFFER_SIZE];
	ssize_t count;
	gboolean was_editable = TRUE;
	int fd;		
	gint context_id2;

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(cd->statusbar),
					 "GnomeSword");
	gtk_statusbar_pop(GTK_STATUSBAR(cd->statusbar), context_id2);
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	//g_warning(filename);
	fd = open (filename, O_RDONLY);
	if (fd == -1)		

	was_editable = gtk_html_get_editable (cd->html);
	if (was_editable)
		gtk_html_set_editable (cd->html, FALSE);

	stream = gtk_html_begin (cd->html);
	if (stream == NULL) {
		close (fd);
		if (was_editable)
			gtk_html_set_editable (cd->html, TRUE);		
	}

	while (1) {
		count = read (fd, buffer, BUFFER_SIZE);
		if (count > 0)
			gtk_html_write (cd->html, stream, buffer, count);
		else
			break;
	}

	close (fd);

	if (count == 0) {
		gtk_html_end (cd->html, stream, GTK_HTML_STREAM_OK);
		if (was_editable)
			gtk_html_set_editable (cd->html, TRUE);
		cd->filename = filename;		
		gtk_statusbar_push(GTK_STATUSBAR(cd->statusbar), context_id2, filename);
	} else {
		gtk_html_end (cd->html, stream, GTK_HTML_STREAM_ERROR);
		if (was_editable)
			gtk_html_set_editable (cd->html, TRUE);
		gtk_statusbar_push(GTK_STATUSBAR(cd->statusbar), context_id2, "error loading file");
	}	
	gtk_widget_destroy(filesel);	
}

static void on_ok_button1_clicked(GtkButton * button, GtkHTMLControlData *cd)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	loadFILE(filesel,cd);
	//gtk_widget_destroy(filesel);
}

static void on_cancel_button1_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}


static
GtkWidget *create_fileselection1(GtkHTMLControlData *cd)
{
	GtkWidget *fileselection1;
	GtkWidget *ok_button1;
	GtkWidget *cancel_button1;

	fileselection1 =
	    gtk_file_selection_new("GnomeSword - Open Note File");
	gtk_object_set_data(GTK_OBJECT(fileselection1), "fileselection1",
			    fileselection1);
	gtk_container_set_border_width(GTK_CONTAINER(fileselection1), 10);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION
					       (fileselection1));

	ok_button1 = GTK_FILE_SELECTION(fileselection1)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselection1), "ok_button1",
			    ok_button1);
	gtk_widget_show(ok_button1);
	GTK_WIDGET_SET_FLAGS(ok_button1, GTK_CAN_DEFAULT);

	cancel_button1 = GTK_FILE_SELECTION(fileselection1)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselection1), "cancel_button1",
			    cancel_button1);
	gtk_widget_show(cancel_button1);
	GTK_WIDGET_SET_FLAGS(cancel_button1, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button1_clicked), cd);
	gtk_signal_connect(GTK_OBJECT(cancel_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button1_clicked),
			   NULL);

	return fileselection1;
}


static void
editor_toolbar_open_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	GtkWidget *dlg;
	
	dlg = create_fileselection1(cd);
	gtk_widget_show(dlg);
}

/* Saving.  */

static gboolean
save_receiver  (const HTMLEngine *engine,
		const char *data,
		unsigned int len,
		void *user_data)
{
	int fd;

	fd = GPOINTER_TO_INT (user_data);

	while (len > 0) {
		ssize_t count;

		count = write (fd, data, len);
		if (count < 0)
			return FALSE;

		len -= count;
	}

	return TRUE;
}
static int
save(gchar *file, GtkHTMLControlData *cd)
{
	int retval;
	int fd;
	gint context_id2;
	gchar buf[255];

	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(cd->statusbar),
					 "GnomeSword");
	gtk_statusbar_pop(GTK_STATUSBAR(cd->statusbar), context_id2);
	
	fd = open (file, O_WRONLY | O_CREAT, 0600);

	if (fd == -1)
		return -1;

	if (!gtk_html_save (cd->html, (GtkHTMLSaveReceiverFn)save_receiver, GINT_TO_POINTER (fd)))
		retval = -1;
	else {
		retval = 0;
		sprintf(buf,"%s - saved",filename);
		cd->filename = filename;
		gtk_statusbar_push(GTK_STATUSBAR(cd->statusbar), context_id2, buf);
	}

	close (fd);		
	return retval;
}

static
void on_ok_button2_clicked(GtkButton * button, GtkHTMLControlData *cd)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	gtk_widget_destroy(filesel);
	save(filename,cd);
}

static
void on_cancel_button2_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));	
	gtk_widget_destroy(filesel);
}

static
GtkWidget *create_fileselectionSave(GtkHTMLControlData *cd)
{
	GtkWidget *fileselectionSave;
	GtkWidget *ok_button2;
	GtkWidget *cancel_button2;

	fileselectionSave =
	    gtk_file_selection_new("GnomeSword - Save StudyPad File");
	gtk_object_set_data(GTK_OBJECT(fileselectionSave),
			    "fileselectionSave", fileselectionSave);
	gtk_container_set_border_width(GTK_CONTAINER(fileselectionSave),
				       10);

	ok_button2 = GTK_FILE_SELECTION(fileselectionSave)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselectionSave), "ok_button2",
			    ok_button2);
	gtk_widget_show(ok_button2);
	GTK_WIDGET_SET_FLAGS(ok_button2, GTK_CAN_DEFAULT);

	cancel_button2 =
	    GTK_FILE_SELECTION(fileselectionSave)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselectionSave),
			    "cancel_button2", cancel_button2);
	gtk_widget_show(cancel_button2);
	GTK_WIDGET_SET_FLAGS(cancel_button2, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button2_clicked), cd);
	gtk_signal_connect(GTK_OBJECT(cancel_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button2_clicked),
			   NULL);

	return fileselectionSave;
}



static void
editor_toolbar_save_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if(*filename)
		save(filename, cd);
	else {
		GtkWidget *dlg;	
		dlg = create_fileselectionSave(cd);
		gtk_widget_show(dlg);		
	}
}

static void
editor_toolbar_saveas_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	GtkWidget *dlg;
	
	dlg = create_fileselectionSave(cd);
	gtk_widget_show(dlg);
}

static gint page_num;
static GnomeFont *font;


static void
print_footer (GtkHTML *html, GnomePrintContext *context,
	      gdouble x, gdouble y, gdouble width, gdouble height, gpointer user_data)
{
	gchar *text = g_strdup_printf ("- %d -", page_num);
	gdouble tw = gnome_font_get_width_string (font, "text");

	if (font) {
		gnome_print_newpath     (context);
		gnome_print_setrgbcolor (context, .0, .0, .0);
		gnome_print_moveto      (context, x + (width - tw)/2, y - (height + gnome_font_get_ascender (font))/2);
		gnome_print_setfont     (context, font);
		gnome_print_show        (context, text);
	}

	g_free (text);
	page_num++;
}



static void
editor_toolbar_print_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	GnomePrintMaster *print_master;
	GnomePrintContext *print_context;
	GtkWidget *preview;
	
	//html = GTK_HTML(lookup_widget(MainFrm, (gchar *) data));
	print_master = gnome_print_master_new ();
	/*  gnome_print_master_set_paper (master, gnome_paper_with_name ("A4")); */

	print_context = gnome_print_master_get_context (print_master);

	page_num = 1;
	font = gnome_font_new_closest ("Helvetica", GNOME_FONT_BOOK, FALSE, 12);
	gtk_html_print_with_header_footer (cd->html, print_context, .0, .03, NULL, print_footer, NULL);
	if (font) gtk_object_unref (GTK_OBJECT (font));

	preview = GTK_WIDGET (gnome_print_master_preview_new (print_master, "GnomeSword Print Preview"));
	gtk_widget_show (preview);

	gtk_object_unref (GTK_OBJECT (print_master));
}

static void
editor_toolbar_sync_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	
}
GString *gstr;
static gboolean
save_note_receiver  (const HTMLEngine *engine,
		const char *data,
		unsigned int len,
		void *user_data)
{
	static gboolean startgrabing = FALSE;
		
	if(!strncmp(data,"</BODY>",7)) startgrabing = FALSE;
	if(startgrabing)
		gstr = g_string_append(gstr,data);
	if(!strcmp(data,"<BODY>")) startgrabing = TRUE;
	
	return TRUE;
}

static void
editor_toolbar_savenote_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	gtk_html_set_editable(cd->html,FALSE); 
	gstr = g_string_new("");
	if (!gtk_html_save(cd->html, (GtkHTMLSaveReceiverFn)save_note_receiver, GINT_TO_POINTER (0)))
		g_warning("file not writen");		
	else
		g_warning("file writen");
	g_warning(gstr->str);
	savenoteSWORD(gstr->str);
	g_string_free(gstr,1);
	gtk_html_set_editable(cd->html,TRUE); 		
}

static void
editor_toolbar_deletenote_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	deletenoteSWORD() ;
}



static GnomeUIInfo editor_toolbar_file_note_uiinfo[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("Sync"), N_("Follow Main Text Window"),
				editor_toolbar_sync_cb, GNOME_STOCK_PIXMAP_JUMP_TO),
	GNOMEUIINFO_ITEM_STOCK (N_("Save"), N_("Save Note"),
				editor_toolbar_savenote_cb, GNOME_STOCK_PIXMAP_ADD),
	GNOMEUIINFO_ITEM_STOCK (N_("Delete"), N_("Delete Note"),
				editor_toolbar_deletenote_cb, GNOME_STOCK_PIXMAP_REMOVE),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK (N_("Print"), N_("Print document"),
				editor_toolbar_print_cb, GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_END
};

static GnomeUIInfo editor_toolbar_file_uiinfo[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("New"), N_("New document"),
				editor_toolbar_new_cb, GNOME_STOCK_PIXMAP_NEW),
	GNOMEUIINFO_ITEM_STOCK (N_("Open"), N_("Open document"),
				editor_toolbar_open_cb, GNOME_STOCK_PIXMAP_OPEN),
	GNOMEUIINFO_ITEM_STOCK (N_("Save"), N_("Save document"),
				editor_toolbar_save_cb, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK (N_("Save AS"), N_("Save document as"),
				editor_toolbar_saveas_cb, GNOME_STOCK_PIXMAP_SAVE_AS),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK (N_("Print"), N_("Print document"),
				editor_toolbar_print_cb, GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_END
};

static void
editor_toolbar_insert_image_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);
	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, TRUE, _("Insert"));
	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_IMAGE, _("Image"),
						   image_insertion,
						   image_insert_cb,
						   image_close_cb);
	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_LINK, _("Link"),
						   link_properties,
						   link_apply_cb,
						   link_close_cb);
	gtk_html_edit_properties_dialog_show (cd->properties_dialog);	
}

static void
editor_toolbar_insert_link_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);

	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, FALSE, _("Insert"));

	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_TEXT, _("Text"),
						   text_properties,
						   text_apply_cb,
						   text_close_cb);
	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_LINK, _("Link"),
						   link_properties,
						   link_apply_cb,
						   link_close_cb);

	gtk_html_edit_properties_dialog_show (cd->properties_dialog);
	gtk_html_edit_properties_dialog_set_page (cd->properties_dialog, GTK_HTML_EDIT_PROPERTY_LINK);
}

static void
editor_toolbar_insert_rule_cb (GtkWidget *widget, GtkHTMLControlData *cd)
{
	if (cd->properties_dialog)
		gtk_html_edit_properties_dialog_close (cd->properties_dialog);

	cd->properties_dialog = gtk_html_edit_properties_dialog_new (cd, TRUE, _("Insert"));

	gtk_html_edit_properties_dialog_add_entry (cd->properties_dialog,
						   GTK_HTML_EDIT_PROPERTY_RULE, _("Rule"),
						   rule_insert,
						   rule_insert_cb,
						   rule_close_cb);

	gtk_html_edit_properties_dialog_show (cd->properties_dialog);
}

static GnomeUIInfo editor_toolbar_insert_uiinfo[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("Image"), N_("Insert an Image"),
				editor_toolbar_insert_image_cb, GNOME_STOCK_PIXMAP_ADD),
	GNOMEUIINFO_ITEM_STOCK (N_("Link"), N_("Insert a Link"),
				editor_toolbar_insert_link_cb, GNOME_STOCK_PIXMAP_ADD),
	GNOMEUIINFO_ITEM_STOCK (N_("Rule"), N_("Insert a Rule"),
				editor_toolbar_insert_rule_cb, GNOME_STOCK_PIXMAP_ADD),
	
	GNOMEUIINFO_END
};

static GtkWidget *
create_edit_toolbar (GtkHTMLControlData *cd)
{
	GtkWidget *frame, *hbox;

	hbox = gtk_hbox_new (FALSE, 0);
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	cd->toolbar_edit = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_container_add (GTK_CONTAINER (frame), cd->toolbar_edit);
	gtk_box_pack_start (GTK_BOX (hbox), frame, FALSE, FALSE, 0);

	gtk_widget_show_all (hbox);
	gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_edit), editor_toolbar_edit_uiinfo, NULL, cd);
	
	return hbox;
}

static GtkWidget *
create_file_toolbar (GtkHTMLControlData *cd)
{
	GtkWidget *frame, *hbox;

	hbox = gtk_hbox_new (FALSE, 0);
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	cd->toolbar_file = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_container_add (GTK_CONTAINER (frame), cd->toolbar_file);
	gtk_box_pack_start (GTK_BOX (hbox), frame, FALSE, FALSE, 0);

	gtk_widget_show_all (hbox);
	if(cd->note_editor)
		gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_file), editor_toolbar_file_note_uiinfo, NULL, cd);
	else
		gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_file), editor_toolbar_file_uiinfo, NULL, cd);
	
	return hbox;
}

static GtkWidget *
create_insert_toolbar (GtkHTMLControlData *cd)
{
	GtkWidget *frame, *hbox;

	hbox = gtk_hbox_new (FALSE, 0);
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	cd->toolbar_insert = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_container_add (GTK_CONTAINER (frame), cd->toolbar_insert);
	gtk_box_pack_start (GTK_BOX (hbox), frame, FALSE, FALSE, 0);

	gtk_widget_show_all (hbox);
	gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_insert), editor_toolbar_insert_uiinfo, NULL, cd);
	
	return hbox;
}

static GtkWidget *
create_style_toolbar (GtkHTMLControlData *cd)
{
	GtkWidget *frame, *hbox;

	hbox = gtk_hbox_new (FALSE, 0);
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);

	cd->toolbar_style = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);

	gtk_container_add (GTK_CONTAINER (frame), cd->toolbar_style);
	gtk_box_pack_start (GTK_BOX (hbox), frame, FALSE, FALSE, 0);

	gtk_widget_show_all (hbox);

	gtk_toolbar_prepend_widget (GTK_TOOLBAR (cd->toolbar_style),
				    setup_paragraph_style_option_menu (cd->html),
				    NULL, NULL);
	gtk_toolbar_prepend_widget (GTK_TOOLBAR (cd->toolbar_style),
				    setup_font_size_option_menu (cd),
				    NULL, NULL);

	gnome_app_fill_toolbar_with_data (GTK_TOOLBAR (cd->toolbar_style), editor_toolbar_style_uiinfo, NULL, cd);
	gtk_toolbar_append_widget (GTK_TOOLBAR (cd->toolbar_style),
				   setup_color_combo (cd),
				   NULL, NULL);

	cd->font_style_changed_connection_id
		= gtk_signal_connect (GTK_OBJECT (cd->html), "insertion_font_style_changed",
				      GTK_SIGNAL_FUNC (insertion_font_style_changed_cb), cd);

	/* The following SUCKS!  */
	cd->tt_button        = editor_toolbar_style_uiinfo [0].widget;
	cd->bold_button      = editor_toolbar_style_uiinfo [1].widget;
	cd->italic_button    = editor_toolbar_style_uiinfo [2].widget;
	cd->underline_button = editor_toolbar_style_uiinfo [3].widget;
	cd->strikeout_button = editor_toolbar_style_uiinfo [4].widget;

	cd->left_align_button = editor_toolbar_alignment_group[0].widget;
	cd->center_button = editor_toolbar_alignment_group[1].widget;
	cd->right_align_button = editor_toolbar_alignment_group[2].widget;

	/* gtk_signal_connect (GTK_OBJECT (cd->html), "destroy",
	   GTK_SIGNAL_FUNC (html_destroy_cb), cd);

	   gtk_signal_connect (GTK_OBJECT (cd->toolbar_style), "destroy",
	   GTK_SIGNAL_FUNC (toolbar_destroy_cb), cd); */

	gtk_signal_connect (GTK_OBJECT (cd->html), "current_paragraph_alignment_changed",
			    GTK_SIGNAL_FUNC (paragraph_alignment_changed_cb), cd);

	return hbox;
}


GtkWidget *
toolbar_style (GtkHTMLControlData *cd)
{
	g_return_val_if_fail (cd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (cd->html), NULL);
	
	return create_style_toolbar (cd);
}

GtkWidget *
toolbar_edit (GtkHTMLControlData *cd)
{
	g_return_val_if_fail (cd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (cd->html), NULL);
	
	return create_edit_toolbar (cd);
}

GtkWidget *
toolbar_file(GtkHTMLControlData *cd)
{
	g_return_val_if_fail (cd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (cd->html), NULL);
	
	return create_file_toolbar (cd);
}

GtkWidget *
toolbar_insert(GtkHTMLControlData *cd)
{
	g_return_val_if_fail (cd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (cd->html), NULL);
	
	return create_insert_toolbar (cd);
}

