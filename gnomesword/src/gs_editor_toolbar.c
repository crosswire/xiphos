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

/*
 * added to gnomesword Mon Dec 17 12:51:37 2001
 *
 */

#include <config.h>
#include <gnome.h>
#include <gal/widgets/widget-color-combo.h>

#include "gs_editor_toolbar.h"
#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlsettings.h>

#include "tt.xpm"


#define EDITOR_TOOLBAR_PATH "/HTMLEditor"


/* Paragraph style option menu.  */

static struct {
	GtkHTMLParagraphStyle style;
	const gchar *description;
} paragraph_style_items[] = {
	{ GTK_HTML_PARAGRAPH_STYLE_NORMAL, N_("Normal") },
	{ GTK_HTML_PARAGRAPH_STYLE_PRE, N_("Preformat") },
	{ GTK_HTML_PARAGRAPH_STYLE_H1, N_("Header 1") },
	{ GTK_HTML_PARAGRAPH_STYLE_H2, N_("Header 2") },
	{ GTK_HTML_PARAGRAPH_STYLE_H3, N_("Header 3") },
	{ GTK_HTML_PARAGRAPH_STYLE_H4, N_("Header 4") },
	{ GTK_HTML_PARAGRAPH_STYLE_H5, N_("Header 5") },
	{ GTK_HTML_PARAGRAPH_STYLE_H6, N_("Header 6") },
	{ GTK_HTML_PARAGRAPH_STYLE_ADDRESS, N_("Address") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMDOTTED, N_("List") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMDIGIT, N_("Num List") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMROMAN, N_("Roman List") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMALPHA, N_("Alpha List") },
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

static void
paragraph_style_menu_item_update (GtkWidget *widget, gpointer format_html)
{
	GtkHTMLParagraphStyle style;
	gint sensitive;

	style = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (widget), "paragraph_style_value"));
	
	sensitive = (format_html 
		     || style == GTK_HTML_PARAGRAPH_STYLE_NORMAL
		     || style == GTK_HTML_PARAGRAPH_STYLE_PRE
		     || style == GTK_HTML_PARAGRAPH_STYLE_ITEMDOTTED
		     || style == GTK_HTML_PARAGRAPH_STYLE_ITEMROMAN
		     || style == GTK_HTML_PARAGRAPH_STYLE_ITEMDIGIT
		     || style == GTK_HTML_PARAGRAPH_STYLE_ITEMALPHA
		     );

	gtk_widget_set_sensitive (widget, sensitive);	
}

static void
paragraph_style_option_menu_set_mode (GtkWidget *option_menu, gboolean format_html)
{
	GtkWidget *menu;
	
	menu = gtk_option_menu_get_menu (GTK_OPTION_MENU (option_menu));
	gtk_container_forall (GTK_CONTAINER (menu), 
			      paragraph_style_menu_item_update, 
			      GINT_TO_POINTER (format_html));
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
set_font_size (GtkWidget *w, GSHTMLEditorControlData *cd)
{
	GtkHTMLFontStyle style = GTK_HTML_FONT_STYLE_SIZE_1 + GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (w),
												    "size"));

	if (!cd->block_font_style_change)
		gtk_html_set_font_style (cd->html, GTK_HTML_FONT_STYLE_MAX & ~GTK_HTML_FONT_STYLE_SIZE_MASK, style);
}

static void
font_size_changed (GtkWidget *w, GtkHTMLParagraphStyle style, GSHTMLEditorControlData *cd)
{
	if (style == GTK_HTML_FONT_STYLE_DEFAULT)
		style = GTK_HTML_FONT_STYLE_SIZE_3;
	cd->block_font_style_change++;
	gtk_option_menu_set_history (GTK_OPTION_MENU (cd->font_size_menu),
				     (style & GTK_HTML_FONT_STYLE_SIZE_MASK) - GTK_HTML_FONT_STYLE_SIZE_1);
	cd->block_font_style_change--;
}

static GtkWidget *
setup_font_size_option_menu (GSHTMLEditorControlData *cd)
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
color_changed (GtkWidget *w, GdkColor *gdk_color, gboolean by_user, GSHTMLEditorControlData *cd)
{
	HTMLColor *color;
	
	/* If the color was changed programatically there's not need to set things */
	if (!by_user)
		return;
		
	color = gdk_color
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
	if (GTK_IS_CONTAINER (w))
		gtk_container_forall (GTK_CONTAINER (w), unset_focus, NULL);
}

static void
set_color_combo (GtkHTML *html, GSHTMLEditorControlData *cd)
{
	color_combo_set_color (COLOR_COMBO (cd->combo),
			       &html_colorset_get_color_allocated (html->engine->painter, HTMLTextColor)->color);
}

static void
realize_engine (GtkHTML *html, GSHTMLEditorControlData *cd)
{
	set_color_combo (html, cd);
	gtk_signal_disconnect_by_func (GTK_OBJECT (html), realize_engine, cd);
}

static void
load_done (GtkHTML *html, GSHTMLEditorControlData *cd)
{
	if (GTK_WIDGET_REALIZED (cd->html))
		set_color_combo (html, cd);
	else
		gtk_signal_connect (GTK_OBJECT (cd->html), "realize", realize_engine, cd);
}

static GtkWidget *
setup_color_combo (GSHTMLEditorControlData *cd)
{
	HTMLColor *color;

	color = html_colorset_get_color (cd->html->engine->settings->color_set, HTMLTextColor);
	if (GTK_WIDGET_REALIZED (cd->html))
		html_color_alloc (color, cd->html->engine->painter);
	else
		gtk_signal_connect (GTK_OBJECT (cd->html), "realize", realize_engine, cd);
        gtk_signal_connect (GTK_OBJECT (cd->html), "load_done", GTK_SIGNAL_FUNC (load_done), cd);

	cd->combo = color_combo_new (NULL, _("Automatic"), &color->color, color_group_fetch ("toolbar_text", cd));
	GTK_WIDGET_UNSET_FLAGS (cd->combo, GTK_CAN_FOCUS);
	gtk_container_forall (GTK_CONTAINER (cd->combo), unset_focus, NULL);
        gtk_signal_connect (GTK_OBJECT (cd->combo), "changed", GTK_SIGNAL_FUNC (color_changed), cd);

	gtk_widget_show_all (cd->combo);
	return cd->combo;
}


/* Font style group.  */

static void
editor_toolbar_tt_cb (GtkWidget *widget, GSHTMLEditorControlData *cd)
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
editor_toolbar_bold_cb (GtkWidget *widget, GSHTMLEditorControlData *cd)
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
editor_toolbar_italic_cb (GtkWidget *widget, GSHTMLEditorControlData *cd)
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
editor_toolbar_underline_cb (GtkWidget *widget, GSHTMLEditorControlData *cd)
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
editor_toolbar_strikeout_cb (GtkWidget *widget, GSHTMLEditorControlData *cd)
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
insertion_font_style_changed_cb (GtkHTML *widget, GtkHTMLFontStyle font_style, GSHTMLEditorControlData *cd)
{
	cd->block_font_style_change++;

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

	cd->block_font_style_change--;
}


/* Alignment group.  */

static void
editor_toolbar_left_align_cb (GtkWidget *widget,
			      GSHTMLEditorControlData *cd)
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
			  GSHTMLEditorControlData *cd)
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
			       GSHTMLEditorControlData *cd)
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
				GSHTMLEditorControlData *cd)
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
			  GSHTMLEditorControlData *cd)
{
	gtk_html_modify_indent_by_delta (GTK_HTML (cd->html), +1);
}

static void
editor_toolbar_unindent_cb (GtkWidget *widget,
			    GSHTMLEditorControlData *cd)
{
	gtk_html_modify_indent_by_delta (GTK_HTML (cd->html), -1);
}


/* Editor toolbar.  */

static GnomeUIInfo editor_toolbar_alignment_group[] = {
	GNOMEUIINFO_ITEM_STOCK (N_("Left align"), N_("Left justifies the paragraphs"),
				editor_toolbar_left_align_cb, GNOME_STOCK_PIXMAP_ALIGN_LEFT),
	GNOMEUIINFO_ITEM_STOCK (N_("Center"), N_("Center justifies the paragraphs"),
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
		    GSHTMLEditorControlData *cd)
{
	if (cd->html)
		gtk_signal_disconnect (GTK_OBJECT (cd->html),
				       cd->font_style_changed_connection_id);
}

static void
html_destroy_cb (GtkObject *object,
		 GSHTMLEditorControlData *cd)
{
	cd->html = NULL;
} */

static GtkWidget *
create_style_toolbar (GSHTMLEditorControlData *cd)
{
	GtkWidget *frame, *handleboxEditorBar;

	handleboxEditorBar = gtk_handle_box_new ();
	//frame = gtk_frame_new (NULL);
	//gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);

	cd->toolbar_style = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);

	gtk_toolbar_set_button_relief(GTK_TOOLBAR(cd->toolbar_style),
				      GTK_RELIEF_NONE);
	
	gtk_container_add (GTK_CONTAINER (handleboxEditorBar), cd->toolbar_style);
	//gtk_box_pack_start (GTK_BOX (handleboxEditorBar), frame, TRUE, TRUE, 0);

	gtk_widget_show_all (handleboxEditorBar);

	cd->paragraph_option = setup_paragraph_style_option_menu (cd->html),
	gtk_toolbar_prepend_widget (GTK_TOOLBAR (cd->toolbar_style),
				    cd->paragraph_option,
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

	
	cd->tt_button        = editor_toolbar_style_uiinfo [0].widget;
	cd->bold_button      = editor_toolbar_style_uiinfo [1].widget;
	cd->italic_button    = editor_toolbar_style_uiinfo [2].widget;
	cd->underline_button = editor_toolbar_style_uiinfo [3].widget;
	cd->strikeout_button = editor_toolbar_style_uiinfo [4].widget;

	cd->left_align_button = editor_toolbar_alignment_group[0].widget;
	cd->center_button = editor_toolbar_alignment_group[1].widget;
	cd->right_align_button = editor_toolbar_alignment_group[2].widget;

	cd->unindent_button  = editor_toolbar_style_uiinfo [8].widget;
	cd->indent_button    = editor_toolbar_style_uiinfo [9].widget;

	/* gtk_signal_connect (GTK_OBJECT (cd->html), "destroy",
	   GTK_SIGNAL_FUNC (html_destroy_cb), cd);

	   gtk_signal_connect (GTK_OBJECT (cd->toolbar_style), "destroy",
	   GTK_SIGNAL_FUNC (toolbar_destroy_cb), cd); */

	gtk_signal_connect (GTK_OBJECT (cd->html), "current_paragraph_alignment_changed",
			    GTK_SIGNAL_FUNC (paragraph_alignment_changed_cb), cd);

	return handleboxEditorBar;
}

static void
toolbar_item_update_sensitivity (GtkWidget *widget, gpointer data)
{
	GSHTMLEditorControlData *cd = (GSHTMLEditorControlData *)data;
	gboolean sensitive;

	sensitive = (cd->format_html
		     || widget == cd->paragraph_option
		     || widget == cd->indent_button
		     || widget == cd->unindent_button);

	gtk_widget_set_sensitive (widget, sensitive);
}

void
toolbar_update_format (GSHTMLEditorControlData *cd)
{
	gtk_container_forall (GTK_CONTAINER (cd->toolbar_style), 
			      toolbar_item_update_sensitivity, cd);

	paragraph_style_option_menu_set_mode (cd->paragraph_option, 
					      cd->format_html);
}


GtkWidget *
toolbar_style (GSHTMLEditorControlData *cd)
{
	g_return_val_if_fail (cd->html != NULL, NULL);
	g_return_val_if_fail (GTK_IS_HTML (cd->html), NULL);

	return create_style_toolbar (cd);
}
