/*
 * GnomeSword Bible Study Tool
 * toolbar_style.c - style toolbar for editors
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
 * most of this code is from the GtkHTML library.
 */
#include <config.h>
#include <gnome.h>
#include <gal/widgets/widget-color-combo.h>

#include <gtkhtml/htmlcolor.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlsettings.h>

#include "gui/toolbar_style.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/settings.h"

#include "./pixmaps/tt.xpm"

static struct {
	GtkHTMLParagraphStyle style;
	const gchar *description;
} paragraph_style_items[] = {
	{ GTK_HTML_PARAGRAPH_STYLE_NORMAL, N_("Normal") },
	{ GTK_HTML_PARAGRAPH_STYLE_PRE, N_("Preformat") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMDOTTED, N_("Bulleted List") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMDIGIT, N_("Numbered List") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMROMAN, N_("Roman List") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMALPHA, N_("Alphabetical List") },
	{ GTK_HTML_PARAGRAPH_STYLE_H1, N_("Header 1") },
	{ GTK_HTML_PARAGRAPH_STYLE_H2, N_("Header 2") },
	{ GTK_HTML_PARAGRAPH_STYLE_H3, N_("Header 3") },
	{ GTK_HTML_PARAGRAPH_STYLE_H4, N_("Header 4") },
	{ GTK_HTML_PARAGRAPH_STYLE_H5, N_("Header 5") },
	{ GTK_HTML_PARAGRAPH_STYLE_H6, N_("Header 6") },
	{ GTK_HTML_PARAGRAPH_STYLE_ADDRESS, N_("Address") },
	{ GTK_HTML_PARAGRAPH_STYLE_NORMAL, NULL },
};
/******************************************************************************
 * Name
 *   paragraph_style_changed_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void paragraph_style_changed_cb (GtkHTML *html,
 *			GtkHTMLParagraphStyle style, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void paragraph_style_changed_cb(GtkHTML * html,
				       GtkHTMLParagraphStyle style,
				       gpointer data)
{
	GtkOptionMenu *option_menu;
	guint i;

	option_menu = GTK_OPTION_MENU(data);

	for (i = 0; paragraph_style_items[i].description != NULL; i++) {
		if (paragraph_style_items[i].style == style) {
			gtk_option_menu_set_history(option_menu, i);
			return;
		}
	}

	g_warning
	    ("Editor component toolbar: unknown paragraph style %d",
	     style);
}

/******************************************************************************
 * Name
 *   paragraph_style_menu_item_activated_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void paragraph_style_menu_item_activated_cb (
 *				GtkWidget *widget, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void paragraph_style_menu_item_activated_cb(GtkWidget * widget,
						   gpointer data)
{
	GtkHTMLParagraphStyle style;
	GtkHTML *html;

	html = GTK_HTML(data);
	style = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT
						    (widget),
						    "paragraph_style_value"));

	/* g_warning ("Setting paragraph style to %d.", style); */

	gtk_html_set_paragraph_style(html, style);
}

/******************************************************************************
 * Name
 *   paragraph_style_menu_item_update
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void paragraph_style_menu_item_update (GtkWidget *widget, 
 *						gpointer format_html)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void paragraph_style_menu_item_update(GtkWidget * widget,
					     gpointer format_html)
{
	GtkHTMLParagraphStyle style;
	gint sensitive;

	style = GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT
						    (widget),
						    "paragraph_style_value"));

	sensitive = (format_html
		     || style == GTK_HTML_PARAGRAPH_STYLE_NORMAL
		     || style == GTK_HTML_PARAGRAPH_STYLE_PRE
		     || style == GTK_HTML_PARAGRAPH_STYLE_ITEMDOTTED
		     || style == GTK_HTML_PARAGRAPH_STYLE_ITEMROMAN
		     || style == GTK_HTML_PARAGRAPH_STYLE_ITEMDIGIT
		     || style == GTK_HTML_PARAGRAPH_STYLE_ITEMALPHA);

	gtk_widget_set_sensitive(widget, sensitive);
}

/******************************************************************************
 * Name
 *   paragraph_style_option_menu_set_mode
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void paragraph_style_option_menu_set_mode (
 *			GtkWidget *option_menu, gboolean format_html)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void paragraph_style_option_menu_set_mode(GtkWidget *
						 option_menu,
						 gboolean format_html)
{
	GtkWidget *menu;

	menu = gtk_option_menu_get_menu(GTK_OPTION_MENU(option_menu));
	gtk_container_forall(GTK_CONTAINER(menu),
			     paragraph_style_menu_item_update,
			     GINT_TO_POINTER(format_html));
}

/******************************************************************************
 * Name
 *   setup_paragraph_style_option_menu
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   GtkWidget *setup_paragraph_style_option_menu (GtkHTML *html)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *setup_paragraph_style_option_menu(GtkHTML * html)
{
	GtkWidget *option_menu;
	GtkWidget *menu;
	guint i;

	option_menu = gtk_option_menu_new();
	menu = gtk_menu_new();

	for (i = 0; paragraph_style_items[i].description != NULL; i++) {
		GtkWidget *menu_item;

		menu_item = gtk_menu_item_new_with_label
		    (_(paragraph_style_items[i].description));
		gtk_widget_show(menu_item);

		gtk_menu_append(GTK_MENU(menu), menu_item);

		gtk_object_set_data(GTK_OBJECT(menu_item),
				    "paragraph_style_value",
				    GINT_TO_POINTER
				    (paragraph_style_items[i].style));
		gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
				   G_CALLBACK
				   (paragraph_style_menu_item_activated_cb),
				   html);
	}

	gtk_option_menu_set_menu(GTK_OPTION_MENU(option_menu), menu);

	gtk_signal_connect(GTK_OBJECT(html),
			   "current_paragraph_style_changed",
			   G_CALLBACK(paragraph_style_changed_cb),
			   option_menu);

	gtk_widget_show(option_menu);

	return option_menu;
}

/******************************************************************************
 * Name
 *   set_font_size
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void set_font_size (GtkWidget *w, GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void set_font_size(GtkWidget * w, GSHTMLEditorControlData * cd)
{
	GtkHTMLFontStyle style = GTK_HTML_FONT_STYLE_SIZE_1 +
	    GPOINTER_TO_INT(gtk_object_get_data(GTK_OBJECT(w),
						"size"));

	if (!cd->block_font_style_change)
		gtk_html_set_font_style(cd->html,
					GTK_HTML_FONT_STYLE_MAX &
					~GTK_HTML_FONT_STYLE_SIZE_MASK,
					style);
}

/******************************************************************************
 * Name
 *   font_size_changed
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void font_size_changed (GtkWidget *w, 
 *		GtkHTMLParagraphStyle style,GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void font_size_changed(GtkWidget * w,
			      GtkHTMLParagraphStyle style,
			      GSHTMLEditorControlData * cd)
{
	if (style == GTK_HTML_FONT_STYLE_DEFAULT)
		style = GTK_HTML_FONT_STYLE_SIZE_3;
	cd->block_font_style_change++;
	gtk_option_menu_set_history(GTK_OPTION_MENU(cd->font_size_menu),
				    (style &
				     GTK_HTML_FONT_STYLE_SIZE_MASK) -
				    GTK_HTML_FONT_STYLE_SIZE_1);
	cd->block_font_style_change--;
}

/******************************************************************************
 * Name
 *   setup_font_size_option_menu
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   GtkWidget *setup_font_size_option_menu (
 *				GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *setup_font_size_option_menu(GSHTMLEditorControlData *
					      cd)
{
	GtkWidget *option_menu;
	GtkWidget *menu;
	guint i;
	gchar size[3];

	cd->font_size_menu = option_menu = gtk_option_menu_new();

	menu = gtk_menu_new();
	size[2] = 0;

	for (i = 0; i < GTK_HTML_FONT_STYLE_SIZE_MAX; i++) {
		GtkWidget *menu_item;

		size[0] = (i > 1) ? '+' : '-';
		size[1] = '0' + ((i > 1) ? i - 2 : 2 - i);

		menu_item = gtk_menu_item_new_with_label(size);
		gtk_widget_show(menu_item);

		gtk_menu_append(GTK_MENU(menu), menu_item);

		gtk_object_set_data(GTK_OBJECT(menu_item), "size",
				    GINT_TO_POINTER(i));
		gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
				   G_CALLBACK(set_font_size), cd);
	}

	gtk_option_menu_set_menu(GTK_OPTION_MENU(option_menu), menu);
	gtk_option_menu_set_history(GTK_OPTION_MENU(option_menu), 2);

	gtk_signal_connect(GTK_OBJECT(cd->html),
			   "insertion_font_style_changed",
			   G_CALLBACK(font_size_changed), cd);

	gtk_widget_show(option_menu);

	return option_menu;
}

/******************************************************************************
 * Name
 *   color_changed
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void color_changed (GtkWidget *w, GdkColor *gdk_color, 
 *			gboolean by_user, GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void color_changed(GtkWidget * w, GdkColor * gdk_color,
			gboolean custom,	
			  gboolean by_user,
			  gboolean is_default,
			  GSHTMLEditorControlData * cd)
{
	HTMLColor *color;

	/* If the color was changed programatically 
	   there's no need to set things */ 
	if (!by_user)
		return;

	color = gdk_color && gdk_color !=
	    &html_colorset_get_color(cd->html->engine->settings->
				     color_set,
				     HTMLTextColor)->
	    color ? html_color_new_from_gdk_color(gdk_color) : NULL;
	if (color) {
		gtk_html_set_color(cd->html, color);
		html_color_unref(color);
	}
}

/******************************************************************************
 * Name
 *   unset_focus
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void unset_focus (GtkWidget *w, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void unset_focus(GtkWidget * w, gpointer data)
{
	GTK_WIDGET_UNSET_FLAGS(w, GTK_CAN_FOCUS);
	if (GTK_IS_CONTAINER(w))
		gtk_container_forall(GTK_CONTAINER(w),
				     unset_focus, NULL);
}

/******************************************************************************
 * Name
 *   set_color_combo
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void set_color_combo (GtkHTML *html, GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void set_color_combo(GtkHTML * html,
			    GSHTMLEditorControlData * cd)
{
	color_combo_set_color(COLOR_COMBO(cd->combo),
			      &html_colorset_get_color_allocated
			      (html->engine->painter,
			       HTMLTextColor)->color);
}

/******************************************************************************
 * Name
 *   realize_engine
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void realize_engine (GtkHTML *html, GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void realize_engine(GtkHTML * html, GSHTMLEditorControlData * cd)
{
	set_color_combo(html, cd);
	gtk_signal_disconnect_by_func(GTK_OBJECT(html),
				      G_CALLBACK(realize_engine), cd);
}

/******************************************************************************
 * Name
 *   load_done
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void load_done (GtkHTML *html, GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void load_done(GtkHTML * html, GSHTMLEditorControlData * cd)
{
	if (GTK_WIDGET_REALIZED(cd->html))
		set_color_combo(html, cd);
	else
		gtk_signal_connect(GTK_OBJECT(cd->html), "realize",
				   G_CALLBACK(realize_engine), cd);
}

/******************************************************************************
 * Name
 *   setup_color_combo
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   GtkWidget *setup_color_combo (GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *setup_color_combo(GSHTMLEditorControlData * cd)
{
	HTMLColor *color;

	color =
	    html_colorset_get_color(cd->html->engine->settings->
				    color_set, HTMLTextColor);
	if (GTK_WIDGET_REALIZED(cd->html))
		html_color_alloc(color, cd->html->engine->painter);
	else
		gtk_signal_connect(GTK_OBJECT(cd->html), "realize",
				   G_CALLBACK(realize_engine), cd);
	gtk_signal_connect(GTK_OBJECT(cd->html), "load_done",
			   G_CALLBACK(load_done), cd);

	cd->combo = color_combo_new(NULL, _("Automatic"), &color->color,
				    color_group_fetch("toolbar_text",
						      cd));
	GTK_WIDGET_UNSET_FLAGS(cd->combo, GTK_CAN_FOCUS);
	gtk_container_forall(GTK_CONTAINER(cd->combo),
			     unset_focus, NULL);

	gtk_widget_show_all(cd->combo);
	return cd->combo;
}

/******************************************************************************
 * Name
 *   editor_toolbar_tt_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_tt_cb (GtkWidget *widget, 
 *					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_tt_cb(GtkWidget * widget,
				 GSHTMLEditorControlData * cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
						 (widget)))
			gtk_html_set_font_style(GTK_HTML(cd->html),
						GTK_HTML_FONT_STYLE_MAX,
						GTK_HTML_FONT_STYLE_FIXED);
		else
			gtk_html_set_font_style(GTK_HTML(cd->html),
						~GTK_HTML_FONT_STYLE_FIXED,
						0);
	}
}

/******************************************************************************
 * Name
 *   editor_toolbar_bold_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_bold_cb (GtkWidget *widget, 
 *					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_bold_cb(GtkWidget * widget,
				   GSHTMLEditorControlData * cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
						 (widget)))
			gtk_html_set_font_style(GTK_HTML(cd->html),
						GTK_HTML_FONT_STYLE_MAX,
						GTK_HTML_FONT_STYLE_BOLD);
		else
			gtk_html_set_font_style(GTK_HTML(cd->html),
						~GTK_HTML_FONT_STYLE_BOLD,
						0);
	}
}

/******************************************************************************
 * Name
 *   editor_toolbar_italic_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_italic_cb (GtkWidget *widget, 
 *					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_italic_cb(GtkWidget * widget,
				     GSHTMLEditorControlData * cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
						 (widget)))
			gtk_html_set_font_style(GTK_HTML(cd->html),
						GTK_HTML_FONT_STYLE_MAX,
						GTK_HTML_FONT_STYLE_ITALIC);
		else
			gtk_html_set_font_style(GTK_HTML(cd->html),
						~GTK_HTML_FONT_STYLE_ITALIC,
						0);
	}
}

/******************************************************************************
 * Name
 *   editor_toolbar_underline_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_underline_cb (GtkWidget *widget, 
 *					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_underline_cb(GtkWidget * widget,
					GSHTMLEditorControlData * cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active
		    (GTK_TOGGLE_BUTTON(widget)))
			gtk_html_set_font_style(GTK_HTML(cd->html),
						GTK_HTML_FONT_STYLE_MAX,
						GTK_HTML_FONT_STYLE_UNDERLINE);
		else
			gtk_html_set_font_style(GTK_HTML(cd->html),
						~GTK_HTML_FONT_STYLE_UNDERLINE,
						0);
	}
}

/******************************************************************************
 * Name
 *   editor_toolbar_strikeout_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_strikeout_cb (GtkWidget *widget, 
					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_strikeout_cb(GtkWidget * widget,
					GSHTMLEditorControlData * cd)
{
	if (!cd->block_font_style_change) {
		if (gtk_toggle_button_get_active
		    (GTK_TOGGLE_BUTTON(widget)))
			gtk_html_set_font_style(GTK_HTML(cd->html),
						GTK_HTML_FONT_STYLE_MAX,
						GTK_HTML_FONT_STYLE_STRIKEOUT);
		else
			gtk_html_set_font_style(GTK_HTML(cd->html),
						~GTK_HTML_FONT_STYLE_STRIKEOUT,
						0);
	}
}

/******************************************************************************
 * Name
 *   insertion_font_style_changed_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void insertion_font_style_changed_cb (GtkHTML *widget, 
 *		GtkHTMLFontStyle font_style,GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void insertion_font_style_changed_cb(GtkHTML * widget,
					    GtkHTMLFontStyle font_style,
					    GSHTMLEditorControlData *
					    cd)
{
	cd->block_font_style_change++;

	if (font_style & GTK_HTML_FONT_STYLE_FIXED)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->tt_button), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->tt_button), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_BOLD)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->bold_button), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->bold_button), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_ITALIC)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->italic_button), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->italic_button),
					     FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_UNDERLINE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->underline_button),
					     TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->underline_button),
					     FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_STRIKEOUT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->strikeout_button),
					     TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (cd->strikeout_button),
					     FALSE);

	cd->block_font_style_change--;
}

/******************************************************************************
 * Name
 *   editor_toolbar_left_align_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_left_align_cb (GtkWidget *widget,
 *					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_left_align_cb(GtkWidget * widget,
					 GSHTMLEditorControlData * cd)
{
	/* If the button is not active at this point, it means that the user clicked on
	   some other button in the radio group.  */
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		return;

	gtk_html_set_paragraph_alignment(GTK_HTML(cd->html),
					 GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT);
}

/******************************************************************************
 * Name
 *   editor_toolbar_center_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_center_cb (GtkWidget *widget,
					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_center_cb(GtkWidget * widget,
				     GSHTMLEditorControlData * cd)
{
	/* If the button is not active at this point, it means that the user clicked on
	   some other button in the radio group.  */
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		return;

	gtk_html_set_paragraph_alignment(GTK_HTML(cd->html),
					 GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER);
}

/******************************************************************************
 * Name
 *   editor_toolbar_right_align_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_right_align_cb (GtkWidget *widget,
					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_right_align_cb(GtkWidget * widget,
					  GSHTMLEditorControlData * cd)
{
	/* If the button is not active at this point, it means that the user clicked on
	   some other button in the radio group.  */
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		return;

	gtk_html_set_paragraph_alignment(GTK_HTML(cd->html),
					 GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT);
}

/******************************************************************************
 * Name
 *   safe_set_active
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void safe_set_active (GtkWidget *widget, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void safe_set_active(GtkWidget * widget, gpointer data)
{
	GtkObject *object;
	GtkToggleButton *toggle_button;

	object = GTK_OBJECT(widget);
	toggle_button = GTK_TOGGLE_BUTTON(widget);

	gtk_signal_handler_block_by_data(object, data);
	gtk_toggle_button_set_active(toggle_button, TRUE);
	gtk_signal_handler_unblock_by_data(object, data);
}

/******************************************************************************
 * Name
 *   paragraph_alignment_changed_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void paragraph_alignment_changed_cb (GtkHTML *widget,
	GtkHTMLParagraphAlignment alignment,GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void paragraph_alignment_changed_cb(GtkHTML * widget,
					   GtkHTMLParagraphAlignment
					   alignment,
					   GSHTMLEditorControlData * cd)
{
	switch (alignment) {
	case GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT:
		safe_set_active(cd->left_align_button, cd);
		break;
	case GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER:
		safe_set_active(cd->center_button, cd);
		break;
	case GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT:
		safe_set_active(cd->right_align_button, cd);
		break;
	default:
		g_warning("Unknown GtkHTMLParagraphAlignment %d.",
			  alignment);
	}
}

/******************************************************************************
 * Name
 *   editor_toolbar_indent_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_indent_cb (GtkWidget *widget,
 *					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_indent_cb(GtkWidget * widget,
				     GSHTMLEditorControlData * cd)
{
	gtk_html_indent_push_level(GTK_HTML (cd->html), 
				HTML_LIST_TYPE_BLOCKQUOTE);
}

/******************************************************************************
 * Name
 *   editor_toolbar_unindent_cb
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void editor_toolbar_unindent_cb (GtkWidget *widget,
 *					GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void editor_toolbar_unindent_cb(GtkWidget * widget,
				       GSHTMLEditorControlData * cd)
{
	gtk_html_indent_pop_level (GTK_HTML (cd->html));
}

/******************************************************************************
 * toolbar items
 */

static GnomeUIInfo editor_toolbar_alignment_group[] = {
	GNOMEUIINFO_ITEM_STOCK(N_("Left align"),
			       N_("Left justifies the paragraphs"),
			       editor_toolbar_left_align_cb,
			       GNOME_STOCK_PIXMAP_ALIGN_LEFT),
	GNOMEUIINFO_ITEM_STOCK(N_("Center"),
			       N_("Center justifies the paragraphs"),
			       editor_toolbar_center_cb,
			       GNOME_STOCK_PIXMAP_ALIGN_CENTER),
	GNOMEUIINFO_ITEM_STOCK(N_("Right align"),
			       N_("Right justifies the paragraphs"),
			       editor_toolbar_right_align_cb,
			       GNOME_STOCK_PIXMAP_ALIGN_RIGHT),
	GNOMEUIINFO_END
};

/******************************************************************************
 * toolbar items
 */

static GnomeUIInfo editor_toolbar_style_uiinfo[] = {

	{GNOME_APP_UI_TOGGLEITEM, N_("Typewriter"),
	 N_("Toggle typewriter font style"),
	 editor_toolbar_tt_cb, NULL, NULL, GNOME_APP_PIXMAP_DATA,
	 tt_xpm},
	{GNOME_APP_UI_TOGGLEITEM, N_("Bold"), N_("Makes the text bold"),
	 editor_toolbar_bold_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK,
	 GNOME_STOCK_PIXMAP_TEXT_BOLD},
	{GNOME_APP_UI_TOGGLEITEM, N_("Italic"),
	 N_("Makes the text italic"),
	 editor_toolbar_italic_cb, NULL, NULL, GNOME_APP_PIXMAP_STOCK,
	 GNOME_STOCK_PIXMAP_TEXT_ITALIC},
	{GNOME_APP_UI_TOGGLEITEM, N_("Underline"),
	 N_("Underlines the text"),
	 editor_toolbar_underline_cb, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_TEXT_UNDERLINE},
	{GNOME_APP_UI_TOGGLEITEM, N_("Strikeout"),
	 N_("Strikes out the text"),
	 editor_toolbar_strikeout_cb, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_TEXT_STRIKEOUT},

	GNOMEUIINFO_SEPARATOR,

	GNOMEUIINFO_RADIOLIST(editor_toolbar_alignment_group),

	GNOMEUIINFO_SEPARATOR,

	GNOMEUIINFO_ITEM_STOCK(N_("Unindent"),
			       N_("Indents the paragraphs less"),
			       editor_toolbar_unindent_cb,
			       GNOME_STOCK_PIXMAP_TEXT_UNINDENT),
	GNOMEUIINFO_ITEM_STOCK(N_("Indent"),
			       N_("Indents the paragraphs more"),
			       editor_toolbar_indent_cb,
			       GNOME_STOCK_PIXMAP_TEXT_INDENT),

	GNOMEUIINFO_END
};

/******************************************************************************
 * Name
 *   create_style_toolbar
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   GtkWidget *create_style_toolbar (GSHTMLEditorControlData *cd)
 *
 * Description
 *   create the toolbar
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_style_toolbar(GSHTMLEditorControlData * cd)
{
	GtkWidget *fontitem;
	GtkWidget *coloritem;
	cd->toolbar_style =
	    gtk_toolbar_new();
	gtk_widget_show_all(cd->toolbar_style);
	gtk_toolbar_set_style (GTK_TOOLBAR (cd->toolbar_style), GTK_TOOLBAR_ICONS);

	cd->paragraph_option =
	    setup_paragraph_style_option_menu(cd->html),
	    gtk_toolbar_prepend_widget(GTK_TOOLBAR(cd->toolbar_style),
				       cd->paragraph_option, NULL,
				       NULL);
	fontitem = setup_font_size_option_menu(cd);
	gtk_toolbar_prepend_widget(GTK_TOOLBAR(cd->toolbar_style),
				   fontitem,
				   NULL, NULL);

	gnome_app_fill_toolbar_with_data(GTK_TOOLBAR(cd->toolbar_style),
					 editor_toolbar_style_uiinfo,
					 NULL, cd);
	coloritem = setup_color_combo(cd);
	gtk_toolbar_append_widget(GTK_TOOLBAR(cd->toolbar_style),
				  coloritem, NULL, NULL);

	cd->font_style_changed_connection_id
	    =
	    g_signal_connect(G_OBJECT(cd->htmlwidget),
			       "insertion_font_style_changed",
			       G_CALLBACK
			       (insertion_font_style_changed_cb), cd);


	cd->tt_button = editor_toolbar_style_uiinfo[0].widget;
	cd->bold_button = editor_toolbar_style_uiinfo[1].widget;
	cd->italic_button = editor_toolbar_style_uiinfo[2].widget;
	cd->underline_button = editor_toolbar_style_uiinfo[3].widget;
	cd->strikeout_button = editor_toolbar_style_uiinfo[4].widget;

	cd->left_align_button =
	    editor_toolbar_alignment_group[0].widget;
	cd->center_button = editor_toolbar_alignment_group[1].widget;
	cd->right_align_button =
	    editor_toolbar_alignment_group[2].widget;

	cd->unindent_button = editor_toolbar_style_uiinfo[8].widget;
	cd->indent_button = editor_toolbar_style_uiinfo[9].widget;

	gtk_toolbar_set_style (GTK_TOOLBAR (cd->toolbar_style), GTK_TOOLBAR_ICONS);
	
	gtk_signal_connect(GTK_OBJECT(cd->html),
			   "current_paragraph_alignment_changed",
			   G_CALLBACK
			   (paragraph_alignment_changed_cb), cd);

	gtk_signal_connect(GTK_OBJECT(cd->combo), "color_changed",
			   G_CALLBACK(color_changed), cd);
	return cd->toolbar_style;
}

/******************************************************************************
 * Name
 *   toolbar_item_update_sensitivity
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void toolbar_item_update_sensitivity (GtkWidget *widget,
 *							gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void toolbar_item_update_sensitivity(GtkWidget * widget,
					    gpointer data) 
{
	GSHTMLEditorControlData *cd = (GSHTMLEditorControlData *) data;
	gboolean sensitive;

	sensitive = (cd->format_html
		     || widget == cd->paragraph_option
		     || widget == cd->indent_button
		     || widget == cd->unindent_button);

	gtk_widget_set_sensitive(widget, sensitive);
}

/******************************************************************************
 * Name
 *   toolbar_update_format
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   void toolbar_update_format (GSHTMLEditorControlData *cd)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void toolbar_update_format(GSHTMLEditorControlData * cd)
{
	gtk_container_forall(GTK_CONTAINER(cd->toolbar_style),
			     toolbar_item_update_sensitivity, cd);

	paragraph_style_option_menu_set_mode(cd->paragraph_option,
					     cd->format_html);
}

/******************************************************************************
 * Name
 *   toolbar_style
 *
 * Synopsis
 *   #include "editor_toolbar.h"
 *
 *   GtkWidget *toolbar_style (GSHTMLEditorControlData *cd)
 *
 * Description
 *   calls create_style_toolbar() and returns created toolbar
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_toolbar_style(GSHTMLEditorControlData * cd)
{
	g_return_val_if_fail(cd->html != NULL, NULL);
	g_return_val_if_fail(GTK_IS_HTML(cd->html), NULL);

	return create_style_toolbar(cd);
}
